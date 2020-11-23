#include <iostream>
#include "istring.h"

using namespace std;
using namespace so;

#define print(...) cout << #__VA_ARGS__ ": " << (__VA_ARGS__) << endl
#define demacate() cout << "------------------------------------------------------------\n"
#define print_str(s) \
    print(s);         \
    print(s.isEmpty()); \
    print(s.isHeapAllocated()); \
    print(s.length());  \
    demacate();

int main() {
    cout << std::boolalpha;

    print(sizeof(IString));
    demacate();

    IString s0;
    print_str(s0)

    IString s0_1 = "";
    print_str(s0_1)

    IString s1 = "hello";
    print_str(s1)

    IString s2 = "abracadabra abracadabra abracadabra";
    print_str(s2);

    auto s2_1 = s2;
    print_str(s2_1)

    auto s3 = s1 + s2;
    print_str(s3)

    auto s4 = IString("hello ", "I'm "s, "here with "sv, s2);
    print_str(s4)

    IString s5 = "world"sv;
    print_str(s5)

    return 0;
}
