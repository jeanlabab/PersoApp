#!/usr/bin/perl -w
use strict;
use warnings;
use import_proprietes_func;
###################################################
#	Modele de fichier d'entree:
#	#les commentaires commencent par "#"
#	#parametres:
#	Concentration Reinecke	2.	mol.m-3	concentrationReinecke	double	1
#
#	#fichier:
#	SpectreSimulateur_750.txt	begin:2	colonnes:2	lambda	double	1	spectreNonNorm	double	1e-9
###################################################

#chemin relatif entre les fichiers et ce script
my $repertoire = "../";

#fichier à lire
my $fichierProprietes    = "proprietes.in";
my $fichierProprietesRep = $repertoire . $fichierProprietes;

#chemins relatif depuis run:
my $mcmEntrees      = "../Mcm3D.15juin2014/mcmCodage/mcmEntrees.h";
my $mcmVariables    = "../Mcm3D.15juin2014/mcmCodage/mcmVariables.h";
my $mcmEntreesRep   = $repertoire . $mcmEntrees;
my $mcmVariablesRep = $repertoire . $mcmVariables;

#génération du nom du script (enlever le chemin à $0):
my $scriptPl = $0;
$scriptPl =~ s/\/.+\///;

#balises dans les fichiers de sortie
my $baliseDebutInsertion = "\/\/#debut de l'ajout par $scriptPl";
my $baliseFinInsertion   = "\/\/#fin de l'ajout par $scriptPl";

#à quelle position sont les paramètres (2= deuxieme colonne = après la première tab):
my $positionValeurParametres = 2;

my $numLigne = 0;
my $l;

#Paramètres
my $nbParametres = 0;
my %parametres   = ();

#Fichiers
my $nbFichiers = 0;
my %fichiers   = ();

###########################################################
#			Extraire les infos du fichier lu
###########################################################
open( PROPRIETES, "<$fichierProprietesRep" ) or die("open: $!");
while ( defined( $l = <PROPRIETES> ) ) {
	chomp $l;
	$numLigne++;
	if ( $l !~ m/^#/ ) {    #Si la ligne ne commence pas par un dièse
		if ( $l =~ m/^([\w ]+)\t/ )
		{ #si la ligne commence par des mots devant une tabulation (pas d'extention, ex:.txt)
			(
				$parametres{$nbParametres}{NOM},
				$parametres{$nbParametres}{VALEUR},
				$parametres{$nbParametres}{UNITE},
				$parametres{$nbParametres}{VARIABLE},
				$parametres{$nbParametres}{TYPECPP},
				$parametres{$nbParametres}{COEFF}
			) = split( /\t/, $l );
			$parametres{$nbParametres}{NUMLIGNE} = $numLigne;
			$nbParametres++;
			next;
		}
		if ( $l =~ m/^(\w+\.txt)\t/ ) {   #si la ligne commence par <un mot>.txt
			(    #alors c'est une ligne contenant un fichier
				$fichiers{$nbFichiers}{NOM},
				$fichiers{$nbFichiers}{DEBUT},
				$fichiers{$nbFichiers}{NBCOLONNES}
			  )
			  = ( $l =~ m/^(\w+\.txt)\tbegin:(\d+)\tcolonnes:(\d+)\t/ )
			  ;    #extraction des trois premières données
			$fichiers{$nbFichiers}{NOMREP} =
			    $repertoire
			  . $fichiers{$nbFichiers}{NOM}
			  ;    #ajout du nom de repertoire devant le nom lu
			my @elements = split( /\t/, $l );    #découpage de la ligne en mots
			for ( my $i = 0 ; $i < $fichiers{$nbFichiers}{NBCOLONNES} ; $i++ ) {
				$fichiers{$nbFichiers}{COLONNES}{$i}{VARIABLE} =
				  $elements[ 2 + $i * 3 + 1 ];
				$fichiers{$nbFichiers}{COLONNES}{$i}{TYPE} =
				  $elements[ 2 + $i * 3 + 2 ];
				$fichiers{$nbFichiers}{COLONNES}{$i}{COEFF} =
				  $elements[ 2 + $i * 3 + 3 ];
				$fichiers{$nbFichiers}{COLONNES}{$i}{NUMERO} =
				  $i + 1;
			}
			$nbFichiers++;
			next;
		}
	}
}
close(PROPRIETES);

###########################################################
#			Compter les lignes des fichiers trouvés
###########################################################
for ( my $i = 0 ; $i < $nbFichiers ; $i++ ) {
	my $nbLignes = 0;
	open( FICHIER, "<$fichiers{$i}{NOMREP}" ) or die("open: $!");
	while (<FICHIER>) { $nbLignes++; }
	close(FICHIER);
	$fichiers{$i}{NBLIGNESTOT} = $nbLignes;
	$fichiers{$i}{NBVALEURS} = $nbLignes - ( $fichiers{$i}{DEBUT} - 1 );
}

###########################################################
#			Afficher un bilan
###########################################################
print("\n$nbParametres parametres trouvés:");
foreach my $k ( keys(%parametres) ) {
	print("\n$k : $parametres{$k}{TYPECPP}\t$parametres{$k}{VARIABLE}");
	print("\t$parametres{$k}{VALEUR}$parametres{$k}{UNITE}");
}
print("\n\n$nbFichiers fichiers trouvés:");
foreach my $k ( keys(%fichiers) ) {
	print("\n$k : $fichiers{$k}{NOM}\t$fichiers{$k}{NBCOLONNES} colonnes");
	print("\t$fichiers{$k}{NBVALEURS} valeurs");
	foreach my $c ( keys( %{$fichiers{$k}{COLONNES}} ) ) {
		print("\n\t$fichiers{$k}{COLONNES}{$c}{NUMERO}");
		print("\t$fichiers{$k}{COLONNES}{$c}{TYPE}");
		print("\t$fichiers{$k}{COLONNES}{$c}{VARIABLE}");
	}
}

###########################################################
#			écrire dans les fichiers de sortie
###########################################################
my $efface = 0.;

open( ENTREES, "<$mcmEntreesRep" ) or die("open: $!");
my @fichierEntrees = <ENTREES>;
close(ENTREES);

open( VARIABLES, "<$mcmVariablesRep" ) or die("open: $!");
my @fichierVariables = <VARIABLES>;
close(VARIABLES);

import_proprietes_func::nettoyerTableau( @fichierEntrees,
	$baliseDebutInsertion, $baliseFinInsertion );
import_proprietes_func::nettoyerTableau( @fichierVariables,
	$baliseDebutInsertion, $baliseFinInsertion );

my $lEntrees;
my $lVariables;

foreach $lEntrees (@fichierEntrees) {
	if ( $lEntrees =~ m/^\Q$baliseDebutInsertion\E/ )
	{    #recherche de la bonne ligne dans mcmEntrees.h
		foreach $lVariables (@fichierVariables) {
			if ( $lVariables =~ m/^\Q$baliseDebutInsertion\E/ )
			{    #recherche de la bonne ligne dans mcmEntrees.h
				######PARAMETRES
				for ( my $i = 0 ; $i < $nbParametres ; $i++ ) {    #parametres
					###FICHIER mcmEntrees.h
					$lEntrees .= "\n//${parametres{$i}{NOM}}\n";
					$lEntrees .=
					    "${parametres{$i}{VARIABLE}}="
					  . "extraireParametre(\"$fichierProprietes\","
					  . "${parametres{$i}{NUMLIGNE}},$positionValeurParametres,"
					  . "${parametres{$i}{COEFF}});\n";
					$lEntrees =~ s/(,[0-9]+)\);\n$/$1\.\);\n/
					  ;  #ajoute un point si il n'y en a pas au dernier argument
					###FICHIER mcmVariables.h
					$lVariables .= "//${parametres{$i}{NOM}}\n";
					$lVariables .=
"${parametres{$i}{TYPECPP}} ${parametres{$i}{VARIABLE}};\n";

				}
				#####FICHIERS
				for ( my $i = 0 ; $i < $nbFichiers ; $i++ ) {    #Fichiers
					$lEntrees   .= "\n//fichier: ${fichiers{$i}{NOM}}\n";
					$lVariables .= "\n//fichier: ${fichiers{$i}{NOM}}\n";
					for ( my $j = 0 ; $j < $fichiers{$i}{NBCOLONNES} ; $j++ ) {
						###FICHIER mcmEntrees.h
						$lEntrees .=
						    "${fichiers{$i}{COLONNES}{$j}{VARIABLE}} = "
						  . "extraireFichier(\"${fichiers{$i}{NOM}}\""
						  . ",${fichiers{$i}{DEBUT}}, ${fichiers{$i}{COLONNES}{$j}{NUMERO}},"
						  . "${fichiers{$i}{NBVALEURS}},${fichiers{$i}{COLONNES}{$j}{COEFF}});\n";
						$lEntrees =~ s/(,[0-9]+)\);\n$/$1\.\);\n/
						  ; #ajoute un point si il n'y en a pas au dernier argument
						###FICHIER mcmVariables.h
						$lVariables .=
						    "std::valarray<${fichiers{$i}{COLONNES}{$j}{TYPE}}>"
						  . " ${fichiers{$i}{COLONNES}{$j}{VARIABLE}};\n";

					}

				}
			}
		}
	}
}

open( ENTREES, ">$mcmEntreesRep" ) or die("open: $!");
foreach $l (@fichierEntrees) { print ENTREES $l; }
close(ENTREES);

open( VARIABLES, ">$mcmVariablesRep" ) or die("open: $!");
foreach $l (@fichierVariables) { print VARIABLES $l; }
close(VARIABLES);
