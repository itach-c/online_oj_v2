
func assertEqual(actual, expected bool) bool {
	return actual == expected
}

type TestCase struct {
	testFunc func() bool
	inputStr string
}

func saveResult(tests []TestCase, resultPath string) int {
	totalTests := len(tests)
	passedTests := 0
	firstFailedInput := ""

	for _, test := range tests {
		if test.testFunc() {
			passedTests++
		} else if firstFailedInput == "" {
			firstFailedInput = test.inputStr
		}
	}

	file, err := os.Create(resultPath)
	if err != nil {
		fmt.Printf("Error creating file: %v\n", err)
		return 1
	}
	defer file.Close()

	result := fmt.Sprintf("%d %d %s", passedTests, totalTests, firstFailedInput)
	_, err = file.WriteString(result)
	if err != nil {
		fmt.Printf("Error writing to file: %v\n", err)
		return 1
	}

	if passedTests == totalTests {
		return 0
	}
	return 1
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: program <result_file_path>")
		os.Exit(1)
	}

	resultPath := os.Args[1]

	tests := []TestCase{
		{
			testFunc: func() bool {
				return assertEqual(Solution{}.isPalindrome(121), true)
			},
			inputStr: "121",
		},
		{
			testFunc: func() bool {
				return assertEqual(Solution{}.isPalindrome(-10), false)
			},
			inputStr: "-10",
		},
		{
			testFunc: func() bool {
				return assertEqual(Solution{}.isPalindrome(12321), true)
			},
			inputStr: "12321",
		},
	}

	os.Exit(saveResult(tests, resultPath))
}
