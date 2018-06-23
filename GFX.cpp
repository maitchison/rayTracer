/*
Simple Graphics Library
Author Matthew Aitchison
Date Feb 2018
*/

#include "GFX.h"

/** Returns if (x,y) is in screen bounds or not. */
bool inBounds(int x, int y) {
	return ((x >= 0) && (y >= 0) && (x < SCREEN_WIDTH) && (y < SCREEN_HEIGHT));
}

/** Place a pixel on the frame buffer.
 * If shallow is true then only the color buffer is updated, not the sample buffer.
 * This allows writing to the screen without destroying the sample buffer.
 */
void GFX::putPixel(int x, int y, Color col, bool shallow)
{
    if (!inBounds(x,y)) return;
    if (shallow) {
        buffer[y*SCREEN_WIDTH + x] = colorToInt24(col);
    } else {
	    sampleBuffer[y*SCREEN_WIDTH+x] = Color(0,0,0,0);
        addSample(x,y,col);
    }
}


/** Sets buffer to sample buffer. */
void GFX::updateBuffer()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
            {
                buffer[y*SCREEN_WIDTH + x] = colorToInt24(sampleBuffer[y*SCREEN_WIDTH + x] / sampleBuffer[y*SCREEN_WIDTH + x].a);
            }
        }
    }
}

/** Adds a simple to the buffer, samples are averaged by weight. */
void GFX::addSample(int x, int y, Color col, float weight)
{
	if (!inBounds(x,y) || weight == 0.0f) return;
	col.a = 1.0f;
    sampleBuffer[y*SCREEN_WIDTH + x] += (col*weight);
	buffer[y*SCREEN_WIDTH + x] = colorToInt24(sampleBuffer[y*SCREEN_WIDTH + x] / sampleBuffer[y*SCREEN_WIDTH + x].a);
}

void GFX::clear(Color col, bool shallow)
{
    col.a = 0.0;
	uint32_t c = colorToInt24(col);
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (!shallow)
			    buffer[y*SCREEN_WIDTH + x] = c;
            sampleBuffer[y*SCREEN_WIDTH + x] = col;
		}
	}
}

/** Blit buffer to screen */
void GFX::blit()
{
	//upload to GPU texture (slow, shoud use glTextSubImage2D
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	//match projection to window resolution (could be in reshape callback)
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//clear and draw quad with texture (could be in display callback)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, tex);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(0, 0);
	glTexCoord2i(0, 1); glVertex2i(0, SCREEN_HEIGHT);
	glTexCoord2i(1, 1); glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
	glTexCoord2i(1, 0); glVertex2i(SCREEN_WIDTH, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush(); //don't need this with GLUT_DOUBLE and glutSwapBuffers

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void GFX::init(void)
{
	// init texture		
	clear();
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

}

// singleton GFX access;
GFX gfx = GFX();