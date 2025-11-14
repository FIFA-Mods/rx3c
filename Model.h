#pragma once
#include <Windows.h>
#undef min
#undef max
#include <string>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <variant>
#include <unordered_set>
#include <filesystem>
#define _USE_MATH_DEFINES
#include <cmath>
#pragma push_macro("isnan")
#pragma push_macro("finite")
#include <fbxsdk.h>
#pragma pop_macro("finite")
#pragma pop_macro("isnan")

using Matrix4x4 = FbxAMatrix;
using Quaternion = FbxQuaternion;

struct Vector2 {
    float x, y;
    Vector2() { x = y = 0.0f; };
    Vector2(float X, float Y) { x = X; y = Y; };
    void Set(float X, float Y) { x = X; y = Y; }
    float SquareLength() const { return x * x + y * y; }
    float Length() const { return std::sqrt(SquareLength()); }
    Vector2 &Normalize() { 
        float l = Length();
        if (l != 0.0f) *this /= Length();
        return *this;
    }
    Vector2 const &operator+=(Vector2 const &o) { x += o.x; y += o.y; return *this; }
    Vector2 const &operator-=(Vector2 const &o) { x -= o.x; y -= o.y; return *this; }
    Vector2 const &operator*=(float f) { x *= f; y *= f; return *this; }
    Vector2 const &operator/=(float f) { x /= f; y /= f; return *this; }
    float operator[](unsigned int i) const { return (i == 1) ? y : x; }
    bool operator==(Vector2 const &other) const { return x == other.x && y == other.y; }
    bool operator!=(Vector2 const &other) const { return x != other.x || y != other.y; }
    bool Equal(Vector2 const &other, float epsilon) const { return std::abs(x - other.x) <= epsilon && std::abs(y - other.y) <= epsilon; }
    Vector2 &operator=(float f) { x = y = f; return *this; }
    Vector2 const SymMul(Vector2 const &o) { return Vector2(x * o.x, y * o.y); }
    FbxDouble2 ToFbx() const { return FbxDouble2(x, y); }
    Vector2 &operator=(FbxDouble2 const &v) { x = (float)v[0]; y = (float)v[1]; return *this; }
};

inline Vector2 operator+(Vector2 const &v1, Vector2 const &v2) { return Vector2(v1.x + v2.x, v1.y + v2.y); }
inline Vector2 operator-(Vector2 const &v1, Vector2 const &v2) { return Vector2(v1.x - v2.x, v1.y - v2.y); }
inline float operator*(Vector2 const &v1, Vector2 const &v2) { return v1.x * v2.x + v1.y * v2.y; }
inline Vector2 operator*(float f, Vector2 const &v) { return Vector2(f * v.x, f * v.y); }
inline Vector2 operator*(Vector2 const &v, float f) { return Vector2(f * v.x, f * v.y); }
inline Vector2 operator/(const Vector2 &v, float f) { return v * (1 / f); }
inline Vector2 operator/(Vector2 const &v, Vector2 const &v2) { return Vector2(v.x / v2.x, v.y / v2.y); }
inline Vector2 operator-(const Vector2 &v) { return Vector2(-v.x, -v.y); }

struct Vector3 {
    float x, y, z;
    Vector3() { x = y = z = 0.0f; };
    Vector3(float X, float Y, float Z) { x = X; y = Y; z = Z; };
    void Set(float X, float Y, float Z) { x = X; y = Y; z = Z; };
    float SquareLength() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(SquareLength()); }
    Vector3 &Normalize() {
        float l = Length();
        if (l != 0.0f) *this /= Length();
        return *this;
    }
    Vector3 &NormalizeSafe() {
        float len = Length();
        if (len > 0.0f) *this /= len;
        return *this;
    }
    Vector3 const &operator+=(Vector3 const &o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vector3 const &operator-=(Vector3 const &o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vector3 const &operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    Vector3 const &operator/=(float f) {
        if (f == 0.0f) return *this;
        float invF = 1.0f / f;
        x *= invF; y *= invF; z *= invF;
        return *this;
    }
    float operator[](unsigned int i) const {
        switch (i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }
        return x;
    }
    float &operator[](unsigned int i) {
        switch (i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        }
        return x;
    }
    bool operator==(Vector3 const &other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!= (Vector3 const &other) const { return x != other.x || y != other.y || z != other.z; }
    bool Equal(Vector3 const &other, float epsilon) const {
        return std::abs(x - other.x) <= epsilon && std::abs(y - other.y) <= epsilon && std::abs(z - other.z) <= epsilon;
    }
    bool operator<(Vector3 const &other) const { return x != other.x ? x < other.x : y != other.y ? y < other.y : z < other.z; }
    Vector3 const SymMul(Vector3 const &o) { return Vector3(x * o.x, y * o.y, z * o.z); }
    FbxDouble3 ToFbx() const { return FbxDouble3(x, y, z); }
    Vector3 &operator=(FbxDouble3 const &v) { x = (float)v[0]; y = (float)v[1]; z = (float)v[2]; return *this; }
};

inline Vector3 operator-(Vector3 const &v) { return Vector3(-v.x, -v.y, -v.z); }
inline Vector3 operator+(Vector3 const &v1, Vector3 const &v2) { return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
inline Vector3 operator-(Vector3 const &v1, Vector3 const &v2) { return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
inline float operator*(Vector3 const &v1, Vector3 const &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
inline Vector3 operator*(float f, Vector3 const &v) { return Vector3(f * v.x, f * v.y, f * v.z); }
inline Vector3 operator*(Vector3 const &v, float f) { return Vector3(f * v.x, f * v.y, f * v.z); }
inline Vector3 operator/(Vector3 const &v, float f) { return v * (1 / f); }
inline Vector3 operator/(Vector3 const &v, Vector3 const &v2) { return Vector3(v.x / v2.x, v.y / v2.y, v.z / v2.z); }
inline Vector3 operator^(Vector3 const &v1, Vector3 const &v2) {
    return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

struct Vector4 {
    float x, y, z, w;
    Vector4() { x = y = z = w = 0.0f; };
    Vector4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; };
    void Set(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; };
    float operator[](unsigned int i) const {
        switch (i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        }
        return x;
    }
    float &operator[](unsigned int i) {
        switch (i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        }
        return x;
    }
    FbxDouble4 ToFbx() const { return FbxDouble4(x, y, z, w); }
    Vector4 &operator=(FbxDouble4 const &v) { x = (float)v[0]; y = (float)v[1]; z = (float)v[2]; w = (float)v[3]; return *this; }
};

struct RGBA {
    unsigned char r, g, b, a;
    inline RGBA() { r = g = b = a = 255; }
    inline RGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A) { r = R; g = G; b = B; a = A; }
    inline void Set(unsigned char R, unsigned char G, unsigned char B, unsigned char A) { r = R; g = G; b = B; a = A; }
    inline RGBA const&operator+=(RGBA const &o) { r += o.r; g += o.g; b += o.b; a += o.a; return *this; }
    inline RGBA const&operator-=(RGBA const &o) { r -= o.r; g -= o.g; b -= o.b; a -= o.a; return *this; }
    inline RGBA const &operator*=(RGBA const &o) { r *= o.r; g *= o.g; b *= o.b; a *= o.a; return *this; }
    inline RGBA const &operator/=(RGBA const &o) { r /= o.r; g /= o.g; b /= o.b; a /= o.a; return *this; }
    inline unsigned char operator[](unsigned int i) const {
        switch (i) {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        case 3: return a;
        }
        return r;
    }
    inline unsigned char &operator[](unsigned int i) {
        switch (i) {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        case 3: return a;
        }
        return r;
    }
    inline bool operator==(RGBA const &other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
    inline bool operator!=(RGBA const &other) const { return r != other.r || g != other.g || b != other.b || a != other.a; }
    inline bool operator<(RGBA const &other) const {
        return r < other.r || (r == other.r && (g < other.g || (g == other.g && (b < other.b || (b == other.b && (a < other.a))))));
    }
    inline bool IsBlack() const {
            static const float epsilon = 10e-3f;
            return std::fabs(r) < epsilon && std::fabs(g) < epsilon && std::fabs(b) < epsilon;
    }
    FbxDouble4 ToFbx() const {
        return FbxDouble4((double)r / 255.0, (double)g / 255.0, (double)b / 255.0, (double)a / 255.0);
    }
};

inline RGBA operator+(RGBA const &v1, RGBA const &v2) { return RGBA(v1.r + v2.r, v1.g + v2.g, v1.b + v2.b, v1.a + v2.a); }
inline RGBA operator-(RGBA const &v1, RGBA const &v2) { return RGBA(v1.r - v2.r, v1.g - v2.g, v1.b - v2.b, v1.a - v2.a); }
inline RGBA operator*(RGBA const &v1, RGBA const &v2) { return RGBA(v1.r * v2.r, v1.g * v2.g, v1.b * v2.b, v1.a * v2.a); }
inline RGBA operator/(RGBA const &v1, RGBA const &v2) { return RGBA(v1.r / v2.r, v1.g / v2.g, v1.b / v2.b, v1.a / v2.a); }
inline RGBA operator+(RGBA const &v, unsigned char f) { return RGBA(f + v.r, f + v.g, f + v.b, f + v.a); }
inline RGBA operator-(RGBA const &v, unsigned char f) { return RGBA(v.r - f, v.g - f, v.b - f, v.a - f); }
inline RGBA operator+(unsigned char f, RGBA const &v) { return RGBA(f + v.r, f + v.g, f + v.b, f + v.a); }
inline RGBA operator-(unsigned char f, RGBA const &v) { return RGBA(f - v.r, f - v.g, f - v.b, f - v.a); }

using PropertyValue = std::variant<int, float, double, bool, std::string, Vector2, Vector3, Vector4, RGBA, Quaternion, Matrix4x4>;

enum VertexFormat {
    V_Position = 0,
    V_Normal = 1,
    V_Tangent = 2,
    V_Binormal = 4,
    V_1TexCoord = 8 * 1,
    V_2TexCoords = 8 * 2,
    V_3TexCoords = 8 * 3,
    V_4TexCoords = 8 * 4,
    V_5TexCoords = 8 * 5,
    V_6TexCoords = 8 * 6,
    V_7TexCoords = 8 * 7,
    V_8TexCoords = 8 * 8,
    V_1Color = 128 * 1,
    V_2Colors = 128 * 2,
    V_3Colors = 128 * 3,
    V_4Colors = 128 * 4,
    V_5Colors = 128 * 5,
    V_6Colors = 128 * 6,
    V_7Colors = 128 * 7,
    V_8Colors = 128 * 8,
    V_1Bone = 2048 * 1,
    V_2Bones = 2048 * 2,
    V_3Bones = 2048 * 3,
    V_4Bones = 2048 * 4,
    V_5Bones = 2048 * 5,
    V_6Bones = 2048 * 6,
    V_7Bones = 2048 * 7,
    V_8Bones = 2048 * 8
};

#define NumTexCoords(format) (((format) >> 3) & 0xF)
#define NumColors(format) (((format) >> 7) & 0xF)
#define NumBones(format) (((format) >> 11) & 0xF)

inline void SetNumTexCoords(uint32_t &format, uint8_t n) {
    format = (format & ~(0xF << 3)) | ((n & 0xF) << 3);
}
inline void SetNumColors(uint32_t &format, uint8_t n) {
    format = (format & ~(0xF << 7)) | ((n & 0xF) << 7);
}
inline void SetNumBones(uint32_t &format, uint8_t n) {
    format = (format & ~(0xF << 11)) | ((n & 0xF) << 11);
}

struct Vertex { // 144 bytes per vertex
    Vector3 pos, normal, tangent, binormal;
    Vector2 uv[8];
    RGBA colors[8];
    float boneWeights[8];
    uint16_t boneIndices[8];
};

struct MeshData {
    uint32_t vertexFormat;
    std::vector<Vertex> vertices;
    std::vector<std::array<uint32_t, 3>> triangles;
    std::string material;

    MeshData() {
        vertexFormat = 0;
    }
};

struct Material {
    std::string name, texture, normalMap;
    RGBA color;
    std::map<std::string, PropertyValue> properties;
};

struct Texture {
    std::string name;
    std::string filename;
    std::map<std::string, PropertyValue> properties;

};

struct Object {
    std::string name, parent;
    Matrix4x4 transform;
    MeshData mesh;
    std::map<std::string, PropertyValue> properties;
};

struct Bone {
    std::string name, parent;
    Matrix4x4 transform;
    std::map<std::string, PropertyValue> properties;
};

struct Skeleton {
    std::vector<Bone> bones;
    std::map<std::string, PropertyValue> properties;
};

enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_UNABLE_TO_CREATE_FBX_MANAGER,

};

inline void Error(std::string const &text) {
    MessageBoxA(NULL, text.c_str(), "Error", MB_ICONERROR);
}

struct Model {
    std::string name;
    std::vector<Object> objects;
    std::vector<Material> materials;
    std::vector<Texture> textures;
    Skeleton skeleton;
    std::map<std::string, PropertyValue> properties;

    int ReadFbx(std::filesystem::path const &filename) {

        return ERROR_NONE;
    }

    int WriteFbx(std::filesystem::path const &filename, bool ascii = false) {
        auto getObjectIndex = [&](std::string const &name) {
            for (size_t i = 0; i < objects.size(); i++) {
                if (objects[i].name == name)
                    return (int)i;
            }
            return -1;
        };
        auto getMaterialIndex = [&](std::string const &name) {
            for (size_t i = 0; i < materials.size(); i++) {
                if (materials[i].name == name)
                    return (int)i;
            }
            return -1;
        };
        auto getBoneIndex = [&](std::string const &name) {
            for (size_t i = 0; i < skeleton.bones.size(); i++) {
                if (skeleton.bones[i].name == name)
                    return (int)i;
            }
            return -1;
        };
        auto attachProperties = [](FbxObject *obj, std::map<std::string, PropertyValue> const &props) {
            for (auto &[key, value] : props) {
                std::visit([&](auto &&val) {
                    using T = std::decay_t<decltype(val)>;
                    FbxProperty prop;
                    if constexpr (std::is_same_v<T, int>) {
                        prop = FbxProperty::Create(obj, FbxIntDT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val);
                    }
                    else if constexpr (std::is_same_v<T, float>) {
                        prop = FbxProperty::Create(obj, FbxFloatDT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val);
                    }
                    else if constexpr (std::is_same_v<T, double>) {
                        prop = FbxProperty::Create(obj, FbxDoubleDT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val);
                    }
                    else if constexpr (std::is_same_v<T, bool>) {
                        prop = FbxProperty::Create(obj, FbxBoolDT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val);
                    }
                    else if constexpr (std::is_same_v<T, std::string>) {
                        prop = FbxProperty::Create(obj, FbxStringDT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(FbxString(val.c_str()));
                    }
                    else if constexpr (std::is_same_v<T, Vector2>) {
                        prop = FbxProperty::Create(obj, FbxDouble2DT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val.ToFbx());
                    }
                    else if constexpr (std::is_same_v<T, Vector3>) {
                        prop = FbxProperty::Create(obj, FbxDouble3DT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val.ToFbx());
                    }
                    else if constexpr (std::is_same_v<T, Vector4>) {
                        prop = FbxProperty::Create(obj, FbxDouble4DT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val.ToFbx());
                    }
                    else if constexpr (std::is_same_v<T, RGBA>) {
                        prop = FbxProperty::Create(obj, FbxDouble4DT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val.ToFbx());
                    }
                    else if constexpr (std::is_same_v<T, Quaternion>) {
                        prop = FbxProperty::Create(obj, FbxQuaternionDT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val); 
                    }
                    else if constexpr (std::is_same_v<T, Matrix4x4>) {
                        prop = FbxProperty::Create(obj, FbxDouble4x4DT, key.c_str());
                        prop.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
                        prop.Set(val);
                    }
                }, value);
            }
        };
        FbxManager *fbxManager = FbxManager::Create();
        if (!fbxManager)
            return ERROR_UNABLE_TO_CREATE_FBX_MANAGER;
        FbxScene *fbxScene = FbxScene::Create(fbxManager, filename.stem().string().c_str());
        attachProperties(fbxScene, properties);
        // create nodes
        std::vector<FbxNode *> fbxNodes(objects.size());
        for (size_t nodeIdx = 0; nodeIdx < objects.size(); nodeIdx++) {
            Object const &obj = objects[nodeIdx];
            FbxNode *fbxNode = FbxNode::Create(fbxScene, obj.name.c_str());
            attachProperties(fbxNode, obj.properties);
            fbxNode->LclTranslation.Set(obj.transform.GetT());
            fbxNode->LclRotation.Set(obj.transform.GetR());
            fbxNode->LclScaling.Set(obj.transform.GetS());
            fbxNodes[nodeIdx] = fbxNode;
        }
        // setup nodes hierarchy
        FbxNode *root = fbxScene->GetRootNode();
        for (size_t nodeIdx = 0; nodeIdx < objects.size(); nodeIdx++) {
            int parentIndex = getObjectIndex(objects[nodeIdx].parent);
            if (parentIndex >= 0 && parentIndex < (int)objects.size())
                fbxNodes[parentIndex]->AddChild(fbxNodes[nodeIdx]);
            else
                root->AddChild(fbxNodes[nodeIdx]);
        }
        bool isSkinned = !skeleton.bones.empty();
        // create textures
        std::map<std::string, FbxFileTexture *> texByName;
        std::vector<FbxFileTexture *> fbxTextures(textures.size());
        for (size_t texIdx = 0; texIdx < textures.size(); texIdx++) {
            Texture const &tex = textures[texIdx];
            FbxFileTexture *fbxTex = FbxFileTexture::Create(fbxScene, tex.name.c_str());
            attachProperties(fbxTex, tex.properties);
            fbxTex->SetFileName(tex.filename.c_str());
            fbxTextures[texIdx] = fbxTex;
            texByName[tex.name] = fbxTex;
        }
        // create materials
        std::vector<FbxSurfaceMaterial *> fbxMaterials(materials.size());
        for (size_t matIdx = 0; matIdx < materials.size(); matIdx++) {
            const Material &mat = materials[matIdx];
            FbxSurfacePhong *fbxMat = FbxSurfacePhong::Create(fbxScene, mat.name.c_str());
            if (mat.color.r != 255 || mat.color.g != 255 || mat.color.b != 255)
                fbxMat->Diffuse.Set(FbxDouble3(double(mat.color.r) / 255.0, double(mat.color.g) / 255.0, double(mat.color.b) / 255.0));
            if (mat.color.a != 255)
                fbxMat->TransparencyFactor.Set(1.0 - (double(mat.color.a) / 255.0));
            if (!mat.texture.empty() && texByName.contains(mat.texture))
                fbxMat->Diffuse.ConnectSrcObject(texByName[mat.texture]);
            if (!mat.normalMap.empty() && texByName.contains(mat.normalMap))
                fbxMat->Bump.ConnectSrcObject(texByName[mat.normalMap]);
            attachProperties(fbxMat, mat.properties);
            fbxMaterials[matIdx] = fbxMat;
        }
        std::vector<FbxMesh *> fbxMeshes;
        // create meshes
        for (size_t nodeIdx = 0; nodeIdx < objects.size(); nodeIdx++) {
            Object const &obj = objects[nodeIdx];
            FbxMesh *fbxMesh = nullptr;
            if (!obj.mesh.triangles.empty()) {
                fbxMesh = FbxMesh::Create(fbxScene, obj.name.c_str());
                const MeshData &mesh = obj.mesh;
                if (mesh.vertices.empty())
                    fbxMesh->InitControlPoints(0);
                else {
                    int cpCount = (int)mesh.vertices.size();
                    fbxMesh->InitControlPoints(cpCount);
                    FbxVector4 *controlPoints = fbxMesh->GetControlPoints();
                    for (int i = 0; i < cpCount; ++i) {
                        const Vertex &v = mesh.vertices[i];
                        controlPoints[i] = FbxVector4((double)v.pos.x, (double)v.pos.y, (double)v.pos.z, 1.0);
                    }
                }
                int numPolygonVertices = (int)mesh.triangles.size() * 3;
                FbxLayerElementNormal *leNormal = nullptr;
                if (mesh.vertexFormat & V_Normal) {
                    leNormal = fbxMesh->CreateElementNormal();
                    leNormal->SetMappingMode(FbxLayerElement::eByPolygonVertex);
                    leNormal->SetReferenceMode(FbxLayerElement::eDirect);
                }
                FbxLayerElementTangent *leTangent = nullptr;
                if (mesh.vertexFormat & V_Tangent) {
                    leTangent = fbxMesh->CreateElementTangent();
                    leTangent->SetMappingMode(FbxLayerElement::eByPolygonVertex);
                    leTangent->SetReferenceMode(FbxLayerElement::eDirect);
                }
                FbxLayerElementBinormal *leBinormal = nullptr;
                if (mesh.vertexFormat & V_Binormal) {
                    leBinormal = fbxMesh->CreateElementBinormal();
                    leBinormal->SetMappingMode(FbxLayerElement::eByPolygonVertex);
                    leBinormal->SetReferenceMode(FbxLayerElement::eDirect);
                }
                std::vector<FbxLayerElementUV *> uvLayers;
                for (unsigned char uvSet = 0; uvSet < NumTexCoords(mesh.vertexFormat); ++uvSet) {
                    std::string uvName = "UV" + std::to_string(uvSet);
                    FbxLayerElementUV *leUV = fbxMesh->CreateElementUV(uvName.c_str());
                    leUV->SetMappingMode(FbxLayerElement::eByPolygonVertex);
                    leUV->SetReferenceMode(FbxLayerElement::eDirect);
                    uvLayers.push_back(leUV);
                }
                std::vector<FbxLayerElementVertexColor *> colorLayers;
                for (unsigned char c = 0; c < NumColors(mesh.vertexFormat); ++c) {
                    FbxLayerElementVertexColor *leVC = fbxMesh->CreateElementVertexColor();
                    leVC->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
                    leVC->SetReferenceMode(FbxGeometryElement::eDirect);
                    colorLayers.push_back(leVC);
                }
                for (size_t t = 0; t < mesh.triangles.size(); ++t) {
                    uint32_t i0 = mesh.triangles[t][0];
                    uint32_t i1 = mesh.triangles[t][1];
                    uint32_t i2 = mesh.triangles[t][2];
                    fbxMesh->BeginPolygon(-1, -1, false);
                    fbxMesh->AddPolygon((int)i0);
                    fbxMesh->AddPolygon((int)i1);
                    fbxMesh->AddPolygon((int)i2);
                    fbxMesh->EndPolygon();
                    const unsigned int cornerIdx[3] = { i0, i1, i2 };
                    for (int corner = 0; corner < 3; ++corner) {
                        const Vertex &v = mesh.vertices[cornerIdx[corner]];
                        if (leNormal)
                            leNormal->GetDirectArray().Add(FbxVector4((double)v.normal.x, (double)v.normal.y, (double)v.normal.z));
                        if (leTangent)
                            leTangent->GetDirectArray().Add(FbxVector4((double)v.tangent.x, (double)v.tangent.y, (double)v.tangent.z));
                        if (leBinormal)
                            leBinormal->GetDirectArray().Add(FbxVector4((double)v.binormal.x, (double)v.binormal.y, (double)v.binormal.z));
                        for (unsigned int uvSet = 0; uvSet < uvLayers.size(); ++uvSet) {
                            const Vector2 &uv = v.uv[uvSet];
                            uvLayers[uvSet]->GetDirectArray().Add(FbxVector2((double)uv.x, 1.0 - (double)uv.y));
                        }
                        for (unsigned int cIdx = 0; cIdx < colorLayers.size(); ++cIdx) {
                            const RGBA &col = v.colors[cIdx];
                            colorLayers[cIdx]->GetDirectArray().Add(FbxColor((double)col.r / 255.0,
                                (double)col.g / 255.0, (double)col.b / 255.0, (double)col.a / 255.0));
                        }
                    }
                }
                if (!uvLayers.empty()) {
                    FbxLayer *layer = fbxMesh->GetLayer(0);
                    if (!layer) {
                        fbxMesh->CreateLayer();
                        layer = fbxMesh->GetLayer(0);
                    }
                    layer->SetUVs(uvLayers[0], FbxLayerElement::eTextureDiffuse);
                }

                if (!mesh.material.empty()) {
                    int materialIndex = getMaterialIndex(mesh.material);
                    if (materialIndex != -1) {
                        fbxNodes[nodeIdx]->AddMaterial(fbxMaterials[materialIndex]);
                        FbxLayerElementMaterial *leMat = fbxMesh->CreateElementMaterial();
                        leMat->SetMappingMode(FbxLayerElement::eByPolygon);
                        leMat->SetReferenceMode(FbxLayerElement::eIndexToDirect);
                        for (size_t p = 0; p < mesh.triangles.size(); ++p)
                            leMat->GetIndexArray().Add(materialIndex);
                    }
                }
                fbxNodes[nodeIdx]->SetNodeAttribute(fbxMesh);
            }
            fbxMeshes.push_back(fbxMesh);
        }
        // skeleton
        if (isSkinned) {
            // create bones
            std::vector<FbxNode *> boneNodes;
            boneNodes.resize(skeleton.bones.size());
            for (size_t boneIdx = 0; boneIdx < skeleton.bones.size(); boneIdx++) {
                const Bone &bone = skeleton.bones[boneIdx];
                FbxSkeleton *fbxSkeleton = FbxSkeleton::Create(fbxScene, bone.name.c_str());
                attachProperties(fbxSkeleton, skeleton.properties);
                int parentIndex = getBoneIndex(bone.parent);
                fbxSkeleton->SetSkeletonType(parentIndex == -1 ? FbxSkeleton::EType::eRoot : FbxSkeleton::EType::eLimbNode);
                FbxNode *fbxNode = FbxNode::Create(fbxScene, bone.name.c_str());
                attachProperties(fbxNode, bone.properties);
                fbxNode->SetNodeAttribute(fbxSkeleton);
                fbxNode->LclTranslation.Set(bone.transform.GetT());
                fbxNode->LclRotation.Set(bone.transform.GetR());
                fbxNode->LclScaling.Set(bone.transform.GetS());
                boneNodes[boneIdx] = fbxNode;
            }
            // setup bones hierarchy
            for (size_t boneIdx = 0; boneIdx < skeleton.bones.size(); boneIdx++) {
                int parentIndex = getBoneIndex(skeleton.bones[boneIdx].parent);
                if (parentIndex == -1)
                    root->AddChild(boneNodes[boneIdx]);
                else
                    boneNodes[parentIndex]->AddChild(boneNodes[boneIdx]);
            }
            // setup skinning
            for (size_t nodeIdx = 0; nodeIdx < objects.size(); nodeIdx++) {
                if (fbxMeshes[nodeIdx]) {
                    Object const &obj = objects[nodeIdx];
                    FbxNode *meshNode = fbxNodes[nodeIdx];
                    MeshData const &mesh = obj.mesh;
                    FbxMesh *fbxMesh = fbxMeshes[nodeIdx];
                    FbxSkin *fbxSkin = FbxSkin::Create(fbxScene, "Skin");
                    std::vector<FbxCluster *> fbxClusters(skeleton.bones.size());
                    for (size_t boneIdx = 0; boneIdx < skeleton.bones.size(); boneIdx++) {
                        FbxCluster *fbxCluster = FbxCluster::Create(fbxScene, ("Cluster_" + skeleton.bones[boneIdx].name).c_str());
                        fbxCluster->SetLink(boneNodes[boneIdx]);
                        fbxCluster->SetLinkMode(FbxCluster::eNormalize);
                        fbxClusters[boneIdx] = fbxCluster;
                    }
                    for (size_t boneIdx = 0; boneIdx < skeleton.bones.size(); boneIdx++) {
                        for (size_t v = 0; v < mesh.vertices.size(); v++) {
                            for (size_t wi = 0; wi < NumBones(mesh.vertexFormat); wi++) {
                                if (mesh.vertices[v].boneIndices[wi] == boneIdx) {
                                    float w = mesh.vertices[v].boneWeights[wi];
                                    if (w > 0.0f)
                                        fbxClusters[boneIdx]->AddControlPointIndex((int)v, (double)w);
                                }
                            }
                        }
                    }
                    for (size_t boneIdx = 0; boneIdx < skeleton.bones.size(); boneIdx++) {
                        fbxClusters[boneIdx]->SetTransformMatrix(meshNode->EvaluateGlobalTransform());
                        fbxClusters[boneIdx]->SetTransformLinkMatrix(boneNodes[boneIdx]->EvaluateGlobalTransform());
                        fbxSkin->AddCluster(fbxClusters[boneIdx]);
                    }
                    fbxMesh->AddDeformer(fbxSkin);
                }
            }
        }
        // export
        FbxExporter *fbxExporter = FbxExporter::Create(fbxManager, "");
        char *pFilenameUtf8 = nullptr;
        FbxWCToUTF8(filename.c_str(), pFilenameUtf8);
        int fileFormat = -1;
        if (ascii) {
            int numFormats = fbxManager->GetIOPluginRegistry()->GetWriterFormatCount();
            for (int i = 0; i < numFormats; ++i) {
                if (fbxManager->GetIOPluginRegistry()->WriterIsFBX(i)) {
                    FbxString desc = fbxManager->GetIOPluginRegistry()->GetWriterFormatDescription(i);
                    if (desc.Find("ascii") >= 0) {
                        fileFormat = i;
                        break;
                    }
                }
            }
        }
        bool exportStatus = fbxExporter->Initialize(pFilenameUtf8, fileFormat, fbxManager->GetIOSettings());
        if (exportStatus)
            fbxExporter->Export(fbxScene);
        fbxExporter->Destroy();
        fbxManager->Destroy();
        return ERROR_NONE;
    }

    Model &operator+=(Model const &other) {
        auto make_unique_name = [](std::string const &base, std::unordered_set<std::string> &existing) {
            if (existing.find(base) == existing.end()) {
                existing.insert(base);
                return base;
            }
            for (int i = 1; i <= 999; ++i) {
                std::ostringstream oss;
                oss << base << '.' << std::setw(3) << std::setfill('0') << i;
                std::string candidate = oss.str();
                if (existing.find(candidate) == existing.end()) {
                    existing.insert(candidate);
                    return candidate;
                }
            }
            std::string fallback = base + "_dup";
            int suffix = 0;
            while (existing.find(fallback + std::to_string(suffix)) != existing.end()) ++suffix;
            fallback += std::to_string(suffix);
            existing.insert(fallback);
            return fallback;
        };
        auto bone_equal = [](Bone const &A, Bone const &B) {
            if (A.name != B.name)
                return false;
            if (A.parent != B.parent)
                return false;
            if (A.properties.size() != B.properties.size()) // TODO: check this
                return false;
            for (auto const &[k, v] : A.properties) {
                auto it = B.properties.find(k);
                if (it == B.properties.end())
                    return false;
            }
            return true;
        };
        auto skeleton_equal = [&bone_equal](Skeleton const &S1, Skeleton const &S2) {
            if (S1.bones.size() != S2.bones.size())
                return false;
            for (size_t i = 0; i < S1.bones.size(); ++i) {
                if (!bone_equal(S1.bones[i], S2.bones[i]))
                    return false;
            }
            return true;
        };
        std::unordered_set<std::string> existing_object_names;
        existing_object_names.reserve(objects.size() + other.objects.size());
        for (auto const &o : objects) existing_object_names.insert(o.name);
        for (auto obj : other.objects) {
            if (existing_object_names.find(obj.name) == existing_object_names.end()) {
                existing_object_names.insert(obj.name);
                objects.push_back(std::move(obj));
            }
            else {
                std::string newname = make_unique_name(obj.name, existing_object_names);
                obj.name = std::move(newname);
                objects.push_back(std::move(obj));
            }
        }
        std::unordered_set<std::string> material_names;
        material_names.reserve(materials.size() + other.materials.size());
        for (auto const &m : materials) material_names.insert(m.name);
        for (auto const &m : other.materials) {
            if (material_names.insert(m.name).second)
                materials.push_back(m);
        }
        std::unordered_set<std::string> texture_names;
        texture_names.reserve(textures.size() + other.textures.size());
        for (auto const &t : textures) texture_names.insert(t.name);
        for (auto const &t : other.textures) {
            if (texture_names.insert(t.name).second)
                textures.push_back(t);
        }
        bool this_has = !skeleton.bones.empty();
        bool other_has = !other.skeleton.bones.empty();
        if (!this_has && other_has)
            skeleton = other.skeleton;
        else if (this_has && other_has && !skeleton_equal(skeleton, other.skeleton))
            throw std::runtime_error("Cannot merge models: skeletons differ");
        for (auto const &[k, v] : other.properties) {
            if (properties.find(k) == properties.end())
                properties.emplace(k, v);
        }
        return *this;
    }
};

inline Model operator+(Model lhs, Model const &rhs) {
    lhs += rhs;
    return lhs;
}
