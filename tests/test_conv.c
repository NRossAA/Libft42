#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "test.h"

/*
** Référence : atoi(3) (hors overflow, qui est UB -> test_ub.c),
** calloc(3) (NULL si count * size déborde), itoa : sujet 42, comparé
** à snprintf("%d").
*/

/* ---------------- atoi ---------------- */

static void	atoi_all(const char **v, size_t n)
{
	size_t	i;

	for (i = 0; i < n; i++)
		CHECKF(ft_atoi(v[i]) == atoi(v[i]), "\"%s\" : ft=%d libc=%d",
			v[i], ft_atoi(v[i]), atoi(v[i]));
}

static void	t_atoi_ws(void)
{
	const char	*v[] = {"42", "   42", "\t\n\v\f\r 42", "\n\n-42", " \t+7"};

	atoi_all(v, sizeof(v) / sizeof(*v));
}

static void	t_atoi_signs(void)
{
	const char	*v[] = {"+42", "-42", "+-42", "-+42", "--42", "++42",
		"- 42", "+ 42"};

	atoi_all(v, sizeof(v) / sizeof(*v));
}

static void	t_atoi_limits(void)
{
	const char	*v[] = {"2147483647", "-2147483648", "0", "-0", "+0",
		"0000042", "-0000042"};

	atoi_all(v, sizeof(v) / sizeof(*v));
}

static void	t_atoi_junk(void)
{
	const char	*v[] = {"42abc", "4 2", "abc", "", "   ", "\x80" "42",
		"42\x80", "-", "+", "\x1c" "42"};

	atoi_all(v, sizeof(v) / sizeof(*v));
}

/* ---------------- itoa ---------------- */

static void	itoa_case(int n)
{
	char	exp[16];
	char	*r;

	snprintf(exp, sizeof(exp), "%d", n);
	r = ft_itoa(n);
	CHECK_STR(r, exp, "ft_itoa(%d)", n);
	free(r);
}

static void	t_itoa_zero(void)
{
	itoa_case(0);
}

static void	t_itoa_small(void)
{
	itoa_case(1);
	itoa_case(-1);
	itoa_case(9);
	itoa_case(-9);
	itoa_case(10);
	itoa_case(-10);
}

static void	t_itoa_big(void)
{
	itoa_case(123456);
	itoa_case(-987654);
	itoa_case(1000000);
}

static void	t_itoa_limits(void)
{
	itoa_case(INT_MAX);
	itoa_case(INT_MIN);
	itoa_case(INT_MIN + 1);
}

/* ---------------- calloc ---------------- */

static int	all_zero(const unsigned char *p, size_t n)
{
	size_t	i;

	for (i = 0; i < n; i++)
		if (p[i])
			return (0);
	return (1);
}

static void	t_calloc_zeroed(void)
{
	unsigned char	*p;

	p = ft_calloc(10, sizeof(int));
	CHECK(p != NULL);
	if (!p)
		return ;
	CHECK(all_zero(p, 10 * sizeof(int)));
	free(p);
}

static void	t_calloc_big(void)
{
	unsigned char	*p;

	p = ft_calloc(1000, 1000);
	CHECK(p != NULL);
	if (!p)
		return ;
	CHECK(all_zero(p, 1000 * 1000));
	free(p);
}

/* calloc(0, x) : NULL ou pointeur unique libérable ; les deux sont OK. */
static void	t_calloc_zero_size(void)
{
	free(ft_calloc(0, 0));
	free(ft_calloc(0, 10));
	free(ft_calloc(10, 0));
}

static void	t_calloc_ovf_wrap0(void)
{
	void	*p;

	p = ft_calloc(SIZE_MAX / 2 + 1, 2);
	CHECKF(p == NULL, "count*size déborde (vaut 0 modulo 2^64)");
	free(p);
	p = ft_calloc((size_t)1 << 32, (size_t)1 << 32);
	CHECKF(p == NULL, "2^32 * 2^32 déborde (vaut 0 modulo 2^64)");
	free(p);
}

static void	t_calloc_ovf_wrap1(void)
{
	void	*p;

	p = ft_calloc(SIZE_MAX, SIZE_MAX);
	CHECKF(p == NULL, "SIZE_MAX * SIZE_MAX déborde (vaut 1 modulo 2^64)");
	free(p);
}

static void	t_calloc_ovf_huge(void)
{
	void	*p;

	p = ft_calloc(SIZE_MAX, 2);
	CHECK(p == NULL);
	free(p);
}

void	suite_conv(void)
{
	run_case("ft_atoi", "espaces \\t\\n\\v\\f\\r", t_atoi_ws);
	run_case("ft_atoi", "signes (+, -, +-, --, ...)", t_atoi_signs);
	run_case("ft_atoi", "INT_MAX, INT_MIN, zéros", t_atoi_limits);
	run_case("ft_atoi", "caractères parasites, vide, > 127", t_atoi_junk);
	run_case("ft_itoa", "0", t_itoa_zero);
	run_case("ft_itoa", "petits nombres +/-", t_itoa_small);
	run_case("ft_itoa", "grands nombres", t_itoa_big);
	run_case("ft_itoa", "INT_MAX, INT_MIN", t_itoa_limits);
	run_case("ft_calloc", "mémoire à zéro", t_calloc_zeroed);
	run_case("ft_calloc", "1 Mo à zéro", t_calloc_big);
	run_case("ft_calloc", "count ou size = 0", t_calloc_zero_size);
	run_case("ft_calloc", "overflow count*size -> 0", t_calloc_ovf_wrap0);
	run_case("ft_calloc", "overflow count*size -> 1", t_calloc_ovf_wrap1);
	run_case("ft_calloc", "overflow vers une taille énorme", t_calloc_ovf_huge);
}
