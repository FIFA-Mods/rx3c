#include "rx3utils.h"

unsigned int FormattingUtils::currentBuf = 0;
char FormattingUtils::buf[FormattingUtils::BUF_SIZE][4096];
unsigned int FormattingUtils::currentBufW = 0;
wchar_t FormattingUtils::bufW[FormattingUtils::BUF_SIZE][4096];

wstring AtoW(string const &str) {
    wstring result;
    result.resize(str.size());
    for (unsigned int i = 0; i < str.size(); i++)
        result[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
    return result;
}

string WtoA(wstring const &str) {
    string result;
    result.resize(str.size());
    for (unsigned int i = 0; i < str.size(); i++)
        result[i] = static_cast<char>(static_cast<unsigned char>(str[i]));
    return result;
}

string ToUpper(string const &str) {
    string result;
    for (size_t i = 0; i < str.length(); i++)
        result += toupper(static_cast<unsigned char>(str[i]));
    return result;
}

string ToLower(string const &str) {
    string result;
    for (size_t i = 0; i < str.length(); i++)
        result += tolower(static_cast<unsigned char>(str[i]));
    return result;
}

wstring ToUpper(wstring const &str) {
    wstring result;
    for (size_t i = 0; i < str.length(); i++)
        result += toupper(static_cast<unsigned short>(str[i]));
    return result;
}

wstring ToLower(wstring const &str) {
    wstring result;
    for (size_t i = 0; i < str.length(); i++)
        result += tolower(static_cast<unsigned short>(str[i]));
    return result;
}

void Replace(string &str, const string &from, const string &to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void Replace(wstring &str, const wstring &from, const wstring &to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void Trim(string &str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start != string::npos)
        str = str.substr(start);
    size_t end = str.find_last_not_of(" \t\r\n");
    if (end != string::npos)
        str = str.substr(0, end + 1);
}

void Trim(wstring &str) {
    size_t start = str.find_first_not_of(L" \t\r\n");
    if (start != wstring::npos)
        str = str.substr(start);
    size_t end = str.find_last_not_of(L" \t\r\n");
    if (end != wstring::npos)
        str = str.substr(0, end + 1);
}

vector<string> Split(string const &line, char delim, bool trim, bool skipEmpty, bool quotesHavePriority) {
    vector<string> result;
    string currStr;
    auto AddStr = [&, trim, skipEmpty]() {
        if (trim)
            Trim(currStr);
        if (!skipEmpty || !currStr.empty())
            result.push_back(currStr);
        currStr.clear();
    };
    bool inQuotes = false;
    for (size_t i = 0; i < line.length(); i++) {
        auto c = line[i];
        if (c == '\r' || (delim != '\n' && c == '\n'))
            break;
        if (!inQuotes) {
            if (quotesHavePriority && c == '"')
                inQuotes = true;
            else if (c == delim)
                AddStr();
            else
                currStr += c;
        }
        else {
            if (c == '"')
                inQuotes = false;
            else
                currStr += c;
        }
    }
    AddStr();
    return result;
}

vector<wstring> Split(wstring const &line, wchar_t delim, bool trim, bool skipEmpty, bool quotesHavePriority) {
    vector<wstring> result;
    wstring currStr;
    auto AddStr = [&, trim, skipEmpty]() {
        if (trim)
            Trim(currStr);
        if (!skipEmpty || !currStr.empty())
            result.push_back(currStr);
        currStr.clear();
    };
    bool inQuotes = false;
    for (size_t i = 0; i < line.length(); i++) {
        auto c = line[i];
        if (c == L'\r' || (delim != L'\n' && c == L'\n'))
            break;
        if (!inQuotes) {
            if (quotesHavePriority && c == L'"')
                inQuotes = true;
            else if (c == delim)
                AddStr();
            else
                currStr += c;
        }
        else {
            if (c == L'"')
                inQuotes = false;
            else
                currStr += c;
        }
    }
    AddStr();
    return result;
}

unsigned int Hash(string const &str) {
    unsigned int hash = 0;
    for (auto const &c : str) {
        hash += c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

UINT MessageIcon(unsigned int iconType) {
    if (iconType == 1)
        return MB_ICONWARNING;
    else if (iconType == 2)
        return MB_ICONERROR;
    return MB_ICONINFORMATION;
}

void FormattingUtils::WindowsMessageBoxA(char const *msg, char const *title, unsigned int icon) {
    MessageBoxA(GetActiveWindow(), msg, title, MessageIcon(icon));
}

void FormattingUtils::WindowsMessageBoxW(wchar_t const *msg, wchar_t const *title, unsigned int icon) {
    MessageBoxW(GetActiveWindow(), msg, title, MessageIcon(icon));
}

char *FormattingUtils::GetBuf() {
    char *result = buf[currentBuf];
    currentBuf += 1;
    if (currentBuf >= BUF_SIZE)
        currentBuf = 0;
    return result;
}

wchar_t *FormattingUtils::GetBufW() {
    wchar_t *result = bufW[currentBufW];
    currentBufW += 1;
    if (currentBufW >= BUF_SIZE)
        currentBufW = 0;
    return result;
}

float SafeConvertFloat(wstring const &str) {
    float result = 0.0f;
    try {
        result = stof(str);
    }
    catch (...) {}
    return result;
}

float SafeConvertFloat(string const &str) {
    float result = 0.0f;
    try {
        result = stof(str);
    }
    catch (...) {}
    return result;
}

double SafeConvertDouble(wstring const &str) {
    double result = 0.0;
    try {
        result = stod(str);
    }
    catch (...) {}
    return result;
}

double SafeConvertDouble(string const &str) {
    double result = 0.0;
    try {
        result = stod(str);
    }
    catch (...) {}
    return result;
}

wstring GetStringWithoutUnicodeChars(wstring const &src) {
    wstring str = src;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == L'ð')
            str[i] = L'o';
        else if (str[i] == L'ß')
            str[i] = L's';
        else if (str[i] == L'Þ')
            str[i] = L'P';
        else if (str[i] == L'ț')
            str[i] = L't';
        else if (str[i] == L'Ț')
            str[i] = L'T';
    }
    int mbSize = WideCharToMultiByte(20127, 0, str.c_str(), -1, NULL, 0, "?", NULL);
    char *mb = new char[mbSize];
    WideCharToMultiByte(20127, 0, str.c_str(), -1, mb, mbSize, "?", NULL);
    int wcSize = MultiByteToWideChar(20127, 0, mb, -1, NULL, 0);
    wchar_t *wc = new wchar_t[wcSize];
    MultiByteToWideChar(20127, 0, mb, -1, wc, wcSize);
    str = wc;
    delete[] mb;
    delete[] wc;
    return str;
}

vector<wstring> FileToLinesW(path const &filePath, wstring const &commentLineBegin) {
    vector<wstring> lines;
    FILE *file = _wfopen(filePath.c_str(), L"rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long fileSizeWithBom = ftell(file);
        fseek(file, 0, SEEK_SET);

        enum class encoding { ascii, utf8, utf16le, utf16be } enc = encoding::ascii;

        long numBytesToSkip = 0;
        if (fileSizeWithBom >= 2) {
            unsigned char bom[3];
            bom[0] = 0;
            fread(&bom, 1, 2, file);
            fseek(file, 0, SEEK_SET);
            if (bom[0] == 0xFE && bom[1] == 0xFF) {
                enc = encoding::utf16be;
                numBytesToSkip = 2;
            }
            else if (bom[0] == 0xFF && bom[1] == 0xFE) {
                enc = encoding::utf16le;
                numBytesToSkip = 2;
            }
            else if (fileSizeWithBom >= 3) {
                bom[0] = 0;
                fread(&bom, 1, 3, file);
                fseek(file, 0, SEEK_SET);
                if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
                    enc = encoding::utf8;
                    numBytesToSkip = 3;
                }
            }
        }
        long totalSize = fileSizeWithBom - numBytesToSkip;
        char *fileData = new char[totalSize];
        fseek(file, numBytesToSkip, SEEK_SET);
        fread(fileData, 1, totalSize, file);
        fclose(file);
        long numWideChars = 0;
        switch (enc) {
        case encoding::ascii:
            numWideChars = totalSize;
            break;
        case encoding::utf8:
            numWideChars = MultiByteToWideChar(CP_UTF8, 0, fileData, totalSize, 0, 0);
            break;
        case encoding::utf16le:
        case encoding::utf16be:
            numWideChars = totalSize / 2;
            break;
        }

        wchar_t *data = new wchar_t[numWideChars];
        memset(data, 0, numWideChars * sizeof(wchar_t));

        switch (enc) {
        case encoding::ascii:
            MultiByteToWideChar(1252, 0, fileData, totalSize, data, numWideChars);
            break;
        case encoding::utf8:
            MultiByteToWideChar(CP_UTF8, 0, fileData, totalSize, data, numWideChars);
            break;
        case encoding::utf16le:
        case encoding::utf16be:
            memcpy(data, fileData, totalSize);
            break;
        }
        delete[] fileData;

        if (enc == encoding::utf16be) {
            for (long i = 0; i < numWideChars; i++)
                data[i] = (data[i] >> 8) | (data[i] << 8);
        }

        wstring currentLine;
        bool inComment = false;
        for (long i = 0; i < numWideChars; i++) {
            if (data[i] == L'\n') {
                if (inComment)
                    inComment = false;
                else {
                    lines.push_back(currentLine);
                    currentLine.clear();
                }
            }
            else if (data[i] == L'\r') {
                if ((i + 1) < numWideChars && data[i + 1] == L'\n')
                    i++;
                if (inComment)
                    inComment = false;
                else {
                    lines.push_back(currentLine);
                    currentLine.clear();
                }
            }
            else if (!inComment) {
                if (!commentLineBegin.empty() && !wcsncmp(&data[i], commentLineBegin.c_str(), commentLineBegin.size())) {
                    if (!currentLine.empty()) {
                        lines.push_back(currentLine);
                        currentLine.clear();
                    }
                    inComment = true;
                }
                else
                    currentLine += data[i];
            }
        }
        if (!inComment)
            lines.push_back(currentLine);
        delete[] data;
    }
    return lines;
}

vector<string> FileToLinesA(path const &filePath, string const &commentLineBegin) {
    vector<string> lines;
    FILE *file = _wfopen(filePath.c_str(), L"rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long fileSizeWithBom = ftell(file);
        fseek(file, 0, SEEK_SET);

        enum class encoding { ascii, utf8, utf16le, utf16be } enc = encoding::ascii;

        long numBytesToSkip = 0;
        if (fileSizeWithBom >= 2) {
            unsigned char bom[3];
            bom[0] = 0;
            fread(&bom, 1, 2, file);
            fseek(file, 0, SEEK_SET);
            if (bom[0] == 0xFE && bom[1] == 0xFF) {
                enc = encoding::utf16be;
                numBytesToSkip = 2;
            }
            else if (bom[0] == 0xFF && bom[1] == 0xFE) {
                enc = encoding::utf16le;
                numBytesToSkip = 2;
            }
            else if (fileSizeWithBom >= 3) {
                bom[0] = 0;
                fread(&bom, 1, 3, file);
                fseek(file, 0, SEEK_SET);
                if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
                    enc = encoding::utf8;
                    numBytesToSkip = 3;
                }
            }
        }
        long totalSize = fileSizeWithBom - numBytesToSkip;
        char *fileData = new char[totalSize];
        fseek(file, numBytesToSkip, SEEK_SET);
        fread(fileData, 1, totalSize, file);
        fclose(file);
        if (enc == encoding::utf16be) {
            wchar_t *fileDataW = (wchar_t *)fileData;
            long numCharsW = totalSize / 2;
            for (long i = 0; i < numCharsW; i++)
                fileDataW[i] = (fileDataW[i] >> 8) | (fileDataW[i] << 8);
        }
        long numChars = 0;
        switch (enc) {
        case encoding::ascii:
        case encoding::utf8:
            numChars = totalSize;
            break;
        case encoding::utf16le:
        case encoding::utf16be:
            numChars = WideCharToMultiByte(CP_UTF8, 0, (wchar_t *)fileData, totalSize, NULL, 0, NULL, NULL);
            break;
        }

        char *data = new char[numChars];
        memset(data, 0, numChars);

        switch (enc) {
        case encoding::ascii:
        case encoding::utf8:
            memcpy(data, fileData, totalSize);
            break;
        case encoding::utf16le:
        case encoding::utf16be:
            WideCharToMultiByte(CP_UTF8, 0, (wchar_t *)fileData, totalSize, data, numChars, NULL, NULL);
            break;
        }
        delete[] fileData;

        string currentLine;
        bool inComment = false;
        for (long i = 0; i < numChars; i++) {
            if (data[i] == '\n') {
                if (inComment)
                    inComment = false;
                else {
                    lines.push_back(currentLine);
                    currentLine.clear();
                }
            }
            else if (data[i] == '\r') {
                if ((i + 1) < numChars && data[i + 1] == '\n')
                    i++;
                if (inComment)
                    inComment = false;
                else {
                    lines.push_back(currentLine);
                    currentLine.clear();
                }
            }
            else if (!inComment) {
                if (!commentLineBegin.empty() && !strncmp(&data[i], commentLineBegin.c_str(), commentLineBegin.size())) {
                    if (!currentLine.empty()) {
                        lines.push_back(currentLine);
                        currentLine.clear();
                    }
                    inComment = true;
                }
                else
                    currentLine += data[i];
            }
        }
        if (!inComment)
            lines.push_back(currentLine);
        delete[] data;
    }
    return lines;
}

bool StartsWith(wstring const &str, wstring const &what) {
    return !str.compare(0, what.size(), what);
}

bool StartsWith(string const &str, string const &what) {
    return !str.compare(0, what.size(), what);
}

bool IsHexadecimalLetter(wchar_t c) {
    return (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
}

bool IsHexadecimalLetter(char c) {
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool IsNumber(wstring const &str, bool hexadecimal) {
    if (str.empty())
        return false;
    wstring cmpStr;
    if (hexadecimal && (StartsWith(str, L"0x") || StartsWith(str, L"0X")))
        cmpStr = str.substr(2);
    else
        cmpStr = str;
    for (wchar_t c : cmpStr) {
        if (isdigit(c))
            continue;
        if (hexadecimal && IsHexadecimalLetter(c))
            continue;
        return false;
    }
    return true;
}

bool IsNumber(string const &str, bool hexadecimal) {
    if (str.empty())
        return false;
    string cmpStr;
    if (hexadecimal && (StartsWith(str, "0x") || StartsWith(str, "0X")))
        cmpStr = str.substr(2);
    else
        cmpStr = str;
    for (char c : cmpStr) {
        if (isdigit(c))
            continue;
        if (hexadecimal && IsHexadecimalLetter(c))
            continue;
        return false;
    }
    return true;
}

namespace memory {
size_t GetNumBytesToAlign(size_t offset, size_t alignment) {
    size_t m = offset % alignment;
    return (m > 0) ? (alignment - m) : 0;
}

size_t GetAligned(size_t offset, size_t alignment) {
    return offset + GetNumBytesToAlign(offset, alignment);
}

void Memory_Fill(void *dst, int val, size_t size) {
    memset(dst, val, size);
}

void Memory_Zero(void *dst, size_t size) {
    Memory_Fill(dst, 0, size);
}

void Memory_Copy(void *dst, void const *src, size_t size) {
    memcpy(dst, src, size);
}
}
