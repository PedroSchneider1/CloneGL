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

bool isDegenerate(Vec2i t0, Vec2i t1, Vec2i t2) {
    return (t0.x == t1.x && t0.x == t2.x) || (t0.y == t1.y && t0.y == t2.y);
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
    if (isDegenerate(t0, t1, t2)) return; // Early exit for degenerate triangles

    // Sort the vertices by y-coordinate ascending
    std::vector<Vec2i> vertices = {t0, t1, t2};
    std::sort(vertices.begin(), vertices.end(), [](const Vec2i &a, const Vec2i &b) {
        return a.y < b.y;
    });
    
    t0 = vertices[0];
    t1 = vertices[1];
    t2 = vertices[2];

    // Paints the inside of the triangle
    int total_height = t2.y - t0.y;
    for(int i=0; i<total_height; i++){
        bool second_half = i > t1.y - t0.y || t1.y == t0.y; // If we are in the second half of the triangle
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i/ total_height;
        float beta = (float)(i-(second_half ? t1.y - t0.y : 0))/ segment_height; // *Be careful: with above conditions no division by zero here
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (A.x>B.x) std::swap(A, B); 
        for (int j=A.x; j<=B.x; j++) { 
            image.set(j, t0.y+i, color); // *Attention, due to int casts t0.y+i != A.y 
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
    // for (int i=0; i<model->nfaces(); i++) {
    //     std::vector<int> face = model->face(i);
    //     for (int j=0; j<3; j++) {
    //         Vec3f v0 = model->vert(face[j]);
    //         Vec3f v1 = model->vert(face[(j+1)%3]);
    //         int x0 = (v0.x+1.)*width/2.;
    //         int y0 = (v0.y+1.)*height/2.;
    //         int x1 = (v1.x+1.)*width/2.;
    //         int y1 = (v1.y+1.)*height/2.;
    //         line(x0, y0, x1, y1, image, white);
    //     }
    // }

    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}