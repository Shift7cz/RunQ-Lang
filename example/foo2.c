int factorialC(int num) {
    int returnValue = 1;
    for (int i = 1; i <= num; i++) {
        returnValue *= i;
    }
    return returnValue;
}