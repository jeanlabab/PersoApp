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

//Methodes eventuellement à surcharger pour pouvoir préciser le séparateur
//Methodes eventuellement à surcharger pour pouvoir préciser le séparateur
double extraireParametre(std::string fichier, int numLigne, int position,
		double coeff) {
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
		const unsigned nbValeurs, const double coeff) {
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
				i = 0;//réinitialisation de i
			}
			if (lecture) {
				std::istringstream iss(ligne);
				std::string mot;
				int j(0);
				while (getline(iss, mot, '\t')) {
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
#endif // MCM_METHODES_H
