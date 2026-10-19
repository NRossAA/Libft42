#include <stdlib.h>
#include <string.h>
#include "test.h"

/*
** Bonus, sujet 42. Une liste vide (t_list *lst == NULL) est un argument
** valide ; un pointeur de liste NULL (t_list **) ne l'est pas (non testé).
** Les listes sont construites à la main (sans ft_lstnew) pour que chaque
** test ne dépende que de la fonction testée.
*/

static t_list	*mk(void *content, t_list *next)
{
	t_list	*n;

	n = malloc(sizeof(*n));
	if (!n)
		abort();
	n->content = content;
	n->next = next;
	return (n);
}

static void	free_list(t_list *l)
{
	t_list	*next;

	while (l)
	{
		next = l->next;
		free(l);
		l = next;
	}
}

static int	g_x = 1;
static int	g_y = 2;
static int	g_z = 3;
static int	g_del_calls;
static void	*g_del_last;
static void	*g_seen[8];
static int	g_nseen;

static void	del_count(void *p)
{
	g_del_calls++;
	g_del_last = p;
}

static void	del_free(void *p)
{
	g_del_calls++;
	free(p);
}

static void	rec(void *p)
{
	if (g_nseen < 8)
		g_seen[g_nseen] = p;
	g_nseen++;
}

/* ---------------- lstnew ---------------- */

static void	t_lstnew(void)
{
	t_list	*n;

	n = ft_lstnew(&g_x);
	CHECK(n != NULL);
	if (!n)
		return ;
	CHECK(n->content == &g_x);
	CHECK(n->next == NULL);
	free(n);
}

static void	t_lstnew_null(void)
{
	t_list	*n;

	n = ft_lstnew(NULL);
	CHECK(n != NULL);
	if (!n)
		return ;
	CHECK(n->content == NULL);
	CHECK(n->next == NULL);
	free(n);
}

/* ---------------- lstadd_front ---------------- */

static void	t_add_front_empty(void)
{
	t_list	*l;
	t_list	*n;

	l = NULL;
	n = mk(&g_x, NULL);
	ft_lstadd_front(&l, n);
	CHECK(l == n);
	CHECK(n->next == NULL);
	free_list(l);
}

static void	t_add_front(void)
{
	t_list	*a;
	t_list	*l;
	t_list	*n;

	a = mk(&g_x, NULL);
	l = a;
	n = mk(&g_y, NULL);
	ft_lstadd_front(&l, n);
	CHECK(l == n);
	CHECK(n->next == a);
	free_list(l);
}

/* ---------------- lstsize / lstlast ---------------- */

static void	t_lstsize(void)
{
	t_list	*l;

	CHECK(ft_lstsize(NULL) == 0);
	l = mk(&g_x, NULL);
	CHECK(ft_lstsize(l) == 1);
	l = mk(&g_y, mk(&g_z, l));
	CHECK(ft_lstsize(l) == 3);
	free_list(l);
}

static void	t_lstlast(void)
{
	t_list	*c;
	t_list	*l;

	CHECK(ft_lstlast(NULL) == NULL);
	c = mk(&g_z, NULL);
	CHECK(ft_lstlast(c) == c);
	l = mk(&g_x, mk(&g_y, c));
	CHECK(ft_lstlast(l) == c);
	free_list(l);
}

/* ---------------- lstadd_back ---------------- */

static void	t_add_back_empty(void)
{
	t_list	*l;
	t_list	*n;

	l = NULL;
	n = mk(&g_x, NULL);
	ft_lstadd_back(&l, n);
	CHECK(l == n);
	free_list(l);
}

static void	t_add_back(void)
{
	t_list	*a;
	t_list	*b;
	t_list	*c;
	t_list	*l;

	b = mk(&g_y, NULL);
	a = mk(&g_x, b);
	l = a;
	c = mk(&g_z, NULL);
	ft_lstadd_back(&l, c);
	CHECK(l == a);
	CHECK(b->next == c);
	CHECK(c->next == NULL);
	free_list(l);
}

/* ---------------- lstdelone ---------------- */

static void	t_lstdelone(void)
{
	t_list	*a;
	t_list	*b;

	b = mk(&g_y, NULL);
	a = mk(&g_x, b);
	g_del_calls = 0;
	ft_lstdelone(a, del_count);
	CHECK(g_del_calls == 1);
	CHECK(g_del_last == &g_x);
	CHECK(b->content == &g_y);
	free(b);
}

/* ---------------- lstclear ---------------- */

static void	t_lstclear(void)
{
	t_list	*l;

	l = mk(strdup("a"), mk(strdup("b"), mk(strdup("c"), NULL)));
	g_del_calls = 0;
	ft_lstclear(&l, del_free);
	CHECK(l == NULL);
	CHECK(g_del_calls == 3);
}

static void	t_lstclear_empty(void)
{
	t_list	*l;

	l = NULL;
	g_del_calls = 0;
	ft_lstclear(&l, del_count);
	CHECK(l == NULL);
	CHECK(g_del_calls == 0);
}

/* ---------------- lstiter ---------------- */

static void	t_lstiter(void)
{
	t_list	*l;

	l = mk(&g_x, mk(&g_y, mk(&g_z, NULL)));
	g_nseen = 0;
	ft_lstiter(l, rec);
	CHECK(g_nseen == 3);
	CHECK(g_seen[0] == &g_x && g_seen[1] == &g_y && g_seen[2] == &g_z);
	free_list(l);
}

static void	t_lstiter_empty(void)
{
	g_nseen = 0;
	ft_lstiter(NULL, rec);
	CHECK(g_nseen == 0);
}

void	suite_lst(void)
{
	run_case("ft_lstnew", "content + next = NULL", t_lstnew);
	run_case("ft_lstnew", "content NULL", t_lstnew_null);
	run_case("ft_lstadd_front", "liste vide", t_add_front_empty);
	run_case("ft_lstadd_front", "liste non vide", t_add_front);
	run_case("ft_lstsize", "0, 1, 3 éléments", t_lstsize);
	run_case("ft_lstlast", "vide, 1, 3 éléments", t_lstlast);
	run_case("ft_lstadd_back", "liste vide", t_add_back_empty);
	run_case("ft_lstadd_back", "liste non vide", t_add_back);
	run_case("ft_lstdelone", "del appelé, next non libéré", t_lstdelone);
	run_case("ft_lstclear", "3 éléments, *lst = NULL, pas de fuite", t_lstclear);
	run_case("ft_lstclear", "liste vide", t_lstclear_empty);
	run_case("ft_lstiter", "ordre d'application", t_lstiter);
	run_case("ft_lstiter", "liste vide", t_lstiter_empty);
}
