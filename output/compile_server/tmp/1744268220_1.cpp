#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
class Solution
{
public:
    bool isPalindrome(int x)
    {
        if (x < 0 || (x % 10 == 0 && x != 0)) {
            return false;
        }

        int reversed_half = 0;
        while (x > reversed_half) {
            reversed_half = reversed_half * 10 + x % 10;
            x /= 10;
        }

        return x == reversed_half || x == reversed_half / 10;
    
    }
};
// 工具函数：断言实际值是否等于期望值
bool assertEqual(bool actual, bool expected)
{
    return actual == expected;
}

// 测试用例，每个包含函数+描述字符串
using TestCase = std::pair<std::function<bool()>, std::string>;
using VTests = std::vector<TestCase>;

// 保存测试结果到文件
int saveResult(VTests &tests, const std::string &result_path)
{
    int total_tests = tests.size();
    int passed_tests = 0;
    std::string first_failed_input;

    for (const auto &test : tests)
    {
        if (test.first())
        {
            passed_tests++;
        }
        else if (first_failed_input.empty())
        {
            first_failed_input = test.second;
        }
    }

    // 创建或写入文件
    int fd = open(result_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0)
    {
        std::string result = std::to_string(passed_tests) + " " +
                             std::to_string(total_tests) + " " +
                             first_failed_input;
        write(fd, result.c_str(), result.size());
        close(fd);
    }

    return (passed_tests == total_tests) ? 0 : 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {  
        std::cerr << "Usage: " << argv[0] << " <result_file_path>\n";
        return 1;
    }


        std::string result_path = argv[1];

        VTests tests = {
            {[]()
             { return assertEqual(Solution().isPalindrome(121), true); }, "121"},
            {[]()
             { return assertEqual(Solution().isPalindrome(-10), false); }, "-10"},
            {[]()
             { return assertEqual(Solution().isPalindrome(12321), true); }, "12321"}};

        return saveResult(tests, result_path);
    
}
