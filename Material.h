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

    // the materials diffuse color. 
	glm::vec3 color;

    // how much the material is reflective .
    float reflectivity = 0.0f;

	Material() {
        this->color = glm::vec3(1,0,1); ///default color...
    }    

    Material(float r, float g, float b, float reflectivity = 0) {
        this->color = glm::vec3(r,g,b);
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
};