/**
 * Collection of example scenes to show the features of the raytracer 
 **/

#pragma once

#include <stdio.h>

#include "Scene.h"
#include "Color.h"
#include "Sphere.h"
#include "Material.h"
#include "Mesh.h"
#include "PLYReader.h"

using namespace std;

/** Generates a parametised material.  types 6+ are textured. */
Material* parameterisedMaterial(int col, int type)
{
    Material* material = new Material();

    // select a base color
    switch (col % 6) {
        case 0: material->diffuseColor = Color(0.9f,0.1f,0.1f,1); break;
        case 1: material->diffuseColor = Color(0.1f,0.9f,0.1f,1); break;
        case 2: material->diffuseColor = Color(0.3f,0.4f,0.9f,1); break;
        case 3: material->diffuseColor = Color(0.1f,0.1f,0.1f,1); break;
        case 4: material->diffuseColor = Color(0.5f,0.5f,0.5f,1); break;
        case 5: material->diffuseColor = Color(1.0f,1.0f,1.0f,1); break;            
    }    

    switch (type % 10) {
        case 0: 
            // simple colored material            
            material->shininess = 5;
            break;
        case 1: 
            // transparient colored material
            material->diffuseColor.a = 0.5f;
            break;
        case 2: 
            // emissive material
            material->emisiveColor = material->diffuseColor * 0.5f;
            material->diffuseColor = Color(0.5f,0.5f,0.5f, 1);            
            break;
        case 3: 
            // refractive
            material->reflectivity = 0.1f;
            material->refractionIndex = 1.25f;
            material->diffuseColor *= 0.5f;
            material->diffuseColor.a = 0.1f;
            break;
        case 4: 
            // reflective
            material->reflectivity = 0.5f;
            material->diffuseColor *= 0.5f;
            material->diffuseColor.a = 1.0f;
            break;        
        case 5: 
            // blured reflection            
            material->reflectivity = 0.9f;
            material->reflectionBlur = 0.5f;
            material->diffuseColor *= 0.1f;
            material->diffuseColor.a = 1.0f;
            break;
        case 6: 
            // make sure color is not too dark.
            material->diffuseColor = (material->diffuseColor + Color(1,1,1,1)) / 2.0f;
            material->diffuseTexture = new CheckerboardTexture();
            break;
        case 7: 
            // make sure color is not too dark.
            material->diffuseColor = (material->diffuseColor + Color(1,1,1,1)) / 2.0f;         
            material->diffuseTexture = new MandelbrotTexture();
            break;
        case 8:
            // make sure color is not too dark.
            material->diffuseColor = (material->diffuseColor + Color(1,1,1,1)) / 2.0f;            
            material->diffuseTexture = new BitmapTexture("./textures/Wood_plank_007_COLOR.png");
            material->normalTexture = new BitmapTexture("./textures/Wood_plank_007_NORM.png", true);
            break;
        case 9:
            // make sure color is not too dark.
            material->diffuseColor = (material->diffuseColor + Color(1,1,1,1)) / 2.0f;
            material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
            material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);    
            break;
    }
    return material;
}

// --------------------------------------------------------------------
// GIScene
// Simple scene to test GI.
// --------------------------------------------------------------------

class GIScene : public Scene
{    
public:

    void loadScene() override {

        name = "GI";

        add(new Light(glm::vec3(-10,30,0), Color(1,1,1,1)));
 
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
        add(plane); 
        add(sphere1);     
        add(sphere2); 
        add(sphere3);     

        // origin marker
        add(new Sphere(glm::vec3(0,-20,0),3.0f));

        camera->lightingModel = LM_GI;    

        // blue sky light
        camera->backgroundColor = Color(0.03,0.05,0.15,1) * 1.0f;	
    }
};    
    

// --------------------------------------------------------------------
// Area Light
// Demonstrates the area light feature.
// --------------------------------------------------------------------

class AreaLightScene : public Scene
{    
public:

    void loadScene() override {

        name = "AreaLight";

        // default light
        Light* light = new Light(glm::vec3(0,2,-15), Color(1,0.5f,0,1));
        light->lightSize = 1.0f;
        add(light);  

        // make the light visible (helps with GI)
        Cube* lightSolid = new Cube(glm::vec3(0,2,-15), glm::vec3(1,8,1));
        lightSolid->castsShadows = false;    
        lightSolid->material->emisiveColor = 5.0f * Color(1.0f,0.5f,0,1);        
        add(lightSolid);

        // some pillars
        Cube* cube;
        for (int i = 0; i < 10; i++) {
            cube = new Cube(glm::vec3(-10+(i*2),0,-10), glm::vec3(0.7,12,0.7));
            add(cube);
        }

        // a sphere
        add(new Sphere(glm::vec3(0,2,0), 1.0f));
        
        // ground plane
        Plane* plane = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        //plane->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
        //plane->material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);    
        plane->material->diffuseColor = Color(0.7,0.7,0.7,1.0);
        plane->material->reflectivity = 0.2f;
        plane->material->shininess = 50;
        add(plane); 

        camera->setLocation(glm::vec3(-9.2,3.0,2));
        camera->setRotation(glm::vec3(0,-0.6,0));    

        // blue sky light
        camera->backgroundColor = Color(0.03,0.05,0.15,1) * 0.2f;
    }    
};


// --------------------------------------------------------------------
// Material Spheres
// --------------------------------------------------------------------
// This scene contains 400 spheres demonstrating the material types.
// --------------------------------------------------------------------
class MaterialSpheresScene : public Scene
{    
public:

    void loadScene() override {

        name = "MaterialSpheres";

        // default light
        Light* light = new Light(glm::vec3(-10,30,0), 0.5f * Color(1,1,1,1));
        add(light);  
        
        // ground plane
        Plane* plane = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        plane->material->diffuseTexture = new CheckerboardTexture(4.0f, Color(0.5f,0.5f,0.5f,1), Color(0.1,0.1,0.1,1));
        plane->material->reflectivity = 0.1f;
        add(plane);     
            
        const int NUM_OBJECTS_X = 20;
        const int NUM_OBJECTS_Y = 20;
        
        // create spheres
        for (int i = 0; i < NUM_OBJECTS_X; i++) {
            for (int j = 0; j < NUM_OBJECTS_Y; j++) {
                Sphere* sphere = new Sphere(glm::vec3((i-(NUM_OBJECTS_X/2))*2,0.5f,(j-(NUM_OBJECTS_Y/2))*2),0.5f);            
                sphere->material = parameterisedMaterial(2+j,2+i+j);            
                sphere->setRotation(glm::vec3(randf(),randf(),randf())); // for texture spheres.
                add(sphere); 
            }    
        }
                    
        camera->lightingModel = LM_DIRECT;
        camera->setLocation(glm::vec3(0,2,12));
        camera->setRotation(glm::vec3(-0.6,0,0));
        camera->move(-6,0,0);

        // blue sky light
        camera->backgroundColor = Color(0.03,0.05,0.15,1) * 0.6f;
    }
};

// --------------------------------------------------------------------
// 100 dragons
// --------------------------------------------------------------------
// This scene contains 100 dragons each with 800k triangles (that is 80 million trianges).
// It demonstrates the efficentcy of the mesh rendering system for high poly counts. */
// --------------------------------------------------------------------
class ManyDragonsScene : public Scene
{    
public:

    void loadScene() override {
    
        name = "ManyDragons";

        // default light
        add(new Light(glm::vec3(-10,30,0), Color(1,1,1,1)));    
        
        // ground plane
        Plane* plane = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        //plane->material->diffuseTexture = new BitmapTexture("./textures/Wood_plank_007_COLOR.png");
        //plane->material->normalTexture = new BitmapTexture("./textures/Wood_plank_007_NORM.png", true);    
        //plane->material->diffuseTexture = new CheckerboardTexture(2.0f);
        add(plane); 

        // mesh objects...
        vector<glm::vec3>* dragonMesh = ReadPLY("./dragon.ply", 10.0f);

        // base dragon
        Mesh* dragon = new Mesh(glm::vec3(0,0,0), dragonMesh);    
        //Sphere* dragon = new Sphere(glm::vec3(0,1.0,0),0.5); 
        //Cube* dragon = new Cube(glm::vec3(0,1,0),glm::vec3(0.5)); 

        const int NUM_OBJECTS_X = 10;
        const int NUM_OBJECTS_Y = 10;
        
        // duplicate dragons
        for (int i = 0; i < NUM_OBJECTS_X; i++) {
            for (int j = 0; j < NUM_OBJECTS_Y; j++) {
                // rather than creating new dragons (which would repartition them and load the mesh into memory again)
                // we can instead create copies of them with the ReferenceObject type.  This allows for a copy of 
                // the origional object, but with a new transform applied.
                ReferenceObject* dragonCopy = new ReferenceObject(glm::vec3((i-(NUM_OBJECTS_X/2))*2,-0.5,(j-(NUM_OBJECTS_Y/2))*2), dragon);
                dragonCopy->setRotation(glm::vec3(0, (randf()-0.5f)*PI*0.1f + (0.4f*PI), 0));
                dragonCopy->material = parameterisedMaterial(j,i);            
                add(dragonCopy); 
            }    
        }
                    
        camera->lightingModel = LM_DIRECT;
        camera->setLocation(glm::vec3(0,3,12));
        camera->setRotation(glm::vec3(-0.5,0,0));

        // blue sky light
        camera->backgroundColor = Color(0.03,0.05,0.15,1) * 0.6f;
    }
};

// --------------------------------------------------------------------
// Dragon mesh
// --------------------------------------------------------------------
// This scene contains a single dragon demonstrating the mesh rendering.
// --------------------------------------------------------------------

class DragonScene : public Scene
{    
public:

    void loadScene() override {

        name = "Dragon";

        // default light
        add(new Light(glm::vec3(-10,30,0), 0.5f * Color(1,1,1,1)));    
        
        // ground plane
        Plane* plane = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        plane->material->diffuseColor = Color(0.3f,0.3f,0.3f,1);

        plane->material->reflectivity = 0.25;
        plane->material->reflectionBlur = 0.1f; // in GI mode specular hilights are handled as blury reflections.
        add(plane); 
        
        // our high res mesh
        Mesh* mesh = new Mesh(glm::vec3(0,0,0), ReadPLY("./dragon.ply", 20.0f));    
        mesh->setLocation(glm::vec3(0,-1,-7.5));        
        add(mesh); 
        
        // lighting blocks    
        Cube* blockLight1 = new Cube(glm::vec3(0,0,-10),glm::vec3(4,2,0.5));
        blockLight1->material = Material::Emissive(Color(1,0,0,1) * 0.5f);
        add(blockLight1);

        Cube* blockLight2 = new Cube(glm::vec3(0,0,-5),glm::vec3(4,2,0.5));
        blockLight2->material = Material::Emissive(Color(0,1,0,1) * 0.33f);
        add(blockLight2);
        
        // blue sky light
        camera->backgroundColor = Color(0.03,0.05,0.15,1) * 0.6f;

        // setup camera and lighting
        camera->lightingModel = LM_GI;
        camera->setLocation(glm::vec3(-3.5,2,-7.5));
        camera->setRotation(glm::vec3(0,4.7,0));
        camera->move(-1,0,0);
    }
};

// --------------------------------------------------------------------
// Test Scene
// --------------------------------------------------------------------
// A simple scene to test the render.
// --------------------------------------------------------------------
class TestScene : public Scene
{    
public:

    void loadScene() override {

        name = "Test";

        // lights
        add(new Light(glm::vec3(-10,30,0), Color(1,0.2,0.2,1)));
        // lights
        add(new Light(glm::vec3(+10,30,0), Color(0.2,1,0.2,1)));
        
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
        add(plane); 
        add(sphere1);     
        add(sphere2); 
        add(sphere3);     

        // origin marker
        add(new Sphere(glm::vec3(0,-20,0),3.0f));
    }
};

// --------------------------------------------------------------------
// Animated Scene
// --------------------------------------------------------------------
// Demonstrates the updating of object transforms.
// --------------------------------------------------------------------
class AnimatedScene : public Scene
{    
    SceneObject* box;

public:

    void update() override {
        glm::vec3 rotation = box->getRotation();
        box->setRotation(rotation + glm::vec3(0.01f,0.1f,0.2f));
    }

    void loadScene() override 
    {
        name = "Animated";
        isAnimated = true;

        // lights
        add(new Light(glm::vec3(-10,30,0), Color(1,0.5,0.5,1)));
        add(new Light(glm::vec3(+10,30,0), Color(0.5,1,0.5,1)));
        add(new Light(glm::vec3(0,30,0), Color(0.5,0.5,1,1)));
            
        Plane* plane = new Plane(glm::vec3(0, -20, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        add(plane); 
        
        /*
        Sphere* sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -50.0), 15.0);
        scene->add(sphere1); 
        */
            
        box = new Cube(glm::vec3(0,0,-50), glm::vec3(10,10,10));    

        //SceneObject* box = new Sphere(glm::vec3(0,0,-50), 15);    
        
        /*
        SceneObject* box = new Plane(
                glm::vec3(-20,-20,0), 
                glm::vec3(+20,-20,0), 
                glm::vec3(+20,+20,0),
                glm::vec3(-20,+20,0));
        */  

        add(box);    

        plane->material = Material::Checkerboard(1.0f);
        
        // origin marker
        add(new Sphere(glm::vec3(0,-20,0),3.0f));
    }
};

// --------------------------------------------------------------------
// Cornell Box
// --------------------------------------------------------------------
// This is a standard cornell box (https://en.wikipedia.org/wiki/Cornell_box) 
// with some additional objects to demonstrate the raytracers capabilities and
//  meet assignment requirements. 
// --------------------------------------------------------------------
class CornellBoxScene : public Scene
{        

public:
    
    void loadScene() override 
    {
        name = "Cornell";

        // lights
        add(new Light(glm::vec3(0,30,0)));

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
        pictureFrame->material = woodMaterial;
        
        Material* pictureMaterial = new Material();
        pictureMaterial->diffuseTexture = new MandelbrotTexture();
        // todo: might be nice to add some ruff normal texture (such as canvas) to this.    
        picture->material = pictureMaterial;

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
        add(petastool);
        add(object);
        
        // 2> transparient:
        pos = glm::vec3(-10,-40,-50);
        petastool = new Cube(pos, glm::vec3(10,10,10));
        object = new Sphere(pos + glm::vec3(0,10,0), 5.0f);
        object->material = Material::Default(Color(0.1,0.5,0.1,0.1));
        add(petastool);
        add(object);

        // 3> textured:
        pos = glm::vec3(+10,-40,-50);
        petastool = new Cube(pos, glm::vec3(10,10,10));
        object = new Sphere(pos + glm::vec3(0,10,0), 5.0f);
        object->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
        object->material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);
        add(petastool);
        add(object);    

        // 4> standard:
        pos = glm::vec3(+30,-40,-50);
        petastool = new Cube(pos, glm::vec3(10,10,10));
        object = new Sphere(pos + glm::vec3(0,10,0), 5.0f);
        object->material = Material::Default(Color(0.1f,0.1f,0.6f,1.0f));
        add(petastool);
        add(object);
            
        add(leftPlane);
        add(rightPlane);
        add(backPlane);
        add(floorPlane);
        add(ceilingPlane);
        add(forePlane);

        add(pictureFrame);
        add(picture);

        add(sphere);        
    }
};