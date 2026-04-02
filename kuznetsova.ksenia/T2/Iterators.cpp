#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <complex>
#include <limits>

namespace nspace {
    struct DataStruct {
        unsigned long long key1 = 0;
        std::complex<double> key2{ 0.0, 0.0 };
        std::string key3;
    };

    std::istream& operator>>(std::istream& in, DataStruct& ds) {
        char c;

        if (!(in >> c) || c != '(') {
            in.setstate(std::ios::failbit);
            return in;
        }

        bool k1 = false, k2 = false, k3 = false;

        for (int i = 0; i < 3 && in; ++i) {
            if (!(in >> c) || c != ':') {
                in.setstate(std::ios::failbit);
                break;
            }

            std::string key;
            in >> key;

            if (!(in >> c) || c != ':') {
                in.setstate(std::ios::failbit);
                break;
            }

            if (key == "key1") {
                char p0, p1;
                if (!(in >> p0 >> p1) || p0 != '0' || (p1 != 'x' && p1 != 'X')) {
                    in.setstate(std::ios::failbit);
                    break;
                }
                in >> std::hex >> ds.key1 >> std::dec;
                if (!(in >> c) || c != ':') {
                    in.setstate(std::ios::failbit);
                    break;
                }
                k1 = true;
            }
            else if (key == "key2") {
                char h, cc, o, cl, cln;
                double r, im;
                if (!(in >> h >> cc >> o) || h != '#' || (cc != 'c' && cc != 'C') || o != '(') {
                    in.setstate(std::ios::failbit);
                    break;
                }
                if (!(in >> r >> im)) {
                    in.setstate(std::ios::failbit);
                    break;
                }
                if (!(in >> cl >> cln) || cl != ')' || cln != ':') {
                    in.setstate(std::ios::failbit);
                    break;
                }
                ds.key2 = { r, im };
                k2 = true;
            }
            else if (key == "key3") {
                char q;
                if (!(in >> q) || q != '"') {
                    in.setstate(std::ios::failbit);
                    break;
                }
                std::getline(in, ds.key3, '"');
                if (!(in >> c) || c != ':') {
                    in.setstate(std::ios::failbit);
                    break;
                }
                k3 = true;
            }
            else {
                in.setstate(std::ios::failbit);
                break;
            }
        }

        if (in && (!(in >> c) || c != ')')) {
            in.setstate(std::ios::failbit);
        }

        if (!in || !k1 || !k2 || !k3) {
            in.setstate(std::ios::failbit);
        }
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

    while (std::cin) {
        std::copy(
            std::istream_iterator<nspace::DataStruct>(std::cin),
            std::istream_iterator<nspace::DataStruct>(),
            std::back_inserter(data)
        );

        if (std::cin.fail() && !std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (std::cin.eof()) {
            break;
        }
    }

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
