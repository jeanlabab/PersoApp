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
#ifndef MCM_METHODES_H
#define MCM_METHODES_H

void imprimerValArray(const std::string nom,
		const std::valarray<double> vecteur, bool isProc1) const {
	if (isProc1) {
		std::cout << "\n-------------------------";
		std::cout << "\n" << nom << std::endl;
		std::cout << "Indice\tValeur";
		for (unsigned int i = 0; i < vecteur.size(); ++i) {
			std::cout << "\n" << i << "\t" << vecteur[i];
		}
		std::cout << "\nfin de " << nom << std::endl;
		std::cout << "-------------------------\n";
	}
}

void imprimerValArray(const std::string nom,
		const std::valarray<double> vecteur) const {
	imprimerValArray(nom, vecteur, 0);
}

double interpolLin(const double valeurX, const std::valarray<double> x,
		const std::valarray<double> y, const bool imprimer) const {
	assert(x.size() == y.size()); //test tailles identiques

			//gestion d'une valeur en dehors du vecteur x
	if (valeurX < x[0]) {
		std::cout << "\n Interpolation hors valeurs: " << valeurX << " < "
				<< x[0] << std::endl;
		return x[0];
	}
	if (valeurX > x[x.size() - 1]) {
		std::cout << "\n Interpolation hors valeurs: " << valeurX << " > "
				<< x[x.size() - 1] << std::endl;
		return x[x.size() - 1];
	}

	//vérification que chaque valeur est plus grande
	//que la précédente dans le vecteur x
	std::valarray<double> diff;
	diff.resize(x.size() - 1);
	for (unsigned int i = 0; i < diff.size(); i++) {
		diff[i] = x[i + 1] - x[i];
	}
	assert(diff.min() >= 0);

	//localisation de la bonne valeur
	unsigned int i(0);
	while (!(x[i] <= valeurX && valeurX < x[i + 1])) {
		i++;
	}
	//calcul du résultat
	double result = y[i]
			+ (y[i + 1] - y[i]) * ((valeurX - x[i]) / (x[i + 1] - x[i]));

	//impression si demandée
	if (imprimer && i != 0 && i != x.size() - 1) {
		std::cout << x[i - 1] << '\t' << y[i - 1] << std::endl;
		std::cout << x[i] << '\t' << y[i] << std::endl;
		std::cout << "---------------------------------" << std::endl;
		std::cout << valeurX << '\t' << result << std::endl;
		std::cout << "---------------------------------" << std::endl;
		std::cout << x[i + 1] << '\t' << y[i + 1] << std::endl;
	}
	if (imprimer && (i == 0 || i == x.size() - 1)) {
		std::cout << "interpolation en bord de vecteur" << std::endl;
	}
	assert(result <= y.max() && result >= y.min());
	return result;
}

double interpolLin(const double valeurX, const std::valarray<double> x,
		const std::valarray<double> y) const {
	return interpolLin(valeurX, x, y, 0);
	//surcharge de interpolLin
}

std::valarray<double> genererCumulInv(const std::valarray<double> x,
		const std::valarray<double> y,
		const unsigned int nbValeursNbAlea) const {
//cumule, normalise et inverse une distribution <y>=f(<x>)
	assert(x.size() == y.size());
	assert(x.size() > 5);
	assert(nbValeursNbAlea > 10);
//Cumulée
	std::valarray<double> cdf;
	cdf.resize(x.size());
	cdf[0] = 0.;
	for (unsigned int l = 1; l < x.size(); l++) {
		cdf[l] = cdf[l - 1] + y[l] * (x[l] - x[l - 1]);
	}
// Normalisation
	cdf /= cdf[cdf.size() - 1];
	assert(cdf[0] == 0.);
	assert(cdf[cdf.size() - 1] == 1.);

//Inversion
	std::valarray<double> cdfInv;
	cdfInv.resize(nbValeursNbAlea);
	cdfInv[0] = x[0];
	cdfInv[nbValeursNbAlea - 1] = x[x.size() - 1];
	for (unsigned int i = 1; i < nbValeursNbAlea - 1; i++) {
		cdfInv[i] = interpolLin((double) i / (double) nbValeursNbAlea, cdf, x);
	}
//vérifications
	assert(cdfInv[0] == x[0]);
	assert(cdfInv[nbValeursNbAlea - 1] == x[x.size() - 1]);
	assert(cdfInv[nbValeursNbAlea - 1] == cdfInv.max());
	//vérification que chaque valeur est plus grande
	//que la précédente dans la pdf
	std::valarray<double> diff;
	diff.resize(cdfInv.size() - 1);
	for (unsigned int i = 0; i < diff.size(); i++) {
		diff[i] = cdfInv[i + 1] - cdfInv[i];
	}
	assert(diff.min() >= 0);

	return cdfInv;
}

double tirageSurCumulInv(const std::valarray<double> cumuleeInversee,
		const double R) const {
//Tirage sur <cumuleeInversee> pour un nombre aléatoire <R>
//exemple de cumuleeInversee: 		(0	0.40	0.5	0.90	1)
//vecteur de nbAleatoire supposé:	(0	0.25	0.5	0.75	1)
// nbNbAlea=cumuleeInversee.size()=5	tailleIntervalle=1/(5-1)=0.25

	assert(0. <= R && R <= 1.);
	//verification que <cumuleeInversee> est bien croissante
	assert(cumuleeInversee[0] == cumuleeInversee.min());
	assert(
			cumuleeInversee[cumuleeInversee.size() - 1]
					== cumuleeInversee.max());

	unsigned int nbNbAlea(cumuleeInversee.size());
	double tailleIntervalle(1. / double(nbNbAlea - 1));
	unsigned int i;
	i = (R / tailleIntervalle);
	assert(i < cumuleeInversee.size());
	double result;
	result = cumuleeInversee[i]
			+ (cumuleeInversee[i + 1] - cumuleeInversee[i])
					* ((R - i * tailleIntervalle) / tailleIntervalle);
	return result;
}

double cdfExpPinv(const double R, const double K) const {
//Cumulée inverse d'une pdf exponentielle de paramètre K pour la valeur R.
	double l = -log(R) / K;
	return l;
}

//Methodes eventuellement à surcharger pour pouvoir préciser le séparateur
double extraireParametre(const std::string fichier, const int numLigne,
		const int position, const double coeff) const {
	/*Fonction qui extrait une valeur stockée dans un fichier <fichier>
	 * à la ligne <numLigne>, après la tabulation n°<position>-1
	 * et la stocke dans une variable <variable>*/
	double variable = 0.;
	std::ifstream monFlux((fichier).c_str());
	if (monFlux) {
		std::string ligne;
		int i(0);
		while (getline(monFlux, ligne)) {
			i++;
			if (i == numLigne) { //lorsque la bonne ligne est atteinte
				std::istringstream iss(ligne);
				std::string mot;
				int j(0);
				while (getline(iss, mot, '\t')) { //découpe la ligne suivant le séparateur spécifié
					j++;
					if (j == position) { //lorsque la position est atteinte
						std::istringstream iss2(mot);
						if (!(iss2 >> variable)) { //erreur si la valeur dans le fichier n'est pas castable en <Type>
							variable = 0;
							std::cout
									<< "probleme de lecture de la variable de la ligne "
									<< numLigne << " du fichier " << fichier
									<< std::endl;
						}
						variable *= coeff;
						break;
					}
				}
				break;
			}
		}
	} else {
		std::cout << "Impossible d'ouvrir " << fichier << std::endl;
	}
	return variable;
}

std::valarray<double> extraireFichier(const std::string fichier,
		const unsigned numLigneDebut, const int position,
		const unsigned nbValeurs, const double coeff) const {
	/*Fonction qui extrait une colonne de valeurs stockée dans un fichier <fichier>
	 * et la stocke dans une variable <variable>*/
	std::valarray<double> variable;
	variable.resize(nbValeurs);
	std::ifstream monFlux((fichier).c_str());
	if (monFlux) {
		std::string ligne;
		unsigned i(0);
		bool lecture(0);
		while (getline(monFlux, ligne)) {
			i++;
			if (i > nbValeurs) {
				std::cout
						<< "\n\nErreur sur le nombre de lignes dans le fichier "
						<< fichier << "\n\n";
			}
			if (!lecture && i == numLigneDebut) {
				lecture = 1; //commencer la lecture
				i = 0; //réinitialisation de i
			}
			if (lecture && i < nbValeurs) {
				std::istringstream iss(ligne);
				std::string mot;
				int j(0);
				while (getline(iss, mot, '\t')) { //séparation de la ligne en mots (sséparateur: '\t')
					j++;
					if (j == position) {
						std::istringstream iss2(mot);
						double temp;
						if (iss2 >> temp) {
							variable[i] = temp;
						} else {
							variable[i] = 0.;
							std::cout
									<< "probleme de lecture de la variable de la ligne "
									<< numLigneDebut + i << " du fichier "
									<< fichier << std::endl;
						}
						break;
					}
				}
			}
		}
		variable *= coeff;
	} else {
		std::cout << "Impossible d'ouvrir " << fichier << std::endl;
	}
	return variable;
}

bool OrientedIntersect(const Ray &ray, Intersection *isect) const {
	/* This methods is similar to Scene::Intersect() defined in scene.h
	 except that here the intersection test is true only if the
	 scalar product of the surface normal and the ray direction is
	 negative at the intersection point (see surface orientation
	 definition in the .pbrt file)                                   */
	bool intersec, orient, oriented_intersec;
	// Call of the Intersect method: intersec = true if there is an intersection
	intersec = aggregate->Intersect(ray, isect);
	// Condition on the scalar product of the surface normal and the ray direction
	orient = (Dot((*isect).dg.nn, ray.d) <= 0.);
	// The returned boolean is true if there is an intersection corresponding to the right orientation
	oriented_intersec = (intersec && orient);
	return oriented_intersec;
}

bool estDansLeVolumeReactionnel(Point X) const {
//Test de l'appartenance au volume réactionnel
	Vector vectTest;
	Ray rayonTest;
	Intersection impactTest;
	bool dansVolumeReactionnel = false;
	for (unsigned int i = 0; i < 3; i++) {
		vectTest = UniformSampleSphere(mcmRng(),mcmRng());
		rayonTest = Ray(X, vectTest, 0., INFINITY, 0.);
		if (Intersect(rayonTest, &impactTest)) {
			if (Dot(impactTest.dg.nn,rayonTest.d)<0){
				dansVolumeReactionnel = true;
			}
			else{
				dansVolumeReactionnel = false;
				return dansVolumeReactionnel;
			}
		}
	}
	return dansVolumeReactionnel;
}

#endif // MCM_METHODES_H
