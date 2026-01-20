/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugo <hugo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:34:49 by hugo              #+#    #+#             */
/*   Updated: 2026/01/20 13:36:18 by hugo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/woody.h"

void	encrypt(uint8_t *buffer, size_t len, t_key *key)
{
	size_t i = 0;
	size_t j = 0;

	while(i < len)
	{
		buffer[i] ^= key->key[j];
		i++;
		j = (j + 1) % key->key_size;
	}

}