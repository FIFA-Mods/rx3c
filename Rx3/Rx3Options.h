#pragma once
#include "rx3utils.h"

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
    map<unsigned char, unsigned char> TextureFormats;

    GameConfig();
    GameConfig(bool _BigEndian, unsigned char _BonesPerVertex, unsigned int _MaxBones, bool _TextureRasterSuffix,
        bool _QuadMeshes, bool _StadiumTexturesAndModelInOneContainer,
        map<unsigned char, unsigned char> const &_TextureFormats);
};

map<string, GameConfig> &GameConfigs();

struct Rx3Options {
    string game;
    string textureFormat;
    string modelFormat;
    path skeletonPath;
    path baseModel;
    map<string, TexFormatTarget> texTargetFormats;
    bool exportQuads;
    bool writeHDR;
    bool writeTexMetadata;
    eFolderOption folderOption;
    GameConfig gameConfig;

    Rx3Options();
    Rx3Options(string const &gameName);
};
