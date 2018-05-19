/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* See Lab07.pdf for details.
*=========================================================================
*/

#include <stdio.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

#include "SceneObject.h"
#include "ContainerObject.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Polyhedron.h"

#include "math.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

// Global list of scene objects.
ContainerObject scene;


/**
 * Computes the colour value obtained by tracing a ray and finding its 
 * closest point of intersection with objects in the scene.
 */
glm::vec3 trace(Ray ray, int depth)
{
	glm::vec3 backgroundColor(0.1,0.0,0.5);
	glm::vec3 lightPosition(10, 40, -3);
	glm::vec3 ambientLight(0.2);   //Ambient color of light

    float shinyness = 4.0;

    RayIntersectionResult intersection = scene.intersect(ray);

    if (!intersection.didCollide()) return backgroundColor;      //If there is no intersection return background colour

    Material* material = intersection.target->material;

    // we check if uv co-ords need calculating as they can be quite slow (transidental functions for example).  
    glm::vec2 uv = glm::vec2(0,0);
    if (material->needsUV()) {        
        uv = intersection.target->getUV(intersection.location);
        printf("uv %f %f\n", uv.x, uv.y);
    }

    // sample materials properties
    glm::vec4 diffuseColor = material->getDiffuseColor(uv);

    // diffuse light
    glm::vec3 normalVector = intersection.normal;
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
    RayIntersectionResult shadowIntersection = scene.intersect(shadow);    
    if (shadowIntersection.didCollide() && shadowIntersection.t < lightVector.length()) {
        diffuseLight = specularLight = 0;
    }    

    // apply the lighting model
    // note: transpariency (alpha) only applies to the ambient and diffuse, the specular remains the same (as this is really a reflection)
    float alpha = diffuseColor.a;
    glm::vec3 colorSum = (ambientLight * glm::vec3(diffuseColor) + diffuseLight * glm::vec3(diffuseColor)) * alpha + specularLight;
    
    // reflection    
    if(material->reflectivity > 0 && depth < MAX_STEPS) {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(intersection.location, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, depth+1); 
        colorSum = colorSum + (0.8f*reflectedCol);
    }

    // transparency 
    // note: we don't bump up the depth counter here as we can't recurse infinitely with transparency.
    if (alpha < 1) {
        if (material->refractionIndex == 1.0) {        
    
            // start the ray a little further on from where we hit.
            Ray transmittedRay = Ray(intersection.location + 0.001f * ray.dir, ray.dir);
            glm::vec3 transmittedCol = trace(transmittedRay, depth); 
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
                glm::vec3 refractedCol = trace(exitRay, depth+1); 
                colorSum = colorSum + (1.0f-alpha)*refractedCol;
            } else {
                // this case shouldn't happen, but might due to rounding... just ignore (i.e. use black color)
                colorSum = glm::vec3(1,0,1);
            }
        }
    }
    
	return colorSum;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'			
		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

	//-- Create a pointer to a sphere object
	Sphere* sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -90.0), 15.0);
    Sphere* sphere2 = new Sphere(glm::vec3(+4.0, +3.0, -50.0), 4.0);
    Sphere* sphere3 = new Sphere(glm::vec3(+8.0, -8.0, -70.0), 4.0);

    Plane* plane = new Plane(glm::vec3(-20, -20, -40), 
        glm::vec3(20, -20, -40), 
        glm::vec3(20, -20, -200), 
        glm::vec3(-20, -20, -200) 
    ); 

    Cylinder* cylinder = new Cylinder(glm::vec3(0,-19,-90), 5, 10);

    sphere1->material = Material::Default();
    sphere2->material = Material::Checkerboard();
    sphere3->material = Material::Default(glm::vec4(0,1,0,1));
    plane->material = Material::Checkerboard();
    cylinder->material =  Material::Default(glm::vec4(1,0,0,1));

    plane->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");

	//--Add the above to the list of scene objects.
	scene.add(plane); 
    //scene.add(sphere1); 
    //scene.add(Polyhedron::Cube());
    //scene.add(sphere2); 
    //scene.add(sphere3); 
    scene.add(cylinder);
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
