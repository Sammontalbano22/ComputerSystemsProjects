mallocTest: duMalloc.o mallocTest.o
	gcc duMalloc.o mallocTest.o -o mallocTest

duMalloc.o: duMalloc.c duMalloc.h
	gcc -c duMalloc.c -o duMalloc.o

mallocTest.o: mallocTest.c duMalloc.h
	gcc -c mallocTest.c -o mallocTest.o

clean:
	rm -f mallocTest duMalloc.o mallocTest.o