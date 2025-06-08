

// ======== TAIL ========
func assertEqual(actual, expected string) bool {
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
        {func() bool { return assertEqual(Solution{}.longestPalindrome("a"), "a") }, `"a"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("ab"), "a") }, `"ab"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("bb"), "bb") }, `"bb"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("aba"), "aba") }, `"aba"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("baab"), "baab") }, `"baab"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("cbbd"), "bb") }, `"cbbd"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("abacdfgdcaba"), "aba") }, `"abacdfgdcaba"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("abaxyz"), "aba") }, `"abaxyz"`},
        {func() bool { return assertEqual(Solution{}.longestPalindrome("abaxyzzyxf"), "xyzzyx") }, `"abaxyzzyxf"`},
        {func() bool {
            s := strings.Repeat("a", 500)
            return assertEqual(Solution{}.longestPalindrome(s), s)
        }, `500×"a"`},
    }
    os.Exit(saveResult(tests, path))
}
