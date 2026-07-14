#include "..\winheader.h"
#include <D3DX11.h>
#include "PackTextures.h"
#include "..\D3DDevice\D3DDevice.h"

unsigned int GetPixelDataSize(unsigned short width, unsigned short height, unsigned char format) {
    if (format == RX3TEXTUREFORMAT_L8)
        return width * height;
    else {
        if (width < 4)
            width = 4;
        if (height < 4)
            height = 4;
        switch (format) {
        case RX3TEXTUREFORMAT_DXT1:
        case RX3TEXTUREFORMAT_ATI1:
            return width * height / 2;
        case RX3TEXTUREFORMAT_DXT3:
        case RX3TEXTUREFORMAT_DXT5:
        case RX3TEXTUREFORMAT_ATI2:
            return width * height;
        }
    }
    return 0;
}
bool PackTexturesToRx3Container(std::vector<PackedTextureInfo> const &textures, std::filesystem::path const &rx3path) {
    auto device = D3DDevice11::GlobalDevice()->Interface();
    auto context = D3DDevice11::GlobalDevice()->Context();
    Rx3Container rx3;
    rx3.AddSection(RX3_SECTION_TEXTURES_HEADER);
    for (auto const &t : textures) {
        char format = t.format;
        D3DX11_IMAGE_INFO imageInfo;
        if (FAILED(D3DX11GetImageInfoFromFileW(t.filePath.c_str(), NULL, &imageInfo, NULL)))
            return false;
        // TODO
        if (imageInfo.ResourceDimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D || (imageInfo.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE))
            return false;
        char destFormat = DXGI_FORMAT_BC1_UNORM;
        if (t.format == RX3TEXTUREFORMAT_DXT1)
            destFormat = DXGI_FORMAT_BC1_UNORM;
        else if (t.format == RX3TEXTUREFORMAT_DXT3)
            destFormat = DXGI_FORMAT_BC2_UNORM;
        else if (t.format == RX3TEXTUREFORMAT_DXT5)
            destFormat = DXGI_FORMAT_BC3_UNORM;
        else if (t.format == RX3TEXTUREFORMAT_ATI1)
            destFormat = DXGI_FORMAT_BC4_UNORM;
        else if (t.format == RX3TEXTUREFORMAT_ATI2)
            destFormat = DXGI_FORMAT_BC5_UNORM;
        else if (t.format == RX3TEXTUREFORMAT_L8)
            destFormat = DXGI_FORMAT_R8_UNORM;
        else {
            switch (imageInfo.Format) {
            case DXGI_FORMAT_BC3_TYPELESS:
            case DXGI_FORMAT_BC3_UNORM:
            case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            case DXGI_FORMAT_R10G10B10A2_UNORM:
            case DXGI_FORMAT_R10G10B10A2_UINT:
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            case DXGI_FORMAT_R8G8B8A8_UINT:
            case DXGI_FORMAT_R8G8B8A8_SNORM:
            case DXGI_FORMAT_R8G8B8A8_SINT:
            case DXGI_FORMAT_A8_UNORM:
            case DXGI_FORMAT_B5G5R5A1_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8X8_UNORM:
            case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                destFormat = DXGI_FORMAT_BC3_UNORM;
                format = RX3TEXTUREFORMAT_DXT5;
                break;
            case DXGI_FORMAT_BC2_TYPELESS:
            case DXGI_FORMAT_BC2_UNORM:
                destFormat = DXGI_FORMAT_BC2_UNORM;
                format = RX3TEXTUREFORMAT_DXT3;
                break;
            case DXGI_FORMAT_BC5_TYPELESS:
            case DXGI_FORMAT_BC5_UNORM:
            case DXGI_FORMAT_BC5_SNORM:
            case DXGI_FORMAT_R8G8_TYPELESS:
            case DXGI_FORMAT_R8G8_UNORM:
            case DXGI_FORMAT_R8G8_UINT:
            case DXGI_FORMAT_R8G8_SNORM:
            case DXGI_FORMAT_R8G8_SINT:
            case DXGI_FORMAT_R16G16_TYPELESS:
            case DXGI_FORMAT_R16G16_FLOAT:
            case DXGI_FORMAT_R16G16_UNORM:
            case DXGI_FORMAT_R16G16_UINT:
            case DXGI_FORMAT_R16G16_SNORM:
            case DXGI_FORMAT_R16G16_SINT:
                destFormat = DXGI_FORMAT_BC5_UNORM;
                format = RX3TEXTUREFORMAT_ATI2;
                break;
            case DXGI_FORMAT_BC4_TYPELESS:
            case DXGI_FORMAT_BC4_UNORM:
            case DXGI_FORMAT_BC4_SNORM:
                destFormat = DXGI_FORMAT_BC4_UNORM;
                format = RX3TEXTUREFORMAT_ATI1;
                break;
            case DXGI_FORMAT_R16_TYPELESS:
            case DXGI_FORMAT_R16_FLOAT:
            case DXGI_FORMAT_D16_UNORM:
            case DXGI_FORMAT_R16_UNORM:
            case DXGI_FORMAT_R16_UINT:
            case DXGI_FORMAT_R16_SNORM:
            case DXGI_FORMAT_R16_SINT:
            case DXGI_FORMAT_R8_TYPELESS:
            case DXGI_FORMAT_R8_UNORM:
            case DXGI_FORMAT_R8_UINT:
            case DXGI_FORMAT_R8_SNORM:
            case DXGI_FORMAT_R8_SINT:
                destFormat = DXGI_FORMAT_R8_UNORM;
                format = RX3TEXTUREFORMAT_L8;
                break;
            default:
                destFormat = DXGI_FORMAT_BC1_UNORM;
                format = RX3TEXTUREFORMAT_DXT1;
                break;
            }
        }
        D3DX11_IMAGE_LOAD_INFO loadInfo;
        if (t.width > 0)
            loadInfo.Width = t.width;
        else
            loadInfo.Width = D3DX11_DEFAULT;
        if (t.height > 0)
            loadInfo.Height = t.height;
        else
            loadInfo.Height = D3DX11_DEFAULT;
        loadInfo.Depth = D3DX11_DEFAULT;
        loadInfo.FirstMipLevel = D3DX11_DEFAULT;
        auto fileExt = ToLower(t.filePath.extension().string());
        bool isDDS = fileExt == ".dds";
        auto numLevels = t.levels;
        if (numLevels == -1) {
            if (false && isDDS)
                numLevels = D3DX11_FROM_FILE;
            else
                numLevels = D3DX11_DEFAULT;
        }
        else if (numLevels == 0)
            numLevels = D3DX11_DEFAULT;
        loadInfo.MipLevels = numLevels;
        loadInfo.Usage = D3D11_USAGE_STAGING;
        loadInfo.BindFlags = 0;
        loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ;
        loadInfo.MiscFlags = D3DX11_DEFAULT;
        loadInfo.Format = (DXGI_FORMAT)destFormat;
        loadInfo.Filter = D3DX11_DEFAULT;
        loadInfo.MipFilter = D3DX11_DEFAULT;
        loadInfo.pSrcInfo = NULL;

        ID3D11Texture2D *texture = nullptr;
        if (FAILED(D3DX11CreateTextureFromFileW(device, t.filePath.c_str(), &loadInfo, NULL, (ID3D11Resource **)&texture, NULL)))
            return false;

        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);
        vector<unsigned char> texHeader;
        Rx3Writer texHeaderWriter(texHeader);
        texHeaderWriter.Put<unsigned int>(0);
        texHeaderWriter.Put<unsigned char>(RX3TEXTURETYPE_2D);
        texHeaderWriter.Put(format);
        texHeaderWriter.Put<unsigned short>(1);
        texHeaderWriter.Put<unsigned short>(desc.Width);
        texHeaderWriter.Put<unsigned short>(desc.Height);
        texHeaderWriter.Put<unsigned short>(1);
        texHeaderWriter.Put<unsigned short>(desc.MipLevels);

        vector<unsigned char> texData;
        Rx3Writer texDataWriter(texData);
        texDataWriter.Put(texHeader.data(), texHeader.size());
        unsigned short w = desc.Width;
        unsigned short h = desc.Height;
        for (int nMipLevel = 0; nMipLevel < desc.MipLevels; nMipLevel++) {
            D3D11_MAPPED_SUBRESOURCE sub;
            Memory_Fill(&sub, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));
            if (FAILED(context->Map(texture, D3D11CalcSubresource(nMipLevel, 0, desc.MipLevels), D3D11_MAP_READ, 0, &sub))) {
                texture->Release();
                return false;
            }
            unsigned int levelSize = GetPixelDataSize(w, h, format);
            unsigned int pitch = 0;
            switch (format) {
            case RX3TEXTUREFORMAT_DXT1:
            case RX3TEXTUREFORMAT_ATI1:
                pitch = max(1u, ((w + 3u) / 4u)) * 8;
                break;
            case RX3TEXTUREFORMAT_DXT3:
            case RX3TEXTUREFORMAT_DXT5:
            case RX3TEXTUREFORMAT_ATI2:
                pitch = std::max(1u, ((w + 3u) / 4u)) * 16;
                break;
            case RX3TEXTUREFORMAT_L8:
                pitch = (w * 8 + 7) / 8;
                break;
            }
            texDataWriter.Put(pitch);
            texDataWriter.Put(levelSize / pitch);
            texDataWriter.Put(levelSize);
            texDataWriter.Put<unsigned int>(0);
            unsigned int byteOffset = 0;
            unsigned int rows = sub.DepthPitch / sub.RowPitch;
            for (unsigned int i = 0; i < rows; i++) {
                texDataWriter.Put(&((unsigned char *)sub.pData)[byteOffset], pitch);
                byteOffset += sub.RowPitch;
            }
            context->Unmap(texture, D3D11CalcSubresource(nMipLevel, 0, desc.MipLevels));
            w /= 2;
            h /= 2;
        }
        texDataWriter.Align();
        SetAt(texData.data(), 0, texData.size());
        Rx3Writer texWriter(rx3.AddSection(RX3_SECTION_TEXTURE));
        texWriter.Put(texData.data(), texData.size());
        texture->Release();
    }
    vector<Rx3Section *> textureSections = rx3.FindAllSections(RX3_SECTION_TEXTURE);
    Rx3Writer texHeadersWriter(rx3.FindFirstSection(RX3_SECTION_TEXTURES_HEADER));
    texHeadersWriter.Put(textureSections.size());
    texHeadersWriter.Align();
    for (auto const &t : textureSections)
        texHeadersWriter.Put(t->data.data(), 16);
    auto &namesSection = rx3.AddSection(RX3_SECTION_NAMES);
    Rx3Writer namesWriter(namesSection);
    unsigned int namesSize = 16 + 8 * textures.size();
    for (auto const &t : textures)
        namesSize += t.name.length() + 1;
    namesWriter.Put(namesSize);
    namesWriter.Put(textures.size());
    namesWriter.Align();
    for (auto const &t : textures) {
        namesWriter.Put<unsigned int>(RX3_SECTION_TEXTURE);
        namesWriter.Put(t.name.size() + 1);
        namesWriter.Put(t.name);
    }
    namesWriter.Align();
    SetAt(namesSection.data.data(), 0, namesSection.data.size());
    rx3.Save(rx3path);
    return true;
}

bool PackFolderTexturesToRx3Container(std::filesystem::path const &folderPath, std::filesystem::path const &rx3path) {
    if (is_directory(folderPath)) {
        struct PackedTextureInfoOrdered : PackedTextureInfo {
            unsigned int priority = 0;
        };
        std::map<std::string, PackedTextureInfoOrdered> texturesMap;
        for (auto const &i : std::filesystem::directory_iterator(folderPath)) {
            std::string id = ToLower(i.path().stem().string());
            std::string ext = ToLower(i.path().extension().string());
            unsigned int priority = 0;
            if (ext == ".dds")
                priority = 5;
            else if (ext == ".png")
                priority = 3;
            else if (ext == ".bmp")
                priority = 2;
            else if (ext == ".jpg")
                priority = 1;
            if (priority != 0) {
                auto &dst = texturesMap[id];
                if (priority > dst.priority) {
                    dst.priority = priority;
                    dst.filePath = i.path();
                    dst.name = i.path().stem().string();
                }
            }
        }
        if (!texturesMap.empty()) {
            std::vector<PackedTextureInfo> texturesToPack;
            for (auto const &[key, pti] : texturesMap)
                texturesToPack.push_back(pti);
            return PackTexturesToRx3Container(texturesToPack, rx3path);
        }
    }
    return false;
}

PackedTextureInfo::PackedTextureInfo() {}

PackedTextureInfo::PackedTextureInfo(std::filesystem::path const &_filePath) {
    name = _filePath.stem().string();
    filePath = _filePath;
}

PackedTextureInfo::PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath) {
    name = _name;
    filePath = _filePath;
}

PackedTextureInfo::PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format) {
    name = _name;
    filePath = _filePath;
    format = _format;
}

PackedTextureInfo::PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format, char _levels) {
    name = _name;
    filePath = _filePath;
    format = _format;
    levels = _levels;
}

PackedTextureInfo::PackedTextureInfo(std::string const &_name, std::filesystem::path const &_filePath, char _format, char _levels, unsigned short _width, unsigned short _height) {
    name = _name;
    filePath = _filePath;
    format = _format;
    levels = _levels;
    width = _width;
    height = _height;
}
