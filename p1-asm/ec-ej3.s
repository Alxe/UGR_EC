.section .data							# Sección de los datos
lista:	
	.int	-1, -1, -1, -1

longlista:								# Longitud de la lista de datos
	.int	(.-lista)/4

media:
	.int 0

resto:
	.int 0					
	
formato:								# Cadena que usamos para imprimir el resultado
	.ascii	"Resultado:\n\t- Media:\n\t\t- Hex: %1$lx\n\t\t- Dec: %1$li\n\t- Resto:\n\t\t- Hex: %2$lx\n\t\t- Dec: %2$li\n\0"

.section .text
_start:	
	.global _start

	mov  $lista,	%ebx				# Movemos la dirección de lista a %eax
	mov  longlista,	%ecx				# Movemos el valor de longlista a %ebx
	call suma 							# Llamamos al procedimiento `suma`	

	mov  %eax,	media
	mov  %edx,	resto

	push resto
	push media
	push $formato						# Acabamos por introducir el formato que pasaremos a printf

	call printf							# Llamamos al procedimiento `printf`
	add  $12,	%ebp					# Truquito para evitar hacer algún que otro `pop`

	mov $1,		%eax					# Proceso de salida de programa
	mov $0,		%ebx
	int $0x80

# Procedimiento de suma
# Entrada:
#	%ebx = Datos que sumar
#	%ecx = Número de datos
# Salida:
#	%eax = Media
#	%edx = Resto
suma:
_suma_begin:
	push %ebp							# Guardamos los valores en la pila
	push %esi
	push %edi

	mov $0,		%ebp					# Iniciamos %ebp a 0
	mov $0,		%esi					# Iniciamos %esi a 0 
	mov $0,		%edi					# Iniciamos %edi a 0

_suma_loop:
	mov (%ebx,%ebp,4), %eax				# Seleccionar elemento con índice i
	cltd								# Extendemos el signo

	add %eax,	%esi					# Sumamos %eax a %esi
	adc %edx,	%edi					# Sumamos %edx a %edi con acarreo

	inc %ebp							# Incrementamos el índice
	cmp %ebp,	%ecx					# Comparamos %ebp con %ecx
	jne _suma_loop						# Si no son iguales, re-entramos en el bucle

_suma_end:
	mov %esi,	%eax
	mov %edi,	%edx
	idiv %ecx			

	pop %edi							# Devolvemos los valores de la pila
	pop %esi
	pop %ebp							
	
	ret				
