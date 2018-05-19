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

struct Camera : SceneObject
{
    // maximum number of recursive steps
    const int MAX_STEPS = 5;

	glm::vec3 rotation = glm::vec3(0, 0, 0);

	float fov = 90.0f;

	int pixelOn = 0;

    // the scene to cast rays through.
	ContainerObject* scene;

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
	
};