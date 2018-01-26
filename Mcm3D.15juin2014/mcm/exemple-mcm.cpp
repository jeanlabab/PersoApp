// Compilation : mpicxx exemple-mcm.cpp mcm.cpp -lgsl -lgslcblas -lm
// Execution : mpiexec -n 3 a.out

#include "mpi.h"
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include "mcm.h"

/*****************************************************************/
/* Calcul de $A = \int_0^1 f(x) dx$ pour $f(x)=x$.               */
/* On utilise une variable aléatoire uniforme $X$ sur $[0,1]$,   */
/* c'est à dire vérifiant $p_X(x)=1$.                            */
/* Le poids est alors $W=f(X)/p_X(X)=X$ et $A = E(W)$.           */
/*****************************************************************/

#define MCM_nombreDeGrandeurs 1 // nombre de grandeurs évaluées simultannément
#define MCM_nombreDeParametres 0 // nombre paramètres (on évalue la sensibilité de chaque grandeur à chaque paramètre)

class mon_algorithme : public mcmMonteCarlo {
public:
  mon_algorithme() : mcmMonteCarlo(MCM_nombreDeGrandeurs, MCM_nombreDeParametres) {}
protected:
  mcmVecteurSommeDeRealisations realisation() const {
    mcmVecteurSommeDeRealisations vsr(dimension);  // le VecteurSommeDeRealisations renvoyé par realisation() 
    vsr.nombreDeRealisations = 1;  // le nombre de réalisations effectuées à chaque appel à realisation() : 1 dans la plupart des cas

    double x;
    double poids;
    x=mcmRng();
    poids=x;

    vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1),poids);

    return vsr; 
  }
};

int main(int argc,char *argv[]) {

  mcmInit();
 
  mon_algorithme algo;
  algo.lectureDuFichierEntree();
  algo.lancement();
  algo.ecritureDuFichierDeSortie();

  mcmCleanup();

  return 0;

}
