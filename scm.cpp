#include "scm.hpp"

int main(int argc, char **argv)
{
    // std::cout << "build completed\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}