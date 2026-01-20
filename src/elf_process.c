/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_process.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugo <hugo@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 18:54:58 by hugo              #+#    #+#             */
/*   Updated: 2026/01/20 18:48:05 by hugo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/woody.h"


unsigned char payload[] = {
  0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52,
  0x41, 0x53, 0xb8, 0x01, 0x00, 0x00, 0x00, 0xbf, 0x01, 0x00, 0x00, 0x00,
  0x48, 0x8d, 0x35, 0x70, 0x00, 0x00, 0x00, 0xba, 0x0e, 0x00, 0x00, 0x00,
  0x0f, 0x05, 0x48, 0x8d, 0x1d, 0xd3, 0xff, 0xff, 0xff, 0x48, 0x8b, 0x05,
  0x71, 0x00, 0x00, 0x00, 0x48, 0x01, 0xd8, 0x48, 0x8b, 0x0d, 0x6f, 0x00,
  0x00, 0x00, 0x4c, 0x8d, 0x05, 0x78, 0x00, 0x00, 0x00, 0x4c, 0x8b, 0x0d,
  0x69, 0x00, 0x00, 0x00, 0x4d, 0x31, 0xd2, 0x4d, 0x31, 0xdb, 0x49, 0x39,
  0xca, 0x73, 0x18, 0x43, 0x8a, 0x14, 0x18, 0x42, 0x30, 0x14, 0x10, 0x49,
  0xff, 0xc2, 0x49, 0xff, 0xc3, 0x4d, 0x39, 0xcb, 0x72, 0x03, 0x4d, 0x31,
  0xdb, 0xeb, 0xe3, 0x48, 0x8d, 0x1d, 0x8a, 0xff, 0xff, 0xff, 0x48, 0x03,
  0x1d, 0x20, 0x00, 0x00, 0x00, 0x41, 0x5b, 0x41, 0x5a, 0x41, 0x59, 0x41,
  0x58, 0x5e, 0x5f, 0x5a, 0x59, 0x58, 0x48, 0x87, 0x1c, 0x24, 0xc3, 0x2e,
  0x2e, 0x2e, 0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e, 0x2e, 0x2e, 0x2e,
  0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00
};

int process_elf(void *mapped, Elf64_Off file_size)
{
    t_woody ctx = {
        .mapped_file       = mapped,
        .file_size         = file_size,
        .text_section      = NULL,
        .last_load_segment = NULL,
        .buffer            = NULL,
        .buffer_size       = 0,
        .key               = (t_key){{0}, 0}
    };

    /*******************************/
    /* 1. Vérification de l'ELF    */
    /*******************************/
    if (check_elf(&ctx) != 0)
    {
        munmap(mapped, file_size);
        return 1;
    }

    printf(YELLOW "ELF file is valid. Ready for further processing.\n" RESET);

    Elf64_Ehdr *src_eh = (Elf64_Ehdr *)ctx.mapped_file;

    if (ctx.file_size < src_eh->e_phoff + src_eh->e_phnum * sizeof(Elf64_Phdr)) {
        fprintf(stderr, RED "Error: file too small for program headers\n" RESET);
        munmap(mapped, file_size);
        return 1;
    }

    /*******************************/
    /* 2. Debug Program Headers    */
    /*******************************/
    // printf("\n\n\n\n\n" CYAN BOLD "Program Headers:\n\n" RESET);

    // Elf64_Phdr *src_phdr =
    //     (Elf64_Phdr *)((uint8_t *)ctx.mapped_file + src_eh->e_phoff);

    // for (int i = 0; i < src_eh->e_phnum; i++) {
    //     if (src_phdr[i].p_type == PT_LOAD ) {
    //         printf(GREEN "Found LOAD segment %d" RESET
    //                ": offset 0x%lx, vaddr 0x%lx, filesz 0x%lx, flags %c%c%c\n",
    //                i,
    //                (unsigned long)src_phdr[i].p_offset,
    //                (unsigned long)src_phdr[i].p_vaddr,
    //                (unsigned long)src_phdr[i].p_filesz,
    //                src_phdr[i].p_flags & PF_R ? 'R' : '-',
    //                src_phdr[i].p_flags & PF_W ? 'W' : '-',
    //                src_phdr[i].p_flags & PF_X ? 'X' : '-');
    //     } else {
    //         printf(BLUE "Segment %d" RESET ": type 0x%x\n",
    //                i, src_phdr[i].p_type);
    //     }
    // }

    /*******************************/
    /* 3. Debug Section Headers    */
    /*******************************/
    printf("\n\n\n\n\n" CYAN BOLD "Section Headers:\n\n" RESET);

    if (ctx.file_size < src_eh->e_shoff + src_eh->e_shnum * sizeof(Elf64_Shdr)) {
        fprintf(stderr, RED "Error: file too small for section headers\n" RESET);
        munmap(mapped, file_size);
        return 1;
    }

    Elf64_Shdr *src_sh_table =
        (Elf64_Shdr *)((uint8_t *)ctx.mapped_file + src_eh->e_shoff);

    // for (int i = 0; i < src_eh->e_shnum; i++) {
    //     printf(GREEN "Section %d" RESET ": offset 0x%lx, size 0x%lx, type 0x%x\n",
    //            i,
    //            (unsigned long)src_sh_table[i].sh_offset,
    //            (unsigned long)src_sh_table[i].sh_size,
    //            src_sh_table[i].sh_type);
    // }

    Elf64_Shdr *src_sh_strtab = &src_sh_table[src_eh->e_shstrndx];
    const char *src_sh_strtab_p =
        (const char *)ctx.mapped_file + src_sh_strtab->sh_offset;

    for (int i = 0; i < src_eh->e_shnum; i++) {
        const char *section_name = src_sh_strtab_p + src_sh_table[i].sh_name;
        if (strcmp(section_name, ".text") == 0) {
            printf(YELLOW ".text section found at offset 0x%lx, size 0x%lx\n" RESET,
                   (unsigned long)src_sh_table[i].sh_offset,
                   (unsigned long)src_sh_table[i].sh_size);
            ctx.text_section = &src_sh_table[i];
            break;
        }
    }

	Elf64_Shdr *text = ctx.text_section;
	
	Elf64_Off	text_off = text->sh_offset;
	Elf64_Addr	text_vaddr = text->sh_addr;
	size_t		text_size = text->sh_size;


    /*******************************/
    /* 4. Création de la copie     */
    /*******************************/
    ctx.buffer_size = ctx.file_size;
    ctx.buffer = malloc(ctx.buffer_size);
    if (!ctx.buffer) {
        fprintf(stderr, RED "Error: memory allocation failed\n" RESET);
        munmap(mapped, file_size);
        return 1;
    }
    memcpy(ctx.buffer, mapped, ctx.file_size);

    Elf64_Ehdr *eh   = (Elf64_Ehdr *)ctx.buffer;
    Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)ctx.buffer + eh->e_phoff);

    /*******************************/
    /* 5. Préparation de la clé    */
    /*******************************/

	/**********************************************/
	/* 5bis. Rendre le segment .text writable     */
	/**********************************************/

	Elf64_Phdr *text_segment = NULL;

	for (int i = 0; i < eh->e_phnum; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		Elf64_Addr seg_start = phdr[i].p_vaddr;
		Elf64_Addr seg_end   = phdr[i].p_vaddr + phdr[i].p_memsz;

		if (text_vaddr >= seg_start && text_vaddr < seg_end) {
			text_segment = &phdr[i];
			break;
		}
	}

	if (!text_segment) {
		fprintf(stderr, RED "Error: could not find PT_LOAD segment for .text\n" RESET);
		free(ctx.buffer);
		munmap(mapped, file_size);
		return 1;
	}

	printf(YELLOW ".text is in PT_LOAD segment with vaddr 0x%lx, flags %c%c%c\n" RESET,
		(unsigned long)text_segment->p_vaddr,
		text_segment->p_flags & PF_R ? 'R' : '-',
		text_segment->p_flags & PF_W ? 'W' : '-',
		text_segment->p_flags & PF_X ? 'X' : '-');

	/* On rend ce segment writable (RWX) pour pouvoir déchiffrer .text */
	text_segment->p_flags |= PF_W;

	printf(YELLOW "Updated .text segment flags to %c%c%c\n" RESET,
		text_segment->p_flags & PF_R ? 'R' : '-',
		text_segment->p_flags & PF_W ? 'W' : '-',
		text_segment->p_flags & PF_X ? 'X' : '-');


    t_key key = {
        .key = {0x42, 0x37, 0x13, 0xA5, 0x5C, 0x9E, 0xFF, 0x00},
        .key_size = 8
    };
    ctx.key = key;

	uint8_t *text_ptr = ctx.buffer + text_off;

	encrypt(text_ptr, text_size, &ctx.key);

    /*******************************/
    /* 6. Append du stub (PAYLOAD) */
    /*******************************/
    
    size_t payload_size = sizeof(payload);

    Elf64_Off stub_file_off = ctx.buffer_size;

    uint8_t *new_buffer = realloc(ctx.buffer, ctx.buffer_size + payload_size);
    if (!new_buffer) {
        fprintf(stderr, RED "Error: memory reallocation failed\n" RESET);
        free(ctx.buffer);
        munmap(mapped, file_size);
        return 1;
    }
    ctx.buffer = new_buffer;

    memcpy(ctx.buffer + ctx.buffer_size, payload, payload_size);
    ctx.buffer_size += payload_size;

    eh   = (Elf64_Ehdr *)ctx.buffer;
    phdr = (Elf64_Phdr *)((uint8_t *)ctx.buffer + eh->e_phoff);

    /**********************************************/
    /* 7. Trouver le dernier segment PT_LOAD      */
    /**********************************************/
    int last_load_index = -1;
    Elf64_Off max_end = 0;

    for (int i = 0; i < eh->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            Elf64_Off segment_end = phdr[i].p_offset + phdr[i].p_filesz;
            if (segment_end > max_end) {
                max_end = segment_end;
                last_load_index = i;
            }
        }
    }
    if (last_load_index == -1) {
        fprintf(stderr, RED "Error: no PT_LOAD segment found\n" RESET);
        free(ctx.buffer);
        munmap(mapped, file_size);
        return 1;
    }

    ctx.last_load_segment = &phdr[last_load_index];
    printf(YELLOW "Last PT_LOAD segment index: %d\n" RESET, last_load_index);

    /**********************************************/
    /* 8. Calcul de stub_vaddr + delta            */
    /**********************************************/

	printf(YELLOW "Calculating stub virtual address...\n" RESET);
    Elf64_Off  delta_in_file = stub_file_off - ctx.last_load_segment->p_offset;
    Elf64_Addr stub_vaddr    = ctx.last_load_segment->p_vaddr + delta_in_file;

    Elf64_Addr orig_entry = eh->e_entry;

    // entry_delta = orig_entry - stub_vaddr (signé)
    Elf64_Sxword entry_delta =
        (Elf64_Sxword)orig_entry - (Elf64_Sxword)stub_vaddr;
	printf(YELLOW "Entry delta calculated: 0x%lx\n" RESET,
		   (unsigned long)entry_delta);

    /**********************************************/
    /* 9. Patch de entry_delta dans le stub       */
    /**********************************************/


	printf(YELLOW "Patching stub: setting entry delta to 0x%lx\n" RESET,
		(unsigned long)entry_delta);

	Elf64_Sxword *entry_delta_ptr =
		(Elf64_Sxword *)(ctx.buffer + stub_file_off + STUB_ENTRY_DELTA_OFFSET);
	*entry_delta_ptr = entry_delta;

	Elf64_Sxword text_delta =
		(Elf64_Sxword)text_vaddr - (Elf64_Sxword)stub_vaddr;

	Elf64_Sxword *text_delta_ptr =
		(Elf64_Sxword *)(ctx.buffer + stub_file_off + STUB_TEXT_DELTA_OFFSET);
	*text_delta_ptr = text_delta;

	// text_size
	Elf64_Xword *text_size_ptr =
		(Elf64_Xword *)(ctx.buffer + stub_file_off + STUB_TEXT_SIZE_OFFSET);
	*text_size_ptr = text_size;

	// key_size
	Elf64_Xword *key_size_ptr =
		(Elf64_Xword *)(ctx.buffer + stub_file_off + STUB_KEY_SIZE_OFFSET);
	*key_size_ptr = ctx.key.key_size;

	// key_data
	uint8_t *key_data_ptr =
		(uint8_t *)(ctx.buffer + stub_file_off + STUB_KEY_DATA_OFFSET);
	memcpy(key_data_ptr, ctx.key.key, ctx.key.key_size);
    /**********************************************/
    /* 10. Étendre le segment pour couvrir le stub*/
    /**********************************************/
    Elf64_Off new_end = stub_file_off + payload_size;
    Elf64_Off old_end =
        ctx.last_load_segment->p_offset + ctx.last_load_segment->p_filesz;

    if (new_end > old_end) {
        ctx.last_load_segment->p_filesz += (new_end - old_end);
        ctx.last_load_segment->p_memsz += (new_end - old_end);
    }

    ctx.last_load_segment->p_flags |= PF_X; // RWX

    eh->e_entry = stub_vaddr;

    /**********************************************/
    /* 11. Debug des changements                  */
    /**********************************************/

    printf("\n\n\n\n\n" CYAN BOLD "Modifications Summary:\n\n" RESET);
    printf(YELLOW "Original entry point : 0x%lx\n" RESET, (unsigned long)orig_entry);
    printf(YELLOW "New entry point set  : 0x%lx\n" RESET, (unsigned long)eh->e_entry);
    printf(YELLOW "Stub virtual address : 0x%lx\n" RESET, (unsigned long)stub_vaddr);
    printf(YELLOW "Stub file offset     : 0x%lx\n" RESET, (unsigned long)stub_file_off);
    printf(YELLOW "Updated last LOAD segment: filesz=0x%lx, memsz=0x%lx, flags=%c%c%c\n" RESET,
           (unsigned long)ctx.last_load_segment->p_filesz,
           (unsigned long)ctx.last_load_segment->p_memsz,
           ctx.last_load_segment->p_flags & PF_R ? 'R' : '-',
           ctx.last_load_segment->p_flags & PF_W ? 'W' : '-',
           ctx.last_load_segment->p_flags & PF_X ? 'X' : '-');

    /**********************************************/
    /* 12. Écriture du fichier final "woody"      */
    /**********************************************/
    int out_fd = open("woody", O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (out_fd < 0) {
        fprintf(stderr, RED "Error: cannot create output file\n" RESET);
        free(ctx.buffer);
        munmap(mapped, file_size);
        return 1;
    }

    if (write(out_fd, ctx.buffer, ctx.buffer_size) != (ssize_t)ctx.buffer_size) {
        fprintf(stderr, RED "Error: writing to output file failed\n" RESET);
        close(out_fd);
        free(ctx.buffer);
        munmap(mapped, file_size);
        return 1;
    }

    close(out_fd);
    free(ctx.buffer);
    munmap(mapped, file_size);

    printf(GREEN "Successfully created 'woody'.\n" RESET);

    return 0;
}
