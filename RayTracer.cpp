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
#include <sstream>

#include <glm/glm.hpp>
#include <GL/glut.h>

#include "SceneObject.h"
#include "ContainerObject.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cube.h"
#include "Scene.h"
#include "Light.h"
#include "Mesh.h"

#include "ExampleScenes.h"

#include "Camera.h"
#include "GFX.h"
#include "PLYReader.h"

#include "math.h"

using namespace std;

//----------------------------------
// Globals
// --------------------------------

float currentTime = 0.00;

// Reference to the current scene
Scene* currentScene;

// Reference to current camera
Camera* camera;

// list of scenes we can switch between.
vector<Scene*> scenes = vector<Scene*>();

// --------------------------------

bool DOUBLE_RENDER = true;
bool AUTO_RENDER = true;

enum RUN_MODE {RM_MANUAL, RM_RENDER_AND_EXIT};

const int LQ_RAYS = 0;
const int HQ_RAYS = 1;

const int RM_NONE = 0;
const int RM_LQ = 1;
const int RM_HQ = 2;

const int requiredPasses = 16;

RUN_MODE mode = RM_MANUAL;

// --------------------------------

float lastFrameTime = 0.0f;
int counter = 0;
int frameOn = 0;

int render_mode = RM_LQ;
int initialScene = 1;

int passes = 0;

// --------------------------------

/** Activates given scene. */
void activateScene(int sceneNumber) 
{
    if (sceneNumber < 0 || sceneNumber >= scenes.size()) {
        printf("Invalid scene number %d", sceneNumber);
        return;
    }

    printf("Activating scene %d.\n", sceneNumber);
    
    currentScene = scenes[sceneNumber];
    if (!currentScene->isLoaded()) {
        currentScene->load();
    }
    camera = currentScene->camera;

    // stick to quick updates in animated mode.
    DOUBLE_RENDER = !currentScene->isAnimated;
}

/** Forces camera redraw. */
void redraw()
{
    render_mode = RM_LQ;
    camera->reset();
    gfx.clear();
    passes = 0;
}

void specialKeyboard(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_F1: camera->lightingModel = LM_DIRECT; break;
        case GLUT_KEY_F2: camera->lightingModel = LM_GI; break;    
        case GLUT_KEY_F3: camera->lightingModel = LM_DEPTH; break;    
        case GLUT_KEY_F4: camera->lightingModel = LM_WORLD; break;    
        case GLUT_KEY_F5: camera->lightingModel = LM_LOCAL; break;    
        case GLUT_KEY_F6: camera->lightingModel = LM_NORMAL; break;    
        case GLUT_KEY_F7: camera->lightingModel = LM_UV; break;    
    }
    redraw();
}

void keyboard(unsigned char key, int x, int y)
{    
    switch (key) {
        case '0': activateScene(0); break;
        case '1': activateScene(1); break;
        case '2': activateScene(2); break; 
        case '3': activateScene(3); break;
        case '4': activateScene(4); break;
        case '5': activateScene(5); break;
        case '6': activateScene(6); break;
        case '7': activateScene(7); break;
        case '8': activateScene(8); break;
        case 'w': camera->move(+3,0); break;
        case 's': camera->move(-3,0); break;
        case 'a': camera->move(0, -3); break;
        case 'd': camera->move(0, +3); break;
        case 'z': camera->move(0, 0, +1); break;
        case 'c': camera->move(0, 0, -1); break;
        case 'q': camera->rotate(+0.1f,0); break;
        case 'e': camera->rotate(-0.1f,0); break;
        case 'p': gfx.screenshot("Screenshot.tga");
        case ' ': 
            // force render, but also print locaiton.
            printf("Camera at:");
            print(camera->getLocation());
            printf("Camera rotation:");
            print(camera->getRotation());        
            break; 
        default:
            // skip the redraw
            return;
    }    
    
    if (AUTO_RENDER) {
        // auto render draws all the time, but camera needs reseting on movement
        gfx.clear(Color(0,0,0,0),true);
        camera->reset();
        render_mode = RM_LQ;
    } else {
        // redraw when changes are made.
        redraw();
    }
}

void update(void)
{    

    // this shouldn't happen, but just in case.
    if (currentScene == NULL || camera == NULL)
        return;

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
            camera->superSample = LQ_RAYS;
            camera->GI_SAMPLES = 4; // super quick render...
            camera->lqMode = true;
			pixelsRendered = camera->render(currentScene, 5 * 1000, false);
			if (pixelsRendered == 0) {
                // update on frame finish.
                currentScene->update();
                if (DOUBLE_RENDER) {                    
				    render_mode = RM_HQ;
				    camera->reset();
                } else if (AUTO_RENDER) {
                    camera->reset();
                    gfx.clear(Color(0,0,0,0), true);
                }
			}
			break;
		case RM_HQ:
            camera->superSample = HQ_RAYS;
            camera->GI_SAMPLES = 64;
            camera->lqMode = false;
			pixelsRendered = camera->render(currentScene, 5 * 100, false);
			if (pixelsRendered == 0) {
                passes++;
                if (mode == RM_RENDER_AND_EXIT && passes >= requiredPasses) {
                    gfx.screenshot(currentScene->name+".tga");
                    exit(0);
                }
                camera->reset();				
			}
			break;
	}
	
	float timeTaken = float(clock() - t) / CLOCKS_PER_SEC;	
	float pixelsPerSecond = (timeTaken == 0) ? -1 : pixelsRendered / timeTaken;
	if (counter == 10) {
		printf("Pixels per second = %dk.\n", (int)(pixelsPerSecond/1000));
	}

	counter++;
	
	glutPostRedisplay();
	lastFrameTime = currentTime;
}

void display(void)
{
	gfx.blit();
}

void initialize()
{
    printf("Initializing graphics library.\n");
    gfx.init();

    printf("Loading scenes.\n");
    
    scenes.push_back(new TestScene()); // this is realy just here to pad out scene '0'.
    scenes.push_back(new BasicScene());        
    scenes.push_back(new CornellBoxScene());        
    scenes.push_back(new AnimatedScene());        
    scenes.push_back(new MaterialSpheresScene());        
    scenes.push_back(new DragonScene());        
    scenes.push_back(new ManyDragonsScene());        
    scenes.push_back(new AreaLightScene());        
    
    activateScene(initialScene);

}

int main(int argc, char *argv[]) {

    switch (argc) {
        case 1: 
            // standard            
            break;
        case 2: 
            // render and exit
            istringstream ss(argv[1]);
            int sceneNumber;
            if (!(ss >> sceneNumber)) {
                cerr << "Invalid input " << argv[1] << ", please use an integer.\n";
                return -1;
            }
            printf("Auto rendering scene %d to file and exiting.\n", sceneNumber);
            mode = RM_RENDER_AND_EXIT;
            initialScene = sceneNumber;
            break;
    }

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("RayTracer");
	initialize();

    if (mode != RM_RENDER_AND_EXIT) {
        // don't  use keyboard commands if we are in render and exit mode.
        glutKeyboardFunc(keyboard);
        glutSpecialFunc(specialKeyboard);
    }

	glutDisplayFunc(display);    
	glutIdleFunc(update);
	glutMainLoop();
	return 0;
}
