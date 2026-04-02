#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <complex>
#include <sstream>

namespace nspace {
    struct DataStruct {
        unsigned long long key1 = 0;
        std::complex<double> key2{ 0.0, 0.0 };
        std::string key3;
    };

    bool parseRecord(const std::string& line, DataStruct& ds) {
        std::istringstream in(line);
        char c;

        if (!(in >> c) || c != '(') return false;

        bool k1 = false, k2 = false, k3 = false;

        for (int i = 0; i < 3 && in; ++i) {
            if (!(in >> c) || c != ':') return false;

            std::string key;
            in >> key;

            if (!(in >> c) || c != ':') return false;

            if (key == "key1") {
                char p0, p1;
                if (!(in >> p0 >> p1) || p0 != '0' || (p1 != 'x' && p1 != 'X')) return false;
                in >> std::hex >> ds.key1 >> std::dec;
                if (!(in >> c) || c != ':') return false;
                k1 = true;
            }
            else if (key == "key2") {
                char h, cc, o, cl, cln;
                double r, im;
                if (!(in >> h >> cc >> o) || h != '#' || (cc != 'c' && cc != 'C') || o != '(') return false;
                if (!(in >> r >> im)) return false;
                if (!(in >> cl >> cln) || cl != ')' || cln != ':') return false;
                ds.key2 = { r, im };
                k2 = true;
            }
            else if (key == "key3") {
                char q;
                if (!(in >> q) || q != '"') return false;
                std::getline(in, ds.key3, '"');
                if (!(in >> c) || c != ':') return false;
                k3 = true;
            }
            else {
                return false;
            }
        }

        if (!(in >> c) || c != ')') return false;

        return k1 && k2 && k3;
    }

    std::istream& operator>>(std::istream& in, DataStruct& ds) {
        std::string line;
        if (std::getline(in, line)) {
            if (parseRecord(line, ds)) {
                return in;
            }
        }
        in.setstate(std::ios::failbit);
        return in;
    }

    std::ostream& operator<<(std::ostream& out, const DataStruct& ds) {
        out << "(:key1 0x" << std::hex << std::uppercase << ds.key1 << std::dec << std::nouppercase;
        out << ":key2 #c(" << std::fixed << std::setprecision(1)
            << ds.key2.real() << " " << ds.key2.imag() << ")";
        out << ":key3 \"" << ds.key3 << "\":)";
        return out;
    }

    bool compareData(const DataStruct& a, const DataStruct& b) {
        if (a.key1 != b.key1) return a.key1 < b.key1;
        double ma = std::abs(a.key2), mb = std::abs(b.key2);
        if (ma != mb) return ma < mb;
        return a.key3.size() < b.key3.size();
    }
}

int main() {
    std::vector<nspace::DataStruct> data;

    std::copy(
        std::istream_iterator<nspace::DataStruct>(std::cin),
        std::istream_iterator<nspace::DataStruct>(),
        std::back_inserter(data)
    );

    if (data.empty()) {
        std::cerr << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 0;
    }

    std::sort(data.begin(), data.end(), nspace::compareData);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<nspace::DataStruct>(std::cout, "\n")
    );

    return 0;
}
