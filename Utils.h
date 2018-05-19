#pragma once

/** Some extentions to the GLM library */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Color.h"
#include <vector>
#include <iostream>
#include <fstream>

namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v);   
}

// a small constant.
const float EPSILON = 0.00001;


/** Loads a file from disk into at std::vector */
void loadFile(std::vector<unsigned char>& buffer, const std::string& filename);

/** Returns x cliped to between a and b. */
float clipf(float x, float a, float b);

/** Returns x cliped to between a and b. */
int clipi(int x, int a,int b);

/** Returns random number in [0,1] */
float randf();

/** Convert color to 24bit form. */
int colorToInt24(Color color);