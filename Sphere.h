/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#pragma once
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple Sphere located at 'center'
 * with the specified radius
 */
class Sphere : public SceneObject
{

private:    
    float radius;

public:

    /** Create a default unit sphere. */
	Sphere(): SceneObject() 
	{		
        this->radius = 1;
	};

    Sphere(glm::vec3 location, float radius) : SceneObject()
	{	
        this->location = location;
        this->radius = radius;
	};

	RayIntersectionResult intersect(Ray ray) override;

	glm::vec3 normal(glm::vec3 p);

    glm::vec2 getUV(glm::vec3 pos) override;

};
