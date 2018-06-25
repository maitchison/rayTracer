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


enum Volume { BV_NONE, BV_SPHERE, BV_AABB };

/** Handles fast bounds check to accelerate intersections. */
// Note: would have been nice do this this with virtual methods but it's faster if we
// can do the binding at compile time (inlining etc).
class BoundingVolume
{

public:

	Volume type = BV_NONE;

	float sphereRadius = 1.0f;
	glm::vec3 boxSize = glm::vec3(1, 1, 1);

	static BoundingVolume Sphere(float radius) {
		BoundingVolume bv = BoundingVolume();
		bv.type = BV_SPHERE;
		bv.sphereRadius = radius;
		return bv;
	}

	static BoundingVolume AABB(glm::vec3 size) {
		BoundingVolume bv = BoundingVolume();
		bv.type = BV_AABB;
		bv.boxSize = size;
		return bv;
	}

	/** Returns if p is inside this volume. */
	bool isInside(const glm::vec3 p) {
		switch (type) {
		case BV_NONE:
			return true;
		case BV_SPHERE:
			return glm::length2(p) <= (sphereRadius * sphereRadius);		
		case BV_AABB:
			return (abs(p.x) < boxSize.x) && (abs(p.y) < boxSize.y) && (abs(p.z) < boxSize.z);
		}
		return false;
	}

	/** radius of sphere that contains volume. */
	inline float getRadius() {
		switch (type) {
		case BV_NONE:
			return 0.0f;
		case BV_SPHERE:
			return sphereRadius;
		case BV_AABB:
			return glm::length(boxSize);
		}
		return 0.0f;
	}

	/** returns if this ray intersects the volume or not. */
	bool rayIntersects(const Ray* ray) {

		float t;

		switch (type) {
		case BV_NONE:
			return true;			
		case BV_SPHERE:
			if (glm::length2(ray->pos) < sphereRadius*sphereRadius) return true; // make sure to intersect if we are inside.
			t = raySphereIntersection(&ray->pos, &ray->dir, sphereRadius);
			return (t > 0 && t < ray->length);
		case BV_AABB:
			if (isInside(ray->pos)) return true;
			t = rayBoxIntersection(boxSize, ray->pos, ray->dir);
			return (t > 0 && t < ray->length);
		}
		return false;
	}

	/** returns distance ray must travel before hitting object, or -1 of ray does not hit object. */
	float rayIntersectionDistance(const Ray* ray) {
		switch (type) {
		case BV_NONE:
			return -1;
		case BV_SPHERE:
			return raySphereIntersection(&ray->pos, &ray->dir, sphereRadius);
		case BV_AABB:
			return rayBoxIntersection(boxSize, ray->pos, ray->dir);
		}
	}

};

class SceneObject
{

public:
	BoundingVolume boundingVolume = BoundingVolume();

protected:
    // our local transforms
    glm::vec3 location = glm::vec3(0,0,0);
    glm::vec3 rotation = glm::vec3(0,0,0); // Euler angles
    glm::vec3 scale = glm::vec3(1,1,1);

    // indicates that scene object applies only a simple translation transformation.
    bool simpleTransform = true;

    void rebuildTransforms() {
        localTransform = glm::mat4x4(1);        

        localTransform = glm::translate(localTransform, location);                
        localTransform = glm::rotate(localTransform, rotation.x, glm::vec3(1,0,0));        
        localTransform = glm::rotate(localTransform, rotation.y, glm::vec3(0,1,0));
        localTransform = glm::rotate(localTransform, rotation.z, glm::vec3(0,0,1));        
        localTransform = glm::scale(localTransform, scale);        
        
        localTransformInv = glm::inverse(localTransform);        

        simpleTransform = (glm::length2(rotation)==0 && (scale.x==scale.y==scale.z==1));
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
    bool intersect(Ray* ray) {
        
        // the idea here is to transform the ray into local space,
        // perform the intersection, then transform the resulting 
        // intersection back into parent space.  This allows for a complex
        // graph based transformation system.
        // Each class must implement the 'intersectObject' method, but can perform all calculations in local
        // space which simplifies things a lot and allows for nested transforms (as we transform the ray not
        // the object)

		glm::vec3 oldDir = ray->dir;
		glm::vec3 oldPos = ray->pos;
        
        if (simpleTransform) {            
            ray->pos -= location;
        } else {
            ray->transform(localTransformInv);
        }

		// first check our volume
		if (!boundingVolume.rayIntersects(ray)) {
			ray->dir = oldDir;
			ray->pos = oldPos;
			return false;
		}

        bool didIntersect = intersectObject(ray);

        if (didIntersect && (ray->collision.uv.x == 0) && ray->collision.target->material->needsUV()) {
            // fetch uv only if required.
			ray->collision.uv = ray->collision.target->getUV(ray->collision.local);
        }

		if (didIntersect) {
			// if we intersected the object, make sure to ignore any objects more distant from this point.
			ray->length = ray->collision.t;

			if (simpleTransform) {
				ray->collision.location += location;				
			}
			else {
				// transform world coords                    
				ray->collision.location = toParent(glm::vec4(ray->collision.location, 1));

				//note: this is not the proper transform.  It should be something to do with the inverse transpose,
				//if the objects scale is set to non uniform this this will be wrong.
				ray->collision.normal = glm::normalize(toParent(glm::vec4(ray->collision.normal, 0)));
				ray->collision.tangent = glm::normalize(toParent(glm::vec4(ray->collision.tangent, 0)));
			}
		}

		// get ray back				
		ray->dir = oldDir;
		ray->pos = oldPos;

		return didIntersect;
  
    }

    /** This should be overridden for each class. */
    virtual bool intersectObject(Ray* ray) {
		return false;
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
            
};