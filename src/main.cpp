#include <iostream>
#include <vector>
#include <algorithm>

int
main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    std::vector<int> v = {1, 2, 3};
    std::for_each(v.begin(), v.end(), [](int x) {
        std::cout << "Hello" << x << "!" << std::endl;
    });

    return 0;
}
