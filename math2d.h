#ifndef MATH_2D_H
#define MATH_2D_H
#include <stdint.h>
#include <stdbool.h>

#define MAX_VECTORS 100
#define MAX_VERTICIES 50
#define MIN_VERTICIES 3

#define PI_F 3.14159265358979323846f
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


Vec2DList create_normals(Shape shape);
Vec2D normalize_vector(Vec2D vector);
Vec2DList normalize_vectors(Vec2DList vectors);
float dot_product(Vec2D vector_1, Vec2D vector_2);
Shape create_square(float width, float height);
Shape create_circle(float radius, uint8_t verticies);
Projection project_shape_on_axis(Shape shape, Axis axis);
bool shape_overlap(Shape shape_1, Shape shape_2);
float cross_2d(Vec2D a, Vec2D b);
Vec2D angular_velocity_cross_vec(float omega, Vec2D r);
Vec2D project_onto_axis(Vec2D v, Axis axis);
float axis_magnitude(Axis axis);
float vector_magnitude(Vec2D vector);
Vec2D rotate_vector(Vec2D vector, float angle);
Vec1D circle_radius(Shape circle, Vec2D centroid);
Vec2D vector_subtract(Vec2D vector_a, Vec2D vector_b);
Vec1D circle_overlap_depth(Vec2D centroid_1, Vec1D radius_1, Vec2D centroid_2, Vec1D radius_2);
float projection_overlap(Projection projection_1, Projection projection_2);
Shape scale_shape(Shape shape, float scale);
Shape scale_shape_y(Shape shape, float scale);
Shape scale_shape_x(Shape shape, float scale);
Vec2D polygon_centroid(Shape shape);
Shape add_vertex(Shape shape);
Shape delete_vertex(Shape shape);
#endif
