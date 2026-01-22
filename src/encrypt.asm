; encrypt.asm

global encrypt

section .text

encrypt:
	; encrypt(uint8_t *data, size_t len, t_key *key)
	; Arguments: data = rdi, len = rsi, key = rdx

	; Charger key_size depuis [rdx + 0x40]
	mov rcx, [rdx + 0x40]		; rcx = key_size
	xor r8, r8					; r8 = i = 0
	xor r9, r9					; r9 = j = 0

	.encrypt_loop:
		cmp r8, rsi				; i >= len ?
		jae .encrypt_done		; si oui, on a fini

		; load key[i]
		mov r11b, [rdx + r9]	; r11b = key[j]
		xor [rdi + r8], r11b	; xor byte ptr [data + i] with key[j]

		inc r8					; i++
		inc r9					; j++

		cmp r9, rcx				; j >= key_size ?
		jl .encrypt_loop		; si non, continuer
		
		xor r9, r9				; j = 0
		jmp .encrypt_loop

	.encrypt_done:
		ret

