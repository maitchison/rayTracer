/**
 * Scene class.
 **/

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "ContainerObject.h"

//* Scene containing lights and objects. */
class Scene : public ContainerObject
{    

protected:
    bool _isLoaded = false;
    
public:

    std::string name = "Scene";

    bool isAnimated = false;

    bool isLoaded() { return _isLoaded; };

    /** List of lights in the scene. */
    std::vector<Light*> lights = std::vector<Light*>();

    // our camera
    Camera* camera;

    Scene() : ContainerObject() {        
        camera = new Camera();                        
    }

    void add(SceneObject* object) override {
        // override so that if we add a light it gets added as a light instead of as a normal object.
        if (dynamic_cast<Light*>(object)) {
            lights.push_back((Light*)object);            
        } else {
            ContainerObject::add(object);
        }   
    } 

    // loads the scene.
    void load() {
        printf("<loading scene>\n");
        loadScene();
        _isLoaded = true;
    }

    // descendants to overwrite.
    virtual void loadScene() {};

    // updates the scene.
    virtual void update() {};        

};