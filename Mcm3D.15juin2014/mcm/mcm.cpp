/*
  Copyright 2010, 2010 STARWest

  See the file STARWest.txt for details about the STARWest research group

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mcm.h"

#define MCM_nom_fichier_entree "mcm.in"
#define MCM_nom_fichier_sortie "mcm.out"
#define MCM_nombre_de_chiffres_de_int 6
#define MCM_nombre_de_chiffres_de_long_int 12
#define MCM_nombre_de_cycles_suplementaires 100 // nombre de cycles a rajouter lorsqu'un processus doit etendre son fichier de germes
#define MCM_gsl_rng gsl_rng_ranlxd2 // nom du generateur choisi dans la gsl (verifier que le fichiers de germe - recopie sous un nouveau nom pour chaque processus - a bien ete construit avec ce generateur et sinon effacer le fichier de germe de chaque processus de facon a ce que chaque processus le reconstruise) 
gsl_rng *MCM_r = gsl_rng_alloc(MCM_gsl_rng);

int MCM_erreur;
int MCM_n_processus; // nombre de processus
int MCM_i_processus; // numero du processus (entre 0 et MCM_n_processus-1)
int MCM_n_send; // nombre de donnees envoyees lors d'un send
int MCM_dest; // numero du processus destinataire lors d'un send
int MCM_tag; // indentificateur du message lors d'un send
int MCM_source; // numero du processus emetteur lors d'un recv
MPI::Status MCM_stat; // status lors d'un recv
int MCM_exchanged_int; // entier utilise pour les echanges de signaux

FILE *MCM_fichier_entree;
FILE *MCM_fichier_sortie;
char *MCM_nom_fichier_germes = (char *) malloc(((3*MCM_nombre_de_chiffres_de_long_int)+11+19+3+1)*sizeof(char));
FILE *MCM_fichier_germes;

long MCM_n_cycle_max; // nombre maximum de cycles de generations aleatoires
long MCM_n_indep; // nombre de generations independantes avant de changer de cycle
long MCM_n_cycle; // nombre de cycles ayant deja ete employes
long MCM_numero_cycle; // numero du cycle en cours pour le processus (entre 1 et MCM_n_cycle_max)
long MCM_numero_generation; // numero de la derniere generation faite a l'interieur du cycle 
int MCM_n_processus_rng; // nombre de processus non termines (du point de vue du processus 0)

char *mcmEntierVersChaine(const long int &entier) {
  // permet de traduire un long int en chaine de caractere
  char *chaine;
  chaine = (char *) malloc(MCM_nombre_de_chiffres_de_long_int*sizeof(char));
  sprintf(chaine, "%li", entier);
  return chaine;
}

void mcmDeterminationDuNomDeFichierDeGermes() {
  // permet de determiner le nom du fichier de germes a partir de MCM_n_cycle_max et MCM_n_indep
  char *chaine;
  chaine = mcmEntierVersChaine(MCM_n_cycle_max);
  strcpy(MCM_nom_fichier_germes,chaine);
  strcat(MCM_nom_fichier_germes,"_cycles_de_");
  chaine = mcmEntierVersChaine(MCM_n_indep);
  strcat(MCM_nom_fichier_germes,chaine);
  strcat(MCM_nom_fichier_germes,"_pour_le_processus_");
  chaine = mcmEntierVersChaine((long int)MCM_i_processus);
  strcat(MCM_nom_fichier_germes,chaine);
  strcat(MCM_nom_fichier_germes,".in");
}

void mcmEcritureNouveauFichierDeGermes() {
  // production d'un nouveau fichier de germes quand
  //  - le fichier de germes correspondant a MCM_n_cycle_max et MCM_n_indep n'existe pas a l'initialisation
  //  - le nombre de cycles necessaire est plus grand que celui correspondant au fichier disponible
  // Dans le second cas, on ne recalcule que les germes correspondant aux cycles supplementaires
  long int litmp1;
  double tmp1;
  bool fichierNonExistant;
  FILE *ancienFichier;
  long int ancienNombreDeCycles;
  fichierNonExistant = (MCM_fichier_germes == NULL);
  if (fichierNonExistant) {
    printf("----------\n");
    printf("ALLERTE !\n");
    printf("\n");
    printf("\n");
    printf("ALLERTE ! Le fichier de germes %s n'est pas present dans le repertoire du processus %d\n",MCM_nom_fichier_germes,MCM_i_processus);
    printf("Le processus %d va devoir generer la sequence des %li*%li nombres aleatoires necessaires a la constitution de ce fichier\n",MCM_i_processus,MCM_n_cycle_max,MCM_n_indep);
    printf("\n");
    printf("\n");
    printf("ALLERTE !\n");
    printf("----------\n");
    fflush(stdout);
    MCM_fichier_germes = fopen(MCM_nom_fichier_germes, "w");
    gsl_rng_fwrite(MCM_fichier_germes,MCM_r);
    for (long int i=0; i<MCM_n_cycle_max-1; i++) {
      for (long int j=0; j<MCM_n_indep; j++) {
	tmp1=gsl_rng_uniform_pos(MCM_r);
      }
      gsl_rng_fwrite(MCM_fichier_germes,MCM_r);
    }
    fclose(MCM_fichier_germes);
  } else {
    printf("----------\n");
    printf("ALLERTE !\n");
    printf("\n");
    printf("\n");
    printf("ALLERTE ! Le fichier de germes n'est pas de taille suffisante pour le processus %d\n",MCM_i_processus);
    litmp1 = MCM_nombre_de_cycles_suplementaires;
    printf("Le processus %d va devoir generer une sequence de %li*%li nombres aleatoires supplementaires pour completer son fichier de germes\n",MCM_i_processus,litmp1,MCM_n_indep);
    printf("\n");
    printf("\n");
    printf("ALLERTE !\n");
    printf("----------\n");
    fflush(stdout);
    ancienFichier = fopen(MCM_nom_fichier_germes, "r");
    ancienNombreDeCycles = MCM_n_cycle_max;
    MCM_n_cycle_max = MCM_n_cycle_max + MCM_nombre_de_cycles_suplementaires;
    mcmDeterminationDuNomDeFichierDeGermes();
    MCM_fichier_germes = fopen(MCM_nom_fichier_germes, "w");
    for (long int i=0; i<ancienNombreDeCycles; i++) {
      MCM_erreur = gsl_rng_fread(ancienFichier,MCM_r);
      gsl_rng_fwrite(MCM_fichier_germes,MCM_r);
    }
    for (long int i=0; i<MCM_nombre_de_cycles_suplementaires; i++) {
      for (long int j=0; j<MCM_n_indep; j++) {
	tmp1=gsl_rng_uniform_pos(MCM_r);
      }
      gsl_rng_fwrite(MCM_fichier_germes,MCM_r);
    }
    fflush(MCM_fichier_germes);
    fclose(MCM_fichier_germes);
  }
}

void mcmLectureEnteteDuFichierEntree() {
  // lecture des parametres descriptifs du generateur a l'initialisation (entete du fichier d'entree)
  if (MCM_i_processus == 1) {
    int itmp1;
    itmp1 = fscanf(MCM_fichier_entree,"Nombre maximum de cycles de generations aleatoires : %li\n",&MCM_n_cycle_max);
    itmp1 = fscanf(MCM_fichier_entree,"Nombre de generations independantes avant de changer de cycle : %li\n",&MCM_n_indep);
    itmp1 = fscanf(MCM_fichier_entree,"Nombre de cycles ayant deja ete employes : %li\n",&MCM_n_cycle);
  }
}

void mcmEcritureFinDuFichierSortie() {
  // ecriture des parametres descriptifs du generateur en fin de calcul (fin du fichier de sortie)
  if (MCM_i_processus == 1) {
    fprintf(MCM_fichier_sortie,"Nombre maximum de cycles de generations aleatoires : %li\n",MCM_n_cycle_max);
    fprintf(MCM_fichier_sortie,"Nombre de generations independantes avant de changer de cycle : %li\n",MCM_n_indep);
    fprintf(MCM_fichier_sortie,"Nombre de cycles ayant deja ete employes : %li\n",MCM_n_cycle);
  }
}

void mcmInit() {
  // initialisation de MPI
  // initialisation de la generation aleatoire sur plusieurs processeurs a partir du fichier d'entree
  // ouverture du fichier de sortie
  int argc;
  char **argv;
  printf("Initialisation MCM\n");
  fflush(stdout);

  //-----------------
  // lancement de MPI
  //-----------------

  MPI::Init();
  MCM_n_processus = MPI::COMM_WORLD.Get_size();
  MCM_i_processus = MPI::COMM_WORLD.Get_rank();
  if (MCM_n_processus >= 3) {

    //-------------------------------------------------------------------
    // Ouverture du fichier d'entree et lecture de l'entete (processus 1)
    //-------------------------------------------------------------------

    if (MCM_i_processus == 1) {
      MCM_fichier_entree = fopen(MCM_nom_fichier_entree, "r");
      if (MCM_fichier_entree == NULL) {
	printf("ERREUR ! Le fichier d'entree %s n'est pas present dans le repertoire du processus 1\n",MCM_nom_fichier_entree);
	fflush(stdout);
	MCM_erreur = 102;
	MPI::COMM_WORLD.Abort(MCM_erreur);
      }
      mcmLectureEnteteDuFichierEntree();
      // envoi des caracteristiques du generateur aleatoire aux autres processus (processus 1)
      MCM_n_send = 1; MCM_tag = 1;
      for (MCM_dest=0; MCM_dest<MCM_n_processus; MCM_dest++) {
	if (MCM_dest != 1) {
	  MPI::COMM_WORLD.Send(&MCM_n_cycle_max,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
	  MPI::COMM_WORLD.Send(&MCM_n_indep,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
	  MPI::COMM_WORLD.Send(&MCM_n_cycle,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
	}
      }
    } else {
      // reception des caracteristiques du generateur aleatoire envoyees par le processus 1
      MCM_source = 1; MCM_n_send = 1; MCM_tag = 1;
      MPI::COMM_WORLD.Recv(&MCM_n_cycle_max,MCM_n_send,MPI::LONG,MCM_source,MPI::ANY_TAG,MCM_stat);
      MPI::COMM_WORLD.Recv(&MCM_n_indep,MCM_n_send,MPI::LONG,MCM_source,MPI::ANY_TAG,MCM_stat);
      MPI::COMM_WORLD.Recv(&MCM_n_cycle,MCM_n_send,MPI::LONG,MCM_source,MPI::ANY_TAG,MCM_stat);
    }
    if (MCM_i_processus != 0) {
      mcmDeterminationDuNomDeFichierDeGermes();
      MCM_fichier_germes = fopen(MCM_nom_fichier_germes, "r");
      if (MCM_fichier_germes == NULL) {
	mcmEcritureNouveauFichierDeGermes();
	MCM_fichier_germes = fopen(MCM_nom_fichier_germes, "r");
      }
    }

    //----------------------------------------------------------
    // processus 0 dedie a la gestion des generations aleatoires
    //----------------------------------------------------------

    if (MCM_i_processus == 0) {
      // attribution d'un numero de cycle a chaque processus
      MCM_n_processus_rng = 0;
      for (MCM_dest=1; MCM_dest<MCM_n_processus; MCM_dest++) {
	MCM_n_processus_rng = MCM_n_processus_rng+1;
	MCM_n_cycle = MCM_n_cycle+1;
	// printf ("processus 0 : envoi du numero de cycle %li au processus %d\n",MCM_n_cycle,MCM_dest);
	// fflush(stdout);
	MCM_n_send = 1; MCM_tag = 1;
	MPI::COMM_WORLD.Send(&MCM_n_cycle,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
      }
      // attente des signaux des processus (demande d'un nouveau cycle ou signal de fin)
      while (MCM_n_processus_rng != 0) {
	MCM_n_send = 1;
	MPI::COMM_WORLD.Recv(&MCM_exchanged_int,MCM_n_send,MPI::INT,MPI::ANY_SOURCE,MPI::ANY_TAG,MCM_stat);
	// printf ("processus 0 : recu signal de %d avec tag %d\n",MCM_stat.Get_source(),MCM_stat.Get_tag());
	// fflush(stdout);
	if (MCM_stat.Get_tag() == 1) { // demande d'un nouveau cycle
	  MCM_n_cycle = MCM_n_cycle+1;
	  MCM_n_send = 1; MCM_dest = MCM_stat.Get_source(); MCM_tag = 1;
	  // printf ("processus 0 : envoi du numero de cycle %li au processus %d\n",MCM_n_cycle,MCM_dest);
	  // fflush(stdout);
	  MPI::COMM_WORLD.Send(&MCM_n_cycle,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
	} else { // fin du processus
	  MCM_n_processus_rng = MCM_n_processus_rng-1;
	  printf ("processus 0 : nombre de processus non termines : %d\n",MCM_n_processus_rng);
	  fflush(stdout);
	}
      }
    }

    //----------------------------------------
    // Reception des premiers numeros de cycle
    //----------------------------------------

    if (MCM_i_processus != 0) {
      MCM_source = 0; MCM_n_send = 1; MCM_tag = 1;
      MPI::COMM_WORLD.Recv(&MCM_numero_cycle,MCM_n_send,MPI::LONG,MCM_source,MPI::ANY_TAG,MCM_stat);
      // printf ("processus %d : recu le numero de cycle %li\n",MCM_i_processus,MCM_numero_cycle);
      // fflush(stdout);
      if (MCM_numero_cycle > MCM_n_cycle_max) {
	fclose(MCM_fichier_germes);
	mcmEcritureNouveauFichierDeGermes();
	MCM_fichier_germes = fopen(MCM_nom_fichier_germes, "r");
      }
      for (long int i = 1; i <= MCM_numero_cycle; i++) {
	MCM_erreur = gsl_rng_fread(MCM_fichier_germes,MCM_r);
      }
      MCM_numero_generation = 0;
    }

    //---------------------------------------------
    // Ouverture du fichier de sortie (processus 1)
    //---------------------------------------------

    if (MCM_i_processus == 1) {
      MCM_fichier_sortie = fopen(MCM_nom_fichier_sortie, "w");
    }
  }
}

void mcmCleanup() {
  // cloture de la generation aleatoire sur plusieurs processeurs
  // ecriture du nouvel etat du generateur a la fin du fichier de sortie
  // cloture de MPI
  if (MCM_n_processus >= 3) {
    printf("Cloture MCM\n");
    fflush(stdout);

    //--------------------------------------
    // envoi du signal de fin au processus 0
    //--------------------------------------

    if (MCM_i_processus != 0) {
      MCM_exchanged_int = 0;
      MCM_tag = 0; MCM_dest = 0; MCM_n_send = 1;
      printf ("processus %d : envoi du signal de fin au processus 0\n",MCM_i_processus);
      fflush(stdout);
      MPI::COMM_WORLD.Send(&MCM_exchanged_int,MCM_n_send,MPI::INT,MCM_dest,MCM_tag);
    }

    //---------------------------------------------------------------
    // le processus 0 envoie les donnees du generateur au processus 1
    //---------------------------------------------------------------

    if (MCM_i_processus == 0) {
      MCM_n_send = 1; MCM_dest = 1; MCM_tag = 2;
      MPI::COMM_WORLD.Send(&MCM_n_cycle_max,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
      MPI::COMM_WORLD.Send(&MCM_n_indep,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
      MPI::COMM_WORLD.Send(&MCM_n_cycle,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
    }
    if (MCM_i_processus == 1) {
      MCM_n_send = 1; MCM_source = 0; MCM_tag = 2;
      MPI::COMM_WORLD.Recv(&MCM_n_cycle_max,MCM_n_send,MPI::LONG,MCM_source,MCM_tag,MCM_stat);
      MPI::COMM_WORLD.Recv(&MCM_n_indep,MCM_n_send,MPI::LONG,MCM_source,MCM_tag,MCM_stat);
      MPI::COMM_WORLD.Recv(&MCM_n_cycle,MCM_n_send,MPI::LONG,MCM_source,MCM_tag,MCM_stat);
    }
   
    //-------------------------------------------------------------------
    // ecriture de la fin du fichier de sortie et fermeture (processus 1)
    //-------------------------------------------------------------------

    mcmEcritureFinDuFichierSortie();
    if (MCM_i_processus == 1) {
      fclose(MCM_fichier_entree);
      fclose(MCM_fichier_sortie);
    }
  }

  //-----------
  // fin de MPI
  //-----------

  MPI::Finalize();
}

double mcmRng() {
  // generation aleatoire (et gestion des germes)
  if (MCM_i_processus == 0) {
    printf ("ERREUR ! Le processus 0 ne doit pas utiliser mcmRng\n");
    fflush(stdout);
    MCM_erreur = 106;
    MPI::COMM_WORLD.Abort(MCM_erreur);
  }

  //----------------------------------------------------------------------
  // generation aleatoire uniforme sur [0,1] en double precision
  // en assurant l'independance des generations aleatoires effectuees
  // par les differents processus (sans limite sur le nombre de processus)
  //----------------------------------------------------------------------

  long int numero_cycle_old;
  if (MCM_numero_generation == MCM_n_indep) {
    // printf ("processus %d : demande d'un nouveau numero de cycle\n",MCM_i_processus);
    // fflush(stdout);
    numero_cycle_old = MCM_numero_cycle;
    MCM_exchanged_int = 0;
    MCM_tag = 1; MCM_dest = 0; MCM_n_send = 1;
    MPI::COMM_WORLD.Send(&MCM_exchanged_int,MCM_n_send,MPI::INT,MCM_dest,MCM_tag);
    MCM_source = 0; MCM_n_send = 1;
    MPI::COMM_WORLD.Recv(&MCM_numero_cycle,MCM_n_send,MPI::LONG,MCM_source,MPI::ANY_TAG,MCM_stat);
    // printf ("dans mcmRng : processus %d : recu le numero de cycle %li\n",MCM_i_processus,MCM_numero_cycle);
    // fflush(stdout);
    if (MCM_numero_cycle > MCM_n_cycle_max) {
      fclose(MCM_fichier_germes);
      mcmEcritureNouveauFichierDeGermes();
      MCM_fichier_germes = fopen(MCM_nom_fichier_germes, "r");
    }
    for (long int i = numero_cycle_old+1; i <= MCM_numero_cycle; i++) {
      MCM_erreur = gsl_rng_fread (MCM_fichier_germes,MCM_r);
    }
    MCM_numero_generation = 0;
  }
  MCM_numero_generation = MCM_numero_generation+1;
  return gsl_rng_uniform_pos(MCM_r);
}

bool mcmIsProcessus0() {
  return (MCM_i_processus == 0);
}

bool mcmIsProcessus1() {
  return (MCM_i_processus == 1);
}

int mcmNombreDeProcessus() {
  return MCM_n_processus;
}

//---------------------
// Classe mcmEstimation
//---------------------

// Declaration dans mcm.h

// Methodes
mcmEstimation::mcmEstimation() {
  estimation=0.;
  incertitude=0.;
}
mcmEstimation::mcmEstimation(const double &est, const double &inc) {
  estimation=est;
  incertitude=inc;
}
mcmEstimation::mcmEstimation(FILE *fichier) {
  lecture(fichier);
}
void mcmEstimation::ecriture() const {
  printf("%lg [incertitude: %lg]\n",estimation,incertitude);
}
void mcmEstimation::ecriture(FILE *fichier) const {
  fprintf(fichier,"%lg [incertitude: %lg]\n",estimation,incertitude);
}
void mcmEstimation::lecture(FILE *fichier) {
  int nbLus = fscanf(fichier,"%lg [incertitude: %lg]\n",&estimation,&incertitude);
  if (nbLus != 2) {
    fprintf(stderr,"ERREUR ! Le fichier lu par mcmEstimation n'est pas au bon format\n");
    fflush(stderr);
    MCM_erreur = 110;
    MPI::COMM_WORLD.Abort(MCM_erreur);
  }
}

//-------------------------------------
// Classe mcmVecteurSommeDeRealisations
//-------------------------------------

// Declaration dans mcm.h

// Methodes
mcmVecteurSommeDeRealisations::mcmVecteurSommeDeRealisations() {
  dimension = 1;
  nombreDeRealisations = 0;
  sommeDesRealisations = new double[dimension];
  sommeDesCarres = new double[dimension];
  sommeDesRealisations[0] = 0.;
  sommeDesCarres[0] = 0.;
}
mcmVecteurSommeDeRealisations::mcmVecteurSommeDeRealisations(const int &dim) {
  dimension = dim;
  nombreDeRealisations= 0 ;
  sommeDesRealisations = new double[dimension];
  sommeDesCarres = new double[dimension];
  for (int i=0; i<dimension; i++) {
    sommeDesRealisations[i] = 0.;
    sommeDesCarres[i] = 0.;
  }
}
mcmVecteurSommeDeRealisations::mcmVecteurSommeDeRealisations(const mcmVecteurSommeDeRealisations &vsr) {
  dimension = vsr.dimension;
  sommeDesRealisations = new double[dimension];
  sommeDesCarres = new double[dimension];
  *this = vsr;
}
mcmVecteurSommeDeRealisations::~mcmVecteurSommeDeRealisations() {
  delete[] sommeDesRealisations;
  delete[] sommeDesCarres;
}
mcmVecteurSommeDeRealisations &mcmVecteurSommeDeRealisations::operator+=(const mcmVecteurSommeDeRealisations &vsr) {
  if (dimension == vsr.dimension) {
    nombreDeRealisations += vsr.nombreDeRealisations;
    for (int i=0; i<dimension; i++) {
      sommeDesRealisations[i] += vsr.sommeDesRealisations[i];
      sommeDesCarres[i] += vsr.sommeDesCarres[i];
    }
  }
  else {
    fprintf(stderr,"ERREUR ! Somme de deux mcmVecteurSommeDeRealisations de dimensions differentes\n");
    fflush(stderr);
    MCM_erreur = 108;
    MPI::COMM_WORLD.Abort(MCM_erreur);
  }
  return *this;
}
void mcmVecteurSommeDeRealisations::operator=(const mcmVecteurSommeDeRealisations &vsr) {
  if (dimension == vsr.dimension) {
    nombreDeRealisations = vsr.nombreDeRealisations;
    for (int i=0; i<dimension; i++) {
      sommeDesRealisations[i] = vsr.sommeDesRealisations[i];
      sommeDesCarres[i] = vsr.sommeDesCarres[i];
    }
  }
  else {
    fprintf(stderr,"ERREUR ! Affectation d'un mcmVecteurSommeDeRealisations avec un object de dimension incompatible avec sa dimension\n");
    fflush(stderr);
    MCM_erreur = 112;
    MPI::COMM_WORLD.Abort(MCM_erreur);
  }
}
mcmEstimation mcmVecteurSommeDeRealisations::estimationEsperanceUneComposante(const int &i) const {
  return mcmEstimation(sommeDesRealisations[i],sommeDesCarres[i],nombreDeRealisations);
}
void mcmVecteurSommeDeRealisations::initialisationUneComposante(const int &i, const mcmEstimation &est, const long int &nr) {
  if (nombreDeRealisations == nr) {
    sommeDesRealisations[i] = nr*est.getEstimation();
    sommeDesCarres[i] = nr*(nr*est.getIncertitude()*est.getIncertitude()+est.getEstimation()*est.getEstimation());
  } else {
    printf("ERREUR ! est.nombreDeRealisations n'a pas ete affecte correctement avant appel a initialisationUneComposante\n");
    fflush(stdout);
    MCM_erreur = 113;
    MPI::COMM_WORLD.Abort(MCM_erreur);
  }
}
void mcmVecteurSommeDeRealisations::initialisationUneComposante(const int &i, const double &poids) {
  if (nombreDeRealisations == 1) {
    sommeDesRealisations[i] = poids;
    sommeDesCarres[i] = poids*poids;
  } else {
    printf("ERREUR ! est.nombreDeRealisations n'a pas ete affecte correctement avant appel a initialisationUneComposante\n");
    fflush(stdout);
    MCM_erreur = 114;
    MPI::COMM_WORLD.Abort(MCM_erreur);
  }
}
void mcmVecteurSommeDeRealisations::initialisationUneComposante(const int &i, const double &sommeDesPoids, const double &sommeDesCarresDesPoids, const long &nr) {
  if (nombreDeRealisations == nr) {
    sommeDesRealisations[i] = sommeDesPoids;
    sommeDesCarres[i] = sommeDesCarresDesPoids;
  } else {
    printf("ERREUR ! est.nombreDeRealisations n'a pas ete affecte correctement avant appel a initialisationUneComposante\n");
    fflush(stdout);
    MCM_erreur = 115;
    MPI::COMM_WORLD.Abort(MCM_erreur);
  }
}

//---------------------
// Classe mcmMonteCarlo
//---------------------

// Declaration dans mcm.h

// Methodes
mcmMonteCarlo::mcmMonteCarlo() : mcmVecteurSommeDeRealisations() {
  nbDeVariablesAleatoires = 0;
  nbDeParametres = 0;
  nombreDeRealisationsDemandees = 0;
}
mcmMonteCarlo::mcmMonteCarlo(const int &nbv) : mcmVecteurSommeDeRealisations(nbv) {
  nbDeVariablesAleatoires = dimension;
  nbDeParametres = 0;
  nombreDeRealisationsDemandees = 0;
}
mcmMonteCarlo::mcmMonteCarlo(const int &nbv, const int &nbp) : mcmVecteurSommeDeRealisations(nbv*(nbp+1)) {
  nbDeVariablesAleatoires = nbv;
  nbDeParametres = nbp;
  nombreDeRealisationsDemandees = 0;
}
void mcmMonteCarlo::lancement(const long int &ntir) {
  int n_esclave=MCM_n_processus-2;
  long int ntir_esclave = ntir/n_esclave;
  if (MCM_i_processus == MCM_n_processus - 1) {
    ntir_esclave = ntir - ntir_esclave*(n_esclave-1);
  } 
  if (MCM_i_processus == 1)
  {
    int i_esclave = 0;
    mcmVecteurSommeDeRealisations vsr_esclave(dimension);
    while (i_esclave != n_esclave)
    {
      // attente du signal de debut d'envoi des resultats d'un esclave
      MCM_n_send = 1;
      MPI::COMM_WORLD.Recv(&vsr_esclave.nombreDeRealisations,MCM_n_send,MPI::LONG,MPI::ANY_SOURCE,MPI::ANY_TAG,MCM_stat);
      i_esclave=i_esclave+1;
      if (MCM_stat.Get_tag() != 0) // caclul esclave reussi
	{
	  // attente des resultats d'un esclave
	  MCM_source = MCM_stat.Get_source();
	  MCM_n_send = dimension;
	  MPI::COMM_WORLD.Recv(vsr_esclave.sommeDesRealisations,MCM_n_send,MPI::DOUBLE,MCM_source,MPI::ANY_TAG,MCM_stat);
	  MPI::COMM_WORLD.Recv(vsr_esclave.sommeDesCarres,MCM_n_send,MPI::DOUBLE,MCM_source,MPI::ANY_TAG,MCM_stat);
	  // addition au compteur total du maitre
	  *this+=vsr_esclave;
	}
      else // erreur sur le caclul esclave 
	{
	  printf ("processus 1 : erreur dans le processus %d donc pas de recuperation des resultats\n",MCM_stat.Get_source());
	  fflush(stdout);
	}
    }
  }
  if (MCM_i_processus > 1)
  {
    nombreDeRealisations = 0;
    while (nombreDeRealisations < ntir_esclave) { *this += realisation(); }
    // signal de debut d'envoi des resultats de l'esclave et transfert du nombre de realisations
    MCM_tag = 1; MCM_dest = 1; MCM_n_send = 1;
    MPI::COMM_WORLD.Send(&nombreDeRealisations,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
    // envoi des resultats de l'esclave
    MCM_tag = 1; MCM_dest = 1; MCM_n_send = dimension;
    MPI::COMM_WORLD.Send(sommeDesRealisations,MCM_n_send,MPI::DOUBLE,MCM_dest,MCM_tag);
    MPI::COMM_WORLD.Send(sommeDesCarres,MCM_n_send,MPI::DOUBLE,MCM_dest,MCM_tag);
  }
}
void mcmMonteCarlo::lancement() {
  if (MCM_i_processus == 1) {
    // envoi des nombres de realisations aux autres processus (processus 1)
    MCM_n_send = 1; MCM_tag = 1;
    for (MCM_dest=2; MCM_dest<MCM_n_processus; MCM_dest++) {
      MPI::COMM_WORLD.Send(&nombreDeRealisationsDemandees,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
      MPI::COMM_WORLD.Send(&nombreDeRealisations,MCM_n_send,MPI::LONG,MCM_dest,MCM_tag);
    }
  } else if (MCM_i_processus > 1) {
    // reception des nombres de realisations envoyes par le processus 1
    MCM_source = 1; MCM_n_send = 1; MCM_tag = 1;
    MPI::COMM_WORLD.Recv(&nombreDeRealisationsDemandees,MCM_n_send,MPI::LONG,MCM_source,MCM_tag,MCM_stat);
    MPI::COMM_WORLD.Recv(&nombreDeRealisations,MCM_n_send,MPI::LONG,MCM_source,MCM_tag,MCM_stat);
  }
  lancement(nombreDeRealisationsDemandees-nombreDeRealisations);
}
void mcmMonteCarlo::ecriture() const {
  if (MCM_i_processus > 0) {
    printf("Nombre de realisations demandees : %li\n",nombreDeRealisationsDemandees);
    printf("Nombre de realisations effectuees : %li\n",nombreDeRealisations);
    if (nombreDeRealisations !=0) {
      printf("Nombre de variables aleatoires : %d\n",nbDeVariablesAleatoires);
      printf("Nombre de parametres : %d\n",nbDeParametres);
      for (int i=1; i<=nbDeVariablesAleatoires; i++) {
	printf("Variable %d sur le processus %d : ",i,MCM_i_processus);    
	estimationEsperanceUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i)).ecriture();
	if (nbDeParametres == 1) {
	  printf("Sensibilite : ");
	  estimationEsperanceUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i,1)).ecriture();
	}
	if (nbDeParametres > 1) {
	  printf("Sensibilites :\n");	
	  for (int j=1; j<=nbDeParametres; j++) {
	    printf("  Parametre %d : ",j);    
	    estimationEsperanceUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i,j)).ecriture();
	  }
	}
      }
    }
    fflush(stdout);
  }
}
void mcmMonteCarlo::ecriture(FILE *fichier) const {
  if (MCM_i_processus > 0) {
    fprintf(fichier,"Nombre de realisations demandees : %li\n",nombreDeRealisationsDemandees);
    fprintf(fichier,"Nombre de realisations effectuees : %li\n",nombreDeRealisations);
    if (nombreDeRealisations !=0) {
      fprintf(fichier,"Nombre de variables aleatoires : %d\n",nbDeVariablesAleatoires);
      fprintf(fichier,"Nombre de parametres : %d\n",nbDeParametres);
      for (int i=1; i<=nbDeVariablesAleatoires; i++) {
	fprintf(fichier,"Variable %d sur le processus %d : ",i,MCM_i_processus);    
	estimationEsperanceUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i)).ecriture(fichier);
	if (nbDeParametres == 1) {
	  fprintf(fichier,"Sensibilite : ");
	  estimationEsperanceUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i,1)).ecriture(fichier);
	}
	if (nbDeParametres > 1) {
	  fprintf(fichier,"Sensibilites :\n");	
	  for (int j=1; j<=nbDeParametres; j++) {
	    fprintf(fichier,"  Parametre %d : ",j);    
	    estimationEsperanceUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i,j)).ecriture(fichier);
	  }
	}
      }
    }
    fflush(fichier);
  }
}
void mcmMonteCarlo::lecture(FILE *fichier) {
  int itmp1,itmp2;
  mcmEstimation est;
  if (MCM_i_processus > 0) {
    fscanf(fichier,"Nombre de realisations demandees : %li\n",&nombreDeRealisationsDemandees);
    fscanf(fichier,"Nombre de realisations effectuees : %li\n",&nombreDeRealisations);
    if (nombreDeRealisationsDemandees <= nombreDeRealisations) {
      printf("ERREUR ! Le nombre de realisations demandees est inferieur ou egal au nombre de realisations deja effectuees\n");
      fflush(stdout);
      MCM_erreur = 105;
      MPI::COMM_WORLD.Abort(MCM_erreur);    
    }
    if (nombreDeRealisations !=0) {
      fscanf(fichier,"Nombre de variables aleatoires : %d\n",&itmp1);
      if (itmp1 != nbDeVariablesAleatoires) {
	printf("ERREUR ! Le fichier n'est pas compatible avec le MonteCarlo utilise\n");
	fflush(stdout);
	MCM_erreur = 103;
	MPI::COMM_WORLD.Abort(MCM_erreur);
      }
      fscanf(fichier,"Nombre de parametres : %d\n",&itmp1);
      if (itmp1 != nbDeParametres) {
	printf("ERREUR ! Le fichier n'est pas compatible avec le MonteCarlo utilise\n");
	fflush(stdout);
	MCM_erreur = 104;
	MPI::COMM_WORLD.Abort(MCM_erreur);
      }
      for (int i=1; i<=nbDeVariablesAleatoires; i++) {
	fscanf(fichier,"Variable %d sur le processus %d : ",&itmp1,&itmp2);    
	est.lecture(fichier);
	initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i),est,nombreDeRealisations);
	if (nbDeParametres == 1) {
	  fscanf(fichier,"Sensibilite : ");
	  est.lecture(fichier);
	  initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i,1),est,nombreDeRealisations);
	}
	if (nbDeParametres > 1) {
	  fscanf(fichier,"Sensibilites :\n");	
	  for (int j=1; j<=nbDeParametres; j++) {
	    fscanf(fichier,"  Parametre %d : ",&itmp1);    
	    est.lecture(fichier);
	    initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(i,j),est,nombreDeRealisations);
	  }
	}
      }
    }
  }
}
void mcmMonteCarlo::ecritureDuFichierDeSortie() const {
  if (MCM_i_processus == 1) {
    ecriture(MCM_fichier_sortie);
  }
}
void mcmMonteCarlo::lectureDuFichierEntree() {
  if (MCM_i_processus == 1) {
    lecture(MCM_fichier_entree);
  }
}
int mcmMonteCarlo::indiceDeLaCoordonneeDuVecteurSommeDeRealisations(const int &indiceDeLaVariableAleatoire) const {
  return indiceDeLaVariableAleatoire-1;
}
int mcmMonteCarlo::indiceDeLaCoordonneeDuVecteurSommeDeRealisations(const int &indiceDeLaVariableAleatoire, const int &indiceDuParametre) const {
  return nbDeVariablesAleatoires+(indiceDeLaVariableAleatoire-1)*nbDeParametres+indiceDuParametre-1;
}

