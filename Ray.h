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

    // If true ray is tracing a shadow.
    bool shadowTrace = false;
    
    Ray()
	{
		pos = glm::vec3(0, 0, 0);
		dir = glm::vec3(0, 0, -1);    
	}	
	
    Ray(glm::vec3 pos, glm::vec3 dir)		
	{
        this->pos = pos;
        this->dir = glm::normalize(dir);        
		
	};
};
