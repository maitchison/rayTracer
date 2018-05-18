/**
 * Contains various polyhedrons
 * 
 * (just the cube for the moment...)
 */

#pragma once
#include <glm/glm.hpp>

#include "Plane.h"
#include "ContainerObject.h"

class Polyhedron : public ContainerObject
{
public:

    Polyhedron() : ContainerObject() {

    }

    /** Creates a unit cube */
    static Polyhedron* Cube(glm::vec3 pos) {

        glm::vec3 v1 = glm::vec3(-0.5,-0.5,-0.5) + pos;
        glm::vec3 v2 = glm::vec3(+0.5,-0.5,-0.5) + pos;
        glm::vec3 v3 = glm::vec3(+0.5,+0.5,-0.5) + pos;
        glm::vec3 v4 = glm::vec3(-0.5,+0.5,-0.5) + pos;

        glm::vec3 v5 = glm::vec3(-0.5,-0.5,+0.5) + pos;
        glm::vec3 v6 = glm::vec3(+0.5,-0.5,+0.5) + pos;
        glm::vec3 v7 = glm::vec3(+0.5,+0.5,+0.5) + pos;
        glm::vec3 v8 = glm::vec3(-0.5,+0.5,+0.5) + pos;

        Polyhedron* cube = new Polyhedron();

        cube->add(new Plane(v1,v2,v3,v4));
        cube->add(new Plane(v5,v6,v7,v8));
        cube->add(new Plane(v4,v3,v7,v8));
        cube->add(new Plane(v2,v1,v5,v6));
        cube->add(new Plane(v3,v2,v6,v7));
        cube->add(new Plane(v8,v3,v1,v4));

        return cube;
    }

};