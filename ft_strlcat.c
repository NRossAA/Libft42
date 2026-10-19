/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrossel <marvin@42lausanne.ch>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 09:57:23 by nrossel           #+#    #+#             */
/*   Updated: 2022/10/31 14:13:00 by nrossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	unsigned int	i;
	unsigned int	dstlen;
	unsigned int	srclen;

	if (dst == NULL)
		dstlen = 0;
	else
		dstlen = ft_strlen(dst);
	if (src == NULL)
		srclen = 0;
	else
		srclen = ft_strlen(src);
	if (dstsize == 0)
		return (srclen);
	i = dstsize;
	while (*dst)
	{
		dst++;
		if (i > 0)
			i--;
	}
	while (*src && i > 1)
	{
		*dst++ = *src++;
		i--;
	}
	*dst = 0;
	if (dstsize > dstlen)
		return (dstlen + srclen);
	return (dstsize + srclen);
}
