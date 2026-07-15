#include "CommandLine.h"
#include <cwctype>

std::wstring CommandLine::ToLower(std::wstring const &str) {
    std::wstring result = str;
    for (auto &c : result)
        c = static_cast<wchar_t>(std::towlower(c));
    return result;
}

CommandLine::CommandLine(int argc, wchar_t *argv[], std::set<std::wstring> const &arguments, std::set<std::wstring> const &options) {
    std::set<std::wstring> _arguments;
    std::set<std::wstring> _options;
    for (auto const &s : arguments) _arguments.insert(ToLower(s));
    for (auto const &s : options)   _options.insert(ToLower(s));
    for (int i = 1; i < argc; i++) {
        std::wstring arg = argv[i];
        if (arg.starts_with(L'-') || arg.starts_with(L'/')) {
            arg = ToLower(arg.substr(1));
            if (_arguments.contains(arg)) {
                if ((i + 1) < argc) {
                    mArguments[arg].push_back(argv[i + 1]);
                    i++;
                }
            }
            else if (_options.contains(arg))
                mOptions.insert(arg);
        }
    }
}

bool CommandLine::HasOption(std::wstring const &option) const {
    return mOptions.contains(ToLower(option));
}

bool CommandLine::HasArgument(std::wstring const &argument) const {
    return mArguments.contains(ToLower(argument));
}

std::wstring CommandLine::GetArgumentString(std::wstring const &argument, std::wstring const &defaultValue) const {
    auto it = mArguments.find(ToLower(argument));
    if (it != mArguments.end() && !it->second.empty()) {
        return it->second.front();
    }
    return defaultValue;
}

std::filesystem::path CommandLine::GetArgumentPath(std::wstring const &argument, std::filesystem::path const &defaultValue) const {
    auto str = GetArgumentString(argument);
    return str.empty() ? defaultValue : std::filesystem::path(str);
}

int CommandLine::GetArgumentInt(std::wstring const &argument, int defaultValue) const {
    auto str = GetArgumentString(argument);
    if (!str.empty()) {
        try { return std::stoi(str); }
        catch (...) {}
    }
    return defaultValue;
}

float CommandLine::GetArgumentFloat(std::wstring const &argument, float defaultValue) const {
    auto str = GetArgumentString(argument);
    if (!str.empty()) {
        try { return std::stof(str); }
        catch (...) {}
    }
    return defaultValue;
}

std::vector<std::wstring> CommandLine::GetArgumentStrings(std::wstring const &argument) const {
    auto it = mArguments.find(ToLower(argument));
    if (it != mArguments.end())
        return it->second;
    return {};
}

std::vector<std::filesystem::path> CommandLine::GetArgumentPaths(std::wstring const &argument) const {
    std::vector<std::filesystem::path> paths;
    auto strings = GetArgumentStrings(argument);
    paths.reserve(strings.size());
    for (auto const &s : strings)
        paths.emplace_back(s);
    return paths;
}
