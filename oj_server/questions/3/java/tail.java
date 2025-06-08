

// ======== TAIL ========
class Main {
    @FunctionalInterface interface TestFunc { boolean run(); }
    static class TestCase { TestFunc f; String in; TestCase(TestFunc f, String in){this.f=f;this.in=in;} }
    static boolean assertEqual(boolean a, boolean e) { return a == e; }
    static int saveResult(List<TestCase> tests, String path) {
        int total = tests.size(), passed = 0; String firstFail = "";
        for (TestCase t : tests) {
            if (t.f.run()) passed++;
            else if (firstFail.isEmpty()) firstFail = t.in;
        }
        try (FileWriter w = new FileWriter(path)) {
            w.write(passed + " " + total + " " + firstFail);
        } catch (IOException ex) { return 1; }
        return passed == total ? 0 : 1;
    }
    public static void main(String[] args) {
        if (args.length < 1) System.exit(1);
        String p = args[0];
        List<TestCase> tests = Arrays.asList(
            new TestCase(() -> { char[] v = {'x'}; char[] e = {'x'}; new Solution().reverseString(v); return assertEqual(Arrays.equals(v,e), true); }, "['x']"),
            new TestFuncTest("hello", "olleh"),
            new TestFuncTest("Hannah", "hannaH"),
            new TestFuncTest("abcd", "dcba"),
            new TestFuncTest("racecar", "racecar"),
            new TestFuncTest("zzzzz", "zzzzz"),
            new TestFuncTest("12", "21"),
            new TestFuncTest("A! #B", "B# !A"),
            new TestCase(() -> {
                char[] v = new char[10000];
                Arrays.fill(v,'a');
                char[] e = v.clone();
                new Solution().reverseString(v);
                return assertEqual(Arrays.equals(v,e), true);
            }, "['a'*10000]"),
            new TestCase(() -> {
                char[] v = new char[10000];
                for (int i = 0; i < 5000; i++) {
                    v[2*i]   = (char)('0' + i%10);
                    v[2*i+1] = (char)('A' + i%26);
                }
                char[] e = v.clone();
                for (int i = 0; i < e.length/2; i++) {
                    char t = e[i]; e[i] = e[e.length-1-i]; e[e.length-1-i] = t;
                }
                new Solution().reverseString(v);
                return assertEqual(Arrays.equals(v,e), true);
            }, "alternating 10000")
        );
        System.exit(saveResult(tests, p));
    }

    // 辅助构造简单测试
    static TestCase TestFuncTest(String in, String ex) {
        return new TestCase(() -> {
            char[] v = in.toCharArray();
            char[] e = ex.toCharArray();
            new Solution().reverseString(v);
            return assertEqual(Arrays.equals(v,e), true);
        }, "\"" + in + "\"");
    }
}
