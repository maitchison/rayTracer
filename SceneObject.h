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
#include "Utils.h"
#include <glm/glm.hpp>

// forwards
class SceneObject;

/** Structure containing information about a ray / object intersection.
 * It is often more efficent to calculate these values at once than to
 * do them individually.  Especially is the object is a composite object. */
struct RayIntersectionResult
{
public:
    // the location of the ray intersection in parent coordinates (if collision occured).
    glm::vec3 location;    

    // the location of the ray intersection in local coordinates (if collision occured).
    glm::vec3 local;

    // surface normal at the point of collision.
    glm::vec3 normal;

    // surface tangent at the point of collision.
    glm::vec3 tangent;
    
    // uv co-ords of target at intersection point.
    glm::vec2 uv;
    
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
    RayIntersectionResult(SceneObject* target, float t, glm::vec3 local, glm::vec3 normal = glm::vec3(), glm::vec3 tangent = glm::vec3()) {
        this->target = target;
        this->t = t;
        this->local = local;
        this->location = local;
        this->normal = normal;                
        this->tangent = tangent;
    }

    static RayIntersectionResult NoCollision() {
        return RayIntersectionResult();
    };
};


class SceneObject
{

protected:
    // our local transforms
    glm::vec3 location = glm::vec3(0,0,0);
    glm::vec3 rotation = glm::vec3(0,0,0); // Euler angles
    glm::vec3 scale = glm::vec3(1,1,1);

    // indicates that scene object applies only a simple translation transformation.
    bool simpleTransform = true;

    // radius of objects bounding sphere in local space (i.e. unscaled). 
    // A negative value disables the sphere bounding optimization.    
    float boundingSphereRadius = -1;

    void rebuildTransforms() {
        localTransform = glm::mat4x4(1);        

        localTransform = glm::translate(localTransform, location);                
        localTransform = glm::rotate(localTransform, rotation.x, glm::vec3(1,0,0));        
        localTransform = glm::rotate(localTransform, rotation.y, glm::vec3(0,1,0));
        localTransform = glm::rotate(localTransform, rotation.z, glm::vec3(0,0,1));        
        localTransform = glm::scale(localTransform, scale);        
        
        localTransformInv = glm::inverse(localTransform);        

        simpleTransform = (glm::length2(rotation)==0.0f && (scale.x==scale.y==scale.z==1.0f));
    }

    // local transformation matrix.
    glm::mat4x4 localTransform = glm::mat4x4(1);

    // inverse local transformation matrix. 
    glm::mat4x4 localTransformInv = glm::mat4x4(1);

public:

    inline void setLocation(glm::vec3 location) {
        this->location = location;
        rebuildTransforms();
    }

    inline void setRotation(glm::vec3 rotation) {
        this->rotation = rotation;
        rebuildTransforms();
    }

    inline float getRadius() {
        return this->boundingSphereRadius;        
    }

    inline void setRadius(float radius) {
        boundingSphereRadius = radius;
    }

    inline void setScale(glm::vec3 scale) {
        if (scale.x == 0 || scale.y == 0 || scale.z == 0) {
            printf("WARNING: Invalid transform.  Scale = 0.");
            return;
        }
        this->scale = scale;
        rebuildTransforms();
    }

    glm::vec3 getLocation() { return location; };
    glm::vec3 getRotation() { return rotation; };
    glm::vec3 getScale() { return scale; };
    float getBoundingSphereRadius() { return boundingSphereRadius; };
    glm::mat4x4 getLocalTransform() { return localTransform; }
    glm::mat4x4 getLocalTransformInv() { return localTransformInv; }

    // this objects material.
	Material* material;
    
    // if this object should cast shadows or not.
    bool castsShadows = true;        
    
	SceneObject(glm::vec3 location = glm::vec3()) {
        this->setLocation(location);
        this->material = new Material();
    }
        
    /** Transforms ray into local space then intersects with object. */
    RayIntersectionResult intersect(Ray ray) {
        
        // the idea here is to transform the ray into local space,
        // perform the intersection, then transform the resulting 
        // intersection back into parent space.  This allows for a complex
        // graph based transformation system.
        // Each class must implement the 'intersectObject' method, but can perform all calculations in local
        // space which simplifies things a lot and allows for nested transforms (as we transform the ray not
        // the object)
        
        static int RAYS_SIMPLE = 0;
        static int RAYS_COMPLEX = 0;

        if (simpleTransform) {            
            ray.pos -= location;
        } else {
            ray.transform(localTransformInv);
        }

        RayIntersectionResult result = intersectObject(ray);

        if (result.target && (result.uv.x == 0) && result.target->material->needsUV()) {
            // fetch uv only if required.
            result.uv = result.target->getUV(result.local);
        }


        if (simpleTransform) {
            result.location += location;
        } else {
            // transform world coords                    
            result.location = toParent(glm::vec4(result.location,1));
            
            //note: this is not the proper transform.  It should be something to do with the inverse transpose,
            //if the objects scale is set to non uniform this this will be wrong.
            result.normal = glm::normalize(toParent(glm::vec4(result.normal,0)));
            result.tangent = glm::normalize(toParent(glm::vec4(result.tangent,0)));            
        }
        
        return result;
    }

    /** This should be overridden for each class. */
    virtual RayIntersectionResult intersectObject(Ray ray) {
        return RayIntersectionResult::NoCollision();
    }    

	virtual ~SceneObject() {}    

    /** returns uv coords of pos (in local space) */
    virtual glm::vec2 getUV(glm::vec3 pos) { return glm::vec2(); };
    
    /** converts from parent coordanate space to local space. */
    glm::vec3 toLocal(glm::vec4 p) {
        return glm::vec3(localTransformInv * p);
    }

    /** converts from parent coordanate space to local space. */
    glm::vec3 toParent(glm::vec4 p) {
        return glm::vec3(localTransform * p);
    }
    
    /** Tests if ray intersects this objects sphere bounding box.  Objects without bounding spheres will always pass this test. 
     * Ray should be in local space.  */
    bool sphereBoundsTest(Ray ray) {
        return boundingSphereRadius < 0 ? true : raySphereIntersection(ray.pos, ray.dir, glm::vec3(0,0,0), boundingSphereRadius) > 0;
    }

};