#pragma once
#include <string>
#include <filesystem>
#include "Rx3Container.h"
#include "..\rx3utils.h"
#include "..\memory.h"
#include <map>

struct PackedTextureInfo {
    std::string name;
    std::filesystem::path filePath;
    char format = -1;
    char levels = -1;
    unsigned short width = 0;
    unsigned short height = 0;

    PackedTextureInfo();
    PackedTextureInfo(std::filesystem::path const &_filePath);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format, char _levels);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format, char _levels, unsigned short _width, unsigned short _height);
};

bool PackTexturesToRx3Container(std::vector<PackedTextureInfo> const &textures, std::filesystem::path const &rx3path);
bool PackFolderTexturesToRx3Container(std::filesystem::path const &folderPath, std::filesystem::path const &rx3path);
