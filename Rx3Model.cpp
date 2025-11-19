#include <Windows.h>
#undef min
#undef max
#include "Rx3Model.h"
#include "Rx3Container.h"
#include "half.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

namespace helper::rx3model {
enum DataType {
    dt_unknown, dt_void, dt_1f32, dt_1s32, dt_1s16, dt_1s8, dt_2f32, dt_2s32, dt_2s16, dt_2s8, dt_3f32, dt_3s32,
    dt_3s16, dt_3s8, dt_4f32, dt_4s32, dt_4s16, dt_4s8, dt_4u8, dt_4u8n, dt_4u8endianswapp, dt_4u8nendianswap,
    dt_2s16n, dt_4s16n, dt_3u10, dt_3s10n, dt_3s11n, dt_2f16, dt_4f16, dt_2s16s, dt_3s16s, dt_1u16rgb565,
    dt_3u8rgb8, dt_4u8rgbx8, dt_1u16rgba4, dt_3u8rgba6, dt_4u8rgba8, dt_2u16, dt_4u16, dt_2u16n, dt_4u16n, dt_custom
};

char const *DataTypeNames[] = {
    "unknown", "void", "1f32", "1s32", "1s16", "1s8", "2f32", "2s32", "2s16", "2s8", "3f32", "3s32",
    "3s16", "3s8", "4f32", "4s32", "4s16", "4s8", "4u8", "4u8n", "4u8endianswapp", "4u8nendianswap",
    "2s16n", "4s16n", "3u10", "3s10n", "3s11n", "2f16", "4f16", "2s16s", "3s16s", "1u16rgb565",
    "3u8rgb8", "4u8rgbx8", "1u16rgba4", "3u8rgba6", "4u8rgba8", "2u16", "4u16", "2u16n", "4u16n", "custom"
};

DataType DataTypeIdFromName(string const &name) {
    for (size_t i = 0; i < std::size(DataTypeNames); i++) {
        if (name == DataTypeNames[i])
            return (DataType)i;
    }
    return dt_unknown;
}

float UnpackFloatFrom10Bit(int value) {
    return (value < 0) ? float(value) / 512.0f : float(value) / 511.0f;
}

float UnpackFloatFrom11Bit(int value) {
    return (value < 0) ? float(value) / 1024.0f : float(value) / 1023.0f;
}

array<float, 4> UnpackVertexAttribute(DataType dt, const unsigned char *data) {
    if (!data) return { 0, 0, 0, 0 };

    if (dt == dt_1f32) {
        float v; memcpy(&v, data, sizeof(v));
        return { v, 0, 0, 1 };
    }
    if (dt == dt_1s32) {
        int32_t v; memcpy(&v, data, sizeof(v));
        return { (float)v, 0, 0, 1 };
    }
    if (dt == dt_1s16) {
        int16_t v; memcpy(&v, data, sizeof(v));
        return { (float)v, 0, 0, 1 };
    }
    if (dt == dt_1s8) {
        int8_t v; memcpy(&v, data, sizeof(v));
        return { (float)v, 0, 0, 1 };
    }

    if (dt == dt_2f32) {
        float v[2]; memcpy(v, data, sizeof(v));
        return { v[0], v[1], 0, 1 };
    }
    if (dt == dt_2s32) {
        int32_t v[2]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], 0, 1 };
    }
    if (dt == dt_2s16) {
        int16_t v[2]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], 0, 1 };
    }
    if (dt == dt_2s8) {
        int8_t v[2]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], 0, 1 };
    }

    if (dt == dt_3f32) {
        float v[3]; memcpy(v, data, sizeof(v));
        return { v[0], v[1], v[2], 1 };
    }
    if (dt == dt_3s32) {
        int32_t v[3]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], 1 };
    }
    if (dt == dt_3s16) {
        int16_t v[3]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], 1 };
    }
    if (dt == dt_3s8) {
        int8_t v[3]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], 1 };
    }

    if (dt == dt_4f32) {
        float v[4]; memcpy(v, data, sizeof(v));
        return { v[0], v[1], v[2], v[3] };
    }
    if (dt == dt_4s32) {
        int32_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], (float)v[3] };
    }
    if (dt == dt_4s16) {
        int16_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], (float)v[3] };
    }
    if (dt == dt_4s8) {
        int8_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], (float)v[3] };
    }

    // Unsigned 8-bit integer color
    if (dt == dt_4u8) {
        uint8_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], (float)v[3] };
    }

    // Normalized unsigned 8-bit color
    if (dt == dt_4u8n) {
        uint8_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0] / 255.0f, (float)v[1] / 255.0f, (float)v[2] / 255.0f, (float)v[3] / 255.0f };
    }

    // Unsigned 8-bit integer color endian swap
    if (dt == dt_4u8endianswapp) {
        uint8_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[3], (float)v[2], (float)v[1], (float)v[0] };
    }

    // Normalized unsigned 8-bit color endian swap
    if (dt == dt_4u8nendianswap) {
        uint8_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[3] / 255.0f, (float)v[2] / 255.0f, (float)v[1] / 255.0f, (float)v[0] / 255.0f };
    }

    // 10-bit unsigned integer vector (e.g., GL_RGB10)
    if (dt == dt_3u10) {
        uint32_t packed; memcpy(&packed, data, sizeof(packed));
        uint32_t x = (packed >> 0) & 0x3FF;
        uint32_t y = (packed >> 10) & 0x3FF;
        uint32_t z = (packed >> 20) & 0x3FF;
        return { (float)x / 1023.0f, (float)y / 1023.0f, (float)z / 1023.0f, 1 };
    }

    // 10-bit signed normalized vector
    if (dt == dt_3s10n) {
        uint32_t packed;
        memcpy(&packed, data, sizeof(packed));
        int32_t x = ((packed >> 0) & 0x3FF) - ((packed & 0x200) ? 1024 : 0);
        int32_t y = ((packed >> 10) & 0x3FF) - ((packed & 0x80000) ? 1024 : 0);
        int32_t z = ((packed >> 20) & 0x3FF) - ((packed & 0x20000000) ? 1024 : 0);
        return { UnpackFloatFrom10Bit(x), UnpackFloatFrom10Bit(y), UnpackFloatFrom10Bit(z), 1 };
    }

    // Half-float (16-bit float)
    if (dt == dt_2f16) {
        half_float::half v[2]; memcpy(v, data, sizeof(v));
        float x = v[0];
        float y = v[1];
        return { x, y, 0, 1 };
    }
    if (dt == dt_4f16) {
        half_float::half v[4]; memcpy(v, data, sizeof(v));
        float x = v[0];
        float y = v[1];
        float z = v[2];
        float w = v[3];
        return { x, y, z, w };
    }

    // RGB565 format
    if (dt == dt_1u16rgb565) {
        uint16_t packed; memcpy(&packed, data, sizeof(packed));
        uint8_t r = (packed >> 11) & 0x1F;
        uint8_t g = (packed >> 5) & 0x3F;
        uint8_t b = (packed >> 0) & 0x1F;
        return { (float)r / 31.0f, (float)g / 63.0f, (float)b / 31.0f, 1 };
    }

    // RGBA4 format
    if (dt == dt_1u16rgba4) {
        uint16_t packed; memcpy(&packed, data, sizeof(packed));
        uint8_t r = (packed >> 12) & 0xF;
        uint8_t g = (packed >> 8) & 0xF;
        uint8_t b = (packed >> 4) & 0xF;
        uint8_t a = (packed >> 0) & 0xF;
        return { (float)r / 15.0f, (float)g / 15.0f, (float)b / 15.0f, (float)a / 15.0f };
    }

    if (dt == dt_3s11n) {
        uint32_t packed;
        memcpy(&packed, data, sizeof(packed));
        int32_t x = ((packed >> 0) & 0x7FF) - ((packed & 0x400) ? 2048 : 0);
        int32_t y = ((packed >> 11) & 0x7FF) - ((packed & 0x200000) ? 2048 : 0);
        int32_t z = ((packed >> 22) & 0x3FF) - ((packed & 0x80000000) ? 1024 : 0);  // Last 10 bits
        return { UnpackFloatFrom11Bit(x), UnpackFloatFrom11Bit(y), UnpackFloatFrom10Bit(z), 1 };
    }

    // 16-bit signed scaled values
    if (dt == dt_2s16s) {
        int16_t v[2]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], 0, 1 };
    }
    if (dt == dt_3s16s) {
        int16_t v[3]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], 1 };
    }

    // 8-bit unsigned integer RGB
    if (dt == dt_3u8rgb8) {
        uint8_t v[3]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], 1 };
    }

    // 8-bit unsigned integer RGBX
    if (dt == dt_4u8rgbx8) {
        uint8_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], 1 }; // Ignore X channel
    }

    // 6-bit-per-channel RGBA
    if (dt == dt_3u8rgba6) {
        uint8_t v[3]; memcpy(v, data, sizeof(v));
        return { (float)v[0] / 63.0f, (float)v[1] / 63.0f, (float)v[2] / 63.0f, 1 };
    }

    // Standard 8-bit RGBA
    if (dt == dt_4u8rgba8) {
        uint8_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0] / 255.0f, (float)v[1] / 255.0f, (float)v[2] / 255.0f, (float)v[3] / 255.0f };
    }

    // 16-bit unsigned integers
    if (dt == dt_2u16) {
        uint16_t v[2]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], 0, 1 };
    }
    if (dt == dt_4u16) {
        uint16_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0], (float)v[1], (float)v[2], (float)v[3] };
    }

    // 16-bit unsigned normalized
    if (dt == dt_2u16n) {
        uint16_t v[2]; memcpy(v, data, sizeof(v));
        return { (float)v[0] / 65535.0f, (float)v[1] / 65535.0f, 0, 1 };
    }
    if (dt == dt_4u16n) {
        uint16_t v[4]; memcpy(v, data, sizeof(v));
        return { (float)v[0] / 65535.0f, (float)v[1] / 65535.0f, (float)v[2] / 65535.0f, (float)v[3] / 65535.0f };
    }

    // Default case: return zero vector
    return { 0, 0, 0, 0 };
}

Vector2 UnpackVector2(DataType dt, const unsigned char *data) {
    auto unpacked = UnpackVertexAttribute(dt, data);
    return Vector2(unpacked[0], unpacked[1]);
}

Vector3 UnpackVector3(DataType dt, const unsigned char *data) {
    auto unpacked = UnpackVertexAttribute(dt, data);
    return Vector3(unpacked[0], unpacked[1], unpacked[2]);
}

RGBA UnpackColor(DataType dt, const unsigned char *data) {
    auto unpacked = UnpackVertexAttribute(dt, data);
    return RGBA(
        (unsigned char)std::clamp(unpacked[0] * 255.0f, 0.0f, 255.0f),
        (unsigned char)std::clamp(unpacked[1] * 255.0f, 0.0f, 255.0f),
        (unsigned char)std::clamp(unpacked[2] * 255.0f, 0.0f, 255.0f),
        (unsigned char)std::clamp(unpacked[2] * 255.0f, 0.0f, 255.0f));
}

vector<pair<unsigned int, string>> ExtractNamesFromChunk(Rx3Chunk const *namesChunk) {
    vector<pair<unsigned int, string>> result;
    Rx3Reader reader(namesChunk);
    reader.Skip(4);
    unsigned int numNames = reader.Read<unsigned int>();
    reader.Skip(8);
    for (unsigned int i = 0; i < numNames; i++) {
        unsigned int id = reader.Read<unsigned int>();
        unsigned int nameLength = reader.Read<unsigned int>();
        result.emplace_back(id, reader.GetString());
        reader.Skip(nameLength);
    }
    return result;
}

template<typename T>
T SafeConvertInt(std::string const &str, bool isHex = false) {
    T result = 0;
    try {
        result = static_cast<T>(std::stoull(str, 0, isHex ? 16 : 10));
    }
    catch (...) {}
    return result;
}

void Trim(std::string &str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start != std::string::npos)
        str = str.substr(start);
    size_t end = str.find_last_not_of(" \t\r\n");
    if (end != std::string::npos)
        str = str.substr(0, end + 1);
}

std::vector<std::string> Split(std::string const &line, char delim, bool trim = false, bool skipEmpty = false, bool quotesHavePriority = false) {
    std::vector<std::string> result;
    std::string currStr;
    auto AddStr = [&, trim, skipEmpty]() {
        if (trim)
            Trim(currStr);
        if (!skipEmpty || !currStr.empty())
            result.push_back(currStr);
        currStr.clear();
    };
    bool inQuotes = false;
    for (size_t i = 0; i < line.length(); i++) {
        auto c = line[i];
        if (c == '\r' || (delim != '\n' && c == '\n'))
            break;
        if (!inQuotes) {
            if (quotesHavePriority && c == '"')
                inQuotes = true;
            else if (c == delim)
                AddStr();
            else
                currStr += c;
        }
        else {
            if (c == '"')
                inQuotes = false;
            else
                currStr += c;
        }
    }
    AddStr();
    return result;
}

bool EndsWithNumber(const std::string &s, int &outNumber) {
    size_t pos = s.rfind('_');
    if (pos == std::string::npos) return false;
    if (pos == s.size() - 1) return false;
    std::string numPart = s.substr(pos + 1);
    if (!std::all_of(numPart.begin(), numPart.end(), ::isdigit))
        return false;
    outNumber = std::stoi(numPart);
    return true;
}
}

void SetupObjectMesh(Object &obj, Rx3Chunk *vfChunk, Rx3Chunk *vbChunk, Rx3Chunk *ibChunk, int primType, Rx3Options const &options) {
    using namespace helper::rx3model;
    obj.mesh.vertexFormat = 0;
    Rx3Reader vertexDeclReader(vfChunk);
    Rx3Reader vertexBufferReader(vbChunk);
    Rx3Reader indexBufferReader(ibChunk);
    vertexDeclReader.Skip(4);
    uint32_t declStrLen = vertexDeclReader.Read<uint32_t>();
    if (declStrLen > 0) {
        vertexDeclReader.Skip(8);
        string decl = vertexDeclReader.GetString();
        auto declElements = Split(decl, ' ');
        if (!declElements.empty()) {
            vertexBufferReader.Skip(4);
            uint32_t numVertices = vertexBufferReader.Read<uint32_t>();
            uint32_t vs = vertexBufferReader.Read<uint32_t>();
            vertexBufferReader.Skip(4);
            auto vb = vertexBufferReader.GetCurrentPtr();
            indexBufferReader.Skip(4);
            uint32_t numIndices = indexBufferReader.Read<uint32_t>();
            uint8_t is = indexBufferReader.Read<uint8_t>();
            indexBufferReader.Skip(7);
            auto ib = indexBufferReader.GetCurrentPtr();
            obj.mesh.vertices.resize(numVertices);
            for (size_t d = 0; d < declElements.size(); d++) {
                auto elementInfo = Split(declElements[d], ':');
                string strUsage, strOffset, strDataType;
                if (elementInfo.size() == 5) {
                    strUsage = elementInfo[0];
                    strOffset = elementInfo[1];
                    strDataType = elementInfo[4];
                }
                else if (elementInfo.size() == 4) {
                    strUsage = elementInfo[0];
                    strOffset = elementInfo[1];
                    strDataType = elementInfo[3];
                }
                else if (elementInfo.size() == 3) {
                    strUsage = elementInfo[0];
                    strOffset = elementInfo[1];
                    strDataType = elementInfo[2];
                }
                char usage = 0;
                unsigned char usageIndex = 0;
                if (strUsage.size() == 2) {
                    usage = strUsage[0];
                    usageIndex = (strUsage[1] >= '0' && strUsage[1] <= '9') ? (strUsage[1] - '0') : 0;
                }
                uint32_t offset = strOffset.empty() ? 0 : SafeConvertInt<uint32_t>(strOffset, true);
                DataType t = DataTypeIdFromName(strDataType);
                if (usage == 'p') {
                    if (usageIndex == 0) {
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.mesh.vertices[v].pos = UnpackVector3(t, vd);
                        }
                    }
                }
                else if (usage == 'n') {
                    if (usageIndex == 0) {
                        obj.mesh.vertexFormat |= V_Normal;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.mesh.vertices[v].normal = UnpackVector3(t, vd);
                        }
                    }
                }
                else if (usage == 'g') {
                    if (usageIndex == 0) {
                        obj.mesh.vertexFormat |= V_Tangent;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.mesh.vertices[v].tangent = UnpackVector3(t, vd);
                        }
                    }
                }
                else if (usage == 'b') {
                    if (usageIndex == 0) {
                        obj.mesh.vertexFormat |= V_Binormal;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.mesh.vertices[v].binormal = UnpackVector3(t, vd);
                        }
                    }
                }
                else if (usage == 't') {
                    if (usageIndex <= 7) {
                        SetNumTexCoords(obj.mesh.vertexFormat, usageIndex + 1);
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.mesh.vertices[v].uv[usageIndex] = UnpackVector2(t, vd);
                        }
                    }
                }
                else if (usage == 'c') {
                    if (usageIndex <= 7) {
                        SetNumColors(obj.mesh.vertexFormat, usageIndex + 1);
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.mesh.vertices[v].colors[usageIndex] = UnpackColor(t, vd);
                        }
                    }
                }
                else if (usage == 'i') {
                    if (usageIndex <= 1) {
                        SetNumBones(obj.mesh.vertexFormat, (usageIndex + 1) * 4);
                        if (t == dt_4u8 && (options.game == "fifa16pc" || options.game == "fifa15pc"))
                            t = dt_4u16;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            array<float, 4> joints = UnpackVertexAttribute(t, vd);
                            for (uint32_t bi = 0; bi < 4; bi++) {
                                obj.mesh.vertices[v].boneIndices[usageIndex * 4 + bi] =
                                    (uint16_t)joints[bi];
                            }
                        }
                    }
                }
                else if (usage == 'w') {
                    if (usageIndex <= 1) {
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            array<float, 4> weights = UnpackVertexAttribute(t, vd);
                            for (uint32_t bi = 0; bi < 4; bi++) {
                                obj.mesh.vertices[v].boneWeights[usageIndex * 4 + bi] =
                                    weights[bi];
                            }
                        }
                    }
                }
            }
            if (is == 1 || is == 2 || is == 4) {
                auto GetIndex = [](const void *buffer, uint32_t index, uint8_t stride) -> uint32_t {
                    auto base = static_cast<const uint8_t *>(buffer) + index * stride;
                    switch (stride) {
                    case 1: return *reinterpret_cast<const uint8_t *>(base);
                    case 2: return *reinterpret_cast<const uint16_t *>(base);
                    case 4: return *reinterpret_cast<const uint32_t *>(base);
                    }
                    return 0;
                };
                auto &triangles = obj.mesh.triangles;
                if (primType == RX3_PRIM_TRIANGLELIST) {
                    triangles.resize(numIndices / 3);
                    for (size_t t = 0; t < triangles.size(); ++t) {
                        uint32_t i0 = GetIndex(ib, t * 3 + 0, is);
                        uint32_t i1 = GetIndex(ib, t * 3 + 1, is);
                        uint32_t i2 = GetIndex(ib, t * 3 + 2, is);
                        triangles[t] = { i0, i1, i2 };
                    }
                }
                else if (primType == RX3_PRIM_TRIANGLESTRIP) {
                    if (numIndices >= 3) {
                        for (size_t k = 0; k + 2 < numIndices; ++k) {
                            uint32_t i0 = GetIndex(ib, k + 0, is);
                            uint32_t i1 = GetIndex(ib, k + 1, is);
                            uint32_t i2 = GetIndex(ib, k + 2, is);
                            std::array<uint32_t, 3> tri;
                            if ((k & 1) == 0)
                                tri = { i0, i1, i2 };
                            else
                                tri = { i1, i0, i2 };
                            if (tri[0] == tri[1] || tri[1] == tri[2] || tri[0] == tri[2])
                                continue;
                            triangles.push_back(tri);
                        }
                    }
                }
            }
        }
    }
}

Model ModelFromSimpleMeshContainer(Rx3Container &rx3, path const &rx3path, Rx3Options const &options) {
    using namespace helper::rx3model;
    Model model;
    auto indexBufferChunks = rx3.FindAllChunks(RX3_CHUNK_INDEX_BUFFER);
    auto vertexBufferChunks = rx3.FindAllChunks(RX3_CHUNK_VERTEX_BUFFER);
    auto vertexFormatChunks = rx3.FindAllChunks(RX3_CHUNK_VERTEX_FORMAT);
    auto meshChunks = rx3.FindAllChunks(RX3_CHUNK_SIMPLE_MESH);
    auto animationSkinChunk = rx3.FindFirstChunk(RX3_CHUNK_ANIMATION_SKIN);
    auto skeletonChunk = rx3.FindFirstChunk(RX3_CHUNK_SKELETON);
    vector<string> boneNames;
    Rx3Container skeletonRx3;
    if (!indexBufferChunks.empty() && indexBufferChunks.size() == vertexBufferChunks.size() &&
        indexBufferChunks.size() == vertexFormatChunks.size()) {
        if (animationSkinChunk && !options.skeletonPath.empty() && exists(options.skeletonPath)) {
            skeletonRx3.Load(options.skeletonPath);
            //auto skeletonAnimationSkinChunk = skeletonRx3.FindFirstChunk(RX3_CHUNK_ANIMATION_SKIN);
            //if (skeletonAnimationSkinChunk)
            //    animationSkinChunk = skeletonAnimationSkinChunk;
            auto skeletonSkeletonChunk = skeletonRx3.FindFirstChunk(RX3_CHUNK_SKELETON);
            if (skeletonSkeletonChunk)
                skeletonChunk = skeletonSkeletonChunk;
            auto skeletonNameTableChunk = skeletonRx3.FindFirstChunk(RX3_CHUNK_NAME_TABLE);
            if (skeletonNameTableChunk) {
                auto names = ExtractNamesFromChunk(skeletonNameTableChunk);
                for (auto const &[id, name] : names) {
                    if (id == RX3_CHUNK_BONE_NAME)
                        boneNames.push_back(name);
                }
            }
        }
        bool hasSkeleton = animationSkinChunk && skeletonChunk;
        vector<string> meshNames;
        auto nameTableChunk = rx3.FindFirstChunk(RX3_CHUNK_NAME_TABLE);
        if (nameTableChunk) {
            auto names = ExtractNamesFromChunk(nameTableChunk);
            for (auto const &[id, name] : names) {
                if (id == RX3_CHUNK_SIMPLE_MESH) {
                    if (name.ends_with(".FxRenderableSimple"))
                        meshNames.push_back(name.substr(0, name.length() - strlen(".FxRenderableSimple")));
                    else
                        meshNames.push_back(name);
                }
            }
        }
        auto numMeshes = indexBufferChunks.size();
        uint32_t numBones = 0;
        vector<uint32_t> primTypes;
        for (auto const &gt : meshChunks) {
            Rx3Reader gtReader(gt);
            primTypes.push_back(gtReader.Read<uint32_t>());
        }
        model.objects.resize(numMeshes);
        if (hasSkeleton) {
            Rx3Reader boneMatricesReader(animationSkinChunk);
            boneMatricesReader.Skip(4);
            numBones = boneMatricesReader.Read<uint32_t>();
            boneMatricesReader.Skip(8);
            if (numBones > 0) {
                auto &bones = model.skeleton.bones;
                bones.resize(numBones);
                for (uint32_t b = 0; b < numBones; b++)
                    bones[b].name = (b < boneNames.size()) ? boneNames[b] : "bone_" + to_string(b);
                vector<std::array<Vector4, 4>> boneInversedMatrices(numBones);
                memcpy(boneInversedMatrices.data(), boneMatricesReader.GetCurrentPtr(), boneInversedMatrices.size() * sizeof(std::array<float, 16>));
                vector<Matrix4x4> fbxBoneInverseMatrices(numBones);
                vector<Matrix4x4> fbxBoneMatrices(numBones);
                for (uint32_t b = 0; b < numBones; b++) {
                    for (uint32_t r = 0; r < 4; r++)
                        fbxBoneInverseMatrices[b].mData[r] = boneInversedMatrices[b][r].ToFbx();
                    bones[b].transform = fbxBoneInverseMatrices[b].Inverse();
                }
                Rx3Reader skeletonReader(skeletonChunk);
                skeletonReader.Skip(16);
                for (uint32_t b = 0; b < numBones; b++) {
                    int16_t parentIndex = -1;
                    if (options.game == "fifa16pc" || options.game == "fifa15pc")
                        parentIndex = skeletonReader.Read<int16_t>();
                    else {
                        parentIndex = skeletonReader.Read<uint8_t>();
                        skeletonReader.Skip(1);
                        if (parentIndex == 255)
                            parentIndex = -1;
                    }
                    if (parentIndex >= 0 && parentIndex < (int32_t)bones.size()) {
                        bones[b].parent = bones[parentIndex].name;
                        bones[b].transform = fbxBoneInverseMatrices[parentIndex] * bones[b].transform;
                    }
                }
            }
        }
        for (size_t i = 0; i < numMeshes; i++) {
            auto &obj = model.objects[i];
            obj.name = i < meshNames.size() ? meshNames[i] : "object_" + to_string(i);
            int primType = i < primTypes.size() ? primTypes[i] : RX3_PRIM_TRIANGLELIST;
            SetupObjectMesh(obj, vertexFormatChunks[i], vertexBufferChunks[i], indexBufferChunks[i], primType, options);
        }
    }
    return model;
}

Model ModelFromSceneContainer(Rx3Container &rx3, path const &rx3path, Rx3Options const &options) {
    using namespace helper::rx3model;
    Model model;
    auto indexBufferChunks = rx3.FindAllChunks(RX3_CHUNK_INDEX_BUFFER);
    auto vertexBufferChunks = rx3.FindAllChunks(RX3_CHUNK_VERTEX_BUFFER);
    auto vertexFormatChunks = rx3.FindAllChunks(RX3_CHUNK_VERTEX_FORMAT);
    auto meshChunks = rx3.FindAllChunks(RX3_CHUNK_SIMPLE_MESH);
    auto sceneInstanceChunks = rx3.FindAllChunks(RX3_CHUNK_SCENE_INSTANCE);
    auto collisionMeshChunks = rx3.FindAllChunks(RX3_CHUNK_COLLISION_TRI_MESH);
    auto sceneLayerChunks = rx3.FindAllChunks(RX3_CHUNK_SCENE_LAYER);
    auto materialSections = rx3.FindAllChunks(RX3_CHUNK_MATERIAL);
    auto locationChunks = rx3.FindAllChunks(RX3_CHUNK_LOCATION);
    path crowdPath;
    if (rx3path.has_parent_path()) {
        auto parentDir = rx3path.parent_path();
        string filename = rx3path.stem().string();
        int stadiumId = -1;
        if (EndsWithNumber(filename, stadiumId) && stadiumId != -1) {
            string crowdFilename = "crowd_" + to_string(stadiumId) + "_1.dat";
            if (exists(parentDir / crowdFilename))
                crowdPath = parentDir / crowdFilename;
            else if (parentDir.has_parent_path() && exists(parentDir.parent_path() / "crowdplacement" / crowdFilename))
                crowdPath = parentDir.parent_path() / "crowdplacement" / crowdFilename;
        }
    }
    if (!sceneLayerChunks.empty()) {
        vector<string> meshNames, locationNames, nodeNames, collisionNames, texNames;
        auto nameTableChunk = rx3.FindFirstChunk(RX3_CHUNK_NAME_TABLE);
        if (nameTableChunk) {
            auto names = ExtractNamesFromChunk(nameTableChunk);
            for (auto const &[id, name] : names) {
                if (id == RX3_CHUNK_SIMPLE_MESH) {
                    if (name.ends_with(".FxRenderableSimple"))
                        meshNames.push_back(name.substr(0, name.length() - strlen(".FxRenderableSimple")));
                    else
                        meshNames.push_back(name);
                }
                else if (id == RX3_CHUNK_LOCATION)
                    locationNames.push_back(name);
                else if (id == RX3_CHUNK_TEXTURE)
                    texNames.push_back(name);
            }
        }
        vector<uint32_t> primTypes;
        for (auto const &gt : meshChunks) {
            Rx3Reader gtReader(gt);
            primTypes.push_back(gtReader.Read<uint32_t>());
        }
        // materials
        if (!materialSections.empty()) {
            model.materials.resize(materialSections.size());
            for (size_t i = 0; i < materialSections.size(); i++) {
                Material &mat = model.materials[i];
                Rx3Reader materialReader(materialSections[i]);
                materialReader.Skip(4);
                auto numMatTextures = materialReader.Read<uint32_t>();
                materialReader.Skip(8);
                string shaderName = materialReader.ReadString();
                mat.name = "Material" + to_string(i + 1) + " [" + shaderName + "]";
                if (numMatTextures > 0) {
                    for (unsigned int t = 0; t < numMatTextures; t++) {
                        string texTypeName = materialReader.ReadString();
                        auto texId = materialReader.Read<uint32_t>();
                        if (texTypeName == "diffuseTexture")
                            mat.texture = texNames[texId];
                        else if (texTypeName == "normalMap")
                            mat.normalMap = texNames[texId];
                        else
                            mat.properties[texTypeName] = texNames[texId];
                    }
                }
            }
        }
        // textures
        if (!texNames.empty()) {
            model.textures.resize(texNames.size());
            for (size_t i = 0; i < texNames.size(); i++) {
                model.textures[i].name = texNames[i];
                model.textures[i].filename = texNames[i] + ".png";
            }
        }
        model.objects.resize(sceneLayerChunks.size());
        for (size_t layerIdx = 0; layerIdx < sceneLayerChunks.size(); layerIdx++) {
            Rx3Reader sceneLayerReader(sceneLayerChunks[layerIdx]);
            sceneLayerReader.Skip(4);
            auto layerType = sceneLayerReader.Read<uint32_t>();
            sceneLayerReader.Skip(8);
            model.objects[layerIdx].name = sceneLayerReader.ReadString();
            auto instanceIdx = sceneLayerReader.Read<uint32_t>(); // or uint16_t?
            if (layerType == 0) { // collision
                if (instanceIdx < collisionMeshChunks.size()) {
                    auto &colObj = model.objects.emplace_back();
                    colObj.parent = model.objects[layerIdx].name;
                    Rx3Reader collisionReader(collisionMeshChunks[instanceIdx]);
                    collisionReader.Skip(16);
                    colObj.name = collisionReader.ReadString();
                    collisionReader.Skip(4);
                    auto numTriangles = collisionReader.Read<uint32_t>();
                    colObj.mesh.triangles.resize(numTriangles);
                    colObj.mesh.vertices.resize(numTriangles * 3);
                    for (uint32_t tri = 0; tri < numTriangles; tri++) {
                        for (uint32_t v = 0; v < 3; v++) {
                            colObj.mesh.triangles[tri][v] = tri * 3 + v;
                            colObj.mesh.vertices[tri * 3 + v].pos = collisionReader.Read<Vector3>();
                        }
                    }
                }
            }
            else { // instance
                if (instanceIdx < sceneInstanceChunks.size()) {
                    Rx3Reader scenInstanceReader(sceneInstanceChunks[instanceIdx]);
                    scenInstanceReader.Skip(16);
                    auto transform = scenInstanceReader.Read<std::array<Vector4, 4>>();
                    for (uint32_t r = 0; r < 4; r++)
                        model.objects[layerIdx].transform.mData[r] = transform[r].ToFbx();
                    scenInstanceReader.Skip(32);
                    auto numMeshes = scenInstanceReader.Read<uint32_t>();
                    scenInstanceReader.Skip(4);
                    for (uint32_t m = 0; m < numMeshes; m++) {
                        scenInstanceReader.Skip(32);
                        auto meshIdx = scenInstanceReader.Read<uint32_t>();
                        auto materialIdx = scenInstanceReader.Read<uint32_t>();
                        if (meshIdx < indexBufferChunks.size() && meshIdx < vertexBufferChunks.size() && meshIdx < vertexFormatChunks.size()) {
                            auto &meshObj = model.objects.emplace_back();
                            meshObj.name = meshIdx < meshNames.size() ? meshNames[meshIdx] : "object_" + to_string(model.objects.size());
                            meshObj.parent = model.objects[layerIdx].name;
                            int primType = meshIdx < primTypes.size() ? primTypes[meshIdx] : RX3_PRIM_TRIANGLELIST;
                            SetupObjectMesh(meshObj, vertexFormatChunks[meshIdx], vertexBufferChunks[meshIdx], indexBufferChunks[meshIdx], primType, options);
                            if (materialIdx < model.materials.size())
                                meshObj.mesh.material = model.materials[materialIdx].name;
                        }
                    }
                }
            }
        }
        if (!locationChunks.empty()) {
            auto &locationsRootObj = model.objects.emplace_back();
            locationsRootObj.name = "Locations";
            for (size_t i = 0; i < locationChunks.size(); i++) {
                Rx3Reader locationReader(locationChunks[i]);
                locationReader.Skip(4);
                auto pos = locationReader.Read<Vector3>();
                auto rot = locationReader.Read<Vector3>();
                auto &locationObj = model.objects.emplace_back();
                locationObj.name = i < locationNames.size() ? locationNames[i] : "location_" + to_string(i + 1);
                locationObj.parent = "Locations";
                locationObj.transform.SetIdentity();
                const double radToDeg = 180.0 / M_PI;
                FbxVector4 rotDegrees = FbxVector4((double)rot.x * radToDeg, (double)rot.y * radToDeg, (double)rot.z * radToDeg);
                locationObj.transform.SetR(rotDegrees);
                locationObj.transform.SetT(pos.ToFbx());
            }
        }
        if (!crowdPath.empty()) {
            FILE *crowdFile = nullptr;
#pragma pack (push, 1)
            struct CrowdHeader {
                uint32_t magic;
                uint16_t version;
                uint32_t numseats;
            };

            struct SeatBlob_0x0103 {
                float x, y, z;
                float rotation;
                uint8_t seatcolor[3];
                uint8_t section;
                uint8_t tier;
                uint8_t attendance;
                uint8_t influencearea;
                uint8_t unused3;
                float shade[4];
                uint8_t unused_animgroups;
                uint8_t unused_numaccs;
            };

            struct SeatBlob_0x0104 {
                float x, y, z;
                float rotation;
                uint8_t seatcolor[3];
                uint8_t section0;
                uint8_t section1;
                uint8_t unknown1;
                uint8_t unknown2;
                uint8_t tier;
                uint8_t attendance;
                float shade[4];
            };

            struct SeatBlob_0x0105 {
                float x, y, z;
                float rotation;
                uint8_t seatcolor[3];
                uint8_t section0;
                uint8_t section1;
                uint8_t tier;
                uint8_t attendance;
                uint8_t nochair;
                uint8_t cardcolors[3];
                uint8_t crowdpattern;
                uint8_t pad[4];
            };
#pragma pack (pop)
            _wfopen_s(&crowdFile, crowdPath.c_str(), L"rb");
            if (crowdFile) {
                CrowdHeader header {};
                fread(&header, sizeof(CrowdHeader), 1, crowdFile);
                if (header.magic == 'DWRC') {
                    auto &crowdObj = model.objects.emplace_back();
                    crowdObj.name = "Crowd";
                    std::map<uint8_t, Object> tiers;
                    for (uint32_t i = 0; i < header.numseats; i++) {
                        Vector3 pos;
                        float angle = 0.0f;
                        RGBA seatcolor(0, 0, 0, 0), shade(0, 0, 0, 0), cardcolors(0, 0, 0, 0);
                        uint8_t tier = 0, section0 = 0, section1 = 0, attendance = 0, nochair = 0, crowdpattern = 0;
                        if (header.version == 0x103) {
                            SeatBlob_0x0103 seat;
                            fread(&seat, sizeof(SeatBlob_0x0103), 1, crowdFile);
                            pos.Set(seat.x, seat.y, seat.z);
                            angle = seat.rotation;
                            seatcolor.Set(seat.seatcolor[0], seat.seatcolor[1], seat.seatcolor[2], 255);
                            shade.Set(seat.shade[0], seat.shade[1], seat.shade[2], seat.shade[3]);
                            section0 = seat.section;
                            tier = seat.tier;
                            attendance = seat.attendance;
                        }
                        else if (header.version == 0x104) {
                            SeatBlob_0x0104 seat;
                            fread(&seat, sizeof(SeatBlob_0x0104), 1, crowdFile);
                            pos.Set(seat.x, seat.y, seat.z);
                            angle = seat.rotation;
                            seatcolor.Set(seat.seatcolor[0], seat.seatcolor[1], seat.seatcolor[2], 255);
                            shade.Set(seat.shade[0], seat.shade[1], seat.shade[2], seat.shade[3]);
                            section0 = seat.section0;
                            section1 = seat.section1;
                            tier = seat.tier;
                            attendance = seat.attendance;
                        }
                        else if (header.version == 0x105) {
                            SeatBlob_0x0105 seat;
                            fread(&seat, sizeof(SeatBlob_0x0105), 1, crowdFile);
                            pos.Set(seat.x, seat.y, seat.z);
                            angle = seat.rotation;
                            seatcolor.Set(seat.seatcolor[0], seat.seatcolor[1], seat.seatcolor[2], 255);
                            section0 = seat.section0;
                            section1 = seat.section1;
                            tier = seat.tier;
                            attendance = seat.attendance;
                            nochair= seat.nochair;
                            cardcolors = RGBA(seat.cardcolors[0], seat.cardcolors[1], seat.cardcolors[2], 255);
                            crowdpattern = seat.crowdpattern;
                        }
                        else
                            continue;
                        Object &obj = tiers[tier];
                        size_t vertIndex = obj.mesh.vertices.size();
                        size_t triIndex = obj.mesh.triangles.size();
                        obj.mesh.vertices.resize(vertIndex + 4);
                        obj.mesh.triangles.resize(triIndex + 4);
                        const float SeatScale = 30.0f;
                        FbxVector4 rect[4] = {
                        { -SeatScale, 0, 0, 1 },
                        { -SeatScale, SeatScale * 2, 0, 1 },
                        {  SeatScale, SeatScale * 2, 0, 1 },
                        {  SeatScale, 0, 0, 1 }
                        };
                        FbxAMatrix mat;
                        mat.SetIdentity();
                        mat.SetR(FbxVector4(0.0, angle + 90.0, 0.0, 0.0));
                        mat.SetT(pos.ToFbx());
                        for (size_t v = 0; v < 4; v++) {
                            FbxVector4 globalPos = mat.MultT(rect[v]);
                            obj.mesh.vertices[vertIndex + v].pos = FbxDouble3(globalPos[0], globalPos[1], globalPos[2]);
                            obj.mesh.vertices[vertIndex + v].colors[0] = seatcolor;
                            obj.mesh.vertices[vertIndex + v].colors[1] = shade;
                            obj.mesh.vertices[vertIndex + v].colors[2] = RGBA(section0, section0, section0, 255);
                            obj.mesh.vertices[vertIndex + v].colors[3] = RGBA(section1, section1, section1, 255);
                            obj.mesh.vertices[vertIndex + v].colors[4] = RGBA(attendance, attendance, attendance, 255);
                            obj.mesh.vertices[vertIndex + v].colors[5] = RGBA(nochair, nochair, nochair, 255);
                            obj.mesh.vertices[vertIndex + v].colors[6] = cardcolors;
                            obj.mesh.vertices[vertIndex + v].colors[7] = RGBA(crowdpattern, crowdpattern, crowdpattern, 255);
                        }
                        obj.mesh.triangles[triIndex] = { vertIndex + 2, vertIndex + 1, vertIndex + 0 };
                        obj.mesh.triangles[triIndex + 1] = { vertIndex + 0, vertIndex + 3, vertIndex + 2 };
                    }
                    std::string layerNames[] = { "SeatColor", "Shade", "NeutralHomeAway", "UltraHomeAway", "Attendance", "NoChair",
                        "CardColors", "CrowdPattern"};
                    for (auto &[tier, obj] : tiers) {
                        obj.name = "tier_" + std::to_string(tier);
                        obj.parent = "Crowd";
                        obj.mesh.vertexFormat = V_8Colors;
                        for (size_t l = 0; l < std::size(layerNames); l++)
                            obj.mesh.colorLayerNames[l] = layerNames[l];
                        model.objects.push_back(obj);
                    }
                }
                fclose(crowdFile);
            }
        }
    }
    return model;
}

Model ModelFromRX3(path const &rx3path, Rx3Options const &options) {
    Rx3Container rx3(rx3path);
    if (rx3.FindFirstChunk(RX3_CHUNK_SCENE_INSTANCE))
        return ModelFromSceneContainer(rx3, rx3path, options);
    else if (rx3.FindFirstChunk(RX3_CHUNK_SIMPLE_MESH))
        return ModelFromSimpleMeshContainer(rx3, rx3path, options);
    return Model();
}
