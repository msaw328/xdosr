void test(char* str) {
    str[0] = 'a';
    return;
}

int kmain() {
    test("Hello from kernel");
    return 0;
}
