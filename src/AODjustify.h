//
// Created by monsinjs on 11/9/18.
//

#ifndef TPAODJUSTIFY_AODJUSTIFY_H
#define TPAODJUSTIFY_AODJUSTIFY_H



/**
 * Structure de bellman
 */
typedef struct {
    long long bell; // Valeur du bellman(i)
    long long ind; // L'indice du dernier mot de la ligne commençant par le mot i
} cell_bell;

typedef struct {
    char **mots; //Tableau contenant tous les mots du paragraphe
    long long *tailles_mots;//Tableau contenant toutes les tailles des mots du paragraphe
    long long nb_mots;
    long long ind_prem_car; //indice du premier caractère du paragraphe
    long long ind_dern_car; //indice du dernier caractère du paragraphe
    char **lignes; //Tableau contenant toutes les lignes du paragraphe
    long long nb_lignes;
    long long nb_espaces;
    long long cout;
} paragraphe;

/**
 *
 * @param filename
 * @return taille du fichier filename
 */
size_t getFilesize(const char *filename);

/**
 * ecrit le nom du fichier.out
 * @param nom_in
 * @return
 */
char *ecriture_out(const char *nom_in);


/**
 * Recupere le nombre de paragraphes du fichier file
 * @param file
 * @param filesize
 * @return nb_paragraphes
 */
long long recup_nb_para(char *file, long long filesize);

/**
 * Retourne deux tableaux d'indices de debut et de fin de chaque paragraphe du fichier d'entree
 * @param mmappedData
 * @param nb_paragraphes
 * @param filesize
 * @return tableaux
 */
long long **getIndiceLastChar(char *mmappedData, long long nb_paragraphes, long long filesize);

/**
 * recupere le nombre de mots du paragraphe commençant par le caractère d'indice debut et finissant par le caractere
 * d'indice fin
 * @param file
 * @param debut
 * @param fin
 * @return nb_mots
 */
long long recup_nb_mots(char *file, long long debut, long long fin);

/**
 * Recupere le tableau des tailles des mots du paragraphe
 * @param file
 * @param debut
 * @param fin
 * @param nb_mots
 * @return tailles
 */
long long *recup_tailles_mots(char *file, long long debut, long long fin, long long nb_mots);


/**
 * Recupere un tableau contenant tous les mots du paragraphe (Ex : mots[15] est le 16e mot du paragraphe)
 * @param file
 * @param nb_mots
 * @param tailles
 * @param debut
 * @return mots
 */
char **recup_mots(char *file, long long nb_mots, long long *tailles, long long debut);


/**
 * Algorithme recursif de l'equation de Bellman pour le paragraphe numero num_par du fichier. On raisonne sur
 * les mots et non sur les caractères.
 * @param num_par
 * @param nb_mots
 * @param i
 * @param M
 * @return Bellman(i)
 */
long long Bellman(long long num_par, long long nb_mots, long long i, long long M);

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
                  bool derniere_ligne);

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
long long *calculs_paragraphes(long long num_par, char **mots, long long *tailles, long long nb_mots, long long M);



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
                          long long nb_ligne);


#endif //TPAODJUSTIFY_AODJUSTIFY_H
