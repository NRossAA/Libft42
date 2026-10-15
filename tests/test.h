#ifndef TEST_H
# define TEST_H

# include <stddef.h>
# include "libft.h"

/*
** Mini-framework : chaque cas tourne dans un processus enfant (fork).
** CHECK* enregistre l'échec et continue ; le cas est marqué FAIL à la fin.
** Un crash, un rapport ASan ou une fuite (LeakSanitizer) font aussi échouer
** le cas, sans arrêter les suivants.
*/

void	checkf_impl(int ok, const char *expr, const char *file, int line,
			const char *fmt, ...);
void	check_str_impl(const char *got, const char *exp, const char *file,
			int line, const char *fmt, ...);
void	ub_note(const char *fmt, ...);
void	run_case(const char *group, const char *name, void (*fn)(void));
void	run_ub(const char *name, void (*ft)(void), void (*libc)(void));
int		sgn(int x);

void	suite_ctype(void);
void	suite_mem(void);
void	suite_str(void);
void	suite_conv(void);
void	suite_alloc(void);
void	suite_fd(void);
void	suite_lst(void);
void	suite_ub(void);

# define CHECK(e)		checkf_impl(!!(e), #e, __FILE__, __LINE__, NULL)
# define CHECKF(e, ...)	checkf_impl(!!(e), #e, __FILE__, __LINE__, __VA_ARGS__)
# define CHECK_STR(got, exp, ...) \
	check_str_impl((got), (exp), __FILE__, __LINE__, __VA_ARGS__)

#endif
