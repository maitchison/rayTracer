/**
 * Handles 2d bitmapped textures. 
 **/

#pragma once

#include "Tools.h"
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>

#include "picoPNG.h"

enum TextureClip {TC_WRAP, TC_CLAMP};
enum TextureSample {TS_NEAREST, TS_BILINEAR};

/** Base class for 2d texture.  Defaults to returning a single color */
class Texture2D
{
public:
    Texture2D() {};
    
    virtual glm::vec4 sample(float u, float v) { return glm::vec4(1,0,1,1); };
};

/** A bitmaped texture. */
class BitmapTexture : public Texture2D
{
public:
    TextureClip clipping = TC_WRAP;
    TextureSample sampling = TS_NEAREST;

    unsigned long width, height;
    std::vector<unsigned char> buffer, image;

    /** Load texture from filename.  Filename should be a valid PNG file. */
    BitmapTexture(const char* filename) : Texture2D() {
        // modified from https://stackoverflow.com/questions/12084889/how-can-i-use-a-texture-jpg-image-for-an-opengl-background-window-in-mac        
        loadFile(buffer, filename);        
        int error = decodePNG(image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size());
        if (error != 0) std::cout << "Error: " << error << std::endl;
    }
    
    /** Samples texture given u and v between 0, 1. */
    glm::vec4 sample(float u, float v) override {

        // handle uv clipping
        switch (clipping) {
            case TC_WRAP:
                u = u - (int)u;
                v = v - (int)v;
                break;
            case TC_CLAMP:
                u = clip(u, 0, 1);
                v = clip(v, 0, 1);
                break;
        }

        // sample texture (nearest only for the moment...)
        // note uv is [0,1] so x is [0,width-1] 
        int x = (int)(u * (width-1));
        int y = (int)(v * (height-1));

        int base = (x + y * width) * 4;        
        return glm::vec4(image[base+0]/255.0f, image[base+1]/255.0f, image[base+2]/255.0f, image[base+3]/255.0f);
        
    }
};

/** Procedual checkerboard texture */
class CheckerboardTexture : public Texture2D
{
private:
    float scale;    
    glm::vec4 color1, color2;
public:

    CheckerboardTexture(glm::vec4 color1 = glm::vec4(1,1,1,1), glm::vec4 color2 = glm::vec4(0,0,0,1)) : Texture2D() {      
        this->color1 = color1;
        this->color2 = color2;
        this->scale = 32.0f;
    }

    /** Returns color at given uv co-ords. */
    glm::vec4 sample(float u, float v) override { 			
    	int x = (u > 0) ? int(u * scale) : int(-u * scale + 1);
		int y = (v > 0) ? int(v * scale) : int(-v * scale + 1);
		bool cell = (x+y) & 1;        
        return cell ? color2 : color1;
    }    
};