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
// #include <math.h>
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

int main(int argc, char** argv) {
  if (argc != 3) usage("Mauvais nombre de paramètres dans l'appel.") ;

  // A vous de compléter
  return 0 ;
}
