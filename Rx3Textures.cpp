#include "Rx3Textures.h"
#include "Rx3Names.h"
#include "errormsg.h"
#include "DirectXTex.h"
#include <wincodec.h>
#include <fstream>
#include "TextFileTable.h"

using namespace DirectX;

DXGI_FORMAT MapRx3FormatToDXGI(unsigned char format) {
    switch (format) {
    case RX3_TEXFORMAT_DXT1:     return DXGI_FORMAT_BC1_UNORM;
    case RX3_TEXFORMAT_DXT3:     return DXGI_FORMAT_BC2_UNORM;
    case RX3_TEXFORMAT_DXT5:     return DXGI_FORMAT_BC3_UNORM;
    case RX3_TEXFORMAT_ARGB8888: return DXGI_FORMAT_B8G8R8A8_UNORM;
    case RX3_TEXFORMAT_L8:       return DXGI_FORMAT_R8_UNORM;
    case RX3_TEXFORMAT_AL8:      return DXGI_FORMAT_R8G8_UNORM;
    case RX3_TEXFORMAT_RG8:      return DXGI_FORMAT_R8G8_UNORM;
    case RX3_TEXFORMAT_BC5:      return DXGI_FORMAT_BC5_UNORM;
    case RX3_TEXFORMAT_RGB565:   return DXGI_FORMAT_B5G6R5_UNORM;
    case RX3_TEXFORMAT_ARGB4444: return DXGI_FORMAT_B4G4R4A4_UNORM;
    case RX3_TEXFORMAT_BC6:      return DXGI_FORMAT_BC6H_UF16;
    case RX3_TEXFORMAT_BC7:      return DXGI_FORMAT_BC7_UNORM;
    case RX3_TEXFORMAT_BC4:      return DXGI_FORMAT_BC4_UNORM;
    default: return DXGI_FORMAT_UNKNOWN;
    }
}

char const *Rx3FormatName(unsigned char format) {
    switch (format) {
    case RX3_TEXFORMAT_DXT1:     return "DXT1";
    case RX3_TEXFORMAT_DXT3:     return "DXT3";
    case RX3_TEXFORMAT_DXT5:     return "DXT5";
    case RX3_TEXFORMAT_ARGB8888: return "ARGB8888";
    case RX3_TEXFORMAT_L8:       return "L8";
    case RX3_TEXFORMAT_AL8:      return "AL8";
    case RX3_TEXFORMAT_RG8:      return "RG8";
    case RX3_TEXFORMAT_BC5:      return "BC5";
    case RX3_TEXFORMAT_RGB565:   return "RGB565";
    case RX3_TEXFORMAT_ARGB4444: return "ARGB4444";
    case RX3_TEXFORMAT_BC6:      return "BC6";
    case RX3_TEXFORMAT_BC7:      return "BC7";
    case RX3_TEXFORMAT_BC4:      return "BC4";
    default: return "Unknown";
    }
}

bool RequiresDX10Header(DXGI_FORMAT fmt, unsigned char rx3BaseFormat) {
    switch (fmt) {
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return true;
    default:
        break;
    }
    if (fmt == DXGI_FORMAT_R8G8_UNORM && rx3BaseFormat == RX3_TEXFORMAT_RG8)
        return true;
    return false;
}

HRESULT ExpandLuminanceAlphaToRGBA(Image const &src, ScratchImage &out) {
    HRESULT hr = out.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, src.width, src.height, 1, 1);
    if (FAILED(hr))
        return hr;
    Image const *dst = out.GetImage(0, 0, 0);
    for (size_t y = 0; y < src.height; y++) {
        unsigned char const *srcRow = src.pixels + y * src.rowPitch;
        unsigned char *dstRow = dst->pixels + y * dst->rowPitch;
        for (size_t x = 0; x < src.width; x++) {
            unsigned char const l = srcRow[x * 2 + 0];
            unsigned char const a = srcRow[x * 2 + 1];
            dstRow[x * 4 + 0] = l;
            dstRow[x * 4 + 1] = l;
            dstRow[x * 4 + 2] = l;
            dstRow[x * 4 + 3] = a;
        }
    }
    return S_OK;
}

void CopyLevelIntoImage(Image const *dstImage, unsigned char const *src, unsigned int srcStride, unsigned int srcHeight) {
    if (!dstImage || !dstImage->pixels || !src)
        return;
    size_t const dstRows = dstImage->rowPitch ? (dstImage->slicePitch / dstImage->rowPitch) : 0;
    size_t const rows = std::min<size_t>(srcHeight, dstRows);
    size_t const rowBytes = std::min<size_t>(srcStride, dstImage->rowPitch);
    for (size_t row = 0; row < rows; row++) {
        std::memcpy(dstImage->pixels + row * dstImage->rowPitch, src + row * srcStride, rowBytes);
    }
}

bool IsSourceOpaque(ScratchImage const &fullImage, Image const &srcImage, unsigned char baseFormat) {
    if (baseFormat == RX3_TEXFORMAT_AL8) {
        for (size_t y = 0; y < srcImage.height; ++y) {
            unsigned char const *row = srcImage.pixels + y * srcImage.rowPitch;
            for (size_t x = 0; x < srcImage.width; ++x) {
                if (row[x * 2 + 1] < 255)
                    return false;
            }
        }
        return true;
    }
    if (!HasAlpha(srcImage.format))
        return true;
    if (fullImage.GetImageCount() == 1)
        return fullImage.IsAlphaAllOpaque();
    ScratchImage single;
    if (FAILED(single.InitializeFromImage(srcImage)))
        return false;
    return single.IsAlphaAllOpaque();
}

unsigned char CalcMaxMipLevel(unsigned short height, unsigned short width) {
    unsigned char result = 1;
    while (width > 1 || height > 1) {
        width = max(width >> 1, 1);
        height = max(height >> 1, 1);
        result++;
    }
    return result;
}

int TexFormatNameToID(std::string const &name) {
    static std::map<std::string, char> texFormatNameToID = {
        { "DXT1", 0 },
        { "BC1", 0 },
        { "DXT3", 1 },
        { "BC2", 1 },
        { "DXT5", 2 },
        { "BC3", 2 },
        { "ARGB8888", 3 },
        { "A8R8G8B8", 3 },
        { "L8", 4 },
        { "AL8", 5 },
        { "A8L8", 5 },
        { "RG8", 6 },
        { "R8G8", 6 },
        { "BC5", 7 },
        { "ATI2", 7 },
        { "RGB565", 8 },
        { "R5G6B5", 8 },
        { "ARGB4444", 9 },
        { "A4R4G4B4", 9 },
        { "BC6", 10 },
        { "BC6H", 10 },
        { "BC6uf", 10 },
        { "BC7", 11 },
        { "BC4", 12 },
        { "ATI1", 12 }
    };
    auto upper = ToUpper(name);
    if (texFormatNameToID.contains(upper))
        return texFormatNameToID[upper];
    return -1;
}

void ReadTexFormatFile(std::filesystem::path const &filePath, std::map<std::string, TexFormatTarget> &out) {
    if (exists(filePath)) {
        TextFileTable texFormatsTable;
        if (texFormatsTable.ReadCSV(filePath)) {
            for (size_t r = 1; r < texFormatsTable.NumRows(); r++) {
                auto const &row = texFormatsTable.Row(r);
                if (row.size() >= 2 && !row[0].empty()) {
                    string name = ToLower(WtoA(row[0]));
                    TexFormatTarget t;
                    for (size_t c = 1; c < min(4, row.size()); c++) {
                        string value = ToLower(WtoA(row[c]));
                        if (IsNumber(value, false))
                            t.levels = SafeConvertInt<char>(row[2]);
                        else {
                            auto format = TexFormatNameToID(value);
                            if (format != -1)
                                t.format = format;
                            else {
                                auto dim = Split(value, 'x');
                                if (dim.size() == 2 && IsNumber(dim[0], false) && IsNumber(dim[1], false)) {
                                    auto IsProperDim = [](int value) { return value > 0 && value <= 16384; };
                                    int width = SafeConvertInt<unsigned short>(dim[0]);
                                    int height = SafeConvertInt<unsigned short>(dim[1]);
                                    if (IsProperDim(width) && IsProperDim(height)) {
                                        t.width = width;
                                        t.height = height;
                                    }
                                }
                            }
                        }
                    }
                    out[name] = t;
                }
            }
        }
    }
}

void ExtractTexturesFromContainer(Rx3Container &container, path const &outputDir, Rx3Options const &rx3options) {
    vector<PackedTextureInfo> vecTexInfo;
    auto texNamesSection = container.FindFirstChunk(RX3_CHUNK_NAME_TABLE);
    vector<string> texNames;
    if (texNamesSection) {
        auto names = ExtractNamesFromSection(texNamesSection);
        for (auto const &[id, name] : names) {
            if (id == RX3_CHUNK_TEXTURE)
                texNames.push_back(name);
        }
    }
    auto textureSections = container.FindAllChunks(RX3_CHUNK_TEXTURE);
    if (textureSections.empty())
        return;
    if (!exists(outputDir))
        create_directories(outputDir);
    bool const saveToPng = rx3options.textureFormat == "png";
    bool const saveToTga = rx3options.textureFormat == "tga";
    bool const isPngOrTga = saveToPng || saveToTga;
    for (size_t i = 0; i < textureSections.size(); i++) {
        string texName;
        if (i < texNames.size() && !texNames[i].empty())
            texName = texNames[i];
        else
            texName = "texture_" + to_string(i);
        Rx3Reader reader(textureSections[i]);
        unsigned int totalSize = reader.Read<unsigned int>();
        unsigned char type = reader.Read<unsigned char>();
        unsigned char baseFormat = reader.Read<unsigned char>();
        unsigned char dataFormat = reader.Read<unsigned char>();
        reader.Skip(1);
        unsigned short width = reader.Read<unsigned short>();
        unsigned short height = reader.Read<unsigned short>();
        unsigned short depth = reader.Read<unsigned short>();
        unsigned short mips = reader.Read<unsigned short>();
        auto &texInfo = vecTexInfo.emplace_back();
        texInfo.name = texName;
        texInfo.width = width;
        texInfo.height = height;
        texInfo.levels = (unsigned char)mips;
        texInfo.format = baseFormat;
        DXGI_FORMAT const dxgiFormat = MapRx3FormatToDXGI(baseFormat);
        bool typeSupported = type == RX3_TEXTURE_2D || type == RX3_TEXTURE_3D || type == RX3_TEXTURE_CUBE || type == RX3_TEXTURE_ARRAY;
        if (dataFormat != RX3_TEXDATAFORMAT_LINEAR || !typeSupported || dxgiFormat == DXGI_FORMAT_UNKNOWN) {
            ErrorMessage(Format("Unsupported texture format (texture %s in file %s)", texName.c_str(), container.mName.c_str()));
            continue;
        }
        if (width == 0 || height == 0 || depth == 0 || mips == 0)
            continue;
        bool const isCubemap = type == RX3_TEXTURE_CUBE && depth == 6;
        bool const isVolume = type == RX3_TEXTURE_3D;
        bool const isArray = type == RX3_TEXTURE_ARRAY;
        bool const forceDds = isVolume || isArray || isCubemap;
        unsigned short const levelsToKeep = (isPngOrTga && !forceDds) ? 1 : mips;
        TexMetadata metadata = {};
        metadata.width = width;
        metadata.height = height;
        metadata.mipLevels = levelsToKeep;
        metadata.format = dxgiFormat;
        if (isVolume) {
            metadata.dimension = TEX_DIMENSION_TEXTURE3D;
            metadata.depth = depth;
            metadata.arraySize = 1;
        }
        else {
            metadata.dimension = TEX_DIMENSION_TEXTURE2D;
            metadata.depth = 1;
            metadata.arraySize = depth;
            if (isCubemap)
                metadata.miscFlags |= TEX_MISC_TEXTURECUBE;
        }
        ScratchImage image;
        HRESULT hr = image.Initialize(metadata);
        if (FAILED(hr)) {
            ErrorMessage(Format("Failed to allocate texture '%s'", texName.c_str()));
            continue;
        }
        if (isVolume) {
            for (unsigned short l = 0; l < mips; l++) {
                unsigned short const curDepth = std::max<unsigned short>(1, static_cast<unsigned short>(depth >> l));
                for (unsigned short s = 0; s < curDepth; s++) {
                    unsigned int dataStride = reader.Read<unsigned int>();
                    unsigned int dataHeight = reader.Read<unsigned int>();
                    unsigned int levelSize = reader.Read<unsigned int>();
                    reader.Skip(4);
                    if (l < levelsToKeep) {
                        Image const *dstImage = image.GetImage(l, 0, s);
                        auto const *src = reinterpret_cast<unsigned char const *>(reader.GetCurrentPtr());
                        CopyLevelIntoImage(dstImage, src, dataStride, dataHeight);
                    }
                    reader.Skip(levelSize);
                }
            }
        }
        else {
            for (unsigned short f = 0; f < depth; f++) {
                for (unsigned short l = 0; l < mips; l++) {
                    unsigned int dataStride = reader.Read<unsigned int>();
                    unsigned int dataHeight = reader.Read<unsigned int>();
                    unsigned int levelSize = reader.Read<unsigned int>();
                    reader.Skip(4);
                    if (l < levelsToKeep) {
                        Image const *dstImage = image.GetImage(l, f, 0);
                        auto const *src = reinterpret_cast<unsigned char const *>(reader.GetCurrentPtr());
                        CopyLevelIntoImage(dstImage, src, dataStride, dataHeight);
                    }
                    reader.Skip(levelSize);
                }
            }
        }
        if (dxgiFormat == DXGI_FORMAT_BC6H_UF16 && rx3options.writeHDR && !forceDds) {
            Image const *srcImage = image.GetImage(0, 0, 0);
            if (!srcImage) {
                ErrorMessage(Format("Texture '%s' has no image data", texName.c_str()));
                continue;
            }
            ScratchImage converted;
            Image const *finalSrc = srcImage;
            HRESULT hr = Decompress(*srcImage, DXGI_FORMAT_R16G16B16A16_FLOAT, converted);
            if (FAILED(hr)) {
                ErrorMessage(Format("Failed to convert texture '%s' for HDR export", texName.c_str()));
                continue;
            }
            if (converted.GetImageCount() > 0)
                finalSrc = converted.GetImage(0, 0, 0);
            path const destPath = outputDir / (texName + ".hdr");
            hr = SaveToHDRFile(*finalSrc, destPath.c_str());
            if (FAILED(hr)) {
                ErrorMessage(Format("Failed to save texture '%s'", texName.c_str()));
                continue;
            }
            continue;
        }
        if (isPngOrTga && !forceDds) {
            Image const *srcImage = image.GetImage(0, 0, 0);
            if (!srcImage) {
                ErrorMessage(Format("Texture '%s' has no image data", texName.c_str()));
                continue;
            }
            bool opaque = IsSourceOpaque(image, *srcImage, baseFormat);
            DXGI_FORMAT colorTarget = opaque ? DXGI_FORMAT_B8G8R8X8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
            ScratchImage converted;
            Image const *finalSrc = srcImage;
            HRESULT hr = S_OK;
            if (baseFormat == RX3_TEXFORMAT_AL8) {
                ScratchImage rgba;
                hr = ExpandLuminanceAlphaToRGBA(*srcImage, rgba);
                if (SUCCEEDED(hr))
                    hr = Convert(*rgba.GetImage(0, 0, 0), colorTarget, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, converted);
            }
            else if (IsCompressed(srcImage->format))
                hr = Decompress(*srcImage, colorTarget, converted);
            else if (srcImage->format != colorTarget)
                hr = Convert(*srcImage, colorTarget, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, converted);
            if (FAILED(hr)) {
                ErrorMessage(Format("Failed to convert texture '%s' for PNG/TGA export", texName.c_str()));
                continue;
            }
            if (converted.GetImageCount() > 0)
                finalSrc = converted.GetImage(0, 0, 0);
            path destPath = outputDir / (texName + (saveToPng ? ".png" : ".tga"));
            hr = saveToPng
                ? SaveToWICFile(*finalSrc, WIC_FLAGS_FORCE_SRGB, GUID_ContainerFormatPng, destPath.c_str())
                : SaveToTGAFile(*finalSrc, TGA_FLAGS_NONE, destPath.c_str());
            if (FAILED(hr)) {
                ErrorMessage(Format("Failed to save texture '%s'", texName.c_str()));
                continue;
            }
            continue;
        }
        DDS_FLAGS ddsFlags = DDS_FLAGS_NONE;
        if (RequiresDX10Header(metadata.format, baseFormat))
            ddsFlags |= DDS_FLAGS_FORCE_DX10_EXT;
        path destPath = outputDir / (texName + ".dds");
        hr = SaveToDDSFile(image.GetImages(), image.GetImageCount(), metadata, ddsFlags, destPath.c_str());
        if (FAILED(hr)) {
            ErrorMessage(Format("Failed to save texture '%s'", texName.c_str()));
            continue;
        }
    }
    auto hotspotChunk = container.FindFirstChunk(RX3_CHUNK_HOTSPOT);
    if (hotspotChunk) {
        Rx3Reader hotspotReader(hotspotChunk);
        hotspotReader.Skip(4);
        unsigned char version = hotspotReader.Read<unsigned char>();
        if (version == 1) {
            unsigned char numGroups = hotspotReader.Read<unsigned char>();
            hotspotReader.Skip(10);
            ofstream outHotspot(outputDir / (container.mName + ".hotspot"));
            outHotspot << "{" << std::endl;
            outHotspot << "  \"AssetName\": \"" << container.mName << "\"," << std::endl;
            outHotspot << "  \"HotspotFormatVersion\": " << (int)version << "," << std::endl;
            struct HotspotEntry {
                string name, group;
                float bounds[4];
            };
            vector<HotspotEntry> hotspots;
            for (size_t groupIndex = 0; groupIndex < numGroups; groupIndex++) {
                string groupName = hotspotReader.ReadString();
                unsigned char numHotspots = hotspotReader.Read<unsigned char>();
                for (size_t hotspotIndex = 0; hotspotIndex < numHotspots; hotspotIndex++) {
                    auto &hotspot = hotspots.emplace_back();
                    hotspot.group = groupName;
                    hotspot.name = hotspotReader.ReadString();
                    for (size_t bound = 0; bound < 4; bound++)
                        hotspot.bounds[bound] = hotspotReader.Read<float>();
                }
            }
            if (!hotspots.empty()) {
                outHotspot << "  \"Hotspots\": [" << std::endl;
                for (auto const &h : hotspots) {
                    outHotspot << "    {" << std::endl;
                    outHotspot << "      \"Name\": \"" << h.name << "\"," << std::endl;
                    outHotspot << "      \"Group\": \"" << h.group << "\"," << std::endl;
                    outHotspot << "      \"Bounds\": {" << std::endl;
                    outHotspot << "        \"X\": " << h.bounds[0] << "," << std::endl;
                    outHotspot << "        \"Y\": " << h.bounds[1] << "," << std::endl;
                    outHotspot << "        \"Z\": " << h.bounds[2] << "," << std::endl;
                    outHotspot << "        \"W\": " << h.bounds[3] << "," << std::endl;
                    outHotspot << "      }," << std::endl;
                    outHotspot << "    }" << std::endl;
                }
                outHotspot << "  ]" << std::endl;
            }
            outHotspot << "}" << std::endl;
        }
    }
    if (rx3options.writeTexMetadata && !vecTexInfo.empty()) {
        ofstream outMetadata(outputDir / (container.mName + "_metadata.csv"));
        for (auto const &texInfo : vecTexInfo) {
            outMetadata << "\"" << texInfo.name << "\"," << Rx3FormatName(texInfo.format);
            int maxLevels = CalcMaxMipLevel(texInfo.width, texInfo.height);
            if (texInfo.levels < maxLevels)
                outMetadata << "," << (texInfo.levels == 1 ? 1 : ((int)texInfo.levels - maxLevels));
            outMetadata << std::endl;
        }
    }
}

void ExtractTexturesFromRX3(path const &rx3path, path const &outputPath, Rx3Options const &rx3options) {
    Rx3Container rx3(rx3path);
    ExtractTexturesFromContainer(rx3, outputPath, rx3options);
}

unsigned char MapDXGIToRx3Format(DXGI_FORMAT fmt) {
    switch (fmt) {
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return RX3_TEXFORMAT_DXT1;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return RX3_TEXFORMAT_DXT3;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return RX3_TEXFORMAT_DXT5;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return RX3_TEXFORMAT_ARGB8888;
    case DXGI_FORMAT_R8_UNORM:
        return RX3_TEXFORMAT_L8;
    case DXGI_FORMAT_R8G8_UNORM:
        return RX3_TEXFORMAT_RG8;
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        return RX3_TEXFORMAT_BC5;
    case DXGI_FORMAT_B5G6R5_UNORM:
        return RX3_TEXFORMAT_RGB565;
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return RX3_TEXFORMAT_ARGB4444;
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        return RX3_TEXFORMAT_BC6;
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return RX3_TEXFORMAT_BC7;
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return RX3_TEXFORMAT_BC4;
    default: return RX3_TEXFORMAT_DXT1; // Fallback
    }
}

bool IsPowerOfTwo(size_t x) {
    return x > 0 && (x & (x - 1)) == 0;
}

size_t NextPowerOfTwo(size_t x) {
    size_t p = 1;
    while (p < x) p <<= 1;
    return p;
}

bool PackTexturesToRx3Container(std::vector<PackedTextureInfo> const &textures, std::filesystem::path const &rx3path, std::map<unsigned char, unsigned char> formatConversion) {
    Rx3Container rx3;
    rx3.AddChunk(RX3_CHUNK_TEXTURE_BATCH);
    for (auto const &t : textures) {
        auto ext = t.filePath.extension();
        std::string extStr = ext.string();
        std::transform(extStr.begin(), extStr.end(), extStr.begin(), ::tolower);

        bool isDDS = (extStr == ".dds");
        bool isHDR = (extStr == ".hdr");
        bool isPNG = (extStr == ".png");
        bool isTGA = (extStr == ".tga");

        ScratchImage image;
        HRESULT hr = S_OK;

        // 1. Load Image Data
        if (isDDS) hr = LoadFromDDSFile(t.filePath.c_str(), DDS_FLAGS_NONE, nullptr, image);
        else if (isPNG) hr = LoadFromWICFile(t.filePath.c_str(), WIC_FLAGS_NONE, nullptr, image);
        else if (isTGA) hr = LoadFromTGAFile(t.filePath.c_str(), TGA_FLAGS_NONE, nullptr, image);
        else if (isHDR) hr = LoadFromHDRFile(t.filePath.c_str(), nullptr, image);
        else continue; // Unsupported extension

        if (FAILED(hr)) return false;

        auto const &meta = image.GetMetadata();
        size_t originalMips = meta.mipLevels;

        // 2. Resolve Target Format
        unsigned char rx3Format = 0;
        if (t.format == -1) {
            if (isDDS) {
                rx3Format = MapDXGIToRx3Format(meta.format);
            }
            else if (isHDR) {
                rx3Format = RX3_TEXFORMAT_BC6;
            }
            else { // PNG or TGA
                rx3Format = image.IsAlphaAllOpaque() ? RX3_TEXFORMAT_DXT1 : RX3_TEXFORMAT_DXT5;
            }
        }
        else {
            rx3Format = static_cast<unsigned char>(t.format);
        }

        // Apply fallback conversions if mapped (e.g. BC5 -> DXT5)
        if (formatConversion.count(rx3Format)) {
            rx3Format = formatConversion.at(rx3Format);
        }

        DXGI_FORMAT targetDxgi = MapRx3FormatToDXGI(rx3Format);
        if (targetDxgi == DXGI_FORMAT_UNKNOWN) return false;

        // 3. Resolve Dimensions & Handle Power-of-Two rules
        size_t targetW = (t.width > 0) ? t.width : meta.width;
        size_t targetH = (t.height > 0) ? t.height : meta.height;

        if (IsCompressed(targetDxgi)) {
            if (!IsPowerOfTwo(targetW)) targetW = NextPowerOfTwo(targetW);
            if (!IsPowerOfTwo(targetH)) targetH = NextPowerOfTwo(targetH);
        }

        // Resize if required
        if (targetW != meta.width || targetH != meta.height) {
            ScratchImage resizedImage;
            hr = Resize(image.GetImages(), image.GetImageCount(), meta, targetW, targetH, TEX_FILTER_DEFAULT, resizedImage);
            if (FAILED(hr)) return false;
            std::swap(image, resizedImage);
        }

        // 4. Resolve and Generate Mipmaps
        auto const &curMeta = image.GetMetadata();
        bool isVolume = curMeta.dimension == TEX_DIMENSION_TEXTURE3D;

        size_t maxMips = 1;
        size_t mw = targetW, mh = targetH, md = isVolume ? curMeta.depth : 1;
        while (mw > 1 || mh > 1 || md > 1) {
            if (mw > 1) mw >>= 1;
            if (mh > 1) mh >>= 1;
            if (md > 1) md >>= 1;
            maxMips++;
        }

        size_t targetMips = 1;
        if (t.levels == 0) {
            if (isDDS) targetMips = originalMips;
            else if (isPNG || isTGA) targetMips = maxMips;
            else if (isHDR) targetMips = 1;
        }
        else {
            targetMips = t.levels;
        }
        targetMips = std::clamp<size_t>(targetMips, 1, maxMips);

        // DirectXTex drops mips during Resize; we need to regenerate if expected
        if (curMeta.mipLevels != targetMips && targetMips > 1) {
            ScratchImage mipImage;
            if (isVolume) {
                hr = GenerateMipMaps3D(image.GetImages(), image.GetImageCount(), curMeta, TEX_FILTER_DEFAULT, targetMips, mipImage);
            }
            else {
                hr = GenerateMipMaps(image.GetImages(), image.GetImageCount(), curMeta, TEX_FILTER_DEFAULT, targetMips, mipImage);
            }
            if (FAILED(hr)) return false;
            std::swap(image, mipImage);
        }

        // 5. Convert / Compress Format
        auto const &mipMeta = image.GetMetadata();
        if (mipMeta.format != targetDxgi) {
            ScratchImage finalImage;
            if (IsCompressed(targetDxgi)) {
                // BC6H compression takes time, TEX_COMPRESS_PARALLEL can help speed it up if supported
                hr = Compress(image.GetImages(), image.GetImageCount(), mipMeta, targetDxgi, TEX_COMPRESS_DEFAULT, TEX_THRESHOLD_DEFAULT, finalImage);
            }
            else {
                hr = Convert(image.GetImages(), image.GetImageCount(), mipMeta, targetDxgi, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, finalImage);
            }
            if (FAILED(hr)) return false;
            std::swap(image, finalImage);
        }

        auto const &finMeta = image.GetMetadata();

        // 6. Serialize into RX3 binary format
        unsigned char rx3Type = RX3_TEXTURE_2D;
        if (finMeta.dimension == TEX_DIMENSION_TEXTURE3D) rx3Type = RX3_TEXTURE_3D;
        else if (finMeta.miscFlags & TEX_MISC_TEXTURECUBE) rx3Type = RX3_TEXTURE_CUBE;
        else if (finMeta.arraySize > 1) rx3Type = RX3_TEXTURE_ARRAY;

        std::vector<unsigned char> texData;
        Rx3Writer texDataWriter(texData);

        // Write 16-byte Header
        texDataWriter.Put<unsigned int>(0); // Size placeholder
        texDataWriter.Put<unsigned char>(rx3Type);
        texDataWriter.Put<unsigned char>(rx3Format);
        texDataWriter.Put<unsigned char>(RX3_TEXDATAFORMAT_LINEAR);
        texDataWriter.Put<unsigned char>(0); // Padding to align header
        texDataWriter.Put<unsigned short>(static_cast<unsigned short>(finMeta.width));
        texDataWriter.Put<unsigned short>(static_cast<unsigned short>(finMeta.height));
        texDataWriter.Put<unsigned short>(static_cast<unsigned short>(finMeta.arraySize > 1 && rx3Type != RX3_TEXTURE_3D ? finMeta.arraySize : finMeta.depth));
        texDataWriter.Put<unsigned short>(static_cast<unsigned short>(targetMips));

        // Write Texture Data Mip-by-Mip
        if (rx3Type == RX3_TEXTURE_3D) {
            for (size_t l = 0; l < targetMips; ++l) {
                size_t curDepth = std::max<size_t>(1, finMeta.depth >> l);
                for (size_t s = 0; s < curDepth; ++s) {
                    const Image *img = image.GetImage(l, 0, s);
                    size_t rowPitch, slicePitch;
                    ComputePitch(finMeta.format, img->width, img->height, rowPitch, slicePitch, CP_FLAGS_NONE);
                    size_t dataHeight = slicePitch / rowPitch;
                    texDataWriter.Put<unsigned int>(static_cast<unsigned int>(rowPitch));
                    texDataWriter.Put<unsigned int>(static_cast<unsigned int>(dataHeight));
                    texDataWriter.Put<unsigned int>(static_cast<unsigned int>(slicePitch));
                    texDataWriter.Put<unsigned int>(0); // padding
                    texDataWriter.Put(img->pixels, slicePitch);
                }
            }
        }
        else {
            // For Arrays, Cubes, and 2D textures (Loop slices, then mips)
            for (size_t f = 0; f < finMeta.arraySize; ++f) {
                for (size_t l = 0; l < targetMips; ++l) {
                    const Image *img = image.GetImage(l, f, 0);
                    size_t rowPitch, slicePitch;
                    ComputePitch(finMeta.format, img->width, img->height, rowPitch, slicePitch, CP_FLAGS_NONE);
                    // Fallback to manual height calc if ComputePitch acts strange for block compression on tiny mips
                    size_t dataHeight = IsCompressed(finMeta.format) ? std::max<size_t>(1, (img->height + 3) / 4) : img->height;
                    texDataWriter.Put<unsigned int>(static_cast<unsigned int>(rowPitch));
                    texDataWriter.Put<unsigned int>(static_cast<unsigned int>(dataHeight));
                    texDataWriter.Put<unsigned int>(static_cast<unsigned int>(slicePitch));
                    texDataWriter.Put<unsigned int>(0); // padding
                    texDataWriter.Put(img->pixels, slicePitch);
                }
            }
        }
        texDataWriter.Align();
        // Overwrite size placeholder at the beginning
        *reinterpret_cast<unsigned int *>(texData.data()) = static_cast<unsigned int>(texData.size());
        Rx3Writer texWriter(rx3.AddChunk(RX3_CHUNK_TEXTURE));
        texWriter.Put(texData.data(), texData.size());
    }
    // 7. Write Headers Section (Extract 16-byte headers from all populated textures)
    std::vector<Rx3Chunk *> textureChunks = rx3.FindAllChunks(RX3_CHUNK_TEXTURE);
    Rx3Writer texHeadersWriter(rx3.FindFirstChunk(RX3_CHUNK_TEXTURE_BATCH));
    texHeadersWriter.Put<unsigned int>(static_cast<unsigned int>(textureChunks.size()));
    texHeadersWriter.Align();
    for (auto const &chunk : textureChunks)
        texHeadersWriter.Put(chunk->mData.data(), 16);
    // 8. Write Global Names Table
    auto &namesSection = rx3.AddChunk(RX3_CHUNK_NAME_TABLE);
    Rx3Writer namesWriter(namesSection);
    unsigned int namesSize = 16 + 8 * textures.size();
    for (auto const &t : textures)
        namesSize += t.name.length() + 1; // +1 for null terminator
    namesWriter.Put<unsigned int>(namesSize);
    namesWriter.Put<unsigned int>(static_cast<unsigned int>(textures.size()));
    namesWriter.Align();
    for (auto const &t : textures) {
        namesWriter.Put<unsigned int>(RX3_CHUNK_TEXTURE);
        namesWriter.Put<unsigned int>(static_cast<unsigned int>(t.name.size() + 1));
        namesWriter.Put(t.name.c_str(), t.name.size() + 1); // Ensure null byte is pushed
    }
    namesWriter.Align();
    // Write names block size at the very start of its payload
    *reinterpret_cast<unsigned int *>(namesSection.mData.data()) = static_cast<unsigned int>(namesSection.mData.size());
    // 9. Save out to file
    rx3.Save(rx3path);
    return true;
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
