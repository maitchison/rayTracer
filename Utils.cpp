#include "Utils.h"

namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v) 
    {
        return dot(v, v);
    }
    
}

/** Loads a file from disk into at std::vector */
void loadFile(std::vector<unsigned char>& buffer, const std::string& filename) 
{
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

	//get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	//read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else {
        std::cout << "Warning: " << filename << "not found";
        buffer.clear();
    } 
}

/** Returns x cliped to between a and b. */
float clipf(float x, float a, float b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

/** Returns x cliped to between a and b. */
int clipi(int x, int a, int b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

float randf() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

/** Convert color to 24bit form. */
int colorToInt24(Color color)
{
    int _r = clipi(int(color.r * 255), 0, 255);
    int _g = clipi(int(color.g * 255), 0, 255);
    int _b = clipi(int(color.b * 255), 0, 255);
    return (_r)+(_g << 8) + (_b << 16);
}
