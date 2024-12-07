#include <SDL2/SDL.h>
#include <stdio.h>

// Fonction pour vérifier si un pixel est noir
int is_black(Uint32 pixel, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);
    return (r == 0 && g == 0 && b == 0);
}

// Fonction pour parcourir les pixels connectés (Flood Fill)
void flood_fill(SDL_Surface *surface, int x, int y, Uint8 *visited) {
    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) return;

    int index = y * surface->w + x;
    if (visited[index]) return;

    Uint32 *pixels = (Uint32 *)surface->pixels;
    SDL_PixelFormat *format = surface->format;

    if (!is_black(pixels[index], format)) return;

    visited[index] = 1;

    // Appels récursifs pour les voisins
    flood_fill(surface, x + 1, y, visited);
    flood_fill(surface, x - 1, y, visited);
    flood_fill(surface, x, y + 1, visited);
    flood_fill(surface, x, y - 1, visited);
}

// Fonction principale pour compter les lettres
int count_letters(const char *filename) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Surface *image = SDL_LoadBMP(filename);
    if (!image) {
        fprintf(stderr, "Erreur lors du chargement de l'image: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    int width = image->w;
    int height = image->h;

    Uint8 *visited = (Uint8 *)calloc(width * height, sizeof(Uint8));
    if (!visited) {
        fprintf(stderr, "Erreur d'allocation de mémoire.\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return -1;
    }

    int letter_count = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;

            if (!visited[index]) {
                Uint32 *pixels = (Uint32 *)image->pixels;
                if (is_black(pixels[index], image->format)) {
                    // Trouver une nouvelle lettre
                    letter_count++;
                    flood_fill(image, x, y, visited);
                }
            }
        }
    }

    free(visited);
    SDL_FreeSurface(image);
    SDL_Quit();
    return letter_count;
}

// Exemple d'utilisation
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fichier.bmp>\n", argv[0]);
        return 1;
    }

    int letters = count_letters(argv[1]);
    if (letters >= 0) {
        printf("Nombre de lettres : %d\n", letters);
    }

    return 0;
}
