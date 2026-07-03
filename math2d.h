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
typedef struct Collision {
    bool overlap;
    Axis normal;
    Vec1D depth;
} Collision;
typedef Vec1D Friction;
typedef Vec1D Restitution;

typedef Vec1D Orientation;
typedef Vec2D Position;
typedef Vec2D Velocity;
typedef Orientation AngularVelocity;
typedef Orientation AngularAcceleration;
typedef Vec2D Acceleration;
typedef Vec2D Force;
typedef float Mass;
typedef Orientation Torque;


Vec2DList create_normals(Shape shape);
Vec2D normalize_vector(Vec2D vector);
Vec2DList normalize_vectors(Vec2DList vectors);
float dot_product(Vec2D vector_1, Vec2D vector_2);
Shape create_square(float width, float height);
Shape create_circle(float radius, uint8_t verticies);
Projection project_shape_on_axis(Shape shape, Axis axis);
Shape shape_world_translate(Shape shape, Position position, Orientation angle);
bool shape_overlap(Shape shape_1, Shape shape_2);
float polygon_moment_of_inertia(Shape shape, Mass mass);
Collision sat_collision(Shape shape_1, Shape shape_2);
float cross_2d(Vec2D a, Vec2D b);
Vec2D angular_velocity_cross_vec(float omega, Vec2D r);
Position approximate_contact_point(Position p1, Position p2);
Vec2D project_onto_axis(Vec2D v, Axis axis);
float axis_magnitude(Axis axis);
#endif
