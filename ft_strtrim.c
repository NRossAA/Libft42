/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrossel <marvin@42lausanne.ch>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/02 15:05:04 by nrossel           #+#    #+#             */
/*   Updated: 2022/11/02 16:10:38 by nrossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*#include <stdio.h>*/

static int	isset(char c, char const *set)
{
	if (!set || !c)
		return (0);

	int	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t  start;
          size_t  end;

          start = 0;
          while (s1[start] && isset(s1[start], set))
                  start++;
          end = ft_strlen(s1);
          while (end > start && isset(s1[end - 1], set))
                  end--;
          return (ft_substr(s1, start, end - start));
}

/*int	main (int argc, char **argv)
{
	(void) argc;
	{
		char *str1 = argv[1];
		char *str2 = argv[2];
		char *ret = ft_strtrim(str1, str2);
		printf("%s", ret);
	}
}*/
