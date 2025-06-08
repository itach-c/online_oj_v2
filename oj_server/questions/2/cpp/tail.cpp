
// ======== TAIL ========

bool assertEqual(const std::string &actual, const std::string &expected)
{
    return actual == expected;
}

using TestCase = std::pair<std::function<bool()>, std::string>;
using VTests = std::vector<TestCase>;

int saveResult(VTests &tests, const std::string &result_path)
{
    int total_tests = tests.size(), passed_tests = 0;
    std::string first_failed_input;
    for (auto &t : tests) {
        if (t.first()) passed_tests++;
        else if (first_failed_input.empty()) first_failed_input = t.second;
    }
    int fd = open(result_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        std::string r = std::to_string(passed_tests) + " " +
                        std::to_string(total_tests)  + " " +
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
        {[]{ return assertEqual(Solution().longestPalindrome("a"), "a"); }, "\"a\""},
        {[]{ return assertEqual(Solution().longestPalindrome("ab"), "a"); }, "\"ab\""},
        {[]{ return assertEqual(Solution().longestPalindrome("bb"), "bb"); }, "\"bb\""},
        {[]{ return assertEqual(Solution().longestPalindrome("aba"), "aba"); }, "\"aba\""},
        {[]{ return assertEqual(Solution().longestPalindrome("baab"), "baab"); }, "\"baab\""},
        {[]{ return assertEqual(Solution().longestPalindrome("cbbd"), "bb"); }, "\"cbbd\""},
        {[]{ return assertEqual(Solution().longestPalindrome("abacdfgdcaba"), "aba"); }, "\"abacdfgdcaba\""},
        {[]{ return assertEqual(Solution().longestPalindrome("abaxyz"), "aba"); }, "\"abaxyz\""},
        {[]{ return assertEqual(Solution().longestPalindrome("abaxyzzyxf"), "xyzzyx"); }, "\"abaxyzzyxf\""},
        {[]{ std::string s(500, 'a'); return assertEqual(Solution().longestPalindrome(s), s); }, "500×\"a\""}
    };
    return saveResult(tests, p);
}
