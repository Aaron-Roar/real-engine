#ifndef MATH_2D_H
#define MATH_2D_H
#include <stdint.h>
#include <stdbool.h>

#define MAX_VECTORS 100
#define MAX_VERTICIES 50
#define MIN_VERTICIES 4

typedef float Vec1D;
typedef struct {
    float x;
    float y;
} Vec2D;
typedef struct Vec1DList {
    uint16_t amount_of_vectors;
    Vec1D vectors[MAX_VECTORS];
} Vec1DList;
typedef struct Vec2DList {
    uint16_t amount_of_vectors;
    Vec2D vectors[MAX_VECTORS];
} Vec2DList;
//Moveable Objects
typedef struct Shape {
    uint16_t amount_of_vertices;
    Vec2D vertices[MAX_VERTICIES]; //Local coordinate of vertices. Must be translated to world coordinate
} Shape;
typedef Vec2D Axis;
typedef struct Projection {
    float min;
    float max;
} Projection;

typedef Vec1D Orientation;
typedef Vec2D Position;
typedef Vec2D Velocity;
typedef Vec2D Acceleration;
typedef Vec2D Force;
typedef float Mass;

Vec2DList create_normals(Shape shape);
Vec2D normalize_vector(Vec2D vec);
Vec2DList normalize_vectors(Vec2DList vecs);
float dot_product(Vec2D v1, Vec2D v2);
Shape create_square(float width, float height);
Shape create_circle(float radius, uint8_t verticies);
Projection project_shape_on_axis(Shape shape, Axis axis);
Shape shape_world_translate(Shape shape, Position pos, Orientation angle);
bool shape_overlap(Shape shape1, Shape shape2);
#endif
