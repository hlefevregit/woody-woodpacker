/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugo <hugo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 17:55:39 by hugo              #+#    #+#             */
/*   Updated: 2026/01/19 18:56:19 by hugo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/woody.h"

int check_elf(t_woody *ctx)
{
	Elf64_Ehdr *elf_header;

	elf_header = ctx->mapped_file;
	if (elf_header->e_ident[EI_MAG0] != ELFMAG0 ||
		elf_header->e_ident[EI_MAG1] != ELFMAG1 ||
		elf_header->e_ident[EI_MAG2] != ELFMAG2 ||
		elf_header->e_ident[EI_MAG3] != ELFMAG3)
	{
		fprintf(stderr, RED "Error: Not a valid ELF file\n" RESET);
		return 1;
	}
	if (elf_header->e_ident[EI_CLASS] != ELFCLASS64)
	{
		fprintf(stderr, RED "Error: Not a 64-bit ELF file\n" RESET);
		return 1;
	}
	if (elf_header->e_type != ET_EXEC && elf_header->e_type != ET_DYN)
	{
		fprintf(stderr, RED "Error: Not an executable ELF file\n" RESET);
		return 1;
	}
	
	return 0;
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		printf(YELLOW "Usage: %s <input_file>\n" RESET, av[0]);
		return 1;
	}

	int fd = open(av[1], O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return 1;
	}

	off_t file_size = lseek(fd, 0, SEEK_END);
	if (file_size == (off_t)-1)
	{
		perror("lseek");
		close(fd);
		return 1;
	}
	lseek(fd, 0, SEEK_SET);
	void *mapped = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (mapped == MAP_FAILED)
	{
		perror("mmap");
		close(fd);
		return 1;
	}
	close(fd);


	process_elf(mapped, file_size);

	munmap(mapped, file_size);
	

	return 0;
}