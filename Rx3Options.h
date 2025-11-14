#pragma once
#include <string>
#include <filesystem>

struct Rx3Options {
    std::string game;
    std::string textureFormat;
    std::string modelFormat;
    std::filesystem::path skeletonPath;
};
