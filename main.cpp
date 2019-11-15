#include <iostream>
#include <fstream>
#include <limits>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "material.h"
#include "camera.h"


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

hitable *random_scene() {
  int n = 110;
  hitable **list = new hitable*[n+1];
  list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5,0.5,0.5)));
  int i = 1;

  for (int a = -5; a < 5; a++) {
    for (int b = -5; b < 5; b++) {
      float choose_mat = rand_uni(gen);
      vec3 center(a+0.9*rand_uni(gen), 0.2, b+0.9*rand_uni(gen));
      if ((center - vec3(4,0.2,0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          list[i++] = new sphere(center, 0.2, new lambertian(vec3(rand_uni(gen)*rand_uni(gen),
                                                                  rand_uni(gen)*rand_uni(gen),
                                                                  rand_uni(gen)*rand_uni(gen))));
        } else if (choose_mat < 0.95) {
          list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1+rand_uni(gen)),
                                                             0.5*(1+rand_uni(gen)),
                                                             0.5*(1+rand_uni(gen))), 
                                                        0.5*rand_uni(gen)));
        } else {
          list[i++] = new sphere(center, 0.2, new dielectric(1.5));
        }
      }
    }
  }

  list[i++] = new sphere(vec3(0,1,0),  1, new dielectric(1.5));
  list[i++] = new sphere(vec3(-4,1,0), 1, new lambertian(vec3(0.4,0.2,0.1)));
  list[i++] = new sphere(vec3(4,1,0), 1, new metal(vec3(0.7,0.6,0.5), 0));
  return new hitable_list(list, i);
}


int main() {
  
  int nx = 1200;
  int ny = 600;
  int ns = 100;

  std::ofstream fs;
  fs.open("ray_tracing.ppm");

  fs << "P3\n" << nx << " " << ny << "\n255\n";

//  hitable *list[5];
//  list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1,0.2,0.5)));
//  list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8,0.8,0.0)));
//  list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8,0.6,0.2), 0.3));
//  list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
//  list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
//  hitable *world = new hitable_list(list,5);
  hitable *world = random_scene();
  

  //camera cam(90, float(nx)/float(ny));
  vec3 lookfrom(25,2.5,10);
  vec3 lookat(0,0,0);
  float dist_to_focus = (lookfrom-lookat).length();
  float aperture = 0.5;
  camera cam(lookfrom, lookat, vec3(0,1,0), 10, float(nx)/float(ny), aperture, dist_to_focus);


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
    std::cout << j << std::endl;
  }

  fs.close();

  return 0;
}
