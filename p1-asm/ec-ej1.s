.section .data							# Sección de los datos
lista:	
	.int	0 # Añadir más datos: a, b, c, d, ...

longlista:								# Longitud de la lista de datos
	.int	(.-lista)/4

resultado:								# Resultado de la suma
	.quad	0					
	
formato:								# Cadena que usamos para imprimir el resultado
	.ascii	"Resultado:\n\t- Hex: %1$llx\n\t- Dec: %1$llu\n\0"

.section .text
_start:	
	.global _start

	mov  $lista,	%ebx				# Movemos la dirección de lista a %ebx
	mov  longlista,	%ecx				# Movemos el valor de longlista a %ecx
	call suma 							# Llamamos al procedimiento `suma`	

	mov  %edx,	resultado+4			
	mov  %eax,	resultado

	pushl resultado+4 					# Primero introducimos los bits más significativos de resultado
	pushl resultado 					# Después los menos representativos
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
#	%eax = Bits menos representativos (quad word)
#	%edx = Bits más representativos (quad word)
suma:
_suma_begin:
	push %ebp							# Guardamos el valor de %ebp en la pila.

	mov $0,		%ebp					# Iniciamos %ebp a 0
	mov $0,		%eax					# Iniciamos %eax a 0
	mov $0,		%edx					# Iniciamos %edx a 0

_suma_loop:
	add (%ebx, %ebp, 4),	%eax		# Sumamos (%ebx + (%ebp * 4)) y lo almacenamos en %eax
	adc	$0,					%edx		# Incrementamos %edx con el acarreo, si es que hay.
	
	inc %ebp 							# Incrementamos el contador de %ebp
	cmp %ebp,	%ecx 					# Comparamos %ebp y %ecx
	jne _suma_loop 						# En el caso de que no sean iguales, re-entramos en el bucle

_suma_end:
	pop %ebp							# Restauramos %ebp de la pila
	
	ret 								# Retornamos al punto de llamada
