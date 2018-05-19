#include "Tools.h"

namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v) 
    {
        return dot(v, v);
    }
    
}
