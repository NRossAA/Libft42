#include <string.h>
#include <stdlib.h>
#include "test.h"

/*
** Référence : strlen(3), strchr(3), strrchr(3), strncmp(3), strdup(3),
** strlcpy(3)/strlcat(3) (glibc >= 2.38, sémantique BSD),
** strnstr(3) de FreeBSD (absent de la glibc : valeurs attendues écrites
** à la main d'après la page man).
*/

/* ---------------- strlen ---------------- */

static void	t_strlen_empty(void)
{
	CHECK(ft_strlen("") == 0);
}

static void	t_strlen_basic(void)
{
	CHECK(ft_strlen("a") == 1);
	CHECK(ft_strlen("hello") == strlen("hello"));
}

static void	t_strlen_high(void)
{
	CHECK(ft_strlen("\xff\x80\x01 ok") == strlen("\xff\x80\x01 ok"));
}

static void	t_strlen_long(void)
{
	char	*s;

	s = malloc(10001);
	if (!s)
		return ;
	memset(s, 'a', 10000);
	s[10000] = '\0';
	CHECKF(ft_strlen(s) == 10000, "ft=%zu", ft_strlen(s));
	free(s);
}

/* ---------------- strchr / strrchr ---------------- */

static const char	g_s[] = "hello\xe9world";

static void	sc(const char *s, int c)
{
	CHECKF(ft_strchr(s, c) == strchr(s, c), "s=\"%s\" c=%d", s, c);
}

static void	src(const char *s, int c)
{
	CHECKF(ft_strrchr(s, c) == strrchr(s, c), "s=\"%s\" c=%d", s, c);
}

static void	t_strchr_found(void)
{
	sc(g_s, 'h');
	sc(g_s, 'l');
	sc(g_s, 'd');
}

static void	t_strchr_absent(void)
{
	sc(g_s, 'z');
}

static void	t_strchr_nul(void)
{
	sc(g_s, '\0');
	sc("", '\0');
}

static void	t_strchr_high(void)
{
	sc(g_s, 0xe9);
	sc(g_s, (char)0xe9);
	sc(g_s, 'w' + 256);
}

static void	t_strchr_empty(void)
{
	sc("", 'a');
}

static void	t_strrchr_found(void)
{
	src(g_s, 'l');
	src(g_s, 'o');
	src(g_s, 'h');
	src(g_s, 'd');
}

static void	t_strrchr_absent(void)
{
	src(g_s, 'z');
}

static void	t_strrchr_nul(void)
{
	src(g_s, '\0');
	src("", '\0');
}

static void	t_strrchr_high(void)
{
	src(g_s, 0xe9);
	src(g_s, 'w' + 256);
}

static void	t_strrchr_empty(void)
{
	src("", 'a');
}

/* ---------------- strncmp ---------------- */

static void	sncmp(const char *a, const char *b, size_t n)
{
	int	ft;
	int	ref;

	ft = ft_strncmp(a, b, n);
	ref = strncmp(a, b, n);
	CHECKF(sgn(ft) == sgn(ref), "\"%s\" vs \"%s\" n=%zu : ft=%d libc=%d",
		a, b, n, ft, ref);
}

static void	t_strncmp_basic(void)
{
	sncmp("abc", "abd", 2);
	sncmp("abc", "abd", 3);
	sncmp("abd", "abc", 3);
	sncmp("abc", "abc", 3);
}

static void	t_strncmp_zero(void)
{
	sncmp("a", "b", 0);
}

static void	t_strncmp_big_n(void)
{
	sncmp("abc", "abc", 100);
	sncmp("abc", "abcd", 100);
	sncmp("abc", "ab", 3);
}

static void	t_strncmp_empty(void)
{
	sncmp("", "", 5);
	sncmp("", "a", 1);
	sncmp("a", "", 1);
}

static void	t_strncmp_high(void)
{
	sncmp("\x80", "\x01", 1);
	sncmp("a\xff", "a\x01", 2);
}

static void	t_strncmp_after_nul(void)
{
	sncmp("test\0a", "test\0b", 6);
}

/* ---------------- strnstr ---------------- */

static const char	g_h[] = "Foo Bar Baz";

static void	sn(const char *h, const char *nd, size_t len, const char *exp)
{
	char	*r;

	r = ft_strnstr(h, nd, len);
	CHECKF(r == exp, "h=\"%s\" needle=\"%s\" len=%zu : obtenu %s (offset %ld)",
		h, nd, len, r ? "non-NULL" : "NULL", r ? (long)(r - h) : -1L);
}

static void	t_strnstr_found(void)
{
	sn(g_h, "Bar", 11, g_h + 4);
	sn(g_h, "Foo", 11, g_h);
	sn(g_h, "Baz", 11, g_h + 8);
}

static void	t_strnstr_limit(void)
{
	sn(g_h, "Bar", 7, g_h + 4);
	sn(g_h, "Bar", 6, NULL);
	sn(g_h, "Foo", 0, NULL);
	sn(g_h, "Foo Bar", 3, NULL);
}

static void	t_strnstr_empty_needle(void)
{
	static const char	e[] = "";

	sn(g_h, "", 0, g_h);
	sn(g_h, "", 5, g_h);
	sn(e, "", 3, e);
}

static void	t_strnstr_partial(void)
{
	static const char	p[] = "aaab";
	static const char	q[] = "abababc";

	sn(p, "aab", 4, p + 1);
	sn(q, "ababc", 7, q + 2);
}

static void	t_strnstr_long_needle(void)
{
	static const char	p[] = "abc";

	sn(p, "abcd", 10, NULL);
}

static void	t_strnstr_len_beyond(void)
{
	static const char	p[] = "abc";

	sn(p, "c", 100, p + 2);
	sn(p, "z", 100, NULL);
}

static void	t_strnstr_after_nul(void)
{
	static const char	q[] = "ab\0cd";

	sn(q, "cd", 5, NULL);
}

/* ---------------- strlcpy ---------------- */

static void	lcpy(const char *src, size_t size)
{
	char	a[32];
	char	b[32];
	size_t	ra;
	size_t	rb;

	memset(a, 'X', sizeof(a));
	memset(b, 'X', sizeof(b));
	ra = ft_strlcpy(a, src, size);
	rb = strlcpy(b, src, size);
	CHECKF(ra == rb, "src=\"%s\" size=%zu : retour ft=%zu libc=%zu",
		src, size, ra, rb);
	CHECKF(memcmp(a, b, sizeof(a)) == 0,
		"src=\"%s\" size=%zu : buffer ft=\"%.31s\" libc=\"%.31s\"",
		src, size, a, b);
}

static void	t_strlcpy_size0(void)
{
	lcpy("hello", 0);
}

static void	t_strlcpy_size1(void)
{
	lcpy("hello", 1);
}

static void	t_strlcpy_exact(void)
{
	lcpy("hello", 6);
}

static void	t_strlcpy_small(void)
{
	lcpy("hello world", 5);
	lcpy("hello", 5);
}

static void	t_strlcpy_big(void)
{
	lcpy("hello", 32);
}

static void	t_strlcpy_empty(void)
{
	lcpy("", 10);
}

/* ---------------- strlcat ---------------- */

static void	lcat(const char *dst, const char *src, size_t size)
{
	char	a[32];
	char	b[32];
	size_t	ra;
	size_t	rb;

	memset(a, 'X', sizeof(a));
	memset(b, 'X', sizeof(b));
	memcpy(a, dst, strlen(dst) + 1);
	memcpy(b, dst, strlen(dst) + 1);
	ra = ft_strlcat(a, src, size);
	rb = strlcat(b, src, size);
	CHECKF(ra == rb, "dst=\"%s\" src=\"%s\" size=%zu : retour ft=%zu libc=%zu",
		dst, src, size, ra, rb);
	CHECKF(memcmp(a, b, sizeof(a)) == 0,
		"dst=\"%s\" src=\"%s\" size=%zu : buffer ft=\"%.31s\" libc=\"%.31s\"",
		dst, src, size, a, b);
}

static void	t_strlcat_size0(void)
{
	lcat("abc", "de", 0);
}

static void	t_strlcat_size_lt_dst(void)
{
	lcat("abc", "de", 2);
	lcat("abcdef", "xy", 3);
}

static void	t_strlcat_size_eq_dst(void)
{
	lcat("abc", "de", 3);
}

static void	t_strlcat_no_room(void)
{
	lcat("abc", "de", 4);
}

static void	t_strlcat_exact(void)
{
	lcat("abc", "de", 6);
}

static void	t_strlcat_trunc(void)
{
	lcat("abc", "defgh", 6);
}

static void	t_strlcat_big(void)
{
	lcat("abc", "de", 32);
}

static void	t_strlcat_empty(void)
{
	lcat("", "hello", 3);
	lcat("abc", "", 10);
	lcat("", "", 1);
}

/* ---------------- strdup ---------------- */

static void	sdup(const char *s)
{
	char	*r;

	r = ft_strdup(s);
	CHECK_STR(r, s, "ft_strdup");
	CHECK(r != s);
	free(r);
}

static void	t_strdup_basic(void)
{
	sdup("hello");
}

static void	t_strdup_empty(void)
{
	sdup("");
}

static void	t_strdup_high(void)
{
	sdup("\xff\x80 abc");
}

static void	t_strdup_long(void)
{
	char	*s;

	s = malloc(5001);
	if (!s)
		return ;
	memset(s, 'z', 5000);
	s[5000] = '\0';
	sdup(s);
	free(s);
}

void	suite_str(void)
{
	run_case("ft_strlen", "chaîne vide", t_strlen_empty);
	run_case("ft_strlen", "cas simples", t_strlen_basic);
	run_case("ft_strlen", "caractères > 127", t_strlen_high);
	run_case("ft_strlen", "10000 caractères", t_strlen_long);
	run_case("ft_strchr", "trouvé (premier, milieu, dernier)", t_strchr_found);
	run_case("ft_strchr", "absent", t_strchr_absent);
	run_case("ft_strchr", "c = \\0 -> terminateur", t_strchr_nul);
	run_case("ft_strchr", "c > 127, c = 256 + x", t_strchr_high);
	run_case("ft_strchr", "chaîne vide", t_strchr_empty);
	run_case("ft_strrchr", "dernière occurrence", t_strrchr_found);
	run_case("ft_strrchr", "absent", t_strrchr_absent);
	run_case("ft_strrchr", "c = \\0 -> terminateur", t_strrchr_nul);
	run_case("ft_strrchr", "c > 127, c = 256 + x", t_strrchr_high);
	run_case("ft_strrchr", "chaîne vide", t_strrchr_empty);
	run_case("ft_strncmp", "cas simples", t_strncmp_basic);
	run_case("ft_strncmp", "n = 0", t_strncmp_zero);
	run_case("ft_strncmp", "n > longueur, préfixe", t_strncmp_big_n);
	run_case("ft_strncmp", "chaînes vides", t_strncmp_empty);
	run_case("ft_strncmp", "signe avec caractères > 127", t_strncmp_high);
	run_case("ft_strncmp", "arrêt au \\0", t_strncmp_after_nul);
	run_case("ft_strnstr", "trouvé", t_strnstr_found);
	run_case("ft_strnstr", "limite len", t_strnstr_limit);
	run_case("ft_strnstr", "needle vide", t_strnstr_empty_needle);
	run_case("ft_strnstr", "correspondance partielle puis vraie", t_strnstr_partial);
	run_case("ft_strnstr", "needle plus long que haystack", t_strnstr_long_needle);
	run_case("ft_strnstr", "len > longueur de haystack", t_strnstr_len_beyond);
	run_case("ft_strnstr", "pas de recherche après \\0", t_strnstr_after_nul);
	run_case("ft_strlcpy", "dstsize = 0", t_strlcpy_size0);
	run_case("ft_strlcpy", "dstsize = 1", t_strlcpy_size1);
	run_case("ft_strlcpy", "dstsize = strlen + 1", t_strlcpy_exact);
	run_case("ft_strlcpy", "troncature", t_strlcpy_small);
	run_case("ft_strlcpy", "buffer large", t_strlcpy_big);
	run_case("ft_strlcpy", "src vide", t_strlcpy_empty);
	run_case("ft_strlcat", "dstsize = 0", t_strlcat_size0);
	run_case("ft_strlcat", "dstsize < strlen(dst)", t_strlcat_size_lt_dst);
	run_case("ft_strlcat", "dstsize = strlen(dst)", t_strlcat_size_eq_dst);
	run_case("ft_strlcat", "dstsize = strlen(dst) + 1", t_strlcat_no_room);
	run_case("ft_strlcat", "place exacte", t_strlcat_exact);
	run_case("ft_strlcat", "troncature", t_strlcat_trunc);
	run_case("ft_strlcat", "buffer large", t_strlcat_big);
	run_case("ft_strlcat", "dst ou src vide", t_strlcat_empty);
	run_case("ft_strdup", "cas simple, nouveau pointeur", t_strdup_basic);
	run_case("ft_strdup", "chaîne vide", t_strdup_empty);
	run_case("ft_strdup", "caractères > 127", t_strdup_high);
	run_case("ft_strdup", "5000 caractères", t_strdup_long);
}
