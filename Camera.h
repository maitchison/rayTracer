/**
 * Camera class.
 **/

#pragma once

#include <glm/glm.hpp>
#include "SceneObject.h"
#include "Utils.h"
#include "Ray.h"
#include "GFX.h"
#include "ContainerObject.h"
#include "Light.h"
#include "Scene.h"

class Camera : SceneObject
{
protected:
    
    float fov = 90.0f;

	int pixelOn = 0;

    // maximum number of recursive steps
    const int MAX_STEPS = 5;

    // Euclidean rotation angles.
	glm::vec3 rotation = glm::vec3(0, 0, 0);    

public:
	
    // the scene to cast rays through.
	Scene* scene;

    Color backgroundColor = Color(0.1f,0.2f,0.4f,1.0f);

	Camera(glm::vec3 location = glm::vec3(0,0,0));
	
	Color trace(Ray ray, int depth = 0);
	
	/** Render this number of pixels.  Rendering can be done bit by bit.  
	 @param pixels: maximum number of pixels to render.  -1 renders entire image.
	 @param oversample: number of rays to trace per pixel.  1 for standard render.
	 @param defocus: randomly defocus rays by this number of radians.  Requires high oversampling for best results.
	 @param autoReset: causes renderer to render next frame once this frame finishes rendering.
	*/
	int render(int pixels, int oversample=1, float defocus = 0.0f, bool autoReset=false);	

    /** Reset the camerea rendering. */
    void reset()
    {
        pixelOn = 0;
    }

    /** converts from world co-ordanate space to local space. */
    glm::vec4 toLocal(glm::vec4 p) {
        glm::mat4x4 rotationMatrix = glm::mat4x4();
        rotationMatrix = glm::rotate(rotationMatrix, rotation.x, glm::vec3(1,0,0));
        rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0,1,0));
        rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0,0,1));
        return p * rotationMatrix;
    }
    
    /** moves camera.
     * forward: amount to move forwards / backwards
     * strafe: amount to strafe left / right. */
    void move(float forward, float strafe = 0.0f, float up = 0.0f) {        
        location += glm::vec3(toLocal(glm::vec4(0,0,-1,0))) * forward;
        location += glm::vec3(toLocal(glm::vec4(1,0,0,0))) * strafe;        
        location += glm::vec3(toLocal(glm::vec4(0,1,0,0))) * up; 
    }

    /** rotates camera */
    void rotate(float dy, float dx=0) {        
        rotation.x += dx;
        rotation.y += dy;
    }

protected:

    /** Calculates lighting of given light at this intersection point. */
    void calculateLighting(RayIntersectionResult intersection, const Light* light, Color& ambientLightSum, Color& diffuseLightSum, Color& specularLightSum);
	
};