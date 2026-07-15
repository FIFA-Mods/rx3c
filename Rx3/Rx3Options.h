#pragma once
#include <string>
#include <filesystem>
#include <map>

enum eFolderOption {
    FOLDER_OPTION_AUTO,
    FOLDER_OPTION_ALWAYS_CREATE,
    FOLDER_OPTION_NEVER_CREATE
};

struct TexFormatTarget {
    int format = -1;
    char levels = 0;
    unsigned short width = 0;
    unsigned short height = 0;
};

struct GameConfig {
    bool BigEndian = false;
    unsigned char BonesPerVertex = 0;
    unsigned int MaxBones = 0;
    bool TextureRasterSuffix = false;
    bool QuadMeshes = false;
    bool StadiumTexturesAndModelInOneContainer = false;
    std::map<unsigned char, unsigned char> TextureFormats;

    GameConfig();
    GameConfig(bool _BigEndian, unsigned char _BonesPerVertex, unsigned int _MaxBones, bool _TextureRasterSuffix,
        bool _QuadMeshes, bool _StadiumTexturesAndModelInOneContainer,
        std::map<unsigned char, unsigned char> const &_TextureFormats);
};

std::map<std::string, GameConfig> &GameConfigs();

struct Rx3Options {
    std::string game;
    std::string textureFormat;
    std::string modelFormat;
    std::filesystem::path skeletonPath;
    std::map<std::string, TexFormatTarget> texTargetFormats;
    bool exportQuads = false;
    bool writeHDR = true;
    bool writeTexMetadata = true;
    eFolderOption folderOption = FOLDER_OPTION_AUTO;
    GameConfig gameConfig;
};
