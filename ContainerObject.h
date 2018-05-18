/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  A container object is a collection of scene objects that can be 
*  treated as a single object.  For example a cube made up of individual
*  constricted planes.
-------------------------------------------------------------*/

#pragma once

#include "Ray.h"
#include "SceneObject.h"

#include <glm/glm.hpp>
#include <vector>

class ContainerObject : public SceneObject
{
protected:
    std::vector<SceneObject*> children;

public:	
	ContainerObject(void) : SceneObject()
    {

    }

    /** Adds object to container. */
    void add(SceneObject* object);

    /** Intersects ray with object. */
	RayIntersectionResult intersect(Ray ray) override;
	
    
};
