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

	// big hack, use bounds if we are far away.  Should be fine for GI rays.	
	if (ray->giRay && boundingVolume.type != BV_NONE) {
		float distanceToObject2 = glm::length2(ray->pos);
		float bvRadius2 = boundingVolume.getRadius(); 
		bvRadius2 = bvRadius2 * bvRadius2;

		if (distanceToObject2 > bvRadius2*100.0f) {
			float t = boundingVolume.rayIntersectionDistance(ray);
			if (t > 0 && t < ray->length) {
				glm::vec3 local = (ray->pos + ray->dir * t);
				glm::vec3 normal = glm::normalize(local);
				ray->collision = RayIntersectionResult(this, t, local, normal, normal);
				return true;
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




