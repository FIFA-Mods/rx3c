#include "commandline.h"
#include "errormsg.h"
#include "Model.h"
#include "Rx3Model.h"
#include "Rx3Textures.h"
#include "TextFileTable.h"
#include <string>
#include <windows.h>
#include <shobjidl.h>

const char *RX3C_VERSION = "0.100";
const unsigned int RX3C_VERSION_INT = 100;

enum ErrorType {
    NONE = 0,
    NOT_ENOUGHT_ARGUMENTS = 1,
    UNKNOWN_OPERATION_TYPE = 2,
    NO_INPUT_PATH = 3,
    INVALID_INPUT_PATH = 4,
    INVALID_OUTPUT_PATH = 5,
    FAILED_TO_INITIALIZE = 6,
    ERROR_OTHER = 7
};

enum OperationType {
    OP_NONE = 0,
    OP_EXPORT = 1,
    OP_IMPORT = 2
};

int wmain(int argc, wchar_t *argv[]) {
    CommandLine cmd(argc, argv,
        { L"i", L"o", L"game", L"skeleton", L"model", L"texture", L"folderOption", L"texFormatFile"},
        { L"export", L"import", L"recursive", L"silent", L"console", L"exportQuads", L"writeHDR", L"writeTexMetadata"}
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
    path i;
    bool hasInput = cmd.HasArgument(L"i");
    if (!hasInput)
        i = current_path();
    else {
        i = cmd.GetArgumentPath(L"i");
        if (!exists(i)) {
            ErrorMessage("Input path does not exist");
            return ErrorType::INVALID_INPUT_PATH;
        }
    }
    bool isFolder = is_directory(i);
    path o;
    bool hasOutput = cmd.HasArgument(L"o");
    if (hasOutput) {
        o = cmd.GetArgumentPath(L"o");
        if (!exists(o)) {
            //ErrorMessage("Output path does not exist");
            //return ErrorType::INVALID_OUTPUT_PATH;
            create_directories(o);
        }
    }
    else {
        if (isFolder)
            o = i;
        else {
            if (i.has_parent_path())
                o = i.parent_path();
            else
                o = current_path();
        }
    }

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        return FAILED_TO_INITIALIZE;

    Rx3Options rx3options;
    if (cmd.HasArgument(L"game"))
        rx3options.game = ToLower(WtoA(cmd.GetArgumentString(L"game")));
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
    if (cmd.HasArgument(L"texFormatFile"))
        ReadTexFormatFile(cmd.GetArgumentPath(L"texFormatFile"), rx3options.texTargetFormats);

    auto ExportRX3 = [&](path const &in, path const &outFolder) {
        Model m = ModelFromRX3(in, rx3options);
        if (!m.objects.empty() || !m.skeleton.bones.empty()) {
            path modelDirctory = outFolder;
            if (rx3options.folderOption == FOLDER_OPTION_ALWAYS_CREATE)
                modelDirctory /= in.stem();
            if (!exists(modelDirctory))
                create_directories(modelDirctory);
            m.WriteFbx(modelDirctory / (in.stem().wstring() + L".fbx"), rx3options.modelFormat == "fbxascii");
        }
        path textureDirectory = outFolder;
        if (rx3options.folderOption != FOLDER_OPTION_NEVER_CREATE)
            textureDirectory /= in.stem();
        ExtractTexturesFromRX3(in, textureDirectory, rx3options);
    };

    if (operation == OperationType::OP_EXPORT) {
        if (isFolder) {
            vector<path> filesToProcess;
            if (cmd.HasOption(L"recursive")) {
                for (auto const &p : recursive_directory_iterator(i)) {
                    if (!is_directory(p) && ToLower(p.path().extension().wstring()) == L".rx3")
                        filesToProcess.push_back(p.path());
                }
            }
            else {
                for (auto const &p : directory_iterator(i)) {
                    if (!is_directory(p) && ToLower(p.path().extension().wstring()) == L".rx3")
                        filesToProcess.push_back(p.path());
                }
            }
            for (auto const &p : filesToProcess) {
                auto rel = relative(p, i).parent_path();
                auto outSubFolder = o / rel;
                ExportRX3(p, outSubFolder);
            }
        }
        else {
            ExportRX3(i, o);
        }
    }
    CoUninitialize();
    return ErrorType::NONE;
}
