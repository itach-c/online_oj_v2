
// ======== TAIL ========
class Main {
    @FunctionalInterface interface TestFunc { boolean run(); }
    static class TestCase { TestFunc f; String in; TestCase(TestFunc f, String in) { this.f = f; this.in = in; } }
    static boolean assertEqual(int a, int e) { return a == e; }
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
            // 1
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(new ArrayList<>(), 1, 2), -1), "[] 1->2"),
            // 2
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Collections.singletonList(Arrays.asList(1,2,3)), 1, 3), 1), "[1-3] 1->3"),
            // 3
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(Arrays.asList(1,2), Arrays.asList(2,3)), 1, 3), 2), "1->3 via two routes"),
            // 4
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(Arrays.asList(1,2), Arrays.asList(3,4)), 1, 4), -1), "unreachable"),
            // 5
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(Arrays.asList(1,2,3), Arrays.asList(3,4,5), Arrays.asList(5,6)), 1, 6), 3), "1->6 via 3,5"),
            // 6
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(Arrays.asList(1,2,3,4), Arrays.asList(4,5)), 1, 5), 2), "overlap"),
            // 7
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(Arrays.asList(1), Arrays.asList(1,2), Arrays.asList(2,3)), 1, 3), 3), "single stops"),
            // 8
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(Arrays.asList(100,200), Arrays.asList(200,300), Arrays.asList(300,400)), 100, 400), 3), "high IDs"),
            // 9
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(Arrays.asList(1,2,7), Arrays.asList(3,6,7), Arrays.asList(6,8,9), Arrays.asList(9,10,2)), 1, 10), 4), "chain 1->10"),
            // 10
            new TestCase(() -> assertEqual(new Solution().numBusesToDestination(
                Arrays.asList(
                    Arrays.asList(1,2,3,4),
                    Arrays.asList(4,5,6),
                    Arrays.asList(6,7,8),
                    Arrays.asList(8,9,1),
                    Arrays.asList(2,9,10)
                ), 3, 10), 5), "complex loop 3->10")
        );
        System.exit(saveResult(tests, p));
    }
}
