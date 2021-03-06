#pragma once

/**
 * Material Class
 * 
 * Defines an objects material, such as lighting properties, reflections, refraction etc. 
 */

#include <glm/glm.hpp>

#include "Utils.h"
#include "Texture2D.h"


class Material
{
public:

    // materials have colors and textures for diffuse.  If the texture is not assigned the color is used, if the 
    // texture is assigned is it modulated with the color.
    
    Color diffuseColor;                     // materials color (used for ambient color aswell)
    Color emisiveColor;                    // materials emissive color.
	Texture2D* diffuseTexture=NULL;         // the materials diffuse texture
    Texture2D* normalTexture=NULL;          // the normal map for the texture
    
    // refraction / reflection properties
    float reflectivity = 0.0f;              // how much the material is reflective 1=pefect mirror.
    float reflectionBlur = 0.0f;            // how blured the reflection should be (in radians), requires super sampling to look good.
    float refractionIndex = 1.0f;           // refractive index of object.  Must set material diffuse alpha to see this.    
    float shininess = 25.0f;                // how shiny the object is 
    
    /* Create a default white material. */
	Material() {        
        this->diffuseColor = Color(1,1,1,1);
        this->emisiveColor = Color(0,0,0,1);
    }    

    /** Returns if this material requires UV co-ordantes or not */
    bool needsUV()
    {
        return (diffuseTexture || normalTexture);
    }

    /** Gets diffuse color of material at given uv co-ord */
    glm::vec4 getDiffuseColor(glm::vec2 uv = glm::vec2(0,0)) {
        if (diffuseTexture) {
            return diffuseColor * diffuseTexture->sample(uv);
        } else {
            return diffuseColor;
        }
    }

    // Creates a default material with given diffuse color. 
    static Material* Default(glm::vec4 color = glm::vec4(1,1,1,1))
    {
        Material* material = new Material();
        material->diffuseColor = color;
        return material;
    }

    // Creates a default reflective material. 
    static Material* Reflective(glm::vec4 color = glm::vec4(1,1,1,1), float reflectivity = 0.5f)
    {
        Material* material = new Material();
        material->diffuseColor = color;
        material->reflectivity = reflectivity;
        return material;
    }

    // Creates a default refractive material. 
    static Material* Refractive(glm::vec4 color = glm::vec4(1,1,1,0.1), float refractionIndex = 1.33f)
    {
        Material* material = new Material();
        material->diffuseColor = color;
        material->refractionIndex = refractionIndex; 
        material->reflectivity = 0.5f;
        return material;
    }

    // Creates a default checkerboard material. 
    static Material* Checkerboard(float scale = 32.0f, glm::vec4 color1 = glm::vec4(1,1,1,1), glm::vec4 color2 = glm::vec4(0,0,0,1))
    {
        Material* material = new Material();
        material->diffuseTexture = new CheckerboardTexture(scale, color1, color2);                
        return material;
    }

    // Creates a default emissive material. 
    static Material* Emissive(glm::vec4 color = glm::vec4(1,1,1,1))
    {
        Material* material = new Material();
        material->diffuseColor = Color(0.5f,0.5f,0.5f,1);
        material->emisiveColor = color;
        return material;
    }
};