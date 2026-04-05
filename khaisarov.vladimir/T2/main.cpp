#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <limits>

namespace nspace {
    const double EPS = 1e-9;

    struct DataStruct {
        double key1;
        unsigned long long key2;
        std::string key3;
    };

    struct DelimiterIO {
        char exp;
    };

    struct DoubleLitIO {
        double& ref;
    };

    struct UllHexIO {
        unsigned long long& ref;
    };

    struct KeyIO {
        int& ref;
    };

    std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        char c;
        in >> c;
        if (in && std::tolower(c) != std::tolower(dest.exp)) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, KeyIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        char k, e, y, n;
        in >> k >> e >> y >> n;
        if (in && (k != 'k' || e != 'e' || y != 'y' || !(n >= '1' && n <= '3'))) {
            in.setstate(std::ios::failbit);
        } else {
            dest.ref = n - '0';
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, DoubleLitIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        std::string temp;
        char c;

        while (in >> std::ws && in.get(c) && std::tolower(c) != 'd') {
            temp += c;
        }
        size_t dotPos = temp.find('.');
        if (dotPos == std::string::npos || dotPos == 0 || dotPos == temp.length() - 1) {
            in.setstate(std::ios::failbit);
            return in;
        }
        try {
            dest.ref = std::stod(temp);
        } catch (...) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, UllHexIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        if (!(in >> DelimiterIO{'0'} >> DelimiterIO{'x'})) {
            return in;
        }
        return in >> std::hex >> dest.ref >> std::dec;
    }

    std::istream& operator>>(std::istream& in, DataStruct& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        DataStruct input;
        bool has[3] = {false, false, false};

        if (!(in >> DelimiterIO{'('} >> DelimiterIO{':'})) return in;

        for (int i = 0; i < 3; i++) {
            int keyNum = 0;
            if (!(in >> KeyIO{keyNum})) return in;

            if (keyNum == 1 && !has[0]) {
                if (!(in >> DoubleLitIO{input.key1})) return in;
                has[0] = true;
            } else if (keyNum == 2 && !has[1]) {
                if (!(in >> UllHexIO{input.key2})) return in;
                has[1] = true;
            } else if (keyNum == 3 && !has[2]) {
                if (!(in >> DelimiterIO{'"'})) return in;
                std::getline(in, input.key3, '"');
                has[2] = true;
            } else {
                in.setstate(std::ios::failbit);
                return in;
            }
            if (!(in >> DelimiterIO{':'})) return in;
        }

        if (!(in >> DelimiterIO{')'})) return in;

        if (has[0] && has[1] && has[2]) {
            dest = input;
        } else {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
        std::ostream::sentry sentry(out);
        if (!sentry) return out;
        out << "(:key1 " << std::fixed << std::setprecision(1) << src.key1 << "d";

        out << ":key2 0x" << std::uppercase << std::hex << src.key2 << std::dec;
        out << ":key3 \"" << src.key3 << "\":)";
        return out;
    }

    bool compareData(const DataStruct& a, const DataStruct& b) {
        if (std::abs(a.key1 - b.key1) > EPS) {
            return a.key1 < b.key1;
        }
        if (a.key2 != b.key2) {
            return a.key2 < b.key2;
        }
        return a.key3.length() < b.key3.length();
    }
}

int main() {
    std::vector<nspace::DataStruct> data;
    while (!std::cin.eof()) {
        std::copy(
            std::istream_iterator<nspace::DataStruct>(std::cin),
            std::istream_iterator<nspace::DataStruct>(),
            std::back_inserter(data)
        );
        if (std::cin.fail() && !std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(data.begin(), data.end(), nspace::compareData);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<nspace::DataStruct>(std::cout, "\n")
    );

    return 0;
}
