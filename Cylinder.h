/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cylinder class
-------------------------------------------------------------*/

#pragma once
#include <glm/glm.hpp>
#include "SceneObject.h"
#include "Utils.h"

class Cylinder : public SceneObject
{
private:
    float radius;
    float height;  
    // enables cylinder caps  
    bool capped = true;

public:	
	Cylinder() : SceneObject()
    {
        this->radius = 1;
        this->height = 1;
    }
	
    /** Creates a cylinder . */
    Cylinder(glm::vec3 location, float radius, float height)
	{
        this->location = location;
        this->radius = radius;
        this->height = height;        
	};
	
	RayIntersectionResult intersect(Ray ray) override;

    glm::vec2 getUV(glm::vec3 pos) override;
	
};
