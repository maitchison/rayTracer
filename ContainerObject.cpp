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
    // test our sphere bounds first (much faster for larger containers)
    if (!this->sphereBoundsTest(ray)) {
        return RayIntersectionResult::NoCollision();
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




