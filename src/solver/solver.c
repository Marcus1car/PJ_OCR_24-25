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

char **ReadGridFromFile(const char *nom_fichier, int *nb_lignes) 
{
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Error while opening file\n");
        return NULL;
    }
    char **tableau = malloc(MAX_LINES * sizeof(char *));
    if (tableau == NULL) {
        printf("Error while addressing memory\n");
        fclose(fichier);
        return NULL;
    }
    char buffer[MAX_LINE_LENGTH];
    *nb_lignes = 0;
    while (fgets(buffer, MAX_LINE_LENGTH, fichier)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Retirer le '\n'
        tableau[*nb_lignes] = malloc((strlen(buffer) + 1) * sizeof(char));
        if (tableau[*nb_lignes] == NULL) {
            printf("Erreur d'allocation de mémoire pour la ligne.\n");
            fclose(fichier);
            return NULL;
        }
        strcpy(tableau[*nb_lignes], buffer);
        (*nb_lignes)++;
    }
    fclose(fichier);
    return tableau;
}


void FreeBoard(char **tableau, int nb_lignes) {
    for (int i = 0; i < nb_lignes; i++) {
        free(tableau[i]);
    }
    free(tableau);
}

int CountNumLines(const char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Error while opening file\n");
        return -1;
    }
    int nb_lignes = 0;
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), fichier)) {
        nb_lignes++;
    }
    fclose(fichier);
    return nb_lignes;
}

char **ConvertToLowerGrid(char **tableau, int nb_lignes) {

    char **tableau_min = malloc(nb_lignes * sizeof(char *));
    if (tableau_min == NULL) {
        printf("Error while addressing memory\n");
        return NULL;
    }

    for (int i = 0; i < nb_lignes; i++) {

        tableau_min[i] = malloc((strlen(tableau[i]) + 1) * sizeof(char));
        if (tableau_min[i] == NULL) {
            printf("Erreur d'allocation de mémoire pour la ligne %d.\n", i);

            for (int j = 0; j < i; j++) {
                free(tableau_min[j]);
            }
            free(tableau_min);
            return NULL;
        }


        for (int j = 0; j < strlen(tableau[i]); j++) {
            tableau_min[i][j] = tolower(tableau[i][j]);
        }

        tableau_min[i][strlen(tableau[i])] = '\0';
    }

    return tableau_min;
}


int CountCharInLine(const char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Error while opening file\n");
        return -1;
    }
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), fichier)) {
        int longueur = strlen(buffer);
        if (buffer[longueur - 1] == '\n') {
            longueur--; 
        }
        fclose(fichier);
        return longueur;
    }
    fclose(fichier);
    return -1;
}

char *ConvertWordToLower(const char *mot) {

    char *mot_min = malloc((strlen(mot) + 1) * sizeof(char)); 
    if (mot_min == NULL) {
        printf("Error while addressing memory\n");
        return NULL;
    }
    for (int i = 0; i < strlen(mot); i++) {
        mot_min[i] = tolower(mot[i]);
    }
    mot_min[strlen(mot)] = '\0';
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
    char **tableau = ConvertToLowerGrid(grid_file,nblignetab);
    for(size_t k = 0; k < nblignetab; k++) free(grid_file[k]);
    free(grid_file);
    if (tableau == NULL) {
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

            if (resword[0] == tableau[i][j]) {
                searching = true;
            } else {

                if (j == nbcharinline - 1) {
                    if (i == nblignetab - 1) {
                        printf("Not Found\n");
                        FreeBoard(tableau, nblignetab);
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
                if (resword[1] == tableau[tempi + 1][tempj]) {
                    index = 1;
                    tempi++;
                    while (tempi < nblignetab && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempi++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj, tempi-1);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }

            
            tempi = i;
            tempj = j;
            if (tempi > 0) // Up
            {
                if (resword[1] == tableau[tempi - 1][tempj]) {
                    index = 1;
                    tempi--;
                    while (tempi >= 0 && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempi--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj, tempi+1);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }
            tempi = i;
            tempj = j;
            if (tempj + 1 < nbcharinline) // Right
            {
                if (resword[1] == tableau[tempi][tempj + 1]) {
                    index = 1;
                    tempj++;
                    while (tempj < nbcharinline && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempj++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj-1, tempi);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }
            tempi = i;
            tempj = j;
            if (tempj > 0) // Left
            {
                if (resword[1] == tableau[tempi][tempj - 1]) {
                    index = 1;
                    tempj--;
                    while (tempj >= 0 && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempj--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj +1, tempi);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }            
            tempi = i;
            tempj = j;
            if (tempi > 0 && tempj > 0) // Up Left
            {
                if (resword[1] == tableau[tempi - 1][tempj - 1]) {
                    index = 1;
                    tempi--;
                    tempj--;
                    while (tempi >= 0 && tempj >= 0 && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempi--;
                            tempj--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj + 1, tempi + 1);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }           
            tempi = i;
            tempj = j;
            if (tempi > 0 && tempj + 1 < nbcharinline) // Up Right
            {
                if (resword[1] == tableau[tempi - 1][tempj + 1]) {
                    index = 1;
                    tempi--;
                    tempj++;
                    while (tempi >= 0 && tempj < nbcharinline && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempi--;
                            tempj++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj - 1, tempi + 1);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }           
            tempi = i;
            tempj = j;
            if (tempi + 1 < nblignetab && tempj > 0) // Down Left
            {
                if (resword[1] == tableau[tempi + 1][tempj - 1]) {
                    index = 1;
                    tempi++;
                    tempj--;
                    while (tempi < nblignetab && tempj >= 0 && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempi++;
                            tempj--;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj + 1, tempi -1);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }            
            tempi = i;
            tempj = j;
            if (tempi + 1 < nblignetab && tempj + 1 < nbcharinline) // Down Right
            {
                if (resword[1] == tableau[tempi + 1][tempj + 1]) {
                    index = 1;
                    tempi++;
                    tempj++;
                    while (tempi < nblignetab && tempj < nbcharinline && index < reswordCount) {
                        if (resword[index] == tableau[tempi][tempj]) {
                            tempi++;
                            tempj++;
                            index++;
                        } else {
                            break;
                        }
                    }
                    if (index == reswordCount) {
                        printf("(%d,%d)(%d,%d)\n", j, i, tempj - 1, tempi - 1);
                        FreeBoard(tableau, nblignetab);
                        free(resword);
                        return 0;
                    }
                }
            }
            if (j == nbcharinline - 1) {
                    if (i == nblignetab - 1) {
                        printf("Not Found\n");
                        FreeBoard(tableau, nblignetab);
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
    FreeBoard(tableau, nblignetab);
    free(resword);
    return 1;
}
