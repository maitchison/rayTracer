#pragma once

#include "SceneObject.h"
#include "Utils.h"

class Light : public SceneObject
{
public:
    // color of the light
    Color color;

    // how much ambient light this light generates.
    float ambientLight = 0.1f;

    // enables light shadows
    bool shadow = true;

    Light(glm::vec3 location = glm::vec3(0,0,0), Color color = Color(1,1,1,1)) : SceneObject(location) {
        this->color = color;
    }

};