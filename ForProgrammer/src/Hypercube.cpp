#include "Hypercube.h"
#include <cmath>

//Static vertices
const std::array<Vec4, 16> Hypercube::baseVertices4D = { {
    {-1,-1,-1,-1}, { 1,-1,-1,-1}, {-1, 1,-1,-1}, { 1, 1,-1,-1},
    {-1,-1, 1,-1}, { 1,-1, 1,-1}, {-1, 1, 1,-1}, { 1, 1, 1,-1},
    {-1,-1,-1, 1}, { 1,-1,-1, 1}, {-1, 1,-1, 1}, { 1, 1,-1, 1},
    {-1,-1, 1, 1}, { 1,-1, 1, 1}, {-1, 1, 1, 1}, { 1, 1, 1, 1}
} };

//Connecting the vertices
const std::vector<std::pair<int, int>> Hypercube::edges = {
    {0,1}, {0,2}, {0,4}, {0,8}, {1,3}, {1,5}, {1,9}, {2,3},
    {2,6}, {2,10}, {3,7}, {3,11}, {4,5}, {4,6}, {4,12}, {5,7},
    {5,13}, {6,7}, {6,14}, {7,15}, {8,9}, {8,10}, {8,12}, {9,11},
    {9,13}, {10,11}, {10,14}, {11,15}, {12,13}, {12,14}, {13,15}, {14,15}
};

//Constructor
Hypercube::Hypercube() {
    projectedVertices.fill({ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f });
    angles.fill(0.0f);
    activeRotations.fill(false);
    activeRotations[2] = true; // XW
    activeRotations[4] = true; // YW
}

//Update rotations
void Hypercube::updateRotation(float deltaTime) {
    for (int i = 0; i < 6; ++i) {
        if (activeRotations[i]) {
            angles[i] += 0.5f * deltaTime;
        }
    }
}

void Hypercube::toggleRotation(int planeIndex) {
    if (planeIndex >= 0 && planeIndex < 6) {
        activeRotations[planeIndex] = !activeRotations[planeIndex];
    }
}

std::string Hypercube::getActivePlanesString() const {
    std::string result = "Active: ";
    const char* names[] = { "XY", "XZ", "XW", "YZ", "YW", "ZW" };
    bool first = true;
    for (int i = 0; i < 6; ++i) {
        if (activeRotations[i]) {
            if (!first) result += ", ";
            result += names[i];
            first = false;
        }
    }
    if (first) result += "None";
    return result;
}

//Rotate in 4D then Project to 3D
void Hypercube::projectTo3D(float wCameraDistance) {

    std::array<float, 6> c, s;
    for (int i = 0; i < 6; ++i) {
        c[i] = std::cos(angles[i]);
        s[i] = std::sin(angles[i]);
    }

    for (int i = 0; i < 16; ++i) {
        Vec4 v = baseVertices4D[i];
        //Plane rotations
        //XY
        float x1 = v.x * c[0] - v.y * s[0];
        float y1 = v.x * s[0] + v.y * c[0];
        v.x = x1; v.y = y1;

        //XZ
        float x2 = v.x * c[1] - v.z * s[1];
        float z2 = v.x * s[1] + v.z * c[1];
        v.x = x2; v.z = z2;

        //XW
        float x3 = v.x * c[2] - v.w * s[2];
        float w3 = v.x * s[2] + v.w * c[2];
        v.x = x3; v.w = w3;

        //YZ
        float y4 = v.y * c[3] - v.z * s[3];
        float z4 = v.y * s[3] + v.z * c[3];
        v.y = y4; v.z = z4;

        //YW
        float y5 = v.y * c[4] - v.w * s[4];
        float w5 = v.y * s[4] + v.w * c[4];
        v.y = y5; v.w = w5;

        //ZW
        float z6 = v.z * c[5] - v.w * s[5];
        float w6 = v.z * s[5] + v.w * c[5];
        v.z = z6; v.w = w6;

        //Vertex coloring: 0-7 Blue, 8-15 Red
        if (i < 8) {
            projectedVertices[i].r = 0.0f;
            projectedVertices[i].g = 0.0f;
            projectedVertices[i].b = 1.0f;
        } else {
            projectedVertices[i].r = 1.0f;
            projectedVertices[i].g = 0.0f;
            projectedVertices[i].b = 0.0f;
        }

        //Project 4D to 3D
        float wDistance = wCameraDistance - v.w;
        if (wDistance < 0.1f) wDistance = 0.1f;

        float projectionScale = 1.5f / wDistance;

        projectedVertices[i].x = v.x * projectionScale;
        projectedVertices[i].y = v.y * projectionScale;
        projectedVertices[i].z = v.z * projectionScale;
    }
}

const std::array<Vertex3D, 16>& Hypercube::getProjectedVertices() const {
    return projectedVertices;
}

const std::vector<std::pair<int, int>>& Hypercube::getEdges() const {
    return edges;
}