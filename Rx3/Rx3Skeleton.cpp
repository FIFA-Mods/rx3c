#include "Rx3Skeleton.h"
#include "Rx3Container.h"
#include "Rx3Names.h"
#include "Rx3Model.h"

Matrix4x4 ComputeBoneWorldMatrix(Model const &model, int boneIndex,
    vector<Matrix4x4> &worldCache,
    vector<char> &computed) {
    if (computed[boneIndex])
        return worldCache[boneIndex];
    auto const &bone = model.skeleton.bones[boneIndex];
    Matrix4x4 world = bone.matrix;
    if (!bone.parent.empty()) {
        int parentIndex = model.GetBoneIndex(bone.parent);
        if (parentIndex >= 0)
            world = ComputeBoneWorldMatrix(model, parentIndex, worldCache, computed) * bone.matrix;
    }
    worldCache[boneIndex] = world;
    computed[boneIndex] = 1;
    return world;
}

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
        vector<Matrix4x4> worldCache(model.skeleton.bones.size());
        vector<char> computed(model.skeleton.bones.size(), 0);
        for (size_t i = 0; i < numBones; i++) {
            int boneIndex = model.GetBoneIndex(boneNames[i]);
            if (boneIndex >= 0) {
                Matrix4x4 world = ComputeBoneWorldMatrix(model, boneIndex, worldCache, computed);
                matrices[i] = world.Inversed();
                for (uint32_t j = 0; j < 3; j++)
                    matrices[i].m[3][j] *= 100.0f;
            }
        }
        skinChunk->mData.resize(16);
        Rx3Writer skinWriter(skinChunk);
        for (size_t i = 0; i < numBones; i++)
            WriteMatrix4x4(skinWriter, matrices[i]);
    }
    rx3.Save(rx3path);
}

Model ModelFromSkeletonContainer(Rx3Container &rx3, Rx3Options const &options) {
    using namespace helper::rx3model;
    Model model;
    auto animationSkinChunk = rx3.FindFirstChunk(RX3_CHUNK_ANIMATION_SKIN);
    auto skeletonChunk = rx3.FindFirstChunk(RX3_CHUNK_SKELETON);
    if (animationSkinChunk && skeletonChunk) {
        vector<string> boneNames = ExtractNamesFromRx3(rx3, RX3_CHUNK_BONE_NAME);
        Rx3Reader boneMatricesReader(animationSkinChunk);
        boneMatricesReader.Skip(4);
        uint32_t numBones = boneMatricesReader.Read<uint32_t>();
        boneMatricesReader.Skip(8);
        if (numBones > 0) {
            auto &bones = model.skeleton.bones;
            bones.resize(numBones);
            for (uint32_t b = 0; b < numBones; b++)
                bones[b].name = (b < boneNames.size()) ? boneNames[b] : "bone_" + to_string(b);
            vector<Matrix4x4> boneInversedMatrices(numBones);
            for (uint32_t b = 0; b < numBones; b++) {
                ReadMatrix4x4(boneMatricesReader, boneInversedMatrices[b]);
                for (uint32_t j = 0; j < 3; j++)
                    boneInversedMatrices[b].m[3][j] /= 100.0f;
                bones[b].matrix = boneInversedMatrices[b].Inversed();
            }
            Rx3Reader skeletonReader(skeletonChunk);
            skeletonReader.Skip(16);
            for (uint32_t b = 0; b < numBones; b++) {
                int16_t parentIndex = skeletonReader.Read<int16_t>();
                if (parentIndex >= 0 && parentIndex < (int32_t)bones.size()) {
                    bones[b].parent = bones[parentIndex].name;
                    bones[b].matrix = boneInversedMatrices[parentIndex] * bones[b].matrix;
                }
            }
        }
    }
    return model;
}
