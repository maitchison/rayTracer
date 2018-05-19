#pragma once

/** Some extentions to the GLM library */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v);
    
}