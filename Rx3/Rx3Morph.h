#pragma once
#include "Rx3Container.h"
#include "Rx3Options.h"
#include "Model.h"

Model ModelFromMorphTargetsContainer(Rx3Container &rx3, path const &baseModelPath, Rx3Options const &options);
void ModelToMorphTargetsContainer(Model const &model, path const &rx3path, path const &baseModelPath, Rx3Options const &options);
