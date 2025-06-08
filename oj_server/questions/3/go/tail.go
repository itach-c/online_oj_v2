

// ======== TAIL ========
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
        {func() bool { v := []byte{'x'}; exp := []byte{'x'}; Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "['x']"},
        {func() bool { v := []byte("hello"); exp := []byte("olleh"); Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "hello"},
        {func() bool { v := []byte("Hannah"); exp := []byte("hannaH"); Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "Hannah"},
        {func() bool { v := []byte{'a','b','c','d'}; exp := []byte{'d','c','b','a'}; Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "[a,b,c,d]"},
        {func() bool { v := []byte("racecar"); exp := []byte("racecar"); Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "racecar"},
        {func() bool { v := []byte{'z','z','z','z','z'}; exp := []byte{'z','z','z','z','z'}; Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "['z'*5]"},
        {func() bool { v := []byte{'1','2'}; exp := []byte{'2','1'}; Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "[1,2]"},
        {func() bool { v := []byte{'A','!',' ','#','B'}; exp := []byte{'B','#',' ','!','A'}; Solution{}.reverseString(v); return assertEqual(reflect.DeepEqual(v, exp), true) }, "[A,!, ,#,B]"},
        {func() bool {
            v := make([]byte, 10000)
            for i := range v { v[i] = 'a' }
            exp := make([]byte, 10000)
            copy(exp, v)
            Solution{}.reverseString(v)
            return assertEqual(reflect.DeepEqual(v, exp), true)
        }, "['a'*10000]"},
        {func() bool {
            v := make([]byte, 10000)
            for i := 0; i < 5000; i++ {
                v[2*i] = byte('0' + i%10)
                v[2*i+1] = byte('A' + i%26)
            }
            exp := make([]byte, len(v))
            copy(exp, v)
            // reverse exp
            for i := 0; i < len(exp)/2; i++ {
                exp[i], exp[len(exp)-1-i] = exp[len(exp)-1-i], exp[i]
            }
            Solution{}.reverseString(v)
            return assertEqual(reflect.DeepEqual(v, exp), true)
        }, "alternating 10000"},
    }
    os.Exit(saveResult(tests, path))
}
