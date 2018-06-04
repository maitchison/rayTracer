#pragma once

/**
 * Writes TGA files to disk.
 * Taken from http://www.cplusplus.com/forum/general/6194/
 */

#include <fstream>
#include <string>
#include <stdint.h>

using namespace std;

// It is presumed that the image is stored in memory as 
//   RGBA_t data[ height ][ width ]
// where lines are top to bottom and columns are left to right
// (the same way you view the image on the display)

// The routine makes all the appropriate adjustments to match the TGA format specification.

bool write_truecolor_tga( const string& filename, uint32_t* data, unsigned width, unsigned height );