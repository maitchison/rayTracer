/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The ray class
-------------------------------------------------------------*/

#pragma once
#include <glm/glm.hpp>

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
	glm::vec2 uv = glm::vec2(0, 0);

	// radiance sample at location (if calculated)
	Color color = Color(0, 0, 0, 1);

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

class Ray 
{
public:
    glm::vec3 pos;	//The source point of the ray
	glm::vec3 dir;	//The unit direction of the ray

	// information about this rays collision with scene.
	RayIntersectionResult collision = RayIntersectionResult::NoCollision();

	// how far the ray can travel before it should be ignored.
	float length = 99999999.0f;

    // If true ray is tracing a shadow.
    bool shadowTrace = false;
    // This is a global illuminaton ray trace.
    bool giRay = false;
	// This is a sub surface scatter ray trace.
	bool sssRay = false;
	// Test reverse side of material on collision.
	bool reverseNormal = false;
    
    Ray()
	{
		pos = glm::vec3(0, 0, 0);
		dir = glm::vec3(0, 0, -1);    
	}	
	
    Ray(glm::vec3 pos, glm::vec3 dir, bool shadowTrace = false)		
	{
        this->pos = pos;
        this->dir = glm::normalize(dir);        
        this->shadowTrace = shadowTrace;		
	};

    /** Transform ray into another coordinate space as per transformtion matrix. */
    void transform(glm::mat4x4 transform) 
    {
        pos = glm::vec3(transform * glm::vec4(pos,1));
        // could save a normalise here if I made just a rotation matrix? right?
        dir = glm::normalize(glm::vec3(transform * glm::vec4(dir,0)));
    }
};
