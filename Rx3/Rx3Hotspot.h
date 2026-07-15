#pragma once
#include "Rx3Container.h"
#include "Rx3Options.h"
#include <filesystem>

void ExtractHotspotFromRX3(Rx3Container &container, std::filesystem::path const &outputDir, Rx3Options const &rx3options);
void ImportHotspotToRX3(Rx3Container &container, std::filesystem::path const &hotspotFile, Rx3Options const &rx3options);
