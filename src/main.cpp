// All comments with a '*' are from the author of the tutorial

#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include "headers/tgaimage.h"
#include "headers/model.h"
#include "headers/geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);

Model *model = NULL;
const int width  = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
	// Bresenham's line algorithm
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { // If the line is steep, we transpose the image
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    
	int dx = x1-x0; 
    int dy = y1-y0; 

    float derror = std::abs(dy*2); 
    float error = 0; 
    
	int y = y0;
	if(steep) { // Checking steepness before loop for efficiency (removing branching inside loop)
        for(int x = x0; x<=x1; ++x) {
            image.set(y, x, color);
            error += derror;
            if(error > dx) {
                y += (y1>y0? 1 : -1);
                error -= dx*2;
            }
        }
    } else {
        for(int x = x0; x<=x1; ++x) {
            image.set(x, y, color);
            error += derror;
            if(error > dx) {
                y += (y1>y0? 1 : -1);
                error -= dx*2;
            }
        }
    }
}

Vec3f barycentric(Vec2i *pts, Vec2i P) { 
    Vec3f u = Vec3f(pts[2].x-pts[0].x, pts[1].x-pts[0].x, pts[0].x-P.x)^Vec3f(pts[2].y-pts[0].y, pts[1].y-pts[0].y, pts[0].y-P.y);
    /*
        *`pts` and `P` has integer value as coordinates
        so `abs(u[2])` < 1 means `u[2]` is 0, that means
        triangle is degenerate, in this case return something with negative coordinates
    */
    if (std::abs(u.z)<1) return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
}

void triangle(Vec2i *pts, TGAImage &image, TGAColor color) {
    // Calculate the bounding box for the triangle
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width()-1, image.get_height()-1); 
    for (int i=0; i<3; i++) { 
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
	bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

	bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
	bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }

    // Rasterize the triangle
    Vec2i P; 
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts, P); 
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue; 
            image.set(P.x, P.y, color); 
        } 
    } 
} 

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0,0,-1); // *Define light_dir
    
    for (int i = 0; i < model->nfaces(); i++) { 
        std::vector<int> face = model->face(i); // Get the indices of the vertices of the face
        Vec2i screen_coords[3]; 
        Vec3f world_coords[3]; 
        for (int j = 0; j < 3; j++) { 
            Vec3f v = model->vert(face[j]); // Get the vertex coordinates
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.); // Transform to screen coordinates
            world_coords[j] = v; // Store world coordinates
        } 
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]); // Compute normal vector
        n.normalize(); // Normalize the normal vector
        float intensity = n * light_dir; // Compute the intensity of the light
        if (intensity > 0) { 
            triangle(screen_coords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255)); // Draw the triangle with the computed intensity
        } 
    }

    image.flip_vertically(); // to place the origin in the bottom left corner of the image 
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}