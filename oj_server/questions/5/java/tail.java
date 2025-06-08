// ======== TAIL ========
public class Main {

    @FunctionalInterface
    interface TestFunc {
        boolean run();
    }

    static class TestCase {
        TestFunc testFunc;
        String inputStr;

        TestCase(TestFunc testFunc, String inputStr) {
            this.testFunc = testFunc;
            this.inputStr = inputStr;
        }
    }

    static boolean assertEqual(boolean actual, boolean expected) {
        return actual == expected;
    }

    static int saveResult(List<TestCase> tests, String resultPath) {
        int totalTests = tests.size();
        int passedTests = 0;
        String firstFailedInput = "";

        for (TestCase test : tests) {
            if (test.testFunc.run()) {  // 直接调用 run 方法
                passedTests++;
            } else if (firstFailedInput.isEmpty()) {
                firstFailedInput = test.inputStr;
            }
        }

        try (FileWriter writer = new FileWriter(resultPath)) {
            writer.write(passedTests + " " + totalTests + " " + firstFailedInput);
        } catch (IOException e) {
            System.err.println("Error writing to file: " + e.getMessage());
            return 1;
        }

        return (passedTests == totalTests) ? 0 : 1;
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.err.println("Usage: java Main <result_file_path>");
            System.exit(1);
        }

        String resultPath = args[0];

        List<TestCase> tests = new ArrayList<>();
        tests.add(new TestCase(
                () -> assertEqual(new Solution().isPalindrome(121), true),
                "121"
        ));
        tests.add(new TestCase(
                () -> assertEqual(new Solution().isPalindrome(-10), false),
                "-10"
        ));
        tests.add(new TestCase(
                () -> assertEqual(new Solution().isPalindrome(12321), true),
                "12321"
        ));

        int result = saveResult(tests, resultPath);
        System.exit(result);
    }
}