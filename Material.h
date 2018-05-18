/**
 * Material Class
 * 
 * Defines an objects material, such as lighting properties, reflections, refraction etc. 
 */

#pragma once
#include <glm/glm.hpp>

struct Material
{
public:

    // the materials diffuse color, where alpha is the 4th component. 
	glm::vec4 color;

    // how much the material is reflective 1=pefect mirror.
    float reflectivity = 0.0f;

    // refractive index of object.  Must set material diffuse alpha to see this.
    float refractionIndex = 1.0f;

	Material() {
        this->color = glm::vec4(0.5,0.5,0.5,1); ///default color...
    }    

    Material(float r, float g, float b, float reflectivity = 0) {
        this->color = glm::vec4(r,g,b,1);
        this->reflectivity = reflectivity;
    }    
    
    // Creates a default material. 
    static Material Default(float r,float g,float b)
    {
        return Material(r,g,b);
    }

    // Creates a reflective material. 
    static Material Reflective(float r,float g,float b)
    {
        return Material(r,g,b,0.5f);
    }

    // Creates a refractive material. 
    static Material Refractive(float refractionIndex)
    {
        Material material = Material();
        material.color.a = 0.1;
        material.refractionIndex = refractionIndex;
        return material;
    }
};