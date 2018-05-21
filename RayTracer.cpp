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

#include "Camera.h"
#include "GFX.h"

#include "math.h"
#include <GL/glut.h>
using namespace std;

//----------------------------------
// Globals
// --------------------------------

float currentTime = 0.00;

// Global list of scene objects.
ContainerObject* scene;

// Main camera
Camera camera;

// --------------------------------

const int LQ_RAYS = 1;
const int HQ_RAYS = 16;

const int RM_NONE = 0;
const int RM_LQ = 1;
const int RM_HQ = 2;

// --------------------------------

float lastFrameTime = 0.0f;
int counter = 0;
int frameOn = 0;


int render_mode = RM_LQ;

// --------------------------------


void update(void)
{
    currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;	
	float elapsed = currentTime - lastFrameTime;

	if (elapsed < (0.5 / 1000.0)) {
		return;
	}

	clock_t t;
	t = clock();
	int pixelsRendered = 0;
	switch (render_mode) {
		case RM_LQ:
			pixelsRendered = camera.render(20 * 1000, LQ_RAYS);
			if (pixelsRendered == 0) {
				render_mode = RM_HQ;
				camera.pixelOn = 0;
			}
			break;
		case RM_HQ:
			pixelsRendered = camera.render(2 * 100, HQ_RAYS);
			if (pixelsRendered == 0) {
				render_mode = RM_NONE;
			}
			break;
	}
	
	float timeTaken = float(clock() - t) / CLOCKS_PER_SEC;	
	float pixelsPerSecond = (timeTaken == 0) ? -1 : pixelsRendered / timeTaken;
	if (counter == 0) {
		printf("Pixels per second = %fk.\n", pixelsPerSecond/1000);
	}

	counter++;
	
	glutPostRedisplay();
	lastFrameTime = currentTime;
}

void initScene()
{
    scene = new ContainerObject();

	//-- Create a pointer to a sphere object
	Sphere* sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -50.0), 15.0);
    Sphere* sphere2 = new Sphere(glm::vec3(+4.0, +3.0, -30.0), 4.0);
    Sphere* sphere3 = new Sphere(glm::vec3(+8.0, -8.0, -20.0), 4.0);

    Plane* plane = new Plane(glm::vec3(-20, -20, -20), 
        glm::vec3(20, -20, -20), 
        glm::vec3(20, -20, -50), 
        glm::vec3(-20, -20, -50) 
    ); 

    Cylinder* cylinder = new Cylinder(glm::vec3(20,-20,-90), 5, 10);

    sphere1->material = Material::Default();
    sphere2->material = Material::Checkerboard();
    sphere3->material = Material::Reflective(glm::vec4(0,1,0,1));
    plane->material = Material::Default(); plane->material->diffuseTexture = new MandelbrotTexture();
    cylinder->material =  Material::Default(glm::vec4(1,0,0,1));

    sphere1->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
    sphere1->material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);

	//--Add the above to the list of scene objects.
	scene->add(plane); 
    scene->add(sphere1); 
    scene->add(Polyhedron::Cube(glm::vec3(2,2,-10)));
    scene->add(sphere2); 
    scene->add(sphere3); 
    scene->add(cylinder);

    camera.scene = scene;
}

void display(void)
{
	gfx.blit();
}

void initialize()
{
    gfx.init();
    initScene();    
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("RayTracer");
	initialize();
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutMainLoop();
	return 0;
}
