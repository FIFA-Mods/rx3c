#pragma once
#include "Model.h"
#include "Rx3Options.h"
#include <filesystem>

Model ModelFromRX3(std::filesystem::path const &rx3path, Rx3Options const &options);
