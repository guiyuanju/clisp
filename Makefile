build/format.o: format.c
	gcc -o build/format.o -c format.c

build/lisp.o: lisp.c
	gcc -o build/lisp.o -c lisp.c

build/scanner.o: build/format.o scanner.c
	gcc -o build/scanner.o -c scanner.c
build/scanner: build/format.o build/scanner.o
	gcc -o build/scanner build/format.o build/scanner.o
scan: build/scanner
	./build/scanner

build/parser.o: parser.c
	gcc -o build/parser.o -c parser.c
build/parser: build/parser.o build/scanner.o build/format.o build/lisp.o
	gcc -o build/parser build/format.o build/scanner.o build/parser.o build/lisp.o
parse: build/parser
	./build/parser

build/evaluator.o: evaluator.c
	gcc -o build/evaluator.o -c evaluator.c
build/evaluator: build/parser.o build/scanner.o build/format.o build/lisp.o build/evaluator.o
	gcc -o build/evaluator build/format.o build/scanner.o build/parser.o build/lisp.o build/evaluator.o
eval: build/evaluator
	./build/evaluator

clean:
	rm build/*
