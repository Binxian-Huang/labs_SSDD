struct result_values {
    int operation_result;
    string value1<>;
    int value2;
    double value3;
};

program SERVICES {
    version SERVICESVER {
        int INIT() = 0;
        int SET(int key, string value1<>, int value2, double value3) = 1;
        struct result_values GET(int key) = 2;
        int MODIFY(int key, string value1<>, int value2, double value3) = 3;
        int DELETE(int key) = 4;
        int EXIST(int key) = 5;
        int COPY(int key1, int key2) = 6;
    } = 1;
} = 99;
