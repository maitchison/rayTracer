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

    // we check each child object and take the closest collision.
    for (int i = 0; i < children.size(); i++) {

        if (ray.shadowTrace && !children[i]->castsShadows) continue;

        RayIntersectionResult result = children[i]->intersect(ray);

        if (result.didCollide() && (result.t < best.t || !best.didCollide())) {
            best = result;
        }        
    }
    return best;
}




