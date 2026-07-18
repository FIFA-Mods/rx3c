#include "Rx3Skeleton.h"
#include "Rx3Container.h"
#include "Rx3Names.h"
#include "Rx3Model.h"

void ModelToSkeletonContainer(Model const &model, path const &rx3path, path const &baseSkeleton, Rx3Options const &options) {
    using namespace helper::rx3model;
    if (!exists(baseSkeleton))
        return;
    Rx3Container rx3(baseSkeleton);
    Rx3Chunk *skinChunk = rx3.FindFirstChunk(RX3_CHUNK_ANIMATION_SKIN);
    if (!skinChunk)
        return;
    Rx3Reader skinReader(skinChunk);
    skinReader.Skip(4);
    uint32_t numBones = skinReader.Read<uint32_t>();
    skinReader.Skip(8);
    auto boneNames = ExtractNamesFromRx3(rx3, RX3_CHUNK_BONE_NAME);
    if (numBones > 0 && numBones == boneNames.size()) {
        vector<Matrix4x4> matrices(numBones);
        for (size_t i = 0; i < numBones; i++)
            ReadMatrix4x4(skinReader, matrices[i]);
        for (size_t i = 0; i < numBones; i++) {
            int boneIndex = model.GetBoneIndex(boneNames[i]);
            if (boneIndex >= 0)
                matrices[i] = model.skeleton.bones[boneIndex].matrix;
        }
        skinChunk->mData.resize(16);
        Rx3Writer skinWriter(skinChunk);
        for (size_t i = 0; i < numBones; i++)
            WriteMatrix4x4(skinWriter, matrices[i]);
    }
    rx3.Save(rx3path);
}
