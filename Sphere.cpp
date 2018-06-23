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
bool Sphere::intersectObject(Ray* ray)
{    
    glm::vec3 vdif = ray->pos;
    float b = glm::dot(ray->dir, vdif);
    float len2 = glm::dot(vdif, vdif);
    float c = len2 - radius*radius;
    float delta = b*b - c;
   
	if (delta < EPSILON) return false;

    float t1 = -b - sqrt(delta);
    float t2 = -b + sqrt(delta);
    
    float t = -1;
    
    if ((t1 >= 0) && ((t1 <= t2) || (t2 < 0))) t = t1;
    if ((t2 >= 0) && ((t2 <= t1) || (t1 < 0))) t = t2;    

    // ray does not intersect
    if (t < 0 || t > ray->length) {        
		return false;
    } 
	    
    ray->collision.target = this;
	ray->collision.t = t;
	ray->collision.local = ray->collision.location = ray->pos + ray->dir * t;
	ray->collision.normal = glm::normalize(ray->collision.local);

    // might be a faster way of doing this?
    float phi = atan2(ray->collision.local.z, ray->collision.local.x);
	ray->collision.tangent = glm::vec3(-sin(phi), 0, cos(phi));
    
    return true;
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