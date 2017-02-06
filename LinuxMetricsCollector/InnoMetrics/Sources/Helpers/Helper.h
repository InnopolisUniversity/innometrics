//
// Created by alon on 25.10.16.
//

#ifndef INNOMETRICS2_HELPER_H
#define INNOMETRICS2_HELPER_H
#include <string>
#include <cstring>      // strcpy
#include <stdarg.h>     // va_start, etc.
#include <memory>       // std::unique_ptr
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

class Helper {
public:
    static bool replace(std::string& str, const std::string& from, const std::string& to);
    static bool replace(std::wstring& str, const std::wstring& from, const std::wstring& to);
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    //Format print for std::string
    static std::string string_format(const std::string fmt_str, ...);

    // trim from start (in place)
    static void ltrim(std::string &s);
    // trim from end (in place)
    static void rtrim(std::string &s);
    // trim from both ends (in place)
    static void trim(std::string &s);
    // trim from start (copying)
    static std::string ltrimmed(std::string s);
    // trim from end (copying)
    static std::string rtrimmed(std::string s);
    // trim from both ends (copying)
    static std::string trimmed(std::string s);
};


#endif //INNOMETRICS2_HELPER_H
