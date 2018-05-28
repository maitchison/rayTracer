/**
 * Library to read PLY format.
 * 
 * (not tested very well...)
 */


#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

/** Returns if s starts with prefix or not (case sensitive) */
bool startsWith(string s, string prefix) {
    return (s.substr(0, prefix.size()) == prefix);    
}

/** Load in a ply file, return vertices, where each triangle has 3 vertices. 
 * Returns NULL if there was an error. */
vector<glm::vec3>* ReadPLY(const char* filename, float scale = 1.0f) {

    string line;    
    ifstream file(filename);

    int vertexCount;
    int faceCount;

    vector<glm::vec3>* verticesOut = new vector<glm::vec3>();
    vector<glm::vec3>* vertices = new vector<glm::vec3>();
    
    if (file.is_open()) {
        
        string s1;
        string s2;            
        
        // read in header (and ignore)
        while (getline(file, line)) {
            if (startsWith(line, "element vertex")) {
                stringstream ss;            
                ss<<line;
                ss>>s1>>s2>>vertexCount;                
            }
            if (startsWith(line, "element face")) {
                stringstream ss;            
                ss<<line;
                ss>>s1>>s2>>faceCount;                
            }
            if (line == "end_header") break;            
        }

        printf("File has %d vertices %d faces\n", vertexCount, faceCount);            

        for (int v = 0; v < vertexCount; v++) {
            getline(file, line);
            glm::vec3 p;
            stringstream ss;                    
            ss<<line;
            ss>>p.x>>p.y>>p.z;
            vertices->push_back(p);
        }

        for (int f = 0; f < faceCount; f++) {
            getline(file, line);
            int degree, a, b, c;
            stringstream ss;                    
            ss<<line;
            ss>>degree>>a>>b>>c;
            if (degree != 3) {
                cout << "Error, mesh must be triangle based but found face with " << degree << " vertices on line: \n" << line;
                file.close();
                return NULL;
            }            
            // we just add the vertices together, no need to seperate out faces at the moment.            
            verticesOut->push_back((*vertices)[a]*scale);
            verticesOut->push_back((*vertices)[b]*scale);
            verticesOut->push_back((*vertices)[c]*scale);            
        }
        file.close();
        return verticesOut;
    } else {
        printf("Error, can not read PLY file %s", filename);
        return NULL;
    }

}