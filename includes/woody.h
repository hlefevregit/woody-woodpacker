/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugo <hugo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 17:56:02 by hugo              #+#    #+#             */
/*   Updated: 2026/01/20 18:50:03 by hugo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
# define WOODY_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <stdint.h>
# include <elf.h>
# include "colors.h"

# define STUB_ENTRY_DELTA_OFFSET	0x9d
# define STUB_TEXT_DELTA_OFFSET		0xa5
# define STUB_TEXT_SIZE_OFFSET		0xad
# define STUB_KEY_SIZE_OFFSET		0xb5
# define STUB_KEY_DATA_OFFSET		0xbd

typedef struct s_key
{
	uint8_t		key[64];
	size_t		key_size;
}				t_key;


typedef struct s_woody
{
	Elf64_Ehdr  *mapped_file;
	Elf64_Off   file_size;
	Elf64_Off   section_offset;
	Elf64_Off   shellcode_size;
	Elf64_Shdr  *text_section;
	Elf64_Phdr  *last_load_segment;

	uint8_t		*buffer;
	size_t		buffer_size;
	
	t_key		key;

} t_woody;

// ELF processing functions
int		check_elf(t_woody *ctx);
int		process_elf(void *mapped, Elf64_Off file_size);

void	encrypt(uint8_t *buffer, size_t len, t_key *key);


#endif