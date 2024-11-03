# END

all: make_build preprocessing

preprocessing: make_build
	$(MAKE) -C src/preprocessing preprocess man_rota

make_build:
	mkdir -p ./build

.PHONY: clean preprocessing
clean:
	rm -rf ./build
	$(MAKE) -C src/preprocessing clean