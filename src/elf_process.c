/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_process.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugo <hugo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 18:54:58 by hugo              #+#    #+#             */
/*   Updated: 2026/01/19 18:58:13 by hugo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/woody.h"

int process_elf(void *mapped, Elf64_Off file_size)
{
	t_woody ctx = {mapped, file_size, 0, 0, NULL};
	
	if (check_elf(&ctx) != 0)
	{
		munmap(mapped, file_size);
		return 1;
	}

	printf(YELLOW "ELF file is valid. Ready for further processing.\n" RESET);

	printf("\n\n\n\n\n" CYAN BOLD "Program Headers:\n\n" RESET);

	Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)ctx.mapped_file + ctx.mapped_file->e_phoff);
	
	for (int i = 0; i < ctx.mapped_file->e_phnum; i++) {
		if (phdr[i].p_type == PT_LOAD ) {
			printf(GREEN "Found LOAD segment %d" RESET ": offset 0x%lx, vaddr 0x%lx, filesz 0x%lx, flags %c%c%c\n",
				   i, phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_filesz, phdr[i].p_flags & PF_R ? 'R' : '-', phdr[i].p_flags & PF_W ? 'W' : '-', phdr[i].p_flags & PF_X ? 'X' : '-');
		} else {
			printf(BLUE "Segment %d" RESET ": type 0x%x\n", i, phdr[i].p_type);
		}
	}

	Elf64_Ehdr *eh = ctx.mapped_file;

	if (ctx.file_size < eh->e_phoff + eh->e_phnum * sizeof(Elf64_Phdr)) {
    fprintf(stderr, RED "Error: file too small for program headers\n" RESET);
    munmap(mapped, file_size);
    return 1;
}

	printf("\n\n\n\n\n" CYAN BOLD "Section Headers:\n\n" RESET);

	if (ctx.file_size < eh->e_shoff + eh->e_shnum * sizeof(Elf64_Shdr)) {
		fprintf(stderr, RED "Error: file too small for section headers\n" RESET);
		return 1;
	}

	Elf64_Shdr *sh_table = (Elf64_Shdr *)((uint8_t *)ctx.mapped_file + eh->e_shoff);
	for (int i = 0; i < eh->e_shnum; i++) {
		printf(GREEN "Section %d" RESET ": offset 0x%lx, size 0x%lx, type 0x%x\n",
			   i, sh_table[i].sh_offset, sh_table[i].sh_size, sh_table[i].sh_type);
	}

	Elf64_Shdr *sh_strtab = &sh_table[eh->e_shstrndx];
	const char *sh_strtab_p = (const char *)ctx.mapped_file + sh_strtab->sh_offset;

	for (int i = 0; i < eh->e_shnum; i++) {
		const char *section_name = sh_strtab_p + sh_table[i].sh_name;
		if (strcmp(section_name, ".text") == 0) {
			printf(YELLOW ".text section found at offset 0x%lx, size 0x%lx\n" RESET,
				   sh_table[i].sh_offset, sh_table[i].sh_size);
			ctx.text_section = &sh_table[i];
			break;
		}
	}
	return 0;
}