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

/*
  pbrt source code Copyright(c) 1998-2007 Matt Pharr and Greg Humphreys.
  
  This file is part of pbrt.

  pbrt is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Note that the text contents of
  the book "Physically Based Rendering" are *not* licensed under the
  GNU GPL.

  pbrt is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PBRT_SCENE_H
#define PBRT_SCENE_H
// scene.h*
#include "pbrt.h"
#include "primitive.h"
#include "transport.h"
#include "mcm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstddef>      // std::size_t
#include <valarray>     // std::valarray
#include <cassert>
// Scene Declarations
class COREDLL Scene : public mcmMonteCarlo {
public:
	// Scene Public Methods
	void Render();
	Scene(Camera *c, SurfaceIntegrator *in,
		VolumeIntegrator *vi, Sampler *s,
		Primitive *accel, const vector<Light *> &lts,
		VolumeRegion *vr);
	~Scene();
	bool Intersect(const Ray &ray, Intersection *isect) const {
		return aggregate->Intersect(ray, isect);
	}
	bool IntersectP(const Ray &ray) const {
		return aggregate->IntersectP(ray);
	}
	const BBox &WorldBound() const;
	Spectrum Li(const RayDifferential &ray, const Sample *sample,
		float *alpha = NULL) const;
	Spectrum Transmittance(const Ray &ray) const;
	// Scene Data
	Primitive *aggregate;
	vector<Light *> lights;
	Camera *camera;
	VolumeRegion *volumeRegion;
	SurfaceIntegrator *surfaceIntegrator;
	VolumeIntegrator *volumeIntegrator;
	Sampler *sampler;
	BBox bound;
protected:
	//methods
	mcmVecteurSommeDeRealisations realisation() const;
	int mcmTirageDuneSurfaceIntegrable() const {
	  // Tirage aléatoire d'une surface parmis les surfaces intégrables, avec une probabilité proportionnelle à la surface
	  double valeurLimite;
	  double sommeDesPoids;
	  int indiceLight;
	  valeurLimite = mcmSommeDesAiresDesSurfacesIntegrables*mcmRng();
	  sommeDesPoids=0;
	  indiceLight=-1;
	  while (valeurLimite>sommeDesPoids) {
	    indiceLight=indiceLight+1;
	    if ((*lights[indiceLight]).get_mcmSurfaceIntegrable())
	      {
		sommeDesPoids=sommeDesPoids+(*lights[indiceLight]).get_mcmAireDeLaSurfaceIntegrable();
	      }
	  }
	  return indiceLight;
	}
        //data
	double mcmSommeDesAiresDesSurfacesIntegrables;

#include "mcmVariables.h"
#include "mcmMethodes.h"
};
#endif // PBRT_SCENE_H
