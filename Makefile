
all: preprocessing neural_network dataset_gen solver

preprocessing:
	$(MAKE) -C src/preprocessing preprocess man_rota

neural_network:
	$(MAKE) -C src/neural_network/core all

dataset_gen:
	$(MAKE) -C src/neural_network/training_dataset all

solver:
	$(MAKE) -C src/solver all

.PHONY: clean preprocessing
clean:
	$(MAKE) -C src/preprocessing clean
	$(MAKE) -C src/neural_network/core clean
	$(MAKE) -C src/neural_network/training_dataset clean 
	$(MAKE) -C src/solver clean
