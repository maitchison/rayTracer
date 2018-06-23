/*----------------------------------------------------------
-------------------------------------------------------------*/

#include "Cylinder.h"

bool Cylinder::intersectObject(Ray* ray)
{    
    // we project everything down onto the xz plane and do a circle intersection test.
    // this gives us two points of intersection.
    // we then test the cylinder caps by intersecting two planes and checking them.    
    
	glm::vec3 projectedPos = ray->pos;
	glm::vec3 projectedDir = ray->dir;	
    
    projectedDir.y = 0;
    projectedPos.y = 0;

    if (glm::length2(projectedDir) == 0) {
		return false;
    }

    float radius2 = radius * radius;

    // intersect with circle
    float a = glm::length2(projectedDir);
    float b = 2.0f * glm::dot(projectedDir, projectedPos);
    float c = glm::length2(projectedPos) - radius2;

    float det = b*b - (4 * a * c);
    
	if (det < EPSILON) return false;

    float t1 = (-b - sqrt(det)) / (2 * a);
    float t2 = (-b + sqrt(det)) / (2 * a);
    
    // if points are not on the cylinder ignore them
    float y1 = ray->pos.y + ray->dir.y * t1;
    float y2 = ray->pos.y + ray->dir.y * t2;

    if (y1 < 0 || y1 > height) t1 = -1; 
    if (y2 < 0 || y2 > height) t2 = -1; 

    glm::vec3 normal = glm::vec3();
    float t = INFINITY;
    if (t1 > 0 && t1 < t) t = t1;
    if (t2 > 0 && t2 < t) t = t2;
      
    // next calculated t3 and t4 which are the intersections with the caps
    if (capped) {
        
        float t3 = ray->dir.y == 0 ? -1 : (ray->pos.y) / -ray->dir.y;
        float t4 = ray->dir.y == 0 ? -1 : (ray->pos.y - (height)) / -ray->dir.y;
        
        // make sure cap points are on the cylinder.
        
        if (glm::length2((projectedPos + projectedDir * t3)) > radius2) t3 = -1;
        if (glm::length2((projectedPos + projectedDir * t4)) > radius2) t4 = -1;            

        // now find the closest non negative point of intersection    
        if (t3 > 0 && t3 < t) {
            t = t3;
            normal = glm::vec3(0,-1,0);            
        }
        if (t4 > 0 && t4 < t) {
            t = t4;
            normal = glm::vec3(0,+1,0);            
        }     
    }

    // ray does not intersect
    if (t < 0 || t == INFINITY || t > ray->length) {        
        return false;
    } 

    // find normal
    if (glm::dot(normal, normal) == 0) {
        // get normal from main part of cylinder.
        normal = (ray->pos + ray->dir * t);
        normal.y = 0;
        normal = glm::normalize(normal);
    }
    
    ray->collision.target = this;
	ray->collision.t = t;
	ray->collision.local = ray->pos + ray->dir * t;
	ray->collision.location = ray->collision.local;
	ray->collision.normal = normal;
	ray->collision.tangent = normal;
        
    return true;
}

/**
 * Cylindrical UV mapping.
 */
glm::vec2 Cylinder::getUV(glm::vec3 pos)
{
	// spherical mapping		
	float u = 0.5f + (atan2(pos.z, pos.x) / (PI * 2));
	float v = pos.y / height;
	return glm::vec2(u, v);
}