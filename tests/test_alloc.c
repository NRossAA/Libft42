#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include "test.h"

/*
** Fonctions sans équivalent libc : comportement attendu d'après le sujet 42.
** Hypothèses :
**  - substr : start >= strlen(s) -> "" alloué ; len tronqué à ce qui reste.
**  - strtrim : retire les caractères de set au début et à la fin uniquement.
**  - split : pas de mots vides ; tableau terminé par NULL ; c = '\0' -> un
**    seul mot (s entière) si s n'est pas vide.
**  - strmapi / striteri : f appelée une fois par caractère, index 0..n-1.
*/

/* ---------------- substr ---------------- */

static const char	g_hw[] = "Hello World";

static void	ss(const char *s, unsigned int start, size_t len, const char *exp)
{
	char	*r;

	r = ft_substr(s, start, len);
	CHECK_STR(r, exp, "ft_substr(\"%s\", %u, %zu)", s, start, len);
	CHECK(r != s);
	free(r);
}

static void	t_substr_basic(void)
{
	ss(g_hw, 0, 5, "Hello");
	ss(g_hw, 6, 5, "World");
	ss(g_hw, 3, 1, "l");
}

static void	t_substr_len_trunc(void)
{
	ss(g_hw, 6, 100, "World");
	ss(g_hw, 6, SIZE_MAX, "World");
}

static void	t_substr_start_end(void)
{
	ss(g_hw, 11, 5, "");
}

static void	t_substr_start_beyond(void)
{
	ss(g_hw, 20, 5, "");
	ss(g_hw, UINT_MAX, 5, "");
}

static void	t_substr_len0(void)
{
	ss(g_hw, 0, 0, "");
}

static void	t_substr_empty(void)
{
	ss("", 0, 5, "");
}

/* ---------------- strjoin ---------------- */

static void	sj(const char *a, const char *b, const char *exp)
{
	char	*r;

	r = ft_strjoin(a, b);
	CHECK_STR(r, exp, "ft_strjoin(\"%s\", \"%s\")", a, b);
	CHECK(r != a && r != b);
	free(r);
}

static void	t_strjoin_basic(void)
{
	sj("ab", "cd", "abcd");
	sj("Hello ", "World", "Hello World");
}

static void	t_strjoin_empty(void)
{
	sj("", "", "");
	sj("abc", "", "abc");
	sj("", "abc", "abc");
}

/* ---------------- strtrim ---------------- */

static void	st(const char *s, const char *set, const char *exp)
{
	char	*r;

	r = ft_strtrim(s, set);
	CHECK_STR(r, exp, "ft_strtrim(\"%s\", \"%s\")", s, set);
	CHECK(r != s);
	free(r);
}

static void	t_strtrim_basic(void)
{
	st("  hello  ", " ", "hello");
	st("xxhixx", "x", "hi");
	st("abcHELLOcba", "abc", "HELLO");
}

static void	t_strtrim_inner(void)
{
	st("  a b  ", " ", "a b");
}

static void	t_strtrim_one_side(void)
{
	st("ab", "a", "b");
	st("ba", "a", "b");
}

static void	t_strtrim_nothing(void)
{
	st("hello", "xyz", "hello");
}

static void	t_strtrim_all(void)
{
	st("aaaa", "a", "");
}

static void	t_strtrim_single(void)
{
	st("a", "a", "");
}

static void	t_strtrim_empty_set(void)
{
	st("hello", "", "hello");
}

static void	t_strtrim_empty_s(void)
{
	st("", "abc", "");
}

/* ---------------- split ---------------- */

static void	sp(const char *s, char c, const char *const *exp)
{
	char	**r;
	size_t	i;

	r = ft_split(s, c);
	CHECKF(r != NULL, "ft_split(\"%s\", %d) a renvoyé NULL", s, c);
	if (!r)
		return ;
	i = 0;
	while (exp[i] && r[i])
	{
		CHECK_STR(r[i], exp[i], "ft_split(\"%s\", %d)[%zu]", s, c, i);
		i++;
	}
	CHECKF(exp[i] == NULL && r[i] == NULL,
		"ft_split(\"%s\", %d) : nombre de mots incorrect (diverge à l'index %zu)",
		s, c, i);
	for (i = 0; r[i]; i++)
		free(r[i]);
	free(r);
}

static void	t_split_basic(void)
{
	const char	*e[] = {"hello", "world", "foo", NULL};

	sp("hello world foo", ' ', e);
}

static void	t_split_multi(void)
{
	const char	*e[] = {"hello", "world", NULL};

	sp("  hello   world  ", ' ', e);
}

static void	t_split_commas(void)
{
	const char	*e[] = {"a", "b", "c", NULL};

	sp(",a,b,,c,", ',', e);
}

static void	t_split_empty(void)
{
	const char	*e[] = {NULL};

	sp("", ' ', e);
}

static void	t_split_only_sep(void)
{
	const char	*e[] = {NULL};

	sp("     ", ' ', e);
}

static void	t_split_no_sep(void)
{
	const char	*e[] = {"hello", NULL};

	sp("hello", ' ', e);
}

static void	t_split_nul_sep(void)
{
	const char	*e1[] = {"hello world", NULL};
	const char	*e2[] = {NULL};

	sp("hello world", '\0', e1);
	sp("", '\0', e2);
}

static void	t_split_high(void)
{
	const char	*e[] = {"\x80x", "y", NULL};

	sp("\xff\x80x\xffy\xff", (char)0xff, e);
}

static void	t_split_long(void)
{
	const char	*e[] = {"lorem", "ipsum", "dolor", "sit", "amet,",
		"consectetur", "adipiscing", "elit.", "Sed", "non", "risus.", NULL};

	sp("lorem ipsum dolor sit amet, consectetur adipiscing elit. "
		"Sed non risus.", ' ', e);
}

/* ---------------- strmapi / striteri ---------------- */

static unsigned int	g_idx[64];
static size_t		g_calls;

static char	map_upper(unsigned int i, char c)
{
	if (g_calls < 64)
		g_idx[g_calls] = i;
	g_calls++;
	return ((char)toupper((unsigned char)c));
}

static char	map_add_index(unsigned int i, char c)
{
	if (g_calls < 64)
		g_idx[g_calls] = i;
	g_calls++;
	return ((char)(c + (char)i));
}

static void	check_indices(size_t n)
{
	size_t	k;

	CHECKF(g_calls == n, "f appelée %zu fois au lieu de %zu", g_calls, n);
	for (k = 0; k < g_calls && k < n && k < 64; k++)
		CHECKF(g_idx[k] == k, "appel n°%zu : index %u", k, g_idx[k]);
}

static void	t_strmapi_basic(void)
{
	const char	s[] = "hello";
	char		*r;

	g_calls = 0;
	r = ft_strmapi(s, map_upper);
	CHECK_STR(r, "HELLO", "ft_strmapi(\"hello\", toupper)");
	CHECK(r != s);
	CHECK(strcmp(s, "hello") == 0);
	check_indices(5);
	free(r);
}

static void	t_strmapi_index(void)
{
	char	*r;

	g_calls = 0;
	r = ft_strmapi("aaaa", map_add_index);
	CHECK_STR(r, "abcd", "ft_strmapi(\"aaaa\", c + i)");
	check_indices(4);
	free(r);
}

static void	t_strmapi_empty(void)
{
	char	*r;

	g_calls = 0;
	r = ft_strmapi("", map_upper);
	CHECK_STR(r, "", "ft_strmapi(\"\", f)");
	check_indices(0);
	free(r);
}

static void	iter_add_index(unsigned int i, char *c)
{
	if (g_calls < 64)
		g_idx[g_calls] = i;
	g_calls++;
	*c = (char)(*c + (char)i);
}

static void	t_striteri_basic(void)
{
	char	s[] = "aaaa";

	g_calls = 0;
	ft_striteri(s, iter_add_index);
	CHECK_STR(s, "abcd", "ft_striteri(\"aaaa\", c += i)");
	check_indices(4);
}

static void	t_striteri_empty(void)
{
	char	s[] = "";

	g_calls = 0;
	ft_striteri(s, iter_add_index);
	check_indices(0);
}

void	suite_alloc(void)
{
	run_case("ft_substr", "cas simples", t_substr_basic);
	run_case("ft_substr", "len tronqué (dont SIZE_MAX)", t_substr_len_trunc);
	run_case("ft_substr", "start == strlen(s)", t_substr_start_end);
	run_case("ft_substr", "start > strlen(s) (dont UINT_MAX)", t_substr_start_beyond);
	run_case("ft_substr", "len = 0", t_substr_len0);
	run_case("ft_substr", "s vide", t_substr_empty);
	run_case("ft_strjoin", "cas simples", t_strjoin_basic);
	run_case("ft_strjoin", "chaînes vides", t_strjoin_empty);
	run_case("ft_strtrim", "cas simples", t_strtrim_basic);
	run_case("ft_strtrim", "caractères internes conservés", t_strtrim_inner);
	run_case("ft_strtrim", "un seul côté", t_strtrim_one_side);
	run_case("ft_strtrim", "rien à retirer", t_strtrim_nothing);
	run_case("ft_strtrim", "tout est retiré", t_strtrim_all);
	run_case("ft_strtrim", "un caractère, retiré", t_strtrim_single);
	run_case("ft_strtrim", "set vide", t_strtrim_empty_set);
	run_case("ft_strtrim", "s1 vide", t_strtrim_empty_s);
	run_case("ft_split", "cas simple", t_split_basic);
	run_case("ft_split", "séparateurs multiples / bords", t_split_multi);
	run_case("ft_split", "virgules, mots vides ignorés", t_split_commas);
	run_case("ft_split", "chaîne vide -> {NULL}", t_split_empty);
	run_case("ft_split", "que des séparateurs -> {NULL}", t_split_only_sep);
	run_case("ft_split", "aucun séparateur", t_split_no_sep);
	run_case("ft_split", "c = \\0", t_split_nul_sep);
	run_case("ft_split", "séparateur > 127", t_split_high);
	run_case("ft_split", "phrase longue", t_split_long);
	run_case("ft_strmapi", "toupper, source intacte, index", t_strmapi_basic);
	run_case("ft_strmapi", "f dépend de l'index", t_strmapi_index);
	run_case("ft_strmapi", "chaîne vide", t_strmapi_empty);
	run_case("ft_striteri", "modification en place, index", t_striteri_basic);
	run_case("ft_striteri", "chaîne vide", t_striteri_empty);
}
