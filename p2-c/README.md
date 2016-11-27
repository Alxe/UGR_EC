p2-c
===

Trabajo realizado por Alejandro Núñez Pérez para la asignatura de Estructuras de Computadores de segundo año, primer cuatrimestre del Grado de Ingeniería Informática de la Universidad de Granada, impartida en la Escuela Técnica Superior de Ingeniería Informática y Telecomunicaciones por el profesor Antonio Cañas Vargas.

El trabajo consiste de varias formas de de medir el Peso Hamming (population count, popcount) y la Paridad de números de 32-bit.


Adjunto un `makefile` que genera tres archivos de texto plano que muestran diez iteraciones de las *n* funciones de cada apartado. **No** he realizado gráficas comparativas porque no obtengo valores con los que hacer comparaciones en condiciones.

popcount
---

### f1
```c
unsigned f1(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i];

		for(unsigned j = 0; j < WSIZE; ++j) {
			r += (x & 0x1);

			x >>= 0x1;
		}
	}

	return r;
}
```

### f2
```c
unsigned f2(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i];

		while(x) {
			r += (x & 0x1);

			x >>= 0x1;
		}
	}

	return r;
}
```

### f3
```c
unsigned f3(unsigned *array, unsigned len) {
	unsigned r = 0;

	for (unsigned i = 0; i < len; ++i) {
		unsigned x = array[i];

		asm(
		"f3_loop:					\n\t"
			"shr $0x1, %[x]			\n\t"   
			"adc $0x0, %[r]			\n\t"
			"test %[x], %[x]		\n\t"
			"jnz f3_loop			\n\t"
			
			: [r] "+r"	(r)          
			: [x] "r"	(x));
	}

	return r;
}
```

### f4
```c
unsigned f4(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i], v = 0;

		for (unsigned j = 0; j < 8; ++j) {
			v += (x & 0x01010101); // mask
			x >>= 0x1;
		}
		
		v += (v >> 16);
		v += (v >> 8);

		r += (v & 0xff);
	}

	return r;
}
```

### f5
```c
unsigned f5(unsigned *array, unsigned len) {
	unsigned 
		val = 0, r = 0,
		SSE_mask[] = { 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f },
		SSE_LUTb[] = { 0x02010100, 0x03020201, 0x03020201, 0x04030302 };

	if (len & 0x3)
		printf("¿len %% 4) != 0?\n");
	
	for (unsigned i = 0; i < len; i += 4) {
		asm("movdqu		%[x],	%%xmm0		\n\t"
			"movdqa 	%%xmm0,	%%xmm1		\n\t"
			"movdqu		%[m],	%%xmm6		\n\t"
			"psrlw		$4,		%%xmm1		\n\t"
			"pand		%%xmm6,	%%xmm0		\n\t"
			"pand		%%xmm6,	%%xmm1		\n\t"

			"movdqu		%[l],	%%xmm2		\n\t"
			"movdqa		%%xmm2,	%%xmm3		\n\t"
			"pshufb		%%xmm0,	%%xmm2		\n\t"
			"pshufb		%%xmm1,	%%xmm3		\n\t"

			"paddb		%%xmm2,	%%xmm3		\n\t"
			"pxor		%%xmm0,	%%xmm0		\n\t"
			"psadbw		%%xmm0,	%%xmm3		\n\t"
			"movhlps	%%xmm3,	%%xmm0		\n\t"
			"paddd		%%xmm3,	%%xmm0		\n\t"
			"movd		%%xmm0,	%[val]		\n\t"
			: 	[val]	"=r"	(val)
			: 	[x] 	"m"		(array[i]),
				[m] 	"m"		(SSE_mask[0]),
				[l] 	"m"		(SSE_LUTb[0])
		);
			
			r += val;
		}
	
	return r;
}
```

parity
---

### f1
```c
unsigned f1(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i], parity = 0;

		for(unsigned j = 0; j < WSIZE; ++j) {
			parity ^= (x & 0x1);

			x >>= 0x1;
		}

		r += parity;
	}

	return r;
}
```

### f2
```c
unsigned f2(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i], parity = 0;

		while(x) {
			parity ^= (x & 0x1);

			x >>= 0x1;
		}

		r += parity;
	}

	return r;
}
```

### f3
```c
unsigned f3(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i], v = 0;

		while(x) {
			v ^= x;

			x >>= 0x1;
		}

		r += (v & 0x1);
	}

	return r;
}
```

### f4
```c
unsigned f4(unsigned *array, unsigned len) {
	unsigned r = 0;

	for (unsigned i = 0; i < len; ++i) {
		unsigned x = array[i], v = 0;

		asm(
		"f4_loop:					\n\t"
			"xor %[x], %[v]			\n\t"
			"shr $0x1, %[x]			\n\t"   
			"test %[x], %[x]		\n\t"
			"jnz f4_loop			\n\t"
			
			: [v] "+r"	(v)          
			: [x] "r"	(x)
		);

		r += (v & 0x1);
	}

	return r;
}
```

### f5
```c
unsigned f5(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i];

		for (unsigned j = 16; (j > 0); (j /= 2)) {
			x ^= (x >> j);
		}

		r += (x & 0x01);
	}

	return r;
}
```

### f6
```c
unsigned f6(unsigned *array, unsigned len) {
	unsigned r = 0;

	for (unsigned i = 0; i < len; ++i) {
		unsigned x = array[i];
		asm(
			"mov	%[x], 	%%edx		\n\t"
			"shr	$16,	%%edx		\n\t"
			"xor	%[x],	%%edx		\n\t"
			"xor	%%dh,	%%dl		\n\t"
			"setpo  %%dl				\n\t"
			"movzx	%%dl,	%[x]		\n\t"
			: [x] "+r" (x)
			:
			: "edx"
		);

		r += x;
	}

	return r;
}
```