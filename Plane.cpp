/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include <math.h>

/**
* Returns if a point p is inside the plane or not.  
* Point p is assumed to lie on the plane, and if it doesn't it's projection onto the plane is used.
* For infinite planes all points are considered 'inside' the bounds.
*/
bool Plane::isInside(glm::vec3 p)
{
    if (!bounded) return true;
    
    float a1 = glm::dot(glm::cross(v2-v1,p-v1), normal);
    float a2 = glm::dot(glm::cross(v3-v2,p-v2), normal);
    float a3 = glm::dot(glm::cross(v4-v3,p-v3), normal);
    float a4 = glm::dot(glm::cross(v1-v4,p-v4), normal);	
	
	return (a1 > 0 && a2 > 0 && a3 > 0 && a4 > 0);
}

/**
* Plane's intersection method.  The input is a ray (pos, dir). 
*/
RayIntersectionResult Plane::intersect(Ray ray)
{	
	glm::vec3 vdif = v1 - ray.pos;
	float vdotn = glm::dot(ray.dir, normal);
	if(fabs(vdotn) < 1.e-4) return RayIntersectionResult();
    float t = glm::dot(vdif, normal)/vdotn;
	if(fabs(t) < 0.0001) return RayIntersectionResult();
	glm::vec3 q = ray.pos + ray.dir*t;
	if (isInside(q)) {        
        return RayIntersectionResult(this, t, q, normal);
    } 
    else {
        return RayIntersectionResult();
    }
}