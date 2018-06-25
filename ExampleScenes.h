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
    switch (col % 5) {
        case 0: material->diffuseColor = Color(0.9f,0.1f,0.2f,1); break;
        case 1: material->diffuseColor = Color(0.1f,0.9f,0.2f,1); break;
        case 2: material->diffuseColor = Color(0.3f,0.4f,0.9f,1); break;
        case 3: material->diffuseColor = Color(0.5f,0.5f,0.5f,1); break;
        case 4: material->diffuseColor = Color(1.0f,1.0f,1.0f,1); break;            
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
// GI
// --------------------------------------------------------------------
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
// Basic
// Simple scene to meet basic requirements of the assignment.
// --------------------------------------------------------------------
class BasicScene : public Scene
{    
public:

    void loadScene() override {

        name = "Basic";

        add(new Light(glm::vec3(-10,30,0), Color(1.0f,1.0f,1.0f,1)));
        
        Sphere* sphere1 = new Sphere(glm::vec3(-10.0, -16.0, -25.0), 4.0);
        Sphere* sphere2 = new Sphere(glm::vec3(0.0, -5.0, -30.0), 10.0);
        Sphere* sphere3 = new Sphere(glm::vec3(+10.0, -16.0, -25.0), 4.0);

        sphere1->material->diffuseColor = Color(0.4f,0.4f,0.4f,1.0f);
        sphere1->material->reflectivity = 0.7f;
        sphere1->material->reflectionBlur = 0.3f;

        sphere2->material->diffuseColor = Color(0.4f,0.4f,0.4f,1.0f);
        sphere2->material->reflectivity = 0.7f;

        sphere3->material->diffuseColor = Color(1,1,1,0.03f);
        sphere3->material->refractionIndex = 1.1f;                

        Plane* plane = new Plane(glm::vec3(0, -20, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));                     
        plane->material = Material::Checkerboard(1.0f);
        
        add(plane); 		
        add(sphere1);     
        add(sphere2); 
        add(sphere3);     

        // extra objects
        Cube* cube = new Cube(glm::vec3(-10,-18,-15), glm::vec3(4,4,4));
        cube->setRotation(glm::vec3(0,2,0));
        cube->material->diffuseTexture = new BitmapTexture("./textures/Wood_plank_007_COLOR.png");
        cube->material->normalTexture = new BitmapTexture("./textures/Wood_plank_007_NORM.png", true);    
        cube->material->diffuseColor = Color(1.0f, 0.7f, 0.6f, 1.0f);
        add(cube);

        Cylinder* cylinder = new Cylinder(glm::vec3(+10,-20,-15), 3.0f, 3.0f);        
        cylinder->material->diffuseColor = Color(0.5,0.5,0.5,1.0);
        cylinder->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
        cylinder->material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);            
        add(cylinder);

        Sphere* sphere = new Sphere(glm::vec3(0,-16,-15), 4.0f);        
        sphere->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
        sphere->material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);            
        add(sphere);
        
        camera->setLocation(glm::vec3(0,-10,+1));
        camera->lightingModel = LM_DIRECT;    

        // white sky light
        camera->backgroundColor = Color(1,1,1,1) * 0.2f;	
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
        Light* light = new Light(glm::vec3(-10,30,0), 0.8f * Color(1,1,1,1));
        add(light);  
        
        // ground plane
        Plane* plane = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        plane->material->diffuseColor = Color(0.3f,0.3f,0.3f,1.0f);
        //plane->material->diffuseTexture = new BitmapTexture("./textures/Rough_rock_015_COLOR.png");
        //plane->material->normalTexture = new BitmapTexture("./textures/Rough_rock_015_NRM.png", true);    
        
        //plane->material->diffuseTexture = new CheckerboardTexture(4.0f, Color(0.5f,0.5f,0.5f,1), Color(0.1,0.1,0.1,1));
        plane->material->reflectivity = 0.25f;
        add(plane);     
            
        const int NUM_OBJECTS_X = 20;
        const int NUM_OBJECTS_Y = 10;
        
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
        camera->setLocation(glm::vec3(0,4.7,10.3));
        camera->setRotation(glm::vec3(-0.6,0,0));
        
        // blue sky light
        camera->backgroundColor = Color(0.03,0.05,0.15,1) * 1.0f;
    }
};

// --------------------------------------------------------------------
// Million Cubes
// --------------------------------------------------------------------
// This scene contains 1,000,000 cubes, and demonstrates the auto clustering
// algorithm for large numbers of scene objects. 
// --------------------------------------------------------------------
class MillionCubes : public Scene
{    
public:

    void loadScene() override {
    
        name = "MillionCubes";

        // default light
        add(new Light(glm::vec3(-10,30,0), 0.5f*Color(1,1,1,1)));    
        
        // ground plane
        Plane* plane = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        add(plane); 

        
        const int NUM_OBJECTS_X = 10;
        const int NUM_OBJECTS_Y = 10;
        const int NUM_OBJECTS_Z = 10;
        
        // generate some cubes
        for (int i = 0; i < NUM_OBJECTS_X; i++) {
            for (int j = 0; j < NUM_OBJECTS_Y; j++) {
                for (int k = 0; k < NUM_OBJECTS_Z; k++) {
                    Cube* cube = new Cube(glm::vec3(i-(NUM_OBJECTS_X/2),j+2,k+5), glm::vec3(0.5f,0.5f,0.5f));                    
                    cube->setRotation(glm::vec3(randf(),randf(), randf()));
                    add(cube);
                }
            }    
        }

        // some light sources for GI
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                Cylinder* lightSphere = new Cylinder(glm::vec3(((i*2)-1)*8,0,((j*2)-1)*8),1.1,3);
                lightSphere->material = parameterisedMaterial(4+i+j*2, 2);
                lightSphere->material->diffuseColor *= 5.0; // make them bright :)
                add(lightSphere);
            }    
        }

        // a bit experimental, but try the auto clustering algorithm
        this->cluster(true);

        camera->lightingModel = LM_GI; // gi looks way better, but is slow :(

        camera->setLocation(glm::vec3(0,4.08,18.21));
        camera->setRotation(glm::vec3(0,0,0));

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
        add(new Light(glm::vec3(0,5.26,14.12), 0.5f*Color(1,1,1,1)));    
        
        // ground plane
        Plane* plane = new Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0,0,1));        
        //plane->material->diffuseTexture = new BitmapTexture("./textures/Wood_plank_007_COLOR.png");
        //plane->material->normalTexture = new BitmapTexture("./textures/Wood_plank_007_NORM.png", true);    
        //plane->material->diffuseTexture = new CheckerboardTexture(2.0f);
		plane->material = Material::Reflective(Color(0.7, 0.8, 0.9, 1), 0.0);

        add(plane); 

        // mesh objects...
        vector<glm::vec3>* dragonMesh = ReadPLY("./dragon.ply", 10.0f);

        // base dragon
        Mesh* dragon = new Mesh(glm::vec3(0,0,0), dragonMesh);    
        //Sphere* dragon = new Sphere(glm::vec3(0,1.0,0),0.5); 
        //Cube* dragon = new Cube(glm::vec3(0,1,0),glm::vec3(0.5)); 

        const int NUM_OBJECTS_X = 11;
        const int NUM_OBJECTS_Y = 11;
        
        // duplicate dragons
		ContainerObject* dragons = new ContainerObject();
        for (int i = 0; i < NUM_OBJECTS_X; i++) {
            for (int j = 0; j < NUM_OBJECTS_Y; j++) {
                // rather than creating new dragons (which would repartition them and load the mesh into memory again)
                // we can instead create copies of them with the ReferenceObject type.  This allows for a copy of 
                // the origional object, but with a new transform applied.
                if (i-(NUM_OBJECTS_X/2) == 2 || (i-(NUM_OBJECTS_X/2) == -2)) {
                    // don't block the light                
                    continue;
                }
                ReferenceObject* dragonCopy = new ReferenceObject(glm::vec3((i-(NUM_OBJECTS_X/2))*1.2,-0.5,(j-(NUM_OBJECTS_Y/2))*2), dragon);
                dragonCopy->setRotation(glm::vec3(0, (randf()-0.5f)*PI*0.3f + (0.4f*PI), 0));
                //dragonCopy->material = parameterisedMaterial(i+(j*3), 0);                
                dragons->add(dragonCopy); 
            }    
        }
	
		// a bit experimental, but try the auto clustering algorithm
		// ~2x speed, but broken at the momement.
		// dragons->cluster();

		add(dragons);
		
		/*
        // back mirror
        Cube* backPlane = new Cube(glm::vec3(0,0,-15), glm::vec3(21,15,2));		
        add(backPlane);
        Cube* leftPlane = new Cube(glm::vec3(-10,0,0), glm::vec3(2,15,30));
        add(leftPlane);
        Cube* rightPlane = new Cube(glm::vec3(+10,0,0), glm::vec3(2,15,30));
        add(rightPlane);     


		Cube* topPlane = new Cube(glm::vec3(+10, 0, 0), glm::vec3(2, 15, 30));
		add(rightPlane);

		*/

		//Cube* mirrorBox = new Cube(glm::vec3(0, 0, 0), glm::vec3(-60, -600, -60)); // negatives reverse the normal direction.
		//mirrorBox->castsShadows = false;
		//add(mirrorBox);

		Cube* lightBox = new Cube(glm::vec3(0, 10, 0), glm::vec3(5, 1, 5));
		lightBox->material = Material::Emissive(Color(1, 1, 1, 1)*0.5f);
		lightBox->castsShadows = false;
		add(lightBox);

		Cube* lightCase = new Cube(glm::vec3(0, 10, 0), glm::vec3(5.5, 0.8, 5.5));
		lightCase->material = Material::Default(Color(0.3, 0.3, 0.3, 1));
		add(lightCase);

		//Material* mirrorMaterial = Material::Reflective(Color(0.3f, 0.3f, 0.5f, 1), 0.2f);
		//mirrorMaterial->reflectionBlur = 0.02f;
		//mirrorBox->material = mirrorMaterial;

        // some light sources for GI
        Cube* light1 = new Cube(glm::vec3(-2.4,0,0), glm::vec3(0.6,0.5,20));
        light1->material = parameterisedMaterial(5, 2);        
        light1->setRotation(glm::vec3(0,0,PI/4));
        add(light1);
        Cube* light2 = new Cube(glm::vec3(+2.4,0,0), glm::vec3(0.6,0.5,20));
        light2->material = parameterisedMaterial(6, 2);        
        light2->setRotation(glm::vec3(0,0,PI/4));
        add(light2);
		light1->material->emisiveColor *= 2.7f;
		light2->material->emisiveColor *= 2.7f;
        
        camera->lightingModel = LM_GI; // gi looks way better, but is slow :(

        //camera->setLocation(glm::vec3(0,7.6,16.3));
        //camera->setLocation(glm::vec3(0,1.56,9.36));
        //camera->setLocation(glm::vec3(0,2.65,15.58));
        camera->setLocation(glm::vec3(0,5.26,14.12));
        camera->setRotation(glm::vec3(-0.5,0,0));
        camera->move(-2,0,0);

        // blue sky light
        camera->backgroundColor = Color(0.03,0.05,0.10,1) * 0.1f;
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
        camera->backgroundColor = Color(0.03,0.05,0.15,1) * 0.26f;

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

        // light
        add(new Light(glm::vec3(-10,30,0), Color(1,1,1,1)));
        
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
		/*
        add(sphere1);     
        add(sphere2); 
        add(sphere3);     
		*/

        // origin marker
		SceneObject* marker = new Sphere(glm::vec3(0, -20, 0), 3.0f);
		marker->material = Material::Emissive(Color(1, 0, 0, 1));
        add(marker);

		// simple lighting
		camera->lightingModel = LM_DIRECT;
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
        Cube* lightBox = new Cube(glm::vec3(0,40,-40), glm::vec3(20,10,20));
        lightBox->material->emisiveColor = Color(1,1,1,1) * 5.0f;
        lightBox->castsShadows = false;
        add(lightBox);        

        // infinite planes are a little faster than clipped planes and won't have potential artifacts at the edges.
        Plane* leftPlane = new Plane(glm::vec3(-40,0,0), glm::vec3(1,0,0));
        Plane* rightPlane = new Plane(glm::vec3(+40,0,0), glm::vec3(-1,0,0));
        Plane* backPlane = new Plane(glm::vec3(0,0,-80), glm::vec3(0,0,1));
        Plane* forePlane = new Plane(glm::vec3(0,0,20), glm::vec3(0,0,-1));
        Plane* floorPlane = new Plane(glm::vec3(0,40,0), glm::vec3(0,1,0));
        Plane* ceilingPlane = new Plane(glm::vec3(0,-40,0), glm::vec3(0,-1,0));

        // make the colors a little pastal.
        leftPlane->material = Material::Default(Color(0.9,0.1,0.1,1.0));
        rightPlane->material = Material::Default(Color(0.1,0.9,0.1,1.0));
        backPlane->material = forePlane->material = floorPlane->material = ceilingPlane->material = Material::Default(Color(0.9,0.9,0.9,1.0));

        // reflective blury sphere
        Sphere* sphere = new Sphere(glm::vec3(0,-20,-60), 10.0f);
        sphere->material = Material::Reflective(glm::vec4(0.1f,0.1f,0.1f,1.0f), 0.8f);

        // a framed mandelbrot picture in the background
        Cube* pictureFrame = new Cube(glm::vec3(0,0,-80), glm::vec3(50,50,10));    
        Cube* picture = new Cube(glm::vec3(0,0,-79), glm::vec3(45,45,10));

        Material* woodMaterial = new Material();
        woodMaterial->diffuseTexture = new BitmapTexture("./textures/Wood_plank_007_COLOR.png");
        woodMaterial->normalTexture = new BitmapTexture("./textures/Wood_plank_007_NORM.png", true);
        pictureFrame->material = woodMaterial;
        
        Material* pictureMaterial = new Material();
        picture->material->diffuseTexture = new MandelbrotTexture();        

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

        camera->lightingModel = LM_GI;
    }
};
