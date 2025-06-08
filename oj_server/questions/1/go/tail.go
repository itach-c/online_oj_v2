
func assertEqual(actual, expected bool) bool { return actual == expected }

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
    if err != nil { return 1 }
    defer f.Close()
    fmt.Fprintf(f, "%d %d %s", passed, total, firstFail)
    if passed == total { return 0 }
    return 1
}

func main() {
    if len(os.Args) < 2 { os.Exit(1) }
    path := os.Args[1]
    tests := []TestCase{
        {func() bool { return assertEqual(Solution{}.isPalindrome(0), true) }, "0"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(-1), false) }, "-1"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(5), true) }, "5"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(10), false) }, "10"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(121), true) }, "121"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(-121), false) }, "-121"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(12321), true) }, "12321"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(1231), false) }, "1231"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(1001), true) }, "1001"},
        {func() bool { return assertEqual(Solution{}.isPalindrome(2147447412), true) }, "2147447412"},
    }
    os.Exit(saveResult(tests, path))
}