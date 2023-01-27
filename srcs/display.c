/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aartiges <aartiges@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/27 05:58:12 by aartiges          #+#    #+#             */
/*   Updated: 2023/01/27 06:03:01 by aartiges         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/includes.h"

// struct sockaddr {
// 	u_short sa_family;
// 	char	sa_data[14];
// };

void	display_sockaddr(struct sockaddr *sockaddr)
{
	u_int8_t	i;

	printf("Display sockaddr %p\n", sockaddr);
	if (!sockaddr)
		return ;
	printf("sa_family\t= %d\n", sockaddr->sa_family);
	printf("sa_data\t= ");
	i = 0;
	while (i < 16)
		printf("%u ", (unsigned char)sockaddr->sa_data[i++]);
	printf("\n");
	printf("sa_len\t= %d\n", sockaddr->sa_len);
}

/*
struct addrinfo {
		int ai_flags;				// input flags
		int ai_family;				// protocol family for socket
		int ai_socktype;			// socket type
		int ai_protocol;			// protocol for socket
		socklen_t ai_addrlen;		// length of socket-address
		struct sockaddr *ai_addr;	// socket-address for socket
		char *ai_canonname;			// canonical name for service location
		struct addrinfo *ai_next;	// pointer to next in list
};
*/

void	display_addrinfo(struct addrinfo *addrinfo)
{
	const int	flags[] = {AI_ADDRCONFIG, AI_ALL, AI_CANONNAME, AI_NUMERICHOST, AI_NUMERICSERV, AI_PASSIVE, AI_V4MAPPED, AI_V4MAPPED_CFG, AI_DEFAULT, AI_UNUSABLE};
	const char	*flagsstr[] = {"AI_ADDRCONFIG", "AI_ALL", "AI_CANONNAME", "AI_NUMERICHOST", "AI_NUMERICSERV", "AI_PASSIVE", "AI_V4MAPPED", "AI_V4MAPPED_CFG", "AI_DEFAULT", "AI_UNUSABLE"};
	u_int8_t	fst = 0;
	u_int8_t	i = 0;

	fst = 0;
	i = 0;
	printf("Display addrinfo %p\n", addrinfo);
	if (!addrinfo)
		return ;
	printf("ai_flags\t= ");
	while (i < 10)
	{
		if (addrinfo->ai_flags & flags[i])
		{
			if (fst)
				printf(" | ");
			printf("%s", flagsstr[i++]);
			fst = 1;
		}
	}
	if (!fst)
		printf("AI_DEFAULT");
	printf("\n");
	printf("ai_family\t= %s\n", addrinfo->ai_family == AF_INET ? "AF_INET" : addrinfo->ai_family == PF_UNSPEC ? "PF_UNSPEC [ANY_PROTOCOL_SUPPORT]" : "??");
	printf("ai_socktype\t= %s\n", addrinfo->ai_socktype == SOCK_STREAM ? "SOCK_STREAM" : (addrinfo->ai_socktype == SOCK_DGRAM ? "SOCK_DGRAM" : (addrinfo->ai_socktype == SOCK_RAW ? "SOCK_RAW": "OTHER_SOCKET")));
	printf("ai_protocol\t= %s\n", addrinfo->ai_protocol == IPPROTO_UDP ? "IPPROTO_UDP" : (addrinfo->ai_protocol == IPPROTO_TCP ? "IPPROTO_TCP" : (addrinfo->ai_protocol == IPPROTO_ICMP ? "IPPROTO_ICMP" : "IPPROTO_OTHER")));
	printf("ai_addrlen\t= %u\n", addrinfo->ai_addrlen);
	printf("ai_addr = [struct sockaddr]\n");
	display_sockaddr(addrinfo->ai_addr);
	printf("ai_canonname\t = %s\nai_next\t= [struct addrinfo]", addrinfo->ai_canonname);
	display_addrinfo(addrinfo->ai_next);
}
