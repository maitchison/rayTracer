/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The ray class
-------------------------------------------------------------*/

#pragma once
#include <glm/glm.hpp>

class Ray 
{
public:
    glm::vec3 pos;	//The source point of the ray
	glm::vec3 dir;	//The unit direction of the ray

	// how far the ray can travel before it should be ignored.
	float length = FP_INFINITE;


    // If true ray is tracing a shadow.
    bool shadowTrace = false;
    // This is a global illuminaton ray trace.
    bool giRay = false;
    
    Ray()
	{
		pos = glm::vec3(0, 0, 0);
		dir = glm::vec3(0, 0, -1);    
	}	
	
    Ray(glm::vec3 pos, glm::vec3 dir, bool shadowTrace = false)		
	{
        this->pos = pos;
        this->dir = glm::normalize(dir);        
        this->shadowTrace = shadowTrace;		
	};

    /** Transform ray into another coordinate space as per transformtion matrix. */
    void transform(glm::mat4x4 transform) 
    {
        pos = glm::vec3(transform * glm::vec4(pos,1));
        // could save a normalise here if I made just a rotation matrix? right?
        dir = glm::normalize(glm::vec3(transform * glm::vec4(dir,0)));
    }
};
