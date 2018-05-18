echo "-----------------------------------------------------------------------"


g++ -std=c++11 -c Ray.cpp
g++ -std=c++11 -c SceneObject.cpp
g++ -std=c++11 -c Sphere.cpp
g++ -std=c++11 -c Plane.cpp

g++ -std=c++11 -w -o RayTracer.exe RayTracer.cpp Ray.o SceneObject.o Sphere.o Plane.o -lGL -lGLU -lglut && ./RayTracer.exe
