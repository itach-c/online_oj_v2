
// ======== TAIL ========

func assertEqual(actual, expected int) bool {
    return actual == expected
}

type TestCase struct {
    testFunc func() bool
    inputStr string
}

func saveResult(tests []TestCase, resultPath string) int {
    total, passed := len(tests), 0
    firstFail := ""
    for _, t := range tests {
        if t.testFunc() {
            passed++
        } else if firstFail == "" {
            firstFail = t.inputStr
        }
    }
    f, err := os.Create(resultPath)
    if err != nil {
        fmt.Printf("Error: %v\n", err)
        return 1
    }
    defer f.Close()
    fmt.Fprintf(f, "%d %d %s", passed, total, firstFail)
    if passed == total {
        return 0
    }
    return 1
}

func main() {
    if len(os.Args) < 2 {
        fmt.Println("Usage: program <result_file_path>")
        os.Exit(1)
    }
    path := os.Args[1]
    tests := []TestCase{
        {func() bool { return assertEqual(Solution{}.numBusesToDestination(nil, 1, 2), -1) }, "[] 1->2"},
        {func() bool { return assertEqual(Solution{}.numBusesToDestination([][]int{{1,2,3}}, 1, 3), 1) }, "[1-3] 1->3"},
        {func() bool { return assertEqual(Solution{}.numBusesToDestination([][]int{{1,2},{2,3}}, 1, 3), 2) }, "1->3 via [1,2],[2,3]"},
        {func() bool { return assertEqual(Solution{}.numBusesToDestination([][]int{{1,2},{3,4}}, 1, 4), -1) }, "1->4 unreachable"},
        {func() bool { return assertEqual(Solution{}.numBusesToDestination([][]int{{1,2,3},{3,4,5},{5,6}}, 1, 6), 3) }, "1->6 via 3,5"},
        {func() bool { return assertEqual(Solution{}.numBusesToDestination([][]int{{1,2,3,4},{4,5}}, 1, 5), 2) }, "overlap 4"},
        {func() bool { return assertEqual(Solution{}.numBusesToDestination([][]int{{1},{1,2},{2,3}}, 1, 3), 3) }, "single-stop transfers"},
        {func() bool { return assertEqual(Solution{}.numBusesToDestination([][]int{{100,200},{200,300},{300,400}}, 100, 400), 3) }, "high IDs"},
        {func() bool {
            r := [][]int{{1,2,7},{3,6,7},{6,8,9},{9,10,2}}
            return assertEqual(Solution{}.numBusesToDestination(r, 1, 10), 4)
        }, "chain 1->10"},
        {func() bool {
            r := [][]int{
                {1,2,3,4},
                {4,5,6},
                {6,7,8},
                {8,9,1},
                {2,9,10},
            }
            return assertEqual(Solution{}.numBusesToDestination(r, 3, 10), 5)
        }, "complex loop 3->10"},
    }
    os.Exit(saveResult(tests, path))
}
