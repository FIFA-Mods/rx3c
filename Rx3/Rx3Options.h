#pragma once
#include "rx3utils.h"
#include "Model.h"
#include "Rx3Container.h"

extern const char *RX3C_VERSION;

enum eFolderOption {
    FOLDER_OPTION_AUTO,
    FOLDER_OPTION_ALWAYS_CREATE,
    FOLDER_OPTION_NEVER_CREATE
};

enum eBoneMatricesOption {
    BONE_MATRICES_FROM_FILE,
    BONE_MATRICES_FROM_SKELETON,
    BONE_MATRICES_FROM_BASE_MODEL
};

struct TexFormatTarget {
    int format = -1;
    char levels = 0;
    unsigned short width = 0;
    unsigned short height = 0;
};

enum eSkinPaletteOpcodesPolicty {
    SKIN_PALETTE_OPCODES_ALWAYS,
    SKIN_PALETTE_OPCODES_NEVER,
    SKIN_PALETTE_OPCODES_16BIT_BONE_IDS
};

struct GameConfig {
    bool BigEndian = false;
    unsigned char MaxBonesPerVertex = 0;
    unsigned int MaxBonesPerMesh = 0;
    bool TextureRasterSuffix = false;
    bool QuadMeshes = false;
    bool StadiumTexturesAndModelInOneContainer = false;
    eSkinPaletteOpcodesPolicty SkinPaletteOpcodesPolicy = SKIN_PALETTE_OPCODES_ALWAYS;
    map<unsigned char, unsigned char> TextureFormats;

    GameConfig();
    GameConfig(bool _BigEndian, unsigned char _MaxBonesPerVertex, unsigned int _MaxBonesPerMesh, bool _TextureRasterSuffix,
        bool _QuadMeshes, bool _StadiumTexturesAndModelInOneContainer, eSkinPaletteOpcodesPolicty _SkinPaletteOpcodesPolicy,
        map<unsigned char, unsigned char> const &_TextureFormats);
};

map<string, GameConfig> &GameConfigs();

void AddMetadataToRx3(Rx3Container &rx3, path const &in, path const &out, string const &cmdLine);

struct Rx3Options {
    string cmdLine;
    string game;
    string textureFormat;
    string modelFormat;
    bool exportQuads;
    bool writeHDR;
    bool writeTexMetadata;
    bool tristrip;
    bool precisePositions;
    bool metadata;
    eFolderOption folderOption;
    eBoneMatricesOption boneMatricesOption;
    GameConfig gameConfig;
    map<string, TexFormatTarget> texTargetFormats;
    Model baseModel;
    Skeleton targetSkeleton;

    Rx3Options();
    Rx3Options(string const &gameName);
};
