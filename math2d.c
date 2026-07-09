#include <math.h>
#include "math2d.h"
#include <float.h>


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

Vec2D rotate_vector(Vec2D vector, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);

    return (Vec2D){
        .x = vector.x * c - vector.y * s,
        .y = vector.x * s + vector.y * c
    };
}

//Circles
Vec1D circle_radius(Shape circle, Vec2D centroid) {
  return sqrt(
      (circle.vertices[0].x - centroid.x)*(circle.vertices[0].x - centroid.x)
      + (circle.vertices[0].y - centroid.y)*(circle.vertices[0].y - centroid.y)
      );
}

Vec2D vector_subtract(Vec2D vector_a, Vec2D vector_b) {
  return (Vec2D){.x = (vector_a.x - vector_b.x), .y = (vector_a.y - vector_b.y)};
}

Vec1D circle_overlap_depth(Vec2D centroid_1, Vec1D radius_1, Vec2D centroid_2, Vec1D radius_2) {
  return (radius_1 + radius_2) - sqrt(
      (centroid_1.x - centroid_2.x)*(centroid_1.x - centroid_2.x) 
      +(centroid_1.y - centroid_2.y)*(centroid_1.y - centroid_2.y)
      );
}
