#include <math.h>
#include "math2d.h"
#include <float.h>

#define PI_F 3.14159265358979323846f

float list_maximum(Vec1DList list) {
    float max_value = list.vectors[0];
    for(int i = 0; i < list.amount_of_vectors; i += 1) {
        if( list.vectors[i] > max_value)
            max_value = list.vectors[i];
    }
    return max_value;
}

float list_minimum(Vec1DList list) {
    float min_value = list.vectors[0];
    for(int i = 0; i < list.amount_of_vectors; i += 1) {
        if( list.vectors[i] < min_value)
            min_value = list.vectors[i];
    }
    return min_value;
}

Vec2DList create_normals(Shape shape) {
    Vec2DList normals = {0};
    if (shape.amount_of_vertices <= 1 || shape.amount_of_vertices > MAX_VECTORS) {
        //Error
        return normals;
    }

    normals.amount_of_vectors = shape.amount_of_vertices;

    //Could be seg fault if out of range!!
    for(int i = 0; i < shape.amount_of_vertices; i++) {
        int j = (i + 1)%shape.amount_of_vertices;  //makes the last vertex wrap to the first one

        // Getting vector by final-initial then doing perpendicular
        // (x, y) -> PERPENDICULAR -> (-y, x)
        normals.vectors[i].y = -(shape.vertices[j].x - shape.vertices[i].x);
        normals.vectors[i].x = shape.vertices[j].y - shape.vertices[i].y;
    }

    return normals;
}

Vec2D normalize_vector(Vec2D vector) {
    return (Vec2D){
        .x = vector.x/sqrt(vector.x*vector.x + vector.y*vector.y),
        .y = vector.y/sqrt(vector.x*vector.x + vector.y*vector.y),
    };
}

Vec2DList normalize_vectors(Vec2DList vectors) {
    Vec2DList normalized_vecs = {0};
    normalized_vecs.amount_of_vectors = vectors.amount_of_vectors;
    for(int i = 0; i < vectors.amount_of_vectors; i += 1) {
        normalized_vecs.vectors[i] = normalize_vector(vectors.vectors[i]);
    }
    return normalized_vecs;
}

float dot_product(Vec2D vector_1, Vec2D vector_2) {
    return vector_1.x*vector_2.x + vector_1.y*vector_2.y;
}

Shape create_square(float width, float height) {
    Shape shape = {
        .amount_of_vertices = 4,
        .vertices = {
            {.x = width/2, .y = height/2},
            {.x = width/2, .y = -height/2},
            {.x = -width/2, .y = -height/2},
            {.x = -width/2, .y = height/2},
        }
    };
    return shape;
}

Shape create_circle(float radius, uint8_t verticies) {
    Shape shape = {0};
    if(verticies < MIN_VERTICIES) {
        shape.amount_of_vertices = MIN_VERTICIES;
    }
    else if(verticies > MAX_VERTICIES) {
        shape.amount_of_vertices = MAX_VERTICIES;
    }
    else {
        shape.amount_of_vertices = verticies;
    }

    float angle_increment = ((float)2*PI_F)/((float)shape.amount_of_vertices);
    for(int i = 0; i < shape.amount_of_vertices; i++) {
        shape.vertices[i].x = cosf(angle_increment*i)*radius;
        shape.vertices[i].y = sinf(angle_increment*i)*radius;
    }
    return shape;
}

Projection project_shape_on_axis(Shape shape, Axis axis) {
    Vec1DList list = {0};
    list.amount_of_vectors = shape.amount_of_vertices;
    for(int i = 0; i < shape.amount_of_vertices; i += 1) {
        list.vectors[i] = dot_product(shape.vertices[i], axis);
    }
    return (Projection){
        .max = list_maximum(list),
        .min = list_minimum(list)
    };
}

Position polygon_centroid(Shape shape)
{
    double area_sum = 0.0;
    double cx_sum = 0.0;
    double cy_sum = 0.0;

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        int j = (i + 1) % shape.amount_of_vertices;

        double xi = shape.vertices[i].x;
        double yi = shape.vertices[i].y;
        double xj = shape.vertices[j].x;
        double yj = shape.vertices[j].y;

        double cross = xi * yj - xj * yi;

        area_sum += cross;
        cx_sum += (xi + xj) * cross;
        cy_sum += (yi + yj) * cross;
    }

    double area = area_sum * 0.5;

    if (fabs(area) < 1e-8) {
        // Degenerate polygon fallback: average vertices
        Position avg = {0};

        for (int i = 0; i < shape.amount_of_vertices; i++) {
            avg.x += shape.vertices[i].x;
            avg.y += shape.vertices[i].y;
        }

        avg.x /= shape.amount_of_vertices;
        avg.y /= shape.amount_of_vertices;

        return avg;
    }

    Position centroid = {
        .x = cx_sum / (6.0 * area),
        .y = cy_sum / (6.0 * area),
    };

    return centroid;
}

Shape shape_world_translate(Shape shape, Position position, Orientation angle) {
    Shape world_shape = {0};
    world_shape.amount_of_vertices = shape.amount_of_vertices;

    Position center = polygon_centroid(shape);

    float cos_a = cosf(angle);
    float sin_a = sinf(angle);

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        float x = shape.vertices[i].x - center.x;
        float y = shape.vertices[i].y - center.y;

        float rotated_x = x*cos_a - y*sin_a;
        float rotated_y = x*sin_a + y*cos_a;

        world_shape.vertices[i].x = position.x + rotated_x;
        world_shape.vertices[i].y = position.y + rotated_y;
    }

    return world_shape;
}

float projection_overlap(Projection projection_1, Projection projection_2) {
    return fminf(projection_1.max, projection_2.max) - fmaxf(projection_1.min, projection_2.min);
}

bool shape_overlap_on_axes(Shape shape_1, Shape shape_2, Vec2DList axes) {
    for (int i = 0; i < axes.amount_of_vectors; i += 1) {
        Axis axis = axes.vectors[i];

        Projection p1 = project_shape_on_axis(shape_1, axis);
        Projection p2 = project_shape_on_axis(shape_2, axis);

        float overlap = projection_overlap(p1, p2);

        if (overlap <= 0.0f) {
            return false;
        }
    }

    return true;
}

bool shape_overlap(Shape shape_1, Shape shape_2) {
    Vec2DList shape1_normals = normalize_vectors(create_normals(shape_1));
    Vec2DList shape2_normals = normalize_vectors(create_normals(shape_2));

    if (!shape_overlap_on_axes(shape_1, shape_2, shape1_normals)) {
        return false;
    }

    if (!shape_overlap_on_axes(shape_1, shape_2, shape2_normals)) {
        return false;
    }

    return true;
}


float polygon_moment_of_inertia(Shape shape, Mass mass)
{
    Position c = polygon_centroid(shape);

    float area_sum = 0.0f;
    float inertia_sum = 0.0f;

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        int j = (i + 1) % shape.amount_of_vertices;

        float xi = shape.vertices[i].x - c.x;
        float yi = shape.vertices[i].y - c.y;

        float xj = shape.vertices[j].x - c.x;
        float yj = shape.vertices[j].y - c.y;

        float cross = xi * yj - xj * yi;

        float q =
            xi*xi + xi*xj + xj*xj +
            yi*yi + yi*yj + yj*yj;

        area_sum += cross;
        inertia_sum += cross * q;
    }

    float area = 0.5f * area_sum;
    float area_moment = inertia_sum / 12.0f;

    if (fabsf(area) < 1e-8f) {
        return 0.0f; // invalid/degenerate polygon
    }

    float density = mass / fabsf(area);
    float inertia = density * fabsf(area_moment);

    return inertia;
}


Collision sat_collision_on_axes(Shape shape_1, Shape shape_2, Vec2DList axes, Collision collision) {
    for (int i = 0; i < axes.amount_of_vectors; i += 1) {
        Axis axis = axes.vectors[i];

        Projection p1 = project_shape_on_axis(shape_1, axis);
        Projection p2 = project_shape_on_axis(shape_2, axis);

        float overlap = projection_overlap(p1, p2);

        if (overlap <= 0.0f) {
            return (Collision){ .overlap = false };
        }

        if (overlap < collision.depth) {
            collision.depth = overlap;
            collision.normal = axis;
        }
    }

    return collision;
}

Collision sat_collision(Shape shape_1, Shape shape_2)
{
    Collision collision = {
        .overlap = true,
        .normal = {0},
        .depth = FLT_MAX
    };

    Vec2DList shape1_axes = normalize_vectors(create_normals(shape_1));
    Vec2DList shape2_axes = normalize_vectors(create_normals(shape_2));

    collision = sat_collision_on_axes(shape_1, shape_2, shape1_axes, collision);

    if (!collision.overlap) {
        return collision;
    }

    collision = sat_collision_on_axes(shape_1, shape_2, shape2_axes, collision);

    if (!collision.overlap) {
        return collision;
    }

    Position c1 = polygon_centroid(shape_1);
    Position c2 = polygon_centroid(shape_2);

    Vec2D center_delta = {
        .x = c2.x - c1.x,
        .y = c2.y - c1.y
    };

    if (dot_product(center_delta, collision.normal) < 0.0f) {
        collision.normal.x *= -1.0f;
        collision.normal.y *= -1.0f;
    }

    return collision;
}

float cross_2d(Vec2D a, Vec2D b)
{
    return a.x * b.y - a.y * b.x;
}

Vec2D angular_velocity_cross_vec(float omega, Vec2D r)
{
    return (Vec2D){
        .x = -omega * r.y,
        .y =  omega * r.x
    };
}

//uses center (wrong for now)
Position approximate_contact_point(Position p1, Position p2)
{
    return (Position){
        .x = (p1.x + p2.x) * 0.5f,
        .y = (p1.y + p2.y) * 0.5f
    };
}

Vec2D project_onto_axis(Vec2D v, Axis axis) {
    float amount = dot_product(v, axis);

    return (Vec2D){
        .x = axis.x * amount,
        .y = axis.y * amount
    };
}

float vector_magnitude(Vec2D vector) {
    return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

float axis_magnitude(Axis axis) {
    return sqrtf(axis.x * axis.x + axis.y * axis.y);
}

Vec2D rotate_vector(Vec2D vector, Orientation angle) {
    float c = cosf(angle);
    float s = sinf(angle);

    return (Vec2D){
        .x = vector.x * c - vector.y * s,
        .y = vector.x * s + vector.y * c
    };
}
