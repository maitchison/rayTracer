/*----------------------------------------------------------
-------------------------------------------------------------*/

#include "Cylinder.h"

RayIntersectionResult Cylinder::intersectObject(Ray ray)
{    
    // we project everything down onto the xz plane and do a circle intersection test.
    // this gives us two points of intersection.
    // we then test the cylinder caps by intersecting two planes and checking them.    
    
    Ray projectedRay(ray);
    
    projectedRay.dir.y = 0;
    projectedRay.pos.y = 0;

    if (glm::length2(projectedRay.dir) == 0) {
        return RayIntersectionResult::NoCollision();
    }

    float radius2 = radius * radius;

    // intersect with circle
    float a = glm::length2(projectedRay.dir);
    float b = 2.0 * glm::dot(projectedRay.dir, projectedRay.pos);
    float c = glm::length2(projectedRay.pos) - radius2;

    float det = b*b - (4 * a * c);
    
	if (det < EPSILON) return RayIntersectionResult::NoCollision();

    float t1 = (-b - sqrt(det)) / (2 * a);
    float t2 = (-b + sqrt(det)) / (2 * a);
    
    // if points are not on the cylinder ignore them
    float y1 = ray.pos.y + ray.dir.y * t1;
    float y2 = ray.pos.y + ray.dir.y * t2;

    if (y1 < 0 || y1 > height) t1 = -1; 
    if (y2 < 0 || y2 > height) t2 = -1; 

    glm::vec3 normal = glm::vec3();
    float t = INFINITY;
    if (t1 > 0 && t1 < t) t = t1;
    if (t2 > 0 && t2 < t) t = t2;
      
    // next calculated t3 and t4 which are the intersections with the caps
    if (capped) {
        
        float t3 = ray.dir.y == 0 ? -1 : (ray.pos.y) / -ray.dir.y;
        float t4 = ray.dir.y == 0 ? -1 : (ray.pos.y - (height)) / -ray.dir.y;        
        
        // make sure cap points are on the cylinder.
        
        if (glm::length2((projectedRay.pos + projectedRay.dir * t3)) > radius2) t3 = -1;
        if (glm::length2((projectedRay.pos + projectedRay.dir * t4)) > radius2) t4 = -1;            

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
    if (t < 0 || t == INFINITY) {        
        return RayIntersectionResult::NoCollision();
    } 

    // find normal
    if (glm::dot(normal, normal) == 0) {
        // get normal from main part of cylinder.
        normal = (ray.pos + ray.dir * t);
        normal.y = 0;
        normal = glm::normalize(normal);
    }
    
    RayIntersectionResult result = RayIntersectionResult();
    result.target = this;
    result.t = t;
    result.local = ray.pos + ray.dir * t;
    result.location = result.local;
    result.normal = normal;    
    result.tangent = normal;    
        
    return result;
}

/**
 * Cylindrical UV mapping.
 */
glm::vec2 Cylinder::getUV(glm::vec3 pos)
{
	// spherical mapping		
	float u = 0.5f + (atan2(pos.z, pos.x) / (M_PI * 2));
	float v = pos.y / height;
	return glm::vec2(u, v);
}