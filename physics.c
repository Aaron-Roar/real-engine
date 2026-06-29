#include "entity_components.h"

#define MAX_VECTORS 100

typedef struct Vectors {
    int vector_amount;
    Vec2D vectors[MAX_VECTORS];
} Vectors;

//Not tested
Vectors create_normals(Shape shape) {
    Vectors normals = {0};
    if (shape.vertex_amount <= 1 || shape.vertex_amount > MAX_VECTORS) {
        //Error
        return normals;
    }

    normals.vector_amount = shape.vertex_amount;

    //Could be seg fault if out of range!!
    for(int i = 0; i < shape.vertex_amount; i++) {
        int j = (i + 1)%shape.vertex_amount;  //makes the last vertex wrap to the first one

        // Getting vector by final-initial then doing perpendicular
        // (x, y) -> PERPENDICULAR -> (-y, x)
        normals.vectors[i].y = -(shape.vertices[j].x - shape.vertices[i].x);
        normals.vectors[i].x = shape.vertices[j].y - shape.vertices[i].y;
    }

    return normals;
}

