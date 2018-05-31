/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Sphere.h"

/**
* Sphere's intersection method.  The input is a ray (pos, dir). 
*/
RayIntersectionResult Sphere::intersectObject(Ray ray)
{    
    glm::vec3 vdif = ray.pos;
    float b = glm::dot(ray.dir, vdif);
    float len2 = glm::dot(vdif, vdif);
    float c = len2 - radius*radius;
    float delta = b*b - c;
   
	if (delta < EPSILON) return RayIntersectionResult();

    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);
    
    float t = -1;
    
    if ((t1 >= 0) && ((t1 <= t2) || (t2 < 0))) t = t1;
    if ((t2 >= 0) && ((t2 <= t1) || (t1 < 0))) t = t2;    

    // ray does not intersect
    if (t < 0) {        
        return RayIntersectionResult();
    } 

    RayIntersectionResult result = RayIntersectionResult();
    result.target = this;
    result.t = t;
    result.local = result.location = ray.pos + ray.dir * t;
    result.normal = glm::normalize(result.local);    

    // might be a faster way of doing this?
    float phi = atan2(result.local.z,result.local.x);    
    result.tangent = glm::vec3(-sin(phi), 0, cos(phi));    
    
    return result;
}

/**
 * Spherical UV mapping.
 */
glm::vec2 Sphere::getUV(glm::vec3 pos)
{
	// spherical mapping	
	pos = glm::normalize(pos);
	float u = 0.5f + (atan2(pos.z, pos.x) / (PI * 2));
	float v = 0.5f - (asin(pos.y) / PI);

	return glm::vec2(u, v);
}