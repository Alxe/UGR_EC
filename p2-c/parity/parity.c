#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define WSIZE 8*sizeof(unsigned)

unsigned listl = 4;
unsigned lista[] = { 0x80000000, 0x00100000, 0x00000800, 0x00000001 };

// unsigned listl = 8;
// unsigned lista[] = { 0x7fffffff, 0xffefffff, 0xfffff7ff, 0xfffffffe,
// 					 0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef };

// unsigned listl = 8;
// unsigned lista[] = { 0x00000000, 0x10284880, 0x3590ac06, 0x70b0d0e0,
// 					 0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef };

/*
 *	Parity 1:
 *	
 */
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

/*
 *	Parity 2:
 *
 */
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

/*
 *	Parity 3:
 *
 */
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

/*
 *	Parity 4:
 *
 */
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

/*
 *	Parity 5:
 *	
 */
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

/*
 *	Parity 6:
 *
 */
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


unsigned crono_f(unsigned (*func)()) {
	struct timeval t1, t2;

	gettimeofday(&t1, 0);
	unsigned int r = func(lista, listl);
	gettimeofday(&t2, 0);

	return
		(t2.tv_sec - t1.tv_sec) * 1e6 + 
		(t2.tv_usec - t1.tv_usec);
}

int main()
{
	const int SIZE = (1 << 20);

	unsigned arr[SIZE];
	for(unsigned i = 0; i < SIZE; ++i) {
		arr[i] = i + i;
	}

	printf("%s\t%s\t%s\t%s\t%s\t%s\n", "f1", "f2", "f3", "f4", "f5", "i");

	for(unsigned i = 0; i < 10; ++i) {
		printf("%d\t", crono_f(f1));
		printf("%d\t", crono_f(f2));
		printf("%d\t", crono_f(f3));
		printf("%d\t", crono_f(f4));
		printf("%d\t", crono_f(f5));
		printf("%d\n", (i+1));
	}
	
	return 0;
}
