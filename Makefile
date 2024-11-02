# END
include ./src/neural_network/core/Makefile

all: make_build


make_build:
	mkdir -p ./build

.PHONY: clean
clean:
	rm -rf ./build

