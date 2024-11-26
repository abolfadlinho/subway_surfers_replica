#pragma once

#include <vector>
#include <string>
#include <map>

struct Position {
    float x, y, z;
};

struct Normal {
    float x, y, z;
};

struct Color {
    float r, g, b;
};

class OBJModel {
public:
    OBJModel();
    ~OBJModel();

    void LoadFromFile(const char* filename);
    std::vector<float> GetVertexData();
    int GetVertexCount();

private:
    void LoadMaterialFile(const char* filename);
    bool StartWith(std::string& line, const char* text);

    void ParseFace(const std::string& line, const char* currentMtlName,
        const std::vector<Position>& vertices,
        const std::vector<Normal>& normals);

    bool ParseFaceVertex(const std::string& faceVertex, int& vIdx, int& vtIdx, int& nIdx);

    void AddVertexData(int vIdx, int nIdx, const char* mtl,
        const std::vector<Position>& vertices,
        const std::vector<Normal>& normals);

    std::vector<float> mVertexData;
    std::map<std::string, Color> mMaterialsMap;
};
