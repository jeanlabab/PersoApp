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

//#debut de l'ajout par import_proprietes.pl
//Concentration Reinecke
double concentrationReinecke;
//Concentration Coproduit
double concentrationCoprod;

//fichier: SpectreSimulateur_750.txt
std::valarray<double> lambdaSpectre;
std::valarray<double> spectreNonNorm;

//fichier: EaReinecke.txt
std::valarray<double> lambdaReinecke;
std::valarray<double> arrayEaReinecke;

//fichier: EaCoproduit.txt
std::valarray<double> lambdaCoprod;
std::valarray<double> arrayEaCoprod;
//#fin de l'ajout par import_proprietes.pl
std::valarray<double> cumulInvLambda;




