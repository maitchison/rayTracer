/*----------------------------------------------------------
* COSC363  Ray Tracer
*
-------------------------------------------------------------*/

#include "ContainerObject.h"

void ContainerObject::add(SceneObject* object) 
{
    children.push_back(object);
}

RayIntersectionResult ContainerObject::intersectObject(Ray ray)
{

    if (boundingSphereRadius > 0) {

		// this doesn't seem to help much yet, I think it's because we are copying ray so 
		// the best.t gets lost too oftn.
		if (boundingSphereRadius > ray.length) RayIntersectionResult::NoCollision();
		
        float t = raySphereIntersection(ray.pos, ray.dir, glm::vec3(0,0,0), boundingSphereRadius);

        // big hack, use bounds if we are far away.  Should be fine for GI rays.
        if (ray.giRay && t > boundingSphereRadius*10.0f) {
            glm::vec3 local = (ray.pos + ray.dir * t);
            glm::vec3 normal = glm::normalize(local);
            return RayIntersectionResult(this, t, local, normal, normal);
        }
        // test our sphere bounds first (much faster for larger containers)
        if (t<=0) {
            return RayIntersectionResult::NoCollision();
        } 
    }
        
    RayIntersectionResult best = RayIntersectionResult();    

    if (showBounds) {		
        best.t = raySphereIntersection(ray.pos, ray.dir, glm::vec3(0,0,0), boundingSphereRadius);
        best.local = best.location = ray.pos + ray.dir * best.t;
        best.target = this;
        best.normal = glm::normalize(location - best.location);		
        return best;
    }

    // we check each child object and take the closest collision.
    for (int i = 0; i < children.size(); i++) {

        if (ray.shadowTrace && !children[i]->castsShadows) continue;

        RayIntersectionResult result = children[i]->intersect(ray);

        if (result.didCollide() && (result.t < best.t || !best.didCollide())) {
            best = result;
			ray.length = best.t;
        }        
    }

    if (useContainerMaterial && best.didCollide()) {
        // if we set the target to ourselves then getting the uvs won't work later on so we 
        // eval them here.        
        if (material->needsUV()) {            
            best.uv = best.target->getUV(best.local);            
        }
        best.target = this;
        
    }
    return best;
}




