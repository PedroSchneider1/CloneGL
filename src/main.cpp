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
const TGAColor blue  = TGAColor(0,   0,   255, 255);

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

void rasterize(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color, int ybuffer[]) {
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }
    for (int x=p0.x; x<=p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t + .5;
        if (ybuffer[x]<y) {
            ybuffer[x] = y;
            image.set(x, 0, color);
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
    { // *Just dumping the 2d scene (yay we have enough dimensions!)
        TGAImage scene(width, height, TGAImage::RGB);

        // *Scene "2d mesh"
        line(20, 34,   744, 400, scene, red);
        line(120, 434, 444, 400, scene, green);
        line(330, 463, 594, 200, scene, blue);

        // Screen line
        line(10, 10, 790, 10, scene, white);

        scene.flip_vertically();
        scene.write_tga_file("scene.tga");
    }

    {
        TGAImage render(width, 16, TGAImage::RGB);
        int ybuffer[width];
        for (int i=0; i<width; i++) {
            ybuffer[i] = std::numeric_limits<int>::min();
        }
        rasterize(Vec2i(20, 34),   Vec2i(744, 400), render, red,   ybuffer);
        rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
        rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue,  ybuffer);

        // *1-pixel wide image is bad for eyes, lets widen it
        for (int i=0; i<width; i++) {
            for (int j=1; j<16; j++) {
                render.set(i, j, render.get(i, 0));
            }
        }
        render.flip_vertically();
        render.write_tga_file("render.tga");
    }
    return 0;
}