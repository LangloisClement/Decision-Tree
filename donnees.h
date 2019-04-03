#ifndef DONNEES_H
#define DONNEES_H

//----------------------------------------------------------
// Données
//----------------------------------------------------------

typedef struct 
{
	unsigned int nb_colonnes; // la 1ere <=> classe à prédire (Y). Autres colonnes <=> variables d'observation (Xi)
	unsigned int nb_lignes;   // <=> les individus
	double** matrice;         // tableau de tableaux de réels (i.e. tableaux 2D de réels)
} matrice_donnees; //structure contenant les données d'apprentissage (donnée par le responsable)

typedef struct _noeud
{
	matrice_donnees* donnes; //accès à la matrice des données d'apprentissage
	int* individu; //individu présent à ce noeud
	int nb_individu; //nombre d'individu présent à ce noeud
	int valeur_etude; //valeur de Y
	double * med; //tableau contenant les valeur utile relative à la médianne: Xi et la valeur de la médianne
	struct _noeud* parent; //accès au parent du noeud (et donc les étapes précédentes)
	struct _noeud* fils_G; //accès au fils gauche du noeud, contient les individus dont le Xi est inférieur à la medianne choisie
	struct _noeud* fils_D; //accès au fils gauche du noeud, contient les individus dont le Xi est inférieur à la medianne choisie
} noeud; //structure de création de l'arbre

matrice_donnees* charger_donnnees(const char* nom_fichier); //permet de charger les données d'apprentissage et d'initialiser la matrice_donnees

// Usage var =  liberer_donnees(var);
matrice_donnees* liberer_donnees(matrice_donnees * data); //permet de liberer l'espace mémoire d'une matrice_donnees

void permuter(double * a,double * b); //fonction d'échange de valeur pour le quick sort

int partition(double tableau[],int down,int up); //fonction de tri du  quick sort

void quick(double tab[],int bas,int haut); //fonction d'appelle récursif du quick sort

double median_corrige(double* tableau,int taille); //calcule la médianne corrigé d'un tableau de double trié

double* test_propor(matrice_donnees* mat,int* echant,int taille_echant,int para_etud); //détermine le meilleur Xi et la medianne associée pour la division

noeud* init_noeud(); //initialise l'espace mémoire d'un noeud

void generer_arbre(int Y,int hauteur_max,int hauteur_actu,double propor_min,double propor_max,int ind_min,noeud* branche); //génére l'arbre de décision suivant les paramètres entrés par l'utilisateur

void afficher_tableau_int(int* tableau, int taille); //permet l'affichage d'un tableau d'entier

//void affichage_arborescence(noeud * arbre, int offset); //affichage sous forme arborescente données par le responsable lors du TD7 (attention ne fonctionne pas dans ce programme)

int hauteur_arbre(noeud const * racine); //calcule la hauteur maximale d'un arbre passé en paramètre

int nb_feuilles(noeud const * racine); //calcule le nombre de feuilles d'un arbre passé en paramètre

void lanceur(); //fonction d'appel du programme

void menu(noeud * arbre); //fonction menu

void affichage_arborescence_bis(noeud * arbre, int offset); //affichage sous forme arborescente del'arbre de décision

double proportion(noeud * branche,int Y); //calcul la proportion d'un paramètre Y parmis les individus d'un noeud

void affichage_feuilles(noeud * arbre); //affichage des feuilles de l'arbre (attention le chemin d'accès ce lit de droite à gauche)

void chemin_acces(noeud * feuille); //affiche le chemin d'accès depuis la racine de l'arbre jusqu'à la feuille passé en paramètre (attention le chemin d'accès ce lit de droite à gauche)

void prediction(noeud * arbre,double* ind); //méthode perméttant de déduire l'appartenace d'un individu au groupe de l'arbre

#endif
