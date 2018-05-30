#pragma once

#include "SceneObject.h"
#include "Utils.h"

class Light : public SceneObject
{
public:
    // color of the light
    Color color;

    // if > 0 then light is treated as an area light over a cube of this length.
    float lightSize = 0.0f;

    // how much ambient light this light generates.
    float ambientLight = 0.1f;

    // enables light shadows
    bool shadow = true;

    Light(glm::vec3 location = glm::vec3(0,0,0), Color color = Color(1,1,1,1)) : SceneObject(location) {
        this->color = color;
    }

    /** Samples the lights locations, a sample will be drawn from the lights volume. */
    glm::vec3 sampleLocation() {
        if (lightSize <= 0.0f) 
            return this->getLocation();
        else {       
            return this->getLocation() + glm::vec3(
                ((randf()-0.5f)*lightSize),
                ((randf()-0.5f)*lightSize),
                ((randf()-0.5f)*lightSize)
            );
        }
    }

};