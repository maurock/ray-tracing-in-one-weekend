#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "float.h"
#include "Camera.h"
#include "HitableList.h"
#include "Sphere.h"
#include "Material.h"

using namespace std;


Vec3 color(const Ray& r, Hitable *world, int depth){
	hit_record rec;
	if(world->hit(r, 0.0001, FLT_MAX, rec)){
		Ray scattered;
		Vec3 attenuation;

		if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			// If the ray hits again, repeat the process until it doesn't hit anything or the depth limit
			return attenuation*color(scattered, world, depth+1);
		}else{
			return Vec3(0,0,0);
		}
	}else{
		Vec3 unit_direction = unit_vector(r.direction());		// Unit vector so -1 < y < 1
		float t = 0.5 * (unit_direction.y() + 1.0);					// Trick to have 0 < t < 1
		// This is called LERP (linear blend, or linear interpolation).
		// It is always in the form: blended_value = (1-t) * start_value + t* end_value
		return (1.0 - t)*Vec3(0.1,0.7, 1.0) + t*Vec3(1,1,1);
	}
}


Hitable *random_scene() {
	cout << "FUNZIA3";
	int n = 380;
	Hitable **list = new Hitable*[n+1];
	list[0] = new Sphere(Vec3(0,-1000,0), 1000, new Metal(Vec3(0.6, 0.6, 0.6), 0.2));

	int i = 1;
	for (int a = -7; a < 7; a++){
		for (int b = -5; b < 5; b++){
			float choose_mat = (double) rand() / double(RAND_MAX);

			Vec3 center(a + 0.9*((double) rand() / double(RAND_MAX)), 0.2, b + 0.9*((double) rand() / double(RAND_MAX)));
			if ((center - Vec3(4,0.2,0)).length() > 0.9){	// Not overlapping with big spheres
				if (choose_mat < 0.8) {	// Lambertian
					list[i++] = new Sphere(center, 0.2, new Lambertian(Vec3(((double) rand() / double(RAND_MAX))*((double) rand() / double(RAND_MAX)),((double) rand() / double(RAND_MAX))*((double) rand() / double(RAND_MAX)) ,((double) rand() / double(RAND_MAX))*((double) rand() / double(RAND_MAX)) )));
				}
				else if(choose_mat < 0.95){		//Metal
					list[i++] = new Sphere(center, 0.2, new Metal(Vec3(0.5*(1 +(double) rand() / double(RAND_MAX)), 0.5*(1 +(double) rand() / double(RAND_MAX)), 0.5*(1 +(double) rand() / double(RAND_MAX))),(double) rand() / double(RAND_MAX)));
				}
				else{		// glass
					list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new Sphere(Vec3(-4, 1, 0), 1, new Lambertian(Vec3(0.1, 0.7, 0.95)));
	//list[i++] = new Sphere(Vec3(4, 1, 0), 1, new Metal(Vec3(0.7, 0.6, 0.5), 0));
	list[i++] = new Sphere(Vec3(4, 1, 0), 1, new Metal(Vec3(0.6, 1.0, 0.6), 0));
	list[i++] = new Sphere(Vec3(0, 1, 0), 1, new Dielectric(1.5));

	return new HitableList(list, i);
}

int main(){
	int nx = 900;
	int ny = 450;
	int ns = 50;	// number of samples for antialiasing
	cout << "FUNZIA1";
	ofstream myfile;
	myfile.open("view.ppm");
	myfile << "P3\n" << nx << " " << ny << "\n255\n";
	/*
	Create world with hitable lists
	Hitable *list[5];
	list[0] = new Sphere(Vec3(0,0,-1), 0.5, new Lambertian(Vec3(0.1, 0.2, 0.5)));
	list[1] = new Sphere(Vec3(0,-100.5,-1), 100, new Lambertian(Vec3(0.8, 0.8, 0.0)));
	list[2] = new Sphere(Vec3(1,0,-1), 0.5, new Metal(Vec3(0.8, 0.6, 0.2), 0));
	list[3] = new Sphere(Vec3(-1,0,-1), 0.5, new Dielectric(1.5));
	list[4] = new Sphere(Vec3(-1,0,-1), 0.45, new Dielectric(1.5));
	Hitable *world = new HitableList(list, 5);
	*/

	Hitable *world = random_scene();

	//cout << "FUNZIA2";
	Camera cam(Vec3(9,2, 3), Vec3(-1,0,0), Vec3(0,1,0), 30, float(nx)/float(ny));

	for (int j = ny - 1; j>=0; j--){
		for (int i = 0; i<nx; i++){

			// Add Antialiasing. I shoot ns rays in different location inside a pixel,
			// and average them.
			Vec3 col(0, 0, 0);
			for (int s=0; s < ns; s++){

				// u and v represents the percentage of the horizontal and vertical values
				float u = float(i + rand() / double(RAND_MAX)) / float(nx);
				float v = float(j + rand() / double(RAND_MAX)) / float(ny);
				Ray r = cam.get_ray(u,v);
				Vec3 p = r.point_at_parameter(2.0);

				// Components range from 0.0 to 1.0
				col += color(r, world, 0);
			}
			col /= float(ns);
			// Gamma correction
			col = Vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			// Convert to RGB
			int ir = int(255.99* col[0]);
			int ig = int(255.99* col[1]);
			int ib = int(255.99* col[2]);

			myfile << ir << " " << ig << " " << ib << "\n";
		}
	}
	myfile.close();
}





/* IF WE ONLY HAVE ONE SPHERE
// Calculate when the discriminant of the equation of a vector hitting a sphere
// is higher than zero -> it hits the sphere, and where it hits
float hit_sphere(const Vec3& center, float radius, const Ray& ray){
	// b^2 - 4 * a * c. Look the math for the actual values inside
	Vec3 oc = ray.origin() - center;	// A - C
	float a = dot(ray.direction(), ray.direction());
	float b = 2.0 * dot(ray.direction(), oc);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b*b - 4*a*c;
	if (discriminant < 0){
		return -1.0;
	}else{
		// Calculate the variable in the equation secondo grado. We take the smallest
		return (-b-sqrt(discriminant)/ (2.0 *a));
	}
} */

/* SIMPLE VERSION. COLOR JUST ON SPHERE
// Create function that returns the color of the background.
// This specific color linearly blends white and blue depending on y
Vec3 color(const Ray& r){
	float t = hit_sphere(Vec3(0,0,-1),0.5,r);
	if(t>=0.0){
		// To visualize the Normal each component is mapped to interval (0,1), and
		// (x,y,z) mapped to (r,g,b)
		// r.point_at_parameter returns the point in space where the ray hits the sphere
		// subtracting to the origin, returns a vector from the origin to the point where it hits the sphere
		Vec3 N = unit_vector(r.point_at_parameter(t) - Vec3(0,0,-1));
		cout << N.x() << " " << N.y() << " " << N.z() << " " << endl;
		return 0.5 * Vec3(N.x()+1,N.y()+1,N.z()+1);
	}
	Vec3 unit_direction = unit_vector(r.direction());		// Unit vector so -1 < y < 1
	t = 0.5 * (unit_direction.y() + 1.0);
	// Trick to have 0 < t < 1
	// This is called LERP (linear blend, or linear interpolation).
	// It is always in the form: blended_value = (1-t) * start_value + t* end_value
	return (1.0 - t)*Vec3(1,1,1) + t*Vec3(0.5,0.7, 1.0);
}*/
