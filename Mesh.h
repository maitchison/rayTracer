/**
 * Mesh object 
 */

#pragma once

#include <glm/glm.hpp>
#include <algorithm>

#include "SceneObject.h"
#include "ContainerObject.h"
#include "Plane.h"
#include "Sphere.h"
#include "Utils.h"

class Mesh : public ContainerObject
{

protected:
        
    // Sets this objects mesh.  Normals will be calculated using right hand rule. 
    // if subdivision is set the mesh will be recursively subdivided into sub objects (faster to render)
    void setMesh(std::vector<glm::vec3>* vertices, bool subDivide=true) {

        // some debuging helpers.
        const bool SHOW_VERTICES = false;
        const bool SHOW_ORIGIN = false;

        // maximum number of triangles per subdivision.
        const int SUB_DIVISION_THRESHOLD = 4;

        int triangles = vertices->size() / 3;

        if (SHOW_ORIGIN) {
            Sphere* center = new Sphere(glm::vec3(0,0,0), 0.05f);
            center->material->emisiveColor = Color(0,1,0,1);
            add(center);
        }

        if (triangles == 0) {
            return;
        }

        if (subDivide && (triangles > SUB_DIVISION_THRESHOLD)) {
            // the idea here is divide on space, so find the longest dimension, cut it in half and create two subdivisions.
            // a better idea may have been to use the median vertex, but I'm no so interested in even bins so much as dividing
            // the space up.  I think, however, that I loose all guarintees about running time, but in practice it should still
            // be nlogn except on pathalogical cases.

            // note: we iterate over the dimensions here, rather than specific x,y,z seperately.
            float _min[3] = {+INFINITY, +INFINITY, +INFINITY};
            float _max[3] = {-INFINITY, -INFINITY, -INFINITY};
            float delta[3];

            for (int j = 0; j < 3; j++) {
                for (int i = 0; i < vertices->size(); i++) {
                    if ((*vertices)[i][j] < _min[j]) _min[j] = (*vertices)[i][j];
                    if ((*vertices)[i][j] > _max[j]) _max[j] = (*vertices)[i][j];                
                }                
                delta[j] = _max[j]-_min[j];
            }

            // find which axis to divide on
            int dim = 0;
            if (delta[0] > delta[1] && delta[0] > delta[2]) {
                dim = 0;                
            } else if (delta[1] > delta[2]) {
                dim = 1;
            } else {
                dim = 2;
            }
            // when we subdivide, we use only the first vertex of each triangle to decide which half it goes into
            float threshold = (_min[dim] + _max[dim]) / 2;
    
            std::vector<glm::vec3>* leftHalf = new std::vector<glm::vec3>();
            std::vector<glm::vec3>* rightHalf = new std::vector<glm::vec3>();

            glm::vec3 leftCenter = glm::vec3(0,0,0);
            glm::vec3 rightCenter = glm::vec3(0,0,0);

            for (int f = 0; f < triangles; f++) {
                if ((*vertices)[f*3][dim] < threshold) {
                    for (int j = 0; j < 3; j++) {
                        leftHalf->push_back((*vertices)[f*3+j]);
                        leftCenter += (*vertices)[f*3+j];
                    }                                    
                } else {
                    for (int j = 0; j < 3; j++) {
                        rightHalf->push_back((*vertices)[f*3+j]);
                        rightCenter += (*vertices)[f*3+j];
                    }                                       
                }
            }

            if (leftHalf->size() == 0 || rightHalf->size() == 0) {
                // we did not actually divide the mesh.  This is a problem, and it can happen in some cases.
                // in this case we just give up and ignore subdivision.
                //printf(">>>>>  Warning, failed to split mesh with %d triangles!\n", triangles);
                setMesh(vertices, false);
                return;
            }

            //printf("Split into [%d / %d] along dimension %d\n", (int)leftHalf->size(), (int)rightHalf->size(), dim);

            leftCenter /= leftHalf->size();
            rightCenter /= rightHalf->size();
            
            // center the meshes (this is required for optimal sphere bounds)
            for (int i = 0; i < leftHalf->size(); i++) {
                (*leftHalf)[i] -= leftCenter;
            }
            for (int i = 0; i < rightHalf->size(); i++) {
                (*rightHalf)[i] -= rightCenter;
            }
            
            // create the two halves and center meshes
            Mesh* left = new Mesh(leftCenter, leftHalf);
            Mesh* right = new Mesh(rightCenter, rightHalf);
            
            add(left);
            add(right);

            float boundingSphereRadius = maxf(
                glm::length(leftCenter) + left->boundingVolume.getRadius(), 
                glm::length(rightCenter) + right->boundingVolume.getRadius()
            );

			boundingVolume = BoundingVolume::Sphere(boundingSphereRadius);
                
            return;

        }    

        // the base case, just add the triangles to the object and calculate a bounding radius.        

        int faces = vertices->size() / 3;
        float boundingSphereRadius = -1;
        
        for (int f = 0; f < faces; f++) {
            // this will show vertices
            if (SHOW_VERTICES) {
                Sphere* vertexSphere = new Sphere((*vertices)[f*3+0], 0.3f);
                add(vertexSphere);
            }

            Plane* triangle = new Triangle(
                (*vertices)[f*3+0],
                (*vertices)[f*3+1],
                (*vertices)[f*3+2]            
            );

            if (!showDivision) {
                add(triangle);  
            }                                              

            for (int i = 0; i < 3; i++) {

                float r = glm::length((*vertices)[f*3+i]);
                if (r > boundingSphereRadius) boundingSphereRadius = r;
            }                            
        }

        if (showDivision) {
            Sphere* sphere = new Sphere(glm::vec3(0,0,0), boundingSphereRadius);
            sphere->material->diffuseColor.a = 0.25;
            add(sphere);
        }

		boundingVolume = BoundingVolume::Sphere(boundingSphereRadius);

        //printf("Size of mesh: %f\n", boundingSphereRadius);

    }    

public:

    bool showDivision = false;

    /** Creates mesh from vertices.  Every triad of vertices is interpreted as a triangle. */
    Mesh(glm::vec3 location, std::vector<glm::vec3>* vertices) : ContainerObject(location) {
        useContainerMaterial = true;
        setMesh(vertices);                
    }    

};