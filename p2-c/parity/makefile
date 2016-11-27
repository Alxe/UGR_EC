
all: data_O0 data_O1 data_O2

test: test_O0 test_O1 test_O2

test_O0: data_O0
	cat ./parity_O0.dat

test_O1: data_O1
	cat ./parity_O1.dat

test_O2: data_O2
	cat ./parity_O2.dat

data_O0: parity_O0
	./parity_O0 > ./parity_O0.dat

data_O1: parity_O1
	./parity_O1 > ./parity_O1.dat

data_O2: parity_O2
	./parity_O2 > ./parity_O2.dat

parity_O0: parity.c
	gcc parity.c -o parity_O0 -O0

parity_O1: parity.c
	gcc parity.c -o parity_O1 -O1

parity_O2: parity.c
	gcc parity.c -o parity_O2 -O2