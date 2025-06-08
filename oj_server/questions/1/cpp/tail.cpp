
bool assertEqual(bool actual, bool expected)
{
    return actual == expected;
}

using TestCase = std::pair<std::function<bool()>, std::string>;
using VTests = std::vector<TestCase>;

int saveResult(VTests &tests, const std::string &result_path)
{
    int total_tests = tests.size(), passed_tests = 0;
    std::string first_failed_input;

    for (auto &t : tests)
        if (t.first()) passed_tests++;
        else if (first_failed_input.empty()) first_failed_input = t.second;

    int fd = open(result_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        std::string r = std::to_string(passed_tests) + " " +
                        std::to_string(total_tests) + " " +
                        first_failed_input;
        write(fd, r.c_str(), r.size());
        close(fd);
    }
    return (passed_tests == total_tests) ? 0 : 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;
    std::string p = argv[1];
    VTests tests = {
        {[]{ return assertEqual(Solution().isPalindrome(0), true); }, "0"},
        {[]{ return assertEqual(Solution().isPalindrome(-1), false); }, "-1"},
        {[]{ return assertEqual(Solution().isPalindrome(5), true); }, "5"},
        {[]{ return assertEqual(Solution().isPalindrome(10), false); }, "10"},
        {[]{ return assertEqual(Solution().isPalindrome(121), true); }, "121"},
        {[]{ return assertEqual(Solution().isPalindrome(-121), false); }, "-121"},
        {[]{ return assertEqual(Solution().isPalindrome(12321), true); }, "12321"},
        {[]{ return assertEqual(Solution().isPalindrome(1231), false); }, "1231"},
        {[]{ return assertEqual(Solution().isPalindrome(1001), true); }, "1001"},
        {[]{ return assertEqual(Solution().isPalindrome(2147447412), true); }, "2147447412"}
    };
    return saveResult(tests, p);
}