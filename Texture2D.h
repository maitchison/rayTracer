/**
 * Handles 2d bitmapped textures. 
 **/

#pragma once

#include "Utils.h"
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

    TextureClip clipping = TC_WRAP;
    TextureSample sampling = TS_NEAREST;

    // if this texture represents a normal map or not.
    bool isNormalMap = false;


    Texture2D() {};
    
    virtual glm::vec4 sample(glm::vec2 uv) { return glm::vec4(1,0,1,1); };

    /** Samples the texture as a normal map.  If the texture as not been marked as a normal map it will be treated as a bump map. */
    glm::vec3 sampleNormalMap(glm::vec2 uv) {        
        if (isNormalMap) {
            return glm::vec3(sample(uv));
        } else {
            // NIY: sample as bump map...
            return glm::vec3(0,0,1);
        }        
    }

    /** Applies clipping. */ 
    glm::vec2 clip(glm::vec2 uv) {
        float u = uv.x;
        float v = uv.y;
        switch (clipping) {
            case TC_WRAP:
                u = u - (int)u;
                v = v - (int)v;
                break;
            case TC_CLAMP:
                u = clipf(u, 0, 1);
                v = clipf(v, 0, 1);
                break;
        }
        return glm::vec2(u,v);
    }

};

/** A bitmaped texture. */
class BitmapTexture : public Texture2D
{
public:
        
    unsigned long width, height;
    std::vector<unsigned char> buffer, image;

    /** Load texture from filename.  Filename should be a valid PNG file.
     * @param filename The filename to load.
     * @param isNormalMap If true texture will be treated as a normal map.
     */
    BitmapTexture(const char* filename, bool isNormalMap = false) : Texture2D() {
        this->isNormalMap = isNormalMap;
        loadFile(buffer, filename);        
        int error = decodePNG(image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size());
        if (error != 0) std::cout << "Error: " << error << std::endl;
    }
        
    /** Samples texture given u and v between 0, 1. */
    glm::vec4 sample(glm::vec2 uv) override {

        uv = clip(uv);
        
        // sample texture (nearest only for the moment...)
        // note uv is [0,1] so x is [0,width-1] 
        int x = (int)(uv.x * (width-1));
        int y = (int)(uv.y * (height-1));

        int base = (x + y * width) * 4;        
        return glm::vec4(image[base+0]/255.0f, image[base+1]/255.0f, image[base+2]/255.0f, image[base+3]/255.0f);
        
    }
};

/** Procedual checkerboard texture */
class CheckerboardTexture : public Texture2D
{
private:
    glm::vec4 color1, color2;
public:
    float scale;    
    
    CheckerboardTexture(float scale = 32.0f, glm::vec4 color1 = glm::vec4(1,1,1,1), glm::vec4 color2 = glm::vec4(0,0,0,1)) : Texture2D() {      
        this->color1 = color1;
        this->color2 = color2;
        this->scale = scale;
    }

    /** Returns color at given uv co-ords. */
    glm::vec4 sample(glm::vec2 uv) override { 			
        float u = uv.x;
        float v = uv.y;
    	int x = (u > 0) ? int(u * scale) : int(-u * scale + 1);
		int y = (v > 0) ? int(v * scale) : int(-v * scale + 1);
		bool cell = (x+y) & 1;        
        return cell ? color2 : color1;
    }    
};

/** Procedual mandelbrot set texture */
class MandelbrotTexture : public Texture2D
{
private:
    float scale;    
    const int MAX_ITTERATIONS = 512;
    glm::vec4 color1, color2;
public:

    MandelbrotTexture() : Texture2D() {              
    }

    /** Returns color at given uv co-ords. */
    glm::vec4 sample(glm::vec2 uv) override { 			
        float u = uv.x;
        float v = uv.y;

        float cx = (u-0.75f)*3.0f;
        float cy = (v-0.5f)*2.0f;
        
        float x = 0;
        float y = 0;

        int iterations = 0;

        for (int i = 0; i < MAX_ITTERATIONS; i++) {
            float norm = x*x + y*y;
            if (norm >= 4.0f) break;
            float _x = x*x - y*y + cx;
            y = 2 * x * y + cy;
            x = _x;         
            iterations++;   
        }

    // simple color mapping.
    if (iterations == MAX_ITTERATIONS) 
        return glm::vec4(0,0,0,1);
    else 
        return glm::vec4((float)iterations/MAX_ITTERATIONS, (float)((iterations*4) % MAX_ITTERATIONS)/MAX_ITTERATIONS, 0.5f,1.0f);
    
    }    	    
};