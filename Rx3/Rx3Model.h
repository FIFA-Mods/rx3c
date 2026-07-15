#pragma once
#include "Model.h"
#include "Rx3Options.h"
#include "Rx3Container.h"
#include <filesystem>

void ExtractModelFromRX3(Rx3Container &container, std::filesystem::path const &outputDir, Rx3Options const &rx3options);
//void ImportModelToRX3(Rx3Container &container, std::filesystem::path const &hotspotFile, Rx3Options const &rx3options);
