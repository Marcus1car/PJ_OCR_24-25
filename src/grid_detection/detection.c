#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Définition de la structure de la grille
typedef struct {
    int x, y, w, h; // Position et taille de la case
} Cell;

// Fonction pour charger une image BMP avec SDL
SDL_Surface* load_image(const char *filename) {
    SDL_Surface *image = SDL_LoadBMP(filename);
    if (!image) {
        printf("Erreur de chargement de l'image : %s\n", SDL_GetError());
        exit(1);
    }
    return image;
}

// Fonction pour appliquer la transformée de Hough (basique)
void hough_transform(
    unsigned char* edge_image, int width, int height,
    int max_dist, int max_theta, uint8_t* hough_image)
{
    // allocate the accumulator
    int* accumulator = calloc(max_dist * max_theta, sizeof(int));

    for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
    {
        // if the pixel is not an edge, do nothing
        if (edge_image[y * width + x] == 0)
            continue;

        for (int theta = 0; theta < max_theta; theta++)
        {
            // calculate rho for each theta
            double rad = theta * M_PI / 180.0;
            int dist = x * cos(rad) + y * sin(rad);
            if(dist < 0) dist = -dist;
            if(dist >= max_dist) 
                continue;

            // increment the accumulator for the current rho and theta
            accumulator[dist * max_theta + theta]++;
        }
    }

    // find the maximum value in the accumulator
    int max = 0;
    for (int i = 0; i < max_dist * max_theta; i++)
    {
        if (accumulator[i] > max)
            max = accumulator[i];
    }

    // normalize the accumulator
    // some loss of precision here
    // I might need to fix this later
    for (int i = 0; i < max_dist * max_theta; i++)
    {
        if (max == 0)
            hough_image[i] = 0;
        else
            hough_image[i] = (uint8_t)(accumulator[i] * 255 / max);
    }

    // free the accumulator
    free(accumulator);
}

// Fonction pour calculer la distance entre deux cases
double calculate_distance(Cell *a, Cell *b) {
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

// Implémentation basique de DBSCAN
void dbscan(SDL_Surface *image, Cell **cells, int *num_cells, double eps, int min_points) {
    if (!image) {
        printf("Erreur : Image n'est pas valide.\n");
        return;
    }

    int width = image->w;
    int height = image->h;
    int cell_size = 20;

    bool *visited = calloc(width * height, sizeof(bool));
    if (!visited) {
        printf("Erreur : Allocation mémoire pour 'visited'.\n");
        return;
    }

    bool *is_core = calloc(width * height, sizeof(bool));
    if (!is_core) {
        free(visited);
        printf("Erreur : Allocation mémoire pour 'is_core'.\n");
        return;
    }

    for (int y = 0; y < height; y += cell_size) {
        for (int x = 0; x < width; x += cell_size) {
            if (*num_cells >= 1000) { // Protection contre le dépassement
                printf("Erreur : Trop de cellules détectées (limite : 1000).\n");
                break;
            }

            Cell *new_cell = malloc(sizeof(Cell));
            if (!new_cell) {
                printf("Erreur : Allocation mémoire pour une nouvelle case.\n");
                break;
            }

            new_cell->x = x;
            new_cell->y = y;
            new_cell->w = cell_size;
            new_cell->h = cell_size;

            cells[*num_cells] = new_cell;
            (*num_cells)++;
        }
    }

    free(visited);
    free(is_core);
}

// Fonction pour enregistrer chaque case comme une image
void save_cell_images(SDL_Surface *image, Cell **cells, int num_cells, const char *output_folder) {
    if (!image || !cells || num_cells <= 0 || !output_folder) {
        printf("Erreur : Paramètres invalides pour l'enregistrement des images des cases.\n");
        return;
    }

    // Créer le dossier si nécessaire
    char command[256];
    snprintf(command, sizeof(command), "mkdir -p %s", output_folder);
    system(command);

    for (int i = 0; i < num_cells; i++) {
        // Extraire la sous-surface de l'image pour chaque case
        SDL_Rect rect = { cells[i]->x, cells[i]->y, cells[i]->w, cells[i]->h };
        SDL_Surface *cell_image = SDL_CreateRGBSurface(0, cells[i]->w, cells[i]->h, 32,
                                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        if (!cell_image) {
            printf("Erreur de création de surface pour la case %d\n", i);
            continue;
        }

        // Copier la portion de l'image vers la nouvelle surface
        if (SDL_BlitSurface(image, &rect, cell_image, NULL) < 0) {
            printf("Erreur de copie de surface : %s\n", SDL_GetError());
            SDL_FreeSurface(cell_image);
            continue;
        }

        // Enregistrer l'image de la case
        char filename[256];
        snprintf(filename, sizeof(filename), "%s/cell_%d.bmp", output_folder, i);
        if (SDL_SaveBMP(cell_image, filename) < 0) {
            printf("Erreur d'enregistrement de l'image %s : %s\n", filename, SDL_GetError());
        }

        SDL_FreeSurface(cell_image);
    }
}

// Fonction pour reconnaître les mots à rechercher (en utilisant les cases proches)
void recognize_words(Cell **cells, int num_cells, const char *output_folder_words, const char *output_folder_letters, SDL_Surface *image) {
    if (!cells || num_cells <= 0 || !output_folder_words || !output_folder_letters || !image) {
        printf("Erreur : Paramètres invalides pour la reconnaissance des mots.\n");
        return;
    }

    // Créer les dossiers pour les mots et les lettres
    char command[256];
    snprintf(command, sizeof(command), "mkdir -p %s", output_folder_words);
    system(command);

    snprintf(command, sizeof(command), "mkdir -p %s", output_folder_letters);
    system(command);

    FILE *file = fopen("words.txt", "w");
    if (!file) {
        printf("Erreur d'ouverture du fichier des mots\n");
        return;
    }

    int letter_count = 0; // Compteur pour les lettres
    for (int i = 0; i < num_cells; i++) {
        for (int j = i + 1; j < num_cells; j++) {
            if (calculate_distance(cells[i], cells[j]) < 50) { // Seuil de proximité
                fprintf(file, "Mot trouvé : (%d,%d) -> (%d,%d)\n", cells[i]->x, cells[i]->y, cells[j]->x, cells[j]->y);

                // Enregistrer les lettres associées dans le dossier
                SDL_Rect rect = { cells[i]->x, cells[i]->y, cells[i]->w, cells[i]->h };
                SDL_Surface *letter_image = SDL_CreateRGBSurface(0, cells[i]->w, cells[i]->h, 32,
                                                                 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
                if (!letter_image) {
                    printf("Erreur de création de surface pour la lettre %d\n", letter_count);
                    continue;
                }

                // Copier la portion de l'image vers la nouvelle surface
                if (SDL_BlitSurface(image, &rect, letter_image, NULL) < 0) {
                    printf("Erreur de copie de surface : %s\n", SDL_GetError());
                    SDL_FreeSurface(letter_image);
                    continue;
                }

                // Enregistrer l'image de la lettre
                char filename[256];
                snprintf(filename, sizeof(filename), "%s/letter_%d.bmp", output_folder_letters, letter_count++);
                if (SDL_SaveBMP(letter_image, filename) < 0) {
                    printf("Erreur d'enregistrement de l'image %s : %s\n", filename, SDL_GetError());
                }

                SDL_FreeSurface(letter_image);
            }
        }
    }

    fclose(file);
}
// Fonction principale
int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur d'initialisation SDL : %s\n", SDL_GetError());
        return 1;
    }

    // Charger l'image
    const char *image_file = "image.bmp";
    SDL_Surface *image = load_image(image_file);

    if (!image) {
        printf("Erreur : Impossible de charger l'image %s\n", image_file);
        SDL_Quit();
        return 1;
    }

    // Initialisation des paramètres pour la transformée de Hough
    int width = image->w;
    int height = image->h;
    int max_dist = (int)sqrt(width * width + height * height); // Distance max
    int max_theta = 180; // Résolution angulaire (en degrés)

    uint8_t *hough_image = malloc(max_dist * max_theta);
    if (!hough_image) {
        printf("Erreur d'allocation mémoire pour l'image Hough.\n");
        SDL_FreeSurface(image);
        SDL_Quit();
        return 1;
    }

    unsigned char *edge_image = (unsigned char *)image->pixels;
    hough_transform(edge_image, width, height, max_dist, max_theta, hough_image);

    Cell *cells[1000];
    int num_cells = 0;
    dbscan(image, cells, &num_cells, 50.0, 5);

    const char *cells_folder = "cells";
    save_cell_images(image, cells, num_cells, cells_folder);

    const char *words_folder = "words";
    const char *letters_folder = "letters";
    recognize_words(cells, num_cells, words_folder, letters_folder, image);

    for (int i = 0; i < num_cells; i++) {
        free(cells[i]);
    }
    free(hough_image);
    SDL_FreeSurface(image);
    SDL_Quit();

    printf("Traitement terminé avec succès.\n");
    return 0;
}