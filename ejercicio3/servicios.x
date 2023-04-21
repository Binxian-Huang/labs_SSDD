struct result_values {
    int key;
    sting value1<>;
    int value2,
    double value3;
}

program SERVICES {
    version SERVICESVER {
        int INIT() = 0;
        int SET(int key, string value1<>, int value2, double value3) = 1;
        struct result_values GET(int key) = 2;
        int MODIFY(int key, string value1<>, int value2, double value3) = 1;
        int DELETE(int key) = 1;
        int EXIST(int key) = 1;
        int COPY(int key1, int key2) = 1;
    } = 1;
} = 99;
