
align 16

global inportsw
inportsw:
	push edi
	mov edx,[esp+8]
	mov edi,[esp+12]
	mov ecx,[esp+16]

	xor eax,eax
.insw_startloop:
	cmp eax,ecx
	je .insw_end

	insw

	inc eax
	jmp .insw_startloop

.insw_end:
	pop edi
ret


align 16
global outportsw
outportsw:
	push esi
	mov edx,[esp+8]
	mov esi,[esp+12]
	mov ecx,[esp+16]
	
	xor eax,eax
.outsw_startloop:
	cmp eax,ecx
	je .outsw_end

	outsw

	inc eax
	jmp .outsw_startloop
.outsw_end:
	pop esi
ret
