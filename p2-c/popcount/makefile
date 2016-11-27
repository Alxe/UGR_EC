
all: data_O0 data_O1 data_O2

test: test_O0 test_O1 test_O2

test_O0: data_O0
	cat ./popcount_O0.dat

test_O1: data_O1
	cat ./popcount_O1.dat

test_O2: data_O2
	cat ./popcount_O2.dat

data_O0: popcount_O0
	./popcount_O0 > ./popcount_O0.dat

data_O1: popcount_O1
	./popcount_O1 > ./popcount_O1.dat

data_O2: popcount_O2
	./popcount_O2 > ./popcount_O2.dat

popcount_O0: popcount.c
	gcc popcount.c -o popcount_O0 -O0

popcount_O1: popcount.c
	gcc popcount.c -o popcount_O1 -O1

popcount_O2: popcount.c
	gcc popcount.c -o popcount_O2 -O2