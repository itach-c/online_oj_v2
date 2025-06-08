

// ======== TAIL ========
class Main {
    @FunctionalInterface interface TestFunc { boolean run(); }
    static class TestCase { TestFunc f; String in; TestCase(TestFunc f, String in){this.f=f;this.in=in;} }
    static boolean assertEqual(String a, String e) { return a.equals(e); }
    static int saveResult(List<TestCase> tests, String path) {
        int total = tests.size(), passed = 0; String firstFail = "";
        for (TestCase t : tests) {
            if (t.f.run()) passed++;
            else if (firstFail.isEmpty()) firstFail = t.in;
        }
        try (FileWriter w = new FileWriter(path)) {
            w.write(passed + " " + total + " " + firstFail);
        } catch (IOException ex) {
            return 1;
        }
        return passed == total ? 0 : 1;
    }
    public static void main(String[] args) {
        if (args.length < 1) System.exit(1);
        String p = args[0];
        List<TestCase> tests = Arrays.asList(
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("a"), "a"), "\"a\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("ab"), "a"), "\"ab\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("bb"), "bb"), "\"bb\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("aba"), "aba"), "\"aba\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("baab"), "baab"), "\"baab\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("cbbd"), "bb"), "\"cbbd\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("abacdfgdcaba"), "aba"), "\"abacdfgdcaba\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("abaxyz"), "aba"), "\"abaxyz\""),
            new TestCase(() -> assertEqual(new Solution().longestPalindrome("abaxyzzyxf"), "xyzzyx"), "\"abaxyzzyxf\""),
            new TestCase(() -> {
                char[] arr = new char[500];
                Arrays.fill(arr, 'a');
                String s = new String(arr);
                return assertEqual(new Solution().longestPalindrome(s), s);
            }, "500×\"a\"")
        );
        System.exit(saveResult(tests, p));
    }
}
