#ifndef SPHEREH
#define SPHEREH

#include "Hitable.h"

class Sphere: public Hitable {
	public:
		Sphere();
		Sphere(Vec3 cen, float r, Material *mat) : center(cen), radius(r), mat_ptr(mat) {};
		virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec) const;
		Vec3 center;
		float radius;
		Material *mat_ptr;
};

// Calculate when the discriminant of the equation of a vector hitting a sphere
// is higher than zero
bool Sphere::hit(const Ray& r, float tmin, float tmax, hit_record& rec) const {
	Vec3 oc = r.origin() - center;	// A - C
	float a = dot(r.direction(), r.direction());
	float b =  dot(r.direction(), oc);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b*b - a*c;
	if (discriminant > 0){
		// Calculate the variable in the equation secondo grado. We take the smallest
		float temp = (-b -sqrt(b*b - a*c))/a;
		// The hits only count if the t is in an interval. Ray tracers find this useful
		if (temp < tmax && temp > tmin){
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center)/radius;	// Divided by radius to have unit length
			rec.mat_ptr = mat_ptr;
			return true;
			}
		// Here we take the biggest solution to see if it's in the interval
		temp = (-b -sqrt(b*b - a*c))/a;
		if (temp < tmax && temp > tmin){
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center)/radius;		// Divided by radius to have unit length
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

#endif
