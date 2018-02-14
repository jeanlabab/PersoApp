/*
 Copyright 2010, 2010 STARWest
 file://localhost/Volumes/vrai-home/dauchet/Pbrt/Mcm3D/mcmCodage/mcmEntrees.h
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
concentrationReinecke=extraireParametre("proprietes.in",3,2,1.);

//Concentration Coproduit
concentrationCoprod=extraireParametre("proprietes.in",4,2,1.);

//fichier: SpectreSimulateur_750.txt
 lambda = extraireFichier("SpectreSimulateur_750.txt",2, 1,701,1.);
 spectreNonNorm = extraireFichier("SpectreSimulateur_750.txt",2, 2,701,1.);

//fichier: EaReinecke.txt
 lambdaReinecke = extraireFichier("EaReinecke.txt",2, 1,203,1e-9);
 eaReinecke = extraireFichier("EaReinecke.txt",2, 2,203,1.);

//fichier: EaCoproduit.txt
 lambdaCoprod = extraireFichier("EaCoproduit.txt",2, 1,401,1.);
 eaCoprod = extraireFichier("EaCoproduit.txt",2, 2,401,1.);
//#fin de l'ajout par import_proprietes.pl

