/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  A container object is a collection of scene objects that can be 
*  treated as a single object.  For example a cube made up of individual
*  constricted planes.
-------------------------------------------------------------*/

#pragma once

#include "SceneObject.h"

#include <glm/glm.hpp>
#include <vector>

using namespace std;

/** A copy of another object, but with an additional transform layer and a different material. */
class ReferenceObject : public SceneObject
{
protected:
    SceneObject* reference;
public:
    ReferenceObject(glm::vec3 location, SceneObject* reference) : SceneObject(location)
    {
        this->reference = reference;                
        this->boundingVolume = reference->boundingVolume;
    }

    bool intersectObject(Ray* ray) {
        bool didCollide = this->reference->intersect(ray);
        if (didCollide) ray->collision.target = this;
        return didCollide;
    }
};

class ContainerObject : public SceneObject
{
protected:
    vector<SceneObject*> children = vector<SceneObject*>();

public:	
	ContainerObject(glm::vec3 location = glm::vec3()) : SceneObject(location)
    {

    }

    bool showBounds = false;
    bool useContainerMaterial = false;	

    /** Adds object to container. */
    virtual void add(SceneObject* object);

    /** Intersects ray with object. */
	bool intersectObject(Ray* ray) override; 

    /** Sets material for all child objects. */
    void setChildrenMaterial(Material* material)
    {
        this->material = material;
        for (int i = 0; i < (int)children.size(); i++) {
            children[i]->material = material;
        }
    }

    /** Calculate radius based on objects within the group */
    void autoRadius() 
    {
        float newRadius = 0;
        float r;
        for (int i = 0; i < (int)children.size(); i++) {
            if (children[i]->boundingVolume.type = BV_NONE) {
                printf("Warning, container has child with no bounding radius, auto bounding sphere may be incorrect.\n");
                r = 1.0f;
            } else {
                r = glm::length(children[i]->getLocation()) + children[i]->boundingVolume.getRadius();
            }            
            newRadius = maxf(r, newRadius);            
        }
        boundingVolume = BoundingVolume::Sphere(newRadius);
    }

    /** Clusters objects within container into proximal groups and assigns them to new sub-objects.  This can speed
     *  up raytracing through larger object containers. 
     *  Note: this is an n^2 algorithms so will be slow on large (1000+) containers.
     * */
    void cluster(bool recurse=true, float CLUSTER_RADIUS = 3.0f)
    {

        // maximum number of objects per cluster.
        int MAX_OBJECTS = 8;

        // the basic idea here is to measure the all pairs distances between objects then group near objects into
        // the same cluster.
        int initialChildrenCount = children.size();

        if (children.size() <= 4) {
            // no need to cluster.
            return;
        }

        // we clear our children, then add them back in clustered form.
        vector<SceneObject*> childrenCopy = vector<SceneObject*>(children);
        children.clear();

        // we take the first remaining object, check it's neighbours, and either cluster it or add it directly.
        // then we move onto the next remaining object until there are none left.
        do {
            SceneObject* target = childrenCopy[0];

            vector<SceneObject*> neighbours = vector<SceneObject*>();                        
        
            // going backwards allows us to remove as we go.
            for (int i = childrenCopy.size()-1; i >= 0; i--) {
                
                float distance = glm::length(target->getLocation() - childrenCopy[i]->getLocation());
                if (childrenCopy[i]==target || distance < (target->boundingVolume.getRadius() * CLUSTER_RADIUS)) {
                    neighbours.push_back(childrenCopy[i]);
                    childrenCopy.erase(childrenCopy.begin()+i);
                }                
            }

            if (neighbours.size() >= 2) {
                // cluster these objects into a group.
                ContainerObject* group = new ContainerObject();
                group->setLocation(target->getLocation());
                for (int i = 0; i < (int)neighbours.size(); i++) {
                    glm::vec3 offsetLocation = neighbours[i]->getLocation() - group->getLocation();
                    neighbours[i]->setLocation(offsetLocation);
                    group->add(neighbours[i]);                    
                }
                group->autoRadius();                
                add(group);

                // if this group has too many objects recluster it with a reduced radius.
                if (recurse && (int)group->children.size() > MAX_OBJECTS) {
                    group->cluster(true, CLUSTER_RADIUS / 2.0f);
                }

            } else {
                // no neighbours, just add back in
                add(target);
            }
        } while (childrenCopy.size() > 0);

        bool didReduce = (int)children.size() < initialChildrenCount;

        printf("Clustered object with %d children, reduced to %d\n", initialChildrenCount, (int)children.size());

        if (recurse && didReduce) {
            this->cluster(true);
        }

    }
    
};
