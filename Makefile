include ./scripts/FRAMEWORK

CXX=g++-4.8
CC=gcc

export

all: compile test

compile:
	./scripts/compile.sh

test:
	./scripts/test_prefetcher.py


clean:
	rm -Rf build
	rm -Rf output
	rm -Rf stats.txt
