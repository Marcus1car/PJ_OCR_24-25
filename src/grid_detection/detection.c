#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction pour vérifier si un pixel est noir
int is_black(Uint32 pixel, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel, format, &r, &g, &b);
    return (r == 0 && g == 0 && b == 0);
}

// Flood Fill pour trouver la zone d'une lettre
void flood_fill(SDL_Surface *surface, int x, int y, Uint8 *visited, int *min_x, int *min_y, int *max_x, int *max_y) {
    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h) return;

    int index = y * surface->w + x;
    if (visited[index]) return;

    Uint32 *pixels = (Uint32 *)surface->pixels;
    SDL_PixelFormat *format = surface->format;

    if (!is_black(pixels[index], format)) return;

    visited[index] = 1;

    // Mettre à jour les bornes de la lettre
    if (x < *min_x) *min_x = x;
    if (x > *max_x) *max_x = x;
    if (y < *min_y) *min_y = y;
    if (y > *max_y) *max_y = y;

    // Appels récursifs pour les voisins
    flood_fill(surface, x + 1, y, visited, min_x, min_y, max_x, max_y);
    flood_fill(surface, x - 1, y, visited, min_x, min_y, max_x, max_y);
    flood_fill(surface, x, y + 1, visited, min_x, min_y, max_x, max_y);
    flood_fill(surface, x, y - 1, visited, min_x, min_y, max_x, max_y);
}

// Sauvegarde une lettre comme image BMP
void save_letter(SDL_Surface *source, int min_x, int min_y, int max_x, int max_y, const char *output_dir, int letter_index) {
    int width = max_x - min_x + 1;
    int height = max_y - min_y + 1;

    SDL_Surface *letter = SDL_CreateRGBSurface(0, width, height, source->format->BitsPerPixel,
                                               source->format->Rmask, source->format->Gmask,
                                               source->format->Bmask, source->format->Amask);

    if (!letter) {
        fprintf(stderr, "Erreur lors de la création de la surface pour la lettre : %s\n", SDL_GetError());
        return;
    }

    SDL_Rect src_rect = {min_x, min_y, width, height};
    SDL_BlitSurface(source, &src_rect, letter, NULL);

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/letter_%d.bmp", output_dir, letter_index);
    if (SDL_SaveBMP(letter, filename) != 0) {
        fprintf(stderr, "Erreur lors de la sauvegarde de la lettre : %s\n", SDL_GetError());
    } else {
        printf("Lettre sauvegardée : %s\n", filename);
    }

    SDL_FreeSurface(letter);
}

// Fonction principale pour extraire les lettres
void extract_letters(const char *filename, const char *output_dir) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return;
    }

    SDL_Surface *image = SDL_LoadBMP(filename);
    if (!image) {
        fprintf(stderr, "Erreur lors du chargement de l'image: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    int width = image->w;
    int height = image->h;

    Uint8 *visited = (Uint8 *)calloc(width * height, sizeof(Uint8));
    if (!visited) {
        fprintf(stderr, "Erreur d'allocation de mémoire.\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return;
    }

    int letter_index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;

            if (!visited[index]) {
                Uint32 *pixels = (Uint32 *)image->pixels;
                if (is_black(pixels[index], image->format)) {
                    // Détecter une nouvelle lettre
                    int min_x = x, min_y = y, max_x = x, max_y = y;
                    flood_fill(image, x, y, visited, &min_x, &min_y, &max_x, &max_y);

                    // Sauvegarder la lettre
                    save_letter(image, min_x, min_y, max_x, max_y, output_dir, letter_index++);
                }
            }
        }
    }

    free(visited);
    SDL_FreeSurface(image);
    SDL_Quit();
}

// Exemple d'utilisation
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <fichier.bmp> <dossier_output>\n", argv[0]);
        return 1;
    }

    extract_letters(argv[1], argv[2]);
    return 0;
}
