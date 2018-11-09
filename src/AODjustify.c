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
void usage(char * error_msg) {
   fprintf( stderr, "AODjustify ERROR> %s\n", error_msg) ;
   fprintf( stderr, "AODjustify ERROR> Usage: AODjustify <M> <file> \n"
                    "  copie le fichier <file>.in dans le fichier <file>.out  en le justifiant optimalement sur une ligne de taille <M>. \n") ;
   exit(-1) ;
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

int Bellman(char *file, int i, int M, int* tab_mem, int **bellman_mem_inter) {
    if (tab_mem[i] == -1) {

        for (int j = i; j > i+M; ++j) {
            if ((file[j] == 10 ) && (file[j - 1] == 10 )) {
                    tab_mem[i] = 0;
                    return tab_mem[i];

                }
            }

        for (int j = i; j<i+M; ++j) {
            if (file[j] == 32) {
                bellman_mem_inter[i][j - i -1] = Bellman(file, j + 1, M, tab_mem, bellman_mem_inter) + (M - (j - i)) ^ 3;
            }
        }
        tab_mem[i] = min(bellman_mem_inter[i], M);
        return tab_mem[i];
    }

}

void* carac_fin_ligne( int M, int** bellman_mem_inter, int* bellman_car_fin_ligne){
    bellman_car_fin_ligne[0] = argmin(bellman_mem_inter[0], M);
    printf("%d\n", bellman_car_fin_ligne[0]);
    for (int j = 0+1; j < 0 + M; ++j) {
        bellman_car_fin_ligne[j-0] = bellman_car_fin_ligne[j-0-1] + argmin(bellman_mem_inter[bellman_car_fin_ligne[j-0-1] +1], M);
    }
}

size_t getFilesize(const char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

int main(int argc, char** argv) {
    if (argc != 3) usage("Mauvais nombre de paramètres dans l'appel.") ;
    int M = atoi(argv[1]);
    printf("%d\n", M);
    size_t filesize = getFilesize(argv[2]);
    //On ouvre le fichier et recupere sa reference
    int fd = open(argv[2], O_RDONLY, 0);
    assert(fd != -1);
    //On execute mmap
    char* mmappedData = (char*) mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(mmappedData != MAP_FAILED);

    printf("%d\n", mmappedData[15]);
    //initialisation des tableaux de memoïsation
    int* tab_memo;
    tab_memo = malloc(sizeof(int) * filesize);
    for (int l = 0; l < filesize; ++l) {
        tab_memo[l] = -1;
    }
    int** bellman_mem_inter;
    bellman_mem_inter = malloc(sizeof(int*) * filesize);
    int* bellman_carac_fin_ligne;
    bellman_carac_fin_ligne = malloc(sizeof(int)* filesize );
    for (size_t i = 0; i < filesize; ++i) {
        bellman_mem_inter[i] = malloc(sizeof(int) * M);
        bellman_carac_fin_ligne[i] = 500000000;
        for (int j = 0; j < M; ++j) {
            bellman_mem_inter[i][j] = 500000000;  //+ inf
        }
    }

    for (size_t k = 0; k < M +1 ; ++k) {
        {
            printf("%d,  ", bellman_mem_inter[0][k]);
        }
    }
    printf("\n\n\n");
    int cout_para = Bellman(mmappedData, 0, M, tab_memo, bellman_mem_inter);

    for (size_t k = 0; k < filesize ; ++k) {


                printf("%d,  ", tab_memo[k]);


    }
    printf("\n%i\n", cout_para);

    exit(1);
    carac_fin_ligne(M, bellman_mem_inter, bellman_carac_fin_ligne);

//    for (int k = 0; k < M; ++k) {
//        if (bellman_carac_fin_ligne[k] < 50000){
//            printf("%d\n", bellman_carac_fin_ligne[k]);
//        }
//    }
//    printf("\n");
//    printf("%i\n", cout_para);
//    printf("indice du caractère de fin de ligne : %i\n", bellman_carac_fin_ligne[0]);
//    printf("%d\n", mmappedData[bellman_carac_fin_ligne[0]]);
//    printf("%d\n", mmappedData[bellman_carac_fin_ligne[0]+1]);
//    printf("%d\n", (int) ceil(45/M) + 1);





    //Cleanup
    int rc = munmap(mmappedData, filesize);
    assert(rc == 0);
    close(fd);


  return 0 ;
}
