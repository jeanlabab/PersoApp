// Compilation : mpicxx exemple-mcm-avec-sensibilite.cpp mcm.cpp -lgsl -lgslcblas -lm
// Execution : mpiexec -n 3 a.out

#include "mpi.h"
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include "mcm.h"

/*****************************************************************/
/* Calcul de $A_1 = \int_0^1 f(x) dx$ pour $f(x)=p_1 x^2$        */
/* et de $A_2 = \int_0^1 g(x) dx$ pour $g(x)=sin(p_2 x)$.        */
/* On utilise une variable aléatoire uniforme $X$ sur $[0,1]$,   */
/* c'est à dire vérifiant $p_X(x)=1$.                            */
/* Les poids sont alors $W_1=f(X)/p_X(X)=p_1 X^2$ et             */
/* $W_2=g(X)/p_X(X)=sin(p_2 X)$, avec $A_1=E(W_1)$ et            */
/* $A_2=E(W_2)$.                                                 */
/*                                                               */
/* On calcule également les sensibilités de $A_1$ et $A_2$ aux   */
/* paramètres $p_1$ et $p_2$.                                    */
/* Les poids correspondant aux sensibilités sont                 */
/* $W_{1,1}=X^2$, $W_{1,2}=0$, $W_{2,1}=0$ et                    */
/* $W_{2,2}=X cos(p_2 X)$, avec                                  */
/* $\frac{\partial A_1}{\partial p_1}=E(W_{1,1})$                */
/* $\frac{\partial A_1}{\partial p_2}=E(W_{1,2})$                */
/* $\frac{\partial A_2}{\partial p_1}=E(W_{2,1})$                */
/* $\frac{\partial A_2}{\partial p_2}=E(W_{2,2})$                */
/*                                                               */
/* Les solutions analytiques sont                                */
/* $A_1=p_1/3$,                                                  */
/* $A_2=(1-cos(p_2))/p_2$ (soit 0.63662 pour $p_2=\pi/2$),       */
/* $\frac{\partial A_1}{\partial p_1}=1/3$,                      */
/* $\frac{\partial A_1}{\partial p_2}=0$                         */
/* $\frac{\partial A_2}{\partial p_1}=0$                         */
/* et $\frac{\partial A_2}{\partial p_2}=                        */
/*            \frac{p_2 sin(p_2) - (1-cos(p_2))}{p_2^2}$         */
/* (soit $\frac{\partial A_2}{\partial p_2}=0.23133$ pour        */
/* $p_2=\pi/2$)                                                  */
/*****************************************************************/

#define MCM_nombreDeGrandeurs 2 // nombre de grandeurs évaluées simultannément
#define MCM_nombreDeParametres 2 // nombre paramètres (on évalue la sensibilité de chaque grandeur à chaque paramètre)

class mon_algorithme : public mcmMonteCarlo {
public:
  mon_algorithme() : mcmMonteCarlo(MCM_nombreDeGrandeurs, MCM_nombreDeParametres) {}
protected:
  mcmVecteurSommeDeRealisations realisation() const {
    mcmVecteurSommeDeRealisations vsr(dimension);  // le VecteurSommeDeRealisations renvoyé par realisation() 
    vsr.nombreDeRealisations = 1;  // le nombre de réalisations effectuées à chaque appel à realisation() : 1 dans la plupart des cas

    double p1 = 3.;
    double p2 = M_PI/2.;
    double x;
    double poidsGrandeur1; // poids pour la première grandeur
    double poidsSensibilite1Grandeur1; // poids pour la sensibilité au premier paramètre de la première grandeur
    double poidsSensibilite2Grandeur1; // poids pour la sensibilité au second paramètre de la première grandeur
    double poidsGrandeur2; // poids pour la seconde grandeur
    double poidsSensibilite1Grandeur2; // poids pour la sensibilité au premier paramètre de la seconde grandeur
    double poidsSensibilite2Grandeur2; // poids pour la sensibilité au second paramètre de la seconde grandeur

    x=mcmRng();
    poidsGrandeur1=p1*x*x;
    poidsSensibilite1Grandeur1=x*x;
    poidsSensibilite2Grandeur1=0;
    poidsGrandeur2=sin(p2*x);
    poidsSensibilite1Grandeur2=0;
    poidsSensibilite2Grandeur2=x*cos(p2*x);

    vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1),poidsGrandeur1);
    vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1,1),poidsSensibilite1Grandeur1);
    vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1,2),poidsSensibilite2Grandeur1);
    vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2),poidsGrandeur2);
    vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2,1),poidsSensibilite1Grandeur2);
    vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2,2),poidsSensibilite2Grandeur2);

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
