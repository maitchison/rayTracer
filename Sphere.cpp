/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Sphere.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray (pos, dir). 
*/
RayIntersectionResult Sphere::intersect(Ray ray)
{
    glm::vec3 vdif = ray.pos - center;
    float b = glm::dot(ray.dir, vdif);
    float len = glm::length(vdif);
    float c = len*len - radius*radius;
    float delta = b*b - c;
   
	if(fabs(delta) < 0.001) return RayIntersectionResult(); 
    if(delta < 0.0) return RayIntersectionResult();

    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);
    
    RayIntersectionResult result = RayIntersectionResult();
    result.t = (t1 < t2) ? t1: t2;

    // ray does not intersect
    if (result.t < 0) {
        result.t = -1;
        return result;
    } 

    result.location = ray.pos + ray.dir * result.t;
    result.normal = normal(result.location);
    result.target = this;
    
    return result;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Sphere::normal(glm::vec3 p)
{
    glm::vec3 n = p - center;
    n = glm::normalize(n);
    return n;
}
