/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugo <hugo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 17:56:02 by hugo              #+#    #+#             */
/*   Updated: 2026/01/19 18:56:56 by hugo             ###   ########.fr       */
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


typedef struct s_woody
{
	Elf64_Ehdr  *mapped_file;
	Elf64_Off   file_size;
	Elf64_Off   section_offset;
	Elf64_Off   shellcode_size;
	Elf64_Shdr  *text_section;
	

} t_woody;


// ELF processing functions
int		check_elf(t_woody *ctx);
int		process_elf(void *mapped, Elf64_Off file_size);



void	inject_shellcode(t_woody *ctx);



#endif