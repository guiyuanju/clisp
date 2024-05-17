build/format.o: format.c
	gcc -o build/format.o -c format.c

build/scanner.o: build/format.o scanner.c
	gcc -o build/scanner.o -c scanner.c
build/scanner: build/format.o build/scanner.o
	gcc -o build/scanner build/format.o build/scanner.o
scan: build/scanner
	./build/scanner

build/parser.o: parser.c
	gcc -o build/parser.o -c parser.c
build/parser: build/parser.o build/scanner.o build/format.o
	gcc -o build/parser build/format.o build/scanner.o build/parser.o
parse: build/parser
	./build/parser

clean:
	rm build/*
