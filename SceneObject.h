/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The object class
*  This is a generic type for storing objects in the scene.
*  Being an abstract class, this class cannot be instantiated.
*  Sphere, Plane etc, must be defined as subclasses of Object
*      and provide implementations for the virtual functions
*      intersect()  and normal().
-------------------------------------------------------------*/

#pragma once
#include "Material.h"
#include "Ray.h"
#include <glm/glm.hpp>


// forwards
class SceneObject;

/** Structure containing information about a ray / object intersection.
 * It is often more efficent to calculate these values at once than to
 * do them individually.  Especially is the object is a composite object. */
struct RayIntersectionResult
{
public:
    // the location of the ray intersection.
    glm::vec3 location;    

    // surface normal at the point of collision.
    glm::vec3 normal;
    
    // pointer to object we collided with.
    SceneObject* target = NULL;

    // distance (in units) from ray origin to collision point.  Negative for no collision.
    float t = -1;

    /** Returns if ray collided with object or not. */
    bool didCollide() {
        return (t >= 0) && (target != NULL);
    }

    /** Creates a ray intersection result where the ray does not collide with the object. */
    RayIntersectionResult() {};

    /** Creates a ray intersection result with given parameters. */
    RayIntersectionResult(SceneObject* target, float t, glm::vec3 location, glm::vec3 normal = glm::vec3()) {
        this->target = target;
        this->t = t;
        this->location = location;
        this->normal = normal;                
    }
};

class SceneObject
{
public:
    // this objects material.
	Material* material;

    // the anchor point of the object.
    glm::vec3 location;

    // if this object should cast shadows or not.
    bool castsShadows = true;    
    
	SceneObject(glm::vec3 location = glm::vec3(0,0,0)) {
        this->location = location;
        this->material = new Material();
    }
    
    virtual RayIntersectionResult intersect(Ray ray) {
        return RayIntersectionResult();
    }

	virtual ~SceneObject() {}    

    virtual glm::vec2 getUV(glm::vec3 pos) {return glm::vec2(); };

    virtual glm::vec3 getTangent(glm::vec3 p) {return glm::vec3(); };

};