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
#include "Cube.h"
#include "Scene.h"
#include "Light.h"

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
Scene* scene;

// Main camera
Camera camera;

// --------------------------------

bool DOUBLE_RENDER = true;
bool AUTO_RENDER = true;

const int LQ_RAYS = 0;
const int HQ_RAYS = 1;

const int RM_NONE = 0;
const int RM_LQ = 1;
const int RM_HQ = 2;

// --------------------------------

float lastFrameTime = 0.0f;
int counter = 0;
int frameOn = 0;


int render_mode = RM_LQ;

// if assigned this object will animate.
SceneObject* animatedObject = NULL;

// --------------------------------

/** Forces camera redraw. */
void redraw()
{
    render_mode = RM_LQ;
    camera.reset();
    gfx.clear();
}

void keyboard(unsigned char key, int x, int y)
{    
    switch (key) {
        case 'w': camera.move(+5,0); break;
        case 's': camera.move(-5,0); break;
        case 'a': camera.move(0, -5); break;
        case 'd': camera.move(0, +5); break;
        case 'z': camera.move(0, 0, +5); break;
        case 'c': camera.move(0, 0, -5); break;
        case 'q': camera.rotate(+0.1f,0); break;
        case 'e': camera.rotate(-0.1f,0); break;
        case ' ': break; // force render
        default:
            // skip the redraw
            return;
    }
    
    if (AUTO_RENDER) {
        // auto render draws all the time, but camera needs reseting on movement
        gfx.clear(Color(0,0,0,0),true);
        camera.reset();
        render_mode = RM_LQ;
    } else {
        // redraw when changes are made.
        redraw();
    }
}

void update(void)
{    
    currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;	
	float elapsed = currentTime - lastFrameTime;

	if (elapsed < (0.5 / 1000.0)) {
		return;
	}

    if (animatedObject) {        
        animatedObject->setRotation(animatedObject->getRotation() + 0.001f * glm::vec3(2,1,0));
    }

	clock_t t;
	t = clock();
	int pixelsRendered = 0;
	switch (render_mode) {
		case RM_LQ:
            camera.superSample = LQ_RAYS;
            camera.GI_SAMPLES = 4; // super quick render...
            camera.lqMode = true;
			pixelsRendered = camera.render(5 * 1000, false);
			if (pixelsRendered == 0) {
                if (DOUBLE_RENDER) {                    
				    render_mode = RM_HQ;
				    camera.reset();
                } else if (AUTO_RENDER) {
                    camera.reset();
                }
			}
			break;
		case RM_HQ:
            camera.superSample = HQ_RAYS;
            camera.GI_SAMPLES = 64;
            camera.lqMode = false;
			pixelsRendered = camera.render(5 * 100, true);
			if (pixelsRendered == 0) {
				render_mode = RM_NONE;
			}
			break;
	}
	
	float timeTaken = float(clock() - t) / CLOCKS_PER_SEC;	
	float pixelsPerSecond = (timeTaken == 0) ? -1 : pixelsRendered / timeTaken;
	if (counter == 10) {
		printf("Pixels per second = %fk.\n", pixelsPerSecond/1000);
	}

	counter++;
	
	glutPostRedisplay();
	lastFrameTime = currentTime;
}

/** Simple scene to test GI. */
void initGIScene()
{    
    //scene->add(new Light(glm::vec3(-10,30,0), Color(1,1,1,1)));
	//-- Create a pointer to a sphere object
	Sphere* sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -50.0), 15.0);
    Sphere* sphere2 = new Sphere(glm::vec3(+4.0, +3.0, -30.0), 4.0);
    Sphere* sphere3 = new Sphere(glm::vec3(-16.0, +8.0, -20.0), 4.0);

    Plane* plane = new Plane(glm::vec3(0, -20, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        

    sphere1->material = Material::Default(Color(1,1,1,1));
    sphere2->material = Material::Checkerboard();
    sphere3->material = Material::Default(Color(0,1,0,1));    
    plane->material = Material::Checkerboard(1.0f);
    
    plane->material->reflectivity = 0.5f;
    plane->material->reflectionBlur = 0.01f;

    sphere3->material->emisiveColor = Color(1,0.2f,0.9f,1)*1.0f;
    sphere2->material->emisiveColor = Color(1,0.75f,0,1)*0.5f;
    
	//--Add the above to the list of scene objects.
	scene->add(plane); 
    scene->add(sphere1);     
    scene->add(sphere2); 
    scene->add(sphere3);     

    // origin marker
    scene->add(new Sphere(glm::vec3(0,-20,0),3.0f));

    camera.lightingModel = LM_GI;    

    // blue sky light
    camera.backgroundColor = Color(0.03,0.05,0.15,1) * 1.0f;
}

/**
 * A simple scene to test the raytracer 
 */
void initTestScene()
{

    // lights
    scene->add(new Light(glm::vec3(-10,30,0), Color(1,0.2,0.2,1)));
    // lights
    scene->add(new Light(glm::vec3(+10,30,0), Color(0.2,1,0.2,1)));
    
	//-- Create a pointer to a sphere object
	Sphere* sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -50.0), 15.0);
    Sphere* sphere2 = new Sphere(glm::vec3(+4.0, +3.0, -30.0), 4.0);
    Sphere* sphere3 = new Sphere(glm::vec3(+8.0, -8.0, -20.0), 4.0);

    Plane* plane = new Plane(glm::vec3(0, -20, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        

    sphere1->material = Material::Default(Color(1,1,1,0.1));
    sphere2->material = Material::Checkerboard();
    sphere3->material = Material::Reflective(Color(0,1,0,1));
    sphere3->material->reflectionBlur = 0.3f;
    plane->material = Material::Checkerboard(1.0f);
    
	//--Add the above to the list of scene objects.
	scene->add(plane); 
    scene->add(sphere1);     
    scene->add(sphere2); 
    scene->add(sphere3);     

    // origin marker
    scene->add(new Sphere(glm::vec3(0,-20,0),3.0f));
}


/** Mostly to test the transforms. */
void initAnimatedScene() 
{
    // lights
    scene->add(new Light(glm::vec3(-10,30,0), Color(1,0.5,0.5,1)));
    scene->add(new Light(glm::vec3(+10,30,0), Color(0.5,1,0.5,1)));
    scene->add(new Light(glm::vec3(0,30,0), Color(0.5,0.5,1,1)));
    	
    Plane* plane = new Plane(glm::vec3(0, -20, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
    scene->add(plane); 
    
    /*
    Sphere* sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -50.0), 15.0);
    scene->add(sphere1); 
    */
        
    SceneObject* box = new Cube(glm::vec3(0,0,-50), glm::vec3(10,10,10));    

    //SceneObject* box = new Sphere(glm::vec3(0,0,-50), 15);    
    
    /*
    SceneObject* box = new Plane(
            glm::vec3(-20,-20,0), 
            glm::vec3(+20,-20,0), 
            glm::vec3(+20,+20,0),
            glm::vec3(-20,+20,0));
    */  

    animatedObject = box;
    scene->add(box);    

    plane->material = Material::Checkerboard(1.0f);
	
    // origin marker
    scene->add(new Sphere(glm::vec3(0,-20,0),3.0f));
}

/**
 * This is a standard cornell box (https://en.wikipedia.org/wiki/Cornell_box) with some additional objects to demonstrate
 * the raytracers capabilities and meet assignment requirements. 
 * */
void initCornellScene()
{
    // lights
    scene->add(new Light(glm::vec3(0,30,0)));

    // infinite planes are a little faster than clipped planes and won't have potential artifacts at the edges.
    Plane* leftPlane = new Plane(glm::vec3(-40,0,0), glm::vec3(1,0,0));
    Plane* rightPlane = new Plane(glm::vec3(+40,0,0), glm::vec3(-1,0,0));
    Plane* backPlane = new Plane(glm::vec3(0,0,-80), glm::vec3(0,0,1));
    Plane* forePlane = new Plane(glm::vec3(0,0,20), glm::vec3(0,0,-1));
    Plane* floorPlane = new Plane(glm::vec3(0,40,0), glm::vec3(0,-1,0));
    Plane* ceilingPlane = new Plane(glm::vec3(0,-40,0), glm::vec3(0,1,0));

    // make the colors a little pastal.
    leftPlane->material = Material::Default(Color(0.9,0.1,0.1,1.0));
    rightPlane->material = Material::Default(Color(0.1,0.9,0.1,1.0));
    backPlane->material = forePlane->material = floorPlane->material = ceilingPlane->material = Material::Default(Color(0.9,0.9,0.9,1.0));

    // reflective blury sphere
    Sphere* sphere = new Sphere(glm::vec3(0,-20,-70), 10.0f);
    sphere->material = Material::Reflective(glm::vec4(0.1f,0.1f,0.1f,1.0f), 0.8f);

    // a framed mandelbrot picture in the background

    Cube* pictureFrame = new Cube(glm::vec3(0,0,-80), glm::vec3(50,50,10));    
    Cube* picture = new Cube(glm::vec3(0,0,-79), glm::vec3(45,45,10));

    Material* woodMaterial = new Material();
    woodMaterial->diffuseTexture = new BitmapTexture("./textures/Wood_plank_007_COLOR.png");
    woodMaterial->normalTexture = new BitmapTexture("./textures/Wood_plank_007_NORM.png", true);
    pictureFrame->setMaterial(woodMaterial);
    
    Material* pictureMaterial = new Material();
    pictureMaterial->diffuseTexture = new MandelbrotTexture();
    // todo: might be nice to add some ruff normal texture (such as canvas) to this.    
    picture->setMaterial(pictureMaterial);

    // create pedestals with object ontop.
    glm::vec3 pos;
    Cube* petastool;
    SceneObject* object;
    
    // note this would work better with grouped objects and object transforms... 

    // 1> refractive:
    pos = glm::vec3(-30,-40,-50);
    petastool = new Cube(pos, glm::vec3(10,10,10));
    object = new Sphere(pos + glm::vec3(0,10,0), 5.0f);
    object->material = Material::Refractive(Color(0.5,0.1,0.1,0.1));
    scene->add(petastool);
    scene->add(object);
    
    // 2> transparient:
    pos = glm::vec3(-10,-40,-50);
    petastool = new Cube(pos, glm::vec3(10,10,10));
    object = new Sphere(pos + glm::vec3(0,10,0), 5.0f);
    object->material = Material::Default(Color(0.1,0.5,0.1,0.1));
    scene->add(petastool);
    scene->add(object);

    // 3> textured:
    pos = glm::vec3(+10,-40,-50);
    petastool = new Cube(pos, glm::vec3(10,10,10));
    object = new Sphere(pos + glm::vec3(0,10,0), 5.0f);
    object->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
    object->material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);
    scene->add(petastool);
    scene->add(object);    

    // 4> standard:
    pos = glm::vec3(+30,-40,-50);
    petastool = new Cube(pos, glm::vec3(10,10,10));
    object = new Sphere(pos + glm::vec3(0,10,0), 5.0f);
    object->material = Material::Default(Color(0.1f,0.1f,0.6f,1.0f));
    scene->add(petastool);
    scene->add(object);
        
    scene->add(leftPlane);
    scene->add(rightPlane);
    scene->add(backPlane);
    scene->add(floorPlane);
    scene->add(ceilingPlane);
    scene->add(forePlane);

    scene->add(pictureFrame);
    scene->add(picture);

    scene->add(sphere);
}

void initScene()
{
    scene = new Scene();

    initGIScene();

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
    glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutMainLoop();
	return 0;
}
