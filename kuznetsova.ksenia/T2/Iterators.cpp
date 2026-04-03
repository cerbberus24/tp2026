#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <complex>
#include <cmath>
#include <iterator>
#include <sstream>
#include <cctype>
#include <iomanip>

struct DataStruct {
    unsigned long long key1;
    std::complex<double> key2;
    std::string key3;
};

bool parseHexULL(const std::string& str, unsigned long long& value) {
    if (str.length() < 3) {
        return false;
    }
    if (str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) {
        return false;
    }
    for (size_t i = 2; i < str.length(); ++i) {
        char c = str[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            return false;
        }
    }
    std::stringstream s(str);
    s >> std::hex >> value;
    return !s.fail();
}

bool parseComplex(const std::string& str, std::complex<double>& value) {
    if (str.length() < 5) {
        return false;
    }
    if (str[0] != '#' || str[1] != 'c' || str[2] != '(' || str.back() != ')') {
        return false;
    }

    std::string inner = str.substr(3, str.length() - 4);

    size_t space_pos = inner.find(' ');
    if (space_pos == std::string::npos || space_pos == 0 || space_pos == inner.length() - 1) {
        return false;
    }

    for (size_t i = 0; i < inner.length(); ++i) {
        if (i != space_pos && inner[i] == ' ') {
            return false;
        }
    }

    std::string real_str = inner.substr(0, space_pos);
    std::string imag_str = inner.substr(space_pos + 1);

    if (real_str.empty() || imag_str.empty()) {
        return false;
    }

    double real, imag;
    std::stringstream ss_real(real_str);
    std::stringstream ss_imag(imag_str);

    if (!(ss_real >> real) || !(ss_imag >> imag)) {
        return false;
    }

    if (ss_real >> std::ws, !ss_real.eof() || ss_imag >> std::ws, !ss_imag.eof()) {
        return false;
    }

    value = std::complex<double>(real, imag);
    return true;
}

bool parseString(const std::string& str, std::string& value) {
    if (str.length() < 2 || str.front() != '"' || str.back() != '"') {
        return false;
    }
    value = str.substr(1, str.length() - 2);
    return true;
}

std::istream& operator>>(std::istream& is, DataStruct& data) {
    std::string line;
    if (!std::getline(is, line)) {
        return is;
    }
    size_t start = line.find('(');
    if (start == std::string::npos) {
        is.setstate(std::ios::failbit);
        return is;
    }
    size_t end = line.rfind(')');
    if (end == std::string::npos || end <= start) {
        is.setstate(std::ios::failbit);
        return is;
    }
    std::string content = line.substr(start + 1, end - start - 1);
    unsigned long long key1 = 0;
    std::complex<double> key2(0, 0);
    std::string key3;
    bool key1_found = false, key2_found = false, key3_found = false;
    std::vector<std::string> parts;
    bool in_quotes = false;
    bool in_complex = false;
    size_t last_pos = 0;

    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '"') {
            in_quotes = !in_quotes;
        }
        else if (!in_quotes && content[i] == '#') {
            in_complex = true;
        }
        else if (!in_quotes && content[i] == ')' && in_complex) {
            in_complex = false;
        }
        else if (!in_quotes && !in_complex && content[i] == ':') {
            if (i > last_pos) {
                parts.push_back(content.substr(last_pos, i - last_pos));
            }
            last_pos = i + 1;
        }
    }
    if (last_pos < content.length()) {
        parts.push_back(content.substr(last_pos));
    }
    for (const auto& part : parts) {
        if (part.empty()) {
            continue;
        }
        size_t space_pos = part.find(' ');
        if (space_pos == std::string::npos) {
            continue;
        }
        std::string name = part.substr(0, space_pos);
        std::string value = part.substr(space_pos + 1);
        if (name == "key1") {
            if (parseHexULL(value, key1)) {
                key1_found = true;
            }
        }
        else if (name == "key2") {
            if (parseComplex(value, key2)) {
                key2_found = true;
            }
        }
        else if (name == "key3") {
            if (parseString(value, key3)) {
                key3_found = true;
            }
        }
    }
    if (key1_found && key2_found && key3_found) {
        data.key1 = key1;
        data.key2 = key2;
        data.key3 = key3;
    }
    else {
        is.setstate(std::ios::failbit);
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const DataStruct& data) {
    os << "(:key1 0x" << std::hex << std::uppercase << data.key1 << std::dec << std::nouppercase;
    os << ":key2 #c(" << std::fixed << std::setprecision(1);
    os << data.key2.real() << " " << data.key2.imag() << std::defaultfloat << "):";
    os << ":key3 \"" << data.key3 << "\":)";
    return os;
}

bool compareData(const DataStruct& d1, const DataStruct& d2) {
    if (d1.key1 != d2.key1) {
        return d1.key1 < d2.key1;
    }
    double abs_key1 = std::abs(d1.key2);
    double abs_key2 = std::abs(d2.key2);
    if (abs_key1 != abs_key2) {
        return abs_key1 < abs_key2;
    }
    return d1.key3.length() < d2.key3.length();
}

int main() {
    std::vector<DataStruct> vec;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        if (line.find('(') == std::string::npos) {
            continue;
        }
        std::istringstream is(line);
        DataStruct temp;
        if (is >> temp) {
            vec.push_back(temp);
        }
    }
    std::sort(vec.begin(), vec.end(), compareData);
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}
