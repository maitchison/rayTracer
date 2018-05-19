#include "Camera.h"

Camera::Camera(glm::vec3 location) : SceneObject(location)
{
}

Color Camera::trace(Ray ray, int depth)
{
	glm::vec3 lightPosition(10, 40, -3);

	Color ambientLight = Color(0.2f,0.2f,0.2f, 1.0f);   //Ambient color of light

    float shinyness = 4.0;

    RayIntersectionResult intersection = scene->intersect(ray);

    if (!intersection.didCollide()) return backgroundColor;      //If there is no intersection return background colour

    Material* material = intersection.target->material;

    // we check if uv co-ords need calculating as they can be quite slow (transidental functions for example).  
    glm::vec2 uv = glm::vec2(0,0);
    if (material->needsUV()) {        
        uv = intersection.target->getUV(intersection.location);        
    }

    // sample materials properties
    glm::vec4 diffuseColor = material->getDiffuseColor(uv);

    // get normal vector
    
    glm::vec3 normalVector = intersection.normal;

    if (material->normalTexture) {
        // we find the 3 vectors required to transform the normal map from object space to world space.
        glm::vec3 materialNormal = glm::vec3(material->normalTexture->sample(uv.x, uv.y) * 2.0f - 1.0f);

        // soften the normal map a little
        materialNormal = glm::normalize(materialNormal + 2.0f * glm::vec3(0,0,1));

        glm::vec3 normal = normalVector;
        glm::vec3 tangent = intersection.target->getTangent(intersection.location);
        glm::vec3 bitangent = glm::cross(normal, tangent);
        normalVector = glm::vec3(
            materialNormal.x*tangent + materialNormal.y*bitangent + materialNormal.z*normal             
        );
    }

    // diffuse light    
    glm::vec3 lightVector = glm::normalize(lightPosition - intersection.location);
    float diffuseLight = glm::dot(lightVector, normalVector);
    if (diffuseLight < 0) diffuseLight = 0;

    // specular light
    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    glm::vec3 viewVector = glm::normalize(glm::vec3(intersection.location.x, intersection.location.y, -intersection.location.z));
    float specularLight = glm::dot(reflVector, viewVector);
    if (specularLight < 0) {
        specularLight = 0;
    } else {
        specularLight = (float)pow(specularLight, shinyness);
    }

    // shadow
    Ray shadow(intersection.location + lightVector * 0.001f, lightVector);
    RayIntersectionResult shadowIntersection = scene->intersect(shadow);    
    if (shadowIntersection.didCollide() && shadowIntersection.t < lightVector.length()) {
        diffuseLight = specularLight = 0;
    }    

    // apply the lighting model
    // note: transpariency (alpha) only applies to the ambient and diffuse, the specular remains the same (as this is really a reflection)
    float alpha = diffuseColor.a;
    Color colorSum = (ambientLight * diffuseColor + diffuseLight * diffuseColor) * alpha + specularLight;
    
    // reflection    
    if(material->reflectivity > 0 && depth < MAX_STEPS) {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(intersection.location, reflectedDir);
        Color reflectedCol = trace(reflectedRay, depth+1); 
        colorSum = colorSum + (0.8f*reflectedCol);
    }

    // transparency 
    // note: we don't bump up the depth counter here as we can't recurse infinitely with transparency.
    if (alpha < 1) {
        if (material->refractionIndex == 1.0) {        
    
            // start the ray a little further on from where we hit.
            Ray transmittedRay = Ray(intersection.location + 0.001f * ray.dir, ray.dir);
            Color transmittedCol = trace(transmittedRay, depth); 
            colorSum = colorSum + (1.0f-alpha)*transmittedCol;
            
        } else {            

            glm::vec3 refractedDir = glm::refract(ray.dir, intersection.normal, 1.0f/material->refractionIndex);

            Ray refractedRay = Ray(intersection.location + refractedDir * 0.001f , refractedDir);
            
            // the refracted ray will exit the object at this location, don't trace against entire scene, just trace against the 
            // specific object (faster, and less prone to error).
            RayIntersectionResult exitPoint = intersection.target->intersect(refractedRay);

            if (exitPoint.didCollide()) {
                glm::vec3 exitDir = glm::refract(refractedDir, -exitPoint.normal, material->refractionIndex);
                Ray exitRay = Ray(exitPoint.location + exitDir * 0.001f, exitDir);
                Color refractedCol = trace(exitRay, depth+1); 
                colorSum = colorSum + (1.0f-alpha)*refractedCol;
            } else {
                // this case shouldn't happen, but might due to rounding... just ignore (i.e. use black color)
                colorSum = Color(1,0,1,1);
            }
        }
    }
    
	return colorSum;
}

/** Renders given number of pixels before returning control. */
int Camera::render(int pixels, int oversample, float defocus, bool autoReset)
{	
	int totalPixels = SCREEN_WIDTH * SCREEN_HEIGHT;
	float aspectRatio = float(SCREEN_WIDTH / SCREEN_HEIGHT);

	if (pixels == -1) {
		pixels = totalPixels - pixelOn;
	}
	
	int pixelsDone = 0;

	#pragma loop(hint_parallel(4))  
	for (int i = 0; i < pixels; i++) {

		pixelOn++;		

		if (pixelOn >= totalPixels)
		{
			// reset.
			if (autoReset) pixelOn = 0;
			return i;
		}

		int x = 0;
		int y = 0;

		x = pixelOn % SCREEN_WIDTH;
		y = pixelOn / SCREEN_WIDTH;
		
		Color outputCol = Color(0, 0, 0, 1);

		for (int j = 0; j < oversample; j++) {
			float jitterx = (oversample == 1) ? 0.5 : randf();
			float jittery = (oversample == 1) ? 0.5 : randf();

			// find the rays direction
			float rx = (2 * ((x + jitterx) / SCREEN_WIDTH) - 1) * tan(fov / 2 * M_PI / 180) * aspectRatio;
			float ry = (1 - 2 * ((y + jittery) / SCREEN_HEIGHT)) * tan(fov / 2 * M_PI / 180);
			glm::vec3 dir = glm::normalize(glm::vec3(rx, -ry, -1));

            // defocus
            if (defocus) {
                dir.x += randf() * defocus;
                dir.y += randf() * defocus;
                dir = glm::normalize(dir);
            }
			
			Ray ray = Ray(location, dir);
			Color col = trace(ray);
			outputCol = outputCol + (col * (1.0f/oversample));
		}
				
        gfx.putPixel(x, y, outputCol);
        gfx.putPixel(x, y+2, Color(1,1,1,1));
        gfx.putPixel(x, y+1, Color(0,0,0,1));
        gfx.putPixel(x, y+3, Color(0,0,0,1));
    
		pixelsDone++;

	}
	return pixelsDone; 
}