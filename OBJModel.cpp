#include "OBJModel.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>

OBJModel::OBJModel()
{
}

OBJModel::~OBJModel()
{
}

void OBJModel::LoadFromFile(const char* filename)
{
    std::vector<Position> vertices;
    std::vector<Normal> normals;

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    char currentMtlName[100] = "";

    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        if (StartWith(line, "mtllib")) {
            char mtlFileName[100];
            if (sscanf_s(line.c_str(), "mtllib %s", mtlFileName, sizeof(mtlFileName)) == 1) {
                LoadMaterialFile(mtlFileName);
            }
        }
        else if (StartWith(line, "v ")) {
            Position pos;
            if (sscanf_s(line.c_str(), "v %f %f %f", &pos.x, &pos.y, &pos.z) == 3) {
                vertices.push_back(pos);
            }
        }
        else if (StartWith(line, "vn ")) {
            Normal normal;
            if (sscanf_s(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z) == 3) {
                normals.push_back(normal);
            }
        }
        else if (StartWith(line, "usemtl")) {
            if (sscanf_s(line.c_str(), "usemtl %s", currentMtlName, sizeof(currentMtlName)) != 1) {
                currentMtlName[0] = '\0'; // Clear if parsing fails
            }
        }
        else if (StartWith(line, "f ")) {
            ParseFace(line, currentMtlName, vertices, normals);
        }
    }
	std::cout << "done" << std::endl;
}

std::vector<float> OBJModel::GetVertexData()
{
    return mVertexData;
}

int OBJModel::GetVertexCount()
{
    return static_cast<int>(mVertexData.size() / 9);
}

void OBJModel::LoadMaterialFile(const char* filename)
{
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open material file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string currentMaterial;

    while (std::getline(file, line)) {
        if (StartWith(line, "newmtl")) {
            char mtlName[100];
            if (sscanf_s(line.c_str(), "newmtl %s", mtlName, sizeof(mtlName)) == 1) {
                currentMaterial = mtlName;
                mMaterialsMap[currentMaterial] = Color();
            }
        }
        else if (StartWith(line, "Kd") && !currentMaterial.empty()) {
            Color& color = mMaterialsMap[currentMaterial];
            sscanf_s(line.c_str(), "Kd %f %f %f", &color.r, &color.g, &color.b);
        }
    }
}

bool OBJModel::StartWith(std::string& line, const char* text)
{
    return line.rfind(text, 0) == 0; // Returns true if 'text' is a prefix of 'line'
}

void OBJModel::ParseFace(const std::string& line, const char* currentMtlName,
    const std::vector<Position>& vertices,
    const std::vector<Normal>& normals)
{
    std::istringstream lineStream(line);
    std::string prefix;
    lineStream >> prefix; // Read the "f" prefix

    std::vector<std::string> faceVertices;
    std::string vertex;
    while (lineStream >> vertex) {
        faceVertices.push_back(vertex);
    }

    if (faceVertices.size() < 3) {
        std::cerr << "Invalid face format: less than 3 vertices. Line: " << line << std::endl;
        return;
    }

    // Triangulate the face if it has more than 3 vertices
    for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
        int vIdx1 = 0, vtIdx1 = 0, nIdx1 = 0;
        int vIdx2 = 0, vtIdx2 = 0, nIdx2 = 0;
        int vIdx3 = 0, vtIdx3 = 0, nIdx3 = 0;

        // Parse the first vertex
        if (!ParseFaceVertex(faceVertices[0], vIdx1, vtIdx1, nIdx1)) {
            std::cerr << "Failed to parse face vertex: " << faceVertices[0] << std::endl;
            return;
        }

        // Parse the second vertex
        if (!ParseFaceVertex(faceVertices[i], vIdx2, vtIdx2, nIdx2)) {
            std::cerr << "Failed to parse face vertex: " << faceVertices[i] << std::endl;
            return;
        }

        // Parse the third vertex
        if (!ParseFaceVertex(faceVertices[i + 1], vIdx3, vtIdx3, nIdx3)) {
            std::cerr << "Failed to parse face vertex: " << faceVertices[i + 1] << std::endl;
            return;
        }

        // Add the triangle to the vertex data
        AddVertexData(vIdx1, nIdx1, currentMtlName, vertices, normals);
        AddVertexData(vIdx2, nIdx2, currentMtlName, vertices, normals);
        AddVertexData(vIdx3, nIdx3, currentMtlName, vertices, normals);
    }
}


bool OBJModel::ParseFaceVertex(const std::string& faceVertex, int& vIdx, int& vtIdx, int& nIdx)
{
    // Initialize indices to 0 to handle missing data gracefully
    vIdx = vtIdx = nIdx = 0;

    // Try to match various valid face vertex formats
    if (sscanf_s(faceVertex.c_str(), "%d/%d/%d", &vIdx, &vtIdx, &nIdx) == 3) {
        return true; // v/vt/vn
    }
    else if (sscanf_s(faceVertex.c_str(), "%d//%d", &vIdx, &nIdx) == 2) {
        vtIdx = 0; // No texture coordinate
        return true; // v//vn
    }
    else if (sscanf_s(faceVertex.c_str(), "%d/%d", &vIdx, &vtIdx) == 2) {
        nIdx = 0; // No normal
        return true; // v/vt
    }
    else if (sscanf_s(faceVertex.c_str(), "%d", &vIdx) == 1) {
        vtIdx = nIdx = 0; // No texture or normal
        return true; // v
    }

    // If none of the formats matched, parsing failed
    return false;
}



void OBJModel::AddVertexData(int vIdx, int nIdx, const char* mtl,
    const std::vector<Position>& vertices,
    const std::vector<Normal>& normals)
{
    if (vIdx - 1 < 0 || vIdx - 1 >= vertices.size() ||
        (nIdx > 0 && (nIdx - 1 < 0 || nIdx - 1 >= normals.size()))) {
        std::cerr << "Error: Invalid vertex or normal index (vIdx=" << vIdx
            << ", nIdx=" << nIdx << ")." << std::endl;
        return;
    }

    Color c = { 1.0f, 1.0f, 1.0f };
    if (mMaterialsMap.find(mtl) != mMaterialsMap.end()) {
        c = mMaterialsMap[mtl];
    }
    else {
        std::cerr << "Warning: Material not found: " << mtl << ". Using default color." << std::endl;
    }

    Position pos = vertices[vIdx - 1];
    Normal normal = nIdx > 0 ? normals[nIdx - 1] : Normal{ 0.0f, 0.0f, 0.0f };

    mVertexData.push_back(pos.x);
    mVertexData.push_back(pos.y);
    mVertexData.push_back(pos.z);
    mVertexData.push_back(c.r);
    mVertexData.push_back(c.g);
    mVertexData.push_back(c.b);
    mVertexData.push_back(normal.x);
    mVertexData.push_back(normal.y);
    mVertexData.push_back(normal.z);
}
