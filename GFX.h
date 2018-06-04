/**
 * Very simple graphics library for blitting a buffer to the screen. 
 **/

#pragma once

#include "Color.h"
#include "Utils.h"
#include <stdint.h>
#include "TGAWriter.h"

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
	#include <GL/gl.h>
#endif


const int SCREEN_WIDTH = 2560 / 4;
const int SCREEN_HEIGHT = 1440 / 4;

class GFX
{
	
	uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    // stores acculated color, where the total weight is recorded in the alpha chanel.
    // for example the color 10,5,1,10 has a color of (1.0, 0.5, 0.1) and 10 samples.
    Color sampleBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

	GLuint tex;

public:
	void putPixel(int x, int y, Color col, bool shallow=false);    
    void addSample(int x, int y, Color col, float weight = 1.0);
	void clear(Color col = Color(0,0,0,1), bool shallow=false);
    void updateBuffer();
    void screenshot(std::string filename) {
        const char *cstr = filename.c_str();
        updateBuffer();
        printf("Saving screenshot %s\n", cstr);
        write_truecolor_tga(cstr, *buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
	void blit();    
	void init(void);
};

// singleton access to graphics library.
extern GFX gfx;
