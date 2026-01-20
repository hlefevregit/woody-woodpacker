; input.asm
bits 64
global woody_stub

section .text

woody_stub:
    ; Sauver tous les registres qui seront modifiés
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11

    ; write(1, "....WOODY....\n", len)
    mov eax, 1                     	; sys_write
    mov edi, 1                     	; fd = 1 (stdout)
    lea rsi, [rel woody_msg]       	; buffer
    mov edx, woody_msg_end - woody_msg
    syscall

	; ------------------------------
    ; Déchiffrement de .text
    ; ------------------------------

	; base = adresse de .text chiffrée = B + text_vaddr
	lea rbx, [rel woody_stub]		; rbx = B + woody_stub_vaddr

	; addr_text = base + text_delta
	mov rax, [rel text_delta]		; rax = text_delta
	add rax, rbx					; rax = adresse runtime de .text chiffrée = B + text_vaddr

	; len = text_size
	mov rcx, [rel text_size]		; rcx = taille de .text = len

	; key = adresse de la clé
	lea r8, [rel key_data]			; r8 = adresse de la clé = &key[0]

	; key_size
	mov r9, [rel key_size]			; r9 = taille de la clé = key_size

	xor r10, r10					; r10 = i = 0
	xor r11, r11					; r11 = j = 0

.decrypt_loop:

	cmp r10, rcx					; i >= len ?
	jae .decrypt_done				; si oui, on a fini


	; load key[j]
	mov dl, [r8 + r11]				; dl = key[j]
	; xor byte ptr [addr_text + i] with key[j] (dl)
	xor byte [rax + r10], dl

	inc r10							; i++
	inc r11							; j++
	cmp r11, r9						; j >= key_size ?
	jb .noreset_j					; si non, pas de reset j
	xor r11, r11					; si oui, j = 0

.noreset_j:
	jmp .decrypt_loop

.decrypt_done:

	; ------------------------------
    ; Restaurer les registres et jump vers l'ancien entry point
    ; ------------------------------

    ; Calculer l'adresse de l'ancien entry point avant de restaurer rax
    lea rbx, [rel woody_stub]
    add rbx, [rel entry_delta]     ; rbx = B + orig_entry

    ; Restaurer tous les registres (sauf rbx qui contient l'adresse de saut)
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rax
    ; rbx est restauré mais ensuite écrasé avec l'adresse de saut
    
    ; Échanger rbx avec la valeur sauvegardée sur la pile et sauter
    xchg rbx, [rsp]
    ret                            ; pop l'adresse et sauter (équivalent à pop rip)

woody_msg:
    db '....WOODY....', 0x0a
woody_msg_end:

entry_delta:
    dq 0        ; sera patché par le packer (C)

text_delta:
	dq 0

text_size:
	dq 0

key_size:
	dq 0

key_data:
	; espace réservé pour la clé (patchée par le packer)
	times 64 db 0