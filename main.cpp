#include "commandline.h"
#include "errormsg.h"
#include "Model.h"
#include "Rx3Model.h"
#include "Rx3Textures.h"
#include "Rx3Hotspot.h"
#include "TextFileTable.h"
#include <string>
#include <windows.h>
#include <shobjidl.h>

const char *RX3C_VERSION = "0.200";
const unsigned int RX3C_VERSION_INT = 200;

enum ErrorType {
    NONE = 0,
    NOT_ENOUGHT_ARGUMENTS = 1,
    UNKNOWN_OPERATION_TYPE = 2,
    NO_INPUT_PATH = 3,
    INVALID_INPUT_PATH = 4,
    INPUT_PATH_MULTIPLE_FOLDERS = 5,
    OUTPUT_PATH_IS_NOT_A_FOLDER = 6,
    UNKNOWN_GAME_TAG = 7,
    INVALID_OUTPUT_PATH = 8,
    FAILED_TO_INITIALIZE = 9,
    ERROR_OTHER = 10
};

enum OperationType {
    OP_NONE = 0,
    OP_EXPORT = 1,
    OP_IMPORT = 2
};

int wmain(int argc, wchar_t *argv[]) {
    CommandLine cmd(argc, argv,
        // arguments
        { L"i", L"o", L"game", L"skeleton", L"model", L"texture", L"folderOption", L"texFormatFile"},
        // options
        { L"export", L"import", L"recursive", L"silent", L"console", L"exportQuads", L"writeHDR", L"writeTexMetadata",
          L"noMetadata" }
    );
    if (cmd.HasOption(L"silent"))
        SetErrorDisplayType(ErrorDisplayType::ERR_NONE);
    else {
        if (!cmd.HasOption(L"console"))
            SetErrorDisplayType(ErrorDisplayType::ERR_MESSAGE_BOX);
        else
            SetErrorDisplayType(ErrorDisplayType::ERR_CONSOLE);
    }
    OperationType operation = OperationType::OP_NONE;
    if (cmd.HasOption(L"export"))
        operation = OperationType::OP_EXPORT;
    else if (cmd.HasOption(L"import"))
        operation = OperationType::OP_IMPORT;
    if (operation == OperationType::OP_NONE)
        return ErrorType::UNKNOWN_OPERATION_TYPE;
    path inputFolder;
    vector<path> inputFiles;
    bool isFolder = false;
    bool hasInput = cmd.HasArgument(L"i");
    if (!hasInput) {
        inputFolder = current_path();
        isFolder = true;
    }
    else {
        inputFiles = cmd.GetArgumentPaths(L"i");
        bool notExists = false;
        if (inputFiles.size() == 1) {
            if (!exists(inputFiles[0]))
                notExists = true;
            else {
                if (is_directory(inputFiles[0])) {
                    inputFolder = inputFiles[0];
                    inputFiles.clear();
                    isFolder = true;
                }
            }
        }
        else {
            for (auto const &i : inputFiles) {
                if (!exists(i)) {
                    notExists = true;
                    break;
                }
                else {
                    if (is_directory(i)) {
                        ErrorMessage("Directories are not allowed when multiple inputs are specified");
                        return ErrorType::INPUT_PATH_MULTIPLE_FOLDERS;
                    }
                }
            }
        }
        if (notExists) {
            ErrorMessage("Input path does not exist");
            return ErrorType::INVALID_INPUT_PATH;
        }
    }
    path o;
    bool hasOutput = cmd.HasArgument(L"o");
    if (hasOutput) {
        o = cmd.GetArgumentPath(L"o");
        if (exists(o))
            create_directories(o);
        else if (!is_directory(o)) {
            ErrorMessage("Output path is not a folder");
            return ErrorType::OUTPUT_PATH_IS_NOT_A_FOLDER;
        }
    }
    else {
        if (isFolder)
            o = inputFolder;
        else
            o = current_path();
    }

    Rx3Options rx3options;

    if (cmd.HasArgument(L"game"))
        rx3options.game = ToLower(WtoA(cmd.GetArgumentString(L"game")));
    if (!GameConfigs().contains(rx3options.game)) {
        ErrorMessage("Unknown game");
        return ErrorType::UNKNOWN_GAME_TAG;
    }
    rx3options.gameConfig = GameConfigs()[rx3options.game];

    if (cmd.HasArgument(L"model"))
        rx3options.modelFormat = ToLower(WtoA(cmd.GetArgumentString(L"model")));
    if (cmd.HasArgument(L"texture"))
        rx3options.textureFormat = ToLower(WtoA(cmd.GetArgumentString(L"texture")));
    if (cmd.HasArgument(L"skeleton"))
        rx3options.skeletonPath = cmd.GetArgumentPath(L"skeleton");
    if (cmd.HasArgument(L"folderOption")) {
        auto strFolderOption = ToLower(cmd.GetArgumentString(L"folderOption"));
        if (strFolderOption == L"alwaysCreate")
            rx3options.folderOption = FOLDER_OPTION_ALWAYS_CREATE;
        else if (strFolderOption == L"neverCreate")
            rx3options.folderOption = FOLDER_OPTION_NEVER_CREATE;
    }
    if (cmd.HasArgument(L"skeleton"))
        rx3options.skeletonPath = cmd.GetArgumentPath(L"skeleton");
    rx3options.exportQuads = cmd.HasOption(L"exportQuads");
    rx3options.writeHDR = cmd.HasOption(L"writeHDR");
    rx3options.writeTexMetadata = cmd.HasOption(L"writeTexMetadata");
    if (cmd.HasArgument(L"texFormatFile")) {
        vector<string> order;
        ReadTexFormatFile(cmd.GetArgumentPath(L"texFormatFile"), rx3options.texTargetFormats, order);
    }
    bool noMetadata = cmd.HasOption(L"noMetadata");

    auto ExportRX3 = [&](path const &in, path const &outFolder) {
        Rx3Container rx3(in);
        bool createFolder = rx3options.folderOption == FOLDER_OPTION_ALWAYS_CREATE ||
            (rx3options.folderOption == FOLDER_OPTION_AUTO && rx3.FindFirstChunk(RX3_CHUNK_TEXTURE_BATCH));
        path outDir = createFolder ? (outFolder / rx3.mName) : outFolder;
        ExtractTexturesFromRX3(rx3, outDir, rx3options);
        ExtractHotspotFromRX3(rx3, outDir, rx3options);
        ExtractModelFromRX3(rx3, outDir, rx3options);
    };

    auto ImportRX3 = [&](vector<path> const &inFiles, wstring const &rx3DefaultName, path const &outFolder) {
        vector<path> inTextures;
        vector<path> inModels;
        path inHotspot, inMetadata;
        std::map<wstring, vector<path>> textureGroups;
        vector<path> tempHotspots;
        for (auto const &file : inFiles) {
            wstring ext = ToLower(file.extension().wstring());
            if (ext == L".fbx")
                inModels.push_back(file);
            else if (ext == L".hotspot")
                tempHotspots.push_back(file);
            else if (ext == L".dds" || ext == L".png" || ext == L".tga" || ext == L".hdr") {
                path groupKey = file.parent_path() / ToLower(file.stem().wstring());
                textureGroups[groupKey.wstring()].push_back(file);
            }
        }
        if (!tempHotspots.empty()) {
            inHotspot = tempHotspots.front();
            for (auto const &hs : tempHotspots) {
                if (ToLower(hs.stem().wstring()) == ToLower(rx3DefaultName)) {
                    inHotspot = hs;
                    break;
                }
            }
        }
        vector<wstring> extPriority = { L".dds", L".hdr", L".png", L".tga" };
        if (!rx3options.textureFormat.empty()) {
            wstring prefExt = L"." + AtoW(rx3options.textureFormat);
            prefExt = ToLower(prefExt);
            auto it = std::find(extPriority.begin(), extPriority.end(), prefExt);
            if (it != extPriority.end())
                extPriority.erase(it);
            extPriority.insert(extPriority.begin(), prefExt);
        }
        for (auto const &[groupKey, groupFiles] : textureGroups) {
            if (groupFiles.size() == 1)
                inTextures.push_back(groupFiles.front());
            else {
                path bestFile;
                size_t bestRank = extPriority.size() + 1;
                for (auto const &file : groupFiles) {
                    wstring ext = ToLower(file.extension().wstring());
                    auto it = std::find(extPriority.begin(), extPriority.end(), ext);
                    size_t rank = (it != extPriority.end()) ? std::distance(extPriority.begin(), it) : extPriority.size();
                    if (rank < bestRank) {
                        bestRank = rank;
                        bestFile = file;
                    }
                }
                inTextures.push_back(bestFile);
            }
        }
        bool hasNameCollision = false;
        for (auto const &inModel : inModels) {
            if (ToLower(inModel.stem().wstring()) == ToLower(rx3DefaultName)) {
                hasNameCollision = true;
                break;
            }
        }
        if (!inTextures.empty()) {
            Rx3Container rx3;
            rx3.AddChunk(RX3_CHUNK_TEXTURE_BATCH);
            ImportTexturesToRX3(rx3, inTextures, inMetadata, rx3options);
            if (!inHotspot.empty())
                ImportHotspotToRX3(rx3, inHotspot, rx3options);
            wstring textureFileName = rx3DefaultName;
            if (hasNameCollision) {
                textureFileName += L"_textures";
            }
            rx3.Save(outFolder / (textureFileName + L".rx3"));
        }
        if (!inModels.empty()) {
            for (auto const &inModel : inModels) {
                //Rx3Container rx3;
                //rx3.AddChunk(RX3_CHUNK_INDEX_BUFFER_BATCH);
                //if (rx3options.gameConfig.QuadMeshes)
                //    rx3.AddChunk(RX3_CHUNK_QUAD_INDEX_BUFFER_BATCH);
                //rx3.Save(outFolder / (inModel.stem().wstring() + L".rx3"));
            }
        }
    };

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        return FAILED_TO_INITIALIZE;
    if (operation == OperationType::OP_EXPORT) {
        if (isFolder) {
            vector<path> filesToProcess;
            if (cmd.HasOption(L"recursive")) {
                for (auto const &p : recursive_directory_iterator(inputFolder)) {
                    if (!is_directory(p) && ToLower(p.path().extension().wstring()) == L".rx3")
                        filesToProcess.push_back(p.path());
                }
            }
            else {
                for (auto const &p : directory_iterator(inputFolder)) {
                    if (!is_directory(p) && ToLower(p.path().extension().wstring()) == L".rx3")
                        filesToProcess.push_back(p.path());
                }
            }
            for (auto const &p : filesToProcess) {
                auto rel = relative(p, inputFolder).parent_path();
                auto outSubFolder = o / rel;
                ExportRX3(p, outSubFolder);
            }
        }
        else {
            for (auto const &f : inputFiles)
                ExportRX3(f, o);
        }
    }
    else if (operation == OperationType::OP_IMPORT) {
        if (isFolder) {
            if (cmd.HasOption(L"recursive")) {
                map<path, vector<path>> filesByDirectory;
                for (auto const &p : recursive_directory_iterator(inputFolder)) {
                    if (is_regular_file(p))
                        filesByDirectory[p.path().parent_path()].push_back(p.path());
                }
                for (auto const &[dirPath, files] : filesByDirectory)
                    ImportRX3(files, dirPath.stem().wstring(), o);
            }
            else {
                vector<path> filesToProcess;
                for (auto const &p : directory_iterator(inputFolder)) {
                    if (is_regular_file(p))
                        filesToProcess.push_back(p.path());
                }
                if (!filesToProcess.empty())
                    ImportRX3(filesToProcess, inputFolder.stem().wstring(), o);
            }
        }
        else {
            if (!inputFiles.empty()) {
                wstring defaultName = L"unnamed";
                if (inputFiles[0].has_parent_path())
                    defaultName = inputFiles[0].parent_path().wstring();
                ImportRX3(inputFiles, defaultName, o);
            }
        }
    }
    CoUninitialize();
    return ErrorType::NONE;
}
