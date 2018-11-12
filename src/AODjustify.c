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
#include <stdbool.h>
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

/**
 * Tableaux de memoisation
 * tab_mem stocke les valeurs Bellman(i) pour chaque paragraphe du fichier
 * E_glob stocke tous les E(i,k) pour chaque paragraphe du fichier
 */
static cell_bell **tab_mem;
static long long ***E_glob;


/**
 *
 * @param filename
 * @return taille du fichier filename
 */
size_t getFilesize(const char *filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}


long long puissance(long long a, long long b){
    long long puissance = a;
    for (long long i = 0; i < b-1 ; ++i) {
        puissance *= a;
    }
    return puissance;
}

/**
 * ecrit le nom du fichier.out
 * @param nom_in
 * @return
 */
char *ecriture_out(const char *nom_in) {
    size_t taille = strlen(nom_in);
    size_t taille_copie = taille + 1;
    char *copie = malloc((taille_copie + 1) * sizeof(char));
    uint32_t i = 0;
    while ((nom_in[i] != '.') && (i < taille)) {
        copie[i] = nom_in[i];
        i++;
    }
    if (i < taille) {
        char completion[] = ".out";
        strcat(copie, completion);
    }
    return copie;
}

/**
 * Recupere le nombre de paragraphes du fichier file
 * @param file
 * @param filesize
 * @return nb_paragraphes
 */
long long recup_nb_para(char *file, long long filesize) {
    long long nb_paragraphes = 0;
    size_t ind_caractere = 0;
    while (ind_caractere < filesize - 3) {
        if ((file[ind_caractere] == 10) && (file[ind_caractere + 1] == 10)) {
            nb_paragraphes++;
        }
        ind_caractere++;
    }
    nb_paragraphes++;
    return nb_paragraphes;
}

/**
 * Retourne deux tableaux d'indices de debut et de fin de chaque paragraphe du fichier d'entree
 * @param mmappedData
 * @param nb_paragraphes
 * @param filesize
 * @return tableaux
 */
long long **getIndiceLastChar(char *mmappedData, long long nb_paragraphes, long long filesize) {
    long long indice_caractere = 0;
    long long indice_paragraphe = 0;
    long long **tableaux = malloc(2 * sizeof(long long *));
    long long *tab_indices_fin_paragraphes = malloc(nb_paragraphes * sizeof(long long));
    long long *tab_indices_debut_paragraphes = malloc(nb_paragraphes * sizeof(long long));
    while (indice_caractere < filesize - 1) {
        if ((mmappedData[indice_caractere] == 10) && (mmappedData[indice_caractere + 1] == 10)) {
            tab_indices_debut_paragraphes[indice_paragraphe + 1] = indice_caractere + 2;
            if ((mmappedData[indice_caractere - 1]) == 10) {
                tab_indices_fin_paragraphes[indice_paragraphe] = indice_caractere + 1;
            } else {
                tab_indices_fin_paragraphes[indice_paragraphe] = indice_caractere - 1;
            } //Si ca finit par trois retour chariots
            indice_paragraphe++;
        }
        indice_caractere++;
    }
    /*cas du dernier paragraphe si il ne finit pas par des retours chariot*/
    if (!((mmappedData[indice_caractere - 1] == 10) && (mmappedData[indice_caractere] == 10))) {
        tab_indices_debut_paragraphes[indice_paragraphe + 1] = indice_caractere + 3;
        tab_indices_fin_paragraphes[indice_paragraphe] = indice_caractere;
    }
    tableaux[0] = tab_indices_debut_paragraphes;
    tableaux[1] = tab_indices_fin_paragraphes;
    return tableaux;
}

/**
 * recupere le nombre de mots du paragraphe commençant par le caractère d'indice debut et finissant par le caractere
 * d'indice fin
 * @param file
 * @param debut
 * @param fin
 * @return nb_mots
 */
long long recup_nb_mots(char *file, long long debut, long long fin) {
    long long nb_mots = 0;
    for (long long i = debut; i <= fin - 1; ++i) {
        if (((file[i] == 32) && (file[i + 1] != 32))) {
            nb_mots++;
        }
    }
    nb_mots++; //on oublie pas le dernier mot du paragraphe.
    return nb_mots;
}

/**
 * Recupere le tableau des tailles des mots du paragraphe
 * @param file
 * @param debut
 * @param fin
 * @param nb_mots
 * @return tailles
 */
long long *recup_tailles_mots(char *file, long long debut, long long fin, long long nb_mots) {
    long long *tailles = malloc(sizeof(long long) * nb_mots);
    long long i = debut;
    long long j = debut;
    long long k = 0;
    while (k < nb_mots - 1) {
        while (file[j] != 32 || file[j + 1] == 32) {
            j++;
        }
        tailles[k] = j - i;
        i += tailles[k] + 1;
        j++;
        k++;
    }
    tailles[k] = fin - i + 1;
    return tailles;
}

/**
 * Recupere un tableau contenant tous les mots du paragraphe (Ex : mots[15] est le 16e mot du paragraphe)
 * @param file
 * @param nb_mots
 * @param tailles
 * @param debut
 * @return mots
 */
char **recup_mots(char *file, long long nb_mots, long long *tailles, long long debut) {
    char **mots = malloc(sizeof(char *) * nb_mots);
    long long k = debut;
    for (long long i = 0; i < nb_mots; ++i) {
        mots[i] = malloc(sizeof(char) * tailles[i]);
        for (long long j = 0; j < tailles[i]; ++j) {
            mots[i][j] = file[k];
            k++;
        }
        k++;
    }
    return mots;
}

/*On raisonne sur les mots, c'est pourquoi on a besoin des tableaux tailles_mots ainsi que mots pour pouvoir
 * continuer */

/**
 * Algorithme recursif de l'equation de Bellman pour le paragraphe numero num_par du fichier. On raisonne sur
 * les mots et non sur les caractères.
 * @param num_par
 * @param nb_mots
 * @param i
 * @param M
 * @return Bellman(i)
 */
long long Bellman(long long num_par, long long nb_mots, long long i, long long M) {
    if (tab_mem[num_par][i].bell != -1) {
        return tab_mem[num_par][i].bell;
    }
    long long temp_ind;
    long long temp = 0, min = 0;
    if (E_glob[num_par][i][nb_mots - 1] >= 0) {
        tab_mem[num_par][i].bell = 0;
        tab_mem[num_par][i].ind = i;
    } else {
        temp_ind = i;
        for (long long k = i; k < nb_mots; ++k) {

            if (E_glob[num_par][i][k] < 0) break;


            temp = Bellman(num_par, nb_mots, k + 1, M) + puissance(E_glob[num_par][i][k], 3);

            if (k == i) min = temp;

            if (temp < min) {
                min = temp;
                temp_ind = k;
            }
        }
        tab_mem[num_par][i].bell = min;

        tab_mem[num_par][i].ind = temp_ind;
    }
    return tab_mem[num_par][i].bell;
}

/**
 * Ecrit dans ligne la ligne du paragraphe num_par commençant par le mot d'indice first et finissant par le mot
 * d'indice last.
 * @param num_par
 * @param ligne
 * @param mots
 * @param tailles
 * @param first
 * @param last
 * @param M
 * @param derniere_ligne
 */
void ecrire_ligne(long long num_par, char *ligne, char **mots, long long *tailles, long long first, long long last, long long M,
                  bool derniere_ligne) {
    long long i = first;
    long long j = 0; //Indice parcourant les caractères des mots
    long long k = 0; //Indice parcourant la ligne

    if (derniere_ligne) {
        while (i < last) {
            while (j < tailles[i]) {
                ligne[k] = mots[i][j];
                j++;
                k++;
            }
            ligne[k] = 32;
            k++;
            i++;
            j = 0;
        }
    } else {
        long long nb_espaces_a_rajouter = E_glob[num_par][first][last];
        long long nb_espaces_rajoutes = 0;
        long long q = nb_espaces_a_rajouter / (last - first);
        while (i < last) {
            while (j < tailles[i]) {
                ligne[k] = mots[i][j];
                j++;
                k++;
            }
            ligne[k] = 32;
            k++;
            while (nb_espaces_rajoutes != nb_espaces_a_rajouter) {
                ligne[k] = 32;
                k++;
                nb_espaces_rajoutes++;
            }
            j = 0;
            i++;
        }
    }

    for (long long m = 0; m < tailles[last]; ++m) {
        ligne[k] = mots[last][m];
        k++;
    }
    ligne[k] = 10;
}

void write_line_in_file(char *mmapOut, long long sizeout, char **lignes, long long nb_lignes, long long M){
    long long k = 0;
    long long ind_char = 0;
    long long ind_ligne = 0;
    while (k < sizeout){
        while (ind_ligne < nb_lignes){
            while (ind_char < M){
                mmapOut[k] = lignes[ind_ligne][ind_char];
                k++;
                ind_char++;
            }
            ind_char = 0;
            ind_ligne++;
        }
    }
}

/**
 * Retourne le nb de lignes justifiées ainsi que le nombre d'espaces à rajouter du paragraphes num_par
 * et calcule aussi les E(i,k) ainsi que les valeurs de bellman(i) pour chaque mot.
 * Ce programme permet egalement de stocker les indices de mots de fin de ligne grâce à la structure cell_bell.
 * @param num_par
 * @param mots
 * @param tailles
 * @param nb_mots
 * @param M
 * @return
 */
long long *calculs_paragraphes(long long num_par, char **mots, long long *tailles, long long nb_mots, long long M) {
    long long *res = malloc(3 * sizeof(long long));
    long long first = 0, last = 0;
    long long bell_0 = 0;
    long long nb_ligne = 0, nb_espaces = 0;
    E_glob[num_par] = malloc(sizeof(long long *) * nb_mots);
    for (long long i = 0; i < nb_mots; ++i) {
        E_glob[num_par][i] = malloc(sizeof(long long) * nb_mots);
        E_glob[num_par][i][i] = M - tailles[i];
        long long k = 0;
        while (k < nb_mots) {
            if (k < i) {
                E_glob[num_par][i][k] = 70000;
            }
            if (k > i) {
                long long deltaik = tailles[k];
                for (long long j = i; j < k; ++j) {
                    deltaik += tailles[j] + 1;
                }
                E_glob[num_par][i][k] = M - deltaik;
            }
            k++;
        }
    }
    tab_mem[num_par] = malloc(nb_mots * sizeof(cell_bell));

    for (long long i = nb_mots - 1; i >= 0; --i) { ;
        tab_mem[num_par][i].bell = -1;
        Bellman(num_par, nb_mots, i, M);
    }
    while (E_glob[num_par][first][nb_mots - 1] < 0) {
        last = tab_mem[num_par][first].ind;
        nb_espaces += E_glob[num_par][first][last];
        nb_ligne++;
        first = last + 1;
    }
    bell_0 = tab_mem[num_par][0].bell;
    res[0] = nb_ligne + 1;
    res[1] = nb_espaces;
    res[2] = bell_0;
    return res;
}

/**
 * Justifie le paragraphe num_par. Appelle la fonction ecrire_ligne et remplit les lignes du paragraphe.
 * @param num_par
 * @param paragraphe
 * @param mots
 * @param tailles
 * @param nb_mots
 * @param M
 * @param nb_ligne
 */
void justifier_paragraphe(long long num_par, paragraphe paragraphe, char **mots, long long *tailles, long long nb_mots, long long M,
                          long long nb_ligne) {
    long long first = 0, last = 0;
    long long j = 0;
    while (E_glob[num_par][first][nb_mots - 1] < 0) { //tant qu'on est pas sur la derniere ligne, on ecrit
        last = tab_mem[num_par][first].ind;
        ecrire_ligne(num_par, paragraphe.lignes[j], mots, tailles, first, last, M, 0);
        j++;
        first = last + 1;
    }
    paragraphe.lignes[nb_ligne - 1] = malloc(sizeof(char) * (M - E_glob[num_par][first][nb_mots - 1] + 1));
    last = nb_mots - 1;
    ecrire_ligne(num_par, paragraphe.lignes[nb_ligne - 1], mots, tailles, first, last, M, 1);
}


long long main(long long argc, char **argv) {
    if (argc != 3) usage("Mauvais nombre de paramètres dans l'appel.");
    long long M = atoi(argv[1]);
    size_t filesize = getFilesize(argv[2]);
    //On ouvre le fichier et recupere sa reference
    long long fd = open(argv[2], O_RDONLY, 0);
    assert(fd != -1);
    //On execute mmap
    char *mmappedData = (char *) mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(mmappedData != MAP_FAILED);

    //On crée le paragraphe text:
    paragraphe *text;
    long long nb_par = recup_nb_para(mmappedData,(long long) filesize);
    text = malloc(sizeof(paragraphe) * nb_par);
    tab_mem = malloc(sizeof(cell_bell *) * nb_par);
    E_glob = malloc(sizeof(long long **) * nb_par);
    long long **tab_ind = getIndiceLastChar(mmappedData, nb_par, filesize); //On recupere les indices de debut et de fin de
    //chaque paragraphe
    for (long long i = 0; i < nb_par; ++i) {
        text[i].ind_prem_car = tab_ind[0][i];
        text[i].ind_dern_car = tab_ind[1][i];
        text[i].nb_mots = recup_nb_mots(mmappedData, text[i].ind_prem_car, text[i].ind_dern_car);
        text[i].tailles_mots = recup_tailles_mots(mmappedData, text
                                                  [i].ind_prem_car,
                                                  text[i].ind_dern_car, text
                                                  [i].nb_mots);
        text[i].mots = recup_mots(mmappedData, text[i].nb_mots, text[i].tailles_mots, text[i].ind_prem_car);

        long long *tab_calculs = calculs_paragraphes(i, text[i].mots, text[i].tailles_mots, text[i].nb_mots, M);
        text[i].nb_espaces = tab_calculs[1];
        text[i].nb_lignes = tab_calculs[0];
        text[i].cout = tab_calculs[2];
        text[i].lignes = malloc(sizeof(char *) * text[i].nb_lignes);
        for (long long j = 0; j < text[i].nb_lignes - 1; ++j) {
            text[i].lignes[j] = malloc(sizeof(char) * (M + 1));
        }
        justifier_paragraphe(i, text[i], text[i].mots, text[i].tailles_mots, text[i].nb_mots, M, text[i].nb_lignes);
    }
    long long nb_lignes_tot = 0;
    long long nb_espaces_tot = 0;
    long long cout_tot = 0;
    for (long long k = 0; k < nb_par; ++k) {
        cout_tot += text[k].cout;
        nb_lignes_tot += text[k].nb_lignes;
        nb_espaces_tot += text[k].nb_espaces;
    }
    const char *filepath = ecriture_out(argv[2]);


    long long fdout = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0600); //open crée le fichier

    if (fdout == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    size_t sizeout = filesize + (nb_lignes_tot-nb_par) + nb_espaces_tot ; /// + le nb d'espaces ajoutés;
    if (lseek(fdout, sizeout - 1, SEEK_SET) == -1) {
        close(fdout);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }
    if (write(fdout, "", 1) == -1) {
        close(fdout);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }
    char *map = (char *) mmap(0, sizeout, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);
    if (map == MAP_FAILED) {
        close(fdout);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    for (long long l = 0; l < nb_par; ++l) {
        write_line_in_file(map, sizeout, text[l].lignes, text[l].nb_lignes, M);
    }
    // Write it now to disk
    if (msync(map, sizeout, MS_SYNC) == -1) {
        perror("Could not sync the file to disk");
    }

    // Don't forget to free the mmapped memory
    if (munmap(map, sizeout) == -1) {
        close(fdout);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Un-mmaping doesn't close the file, so we still need to do that.
    close(fdout);




    //Cleanup in
    long long rc = munmap(mmappedData, filesize);
    assert(rc == 0);
    close(fd);


    return 0;
}
