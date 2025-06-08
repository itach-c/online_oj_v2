// 在这里补充用户实现


// ======== TAIL ========

bool assertEqual(int actual, int expected)
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
        // 1. 空线路
        {[]{ vector<vector<int>> r; return assertEqual(Solution().numBusesToDestination(r, 1, 2), -1); }, "[] 1->2"},
        // 2. 单一路线直达
        {[]{ vector<vector<int>> r{{1,2,3}}; return assertEqual(Solution().numBusesToDestination(r, 1, 3), 1); }, "[1,2,3] 1->3"},
        // 3. 两段换乘
        {[]{ vector<vector<int>> r{{1,2},{2,3}}; return assertEqual(Solution().numBusesToDestination(r, 1, 3), 2); }, "[1,2],[2,3] 1->3"},
        // 4. 不可达
        {[]{ vector<vector<int>> r{{1,2},{3,4}}; return assertEqual(Solution().numBusesToDestination(r, 1, 4), -1); }, "[1,2],[3,4] 1->4"},
        // 5. 三次换乘
        {[]{ vector<vector<int>> r{{1,2,3},{3,4,5},{5,6}}; return assertEqual(Solution().numBusesToDestination(r, 1, 6), 3); }, "1->6 via 3,5"},
        // 6. 路线重叠
        {[]{ vector<vector<int>> r{{1,2,3,4},{4,5}}; return assertEqual(Solution().numBusesToDestination(r, 1, 5), 2); }, "[1-4],[4-5] 1->5"},
        // 7. 单站与小换乘
        {[]{ vector<vector<int>> r{{1},{1,2},{2,3}}; return assertEqual(Solution().numBusesToDestination(r, 1, 3), 3); }, "1->3 via single stops"},
        // 8. 大编号
        {[]{ vector<vector<int>> r{{100,200},{200,300},{300,400}}; return assertEqual(Solution().numBusesToDestination(r, 100, 400), 3); }, "100->400"},
        // 9. 链式多转更复杂
        {[]{ vector<vector<int>> r{{1,2,7},{3,6,7},{6,8,9},{9,10,2}}; return assertEqual(Solution().numBusesToDestination(r, 1, 10), 4); }, "1->10 via 7,6,9"},
        // 10. 环与重复站点大规模测试
        {[]{ vector<vector<int>> r{
                    {1,2,3,4},
                    {4,5,6},
                    {6,7,8},
                    {8,9,1},
                    {2,9,10}
                };
                return assertEqual(Solution().numBusesToDestination(r, 3, 10), 5);
          }, "3->10 complex loop"}
    };
    return saveResult(tests, p);
}
