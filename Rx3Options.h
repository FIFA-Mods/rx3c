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

struct Rx3Options {
    std::string game;
    std::string textureFormat;
    std::string modelFormat;
    std::filesystem::path skeletonPath;
    std::map<std::string, TexFormatTarget> texTargetFormats;
    bool exportQuads = false;
    bool writeHDR = true;
    bool writeTexMetadata = true;
    eFolderOption folderOption;
};
