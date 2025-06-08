
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
    for (auto &t : tests) {
        if (t.first()) passed_tests++;
        else if (first_failed_input.empty()) first_failed_input = t.second;
    }
    int fd = open(result_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        std::string out = std::to_string(passed_tests) + " " +
                          std::to_string(total_tests) + " " +
                          first_failed_input;
        write(fd, out.c_str(), out.size());
        close(fd);
    }
    return (passed_tests == total_tests) ? 0 : 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;
    std::string p = argv[1];
    VTests tests = {
        // 1. 单字符
        {[]{ std::vector<char> v{'x'}, exp{'x'}; Solution().reverseString(v); return assertEqual(v==exp, true); }, "['x']"},
        // 2. hello
        {[]{ std::vector<char> v{'h','e','l','l','o'}, exp{'o','l','l','e','h'}; Solution().reverseString(v); return assertEqual(v==exp, true); }, "[h,e,l,l,o]"},
        // 3. Hannah
        {[]{ std::vector<char> v{'H','a','n','n','a','h'}, exp{'h','a','n','n','a','H'}; Solution().reverseString(v); return assertEqual(v==exp, true); }, "[H,a,n,n,a,h]"},
        // 4. 偶数长度
        {[]{ std::vector<char> v{'a','b','c','d'}, exp{'d','c','b','a'}; Solution().reverseString(v); return assertEqual(v==exp, true); }, "[a,b,c,d]"},
        // 5. 回文串
        {[]{ std::vector<char> v{'r','a','c','e','c','a','r'}, exp{'r','a','c','e','c','a','r'}; Solution().reverseString(v); return assertEqual(v==exp, true); }, "[r,a,c,e,c,a,r]"},
        // 6. 全相同
        {[]{ std::vector<char> v(5,'z'), exp(5,'z'); Solution().reverseString(v); return assertEqual(v==exp, true); }, "['z'*5]"},
        // 7. 长度2
        {[]{ std::vector<char> v{'1','2'}, exp{'2','1'}; Solution().reverseString(v); return assertEqual(v==exp, true); }, "[1,2]"},
        // 8. 混合字符
        {[]{ std::vector<char> v{'A','!',' ','#','B'}, exp{'B','#',' ','!','A'}; Solution().reverseString(v); return assertEqual(v==exp, true); }, "[A,!, ,#,B]"},
        // 9. 大规模重复
        {[]{ std::vector<char> v(10000,'a'), exp(10000,'a'); Solution().reverseString(v); return assertEqual(v==exp, true); }, "['a'*10000]"},
        // 10. 大规模交替
        {[]{ std::vector<char> v; v.reserve(10000);
                for (int i = 0; i < 5000; ++i) { v.push_back('0'+(i%10)); v.push_back('A'+(i%26)); }
             std::vector<char> exp = v;
             std::reverse(exp.begin(), exp.end());
             Solution().reverseString(v);
             return assertEqual(v==exp, true);
         }, "alternating 10000"}      
    };
    return saveResult(tests, p);
}
