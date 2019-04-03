#include "donnees.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//----------------------------------------------------------
// Données
//----------------------------------------------------------

matrice_donnees* charger_donnnees(const char* nom_fichier) //permet de charger les données d'apprentissage et d'initialiser la matrice_donnees
{
	FILE* fichier = fopen(nom_fichier,"r");
	if( fichier != NULL )
	{
		unsigned int nb_lignes;
		unsigned int nb_colonnes;
	
		// Etape 1 - traitement première ligne
		fscanf(fichier, "%u %u", &nb_lignes, &nb_colonnes); // %u <=> unsigned int
		fgetc(fichier); // lecture du retour chariot ('\n'). Valeur ignorée => passage à la ligne suivante
				
		// Etape 2 - allocation des lignes de la matrice
		double** matrice = (double**) malloc( nb_lignes * sizeof(double*) );
		
		// Etape 3 - remplissage de la matrice
		for(int ligne = 0 ; ligne < nb_lignes ; ligne++)
		{
			// allocation des colonnes de la matrice (pour chaque ligne)
			matrice[ligne] = (double*) malloc( nb_colonnes * sizeof(double) );

			for(int colonne = 0 ; colonne < nb_colonnes ; colonne++)
			{
				if(colonne != nb_colonnes-1)
				{
					fscanf(fichier, "%lg ", &matrice[ligne][colonne]);
				}
				else // dernière colonne (où le réel n'est pas suivi d'un espace)
				{
					fscanf(fichier, "%lg" , &matrice[ligne][colonne]);
				}
			}
			
			fgetc(fichier); // lecture du retour chariot ('\n'). Valeur ignorée => passage à la ligne suivante
		}
		
		matrice_donnees * data = (matrice_donnees*) malloc(sizeof(matrice_donnees));
		data->nb_colonnes = nb_colonnes;
		data->nb_lignes = nb_lignes;
		data->matrice = matrice;
		
		fclose(fichier);
		return data;
	}
	
	printf("Fichier %s inconnu.\n", nom_fichier);
	return NULL;
}

// Usage : var = liberer_donnees(var);  => var devient NULL
matrice_donnees* liberer_donnees(matrice_donnees * data) //permet de liberer l'espace mémoire d'une matrice_donnees
{
	if(data != NULL)
	{
		for(int ligne = 0 ; ligne < data->nb_lignes ; ligne++)
		{
			if(data->matrice[ligne] != NULL) // normalement toujours vrai
				free(data->matrice[ligne]);
		}
		free(data->matrice);
		free(data);
	}
	return NULL;
}

void permuter(double * a,double * b) //fonction d'échange de valeur pour le quick sort
{
	double tmp= 0.0;
	tmp=*a;
	*a=*b;
	*b=tmp;
}

int partition(double tableau[],int down,int up) //fonction de tri du  quick sort
{
	double pivot=tableau[down];
	permuter(&tableau[down],&tableau[up-1]);
	int leftwall=down;
	
	for(int i=down;i<up;i++)
	{
		if(tableau[i]<pivot)
		{
			permuter(&tableau[i],&tableau[leftwall]);
			leftwall++;
		}
	}
	permuter(&tableau[up-1],&tableau[leftwall]);
	return leftwall;
}

void quick(double tab[],int bas,int haut) //fonction d'appelle récursif du quick sort
{
	if(bas<haut)
	{
		int pivot_local=partition(tab,bas,haut);
		quick(tab,bas,pivot_local);
		quick(tab,pivot_local+1,haut);
	}
}

double median_corrige(double* tableau,int taille) //calcule la médianne corrigé d'un tableau de double trié
{
	double med=0; //initialisation de la variable de retour
	if(taille%2==0) //cas paire
	{
		med=(tableau[taille/2-1]+tableau[taille/2])/2;
	}
	else //cas impaire
	{
		med=tableau[taille/2];
	}
	int pos=taille-1;
	while(med==tableau[taille-1]&&pos>-1) //correction de la medianne
	{
		med=tableau[pos];
		pos--;
	}
	if(pos==-1) med=-1; //si impossible de corrigé
	//printf("%lg",med);
	return med; //renvoie la valeur
}

double* test_propor(matrice_donnees* mat,int* echant,int taille_echant,int para_etud) //détermine le meilleur Xi et la medianne associée pour la division
{
	double* tabretour=(double*) malloc(sizeof(double)*3); //Xi,med de Xi,meilleur propor
	tabretour[0]=0.0,tabretour[1]=0.0,tabretour[2]=0.0; //initialisation du tableau de retour
	double* tab=(double*) malloc(sizeof(double)*taille_echant); //création d'un tableau temporaire pour stocker les valeur de Xi sans affecter la matrice
	for(int i=1;i<mat->nb_colonnes;i++) //pour tous les Xi
	{
		for(int j=0;j<taille_echant;j++)
		{
			tab[j]=mat->matrice[echant[j]][i]; //remplie le tableau
		}
		quick(tab,0,taille_echant); //tri du tableau
		double med=median_corrige(tab,taille_echant); //calcule de la medianne
		int compt=0,infvalide=0,supvalide=0; //initialisation des compteurs
		for(int j=0;j<taille_echant;j++) //parcour de la matrice
		{
			if(mat->matrice[echant[j]][i]<=med) //si inférieur à la medianne
			{
				compt++;
				if(mat->matrice[echant[j]][0]==para_etud) infvalide++; //si inférieur et posséde la valeur de Y
			}
			else //si supérieur à la medianne
			{
				if(mat->matrice[echant[j]][0]==para_etud) supvalide++; //si supérieur et posséde la valeur de Y
			}
		}
		if((double) infvalide/(double) compt *100.0<(double) supvalide/(double) (taille_echant-compt) *100.0) //calule des proportion pour chaque sous échantillon et choix du sous échantillon inférieur et posséde la valeur de Y sup ou inf
		{
			if((double) supvalide/(double) (taille_echant-compt) *100.0>tabretour[2]) //comparaison à la médianne déjà existante
			{
				tabretour[0]=i;
				tabretour[1]=med;
				tabretour[2]=(double) supvalide/(double) (taille_echant-compt) *100.0; //remplacement
			}
		}
		else
		{
			if((double) infvalide/(double) compt *100.0>tabretour[2]) //comparaison à la médianne déjà existante
			{
				tabretour[0]=i;
				tabretour[1]=med;
				tabretour[2]=(double) infvalide/(double) compt *100.0; //remplacement
			}
		}
	}
	free(tab); //libère l'espace mémoire du tableau
	return tabretour; //renvoie le tableau avec le meuilleur Xi et sa médianne
}

noeud* init_noeud() //initialise l'espace mémoire d'un noeud
{
	noeud* retour=(noeud*) malloc(sizeof(noeud)); //attribution mémoire
	if(retour!=NULL)
	{
		retour->fils_D=NULL;
		retour->fils_G=NULL;
		retour->parent=NULL;
		retour->donnes=NULL;
		retour->med=NULL;
		retour->individu=NULL;
		retour->nb_individu=0;
		retour->valeur_etude=0;
	}
	return retour;
}

noeud* init_racine(matrice_donnees * data) //initialise l'espace mémoire de la racine d'un arbre
{
	noeud* retour=(noeud*) malloc(sizeof(noeud)); //attribution mémoire
	if(retour!=NULL)
	{
		retour->fils_D=NULL;
		retour->fils_G=NULL;
		retour->parent=NULL;
		retour->donnes=data; //attribution de la matrice
		retour->med=NULL;
		retour->nb_individu=data->nb_lignes;
		retour->valeur_etude=0;
		retour->individu=malloc(sizeof(int)*data->nb_lignes);
		for(int i=0;i<retour->nb_individu;i++)
		{
			retour->individu[i]=i; //remplissage du tableau d'individu
		}
	}
	return retour;
}

void generer_arbre(int Y,int hauteur_max,int hauteur_actu,double propor_min,double propor_max,int ind_min,noeud* branche) //génére l'arbre de décision suivant les paramètres entrés par l'utilisateur
{
	bool flag=true; //flag d'arrêt récursif
	if(hauteur_actu>hauteur_max) flag=false; //test de la hauteur de l'arbre
	if(branche->nb_individu<ind_min) flag=false; //test du nombre d'individu
	double p=proportion(branche,Y); //calue de la proportion
	if(p<propor_min ||p>propor_max) flag=false; //test de la proportion
	if(flag) //si tous les tests sont réussis
	{
		double* tabpropr=test_propor(branche->donnes,branche->individu,branche->nb_individu,Y); //choix du meilleur Xi
		branche->med=malloc(sizeof(double)*2); //création du tableau mémoire pour Xi et sa médianne
		branche->med[0]=tabpropr[0]; //initialisation
		branche->med[1]=tabpropr[1];
		int compt=0;
		for(int i=0;i<branche->nb_individu;i++)
		{
			if(branche->donnes->matrice[branche->individu[i]][(int) tabpropr[0]]<=tabpropr[1]) compt++;
		}
		int* nouv_tab_G=(int*) malloc(sizeof(int)*compt); //création des tableau d'individu pour les fils gauche et droit
		int* nouv_tab_D=(int*) malloc(sizeof(int)*(branche->nb_individu-compt));
		int ig=0,id=0; //index des sous tableau
		for(int i=0;i<branche->nb_individu;i++)
		{
			if(branche->donnes->matrice[branche->individu[i]][(int) tabpropr[0]]<=tabpropr[1])
			{
				nouv_tab_G[ig]=branche->individu[i]; //remplissage du tableau de gauche
				ig++;
			}
			else
			{
				nouv_tab_D[id]=branche->individu[i]; //remplissage du tableau de droite
				id++;
			}
		}
		noeud* fG=init_noeud(); //création du fils gauche
		fG->donnes=branche->donnes;
		fG->individu=nouv_tab_G;
		fG->nb_individu=compt;
		fG->valeur_etude=Y;
		fG->parent=branche;
		
		noeud* fD=init_noeud(); //création du fils droit
		fD->donnes=branche->donnes;
		fD->individu=nouv_tab_D;
		fD->valeur_etude=Y;
		fD->nb_individu=branche->nb_individu-compt;
		fD->parent=branche;
		
		branche->fils_D=fD; //association
		branche->fils_G=fG;
		
		
		generer_arbre(Y, hauteur_max, hauteur_actu+1, propor_min, propor_max, ind_min, branche->fils_G); //appel récursif
		generer_arbre(Y, hauteur_max, hauteur_actu+1, propor_min, propor_max, ind_min, branche->fils_D);
		
	}
}

bool est_feuille(noeud const * branche) //permet de déterminer si un noeud est une feuille
{
	return (branche->fils_G==NULL && branche->fils_D==NULL);
}

// Affiche d'un certain nombre d'espaces, proportionnel à offset
void afficher_offset(int offset)
{
	for(int i = 0 ; i < offset ; i++)
	{
		printf("  "); // 2 espaces
	}
}

// Affichage récursif <=> parcours préfixé
// Précondition : l'arbre peut-être NULL (dans ce cas rien ne s'affiche)
// Initialisation : offset = 0
/*void affichage_arborescence(noeud * arbre, int offset)
{
	if(arbre != NULL)
	{
		// Etape 1 - afficher la valeur
		printf("\n");
		afficher_offset(offset);
		if( offset != 0 ) // tous les éléments sauf la racine
		{
			printf("|-");
		}
		printf("%d", arbre->valeur);
		
		// Etape 2 - appel récursif avec sous-arbre gauche
			// Si à gauche (et uniquement à gauche) c'est NULL on affiche "|-x"
			if( !est_feuille(arbre) && (arbre->fils_gauche == NULL) )
			{
				printf("\n");
				afficher_offset(offset+1);
				printf("|-x");
			}
		affichage_arborescence(arbre->fils_gauche, offset+1);
		
		
		// Etape 2 - appel récursif avec sous-arbre de droite
		
			// Si à droite (et uniquement à droite) c'est NULL on affiche "|-x"
			if( !est_feuille(arbre) && (arbre->fils_droite == NULL) )
			{
				printf("\n");
				afficher_offset(offset+1);
				printf("|-x");
			}
		affichage_arborescence(arbre->fils_droite, offset+1);
	}
	//else <=> arrêt de la récursivité
}*/

int hauteur_arbre(noeud const * racine) //calcule la hauteur maximale d'un arbre passé en paramètre
{
	if(est_feuille(racine)) return 1; //si c'est un feuille arrêt du récursif et renvoie 1
	if(hauteur_arbre(racine->fils_G)>hauteur_arbre(racine->fils_D)) return hauteur_arbre(racine->fils_G)+1; //test si la partie gauche est plus haute que la partie droite
	else return hauteur_arbre(racine->fils_D)+1; //sinon
}

int nb_feuilles(noeud const * racine) //calcule le nombre de feuilles d'un arbre passé en paramètre
{
	if(est_feuille(racine)) return 1; //si c'est un feuille arrêt du récursif et renvoie 1
	return nb_feuilles(racine->fils_G)+nb_feuilles(racine->fils_D); //appel récursif
}


void lanceur() //fonction d'appel du programme
{
	printf("Bienvenue, veuillez indiquer le ficher contenant les donnees d'apprentisages\n");
	char* file=malloc(sizeof(char)*40); //création du tableau contenant le chemin vers les données d'aprentissage
	scanf("%s",file);
	matrice_donnees * data=charger_donnnees(file); //chargemant des données d'apprentissage
	free(file); //libération du chemin d'accès
	if(data!=NULL) //si on a pu char gé les données
	{
		int valY=-1; //initialisation des variables de l'arbre
		int h_max=2*(data->nb_colonnes-1);
		int i_min=data->nb_lignes/10;
		double propor_min=0.1;
		double propor_max=0.9;
		bool max_flag;
		bool min_flag;
		printf("Veuillez entrez la valeur du parametre etudie (attentions c'est un entier)\n"); //demande des valeurs
		scanf("%d",&valY);
		printf("Veuillez entrez la hauteur maximal de l'arbre\n");
		scanf("%d",&h_max);
		printf("Veuillez entrez le nombre minimal d'individue par sous-echantillons\n");
		scanf("%d",&i_min);
		do //boucle pour les proportion maximale et minimale
		{
			printf("Veuillez entrez la proprotion minimale par sous-echantillons\n");
			scanf("%lg",&propor_min);
			printf("Veuillez entrez la proprotion maximale par sous-echantillons\n");
			scanf("%lg",&propor_max);
			max_flag=(propor_max>0 && propor_max<1);
			min_flag=(propor_min>0 && propor_min<1);
		} while(propor_max<propor_min || !(max_flag) || !(min_flag));
		
		noeud *racine=init_racine(data); //initialisation de la racine
		racine->valeur_etude=valY;
		generer_arbre(valY,h_max,0,propor_min,propor_max,i_min,racine); //création de l'arbre
		menu(racine);
	}
	else printf("ERREUR DE CHARGEMENT DES DONNEES, ARRET DU SYSTEME"); //message d'erreur
}

void menu(noeud * arbre)
{
	bool exit=false; //flag d'arrêt du programme
	double* ind_test=(double*) malloc(sizeof(double)*arbre->donnes->nb_colonnes);//création du tableau pour les individu à prédire
	do //boucle
	{
		printf("Les options sont les suivantes:\n"); //affichage du menu
		printf("1- Afficher la hauteur maximale de l'arbre\n");
		printf("2- Afficher le nombres de feuilles de l'arbre\n");
		printf("3- Affichage arborescent\n");
		printf("4- Affichage des feuilles\n");
		printf("5- Prediction d'un individu\n");
		printf("6- Quitter\n");
		printf("Que voullez vous faire?\n");
		int reponse=0; //initialise la variable réponse
		scanf("%d",&reponse);
		switch (reponse) //switch suivant réponse
		{
			case 1:
				printf("La hauteur maximale de l'arbre est: %d\n",hauteur_arbre(arbre)); //affiche la hauteur max de l'arbre
				break;
			case 2:
				printf("Le nombre de feuilles de l'arbre est: %d\n",nb_feuilles(arbre));// affiche le nombre de feuilles de l'arbre
				break;
			case 3:
				affichage_arborescence_bis(arbre,0); //affichage arborescent de l'arbre
				break;
			case 4:
				affichage_feuilles(arbre); //affichage des feuilles de l'arbre
				break;
			case 5:
				for(int i=1;i<arbre->donnes->nb_colonnes;i++)
				{
					printf("Veuillez entrez les caracteristique de l'individu\n"); //saisie de l'individu
					scanf("%lg",&ind_test[i]);
				}
				prediction(arbre,ind_test); //test de l'individu
				break;
			case 6:
				exit=true; //sortie du programme
				break;
			default: //cas autres
				printf("Reponse invalide, veuillez recommencer\n"); 
				break;
		}
		printf("\n");
	} while(!exit);
}

void affichage_arborescence_bis(noeud * arbre, int offset) //affichage sous forme arborescente del'arbre de décision
{
	if(arbre != NULL)
	{
		// Etape 1 - afficher la valeur
		printf("\n");
		afficher_offset(offset);
		if( offset != 0 ) // tous les éléments sauf la racine
		{
			double propor=proportion(arbre,arbre->valeur_etude);
			if(arbre->donnes->matrice[arbre->individu[0]][(int) arbre->parent->med[0]]<=arbre->parent->med[1]) printf("|-Precision de l'echantillon : %.2lf, chemin depuis le parent: X%lg<=%lg",propor,arbre->parent->med[0],arbre->parent->med[1]);
			else printf("|-Precision de l'echantillon : %.2lf, chemin depuis le parent: X%lg>%lg",propor,arbre->parent->med[0],arbre->parent->med[1]);
		}
		else
		{
			double propor=proportion(arbre,arbre->valeur_etude);
			printf("Precision de l'echantillon : %.2lf",propor);
		}
		// Etape 2 - appel récursif avec sous-arbre gauche
			// Si à gauche (et uniquement à gauche) c'est NULL on affiche "|-x"
			if( !est_feuille(arbre) && (arbre->fils_G == NULL) )
			{
				printf("\n");
				afficher_offset(offset+1);
				printf("|-x");
			}
		affichage_arborescence_bis(arbre->fils_G, offset+1);
		
		
		// Etape 2 - appel récursif avec sous-arbre de droite
		
			// Si à droite (et uniquement à droite) c'est NULL on affiche "|-x"
			if( !est_feuille(arbre) && (arbre->fils_D == NULL) )
			{
				printf("\n");
				afficher_offset(offset+1);
				printf("|-x");
			}
		affichage_arborescence_bis(arbre->fils_D, offset+1);
		
	}
	//else <=> arrêt de la récursivité
}

double proportion(noeud * branche,int Y) //calcul la proportion d'un paramètre Y parmis les individus d'un noeud
{
	double compt=0.0;
	for(int i=0;i<branche->nb_individu;i++)
	{
		if(branche->donnes->matrice[branche->individu[i]][0]==Y) compt++;
	}
	return compt/(double) branche->nb_individu;
}

void affichage_feuilles(noeud * arbre) //affichage des feuilles de l'arbre (attention le chemin d'accès ce lit de droite à gauche)
{
	if(!est_feuille(arbre)) //si ce n'est pas une feuille
	{
		affichage_feuilles(arbre->fils_G); //appelle récursif jusqu'aux feuilles
		affichage_feuilles(arbre->fils_D);
	}
	else
	{
		printf("\n");
		printf("|-Precision de la feuille: %.2lf, nombre d'element dans le sous echantillon: %d",proportion(arbre,arbre->valeur_etude),arbre->nb_individu);
		printf("\n");
		afficher_offset(1);
		chemin_acces(arbre); //appel du chemin d'accès
	}
}

void afficher_tableau(double tableau[], int taille) //permet d'afficher un tableau de double
{
	for(int i=0;i<taille;i++)
	{
		printf("%lg ",tableau[i]);
	}
}

void afficher_tableau_int(int* tableau, int taille) //permet d'afficher un tableau d'entier
{
	for(int i=0;i<taille;i++)
	{
		printf("%d ",tableau[i]);
	}
}

void chemin_acces(noeud * feuille) //affiche le chemin d'accès depuis la racine de l'arbre jusqu'à la feuille passé en paramètre (attention le chemin d'accès ce lit de droite à gauche)
{
	while(feuille->parent->parent!=NULL) //tant que l'on n'est pas à la racine
	{
		if(feuille->donnes->matrice[feuille->individu[0]][(int) feuille->parent->med[0]]<=feuille->parent->med[1]) printf("X%lg<=%lg <- ",feuille->parent->med[0],feuille->parent->med[1]); //affiche les choix de la feuille jusqu'a la racine
		else printf("X%lg>%lg <- ",feuille->parent->med[0],feuille->parent->med[1]);
		feuille=feuille->parent; //remonte d'un niveau
	}
	if(feuille->donnes->matrice[feuille->individu[0]][(int) feuille->parent->med[0]]<=feuille->parent->med[1]) printf("X%lg<=%lg",feuille->parent->med[0],feuille->parent->med[1]); //affichage pour la racine
	else printf("X%lg>%lg",feuille->parent->med[0],feuille->parent->med[1]);
}

void prediction(noeud * arbre,double* ind) //méthode perméttant de déduire l'appartenace d'un individu au groupe de l'arbre
{
	while(!est_feuille(arbre)) //tant que l'on n'est pas sur un feuille
	{
		if(ind[(int) arbre->med[0]]<=arbre->med[1]) arbre=arbre->fils_G; //parcour de l'arbre
		else arbre=arbre->fils_D;
	}
	double p=proportion(arbre,arbre->valeur_etude);
	printf("l'individu se trouve dans un groupe avec %.2lf pourcent de type de l'arbre",p*100); //affichage du résultat
}





