
extrn ?OnFunctionEnter@@YAXPEAX0@Z : proc
extrn ?OnFunctionExit@@YAXPEAX@Z : proc


.code

_penter proc

	push rax
	push rbx
	push rcx
	push rdx

	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	;save registers here
	sub rsp,28h
	
	;ret addr = [sp] - 21.
	mov rcx, [rsp+98h]
	sub rcx, 21h

	;bp
	;mov rdx, rsp
	;add rdx, 98h
	mov rdx, 0h

	call   ?OnFunctionEnter@@YAXPEAX0@Z
	
	add rsp,28h

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8

	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	
	ret

_penter endp


_pexit proc

	push rax
	push rbx
	push rcx
	push rdx

	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	;save registers here
	sub rsp,28h

	;bp
	;mov rcx, rsp
	;add rcx, 98h
	mov rcx, 0h

	call   ?OnFunctionExit@@YAXPEAX@Z

	add rsp, 28h

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8

	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax

	ret

_pexit endp

end
