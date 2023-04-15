/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/16 01:11:38 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/16 01:11:39 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

#if BYTE_ORDER == BIG_ENDIAN

uint16_t	ft_htons(uint16_t n)
{
	return (n);
}

uint16_t	ft_ntohs(uint16_t n)
{
	return (n);
}

#else

uint16_t	ft_htons(uint16_t n)
{
	return (((n & 0xFF) << 8) | ((n & 0xFF00) >> 8));
}

uint16_t	ft_ntohs(uint16_t n)
{
	return (((n & 0xFF) << 8) | ((n & 0xFF00) >> 8));
}

#endif
