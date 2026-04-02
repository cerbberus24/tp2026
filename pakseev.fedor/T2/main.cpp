#include "DataStruct.hpp"

#include <algorithm>
#include <vector>
#include <iterator>
#include <limits>

// --- 4 ---
int main() {
    std::vector<DataStruct> vec;

    std::copy(std::istream_iterator<DataStruct>(std::cin),
              std::istream_iterator<DataStruct>(),
              std::back_inserter(vec));

    std::sort(vec.begin(), vec.end(), comparator);
    std::copy(
        std::begin(vec),
        std::end(vec),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}
