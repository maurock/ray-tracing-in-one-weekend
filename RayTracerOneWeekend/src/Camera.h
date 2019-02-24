/*
 * Camera.h
 *
 *  Created on: Feb 14, 2019
 *      Author: mauro
 */

#ifndef CAMERAH
#define CAMERAH

#include "Ray.h"

class Camera{
	public:
		// lookfrom is the origin
		// lookat is the point to look at
		// vup, the view up vector to project on the new plane when we incline it. We can also tilt
		// the plane
		Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, float vfov, float aspect){// vfov is top to bottom in degrees, field of view on the vertical axis
			Vec3 w, u, v;
			float theta = vfov * M_PI/180;	// convert to radiants
			float half_height = tan(theta/2);
			float half_width = aspect * half_height;
			origin = lookfrom;
			w = unit_vector(lookfrom - lookat);
			u = unit_vector(cross(vup, w));
			v = cross(w, u);
			lower_left_corner = origin - u*half_width - v*half_height - w;
			horizontal = 2*half_width*u;
			vertical = 2*half_height*v;
		}
		Ray get_ray(float s, float t){ return Ray(origin, lower_left_corner + s * horizontal + t * vertical - origin); }

		Vec3 origin;
		Vec3 lower_left_corner;
		Vec3 horizontal;
		Vec3 vertical;
};

#endif /* CAMERAH */
