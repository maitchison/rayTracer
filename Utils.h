#pragma once

/** Some extentions to the GLM library */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Color.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v);   
}

// a small constant.
const float EPSILON = 0.00001;

// math doesn't always have this defined for some reason.
#define PI 3.141592654f


/** Loads a file from disk into at std::vector */
void loadFile(std::vector<unsigned char>& buffer, const std::string& filename);

/** Returns x cliped to between a and b. */
float clipf(float x, float a, float b);

/** Returns x cliped to between a and b. */
int clipi(int x, int a,int b);

/** Returns random number in [0,1] */
float randf();

/** Returns fractional part of f (signed) */
float frac(float f);

/** Returns max of a,b */
float maxf(float a, float b);

/** Convert color to 24bit form. */
int colorToInt24(Color color);

/** Returns a rotation matrix for given euler angles (in degrees) */
glm::mat4x4 EulerRotationMatrix(glm::vec3 rotation);

/** Randomly rotate vector r radians from it's current location. */
glm::vec3 defocus(glm::vec3 v, float r);

/** Similar to defocus, in that is rotates v a little bit, but much quicker.  d here is in units, so use small values. */
glm::vec3 distort(glm::vec3 v, float d);

/** Returns distance ray must travel before it hits sphere, or 0 if ray does not intersect sphere. */
float raySphereIntersection(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3 sphereLocation, float radius);

void print(glm::mat4x4 m);
void print(glm::vec4 v);
void print(glm::vec3 v);