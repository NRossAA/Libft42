#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "test.h"

/*
** Sujet 42 : écrire sur le fd donné. On écrit dans un pipe et on relit
** exactement ce qui a été produit (octet par octet, longueur comprise).
*/

static int		g_p[2];
static char		g_out[512];
static size_t	g_len;

static int	cap_fd(void)
{
	if (pipe(g_p) < 0)
		abort();
	return (g_p[1]);
}

static void	cap_end(void)
{
	ssize_t	n;

	close(g_p[1]);
	g_len = 0;
	while ((n = read(g_p[0], g_out + g_len, sizeof(g_out) - 1 - g_len)) > 0)
		g_len += (size_t)n;
	g_out[g_len] = '\0';
	close(g_p[0]);
}

#define CHECK_OUT(exp, explen, what) \
	CHECKF(g_len == (explen) && memcmp(g_out, (exp), (explen)) == 0, \
		"%s : obtenu %zu octet(s) \"%s\"", (what), g_len, g_out)

static void	t_putchar(void)
{
	ft_putchar_fd('a', cap_fd());
	cap_end();
	CHECK_OUT("a", 1, "putchar 'a'");
}

static void	t_putchar_high(void)
{
	ft_putchar_fd((char)0xff, cap_fd());
	cap_end();
	CHECK_OUT("\xff", 1, "putchar 0xff");
}

static void	t_putchar_nul(void)
{
	ft_putchar_fd('\0', cap_fd());
	cap_end();
	CHECK_OUT("", 1, "putchar '\\0'");
}

static void	t_putstr(void)
{
	ft_putstr_fd("hello", cap_fd());
	cap_end();
	CHECK_OUT("hello", 5, "putstr \"hello\"");
}

static void	t_putstr_empty(void)
{
	ft_putstr_fd("", cap_fd());
	cap_end();
	CHECK_OUT("", 0, "putstr \"\"");
}

static void	t_putendl(void)
{
	ft_putendl_fd("hello", cap_fd());
	cap_end();
	CHECK_OUT("hello\n", 6, "putendl \"hello\"");
}

static void	t_putendl_empty(void)
{
	ft_putendl_fd("", cap_fd());
	cap_end();
	CHECK_OUT("\n", 1, "putendl \"\"");
}

static void	nbr(int n)
{
	char	exp[16];
	char	what[32];

	snprintf(exp, sizeof(exp), "%d", n);
	snprintf(what, sizeof(what), "putnbr %d", n);
	ft_putnbr_fd(n, cap_fd());
	cap_end();
	CHECK_OUT(exp, strlen(exp), what);
}

static void	t_putnbr_small(void)
{
	nbr(0);
	nbr(7);
	nbr(-7);
	nbr(42);
	nbr(-42);
}

static void	t_putnbr_big(void)
{
	nbr(1000000);
	nbr(-1000000);
}

static void	t_putnbr_limits(void)
{
	nbr(INT_MAX);
	nbr(INT_MIN);
	nbr(INT_MIN + 1);
}

/* write(2) sur un fd invalide échoue (EBADF) : pas de crash attendu. */
static void	t_bad_fd(void)
{
	ft_putchar_fd('a', -1);
	ft_putstr_fd("abc", -1);
	ft_putendl_fd("abc", -1);
	ft_putnbr_fd(42, -1);
}

void	suite_fd(void)
{
	run_case("ft_putchar_fd", "'a'", t_putchar);
	run_case("ft_putchar_fd", "octet 0xff", t_putchar_high);
	run_case("ft_putchar_fd", "'\\0' écrit 1 octet", t_putchar_nul);
	run_case("ft_putstr_fd", "\"hello\"", t_putstr);
	run_case("ft_putstr_fd", "chaîne vide", t_putstr_empty);
	run_case("ft_putendl_fd", "\"hello\" + \\n", t_putendl);
	run_case("ft_putendl_fd", "chaîne vide -> \\n", t_putendl_empty);
	run_case("ft_putnbr_fd", "0, petits +/-", t_putnbr_small);
	run_case("ft_putnbr_fd", "grands nombres", t_putnbr_big);
	run_case("ft_putnbr_fd", "INT_MAX, INT_MIN", t_putnbr_limits);
	run_case("ft_put*_fd", "fd invalide (-1) : pas de crash", t_bad_fd);
}
