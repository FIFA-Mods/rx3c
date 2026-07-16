#include "Rx3Model.h"
#include "Rx3Container.h"
#include "Rx3Names.h"
#include "Rx3Scene.h"
#include "half.hpp"

namespace helper::rx3model {

char const *DataTypeNames[] = {
    "unknown", "void", "1f32", "1s32", "1s16", "1s8", "2f32", "2s32", "2s16", "2s8", "3f32", "3s32",
    "3s16", "3s8", "4f32", "4s32", "4s16", "4s8", "4u8", "4u8n", "4u8endianswapp", "4u8nendianswap",
    "2s16n", "4s16n", "3u10", "3s10n", "3s11n", "2f16", "4f16", "2s16s", "3s16s", "1u16rgb565",
    "3u8rgb8", "4u8rgbx8", "1u16rgba4", "3u8rgba6", "4u8rgba8", "2u16", "4u16", "2u16n", "4u16n", "custom"
};

DataType DataTypeIdFromName(string const &name) {
    for (size_t i = 0; i < size(DataTypeNames); i++) {
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
        (unsigned char)clamp(unpacked[0] * 255.0f, 0.0f, 255.0f),
        (unsigned char)clamp(unpacked[1] * 255.0f, 0.0f, 255.0f),
        (unsigned char)clamp(unpacked[2] * 255.0f, 0.0f, 255.0f),
        (unsigned char)clamp(unpacked[2] * 255.0f, 0.0f, 255.0f));
}

Matrix4x4 ReadMatrix4x4(Rx3Reader &reader) {
    Matrix4x4 mat;
    for (uint32_t r = 0; r < 4; r++) {
        for (uint32_t c = 0; c < 4; c++)
            mat.m[r][c] = reader.Read<float>();
    }
    return mat;
}

void ReadMatrix4x4(Rx3Reader &reader, Matrix4x4 &out) {
    out = ReadMatrix4x4(reader);
}

Vector3 ReadVector3(Rx3Reader &reader) {
    Vector3 v;
    for (uint32_t i = 0; i < 3; i++)
        v[i] = reader.Read<float>();
    return v;
}

void ReadVector3(Rx3Reader &reader, Vector3 &out) {
    out = ReadVector3(reader);
}
}

void SetupObjectMesh(Object &obj, Rx3Chunk *vfChunk, Rx3Chunk *vbChunk, Rx3Chunk *ibChunk, Rx3Chunk *qibChunk, int primType,
    Rx3Options const &options)
{
    using namespace helper::rx3model;
    obj.vertexFormat = 0;
    Rx3Reader vertexDeclReader(vfChunk);
    Rx3Reader vertexBufferReader(vbChunk);
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
            obj.vertices.resize(numVertices);
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
                            obj.vertices[v].pos = UnpackVector3(t, vd) / 100.0f;
                        }
                    }
                }
                else if (usage == 'n') {
                    if (usageIndex == 0) {
                        obj.vertexFormat |= V_Normal;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.vertices[v].normal = UnpackVector3(t, vd);
                        }
                    }
                }
                else if (usage == 'g') {
                    if (usageIndex == 0) {
                        obj.vertexFormat |= V_Tangent;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.vertices[v].tangent = UnpackVector3(t, vd);
                        }
                    }
                }
                else if (usage == 'b') {
                    if (usageIndex == 0) {
                        obj.vertexFormat |= V_Binormal;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.vertices[v].binormal = UnpackVector3(t, vd);
                        }
                    }
                }
                else if (usage == 't') {
                    if (usageIndex <= 7) {
                        SetNumTexCoords(obj.vertexFormat, usageIndex + 1);
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.vertices[v].uv[usageIndex] = UnpackVector2(t, vd);
                            obj.vertices[v].uv[usageIndex].y = 1.0f - obj.vertices[v].uv[usageIndex].y;
                        }
                    }
                }
                else if (usage == 'c') {
                    if (usageIndex <= 7) {
                        SetNumColors(obj.vertexFormat, usageIndex + 1);
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            obj.vertices[v].colors[usageIndex] = UnpackColor(t, vd);
                        }
                    }
                }
                else if (usage == 'i') {
                    if (usageIndex <= 1) {
                        SetNumBones(obj.vertexFormat, (usageIndex + 1) * 4);
                        if (t == dt_4u8 && options.gameConfig.MaxBones > 255)
                            t = dt_4u16;
                        for (uint32_t v = 0; v < numVertices; v++) {
                            const unsigned char *vd = (const unsigned char *)vb + v * vs + offset;
                            array<float, 4> joints = UnpackVertexAttribute(t, vd);
                            for (uint32_t bi = 0; bi < 4; bi++) {
                                obj.vertices[v].boneIndices[usageIndex * 4 + bi] =
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
                                obj.vertices[v].boneWeights[usageIndex * 4 + bi] =
                                    weights[bi];
                            }
                        }
                    }
                }
            }
            auto ReadIndex = [](Rx3Reader &reader, uint8_t stride) -> uint32_t {
                if (stride == 1)
                    return reader.Read<uint8_t>();
                else if (stride == 2)
                    return reader.Read<uint16_t>();
                else if (stride == 4)
                    return reader.Read<uint32_t>();
                return 0;
            };
            if (options.exportQuads && qibChunk) {
                Rx3Reader ibReader(qibChunk);
                ibReader.Skip(4);
                uint32_t numIndices = ibReader.Read<uint32_t>();
                uint8_t is = ibReader.Read<uint8_t>();
                ibReader.Skip(7);
                if (is == 1 || is == 2 || is == 4) {
                    auto &quads = obj.meshes.emplace_back().quads;
                    quads.resize(numIndices / 4);
                    for (size_t t = 0; t < quads.size(); ++t)
                        quads[t] = { ReadIndex(ibReader, is), ReadIndex(ibReader, is), ReadIndex(ibReader, is), ReadIndex(ibReader, is) };
                }
            }
            else {
                Rx3Reader ibReader(ibChunk);
                ibReader.Skip(4);
                uint32_t numIndices = ibReader.Read<uint32_t>();
                uint8_t is = ibReader.Read<uint8_t>();
                ibReader.Skip(7);
                if (is == 1 || is == 2 || is == 4) {
                    auto &triangles = obj.meshes.emplace_back().triangles;
                    if (primType == RX3_PRIM_TRIANGLELIST) {
                        triangles.resize(numIndices / 3);
                        for (size_t t = 0; t < triangles.size(); ++t)
                            triangles[t] = { ReadIndex(ibReader, is), ReadIndex(ibReader, is), ReadIndex(ibReader, is) };
                    }
                    else if (primType == RX3_PRIM_TRIANGLESTRIP) {
                        if (numIndices >= 3) {
                            for (size_t k = 0; k + 2 < numIndices; ++k) {
                                uint32_t i0 = ReadIndex(ibReader, is);
                                uint32_t i1 = ReadIndex(ibReader, is);
                                uint32_t i2 = ReadIndex(ibReader, is);
                                array<uint32_t, 3> tri;
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
}

Model ModelFromSimpleMeshContainer(Rx3Container &rx3, Rx3Options const &options) {
    using namespace helper::rx3model;
    Model model;
    auto indexBufferChunks = rx3.FindAllChunks(RX3_CHUNK_INDEX_BUFFER);
    auto quadBufferChunks = rx3.FindAllChunks(RX3_CHUNK_QUAD_INDEX_BUFFER);
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
            primTypes.push_back(gtReader.Read<uint16_t>());
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
                vector<Matrix4x4> boneInversedMatrices(numBones);
                for (uint32_t b = 0; b < numBones; b++) {
                    ReadMatrix4x4(boneMatricesReader, boneInversedMatrices[b]);
                    for (uint32_t j = 0; j < 3; j++)
                        boneInversedMatrices[b].m[3][j] /= 100.0f;
                    bones[b].matrix = boneInversedMatrices[b].Inversed();
                }
                Rx3Reader skeletonReader(skeletonChunk);
                skeletonReader.Skip(16);
                for (uint32_t b = 0; b < numBones; b++) {
                    int16_t parentIndex = -1;
                    if (options.gameConfig.MaxBones > 255)
                        parentIndex = skeletonReader.Read<int16_t>();
                    else {
                        parentIndex = skeletonReader.Read<uint8_t>();
                        skeletonReader.Skip(1);
                        if (parentIndex == 255)
                            parentIndex = -1;
                    }
                    if (parentIndex >= 0 && parentIndex < (int32_t)bones.size()) {
                        bones[b].parent = bones[parentIndex].name;
                        bones[b].matrix = boneInversedMatrices[parentIndex] * bones[b].matrix;
                    }
                }
            }
        }
        for (size_t i = 0; i < numMeshes; i++) {
            auto &obj = model.objects[i];
            obj.name = i < meshNames.size() ? meshNames[i] : "object_" + to_string(i);
            int primType = i < primTypes.size() ? primTypes[i] : RX3_PRIM_TRIANGLELIST;
            auto quadBuffer = quadBufferChunks.size() == indexBufferChunks.size() ? quadBufferChunks[i] : nullptr;
            SetupObjectMesh(obj, vertexFormatChunks[i], vertexBufferChunks[i], indexBufferChunks[i], quadBuffer, primType, options);
        }
    }
    return model;
}

Model ModelFromRX3(Rx3Container &rx3, Rx3Options const &options) {
    if (rx3.FindFirstChunk(RX3_CHUNK_SCENE_INSTANCE))
        return ModelFromSceneContainer(rx3, options);
    else if (rx3.FindFirstChunk(RX3_CHUNK_SIMPLE_MESH))
        return ModelFromSimpleMeshContainer(rx3, options);
    return Model();
}

void ModelToRX3SimpleMesh(Model const &model, path const &rx3path, Rx3Options const &options) {
    Rx3Container rx3;
    // ibbatch, quadibbatch, vertexformat's, nametable, ib's, qib's, boneremap's, vb's, animationskin's,
    // simplemesh's, adjacency's
    vector<vector<unsigned char>> vbs, ibs, boneremap;

}

void ExtractModelFromRX3(Rx3Container &container, path const &outputDir, Rx3Options const &rx3options) {
    Model m = ModelFromRX3(container, rx3options);
    if (!m.objects.empty() || !m.skeleton.bones.empty()) {
        if (!exists(outputDir))
            create_directories(outputDir);
        m.WriteFbx(outputDir / (container.mName + ".fbx"), rx3options.modelFormat == "fbxascii");
    }
}
