#pragma once

#include "SceneObject.h"
#include "Utils.h"

class Light : public SceneObject
{
public:
    // color of the light
    Color color;

    // if > 0 then light is treated as an area light of given radius
    float lightRadius = 0.0f;

    // how much ambient light this light generates.
    float ambientLight = 0.1f;

    // enables light shadows
    bool shadow = true;

    Light(glm::vec3 location = glm::vec3(0,0,0), Color color = Color(1,1,1,1)) : SceneObject(location) {
        this->color = color;
    }

};