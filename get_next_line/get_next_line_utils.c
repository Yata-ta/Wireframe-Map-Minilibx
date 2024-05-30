/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artuda-s <artuda-s@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 17:50:52 by artuda-s          #+#    #+#             */
/*   Updated: 2024/05/13 17:50:53 by artuda-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (char)c)
			return (1);
		else
			s++;
	}
	return (0);
}

char	*ft_strcalloc(size_t nitems, size_t size)
{
	char	*pstr;
	int		i;

	i = 0;
	pstr = (char *)malloc(nitems * size);
	if (pstr == NULL)
		return (NULL);
	while (nitems > 0)
	{
		pstr[i] = 0;
		i++;
		nitems--;
	}
	return (pstr);
}

char	*strjoin(char *s1, char *s2)
{
	char	*nstr;
	size_t	i;

	nstr = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!nstr)
		return (nstr = NULL, NULL);
	i = 0;
	while (*s1)
		nstr[i++] = *(s1++);
	while (*s2)
		nstr[i++] = *(s2++);
	nstr[i] = 0;
	return (nstr);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	i;
	size_t	slen;

	slen = ft_strlen((s));
	if ((slen < start) || *s == 0)
		return (ft_strcalloc(1, sizeof(char)));
	if (len > ft_strlen(s + start))
		len = ft_strlen(s + start);
	substr = (char *)malloc(sizeof(char) * (len + 1));
	if (!substr)
		return (NULL);
	i = 0;
	while (i < len)
	{
		substr[i] = s[start + i];
		i++;
	}
	substr[i] = 0;
	return (substr);
}