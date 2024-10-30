#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 100
#define MAX_LINES 100


/**
 * @brief return a char[][], a grid of the crossword from a file.
 *
 * @param path Path of the file you want to extract the grid from.
 * @param nb_lines Number of lines of the given file.
 *
 */
char **ReadGridFromFile(const char *path, int *nb_lines) 
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Error while opening file\n");
        return NULL;
    }
    char **grid = malloc(MAX_LINES * sizeof(char *));
    if (grid == NULL) {
        printf("Error while addressing memory\n");
        fclose(file);
        return NULL;
    }
    char buffer[MAX_LINE_LENGTH];
    *nb_lines = 0;
    while (fgets(buffer, MAX_LINE_LENGTH, file)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Retirer le '\n'
        grid[*nb_lines] = malloc((strlen(buffer) + 1) * sizeof(char));
        if (grid[*nb_lines] == NULL) {
            printf("Erreur d'allocation de mémoire pour la ligne.\n");
            fclose(file);
            return NULL;
        }
        strcpy(grid[*nb_lines], buffer);
        (*nb_lines)++;
    }
    fclose(file);
    return grid;
}

/**
 * @brief Free each line of the grid and itself.
 *
 * @param grid The grid of the characters from the crossword.
 * @param nb_lines Number of lines in the grid.
 *
 */

void FreeBoard(char **grid, int nb_lines) {
    for (int i = 0; i < nb_lines; i++) {
        free(grid[i]);
    }
    free(grid);
}

/**
 * @brief return an integer value of the amount of lines from a file.
 *
 * @param path Path of the file you want to extract the grid from and count the lines.
 *
 */
int CountNumLines(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Error while opening file\n");
        return -1;
    }
    int nb_lines = 0;
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file)) {
        nb_lines++;
    }
    fclose(file);
    return nb_lines;
}

/**
 * @brief return a char[][], a grid of the crossword but all letters to lower.
 *
 * @param grid The grid of the characters from the crossword.
 * @param nb_lines Number of lines in the grid.
 *
 */
char **ConvertToLowerGrid(char **grid, int nb_lines) {

    char **tableau_min = malloc(nb_lines * sizeof(char *));
    if (tableau_min == NULL) {
        printf("Error while addressing memory\n");
        return NULL;
    }

    for (int i = 0; i < nb_lines; i++) {

        tableau_min[i] = malloc((strlen(grid[i]) + 1) * sizeof(char));
        if (tableau_min[i] == NULL) {
            printf("Erreur d'allocation de mémoire pour la ligne %d.\n", i);

            for (int j = 0; j < i; j++) {
                free(tableau_min[j]);
            }
            free(tableau_min);
            return NULL;
        }


        for (int j = 0; j < strlen(grid[i]); j++) {
            tableau_min[i][j] = tolower(grid[i][j]);
        }

        tableau_min[i][strlen(grid[i])] = '\0';
    }

    return tableau_min;
}

/**
 * @brief return an integer value of the amount of character per line of the grid.
 *
 * @param path Path of the file you want to extract the grid from.
 *
 */
int CountCharInLine(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("Error while opening file\n");
        return -1;
    }
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), file)) {
        int longueur = strlen(buffer);
        if (buffer[longueur - 1] == '\n') {
            longueur--; 
        }
        fclose(file);
        return longueur;
    }
    fclose(file);
    return -1;
}
/**
 * @brief convert a string to lower.
 *
 * @param word A string to convert to lower.
 *
 */
char *ConvertWordToLower(const char *word) {

    char *mot_min = malloc((strlen(word) + 1) * sizeof(char)); 
    if (mot_min == NULL) {
        printf("Error while addressing memory\n");
        return NULL;
    }
    for (int i = 0; i < strlen(word); i++) {
        mot_min[i] = tolower(word[i]);
    }
    mot_min[strlen(word)] = '\0';
    return mot_min;
}


int main(int argc, char **argv) {
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: ./solver <grid_file> <researched word>");
    char *resword = calloc(strlen(argv[2]) + 1, sizeof(char));
    char* test = ConvertWordToLower(argv[2]);
    strcpy(resword, test);
    free(test);
    int nblignetab = CountNumLines(argv[1]);
    if (nblignetab == -1) {
        free(resword);
        return 1;
    }
    char** grid_file = ReadGridFromFile(argv[1], &nblignetab);
    char **grid = ConvertToLowerGrid(grid_file,nblignetab);
    for(size_t k = 0; k < nblignetab; k++) free(grid_file[k]);
    free(grid_file);
    if (grid == NULL) {
        free(resword);
        return 1;
    }
    int i = 0, j = 0;
    int index = 0;
    bool searching = false;
    int tempi = 0, tempj = 0;
    int reswordCount = strlen(resword);
    int nbcharinline = CountCharInLine(argv[1]);
    while (true) {
        if (!searching) {

            if (resword[0] == grid[i][j]) {
                searching = true;
            } else {

                if (j == nbcharinline - 1) {
                    if (i == nblignetab - 1) {
                        printf("Not Found\n");
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 1;
                    } else {
                        i++;
                        j = 0;
                    }
                } else {
                    j++;
                }
            }
        } else {

            tempi = i; 
            tempj = j;
            if (tempi + 1 < nblignetab) // Down
            {
                if (resword[1] == grid[tempi + 1][tempj]) {
                    index = 1;
                    tempi++;
                    while (tempi < nblignetab && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempi++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj, tempi-1);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }

            
            tempi = i;
            tempj = j;
            if (tempi > 0) // Up
            {
                if (resword[1] == grid[tempi - 1][tempj]) {
                    index = 1;
                    tempi--;
                    while (tempi >= 0 && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempi--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj, tempi+1);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }
            tempi = i;
            tempj = j;
            if (tempj + 1 < nbcharinline) // Right
            {
                if (resword[1] == grid[tempi][tempj + 1]) {
                    index = 1;
                    tempj++;
                    while (tempj < nbcharinline && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempj++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj-1, tempi);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }
            tempi = i;
            tempj = j;
            if (tempj > 0) // Left
            {
                if (resword[1] == grid[tempi][tempj - 1]) {
                    index = 1;
                    tempj--;
                    while (tempj >= 0 && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempj--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj +1, tempi);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }            
            tempi = i;
            tempj = j;
            if (tempi > 0 && tempj > 0) // Up Left
            {
                if (resword[1] == grid[tempi - 1][tempj - 1]) {
                    index = 1;
                    tempi--;
                    tempj--;
                    while (tempi >= 0 && tempj >= 0 && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempi--;
                            tempj--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj + 1, tempi + 1);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }           
            tempi = i;
            tempj = j;
            if (tempi > 0 && tempj + 1 < nbcharinline) // Up Right
            {
                if (resword[1] == grid[tempi - 1][tempj + 1]) {
                    index = 1;
                    tempi--;
                    tempj++;
                    while (tempi >= 0 && tempj < nbcharinline && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempi--;
                            tempj++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj - 1, tempi + 1);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }           
            tempi = i;
            tempj = j;
            if (tempi + 1 < nblignetab && tempj > 0) // Down Left
            {
                if (resword[1] == grid[tempi + 1][tempj - 1]) {
                    index = 1;
                    tempi++;
                    tempj--;
                    while (tempi < nblignetab && tempj >= 0 && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempi++;
                            tempj--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj + 1, tempi -1);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }            
            tempi = i;
            tempj = j;
            if (tempi + 1 < nblignetab && tempj + 1 < nbcharinline) // Down Right
            {
                if (resword[1] == grid[tempi + 1][tempj + 1]) {
                    index = 1;
                    tempi++;
                    tempj++;
                    while (tempi < nblignetab && tempj < nbcharinline && index < reswordCount) {
                        if (resword[index] == grid[tempi][tempj]) {
                            tempi++;
                            tempj++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj - 1, tempi - 1);
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }
            if (j == nbcharinline - 1) {
                    if (i == nblignetab - 1) {
                        printf("Not Found\n");
                        FreeBoard(grid, nblignetab);
                        free(resword);
                        return 1;
                    } else {
                        i++;
                        j = 0;
                    }
                } else {
                    j++;
                }
        }
    }
    FreeBoard(grid, nblignetab);
    free(resword);
    return 1;
}
