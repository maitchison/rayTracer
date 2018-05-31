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
#include <math.h>

class Plane : public SceneObject
{
protected:
    glm::vec3 v1, v2, v3, v4;  // The four vertices bounding the plane    
    glm::vec3 normal;          // The planes normal. 
    glm::vec3 tangent;         // The planes tangent, facing in the 'up' direction. 
    glm::vec3 bitangent;       // The bitangent vector

    bool bounded; // If true plane is defined by the four vertices, otherwise it is defined by first point and normal.

    glm::vec2 uvScale;
    

public:	
	Plane() : SceneObject()
    {

    }
	
    /** Creates a bounded plane as defined by 4 vertices. */
    Plane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) : SceneObject()
	{
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
        this->v4 = v4;
        this->bounded = true;
        // note: in contrast to the lab we calculate this on initialization and reuse it, which will be faster than recalculating it each time.
        this->normal = glm::normalize(glm::cross(v2-v1, v4-v1)); 		
        this->tangent = glm::normalize(v2-v1); 		
        this->bitangent = glm::normalize(v4-v1); 
        this->uvScale = 1.0f/glm::vec2(glm::length(v2-v1), glm::length(v4-v1));                
	};

    /** Creates a plane as defined by a point a normal, and an 'up' direction. */
    Plane(glm::vec3 v1, glm::vec3 normal, glm::vec3 up = glm::vec3(0,1,0)) : SceneObject()
	{
        this->v1 = v1;
        this->normal = glm::normalize(normal);
        this->tangent = glm::normalize(up);
        this->bounded = false;        
        this->bitangent = glm::cross(this->tangent, this->normal);        
        this->uvScale = 1.0f/glm::vec2(glm::length(normal), glm::length(up));
	};

	virtual bool isInside(glm::vec3 p);
	
	RayIntersectionResult intersectObject(Ray ray) override;

    glm::vec2 getUV(glm::vec3 pos) override;    
	
};

class Triangle : public Plane
{
public:	
	
    Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) : Plane()
	{
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
        this->bounded = true;
        this->normal = glm::normalize(glm::cross(v2-v1, v3-v1)); 		
        this->tangent = glm::normalize(v2-v1); 		
        this->bitangent = glm::normalize(v3-v1); 		
        this->uvScale = 1.0f/glm::vec2(glm::length(v2-v1), glm::length(v3-v1));        
	};
    
	bool isInside(glm::vec3 p) override {
        
        float a1 = glm::dot(glm::cross(v2-v1,p-v1), normal);
        float a2 = glm::dot(glm::cross(v3-v2,p-v2), normal);
        float a3 = glm::dot(glm::cross(v1-v3,p-v3), normal);        
	
	    return (a1 >= 0 && a2 >= 0 && a3 >= 0);
    }
};
