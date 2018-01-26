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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <mpi.h>

void mcmInit();
void mcmCleanup();
double mcmRng();
bool mcmIsProcessus0();
bool mcmIsProcessus1();
int mcmNombreDeProcessus();

// Declaration de la classe mcmEstimation
class mcmEstimation {
 private:
  double estimation;
  double incertitude;
 public:
  mcmEstimation();
  mcmEstimation(const double &est, const double &inc);
  mcmEstimation(FILE *fichier);
  mcmEstimation(const double &sommeDesRealisations, const double &sommeDesCarres, const long int &nombreDeRealisations) {
    set(sommeDesRealisations, sommeDesCarres, nombreDeRealisations);
  }
  void set(const double &sommeDesRealisations, const double &sommeDesCarres, const long int &nombreDeRealisations) {
    estimation = sommeDesRealisations/nombreDeRealisations;
    incertitude = sqrt((sommeDesCarres/nombreDeRealisations-estimation*estimation)/nombreDeRealisations );
  }
  double getEstimation() const {
    return estimation;
  }
  double getIncertitude() const {
    return incertitude;
  }
  void ecriture() const;
  void ecriture(FILE *fichier) const;
  void lecture(FILE *fichier);
};

// Declarations de la classe mcmVecteurSommeDeRealisations
class mcmVecteurSommeDeRealisations {
 public:
  int dimension;
  long nombreDeRealisations;
  double *sommeDesRealisations;
  double *sommeDesCarres;
  mcmVecteurSommeDeRealisations();
  mcmVecteurSommeDeRealisations(const int &dim);
  mcmVecteurSommeDeRealisations(const mcmVecteurSommeDeRealisations &vsr); // constructeur de copie (necessaire car MPI l'utilise probablement)
  ~mcmVecteurSommeDeRealisations(); // destructeur (necessaire car nous avons alloue dynamiquement de la memoire)
  mcmVecteurSommeDeRealisations &operator+=(const mcmVecteurSommeDeRealisations &vsr);
  void operator=(const mcmVecteurSommeDeRealisations &vsr); // operateur d'affectation (necessaire car MPI l'utilise probablement)
  mcmEstimation estimationEsperanceUneComposante(const int &i) const;
  void initialisationUneComposante(const int &i, const mcmEstimation &est, const long int &nr); // a partir d'une mcmEstimation
  void initialisationUneComposante(const int &i, const double &poids); // a partir d'une seule realisation
  void initialisationUneComposante(const int &i, const double &sommeDesPoids, const double &sommeDesCarresDesPoids, const long &nr); // a partir de nr relalisations
};

// Declarations de la classe mcmMonteCarlo
class mcmMonteCarlo : public mcmVecteurSommeDeRealisations {
 public:
  mcmMonteCarlo();
  mcmMonteCarlo(const int &nbv);
  mcmMonteCarlo(const int &nbv, const int &nbp);
  void lancement(const long &ntir);
  void lancement();
  void ecriture() const;
  void ecriture(FILE *fichier) const;
  void lecture(FILE *fichier);
  void ecritureDuFichierDeSortie() const;
  void lectureDuFichierEntree();
 protected:
  int nbDeVariablesAleatoires;
  int nbDeParametres;
  long nombreDeRealisationsDemandees;
  int indiceDeLaCoordonneeDuVecteurSommeDeRealisations(const int &indiceDeLaVariableAleatoire) const;
  int indiceDeLaCoordonneeDuVecteurSommeDeRealisations(const int &indiceDeLaVariableAleatoire, const int &indiceDuParametre) const;
  virtual mcmVecteurSommeDeRealisations realisation() const =0;
};
