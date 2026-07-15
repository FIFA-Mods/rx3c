#pragma once
#include <string>
#include <map>
#include <set>
#include <vector>
#include <filesystem>

class CommandLine {
    std::set<std::wstring> mOptions;
    std::map<std::wstring, std::vector<std::wstring>> mArguments;

public:
    static std::wstring ToLower(std::wstring const &str);
    CommandLine(int argc, wchar_t *argv[], std::set<std::wstring> const &arguments, std::set<std::wstring> const &options);
    bool HasOption(std::wstring const &option) const;
    bool HasArgument(std::wstring const &argument) const;
    std::wstring GetArgumentString(std::wstring const &argument, std::wstring const &defaultValue = L"") const;
    std::filesystem::path GetArgumentPath(std::wstring const &argument, std::filesystem::path const &defaultValue = {}) const;
    int GetArgumentInt(std::wstring const &argument, int defaultValue = -1) const;
    float GetArgumentFloat(std::wstring const &argument, float defaultValue = 0.0f) const;
    std::vector<std::wstring> GetArgumentStrings(std::wstring const &argument) const;
    std::vector<std::filesystem::path> GetArgumentPaths(std::wstring const &argument) const;
};
