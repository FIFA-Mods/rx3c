#pragma once
#include "Model.h"
#include "Rx3Container.h"
#include "Rx3Options.h"

Model ModelFromSkeletonContainer(Rx3Container &rx3, Rx3Options const &options);
void ModelToSkeletonContainer(Model const &model, path const &rx3path, path const &baseSkeleton, Rx3Options const &options);
