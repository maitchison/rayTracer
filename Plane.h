/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#pragma once
#include <glm/glm.hpp>
#include "SceneObject.h"

class Plane : public SceneObject
{
private:
    glm::vec3 v1, v2, v3, v4;  // The four vertices bounding the plane    
    glm::vec3 normal;          // The planes normal. 

    bool bounded; // If true plane is defined by the four vertices, otherwise it is defined by first point and normal.
    

public:	
	Plane(void);
	
    /** Creates a plane as defined by 4 vertices. */
    Plane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4)
	{
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
        this->v4 = v4;
        this->bounded = true;
        // note: in constract to the lab we calculate this on initialization and reuse it, which will be faster than recalculating it each time.
        this->normal = glm::normalize(glm::cross(v2-v1, v4-v1)); 		
	};

    /** Creates a plane as defined by a point and normal. */
    Plane(glm::vec3 v1, glm::vec3 normal)
	{
        this->v1 = v1;
        this->normal = glm::normalize(normal);
        this->bounded = false;        
	};

	bool isInside(glm::vec3 p);
	
	RayIntersectionResult intersect(Ray ray) override;
	
};
