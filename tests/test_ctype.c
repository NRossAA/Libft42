#include <ctype.h>
#include <limits.h>
#include "test.h"

/*
** Référence : ctype(3) en locale "C". Les is* de la libc renvoient
** "non nul" (pas forcément 1) : on compare donc la valeur de vérité.
** Domaine défini : EOF (-1) et 0..255.
*/

#define TRUTH_LOOP(ft, ref) \
	for (c = -1; c <= 255; c++) \
		CHECKF(!ft(c) == !ref(c), "c=%d ft=%d libc=%d", c, ft(c), ref(c))

#define EXACT_LOOP(ft, ref) \
	for (c = -1; c <= 255; c++) \
		CHECKF(ft(c) == ref(c), "c=%d ft=%d libc=%d", c, ft(c), ref(c))

static void	t_isalpha(void)
{
	int	c;

	TRUTH_LOOP(ft_isalpha, isalpha);
}

static void	t_isdigit(void)
{
	int	c;

	TRUTH_LOOP(ft_isdigit, isdigit);
}

static void	t_isalnum(void)
{
	int	c;

	TRUTH_LOOP(ft_isalnum, isalnum);
}

static void	t_isascii(void)
{
	int	c;

	TRUTH_LOOP(ft_isascii, isascii);
}

/* isascii est défini pour tout int (pas seulement EOF..255). */
static void	t_isascii_ints(void)
{
	int		vals[] = {INT_MIN, -129, -128, -2, 128, 256, 1000, INT_MAX};
	size_t	i;

	for (i = 0; i < sizeof(vals) / sizeof(*vals); i++)
		CHECKF(!ft_isascii(vals[i]) == !isascii(vals[i]), "c=%d", vals[i]);
}

static void	t_isprint(void)
{
	int	c;

	TRUTH_LOOP(ft_isprint, isprint);
}

static void	t_toupper(void)
{
	int	c;

	EXACT_LOOP(ft_toupper, toupper);
}

static void	t_tolower(void)
{
	int	c;

	EXACT_LOOP(ft_tolower, tolower);
}

void	suite_ctype(void)
{
	run_case("ft_isalpha", "EOF et 0..255", t_isalpha);
	run_case("ft_isdigit", "EOF et 0..255", t_isdigit);
	run_case("ft_isalnum", "EOF et 0..255", t_isalnum);
	run_case("ft_isascii", "EOF et 0..255", t_isascii);
	run_case("ft_isascii", "INT_MIN, négatifs, >255, INT_MAX", t_isascii_ints);
	run_case("ft_isprint", "EOF et 0..255", t_isprint);
	run_case("ft_toupper", "EOF et 0..255 (valeur exacte)", t_toupper);
	run_case("ft_tolower", "EOF et 0..255 (valeur exacte)", t_tolower);
}
