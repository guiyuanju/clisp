# build/format.o: format.c
# 	gcc -g -o build/format.o -c format.c

# build/lisp.o: lisp.c
# 	gcc -g -o build/lisp.o -c lisp.c

# build/scanner.o: build/format.o scanner.c
# 	gcc -g -o build/scanner.o -c scanner.c
# build/scanner: build/format.o build/scanner.o
# 	gcc -g -o build/scanner build/format.o build/scanner.o
# scan: build/scanner
# 	./build/scanner

# build/parser.o: parser.c
# 	gcc -g -o build/parser.o -c parser.c
# build/parser: build/parser.o build/scanner.o build/format.o build/lisp.o
# 	gcc -g -o build/parser build/format.o build/scanner.o build/parser.o build/lisp.o
# parse: build/parser
# 	./build/parser

# build/evaluator.o: evaluator.c
# 	gcc -g -o build/evaluator.o -c evaluator.c
# build/evaluator: build/parser.o build/scanner.o build/format.o build/lisp.o build/evaluator.o
# 	gcc -g -o build/evaluator build/format.o build/scanner.o build/parser.o build/lisp.o build/evaluator.o
# eval: build/evaluator
# 	./build/evaluator

build/clisp: clisp.c
	gcc -g -o build/clisp -O clisp.c
run: build/clisp
	./build/clisp

clean:
	rm build/*
