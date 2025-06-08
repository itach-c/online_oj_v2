
// ======== TAIL ========
@FunctionalInterface
interface TestFunc { boolean run(); }

static class TestCase {
    TestFunc f; String in;
    TestCase(TestFunc f, String in) { this.f = f; this.in = in; }
}

static boolean assertEqual(boolean a, boolean e) { return a == e; }

static int saveResult(List<TestCase> tests, String path) {
    int total = tests.size(), passed = 0;
    String firstFail = "";
    for (TestCase t : tests) {
        if (t.f.run()) passed++;
        else if (firstFail.isEmpty()) firstFail = t.in;
    }
    try (FileWriter w = new FileWriter(path)) {
        w.write(passed + " " + total + " " + firstFail);
    } catch (IOException _){ return 1; }
    return passed == total ? 0 : 1;
}

public static void main(String[] args) {
    if (args.length < 1) System.exit(1);
    String path = args[0];
    List<TestCase> tests = Arrays.asList(
        new TestCase(() -> assertEqual(new Solution().isPalindrome(0), true), "0"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(-1), false), "-1"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(5), true), "5"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(10), false), "10"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(121), true), "121"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(-121), false), "-121"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(12321), true), "12321"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(1231), false), "1231"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(1001), true), "1001"),
        new TestCase(() -> assertEqual(new Solution().isPalindrome(2147447412), true), "2147447412")
    );
    System.exit(saveResult(tests, path));
}