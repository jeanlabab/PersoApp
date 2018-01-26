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

#ifndef PBRT_LIGHT_H
#define PBRT_LIGHT_H
// light.h*
#include "pbrt.h"
#include "geometry.h"
#include "transform.h"
#include "color.h"
#include "paramset.h"
#include "mc.h"
// Light Declarations
class COREDLL Light {
public:
  // Light Interface
  virtual ~Light();
  Light(const Transform &l2w, int ns = 1) : nSamples(max(1, ns)), LightToWorld(l2w), WorldToLight(l2w.GetInverse()) {
    if (WorldToLight.HasScale()) Warning("Scaling detected in world-to-light transformation!\n"
					 "The system has numerous assumptions, implicit and explicit,\n"
					 "that this transform will have no scale factors in it.\n"
					 "Proceed at your own risk; your image may have errors or\n"
					 "the system may crash as a result of this.");    
    mcmSurfaceIntegrable = false;
  }
	virtual Spectrum Sample_L(const Point &p,
		Vector *wi, VisibilityTester *vis) const = 0;
	virtual Spectrum Power(const Scene *) const = 0;
	virtual bool IsDeltaLight() const = 0;
	virtual Spectrum Le(const RayDifferential &r) const;
	virtual Spectrum Sample_L(const Point &p, float u1,
		float u2, Vector *wi, float *pdf,
		VisibilityTester *vis) const = 0;
	virtual float Pdf(const Point &p,
	                  const Vector &wi) const = 0;
	virtual Spectrum Sample_L(const Point &p, const Normal &n,
			float u1, float u2, Vector *wi, float *pdf,
			VisibilityTester *visibility) const {
		return Sample_L(p, u1, u2, wi, pdf, visibility);
	}
	virtual float Pdf(const Point &p, const Normal &n,
			const Vector &wi) const {
		return Pdf(p, wi);
	}
	virtual Spectrum Sample_L(const Scene *scene, float u1,
		float u2, float u3, float u4,
		Ray *ray, float *pdf) const = 0;
	bool get_mcmSurfaceIntegrable() const { return mcmSurfaceIntegrable; }
	virtual double get_mcmAireDeLaSurfaceIntegrable() const {
	  Severe("ERREUR ! get_mcmAireDeLaSurfaceIntegrable non défini");
		return double();
	}
	virtual Ray mcmTirageUniformeLambertien(double &poids) const {
	  Severe("ERREUR ! mcmTirageUniformeLambertien non défini");
		return Ray();
	}
	virtual Ray mcmTirageUniformeNormal(double &poids) const {
	  Severe("ERREUR ! mcmTirageUniformeNormal non défini");
		return Ray();
	}
	virtual Ray mcmTirageVolumiqueUniformeIsotrope() const {
	  Severe("ERREUR ! mcmTirageVolumiqueUniformeIsotrope non defini");
		return Ray();
	}
	virtual bool mcmEstDansLeVolumeInterieur(Point P) const {
	  Severe("ERREUR ! mcmEstDansLeVolumeInterieur non defini");
		return false;
	}
	virtual float mcmVolumeInterieurDeLaSurfaceIntegrable() const {
	  Severe("ERREUR ! mcmVolumeInterieurDeLaSurfaceIntegrable non defini");
		return float();
	}
	// Light Public Data
	const int nSamples;
protected:
	// Light Protected Data
	const Transform LightToWorld, WorldToLight;
	bool mcmSurfaceIntegrable;
};
struct COREDLL VisibilityTester {
	// VisibilityTester Public Methods
	void SetSegment(const Point &p1, const Point &p2) {
		r = Ray(p1, p2-p1, RAY_EPSILON, 1.f - RAY_EPSILON);
	}
	void SetRay(const Point &p, const Vector &w) {
		r = Ray(p, w, RAY_EPSILON);
	}
	bool Unoccluded(const Scene *scene) const;
	Spectrum Transmittance(const Scene *scene) const;
	Ray r;
};
class AreaLight : public Light {
public:
	// AreaLight Interface
	AreaLight(const Transform &light2world,
		const Spectrum &power, int ns, const Reference<Shape> &shape, bool si = false);
	virtual Spectrum L(const Point &p, const Normal &n,
			const Vector &w) const {
		return Dot(n, w) > 0 ? Lemit : 0.;
	}
	Spectrum Power(const Scene *) const {
		return Lemit * area * M_PI;
	}
	bool IsDeltaLight() const { return false; }
	float Pdf(const Point &, const Vector &) const;
	float Pdf(const Point &, const Normal &, const Vector &) const;
	Spectrum Sample_L(const Point &P, Vector *w, VisibilityTester *visibility) const;
	virtual Spectrum Sample_L(const Point &P, const Normal &N,
		float u1, float u2, Vector *wo, float *pdf,
		VisibilityTester *visibility) const;
	virtual Spectrum Sample_L(const Point &P, float u1, float u2, Vector *wo,
		float *pdf, VisibilityTester *visibility) const;
	Spectrum Sample_L(const Scene *scene, float u1, float u2,
			float u3, float u4, Ray *ray, float *pdf) const;
	double get_mcmAireDeLaSurfaceIntegrable() const { return mcmAireDeLaSurfaceIntegrable; }
	Ray mcmTirageUniformeLambertien(double &poids) const; 
	Ray mcmTirageUniformeNormal(double &poids) const; 
	Ray mcmTirageVolumiqueUniformeIsotrope() const; 
	bool mcmEstDansLeVolumeInterieur(Point P) const; 
	float mcmVolumeInterieurDeLaSurfaceIntegrable() const; 
protected:
	// AreaLight Protected Data
	Spectrum Lemit;
	Reference<Shape> shape;
	float area;
	double mcmAireDeLaSurfaceIntegrable;
};
#endif // PBRT_LIGHT_H
