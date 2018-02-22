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
//boucle sur les reflexions
	while (!absorption) {
		if (!Intersect(rayonSuivi, &impactSurface)) {
			nbPerdu++;
			assert(nbPerdu < 0.04 * nombreDeRealisationsDemandees);
			goto beginning;
		} else {
			distanceSurface = Distance(rayonSuivi.o, impactSurface.dg.p);
			longueurAbs = cdfExpPinv(mcmRng(), kaTot);
			assert(distanceSurface > 0.);
			assert(longueurAbs > 0.);
//interaction sur surface
			if (longueurAbs > distanceSurface) {
				longueurTotChemin += distanceSurface;
				nbReflexions++;
				assert(nbReflexions < 100);
				//reflectivité
				rho = impactSurface.GetBSDF(RayDifferential(rayonSuivi))->rho();
				rho.GetColor(reflectivite);
//roulette russe
//reflection sur surface
				if (mcmRng() < reflectivite[0]) {
					impactSurface.GetBSDF(RayDifferential(rayonSuivi))->Sample_f(
							rayonSuivi.d, &directionReflexion);
					rayonSuivi = Ray(impactSurface.dg.p, -directionReflexion,
							RAY_EPSILON, INFINITY, 0.);
				}
//absorption sur surface
				else {
					poids = 0.;
					absorption = true;
				}
			}
//absorption dans le volume
			else {
				longueurTotChemin += longueurAbs;
				poids = kaReinecke / kaTot;
				absorption = true;
			}
		}
	}

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
