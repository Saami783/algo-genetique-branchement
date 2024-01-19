#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define NUM_ELEMENTS 7
#define NUM_SETS 6


typedef struct {
    int *chromosome;
    int fitness;
} Individual;

void readMatrixFromFile(const char *filename, Individual **problemMatrix, int *num_elements, int *num_sets) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier.");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "%d", num_elements) != 1 || fscanf(file, "%d", num_sets) != 1) {
        fprintf(stderr, "Erreur lors de la lecture des dimensions du fichier.");
        exit(EXIT_FAILURE);
    }

    *problemMatrix = malloc((*num_sets) * sizeof(Individual));
    if (*problemMatrix == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire.");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *num_sets; i++) {
        (*problemMatrix)[i].chromosome = malloc((*num_elements) * sizeof(int));
        if ((*problemMatrix)[i].chromosome == NULL) {
            fprintf(stderr, "Erreur d'allocation mémoire.");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < *num_elements; j++) {
            if (fscanf(file, "%d", &((*problemMatrix)[i].chromosome[j])) != 1) {
                fprintf(stderr, "Erreur de lecture du fichier.");
                exit(EXIT_FAILURE);
            }
        }
        (*problemMatrix)[i].fitness = -1; // Initialise la fitness
    }

    fclose(file);
}


int isFullCoverage(const int *setCover, Individual *problemMatrix, int num_elements, int num_sets) {
    for (int i = 0; i < num_elements; ++i) {
        int covered = 0;
        for (int j = 0; j < num_sets; ++j) {
            if (setCover[j] == 1 && problemMatrix[j].chromosome[i] == 1) {
                covered = 1;
                break;
            }
        }
        if (!covered) return 0;
    }
    return 1;
}

int calculateSetCoverFitness(const int *setCover, int num_sets) {
    int count = 0;
    for (int i = 0; i < num_sets; ++i) {
        count += setCover[i];
    }
    return count;
}

void branchAndBound(Individual *problemMatrix, int num_sets, int num_elements, int depth, int *setCover, int *bestSetCover, int *bestFitness) {
    if (depth == num_sets) {
        if (isFullCoverage(setCover, problemMatrix, num_elements, num_sets)) {
            int fitness = calculateSetCoverFitness(setCover, num_sets);
            if (fitness < *bestFitness) {
                *bestFitness = fitness;
                memcpy(bestSetCover, setCover, num_sets * sizeof(int));
            }
        }
        return;
    }

    // Ne pas inclure le sous-ensemble actuel
    setCover[depth] = 0;
    branchAndBound(problemMatrix, num_sets, num_elements, depth + 1, setCover, bestSetCover, bestFitness);

    // Inclure le sous-ensemble actuel
    setCover[depth] = 1;
    branchAndBound(problemMatrix, num_sets, num_elements, depth + 1, setCover, bestSetCover, bestFitness);
}

void printBestSetCover(const int *bestSetCover, int num_sets) {
    printf("Fitness de la meilleure couverture : ");
    for (int i = 0; i < num_sets; ++i) {
        printf("%d", bestSetCover[i]);
    }
    printf("\n");
}

int main() {
    Individual *problemMatrix;
    int num_elements, num_sets;

    readMatrixFromFile("matrix.txt", &problemMatrix, &num_elements, &num_sets);

    int *setCover = calloc(num_sets, sizeof(int));
    int *bestSetCover = calloc(num_sets, sizeof(int));
    int bestFitness = INT_MAX;

    branchAndBound(problemMatrix, num_sets, num_elements, 0, setCover, bestSetCover, &bestFitness);

    printBestSetCover(bestSetCover, num_sets);

    for (int i = 0; i < num_sets; i++) {
        free(problemMatrix[i].chromosome);
    }
    free(problemMatrix);
    free(setCover);
    free(bestSetCover);

    return 0;
}