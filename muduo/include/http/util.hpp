
#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include "../comm/log.hpp"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
class util
{

public:
    static bool ReadFile(const std::string &filePath, std::string *out)
    {
        std::ifstream file(filePath, std::ios::binary);
        if (!file)
        {
            LOG_ERROR("无法打开文件 file_name : %s", filePath.c_str());
            return false;
        }

        // 读取文件内容
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string data(buffer.begin(), buffer.end());
        *out = data;
        return true;
    }
    // 字符串分隔函数
    static void SplitString(const std::string &str, const std::string &sep, std::vector<std::string> *out)
    {

        size_t start = 0;
        size_t end = str.find(sep);

        while (end != std::string::npos)
        {
            // 提取子字符串并忽略空字符串
            std::string token = str.substr(start, end - start);
            if (!token.empty())
            {
                out->push_back(token);
            }

            start = end + sep.length(); // 更新起始位置
            end = str.find(sep, start); // 查找下一个分隔符
        }

        // 处理最后一个子字符串
        std::string token = str.substr(start);
        if (!token.empty())
        {
            out->push_back(token);
        }
    }

    static bool WriteFile(const std::string &file_name, const std::string &content)
    {
        std::cout << file_name.c_str() << std::endl;
        std::cout << content.c_str() << std::endl;
        int fd = open(file_name.c_str(), O_RDWR | O_CREAT, 0644);
        if (fd < 0)
        {
            LOG_ERROR("打开文件失败,errno: %d error message : %s", errno, strerror(errno));
            return false;
        }
        int n = write(fd, content.c_str(), content.size());

        ::close(fd);

        return true;
    }
    // url编码 为了避免url中的查询字符和http请求中的特殊字符产生歧义，对查询字符串进行编码
    // 编码格式 将特殊字符的ASCII码值转为两个16进制的字符,前置加上%
    // .-~字母数字属于不编码字符
    // 编码格式%HH
    //  查询字符串中的空格变为+
    //特殊字符：$-_.+!*’(),
    //保留字符：&/:;=?@
    static std::string urlEnCode(const std::string &url, bool space_to_plus)
    {
        std::string res;

        for (auto &u : url)
        {
            if (u == '.' || u == '~' || u == '_' || isalnum(u))
            {
                res += u;
                continue;
            }
            else if (u == ' ' && space_to_plus == true)
            {
                res += '+';
            }
            else
            {

                char tmp[4] = {0};
                sprintf(tmp, "%%%02X", u);
                res += tmp;
            }
        }
        return res;
    }
    // url解码
    static char HexToi(char c)
    {
        if (c >= '0' && c <= '9')
        {
            return c - '0';
        }

        else if (c >= 'a' && c <= 'z')
        {

            return c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'Z')
        {

            return c - 'A' + 10;
        }
        else
        {

            return -1;
        }
    }
    static std::string urlDeCode(const std::string &url, bool space_to_plus)
    {

        std::string res;

        for (int i = 0; i < url.size(); i++)
        {

            if (url[i] == '%' && i + 2 < url.size())
            {
                char n1 = url[i + 1];
                char n2 = url[i + 2];
                char result = (HexToi(n1) << 4) + HexToi(n2);
                res += result;
                i += 2;
            }
            else if (url[i] == '+' && space_to_plus == true)
            {
                res += ' ';
            }
            else
            {
                res += url[i];
            }
        }

        return res;
    }

    // 状态码转状态信息

    static std::string getStatusMessage(int statusCode)
    {
        // 定义一个 unordered_map 映射状态码到状态消息
        static const std::unordered_map<int, std::string> statusMessages = {
            // 1xx Informational
            {100, "Continue"},
            {101, "Switching Protocols"},
            {102, "Processing"},

            // 2xx Success
            {200, "OK"},
            {201, "Created"},
            {202, "Accepted"},
            {203, "Non-Authoritative Information"},
            {204, "No Content"},
            {205, "Reset Content"},
            {206, "Partial Content"},
            {207, "Multi-Status"},
            {208, "Already Reported"},
            {226, "IM Used"},

            // 3xx Redirection
            {300, "Multiple Choices"},
            {301, "Moved Permanently"},
            {302, "Found"},
            {303, "See Other"},
            {304, "Not Modified"},
            {305, "Use Proxy"},
            {306, "(Unused)"},
            {307, "Temporary Redirect"},
            {308, "Permanent Redirect"},

            // 4xx Client Errors
            {400, "Bad Request"},
            {401, "Unauthorized"},
            {402, "Payment Required"},
            {403, "Forbidden"},
            {404, "Not Found"},
            {405, "Method Not Allowed"},
            {406, "Not Acceptable"},
            {407, "Proxy Authentication Required"},
            {408, "Request Timeout"},
            {409, "Conflict"},
            {410, "Gone"},
            {411, "Length Required"},
            {412, "Precondition Failed"},
            {413, "Payload Too Large"},
            {414, "URI Too Long"},
            {415, "Unsupported Media Type"},
            {416, "Range Not Satisfiable"},
            {417, "Expectation Failed"},
            {418, "I'm a teapot (RFC 2324)"},
            {421, "Misdirected Request"},
            {422, "Unprocessable Entity"},
            {423, "Locked"},
            {424, "Failed Dependency"},
            {425, "Too Early"},
            {426, "Upgrade Required"},
            {428, "Precondition Required"},
            {429, "Too Many Requests"},
            {431, "Request Header Fields Too Large"},
            {451, "Unavailable For Legal Reasons"},

            // 5xx Server Errors
            {500, "Internal Server Error"},
            {501, "Not Implemented"},
            {502, "Bad Gateway"},
            {503, "Service Unavailable"},
            {504, "Gateway Timeout"},
            {505, "HTTP Version Not Supported"},
            {506, "Variant Also Negotiates"},
            {507, "Insufficient Storage"},
            {508, "Loop Detected"},
            {510, "Not Extended"},
            {511, "Network Authentication Required"}};

        // 查找状态码对应的状态信息
        auto it = statusMessages.find(statusCode);
        if (it != statusMessages.end())
        {
            return it->second; // 返回对应的状态信息
        }
        else
        {
            return "Unknown Status Code"; // 如果没有找到，返回未知状态信息
        }
    }
    static std::string GetMime(const std::string &filename)
    {

        static const std::unordered_map<std::string, std::string> mimeTypes = {
            {".html", "text/html"},
            {".htm", "text/html"},
            {".css", "text/css"},
            {".js", "application/javascript"},
            {".json", "application/json"},
            {".xml", "application/xml"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".png", "image/png"},
            {".gif", "image/gif"},
            {".bmp", "image/bmp"},
            {".tiff", "image/tiff"},
            {".svg", "image/svg+xml"},
            {".pdf", "application/pdf"},
            {".txt", "text/plain"},
            {".csv", "text/csv"},
            {".zip", "application/zip"},
            {".tar", "application/x-tar"},
            {".gz", "application/gzip"},
            {".mp3", "audio/mpeg"},
            {".wav", "audio/wav"},
            {".mp4", "video/mp4"},
            {".avi", "video/x-msvideo"},
            {".mkv", "video/x-matroska"},
            {".webm", "video/webm"},
            {".flv", "video/x-flv"},
            {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
            {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
            {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
            {".rar", "application/vnd.rar"},
            {".7z", "application/x-7z-compressed"}};

        // 获取文件扩展名的位置
        size_t pos = filename.find_last_of(".");
        if (pos == std::string::npos)
        {
            return "application/octet-stream"; // 如果没有扩展名，返回默认 MIME 类型
        }

        // 提取文件扩展名（包括 .）
        std::string extension = filename.substr(pos);

        // 查找扩展名对应的 MIME 类型
        auto it = mimeTypes.find(extension);
        if (it != mimeTypes.end())
        {
            return it->second; // 返回对应的 MIME 类型
        }
        else
        {
            return "application/octet-stream"; // 如果扩展名没有匹配，返回默认 MIME 类型
        }
    }

    static bool isDirectory(const std::string &path)
    {
        struct stat st;
        int ret = stat(path.c_str(), &st);
        if (ret < 0)
        {
            return false;
        }

        return S_ISDIR(st.st_mode);
    }

    // 是否是常规文件
    static bool isRegular(const std::string &path)
    {
        struct stat st;
        int ret = stat(path.c_str(), &st);
        if (ret < 0)
        {
            return false;
        }

        return S_ISREG(st.st_mode);
    }
    // 路径合法有效性判断
    // 发过来的路径都会默认拼相对根目录......../wwwroot

    static bool isValidPath(const std::string &path)
    {
        int level = 0;

        std::vector<std::string> subdir;

        SplitString(path, "/", &subdir);
        for (auto &d : subdir)
        {
            if (d == "..")
            {
                level--;
                if (level < 0)
                    return false;
            }
            else if (d == ".")
            {
                continue;
            }
            else
            {

                level++;
            }
        }
        return true;
    }
};
