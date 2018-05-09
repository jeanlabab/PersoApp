#define MCM_nombreDeVariablesDuMonteCarloDeScene 7  // number of quantities evaluated simultaneously (including the proportion of failures)
#define MCM_nombreDeParametresDuMonteCarloDeScene 0 // number of parameters (we evaluate the sensitivity of each quantity with respect to each parameter)
mcmVecteurSommeDeRealisations Scene::realisation() const {

	mcmVecteurSommeDeRealisations vsr(dimension); // the mcmVecteurSommeDeRealisations object returned by realisation()
	vsr.nombreDeRealisations = 1; // number of performed realizations each time realisation() is called
//***********************************************************************************************
//			DECLARATIONS
//***********************************************************************************************

//impression des chemins
	bool imprimer(true);
	bool imprimer_points(false);

//Photons perdus
	int nbPerdu = 0;
	begining:

//Suivi de rayon
	bool absorption(0);
	int indiceSurface = 0.;
	Ray emission;
	Ray rayonSuivi;
	double poids(0.);
	unsigned int nbReflexions(0);
	Intersection impactSurface;
	double distanceSurface = 0.;
	double longueurTotChemin(0.);
	Vector directionReflexion;
	//Vector directionBas = Vector(0, 0, -1);
	bool reflexionStandard(1);

//Poids
	double hauteurAbsorption(0.);
	bool absBiofilm(0);
	bool absCellulose(0);
	bool reflechi(0);
	bool transmis(0);

//***********************************************************************************************
//			SUIVI DE PHOTON
//***********************************************************************************************

//échantillonnage d'une surface d'émission
	indiceSurface = mcmTirageDuneSurfaceIntegrable();
//echantillonnage d'une direction d'émission
//	do
	emission = (*lights[indiceSurface]).mcmTirageUniformeLambertien(poids);
//	while (Dot(directionBas,emission.d)>0.80);
//	std::cout << emission.d << std::endl;
//	std::cout <<  << std::endl;
	rayonSuivi = Ray(emission.o, emission.d, RAY_EPSILON, INFINITY, 0.);
	if (imprimer) {
		std::cout << "\némission depuis " << emission.o << std::endl;
		std::cout << "suivant " << emission.d << std::endl;
	}
	if (imprimer_points){std::cout <<"\nNouveau rayon\n"<< emission.o << std::endl;}
//boucle sur les reflexions
	while (!absorption) {
		if (!Intersect(rayonSuivi, &impactSurface)) {
			nbPerdu++;
			//if (imprimer) {
			std::cout << "photon perdu, total:" << nbPerdu << std::endl;
			//}
			assert(nbPerdu < 0.1 * nombreDeRealisationsDemandees);
			goto begining;
		} else {
			distanceSurface = Distance(rayonSuivi.o, impactSurface.dg.p);
			assert(distanceSurface > 0.);
//interaction sur surface
			longueurTotChemin += distanceSurface;
			nbReflexions++;
			assert(nbReflexions < 1000);
			//indice de la surface
			impactSurface.GetBSDF(RayDifferential(rayonSuivi))->get_mcmSurfaceAvecNumero(
					indiceSurface);
			if (imprimer) {
				std::cout << "intersection sur surface";
			}
			switch (indiceSurface) {
//interaction avec le fond
			case 0:
				if (mcmRng()>rhoFond){
					transmis = true;
					absorption = true;
				}
				else {
					directionReflexion = rayonSuivi.d;
					directionReflexion.z = -directionReflexion.z;
					rayonSuivi = Ray(impactSurface.dg.p, directionReflexion,
							RAY_EPSILON, INFINITY, 0.);
					reflexionStandard = false;
				}
				if (imprimer) {
					std::cout << " bas de la cuve";
				}
				break;
//interaction avec une extremité
			case 1:
				//gestion des reflexions speculaires, sur un plan Oxz
				//on inverse simplement la coordonnée y pour avoir le vecteur de reflexion
				directionReflexion = rayonSuivi.d;
				directionReflexion.y = -directionReflexion.y;
				rayonSuivi = Ray(impactSurface.dg.p, directionReflexion,
						RAY_EPSILON, INFINITY, 0.);
				reflexionStandard = false;
				if (imprimer) {
					std::cout << " extremite";
				}
				break;
//interaction avec la cellulose
			case 2:
				if (mcmRng() > rhoCellulose) {
					absCellulose = true;
					absorption = true;
				}
				if (imprimer) {
					std::cout << " cellulose";
				}
				break;
//interaction avec le biofilm
			case 3:
				if (mcmRng() > rhoBiofilm) {
					absBiofilm = true;
					absorption = true;
				}
				if (imprimer) {
					std::cout << " biofilm";
				}
				break;
//interaction avec le haut de la cuve
			case 4:
				reflechi = true;
				absorption = true;
				if (imprimer) {
					std::cout << " haut de la cuve";
				}
				break;
			default:
				break;
			}
			if (imprimer) {
				std::cout << " au point " << impactSurface.dg.p << std::endl;
			}
			if (imprimer_points){std::cout << impactSurface.dg.p << std::endl;}
			if (absorption) {
				hauteurAbsorption = impactSurface.dg.p.z;
				std::cout << hauteurAbsorption << std::endl;
			} else if (reflexionStandard) {
				if (reflexionSpeculaire) {//reflexion speculaire
					//gestion des reflexions speculaires, sur un plan Oyz
					//on inverse simplement la coordonnée x pour avoir le vecteur de reflexion
					directionReflexion = rayonSuivi.d;
					directionReflexion.x = -directionReflexion.x;
					rayonSuivi = Ray(impactSurface.dg.p, directionReflexion,
							RAY_EPSILON, INFINITY, 0.);
					if (imprimer) {
						std::cout << "réflexion spéculaire"<< std::endl;
					}
				} else {//reflexion diffuse
					//echantillonnage d'une direction de reflexion
					impactSurface.GetBSDF(RayDifferential(rayonSuivi))->Sample_f(
							rayonSuivi.d, &directionReflexion);
					//nouveau rayon suivi
					rayonSuivi = Ray(impactSurface.dg.p, -directionReflexion,
							RAY_EPSILON, INFINITY, 0.);
					if (imprimer) {
						std::cout << "réflexion duffuse" << std::endl;
					}
				}
			} else {
				reflexionStandard = true;
			}
		}
	}

//***********************************************************************************************
//			FIN DU SUIVI DE PHOTON / PASSAGE DES POIDS
//***********************************************************************************************

	vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1),nbPerdu / (double)(nbPerdu + 1));
	vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2),longueurTotChemin);
	vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(3),absBiofilm);
	vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(4),absCellulose);
	vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(5),reflechi);
	vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(6),transmis);
	vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(7),hauteurAbsorption);

	BSDF::FreeAll(); // Free the memory that Pbrt uses for BSDF usage (at intersection points)

	return vsr;

}
