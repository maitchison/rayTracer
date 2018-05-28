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

// various lighting models for the render
enum LightingModel {
    // direct lighting model, based on blinn.
    LM_DIRECT,
    // indirect global illumination.  Much slower.
    LM_GI,
    // displays UV co-ords
    LM_UV,
    // displays depth
    LM_DEPTH,
    // displays normal
    LM_NORMAL,
    // displays world coords
    LM_WORLD,
    // displays local coords
    LM_LOCAL
};

class Camera : public SceneObject
{
protected:
    
    float fov = 90.0f;

	int pixelOn = 0;

    // a bit of a hack, but this is the intersection result from the last call to 'trace'.
    // idealy I'd return this and the color from the trace function in some kind of struct.
    RayIntersectionResult lastTraceIntersection;
    
public:

    // ----------------------------
    // Render parameters:

    // number of global illumination samples to test per lighting calculation. 
    int GI_SAMPLES = 32;    
	
    // maximum recursive depth (e.g. reflections).
    int MAX_RECUSION_DEPTH = 9;

    // Number of rays to trace per pixel.  0 disables supersampling.
	int superSample=0;

    // Randomly defocus rays by this number of radians.  Requires high oversampling for best results.
    float defocusBlur = 0.0f;

    // renders only 1/4th of the pixels.
    bool lqMode = false;

    // The lighting model to use when rendering.
    LightingModel lightingModel = LM_DIRECT;

    // ----------------------------
    
    // the scene to cast rays through.
	Scene* scene;

    Color backgroundColor = Color(0.1f,0.2f,0.4f,1.0f);

	Camera(glm::vec3 location = glm::vec3(0,0,0));
	

    /**
     * Traces ray through camera's scene and calculates lighting at intersection point.
     * @ray The ray to test
     * @depth Recusion depth
     * @giSamples Number of GI samples to use, 0 to disable.
     * @returns color at the interesection point of the ray and the scene.
     **/
	Color trace(Ray ray, int depth = 0, int giSamples = 0);
	
	/** Render this number of pixels.  Rendering can be done bit by bit.  
	 @param pixels: maximum number of pixels to render.  -1 renders entire image.
	 @param autoReset: causes renderer to render next frame once this frame finishes rendering.
	*/
	int render(int pixels, bool autoReset=false);	

    /** Reset the camerea rendering. */
    void reset()
    {
        pixelOn = 0;
    }
    
    /** moves camera.
     * forward: amount to move forwards / backwards
     * strafe: amount to strafe left / right. */
    void move(float forward, float strafe = 0.0f, float up = 0.0f) {        
        location += glm::vec3(toParent(glm::vec4(0,0,-1,0))) * forward;
        location += glm::vec3(toParent(glm::vec4(1,0,0,0))) * strafe;        
        location += glm::vec3(toParent(glm::vec4(0,1,0,0))) * up; 
        this->rebuildTransforms();
    }

    /** rotates camera */
    void rotate(float dy, float dx=0) {        
        rotation.x += dx;
        rotation.y += dy;
        this->rebuildTransforms();
    }

protected:

    /** Calculates lighting of given light at this intersection point. */
    void calculateLighting(RayIntersectionResult intersection, Light* light, Color& ambientLightSum, Color& diffuseLightSum, Color& specularLightSum);
	
};