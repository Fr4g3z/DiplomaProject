#pragma once

#include <array>
#include <vector>
#include <string>

struct Vertex3D { float x, y, z; 
                  float r, g, b;
                };
struct Vec4 { float x, y, z, w; };

class Hypercube {
public:
    Hypercube();
    void updateRotation(float deltaTime);
    void projectTo3D(float wCameraDistance);

    //Return the new struct
    const std::array<Vertex3D, 16>& getProjectedVertices() const;
    const std::vector<std::pair<int, int>>& getEdges() const;

    void toggleRotation(int planeIndex); //XY, XZ, XW, YZ, YW, ZW
    std::string getActivePlanesString() const;

private:
    static const std::array<Vec4, 16> baseVertices4D;
    static const std::vector<std::pair<int, int>> edges;

    std::array<Vertex3D, 16> projectedVertices;

    std::array<float, 6> angles;  //XY, XZ, XW, YZ, YW, ZW
    std::array<bool, 6> activeRotations;
};