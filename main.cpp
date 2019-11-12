#include <iostream>
#include <fstream>
#include <limits>

#include "vec3.h"
#include "ray.h"
#include "camera.h"
#include "hitable.h"
#include "material.h"


vec3 color(const ray& r, hitable *world, int depth) {
  hit_record rec;
  if (world->hit(r, 0.001, std::numeric_limits<float>::max(), rec)) {
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * color(scattered, world, depth+1);
    } else {
      return vec3(0,0,0);
    }
  } else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
  }
}


int main() {
  
  int nx = 800;
  int ny = 400;
  int ns = 10;

  std::ofstream fs;
  fs.open("ray_tracing.ppm");

  fs << "P3\n" << nx << " " << ny << "\n255\n";


  hitable *list[4];
  list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.8,0.3,0.3)));
  list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8,0.8,0.0)));
  list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 0.9));
  list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8,0.8,0.8), 0.01));
  hitable *world = new hitable_list(list,4);

  camera cam;


  for (int j = ny-1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      vec3 col(0, 0, 0);

      for (int s = 0; s < ns; s++) {

        float u = float(i + rand_uni(gen)) / float(nx);
        float v = float(j + rand_uni(gen)) / float(ny);
        ray r = cam.get_ray(u, v);
        col += color(r, world, 0);
      }

      col /= float(ns);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));


      int ir = int(255.99*col[0]);
      int ig = int(255.99*col[1]);
      int ib = int(255.99*col[2]);

      fs << ir << " " << ig << " " << ib << "\n";
    }
  }

  fs.close();

  return 0;
}
