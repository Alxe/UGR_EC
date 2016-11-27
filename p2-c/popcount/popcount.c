#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define WSIZE 8*sizeof(unsigned)

/*
 *	Calcula el popcount de un número con un bucle `for`
 */
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

/*
 *	Calcula el popcount de un número con un bucle `while`
 */
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

/*
 *	Calcula el popcount de un número con un bucle `while` en Assembly
 */
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

/*
 *	Calcula el popcount de un número con un bucle `for` en C y movimientos bitwise.
 */
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

/*
 * 	Calcula el popcount con instrucciones SSSE3
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

unsigned crono_f(unsigned (*func)(), unsigned arr[], int arrl) {
	struct timeval t1, t2;

	gettimeofday(&t1, 0);
	func(arr, arrl);
	gettimeofday(&t2, 0);

	return
		(t2.tv_sec - t1.tv_sec) * 1e6 + 
		(t2.tv_usec - t1.tv_usec);
}

int main()
{
	unsigned long const listl = (1 << 20);

	unsigned lista[listl];
	for(unsigned i = 0; i < listl; ++i) {
		lista[i] = i;
	}

	printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n", "f1", "f2", "f3", "f4", "f5", "i");

	for(unsigned i = 0; i < 10; ++i) {
		printf("%d\t", crono_f(f1, lista, listl));
		printf("%d\t", crono_f(f2, lista, listl));
		printf("%d\t", crono_f(f3, lista, listl));
		printf("%d\t", crono_f(f4, lista, listl));
		printf("%d\t", crono_f(f5, lista, listl));
		printf("%d\n", (i+1));
	}
	
	return 0;
}
