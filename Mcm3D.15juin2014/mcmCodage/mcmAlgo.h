//ALGO ACTINO DIRECT: Absorption pure, aspect spectral
// Evalue la probabilité d'absorption d'un photon

#define MCM_nombreDeVariablesDuMonteCarloDeScene 3  // number of quantities evaluated simultaneously (including the proportion of failures)
#define MCM_nombreDeParametresDuMonteCarloDeScene 0 // number of parameters (we evaluate the sensitivity of each quantity with respect to each parameter)
mcmVecteurSommeDeRealisations Scene::realisation() const {

	mcmVecteurSommeDeRealisations vsr(dimension); // the mcmVecteurSommeDeRealisations object returned by realisation()
	vsr.nombreDeRealisations = 1; // number of performed realizations each time realisation() is called
//***********************************************************************************************
//			DECLARATIONS
//***********************************************************************************************

//impression des chemins
	bool imprimer(false);

//Photons perdus
	int nbPerdu(0);
	beginning: ;

//Suivi de rayon
	bool absorption(0);
	int indiceSurface = 0.;
	Ray emission;
	double poids = 0.;
	Ray rayonSuivi;
	unsigned int nbReflexions(0);
	Intersection impactSurface;
	double distanceSurface = 0.;
	double longueurAbs(0.);
	double longueurTotChemin(0.);
	Spectrum rho; //intermédiaire pour reflectivité surface
	float *reflectivite = new float[3];
	Vector directionReflexion;
	int nbReflexionsInox(0);
	double reflectiviteInox(0.);
	int nbReflexionsFibres(0);
	double reflectiviteFibres(0.);

//***********************************************************************************************
//			TIRAGE LONGUEUR D'ONDE
//***********************************************************************************************

	double lambda(tirageSurCumulInv(cumulInvLambda, mcmRng()));
	double eaReinecke(interpolLin(lambda, lambdaReinecke, arrayEaReinecke));
	double eaCoproduit(interpolLin(lambda, lambdaCoprod, arrayEaCoprod));
	double kaReinecke(concentrationReinecke * eaReinecke);
	double kaCoproduit(concentrationCoprod * eaCoproduit);
	double kaTot(kaReinecke + kaCoproduit);

//***********************************************************************************************
//			SUIVI DE PHOTON
//***********************************************************************************************

//échantillonnage d'une surface d'émission
	indiceSurface = mcmTirageDuneSurfaceIntegrable();
//echantillonnage d'une direction d'émission
	emission = (*lights[indiceSurface]).mcmTirageUniformeLambertien(poids);
	rayonSuivi = Ray(emission.o, emission.d, RAY_EPSILON, INFINITY, 0.);
	if (imprimer) {
		std::cout << "\némission depuis " << emission.o << std::endl;
		std::cout << "suivant " << emission.d << std::endl;
	}
//boucle sur les reflexions
	while (!absorption) {
		if (!Intersect(rayonSuivi, &impactSurface)) {
			nbPerdu++;
			if (imprimer) {
				std::cout << "photon perdu, total:" << nbPerdu << std::endl;
			}
			assert(nbPerdu < 0.1 * nombreDeRealisationsDemandees);
			goto beginning;
		} else {
			distanceSurface = Distance(rayonSuivi.o, impactSurface.dg.p);
			assert(distanceSurface > 0.);
			longueurAbs = cdfExpPinv(mcmRng(), kaTot);
			assert(longueurAbs > 0.);
//interaction sur surface
			if (longueurAbs > distanceSurface) {
				longueurTotChemin += distanceSurface;
				nbReflexions++;

				assert(nbReflexions < 1000);

				//indice de la surface
				impactSurface.GetBSDF(RayDifferential(rayonSuivi))->get_mcmSurfaceAvecNumero(
						indiceSurface);
				//reflectivité
				rho = impactSurface.GetBSDF(RayDifferential(rayonSuivi))->rho();
				rho.GetColor(reflectivite);
				if (imprimer) {
					std::cout << "intersection sur surface";
				}
//interaction avec les fibres
				if (indiceSurface == 0) {
					reflectiviteFibres = reflectivite[0];
					nbReflexionsFibres++;
					if (imprimer) {
						std::cout << " fibre";
					}
				}
//interaction avec la cuve
				if (indiceSurface == 1) {
					reflectiviteInox = reflectivite[0];
					nbReflexionsInox++;
					if (imprimer) {
						std::cout << " inox";
					}
				}
				if (imprimer) {
					std::cout << " au point " << impactSurface.dg.p
							<< std::endl;
				}
				//echantillonnage d'une direction de reflexion
				impactSurface.GetBSDF(RayDifferential(rayonSuivi))->Sample_f(
						rayonSuivi.d, &directionReflexion);
				//nouveau rayon suivi
				rayonSuivi = Ray(impactSurface.dg.p, -directionReflexion,
						RAY_EPSILON, INFINITY, 0.);
			}
//absorption dans le volume
			else {
				longueurTotChemin += longueurAbs;
				absorption = true;
				if (imprimer) {
					std::cout << "absorption dans le volume " << std::endl;
				}
			}
		}
	}
	poids = kaReinecke / kaTot * exp(-kaTot * longueurTotChemin)
			* std::pow(reflectiviteFibres, nbReflexionsFibres)
			* std::pow(reflectiviteInox, nbReflexionsInox);
	assert(poids >= 0. && poids <= 1.);

//***********************************************************************************************
//			FIN DU SUIVI DE PHOTON / PASSAGE DES POIDS
//***********************************************************************************************
	vsr.initialisationUneComposante(
			indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1),
			nbPerdu / (nbPerdu + 1));
	vsr.initialisationUneComposante(
			indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2), poids);
	vsr.initialisationUneComposante(
			indiceDeLaCoordonneeDuVecteurSommeDeRealisations(3),
			longueurTotChemin);

	BSDF::FreeAll(); // Free the memory that Pbrt uses for BSDF usage (at intersection points)

	return vsr;

}
