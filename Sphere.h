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
    glm::vec3 center;
    float radius;

public:
	Sphere()
		: center(glm::vec3(0)), radius(1)  //Default constructor creates a unit sphere
	{		
	};

    Sphere(glm::vec3 c, float r)
		: center(c), radius(r)
	{	
	};

	RayIntersectionResult intersect(Ray ray) override;

	glm::vec3 normal(glm::vec3 p);

    glm::vec2 getUV(glm::vec3 pos) override;

};
