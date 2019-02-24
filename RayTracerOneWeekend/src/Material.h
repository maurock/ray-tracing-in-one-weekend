/*
 * Material.h
 *
 *  Created on: Feb 15, 2019
 *      Author: mauro
 */

#ifndef MATERIALH
#define MATERIALH
#include "Ray.h"
#include "Hitable.h"

struct hit_record;

class Material {
	public:
		virtual bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const = 0;
};

// Take a random point in a unit sphere tangent to the hitting point.
// If the poinT is outside, re-do the calculus.
Vec3 random_unit_sphere(){
	Vec3 p;
	do{
		// Random point in a cube where x,y,z range -1, 1. This does the linear transformation.
		// F(v) = A * v + b. v and b are vectors, where v =[0,1].
		float rand_a = (double) rand() / double(RAND_MAX);
		float rand_b = (double) rand() / double(RAND_MAX);
		float rand_c = (double) rand() / double(RAND_MAX);
		p = 2*Vec3(rand_a, rand_b, rand_c) - Vec3(1, 1, 1);
	}while(p.squared_length() >= 1.0);
	return p;
}

// Calculate reflection vector
Vec3 reflect(const Vec3& v, const Vec3& n) {
	return v - 2*dot(v,n)*n;
}

// Calculate refraction
// Check this for the calculation: http://viclw17.github.io/2018/08/05/raytracing-dielectric-materials/
bool refract(const Vec3& v, const Vec3& n, float ni_over_nt, Vec3& refracted){
	Vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant>0){
		refracted = ni_over_nt * (uv - dt*n) - n*sqrt(discriminant);
		return true;
	}else{
		return false; // no refracted ray
	}
}

// Calculate specular reflection coefficient, or probability of reflection
float schlick(float cosine, float ref_idx){
	float r0 =  (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1-cosine),5);
}


class Lambertian : public Material {
	public:
		Lambertian(const Vec3& a) : albedo(a) {}
		virtual bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const {
			// Find a point inside the sphere tangent to the hitting point. N + P = center. random_unit ranges from [-1, 1], and it is inside the sphere
			Vec3 target = rec.p + rec.normal + random_unit_sphere();
			scattered = Ray(rec.p, target - rec.p);
			attenuation = albedo;
			return true;
		}
		Vec3 albedo;
};


// fuzz is for the glossy effect. It introduces a small randomness in the reflected ray
class Metal : public Material {
	public:
		Metal(const Vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1;}
		virtual bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const {
			// Find a point inside the sphere tangent to the hitting point. N + P = center. random_unit ranges from [-1, 1], and it is inside the sphere
			Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
			scattered = Ray(rec.p, reflected + fuzz*random_unit_sphere());
			attenuation = albedo;
			return (dot(scattered.direction(), rec.normal) > 0);
		}
		Vec3 albedo;
		float fuzz;
};



class Dielectric : public Material {
	public:
		Dielectric(float ri) : ref_idx(ri) {};
		virtual bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const {
			Vec3 outward_normal;
			Vec3 reflected = reflect(r_in.direction(), rec.normal);
			float ni_over_nt;
			// Attenuation 1 means that it doesn't absorb nothing.
			// The last 0 is to kill the blue channel.
			attenuation = Vec3(1, 1, 1);
			Vec3 refracted;
			float reflect_prob;
			float cosine;
			// When ray shoot through object back into vacuum,
			// ni_over_nt = ref_idx, surface normal has to be inverted.
			if (dot(r_in.direction(), rec.normal) > 0) {
				outward_normal = -rec.normal;
				ni_over_nt = ref_idx;
				cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
			}else{
				// when ray shoots into object, ni_over_nt = 1 / ref_idx.
				outward_normal = rec.normal;
				ni_over_nt = 1/ref_idx;
				cosine = -ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
			}

			// Dealing with Refraction/ Reflection
			if(refract(r_in.direction(), outward_normal, ni_over_nt, refracted)){
				reflect_prob = schlick(cosine, ref_idx);
			}else{
				reflect_prob = 1.0;
			}
			if(((double) rand() / double(RAND_MAX))< reflect_prob){
				scattered = Ray(rec.p, reflected);
			}else{
				scattered = Ray(rec.p, refracted);
			}
		}
		float ref_idx;
};




#endif /* MATERIALH */
