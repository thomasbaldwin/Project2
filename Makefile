CC=cc

%.o: %.c
	$(CC) -c -o $@ $^

install: increment.o
	$(CC) increment.o -o increment

clean:
	rm *.o unit_tests
