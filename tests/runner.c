#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "test.h"

#define EXIT_CHECK	42
#define TIMEOUT_S	5
#define MAX_MSGS	10
#define MAX_FAILS	512
#define LINE_W		44

#define C_RESET		"\033[0m"
#define C_BOLD		"\033[1m"
#define C_DIM		"\033[2m"
#define C_RED		"\033[31m"
#define C_GREEN		"\033[32m"
#define C_YELLOW	"\033[33m"
#define C_CYAN		"\033[36m"

/*
** Sans cette option, ASan tue le processus quand malloc reçoit une taille
** impossible, au lieu de renvoyer NULL comme la libc : on ne pourrait pas
** tester le comportement de calloc en cas d'overflow.
*/
const char	*__asan_default_options(void)
{
	return ("allocator_may_return_null=1");
}

typedef struct s_fail
{
	char	group[48];
	char	name[96];
	char	verdict[32];
}	t_fail;

typedef struct s_ub
{
	int		crashed;
	char	text[160];
	char	where[96];
}	t_ub;

/* État de l'enfant */
static int			g_fail_checks;
static int			g_printed;

/* État global du parent */
static int			g_color;
static int			g_verbose;
static int			g_cases;
static int			g_passed;
static int			g_ub_cases;
static int			g_ub_diverge;
static int			g_nfails;
static t_fail		g_fails[MAX_FAILS];

/* Groupe en cours : les détails sont bufferisés jusqu'à la ligne de bilan */
static const char	*g_group;
static int			g_gtotal;
static int			g_gpass;
static FILE			*g_gdet;
static char			*g_gbuf;
static size_t		g_glen;

static const char	*col(const char *code)
{
	return (g_color ? code : "");
}

int	sgn(int x)
{
	return ((x > 0) - (x < 0));
}

/* Largeur affichée d'une chaîne UTF-8 (pour aligner malgré les accents). */
static size_t	utf8_width(const char *s)
{
	size_t	n;

	n = 0;
	while (*s)
		if (((unsigned char)*s++ & 0xC0) != 0x80)
			n++;
	return (n);
}

/* ------------------------------------------------------------------ */
/* Assertions (exécutées dans l'enfant, messages écrits sur stderr)    */
/* ------------------------------------------------------------------ */

static void	put_escaped(FILE *f, const char *s)
{
	unsigned char	c;

	fputc('"', f);
	while (*s)
	{
		c = (unsigned char)*s++;
		if (c < 32 || c >= 127 || c == '"' || c == '\\')
			fprintf(f, "\\x%02x", c);
		else
			fputc(c, f);
	}
	fputc('"', f);
}

static int	check_head(int ok, const char *file, int line)
{
	if (ok)
		return (0);
	g_fail_checks++;
	if (g_printed++ >= MAX_MSGS)
		return (0);
	fprintf(stderr, "CHECK %s:%d: ", file, line);
	return (1);
}

void	checkf_impl(int ok, const char *expr, const char *file, int line,
			const char *fmt, ...)
{
	va_list	ap;

	if (!check_head(ok, file, line))
		return ;
	fputs(expr, stderr);
	if (fmt)
	{
		fputs("  [", stderr);
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
		fputc(']', stderr);
	}
	fputc('\n', stderr);
}

void	check_str_impl(const char *got, const char *exp, const char *file,
			int line, const char *fmt, ...)
{
	va_list	ap;

	if (!check_head(got != NULL && strcmp(got, exp) == 0, file, line))
		return ;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputs(" : attendu ", stderr);
	put_escaped(stderr, exp);
	fputs(", obtenu ", stderr);
	if (got)
		put_escaped(stderr, got);
	else
		fputs("NULL", stderr);
	fputc('\n', stderr);
}

void	ub_note(const char *fmt, ...)
{
	va_list	ap;

	fputs("NOTE ", stderr);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
}

/* ------------------------------------------------------------------ */
/* Exécution isolée                                                    */
/* ------------------------------------------------------------------ */

static char	*slurp(int fd)
{
	size_t	cap;
	size_t	len;
	ssize_t	n;
	char	*buf;

	cap = 4096;
	len = 0;
	buf = malloc(cap);
	if (!buf)
		exit(2);
	while ((n = read(fd, buf + len, cap - len - 1)) > 0)
	{
		len += (size_t)n;
		if (cap - len - 1 == 0)
		{
			cap *= 2;
			buf = realloc(buf, cap);
			if (!buf)
				exit(2);
		}
	}
	buf[len] = '\0';
	return (buf);
}

/*
** Lance fn dans un enfant dont stdout/stderr sont capturés.
** ub = 1 : sortie par _exit (pas de détection de fuites, sans intérêt ici).
*/
static char	*spawn(void (*fn)(void), int ub, int *status)
{
	int		fds[2];
	pid_t	pid;
	char	*out;

	fflush(stdout);
	fflush(stderr);
	if (pipe(fds) < 0)
	{
		perror("pipe");
		exit(2);
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(2);
	}
	if (pid == 0)
	{
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		dup2(fds[1], STDERR_FILENO);
		close(fds[1]);
		g_fail_checks = 0;
		g_printed = 0;
		alarm(TIMEOUT_S);
		fn();
		fflush(NULL);
		if (ub)
			_exit(0);
		exit(g_fail_checks ? EXIT_CHECK : 0);
	}
	close(fds[1]);
	out = slurp(fds[0]);
	close(fds[0]);
	waitpid(pid, status, 0);
	return (out);
}

/* ------------------------------------------------------------------ */
/* Analyse de la sortie de l'enfant                                    */
/* ------------------------------------------------------------------ */

static const char	*line_with(const char *out, const char *needle)
{
	const char	*p;

	p = strstr(out, needle);
	if (!p)
		return (NULL);
	while (p > out && p[-1] != '\n')
		p--;
	return (p);
}

static int	line_len(const char *l)
{
	const char	*end;

	end = strchr(l, '\n');
	return (end ? (int)(end - l) : (int)strlen(l));
}

/*
** Première frame de la pile dans une fonction ft_, mise en forme
** "ft_strtrim.c:43" (le préfixe ../ et l'adresse sont retirés).
*/
static void	ft_frame(const char *out, char *buf, size_t size)
{
	const char	*l;
	const char	*file;
	int			n;

	buf[0] = '\0';
	l = line_with(out, " in ft_");
	if (!l)
		return ;
	l = strstr(l, " in ft_") + 4;
	n = line_len(l);
	file = memchr(l, ' ', (size_t)n);
	if (!file)
		return ;
	file++;
	if (strncmp(file, "../", 3) == 0)
		file += 3;
	snprintf(buf, size, "%.*s", n - (int)(file - l), file);
}

/* Type d'erreur ASan, ex. "global-buffer-overflow" ou "SEGV sur NULL". */
static int	asan_kind(const char *out, char *buf, size_t size)
{
	const char	*l;
	const char	*end;
	const char	*nl;
	const char	*null;

	l = strstr(out, "Sanitizer: ");
	if (!l)
		return (0);
	l += 11;
	end = l;
	while (*end && *end != ' ' && *end != '\n')
		end++;
	nl = strchr(l, '\n');
	null = strstr(l, "address 0x000000000000");
	snprintf(buf, size, "%.*s%s", (int)(end - l), l,
		null && (!nl || null < nl) ? " sur NULL" : "");
	return (1);
}

static void	verdict_add(char *v, size_t size, const char *what)
{
	if (v[0])
		strncat(v, "+", size - strlen(v) - 1);
	strncat(v, what, size - strlen(v) - 1);
}

/* ------------------------------------------------------------------ */
/* Affichage                                                           */
/* ------------------------------------------------------------------ */

static void	section(const char *title)
{
	size_t	w;

	printf("\n%s%s━━ %s ", col(C_BOLD), col(C_CYAN), title);
	w = utf8_width(title) + 4;
	while (w++ < LINE_W + 12)
		fputs("━", stdout);
	printf("%s\n\n", col(C_RESET));
}

static void	print_details(FILE *f, const char *out, const char *log)
{
	const char	*p;
	const char	*l;
	char		buf[160];

	p = out;
	while ((p = strstr(p, "CHECK ")) != NULL)
	{
		if (p == out || p[-1] == '\n')
			fprintf(f, "          %s%.*s%s\n", col(C_YELLOW),
				line_len(p + 6), p + 6, col(C_RESET));
		p += 6;
	}
	if (asan_kind(out, buf, sizeof(buf)))
	{
		l = line_with(out, "ERROR: LeakSanitizer") ? "fuite mémoire"
			: "erreur mémoire";
		fprintf(f, "          %s%s : %s%s", col(C_RED), l, buf, col(C_RESET));
		ft_frame(out, buf, sizeof(buf));
		if (buf[0])
			fprintf(f, "  %s→ %s%s", col(C_CYAN), buf, col(C_RESET));
		fputc('\n', f);
	}
	if (log[0])
		fprintf(f, "          %slog : tests/%s%s\n", col(C_DIM), log,
			col(C_RESET));
}

static void	flush_group(void)
{
	size_t	used;
	int		ok;

	if (!g_group)
		return ;
	fclose(g_gdet);
	ok = (g_gpass == g_gtotal);
	printf("  %s%s%s %s", col(ok ? C_GREEN : C_RED), ok ? "✓" : "✗",
		col(C_RESET), g_group);
	used = strlen(g_group);
	printf(" %s", col(C_DIM));
	while (used++ < LINE_W - 8)
		fputc('.', stdout);
	printf("%s %s%2d/%-2d%s\n", col(C_RESET), col(ok ? C_GREEN : C_RED),
		g_gpass, g_gtotal, col(C_RESET));
	fwrite(g_gbuf, 1, g_glen, stdout);
	free(g_gbuf);
	g_group = NULL;
}

static void	start_group(const char *group)
{
	flush_group();
	g_group = group;
	g_gtotal = 0;
	g_gpass = 0;
	g_gbuf = NULL;
	g_glen = 0;
	g_gdet = open_memstream(&g_gbuf, &g_glen);
	if (!g_gdet)
		exit(2);
}

static void	save_log(const char *out, char *path, size_t size)
{
	FILE	*f;

	snprintf(path, size, "build/logs/case_%03d.log", g_cases);
	f = fopen(path, "w");
	if (!f)
	{
		path[0] = '\0';
		return ;
	}
	fputs(out, f);
	fclose(f);
}

static void	record_fail(const char *group, const char *name, const char *v)
{
	if (g_nfails >= MAX_FAILS)
		return ;
	snprintf(g_fails[g_nfails].group, sizeof(g_fails[0].group), "%s", group);
	snprintf(g_fails[g_nfails].name, sizeof(g_fails[0].name), "%s", name);
	snprintf(g_fails[g_nfails].verdict, sizeof(g_fails[0].verdict), "%s", v);
	g_nfails++;
}

void	run_case(const char *group, const char *name, void (*fn)(void))
{
	int		status;
	char	*out;
	char	v[32];
	char	path[64];

	if (!g_group || strcmp(g_group, group) != 0)
		start_group(group);
	out = spawn(fn, 0, &status);
	g_cases++;
	g_gtotal++;
	v[0] = '\0';
	if (WIFSIGNALED(status))
		verdict_add(v, sizeof(v),
			WTERMSIG(status) == SIGALRM ? "TIMEOUT" : "SIGNAL");
	else
	{
		if (strstr(out, "ERROR: AddressSanitizer"))
			verdict_add(v, sizeof(v), "ASAN");
		if (strstr(out, "ERROR: LeakSanitizer"))
			verdict_add(v, sizeof(v), "FUITE");
		if (WEXITSTATUS(status) == EXIT_CHECK || strstr(out, "CHECK "))
			verdict_add(v, sizeof(v), "ASSERT");
		if (!v[0] && WEXITSTATUS(status) != 0)
			snprintf(v, sizeof(v), "EXIT %d", WEXITSTATUS(status));
	}
	if (!v[0])
	{
		g_passed++;
		g_gpass++;
		if (g_verbose)
			fprintf(g_gdet, "      %s✓%s %s%s%s\n", col(C_GREEN),
				col(C_RESET), col(C_DIM), name, col(C_RESET));
	}
	else
	{
		fprintf(g_gdet, "      %s✗ %s%s  %s[%s]%s\n", col(C_RED), name,
			col(C_RESET), col(C_BOLD), v, col(C_RESET));
		save_log(out, path, sizeof(path));
		print_details(g_gdet, out, path);
		record_fail(group, name, v);
	}
	free(out);
}

/* ------------------------------------------------------------------ */
/* Cas UB : on décrit ce qui se passe, sans juger                      */
/* ------------------------------------------------------------------ */

static void	describe(void (*fn)(void), t_ub *r)
{
	int			status;
	char		*out;
	const char	*l;

	r->crashed = 1;
	r->where[0] = '\0';
	out = spawn(fn, 1, &status);
	if (WIFSIGNALED(status))
		snprintf(r->text, sizeof(r->text), "%s",
			WTERMSIG(status) == SIGALRM ? "timeout"
			: strsignal(WTERMSIG(status)));
	else if (asan_kind(out, r->text, sizeof(r->text)))
		ft_frame(out, r->where, sizeof(r->where));
	else if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		r->crashed = 0;
		l = line_with(out, "NOTE ");
		if (l)
			snprintf(r->text, sizeof(r->text), "%.*s",
				line_len(l + 5), l + 5);
		else
			snprintf(r->text, sizeof(r->text), "retour normal");
	}
	else
		snprintf(r->text, sizeof(r->text), "exit %d", WEXITSTATUS(status));
	free(out);
}

static void	print_ub_side(const char *label, const t_ub *r)
{
	printf("      %s%-4s%s  ", col(C_DIM), label, col(C_RESET));
	if (r->crashed)
		printf("%scrash%s  %s", col(C_YELLOW), col(C_RESET), r->text);
	else
		printf("%sok%s     %s", col(C_GREEN), col(C_RESET), r->text);
	if (r->where[0])
		printf("  %s→ %s%s", col(C_CYAN), r->where, col(C_RESET));
	fputc('\n', stdout);
}

void	run_ub(const char *name, void (*ft)(void), void (*libc)(void))
{
	t_ub	a;
	t_ub	b;
	int		diverge;

	describe(ft, &a);
	diverge = 0;
	if (libc)
	{
		describe(libc, &b);
		diverge = (a.crashed != b.crashed
				|| (!a.crashed && strcmp(a.text, b.text) != 0));
	}
	g_ub_cases++;
	g_ub_diverge += diverge;
	printf("  %s%s%s", col(C_BOLD), name, col(C_RESET));
	if (diverge)
		printf("  %s≠ diffère de la libc%s", col(C_YELLOW), col(C_RESET));
	fputc('\n', stdout);
	print_ub_side("ft", &a);
	if (libc)
		print_ub_side("libc", &b);
	else
		printf("      %slibc  — pas d'équivalent%s\n", col(C_DIM),
			col(C_RESET));
}

/* ------------------------------------------------------------------ */

static void	summary(void)
{
	int	i;
	int	failed;

	failed = g_cases - g_passed;
	section("Résumé");
	printf("  Cas valides   %s%d / %d passés%s", col(failed ? C_RED : C_GREEN),
		g_passed, g_cases, col(C_RESET));
	if (failed)
		printf("   %s(%d échoués)%s", col(C_RED), failed, col(C_RESET));
	printf("\n  Cas UB        %d observés", g_ub_cases);
	if (g_ub_diverge)
		printf("   %s(%d divergences ft / libc)%s", col(C_YELLOW),
			g_ub_diverge, col(C_RESET));
	printf("\n");
	if (g_nfails)
		printf("\n  Échecs :\n");
	for (i = 0; i < g_nfails; i++)
		printf("    %s✗%s %s%s%s — %s  %s[%s]%s\n", col(C_RED), col(C_RESET),
			col(C_BOLD), g_fails[i].group, col(C_RESET), g_fails[i].name,
			col(C_DIM), g_fails[i].verdict, col(C_RESET));
	if (failed)
		printf("\n  %s%s✗ %d TEST%s ÉCHOUÉ%s%s\n", col(C_BOLD), col(C_RED),
			failed, failed > 1 ? "S" : "", failed > 1 ? "S" : "",
			col(C_RESET));
	else
		printf("\n  %s%s✓ TOUS LES TESTS PASSENT%s\n", col(C_BOLD),
			col(C_GREEN), col(C_RESET));
	if (!g_verbose)
		printf("  %sDétail de chaque cas : make -C tests run V=1%s\n",
			col(C_DIM), col(C_RESET));
}

int	main(int argc, char **argv)
{
	int	i;

	for (i = 1; i < argc; i++)
		if (strcmp(argv[i], "-v") == 0)
			g_verbose = 1;
	g_color = (isatty(STDOUT_FILENO) && !getenv("NO_COLOR"))
		|| getenv("FORCE_COLOR");
	section("Arguments valides : comparaison libc / sujet");
	suite_ctype();
	suite_mem();
	suite_str();
	suite_conv();
	suite_alloc();
	suite_fd();
	suite_lst();
	flush_group();
	section("Comportement indéfini : observé, non compté");
	suite_ub();
	summary();
	return (g_passed != g_cases);
}
