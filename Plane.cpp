/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"

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
	
	return (a1 >= -EPSILON && a2 >= -EPSILON && a3 >= -EPSILON && a4 >= -EPSILON);
}

/**
* Plane's intersection method.  The input is a ray (pos, dir). 
*/
bool Plane::intersectObject(Ray* ray)
{	
	glm::vec3 vdif = ray->pos - v1;
	float vdotn = glm::dot(ray->dir, normal);
	if (fabs(vdotn) < EPSILON) return false;
    float t = glm::dot(vdif, normal)/(-vdotn);

	if (t < EPSILON) return false;
	if (t > ray->length) return false;

	glm::vec3 q = ray->pos + ray->dir*t;
	if (isInside(q)) {                
		ray->collision = RayIntersectionResult(this, t, q, normal, tangent);
		return true; 
    } 
    else {
        return false;
    }
}

/**
 * Planar mapping
 */
glm::vec2 Plane::getUV(glm::vec3 pos) {    

    // find a basis
    glm::vec3 b1 = tangent;
    glm::vec3 b2 = bitangent;        
    
    // change co-ords to that basis
    glm::vec3 p = v1 - pos;
    float u = glm::dot(p, b1);
    float v = glm::dot(p, b2);    

    return glm::vec2(u,v) * uvScale;    
}