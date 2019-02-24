#ifndef HITABLEH
#define HITABLEH

#include "Ray.h"

class Material;

// It is a design choice to select what to store. We store the t of the hit, the point where it hits and its normal.
struct hit_record {
	float t;
	Vec3 p;
	Vec3 normal;
	Material *mat_ptr;
};

class Hitable {
	// a pure virtual function makes sure we always override the function hit
	public:
		virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif
