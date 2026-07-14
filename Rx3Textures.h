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
void ReadTexFormatFile(std::filesystem::path const &filePath, std::map<std::string, TexFormatTarget> &out);

void ExtractTexturesFromContainer(Rx3Container &container, std::filesystem::path const &outputDir, Rx3Options const &rx3options);
void ExtractTexturesFromRX3(std::filesystem::path const &rx3path, std::filesystem::path const &outputPath, Rx3Options const &rx3options);
bool PackTexturesToRx3Container(std::vector<PackedTextureInfo> const &textures, std::filesystem::path const &rx3path, std::map<unsigned char, unsigned char> formatConversion);
//bool PackFolderTexturesToRx3Container(std::filesystem::path const &folderPath, std::filesystem::path const &rx3path);
