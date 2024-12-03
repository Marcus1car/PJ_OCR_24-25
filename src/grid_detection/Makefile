EXEC = hough_transform

# Répertoire source
SRC = detection.c

# Répertoires d'inclusion et de bibliothèque
CFLAGS = -Wall `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm -lSDL2_image

# Compilation et génération de l'exécutable
$(EXEC): $(SRC)
	gcc $(SRC) -o $(EXEC) $(CFLAGS) $(LDFLAGS)

# Cible pour nettoyer les fichiers générés
clean:
	rm -f $(EXEC)
