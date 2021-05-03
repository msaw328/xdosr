char test(char* str) {
    return str[0];
}

int kmain() {
    test("Hello from kernel");
    return 0x1234;
}
