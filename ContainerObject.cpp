/*----------------------------------------------------------
* COSC363  Ray Tracer
*
-------------------------------------------------------------*/

#include "ContainerObject.h"

void ContainerObject::add(SceneObject* object) 
{
    children.push_back(object);
}

bool ContainerObject::intersectObject(Ray* ray)
{

    if (boundingSphereRadius > 0) {

		bool distanceFromSphere2 = glm::length2(ray->pos);
		float sphereRadius2 = boundingSphereRadius * boundingSphereRadius;
		float maxRadius2 = (boundingSphereRadius + ray->length) * (boundingSphereRadius + ray->length);

		// no way to hit the sphere, we are too far away.
		if (distanceFromSphere2 > maxRadius2) return false;

		if (distanceFromSphere2 > boundingSphereRadius*boundingSphereRadius) {
			// we may or may not hit the sphere so check futher

			float t = raySphereIntersection(ray->pos, ray->dir, glm::vec3(0, 0, 0), boundingSphereRadius);

			if (t > ray->length) return false;

			// big hack, use bounds if we are far away.  Should be fine for GI rays.
			if (ray->giRay && t > boundingSphereRadius*10.0f && t < ray->length) {
				glm::vec3 local = (ray->pos + ray->dir * t);
				glm::vec3 normal = glm::normalize(local);
				ray->collision = RayIntersectionResult(this, t, local, normal, normal);
				return true;
			}
			// we did not hit sphere bounds so exit.
			if (t <= 0) {
				return false;
			}

		}

    }
        
    // we check each child object and take the closest collision.
	bool didCollide = false;
    for (int i = 0; i < children.size(); i++) {

        if (ray->shadowTrace && !children[i]->castsShadows) continue;
        didCollide |= children[i]->intersect(ray);        
    }

    if (useContainerMaterial && didCollide) {
        // if we set the target to ourselves then getting the uvs won't work later on so we 
        // eval them here.        
        if (material->needsUV()) {            
            ray->collision.uv = ray->collision.target->getUV(ray->collision.local);
        }
		ray->collision.target = this;        
    }
    return didCollide;
}




