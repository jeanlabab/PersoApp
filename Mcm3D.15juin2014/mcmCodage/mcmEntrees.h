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

//reflectivite cellulose
rhoCellulose=extraireParametre("proprietes.in",13,2,1.);

//reflectivite biofilm
rhoBiofilm=extraireParametre("proprietes.in",14,2,1.);

//reflectivite fond
rhoFond=extraireParametre("proprietes.in",15,2,1.);

//reflexion parois speculaire
reflexionSpeculaire=extraireParametre("proprietes.in",16,2,1.);
//#fin de l'ajout par import_proprietes.pl
