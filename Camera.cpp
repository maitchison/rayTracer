#include "Camera.h"
#include "Scene.h"

// a small offset is applied to reflected rays / shadow rays so they don't self interesect.
const float OFFSET_BIAS = 0.001f;

Camera::Camera(glm::vec3 location) : SceneObject(location)
{
}


void Camera::calculateLighting(RayIntersectionResult intersection, ContainerObject* scene, Light* light, Color& ambientLightSum, Color& diffuseLightSum, Color& specularLightSum)
{
    Material* material = intersection.target->material;

    glm::vec3 lightPos = light->sampleLocation();

    // diffuse light    
    glm::vec3 lightVector = glm::normalize(lightPos - intersection.location);    
    float diffusePower = glm::dot(lightVector, intersection.normal);
    if (diffusePower < 0) diffusePower = 0;
    
    // specular light
    glm::vec3 reflVector = glm::reflect(-lightVector, intersection.normal);
    glm::vec3 viewVector = glm::normalize(this->location - intersection.location);

    float specularPower = glm::dot(reflVector, viewVector);
    if (specularPower < 0) {
        specularPower = 0;
    } else {
        specularPower = (float)pow(specularPower, material->shininess);
    }

    Color diffuseLight = light->color * diffusePower;
    Color specularLight = light->color * specularPower;
    
    // this is an optimization.  Points on the far side of a sphere, for example, will have no lighting
    // so there is no need to do shadow calculations.  
    bool needsShadow = light->shadow && (diffusePower > EPSILON || specularPower > EPSILON);
            
    // shadow
    // note: i'm 90% sure I should be doing the transpariency checks in the other order, i.e. absorb from objects closest 
    // to the light first.  Hovever this will often not be noticiable (I think... ?)
    if (needsShadow) 
    {
        float lightDistance;
        glm::vec3 shadowTestPoint = intersection.location;

        // handle transparient shadows by letting ray continue when meeting a transparient object
        for (int i = 0; i < 9; i++) {            
            // offsetting the shadow trace a little stops self shadowing artifacts
            Ray shadow(shadowTestPoint + lightVector * OFFSET_BIAS, lightVector);
            shadow.shadowTrace=true; // this will ignore objects that do not cast shadows.

            RayIntersectionResult shadowIntersection = scene->intersect(shadow);    
            lightDistance = glm::length(lightPos - shadowTestPoint);

            if (shadowIntersection.didCollide() && shadowIntersection.t < lightDistance) {
                // we sample the uv, so that textured transpariency will work :)        
                Color occluderColor = shadowIntersection.target->material->getDiffuseColor(intersection.uv);
                float transmission = 1.0f - occluderColor.a;
                diffuseLight *= (transmission * occluderColor);
                specularLight *= (transmission * occluderColor);
                // no need to continue if we hit a solid object.
                if (transmission < EPSILON) break;
                shadowTestPoint = shadowIntersection.location;                
            } else {
                // didn't hit anything so stop.
                break;
            }


        }
    }

    // accumulate light.
    ambientLightSum += light->ambientLight * light->color;
    diffuseLightSum += diffuseLight * light->color;
    specularLightSum += specularLight * light->color;
}

Color Camera::trace(Ray ray, Scene* scene, int depth, int giSamples)
{        
    if (depth > MAX_RECUSION_DEPTH) {
        lastTraceIntersection = RayIntersectionResult::NoCollision();
        return Color(0,0,0,1);
    }

    RayIntersectionResult intersection = scene->intersect(ray);
    
    lastTraceIntersection = intersection;

    // sepcial lighting models.
    switch (lightingModel) {
        case LM_UV: 
            // uv co-ords are not always generated, so generate them here.
            if (intersection.didCollide()) {
                intersection.uv = intersection.target->getUV(intersection.local);
            }
            return Color(intersection.uv.x, intersection.uv.y, 0.4f, 1);        
        case LM_DEPTH: 
            return Color(intersection.t/100,intersection.t/100,intersection.t/100,1);
        case LM_WORLD: 
            return Color(intersection.location/30.0f,1.0);
        case LM_LOCAL: 
            return Color(intersection.local/5.0f,1.0);
    }    

    if (!intersection.didCollide()) return backgroundColor;      //If there is no intersection return background colour

    Material* material = intersection.target->material;
    
    // modify normal vector based on normal map (if required)
    if (material->normalTexture) {
        // we find the 3 vectors required to transform the normal map from object space to world space.
        glm::vec3 materialNormal = glm::vec3(material->normalTexture->sampleNormalMap(intersection.uv) * 2.0f - 1.0f);

        // soften the normal map a little
        materialNormal = glm::normalize(materialNormal + 1.0f * glm::vec3(0,0,1));

        glm::vec3 normal = intersection.normal;
        glm::vec3 tangent = intersection.tangent;
        glm::vec3 bitangent = glm::cross(normal, tangent);

        glm::vec3 normalVector = glm::vec3(
            materialNormal.x*tangent + materialNormal.y*bitangent + materialNormal.z*normal             
        );

        // update the intersection normal vector
        intersection.normal = normalVector;
    }

    if (lightingModel == LM_NORMAL) {
        return Color(intersection.normal,1.0f);
    }

    // sum up the lighting from all lights.
    Color ambientLight = Color(0,0,0,1);
    Color diffuseLight = Color(0,0,0,1);
    Color specularLight = Color(0,0,0,1);
    // we only need to look a the lights in direct lighting mode (ignore them in GI mode.)
    if (lightingModel == LM_DIRECT) {
        for (int i = 0; i < scene->lights.size(); i++) {        
            calculateLighting(intersection, scene, scene->lights[i], ambientLight, diffuseLight, specularLight);        
        }
    }

    // add in global lighting (if required)
    if (giSamples > 0) {
                    
        for (int i = 0; i < giSamples; i++) {
            // trace a path from this point in a random direction, then use that points radience as a 'light'                        
            // we take the ray pointing in the normal direction then 'defocus' it by up to 90 degrees.  This ?should? give uniform
            // sampling over the hemisphere.  Would probably be better use a more practical PDF, such as one that samples in the reflected
            // direction more? but this will still work, it'll just converge slowly.

                        
            // this is a fast (but biased) way to sample from the hemisphere, just pick a random location, normalise it, then
            // if it's on the wrong side reverse it.             
            glm::vec3 rayDir = glm::normalize(glm::vec3(randf()-0.5f,randf()-0.5f,randf()-0.5f)); 
            float diffusePower = glm::dot(rayDir, intersection.normal);
            if (diffusePower < 0) {
                rayDir = -rayDir;                    
                diffusePower = -diffusePower;
            }

            float sqrtDiffusePower = sqrt(diffusePower);

            // a nice optimization.  rays at a strong angle of incidence will contribute much less to the 
            // lighting, so we can sample them less by discarding them.

            // we 'split the difference' here, so cosTheta is 0.25 then we apply a 50% discard chance with a
            // 50% attenuation.  If we simply used the diffuse power for discarding and took the sample at
            // full strength we'd get artifcats as we converge as occasinally very bright samples would be taken
            // and strong angles.
            
            if (randf() > sqrtDiffusePower) continue;                
            diffusePower = sqrtDiffusePower;
            
            Ray giRay = Ray(intersection.location + rayDir * OFFSET_BIAS, rayDir);            
            
            // We then test the color of this ray.  
            // We set giSamples to 1 if gi was enabled, and 0 otherwise, this gives a 2 bounce lighting model.            
            Color sampleRadiance = trace(giRay, scene, depth+1, giSamples > 1 ? 1 : 0);             

            if (sampleRadiance.r != sampleRadiance.r) {
                printf("Hmm, radiance is nan?\n");
                continue;
            }

            // We are doing the integral by sampling, so each radiance sample need to be adjusted as follows:
            // (as explained here https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing/global-illumination-path-tracing-practical-implementation.
            // we must divide by the pdf (which is 1/2pi) aswell as n dot d (which is simply theta)
            //
            // essentially we use using the diffuse lighting model only here.  for specular it's better to just
            // set some reflectivty (+ blur if you like)
            
            diffuseLight += (sampleRadiance * (PI*2.0f) * (1.0f/GI_SAMPLES) * sqrtDiffusePower);
        }
        
    }

    // combine lighting
    Color materialColor = material->getDiffuseColor(intersection.uv);
    Color color = (ambientLight + diffuseLight) * materialColor + specularLight + material->emisiveColor;    
        
    // reflection    
    if(material->reflectivity > 0 && depth < MAX_RECUSION_DEPTH) {
        
        glm::vec3 reflectedDir = glm::reflect(ray.dir, intersection.normal);
        
        // add bluring
        if (material->reflectionBlur > EPSILON) {            
            reflectedDir = defocus(reflectedDir, material->reflectionBlur);
        }

        Ray reflectedRay(intersection.location + reflectedDir * OFFSET_BIAS, reflectedDir);
        Color reflectedCol = trace(reflectedRay, scene, depth+1, giSamples); 
        color += (material->reflectivity*reflectedCol);        
    }

    if (materialColor.a < 1) {        
        if (material->refractionIndex == 1.0) {        

            // ----------------
            // transparency 
    
            // start the ray a little further on from where we hit.
            Ray transmittedRay = Ray(intersection.location + OFFSET_BIAS * ray.dir, ray.dir);
            Color transmittedCol = trace(transmittedRay, scene, depth, giSamples); 
            color += (1.0f-materialColor.a)*transmittedCol;
            
        } else {            

            // ----------------
            // refraction
    
            glm::vec3 refractedDir = glm::refract(ray.dir, intersection.normal, 1.0f/material->refractionIndex);

            Ray refractedRay = Ray(intersection.location + refractedDir * 0.001f , refractedDir);
            
            // the refracted ray will exit the object at this location, don't trace against entire scene, just trace against the 
            // specific object (faster, and less prone to error).
            RayIntersectionResult exitPoint = intersection.target->intersect(refractedRay);

            if (exitPoint.didCollide()) {
                glm::vec3 exitDir = glm::refract(refractedDir, -exitPoint.normal, material->refractionIndex);
                Ray exitRay = Ray(exitPoint.location + exitDir * 0.001f, exitDir);
                Color refractedCol = trace(exitRay, scene, depth+1, giSamples); 
                color += (1.0f-materialColor.a)*refractedCol;
            } else {
                // this case shouldn't happen, but might due to rounding... just ignore                 
            }
        }
    }
    
	return color;
}

void Camera::renderPixel(Scene* scene, int pixel)
{        

    float aspectRatio = float(SCREEN_WIDTH / SCREEN_HEIGHT);            

    int x = pixel % SCREEN_WIDTH;
    int y = pixel / SCREEN_WIDTH;        

    if (lqMode && ((x&1==1) || (y&1==1))) {
        return;
    }
    
    Color outputCol = Color(0, 0, 0, 1);

    int requiredSamples = (superSample == 0 ? 1 : superSample);

    for (int j = 0; j < requiredSamples; j++) {        
        float jitterx = (superSample == 0) ? 0.5 : randf();
        float jittery = (superSample == 0) ? 0.5 : randf();

        // find the rays direction
        float rx = (2 * ((x + jitterx) / SCREEN_WIDTH) - 1) * tan(fov / 2 * M_PI / 180) * aspectRatio;
        float ry = (1 - 2 * ((y + jittery) / SCREEN_HEIGHT)) * tan(fov / 2 * M_PI / 180);
        glm::vec3 dir = glm::normalize(glm::vec3(rx, -ry, -1));

        // apply camera tranform
        dir = toParent(glm::vec4(dir.x, dir.y, dir.z, 0.0));

        // defocus
        if (defocusBlur > EPSILON) {
            dir = defocus(dir, defocusBlur);
        }
        
        Ray ray = Ray(location, dir);
        Color col = trace(ray, scene, 0, (lightingModel == LM_GI) ? GI_SAMPLES : 0);
        outputCol = outputCol + (col * (1.0f/requiredSamples));
    }
            
    // higher weight for more samples.

    if (lqMode) {
        // render 2x2 block
        gfx.addSample(x+1, y, outputCol, 0.01f + superSample);        
        gfx.addSample(x, y+1, outputCol, 0.01f + superSample);        
        gfx.addSample(x+1, y+1, outputCol, 0.01f + superSample);        
    } else {
        // show where we are up to.
        gfx.putPixel(x, y+1, Color(0,0,0,1), true);            
        gfx.putPixel(x, y+2, Color(1,1,1,1), true);                
    }

    gfx.addSample(x, y, outputCol, 0.01f + superSample);                                	
}

/** Renders given number of pixels before returning control. */
int Camera::render(Scene* scene, int pixels, bool autoReset)
{	
	int totalPixels = SCREEN_WIDTH * SCREEN_HEIGHT;	

	if (pixels == -1) {
		pixels = totalPixels - pixelOn;
	}
		
    if (pixelOn+pixels > totalPixels) {
        pixels = totalPixels - pixelOn - 1;
    }    

    for (int i = 0; i < pixels; i++) {
        renderPixel(scene, pixelOn+i);
    }
    
    pixelOn += pixels;
    
	return pixels; 
}