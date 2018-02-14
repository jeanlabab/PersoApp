package import_proprietes_func;

use strict;
use warnings;

sub nettoyerTableau {

	#effacer les lignes entre les balises
	my $fin   = pop(@_);
	my $debut = pop(@_);
	my $l;
	my $efface= 0;
	foreach $l (@_) {
		$efface = 0 if ( $l =~ m/^\Q$fin\E/ );
		$l      = "" if ( $efface == 1 );
		$efface = 1  if ( $l =~ m/^\Q$debut\E/ );
	}
	print("\nBalise de fin d'insertion non".
	" trouvée dans un des fichiers.\n") if ($efface==1) ;
	return @_;
}

sub ecrireFichier {
	
}
1;
