#define MCM_nombreDeVariablesDuMonteCarloDeScene 2  // number of quantities evaluated simultaneously (including the proportion of failures)
#define MCM_nombreDeParametresDuMonteCarloDeScene 3 // number of parameters (we evaluate the sensitivity of each quantity with respect to each parameter)
mcmVecteurSommeDeRealisations Scene::realisation() const {

  mcmVecteurSommeDeRealisations vsr(dimension);  // the mcmVecteurSommeDeRealisations object returned by realisation()
  vsr.nombreDeRealisations = 1;  // number of performed realizations each time realisation() is called
  int failuresNumber = 0;  // number of attempted realization that failed for numerical reasons 
  double wFailuresProportion;  // weight used for the evaluation of the failures proportion
  double wFailuresProportion_void;  // weight used for the sensitivities of the failures proportion (which are set to zero as they aren't evaluated)

  double w;
  double w_param1;
  double w_param2;
  double w_param3;

  w=mcmRng();
  w_param1=0.;
  w_param2=0.;
  w_param3=0.;

  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1),wFailuresProportion);
  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1,1),wFailuresProportion_void);
  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1,2),wFailuresProportion_void);
  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(1,3),wFailuresProportion_void);
  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2),w);
  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2,1),w_param1);
  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2,2),w_param2);
  vsr.initialisationUneComposante(indiceDeLaCoordonneeDuVecteurSommeDeRealisations(2,3),w_param3);
	
  /***************************************************************************/
  /* Free the memory that Pbrt uses for BSDF usage (at intersection points)  */
  /***************************************************************************/
  BSDF::FreeAll();

  return vsr; 
}
