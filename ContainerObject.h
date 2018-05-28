/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  A container object is a collection of scene objects that can be 
*  treated as a single object.  For example a cube made up of individual
*  constricted planes.
-------------------------------------------------------------*/

#pragma once

#include "SceneObject.h"

#include <glm/glm.hpp>
#include <vector>

class ContainerObject : public SceneObject
{
protected:
    std::vector<SceneObject*> children;

public:	
	ContainerObject(glm::vec3 location = glm::vec3()) : SceneObject(location)
    {

    }

    bool showBounds = false;

    /** Adds object to container. */
    virtual void add(SceneObject* object);

    /** Intersects ray with object. */
	RayIntersectionResult intersectObject(Ray ray) override; 

    /** Sets material for all child objects. */
    void setMaterial(Material* material)
    {
        this->material = material;
        for (int i = 0; i < children.size(); i++) {
            children[i]->material = material;
        }
    }
    
};
