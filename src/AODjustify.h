//
// Created by monsinjs on 11/9/18.
//

#ifndef TPAODJUSTIFY_AODJUSTIFY_H
#define TPAODJUSTIFY_AODJUSTIFY_H


typedef struct {
    long long bell; // Valeur du bellman(i)
    int ind; // L'indice du dernier mot de la ligne commençant par le mot i
} cell_bell;

typedef struct {
    char* mots; //Tableau contenant tous les mots du paragraphe
    int nb_mots;
    int ind_prem_mot;
    int ind_dern_mot;
} paragraphe;



#endif //TPAODJUSTIFY_AODJUSTIFY_H
