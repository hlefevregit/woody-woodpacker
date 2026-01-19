/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   injection.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugo <hugo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 18:07:58 by hugo              #+#    #+#             */
/*   Updated: 2026/01/19 18:11:58 by hugo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/woody.h"

void	inject_shellcode(t_woody *ctx)
{
	char payload[] = 
		"\xeb\x14\x48\x31\xc0\x48\x31\xff"
		"\x48\x31\xd2\xb0\x01\x40\xb7\x01"
		"\x5e\xb2\x0d\x0f\x05\xc3\xe8\xe7"
		"\xff\xff\xff\x2e\x2e\x2e\x2e\x57"
		"\x4f\x4f\x44\x59\x2e\x2e\x2e\x2e";

	ctx->shellcode_size = sizeof(payload) - 1; // exclude null terminator
	printf("Shellcode of size %ld bytes prepared for injection.\n", ctx->shellcode_size);

	
}