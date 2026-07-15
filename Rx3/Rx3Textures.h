#pragma once
#include "Rx3Container.h"
#include "Rx3Options.h"
#include <filesystem>
#include <map>

struct PackedTextureInfo {
    std::string name;
    std::filesystem::path filePath;
    int format = -1;
    char levels = 0;
    unsigned short width = 0;
    unsigned short height = 0;

    PackedTextureInfo();
    PackedTextureInfo(std::filesystem::path const &_filePath);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format, char _levels);
    PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format, char _levels, unsigned short _width, unsigned short _height);
};

int TexFormatNameToID(std::string const &name);
void ReadTexFormatFile(std::filesystem::path const &filePath, std::map<std::string, TexFormatTarget> &out,
    std::vector<std::string> &outOrder);

void ExtractTexturesFromRX3(Rx3Container &container, std::filesystem::path const &outputDir, Rx3Options const &rx3options);
bool ImportTexturesToRX3(Rx3Container &rx3, std::vector<PackedTextureInfo> const &inTextures, Rx3Options const &rx3options,
    bool withoutNames = false);
bool ImportTexturesToRX3(Rx3Container &rx3, std::vector<std::filesystem::path> const &inTextures,
    std::filesystem::path const &localMetadataFile, Rx3Options const &rx3options, bool withoutNames = false);
