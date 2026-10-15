#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "test.h"

/*
** Arguments dont le comportement est indéfini (libc) ou non spécifié
** (sujet 42). Aucun résultat n'est "attendu" : on observe ce que font
** ft_ et la libc, côte à côte. Rien ici n'est compté comme pass/fail.
**
** NULL passe par un pointeur volatile : sinon gcc voit l'attribut nonnull
** de la glibc (erreur avec -Werror) ou supprime l'appel.
*/

static char *volatile	g_null = NULL;
static char				g_buf[16] = "abc";

static const char	*p(const void *ptr)
{
	return (ptr ? "pointeur non-NULL" : "NULL");
}

#define UB_SIZE(name, expr) static void name(void) { ub_note("retour = %zu", (size_t)(expr)); }
#define UB_INT(name, expr) static void name(void) { ub_note("retour = %d", (int)(expr)); }
#define UB_PTR(name, expr) static void name(void) { ub_note("retour = %s", p(expr)); }
#define UB_VOID(name, stmt) static void name(void) { stmt; ub_note("retour normal"); }

UB_SIZE(f_strlen, ft_strlen(g_null))
UB_SIZE(l_strlen, strlen(g_null))
UB_PTR(f_memset, ft_memset(g_null, 'a', 5))
UB_PTR(l_memset, memset(g_null, 'a', 5))
UB_VOID(f_bzero, ft_bzero(g_null, 5))
UB_VOID(l_bzero, bzero(g_null, 5))
UB_PTR(f_memcpy_nn5, ft_memcpy(g_null, g_null, 5))
UB_PTR(l_memcpy_nn5, memcpy(g_null, g_null, 5))
UB_PTR(f_memcpy_nn0, ft_memcpy(g_null, g_null, 0))
UB_PTR(l_memcpy_nn0, memcpy(g_null, g_null, 0))
UB_PTR(f_memcpy_bn5, ft_memcpy(g_buf, g_null, 5))
UB_PTR(l_memcpy_bn5, memcpy(g_buf, g_null, 5))
UB_PTR(f_memmove, ft_memmove(g_null, g_null, 5))
UB_PTR(l_memmove, memmove(g_null, g_null, 5))
UB_PTR(f_memchr, ft_memchr(g_null, 'a', 5))
UB_PTR(l_memchr, memchr(g_null, 'a', 5))
UB_INT(f_memcmp, ft_memcmp(g_null, "abc", 3))
UB_INT(l_memcmp, memcmp(g_null, "abc", 3))
UB_PTR(f_strchr, ft_strchr(g_null, 'a'))
UB_PTR(l_strchr, strchr(g_null, 'a'))
UB_PTR(f_strrchr, ft_strrchr(g_null, 'a'))
UB_PTR(l_strrchr, strrchr(g_null, 'a'))
UB_INT(f_strncmp, ft_strncmp(g_null, "abc", 3))
UB_INT(l_strncmp, strncmp(g_null, "abc", 3))
UB_INT(f_strncmp0, ft_strncmp(g_null, g_null, 0))
UB_INT(l_strncmp0, strncmp(g_null, g_null, 0))
UB_PTR(f_strnstr_h, ft_strnstr(g_null, "a", 5))
UB_PTR(f_strnstr_n, ft_strnstr("abc", g_null, 3))
UB_SIZE(f_strlcpy_d, ft_strlcpy(g_null, "abc", 0))
UB_SIZE(l_strlcpy_d, strlcpy(g_null, "abc", 0))
UB_SIZE(f_strlcpy_s, ft_strlcpy(g_buf, g_null, 5))
UB_SIZE(l_strlcpy_s, strlcpy(g_buf, g_null, 5))
UB_SIZE(f_strlcat_d, ft_strlcat(g_null, "abc", 0))
UB_SIZE(l_strlcat_d, strlcat(g_null, "abc", 0))
UB_PTR(f_strdup, ft_strdup(g_null))
UB_PTR(l_strdup, strdup(g_null))
UB_INT(f_atoi_null, ft_atoi(g_null))
UB_INT(l_atoi_null, atoi(g_null))
UB_INT(f_atoi_o1, ft_atoi("2147483648"))
UB_INT(l_atoi_o1, atoi("2147483648"))
UB_INT(f_atoi_o2, ft_atoi("-2147483649"))
UB_INT(l_atoi_o2, atoi("-2147483649"))
UB_INT(f_atoi_o3, ft_atoi("99999999999999999999"))
UB_INT(l_atoi_o3, atoi("99999999999999999999"))
UB_INT(f_isalpha_min, ft_isalpha(INT_MIN))
UB_INT(l_isalpha_min, isalpha(INT_MIN))
UB_INT(f_toupper_1000, ft_toupper(1000))
UB_INT(l_toupper_1000, toupper(1000))

static char	map_id(unsigned int i, char c)
{
	(void)i;
	return (c);
}

static void	iter_id(unsigned int i, char *c)
{
	(void)i;
	(void)c;
}

static void	del_nop(void *ptr)
{
	(void)ptr;
}

UB_PTR(f_substr, ft_substr(g_null, 0, 5))
UB_PTR(f_strjoin1, ft_strjoin(g_null, "abc"))
UB_PTR(f_strjoin2, ft_strjoin("abc", g_null))
UB_PTR(f_strtrim1, ft_strtrim(g_null, " "))
UB_PTR(f_strtrim2, ft_strtrim("abc", g_null))
UB_PTR(f_split, ft_split(g_null, ' '))
UB_PTR(f_strmapi1, ft_strmapi(g_null, map_id))
UB_PTR(f_strmapi2, ft_strmapi("abc", NULL))
UB_VOID(f_striteri1, ft_striteri(g_null, iter_id))
UB_VOID(f_striteri2, ft_striteri(g_buf, NULL))
UB_VOID(f_putstr, ft_putstr_fd(g_null, -1))
UB_VOID(f_putendl, ft_putendl_fd(g_null, -1))

static void	f_add_front_null(void)
{
	t_list	n;

	n.content = NULL;
	n.next = NULL;
	ft_lstadd_front(NULL, &n);
	ub_note("retour normal");
}

static void	f_add_back_null(void)
{
	t_list	n;

	n.content = NULL;
	n.next = NULL;
	ft_lstadd_back(NULL, &n);
	ub_note("retour normal");
}

static void	f_add_back_new_null(void)
{
	t_list	n;
	t_list	*l;

	n.content = NULL;
	n.next = NULL;
	l = &n;
	ft_lstadd_back(&l, NULL);
	ub_note("retour normal, liste %s", l == &n && n.next == NULL
		? "inchangée" : "modifiée");
}

UB_VOID(f_lstclear_null, ft_lstclear(NULL, del_nop))

static void	f_lstclear_del_null(void)
{
	t_list	*l;

	l = malloc(sizeof(*l));
	if (!l)
		return ;
	l->content = NULL;
	l->next = NULL;
	ft_lstclear(&l, NULL);
	ub_note("retour normal");
}

static void	f_lstdelone_del_null(void)
{
	t_list	*l;

	l = malloc(sizeof(*l));
	if (!l)
		return ;
	l->content = NULL;
	l->next = NULL;
	ft_lstdelone(l, NULL);
	ub_note("retour normal");
}

static void	f_lstiter_f_null(void)
{
	t_list	n;

	n.content = NULL;
	n.next = NULL;
	ft_lstiter(&n, NULL);
	ub_note("retour normal");
}

void	suite_ub(void)
{
	run_ub("strlen(NULL)", f_strlen, l_strlen);
	run_ub("memset(NULL, 'a', 5)", f_memset, l_memset);
	run_ub("bzero(NULL, 5)", f_bzero, l_bzero);
	run_ub("memcpy(NULL, NULL, 5)", f_memcpy_nn5, l_memcpy_nn5);
	run_ub("memcpy(NULL, NULL, 0)", f_memcpy_nn0, l_memcpy_nn0);
	run_ub("memcpy(buf, NULL, 5)", f_memcpy_bn5, l_memcpy_bn5);
	run_ub("memmove(NULL, NULL, 5)", f_memmove, l_memmove);
	run_ub("memchr(NULL, 'a', 5)", f_memchr, l_memchr);
	run_ub("memcmp(NULL, \"abc\", 3)", f_memcmp, l_memcmp);
	run_ub("strchr(NULL, 'a')", f_strchr, l_strchr);
	run_ub("strrchr(NULL, 'a')", f_strrchr, l_strrchr);
	run_ub("strncmp(NULL, \"abc\", 3)", f_strncmp, l_strncmp);
	run_ub("strncmp(NULL, NULL, 0)", f_strncmp0, l_strncmp0);
	run_ub("strnstr(NULL, \"a\", 5)", f_strnstr_h, NULL);
	run_ub("strnstr(\"abc\", NULL, 3)", f_strnstr_n, NULL);
	run_ub("strlcpy(NULL, \"abc\", 0)", f_strlcpy_d, l_strlcpy_d);
	run_ub("strlcpy(buf, NULL, 5)", f_strlcpy_s, l_strlcpy_s);
	run_ub("strlcat(NULL, \"abc\", 0)", f_strlcat_d, l_strlcat_d);
	run_ub("strdup(NULL)", f_strdup, l_strdup);
	run_ub("atoi(NULL)", f_atoi_null, l_atoi_null);
	run_ub("atoi(\"2147483648\") (overflow)", f_atoi_o1, l_atoi_o1);
	run_ub("atoi(\"-2147483649\") (overflow)", f_atoi_o2, l_atoi_o2);
	run_ub("atoi(\"99999999999999999999\")", f_atoi_o3, l_atoi_o3);
	run_ub("isalpha(INT_MIN)", f_isalpha_min, l_isalpha_min);
	run_ub("toupper(1000)", f_toupper_1000, l_toupper_1000);
	run_ub("ft_substr(NULL, 0, 5)", f_substr, NULL);
	run_ub("ft_strjoin(NULL, \"abc\")", f_strjoin1, NULL);
	run_ub("ft_strjoin(\"abc\", NULL)", f_strjoin2, NULL);
	run_ub("ft_strtrim(NULL, \" \")", f_strtrim1, NULL);
	run_ub("ft_strtrim(\"abc\", NULL)", f_strtrim2, NULL);
	run_ub("ft_split(NULL, ' ')", f_split, NULL);
	run_ub("ft_strmapi(NULL, f)", f_strmapi1, NULL);
	run_ub("ft_strmapi(\"abc\", NULL)", f_strmapi2, NULL);
	run_ub("ft_striteri(NULL, f)", f_striteri1, NULL);
	run_ub("ft_striteri(\"abc\", NULL)", f_striteri2, NULL);
	run_ub("ft_putstr_fd(NULL, fd)", f_putstr, NULL);
	run_ub("ft_putendl_fd(NULL, fd)", f_putendl, NULL);
	run_ub("ft_lstadd_front(NULL, new)", f_add_front_null, NULL);
	run_ub("ft_lstadd_back(NULL, new)", f_add_back_null, NULL);
	run_ub("ft_lstadd_back(&lst, NULL)", f_add_back_new_null, NULL);
	run_ub("ft_lstclear(NULL, del)", f_lstclear_null, NULL);
	run_ub("ft_lstclear(&lst, NULL)", f_lstclear_del_null, NULL);
	run_ub("ft_lstdelone(lst, NULL)", f_lstdelone_del_null, NULL);
	run_ub("ft_lstiter(lst, NULL)", f_lstiter_f_null, NULL);
}
