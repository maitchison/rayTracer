/**
 * A cube is modeled as collection of 6 bounded planes.
 */

#pragma once
#include <glm/glm.hpp>

#include "Plane.h"
#include "ContainerObject.h"

class Cube : public ContainerObject
{
public:

    Cube() : ContainerObject() {

    }

    /** Creates a unit cube */
    Cube(glm::vec3 location, glm::vec3 scale = glm::vec3(1,1,1)) : ContainerObject(location) {

        float w = scale.x/2.0f;
        float h = scale.y/2.0f;
        float d = scale.z/2.0f;

        glm::vec3 v1 = glm::vec3(-w,-h,-d);
        glm::vec3 v2 = glm::vec3(+w,-h,-d);
        glm::vec3 v3 = glm::vec3(+w,+h,-d);
        glm::vec3 v4 = glm::vec3(-w,+h,-d);

        glm::vec3 v5 = glm::vec3(-w,-h,+d);
        glm::vec3 v6 = glm::vec3(+w,-h,+d);
        glm::vec3 v7 = glm::vec3(+w,+h,+d);
        glm::vec3 v8 = glm::vec3(-w,+h,+d);

        add(new Plane(v1,v2,v3,v4)); // front
        add(new Plane(v5,v6,v7,v8)); // back
        add(new Plane(v4,v3,v7,v8)); // top
        add(new Plane(v2,v1,v5,v6)); // bottom        
        add(new Plane(v3,v2,v6,v7)); // left
        add(new Plane(v1,v4,v8,v5)); // right                                        

        // set the radius
        boundingSphereRadius = glm::length(scale);
    }

};