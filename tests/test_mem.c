#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "test.h"

/* Référence : memset(3), bzero(3), memcpy(3), memmove(3), memchr(3), memcmp(3). */

static void	fill2(unsigned char *a, unsigned char *b, size_t n, int v)
{
	memset(a, v, n);
	memset(b, v, n);
}

/* ---------------- memset ---------------- */

static void	t_memset_basic(void)
{
	unsigned char	a[16];
	unsigned char	b[16];
	void			*r;

	fill2(a, b, 16, 'X');
	r = ft_memset(a + 2, 'A', 5);
	memset(b + 2, 'A', 5);
	CHECK(r == a + 2);
	CHECK(memcmp(a, b, 16) == 0);
}

static void	t_memset_zero(void)
{
	unsigned char	a[16];
	unsigned char	b[16];
	void			*r;

	fill2(a, b, 16, 'X');
	r = ft_memset(a, 'A', 0);
	CHECK(r == a);
	CHECK(memcmp(a, b, 16) == 0);
}

/* c est converti en unsigned char. */
static void	t_memset_trunc(void)
{
	int				vals[] = {0x141, -1, 0x180, INT_MIN, 255, 0, INT_MAX};
	unsigned char	a[16];
	unsigned char	b[16];
	size_t			i;

	for (i = 0; i < sizeof(vals) / sizeof(*vals); i++)
	{
		fill2(a, b, 16, 'X');
		ft_memset(a + 1, vals[i], 8);
		memset(b + 1, vals[i], 8);
		CHECKF(memcmp(a, b, 16) == 0, "c=%d", vals[i]);
	}
}

/* ---------------- bzero ---------------- */

static void	t_bzero_basic(void)
{
	unsigned char	a[16];
	unsigned char	b[16];

	fill2(a, b, 16, 'X');
	ft_bzero(a + 3, 5);
	memset(b + 3, 0, 5);
	CHECK(memcmp(a, b, 16) == 0);
}

static void	t_bzero_zero(void)
{
	unsigned char	a[16];
	unsigned char	b[16];

	fill2(a, b, 16, 'X');
	ft_bzero(a, 0);
	CHECK(memcmp(a, b, 16) == 0);
}

/* ---------------- memcpy ---------------- */

static void	t_memcpy_basic(void)
{
	static const unsigned char	src[6] = {'a', 'b', 0, 'c', 0xff, 'd'};
	unsigned char				a[16];
	unsigned char				b[16];
	void						*r;

	fill2(a, b, 16, 'X');
	r = ft_memcpy(a + 1, src, 6);
	memcpy(b + 1, src, 6);
	CHECK(r == a + 1);
	CHECK(memcmp(a, b, 16) == 0);
}

static void	t_memcpy_zero(void)
{
	unsigned char	a[16];
	unsigned char	b[16];
	void			*r;

	fill2(a, b, 16, 'X');
	r = ft_memcpy(a, "hello", 0);
	CHECK(r == a);
	CHECK(memcmp(a, b, 16) == 0);
}

static void	t_memcpy_large(void)
{
	unsigned char	*src;
	unsigned char	*dst;
	size_t			i;

	src = malloc(4096);
	dst = calloc(4096, 1);
	if (!src || !dst)
		return ;
	for (i = 0; i < 4096; i++)
		src[i] = (unsigned char)(i * 7);
	CHECK(ft_memcpy(dst, src, 4096) == dst);
	CHECK(memcmp(dst, src, 4096) == 0);
	free(src);
	free(dst);
}

/* ---------------- memmove ---------------- */

static void	mm_case(size_t dst_off, size_t src_off, size_t n)
{
	unsigned char	a[64];
	unsigned char	b[64];
	size_t			i;
	void			*r;

	for (i = 0; i < 64; i++)
	{
		a[i] = (unsigned char)(i * 3 + 1);
		b[i] = a[i];
	}
	r = ft_memmove(a + dst_off, a + src_off, n);
	memmove(b + dst_off, b + src_off, n);
	CHECKF(r == a + dst_off, "valeur de retour (dst=+%zu)", dst_off);
	CHECKF(memcmp(a, b, 64) == 0, "dst=+%zu src=+%zu n=%zu",
		dst_off, src_off, n);
}

static void	t_memmove_fwd(void)
{
	mm_case(5, 0, 20);
	mm_case(1, 0, 40);
}

static void	t_memmove_bwd(void)
{
	mm_case(0, 5, 20);
	mm_case(0, 1, 40);
}

static void	t_memmove_same(void)
{
	mm_case(3, 3, 10);
}

static void	t_memmove_zero(void)
{
	mm_case(10, 0, 0);
}

static void	t_memmove_disjoint(void)
{
	mm_case(40, 0, 10);
	mm_case(0, 40, 10);
}

static void	t_memmove_large(void)
{
	unsigned char	*a;
	unsigned char	*b;
	size_t			i;

	a = malloc(8192);
	b = malloc(8192);
	if (!a || !b)
		return ;
	for (i = 0; i < 8192; i++)
		a[i] = (unsigned char)(i % 251);
	memcpy(b, a, 8192);
	ft_memmove(a + 1, a, 8191);
	memmove(b + 1, b, 8191);
	CHECK(memcmp(a, b, 8192) == 0);
	ft_memmove(a, a + 1, 8191);
	memmove(b, b + 1, 8191);
	CHECK(memcmp(a, b, 8192) == 0);
	free(a);
	free(b);
}

/* ---------------- memchr ---------------- */

static const unsigned char	g_mc[8] = {'a', 'b', 'c', 0, 'd', 'e', 'f', 0xff};

static void	mc(int c, size_t n)
{
	CHECKF(ft_memchr(g_mc, c, n) == memchr(g_mc, c, n), "c=%d n=%zu", c, n);
}

static void	t_memchr_found(void)
{
	mc('a', 8);
	mc('f', 8);
}

static void	t_memchr_after_nul(void)
{
	mc('d', 8);
	mc('\0', 8);
}

static void	t_memchr_high(void)
{
	mc(0xff, 8);
	mc(-1, 8);
	mc(0x100 + 'b', 8);
}

static void	t_memchr_absent(void)
{
	mc('z', 8);
}

static void	t_memchr_limit(void)
{
	mc('d', 3);
	mc('d', 4);
	mc('d', 5);
	mc('a', 0);
}

/* ---------------- memcmp ---------------- */

static void	mcmp(const void *a, const void *b, size_t n)
{
	int	ft;
	int	ref;

	ft = ft_memcmp(a, b, n);
	ref = memcmp(a, b, n);
	CHECKF(sgn(ft) == sgn(ref), "n=%zu ft=%d libc=%d", n, ft, ref);
}

static void	t_memcmp_equal(void)
{
	mcmp("abc", "abc", 3);
	mcmp("abc", "abc", 4);
}

static void	t_memcmp_diff(void)
{
	mcmp("abcd", "abce", 4);
	mcmp("abce", "abcd", 4);
	mcmp("abcX", "abcY", 3);
}

static void	t_memcmp_zero(void)
{
	mcmp("a", "b", 0);
}

static void	t_memcmp_after_nul(void)
{
	mcmp("ab\0x", "ab\0y", 4);
}

static void	t_memcmp_high(void)
{
	mcmp("\x80", "\x01", 1);
	mcmp("\x01", "\xff", 1);
	mcmp("a\xfe", "a\x7f", 2);
}

void	suite_mem(void)
{
	run_case("ft_memset", "cas simple + valeur de retour", t_memset_basic);
	run_case("ft_memset", "n = 0", t_memset_zero);
	run_case("ft_memset", "c tronqué en unsigned char (>255, <0)", t_memset_trunc);
	run_case("ft_bzero", "cas simple, pas d'écriture hors zone", t_bzero_basic);
	run_case("ft_bzero", "n = 0", t_bzero_zero);
	run_case("ft_memcpy", "octets nuls et >127 + retour", t_memcpy_basic);
	run_case("ft_memcpy", "n = 0", t_memcpy_zero);
	run_case("ft_memcpy", "4096 octets", t_memcpy_large);
	run_case("ft_memmove", "chevauchement dst > src", t_memmove_fwd);
	run_case("ft_memmove", "chevauchement dst < src", t_memmove_bwd);
	run_case("ft_memmove", "dst == src", t_memmove_same);
	run_case("ft_memmove", "n = 0", t_memmove_zero);
	run_case("ft_memmove", "zones disjointes", t_memmove_disjoint);
	run_case("ft_memmove", "8192 octets, décalage de 1", t_memmove_large);
	run_case("ft_memchr", "trouvé", t_memchr_found);
	run_case("ft_memchr", "après un \\0, recherche de \\0", t_memchr_after_nul);
	run_case("ft_memchr", "c > 127, c = -1, c = 256 + x", t_memchr_high);
	run_case("ft_memchr", "absent", t_memchr_absent);
	run_case("ft_memchr", "limite n (dont n = 0)", t_memchr_limit);
	run_case("ft_memcmp", "égal", t_memcmp_equal);
	run_case("ft_memcmp", "différent / arrêt à n", t_memcmp_diff);
	run_case("ft_memcmp", "n = 0", t_memcmp_zero);
	run_case("ft_memcmp", "ne s'arrête pas sur \\0", t_memcmp_after_nul);
	run_case("ft_memcmp", "signe avec octets > 127", t_memcmp_high);
}
