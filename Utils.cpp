#include "Utils.h"

namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v) 
    {
        return dot(v, v);
    }

    /** Returns squared length of vector */
    float length2(glm::vec4 v) 
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

float frac(float f)
{
    return f - int(f);
}

/** Convert color to 24bit form. */
int colorToInt24(Color color)
{
    int _r = clipi(int(color.r * 255), 0, 255);
    int _g = clipi(int(color.g * 255), 0, 255);
    int _b = clipi(int(color.b * 255), 0, 255);
    return (_r)+(_g << 8) + (_b << 16);
}

glm::vec3 defocus(glm::vec3 v, float r)
{    
    v = glm::normalize(v);    
    float theta = randf()*2*PI;
    float phi = randf()*r;

    glm::mat4x4 rotationMatrix = glm::mat4x4(1);

    // any orthogonal vector will do.
    glm::vec3 orthogonalVector = (v.x==v.y==0) ? glm::vec3(1,0,0) : glm::normalize(glm::vec3(-v.y, v.x,0));
    
    rotationMatrix = glm::rotate(rotationMatrix, theta, v);
    rotationMatrix = glm::rotate(rotationMatrix, phi, orthogonalVector);
    
    return glm::vec3(rotationMatrix * glm::vec4(v, 0));
}

glm::vec3 distort(glm::vec3 v, float d)
{        
    return glm::normalize(v + glm::vec3(randf()*d, randf()*d, randf()*d));    
}


glm::mat4x4 EulerRotationMatrix(glm::vec3 rotation)
{
    glm::mat4x4 rotationMatrix = glm::mat4x4();
    rotationMatrix = glm::rotate(rotationMatrix, rotation.x, glm::vec3(1,0,0));
    rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0,1,0));
    rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0,0,1));
    return rotationMatrix;
}   

//faster version
float raySphereIntersection(const glm::vec3* rayPos, const glm::vec3* rayDir, float radius)
{
	float b = glm::dot(*rayDir, *rayPos);
	float len2 = glm::dot(*rayPos, *rayPos);
	float c = len2 - radius * radius;
	float delta = b * b - c;

	if (delta < EPSILON) return 0;

	float sqrt_delta = sqrt(delta);
	float t1 = -b - sqrt_delta;
	float t2 = -b + sqrt_delta;

	float t = -1;

	if ((t1 >= 0) && ((t1 <= t2) || (t2 < 0))) t = t1;
	if ((t2 >= 0) && ((t2 <= t1) || (t1 < 0))) t = t2;

	return t < 0 ? 0 : t;
}

float raySphereIntersection(glm::vec3 rayPos, glm::vec3 rayDir, glm::vec3 sphereLocation, float radius)
{    
    glm::vec3 vdif = rayPos - sphereLocation;
    float b = glm::dot(rayDir, vdif);
    float len2 = glm::dot(vdif, vdif);
    float c = len2 - radius*radius;
    float delta = b*b - c;
   
	if (delta < EPSILON) return 0;

    float sqrt_delta = sqrt(delta); 
    float t1 = -b - sqrt_delta;
    float t2 = -b + sqrt_delta;
    
    float t = -1;
    
    if ((t1 >= 0) && ((t1 <= t2) || (t2 < 0))) t = t1;
    if ((t2 >= 0) && ((t2 <= t1) || (t1 < 0))) t = t2;    

    return t < 0 ? 0 : t;    
}

void print(glm::mat4x4 m)
{
    for (int i = 0; i < 4; i ++) {
        printf("%f %f %f %f\n", (float)m[0][i], (float)m[1][i], (float)m[2][i], (float)m[3][i]);
    }
}

void print(glm::vec3 v)
{
    printf("(%f %f %f)\n", v.x, v.y, v.z);    
}

void print(glm::vec4 v)
{
    printf("(%f %f %f %f)\n", v.x, v.y, v.z, v.w);    
}

float maxf(float a, float b)
{
    return a > b ? a : b;
}