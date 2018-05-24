/**
 * Scene class.
 **/

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Light.h"
#include "ContainerObject.h"

//* Scene containing lights and objects. */
class Scene : public ContainerObject
{
public:

    /** List of lights in the scene. */
    std::vector<Light*> lights = std::vector<Light*>();

    void add(SceneObject* object) override {
        // override so that if we add a light it gets added as a light instead of as a normal object.
        if (dynamic_cast<Light*>(object)) {
            lights.push_back((Light*)object);            
        } else {
            ContainerObject::add(object);
        }
    }
    

};