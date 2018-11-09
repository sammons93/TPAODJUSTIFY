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
#include <stdint.h>
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
#include "AODjustify.h"


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


static cell_bell *tab_mem;
static long *E_glob;


long long Bellman(long long *tailles, int nb_mots, int i, long long M) {
    if (tab_mem[i].bell != -1) {
        return tab_mem[i].bell;
    }
    int temp_ind;
    long long temp = 0, min = 0, E_temp;

    if (E_glob[i] >= 0) {

        tab_mem[i].bell = 0;
        tab_mem[i].ind = i;
    } else {
        temp_ind = i;
        E_temp = M - tailles[i];
        for (int k = 0; k < nb_mots; ++k) {

            if (E_temp < 0) break;


            temp = Bellman(tailles, nb_mots, k + 1, M) + (E_temp) ^ 3;

            if (k == i) min = temp;

            if (temp < min) {
                min = temp;
                temp_ind = k;
            }

            E_temp = E_temp - tailles[k+1] - 1;
        }
        tab_mem[i].bell = min;
        tab_mem[i].ind = temp_ind;
    }
    return tab_mem[i].bell;
}


size_t getFilesize(const char *filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

char *ecriture_out(const char *nom_in)
{
    size_t taille = strlen(nom_in);
    size_t taille_copie = taille + 1;
    char *copie = malloc((taille_copie +1)*sizeof(char));
    uint32_t i = 0;
    while ((nom_in[i]!= '.') && (i < taille)) {
        copie[i] = nom_in[i];
        i++;
    }
    if (i < taille) {
        char completion[] = ".out";
        strcat(copie,completion);
    }
    return copie;
}

int main(int argc, char **argv) {
    if (argc != 3) usage("Mauvais nombre de paramètres dans l'appel.");
    long long M = atoi(argv[1]);
    size_t filesize = getFilesize(argv[2]);
    //On ouvre le fichier et recupere sa reference
    int fd = open(argv[2], O_RDONLY, 0);
    assert(fd != -1);
    //On execute mmap
    char *mmappedData = (char *) mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(mmappedData != MAP_FAILED);
    //initialisation des tableaux de memoïsation

//    tab_mem = malloc(filesize * sizeof(cell_bell));
//    for (int i = 0; i < filesize; ++i) {
//        tab_mem[i].bell = -1;
//        tab_mem[i].ind = i;
//    }
//    long long cout = Bellman(mmappedData, 0, M);
//    printf("cout :%lld\n", cout);
//    printf("cout 1 :%lld\n", tab_mem[45].bell);
//    printf("ind :%d\n", tab_mem[45].ind);

    const char *filepath = ecriture_out(argv[2]);

    printf("%s", filepath);
    int fdout = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600); //open crée le fichier ?

    if (fdout == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    size_t sizeout = filesize; /// + le nb d'espaces ajoutés;

    if (lseek(fdout, sizeout-1, SEEK_SET) == -1)
    {
        close(fdout);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    if (write(fdout, "", 1) == -1)
    {
        close(fdout);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    char *map = (char*) mmap(0, sizeout, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);

    if (map == MAP_FAILED)
    {
        close(fdout);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }


    for (size_t i = 0; i < sizeout; i++)
    {
        map[i] = (char) "f"; // remplir avec Bellman;
    }

    // Write it now to disk
    if (msync(map, sizeout, MS_SYNC) == -1)
    {
        perror("Could not sync the file to disk");
    }

    // Don't forget to free the mmapped memory
    if (munmap(map, sizeout) == -1)
    {
        close(fdout);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fdout);




    //Cleanup in
    int rc = munmap(mmappedData, filesize);
    assert(rc == 0);
    close(fd);


    return 0 ;
}
