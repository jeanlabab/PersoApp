//ALGO 4VsurS//

#define MCM_nombreDeVariablesDuMonteCarloDeScene 2  // number of quantities evaluated simultaneously (including the proportion of failures)
#define MCM_nombreDeParametresDuMonteCarloDeScene 0 // number of parameters (we evaluate the sensitivity of each quantity with respect to each parameter)
mcmVecteurSommeDeRealisations Scene::realisation() const {

mcmVecteurSommeDeRealisations vsr(dimension);  // the mcmVecteurSommeDeRealisations object returned by realisation()
vsr.nombreDeRealisations = 1;  // number of performed realizations each time realisation() is called

//Photons perdus
int nbPerdu;
beginning:;

//Suivi de rayon
int indiceSurface=0.;
Ray emission;
double poids=0.;
Ray rayonSuivi;
Intersection impactSurface;
double distanceSurface=0.;


//echantillonnage d'un chemin optique
indiceSurface = mcmTirageDuneSurfaceIntegrable();//échantillonnage d'une surface d'émission
emission = (*lights[indiceSurface]).mcmTirageUniformeLambertien(poids);//echantillonnage d'une direction d'émission
rayonSuivi = Ray(emission.o,emission.d,RAY_EPSILON,INFINITY,0.);
if (!Intersect(rayonSuivi,&impactSurface)) {
	nbPerdu++;
	if (nbPerdu>0.1*nombreDeRealisationsDemandees){printf("Trop de photons perdus, arrêt.\n");exit(0);}
	goto beginning;
	}

else {
	distanceSurface = Distance(rayonSuivi.o,impactSurface.dg.p);
	}
vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1),distanceSurface);
vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2),2.);


/***************************************************************************/
/* Free the memory that Pbrt uses for BSDF usage (at intersection points)  */
/***************************************************************************/
BSDF::FreeAll();

return vsr;
}
