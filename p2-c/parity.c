#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define WSIZE 8*sizeof(unsigned)

// unsigned listl = 4;
// unsigned lista[] = { 0x80000000, 0x00100000, 0x00000800, 0x00000001 };

unsigned listl = 8;
unsigned lista[] = { 0x7fffffff, 0xffefffff, 0xfffff7ff, 0xfffffffe,
					 0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef };

// unsigned listl = 8;
// unsigned lista[] = { 0x00000000, 0x10284880, 0x3590ac06, 0x70b0d0e0,
// 					 0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef };

unsigned resultado = 0;

/*
 *	Calcula el popcount de un número con un bucle `for`
 */
unsigned f1(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i], parity = 0;

		for(unsigned j = 0; j < WSIZE; ++j) {
			parity ^= (x & 0x1);

			x >>= 0x1;
		}

		r += (parity & 0x1);
	}

	return r;
}

/*
 *	Calcula el popcount de un número con un bucle `while`
 */
unsigned f2(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i], parity = 0;

		while(x) {
			parity ^= (x & 0x1);

			x >>= 0x1;
		}

		r += (parity & 0x1);
	}

	return r;
}

/*
 *	Calcula el popcount de un número con un bucle `while` en Assembly
 */
unsigned f3(unsigned *array, unsigned len) {
	
}

/*
 *	Calcula el popcount de un número con un bucle `for` en C y movimientos bitwise.
 */
unsigned f4(unsigned *array, unsigned len) {
	unsigned r = 0;

	for(unsigned i = 0; i < len; ++i) {
		unsigned x = array[i];
		unsigned val = 0;

		for (unsigned k = 0; k < 8; ++k) {
			val += (x & 0x01010101); // mask
			x >>= 1;
		}
		
		val += (val >> 16);
		val += (val >> 8);

		r += (val & 0xff);
	}

	return r;
}

/*
 *	Referencia: http:/wm.ite.pl/articles/sse-popcount.html
 */
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

void crono(unsigned (*func)(), char* msg) {
	struct timeval tv1,tv2;
	long           tv_usecs;

	gettimeofday(&tv1,NULL);
	resultado = func(lista, listl);
	gettimeofday(&tv2,NULL);

	tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
			 (tv2.tv_usec-tv1.tv_usec);
	printf("resultado = %d\t", resultado);
	printf("%s:%9ld us\n", msg, tv_usecs);
}

int main()
{
	crono(f1, "popcount1 (bucle for de C)");
	crono(f2, "popcount2 (bucle while de C)");
	crono(f3, "popcount3 (bucle while de ASM)");
	crono(f4, "popcount4 (bucle for de C bitwise");
	crono(f5, "popcount5 (ASM SSSE3)");

	exit(0);
}
