.section .data
lista:		.int 1,2,10,  1,2,0b10,  1,2,0x10
longlista:	.int (.-lista)/4
resultado:	.quad -1
cadena:		.ascii "Valor=%llx"

.section .text
_start:	.global _start

	mov    $lista, %ebx
	mov longlista, %ecx
	call suma
	mov %edx, resultado+4
	mov %eax, resultado

	pushl resultado+4
	pushl resultado
	push $cadena

	call printf					
	add  $12,	%ebp

	mov $1, %eax
	mov $0, %ebx
	int $0x80

suma:
	push %edx
	push %ebp

	mov $0, %ebp
	mov $0, %eax
	mov $0, %edx
bucle:
	add (%ebx,%ebp,4), %eax
	adc $0, 	%edx
	inc       %ebp
	cmp  %ebp,%ecx
	jne bucle

	pop %ebp
	pop %edx
	ret
