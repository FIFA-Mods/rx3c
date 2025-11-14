#pragma once
#include "Rx3Container.h"
#include "Rx3Options.h"
#include <set>

void ExtractTexturesFromContainer(Rx3Container &container, path const &outputDir, Rx3Options const &rx3options);
void ExtractTexturesFromRX3(path const &rx3path, path const &outputPath, Rx3Options const &rx3options);
