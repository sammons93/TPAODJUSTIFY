/**
 * \file AODjustify.c
 * \brief Programme de justification optimale d'un texte (cf TP AOD 2018-2019).
 * \author 
 * \version 0.1
 * \date octobre 2018
 *
 * Usage AODjustify <M> <file> 
 * copie le fichier <file>.in dans le fichier <file>.out  en le justifiant optimalement sur une ligne de taille <M>. 
 */

#include <stdio.h>
#include <string.h> // strlen, 
#include <sys/mman.h> // pour mmap
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>
// #include <locale.h>
#include <limits.h>
#include <unistd.h> // exit
#include <fcntl.h> // open
#include <assert.h> // assert
#include <ctype.h> // isspace


/**
 * \fn usage(const char * error_msg)
 * \brief Fonction d'affichage d'un message d'érreur et de la bonne utilisation du programme. Arrêt de l'exécution (exit(-1)).
 *
 * \param error_msg message d'erreur à afficher 
 * \return void 
 */
void usage(char *error_msg) {
    fprintf(stderr, "AODjustify ERROR> %s\n", error_msg);
    fprintf(stderr, "AODjustify ERROR> Usage: AODjustify <M> <file> \n"
            "  copie le fichier <file>.in dans le fichier <file>.out  en le justifiant optimalement sur une ligne de taille <M>. \n");
    exit(-1);
}


int argmin(int *tab, int size) {
    int k = 0;
    int ind;
    int mini = tab[k];
    while (k < size) {
        if (tab[k + 1] < mini) {
            mini = tab[k + 1];
            ind = k + 1;
        }
        k++;
    }
    return ind;
}

int min(int *tab, int size) {
    int k = 0;
    int mini = tab[k];
    while (k < size) {
        if (tab[k + 1] < mini) {
            mini = tab[k + 1];
        }
        k++;
    }
    return mini;
}

int Bellman(void *file, int i, int M, int *bellman_mem, int **bellman_mem_inter) {
    if (bellman_mem[i] == 50000) {
        for (int j = i; j <= i + M; ++j) {
            if (file[j] == RC and file[j + 1] == RC) {
                bellman_mem[i] = 0;
            }
        }
        for (int j = i; j <= i + M; ++j) {
            if (isspace(file[j])) {
                bellman_mem_inter[i][j - i] = Bellman(j + 1) + (M - (j - i)) ^ 3;
            } else { bellman_mem_inter[i][j - i] = 50000; }
        }


        bellman_mem[i] = min(bellman_mem_inter[i], M);
    }
    return bellman_mem[i];
}


int main(int argc, char **argv) {
    if (argc != 3) usage("Mauvais nombre de paramètres dans l'appel.");

    // A vous de compléter
    return 0;
}
