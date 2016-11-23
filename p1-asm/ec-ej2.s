.section .data							# Sección de los datos
lista:	
	.int	0x0a, 0x05, -0x05, -0x10, 0x10

longlista:								# Longitud de la lista de datos
	.int	(.-lista)/4

resultado:								# Resultado de la suma
	.quad	0					
	
formato:								# Cadena que usamos para imprimir el resultado
	.ascii	"Resultado:\n\t- Hex: %1$llx\n\t- Dec: %1$lli\n\0"

.section .text
_start:	
	.global _start

	mov  $lista,	%ebx				# Movemos la dirección de lista a %eax
	mov  longlista,	%ecx				# Movemos el valor de longlista a %ebx
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

# Entrada:
#	%ebx = Datos que sumar
#	%ecx = Número de datos
# Salida:
#	%eax = Bits menos representativos (quad word)
#	%edx = Bits más representativos (quad word)
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

	pop %edi
	pop %esi
	pop %ebp							# Devolvemos el valor de %ebp de la pila
	
	ret				
