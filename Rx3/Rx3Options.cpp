#include "Rx3Options.h"
#include "Rx3Container.h"

std::map<std::string, GameConfig> &GameConfigs() {
    static std::map<std::string, GameConfig> configs = {
        { "fifa12pc", GameConfig(
            true,  // BigEndian
            4,     // BonesPerVertex
            255,   // MaxBones
            true,  // TextureRasterSuffix
            false, // QuadMeshes
            true,  // StadiumTexturesAndModelInOneContainer
            {
                { RX3_TEXFORMAT_DXT1, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_DXT3, RX3_TEXFORMAT_DXT3 },
                { RX3_TEXFORMAT_DXT5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_ARGB8888, RX3_TEXFORMAT_ARGB8888 },
                { RX3_TEXFORMAT_L8, RX3_TEXFORMAT_L8 },
                { RX3_TEXFORMAT_AL8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_RG8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_BC5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_RGB565, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_ARGB4444, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC6, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC7, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_BC4, RX3_TEXFORMAT_L8 }
            }
        )},
        { "fifa13pc", GameConfig(
            true,  // BigEndian
            4,     // BonesPerVertex
            255,   // MaxBones
            true,  // TextureRasterSuffix
            false, // QuadMeshes
            true,  // StadiumTexturesAndModelInOneContainer
            {
                { RX3_TEXFORMAT_DXT1, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_DXT3, RX3_TEXFORMAT_DXT3 },
                { RX3_TEXFORMAT_DXT5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_ARGB8888, RX3_TEXFORMAT_ARGB8888 },
                { RX3_TEXFORMAT_L8, RX3_TEXFORMAT_L8 },
                { RX3_TEXFORMAT_AL8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_RG8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_BC5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_RGB565, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_ARGB4444, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC6, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC7, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_BC4, RX3_TEXFORMAT_L8 }
            }
        )},
        { "fifa14pc", GameConfig(
            false, // BigEndian
            4,     // BonesPerVertex
            255,   // MaxBones
            true,  // TextureRasterSuffix
            false, // QuadMeshes
            false, // StadiumTexturesAndModelInOneContainer
            {
                { RX3_TEXFORMAT_DXT1, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_DXT3, RX3_TEXFORMAT_DXT3 },
                { RX3_TEXFORMAT_DXT5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_ARGB8888, RX3_TEXFORMAT_ARGB8888 },
                { RX3_TEXFORMAT_L8, RX3_TEXFORMAT_L8 },
                { RX3_TEXFORMAT_AL8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_RG8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_BC5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_RGB565, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_ARGB4444, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC6, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC7, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_BC4, RX3_TEXFORMAT_L8 }
            }
        )},
        { "fifa15pc", GameConfig(
            false, // BigEndian
            4,     // BonesPerVertex
            512,   // MaxBones
            false, // TextureRasterSuffix
            false, // QuadMeshes
            false, // StadiumTexturesAndModelInOneContainer
            {
                { RX3_TEXFORMAT_DXT1, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_DXT3, RX3_TEXFORMAT_DXT3 },
                { RX3_TEXFORMAT_DXT5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_ARGB8888, RX3_TEXFORMAT_ARGB8888 },
                { RX3_TEXFORMAT_L8, RX3_TEXFORMAT_L8 },
                { RX3_TEXFORMAT_AL8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_RG8, RX3_TEXFORMAT_RG8 },
                { RX3_TEXFORMAT_BC5, RX3_TEXFORMAT_BC5 },
                { RX3_TEXFORMAT_RGB565, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_ARGB4444, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC6, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC7, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_BC4, RX3_TEXFORMAT_L8 }
            }
        )},
        { "fifa16pc", GameConfig(
            false, // BigEndian
            4,     // BonesPerVertex
            512,   // MaxBones
            false, // TextureRasterSuffix
            true,  // QuadMeshes
            false, // StadiumTexturesAndModelInOneContainer
            {
                { RX3_TEXFORMAT_DXT1, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_DXT3, RX3_TEXFORMAT_DXT3 },
                { RX3_TEXFORMAT_DXT5, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_ARGB8888, RX3_TEXFORMAT_ARGB8888 },
                { RX3_TEXFORMAT_L8, RX3_TEXFORMAT_L8 },
                { RX3_TEXFORMAT_AL8, RX3_TEXFORMAT_AL8 },
                { RX3_TEXFORMAT_RG8, RX3_TEXFORMAT_RG8 },
                { RX3_TEXFORMAT_BC5, RX3_TEXFORMAT_BC5 },
                { RX3_TEXFORMAT_RGB565, RX3_TEXFORMAT_DXT1 },
                { RX3_TEXFORMAT_ARGB4444, RX3_TEXFORMAT_DXT5 },
                { RX3_TEXFORMAT_BC6, RX3_TEXFORMAT_BC6 },
                { RX3_TEXFORMAT_BC7, RX3_TEXFORMAT_BC7 },
                { RX3_TEXFORMAT_BC4, RX3_TEXFORMAT_L8 }
            }
        )}
    };
    return configs;
}

GameConfig::GameConfig() {}

GameConfig::GameConfig(bool _BigEndian, unsigned char _BonesPerVertex, unsigned int _MaxBones, bool _TextureRasterSuffix, bool _QuadMeshes, bool _StadiumTexturesAndModelInOneContainer, std::map<unsigned char, unsigned char> const &_TextureFormats) {
    BigEndian = _BigEndian;
    BonesPerVertex = _BonesPerVertex;
    MaxBones = _MaxBones;
    TextureRasterSuffix = _TextureRasterSuffix;
    QuadMeshes = _QuadMeshes;
    StadiumTexturesAndModelInOneContainer = _StadiumTexturesAndModelInOneContainer;
    TextureFormats = _TextureFormats;
}
