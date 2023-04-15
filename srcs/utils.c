/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 01:46:05 by aartiges          #+#    #+#             */
/*   Updated: 2023/04/15 22:29:36 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

uint16_t	compute_icmp_checksum(const void *buff, int length)
{
	const uint16_t	*ptr = buff;
	uint32_t		sum;

	sum = 0;
	while (length > 1)
	{
		sum += *ptr++;
		length -= 2;
	}
	if (length == 1)
		sum += *(uint8_t *) ptr;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (~sum);
}

int	ft_atoi(const char *str)
{
	long	result;
	long	sign;

	result = 0;
	sign = 1;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		if (result > INT_MAX)
			return (0);
		result = result * 10 + (*str - '0');
		str++;
	}
	return ((int)(result * sign));
}

long	get_diff_tv(struct timeval tv_recv, struct timeval tv_send)
{
	struct timeval	tv_diff;

	tv_diff.tv_sec = tv_recv.tv_sec - tv_send.tv_sec;
	tv_diff.tv_usec = tv_recv.tv_usec - tv_send.tv_usec;
	return (
		(double)tv_diff.tv_sec * 1000.
		+ (double)tv_diff.tv_usec / 1000.);
}

int	is_finish(t_ping *ping)
{
	struct timeval	now;

	if (ping->end)
		return (1);
	if (ping->count && ping->count == ping->packets_sent
		&& ping->packets_sent == ping->packets_rcvd + ping->error)
		return (3);
	gettimeofday(&now, NULL);
	if (ping->deadline.tv_sec != 0
		&& (now.tv_sec > ping->deadline_time.tv_sec
			|| (now.tv_sec == ping->deadline_time.tv_sec
				&& now.tv_usec >= ping->deadline_time.tv_usec)))
		return (2);
	return (0);
}
