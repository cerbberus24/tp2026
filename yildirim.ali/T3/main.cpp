#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iomanip>
#include <string>
#include <cmath>
#include <cctype>

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};
struct Polygon {
    std::vector<Point> points;
};
double polygonArea(const Polygon& p) {
    if (p.points.size() < 3) {
        return 0.0;
    }
    long long sum = 0;
    size_t n = p.points.size();
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        sum += static_cast<long long>(p.points[i].x) * p.points[j].y -
               static_cast<long long>(p.points[j].x) * p.points[i].y;
    }
    return std::abs(sum) / 2.0;
}
struct AreaCalculator {
    double operator()(const Polygon& p) const {
        return polygonArea(p);
    }
};
struct IsEvenVertex {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 0;
    }
};
struct IsOddVertex {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 1;
    }
};
struct IsVertexCount {
    int target;
    IsVertexCount(int n) : target(n) {}
    bool operator()(const Polygon& p) const {
        return static_cast<int>(p.points.size()) == target;
    }
};
struct SumAreaIfEven {
    double operator()(double acc, const Polygon& p) const {
        if (p.points.size() % 2 == 0) {
            return acc + AreaCalculator()(p);
        }
        return acc;
    }
};
struct SumAreaIfOdd {
    double operator()(double acc, const Polygon& p) const {
        if (p.points.size() % 2 == 1) {
            return acc + AreaCalculator()(p);
        }
        return acc;
    }
};
struct SumAreaIfVertex {
    int target;
    SumAreaIfVertex(int n) : target(n) {}
    double operator()(double acc, const Polygon& p) const {
        if (static_cast<int>(p.points.size()) == target) {
            return acc + AreaCalculator()(p);
        }
        return acc;
    }
};
bool isPermutation(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) {
        return false;
    }
    size_t n = a.points.size();
    for (size_t start = 0; start < n; ++start) {
        bool match = true;
        for (size_t i = 0; i < n; ++i) {
            if (!(a.points[i] == b.points[(start + i) % n])) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    for (size_t start = 0; start < n; ++start) {
        bool match = true;
        for (size_t i = 0; i < n; ++i) {
            if (!(a.points[i] == b.points[(start + n - i) % n])) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    return false;
}
struct IsPermutation {
    Polygon target;
    IsPermutation(const Polygon& t) : target(t) {}
    bool operator()(const Polygon& p) const {
        return isPermutation(p, target);
    }
};
bool isSamePolygon(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) {
        return false;
    }
    for (size_t i = 0; i < a.points.size(); ++i) {
        if (a.points[i].x != b.points[i].x) {
            return false;
        }
        if (a.points[i].y != b.points[i].y) {
            return false;
        }
    }
    return true;
}
struct IsSame {
    Polygon target;
    IsSame(const Polygon& t) : target(t) {}
    bool operator()(const Polygon& p) const {
        return isSamePolygon(p, target);
    }
};
struct MaxSeqState {
    size_t cur;
    size_t best;
};
struct MaxSeqAccumulator {
    const Polygon& target;
    MaxSeqAccumulator(const Polygon& t) : target(t) {}
    MaxSeqState operator()(MaxSeqState state, const Polygon& p) const {
        if (isSamePolygon(p, target)) {
            state.cur = state.cur + 1;
            if (state.cur > state.best) {
                state.best = state.cur;
            }
        } else {
            state.cur = 0;
        }
        return state;
    }
};
bool parsePolygon(const std::string& line, Polygon& out) {
    std::istringstream iss(line);
    int n;
    iss >> n;
    if (iss.fail() || n < 3) {
        return false;
    }
    std::vector<Point> pts;
    for (int i = 0; i < n; ++i) {
        char open, semi, close;
        int x, y;
        iss >> open >> x >> semi >> y >> close;
        if (iss.fail() || open != '(' || semi != ';' || close != ')') {
            return false;
        }
        pts.push_back({x, y});
    }
    std::string extra;
    if (iss >> extra) {
        return false;
    }
    out.points = pts;
    return true;
}
std::vector<Polygon> readPolygons(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return {};
    }
    std::vector<Polygon> polygons;
    std::string line;
    while (std::getline(file, line)) {
        Polygon p;
        if (parsePolygon(line, p)) {
            polygons.push_back(p);
        }
    }
    return polygons;
}
bool parsePolygonCommand(const std::vector<std::string>& tokens, Polygon& poly) {
    if (tokens.size() < 2) {
        return false;
    }
    int n;
    try {
        n = std::stoi(tokens[1]);
    } catch (...) {
        return false;
    }
    if (n < 3) {
        return false;
    }
    if (static_cast<int>(tokens.size()) < 2 + n) {
        return false;
    }
    Polygon p;
    for (int i = 0; i < n; ++i) {
        std::string token = tokens[2 + i];
        if (token.size() < 5) {
            return false;
        }
        if (token.front() != '(') {
            return false;
        }
        if (token.back() != ')') {
            return false;
        }
        size_t semi = token.find(';');
        if (semi == std::string::npos) {
            return false;
        }
        int x, y;
        try {
            x = std::stoi(token.substr(1, semi - 1));
            y = std::stoi(token.substr(semi + 1, token.size() - semi - 2));
        } catch (...) {
            return false;
        }
        p.points.push_back({x, y});
    }
    if (static_cast<int>(p.points.size()) != n) {
        return false;
    }
    poly = p;
    return true;
}
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    std::vector<Polygon> polygons = readPolygons(argv[1]);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        if (tokens.empty()) {
            continue;
        }
        std::string cmd = tokens[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

        if (cmd == "PERMS") {
            if (tokens.size() < 2) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            Polygon target;
            if (!parsePolygonCommand(tokens, target)) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            int cnt = std::count_if(polygons.begin(), polygons.end(),
                std::bind(IsPermutation(target), std::placeholders::_1));
            std::cout << cnt << std::endl;
        }
        else if (cmd == "MAXSEQ") {
            if (tokens.size() < 2) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            Polygon target;
            if (!parsePolygonCommand(tokens, target)) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            MaxSeqAccumulator acc(target);
            MaxSeqState state = std::accumulate(polygons.begin(), polygons.end(),
                MaxSeqState{0, 0}, acc);
            std::cout << state.best << std::endl;
        }
        else if (cmd == "AREA") {
            if (tokens.size() < 2) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            std::string sub = tokens[1];
            std::transform(sub.begin(), sub.end(), sub.begin(), ::toupper);
            if (sub == "MEAN") {
                if (polygons.empty()) {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                    continue;
                }
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    std::bind(std::plus<double>(),
                        std::placeholders::_1,
                        std::bind(AreaCalculator(), std::placeholders::_2)));
                double res = sum / polygons.size();
                std::cout << std::fixed << std::setprecision(1) << res << std::endl;
            }
            else if (sub == "EVEN") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    SumAreaIfEven());
                std::cout << std::fixed << std::setprecision(1) << sum << std::endl;
            }
            else if (sub == "ODD") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    SumAreaIfOdd());
                std::cout << std::fixed << std::setprecision(1) << sum << std::endl;
            }
            else {
                try {
                    int n = std::stoi(sub);
                    if (n < 3) {
                        std::cout << "<INVALID COMMAND>" << std::endl;
                        continue;
                    }
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        SumAreaIfVertex(n));
                    std::cout << std::fixed << std::setprecision(1) << sum << std::endl;
                } catch (...) {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                }
            }
        }
        else if (cmd == "COUNT") {
            if (tokens.size() < 2) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            std::string sub = tokens[1];
            if (sub == "EVEN") {
                int cnt = std::count_if(polygons.begin(), polygons.end(),
                    IsEvenVertex());
                std::cout << cnt << std::endl;
            }
            else if (sub == "ODD") {
                int cnt = std::count_if(polygons.begin(), polygons.end(),
                    IsOddVertex());
                std::cout << cnt << std::endl;
            }
            else {
                try {
                    int n = std::stoi(sub);
                    if (n < 3) {
                        std::cout << "<INVALID COMMAND>" << std::endl;
                        continue;
                    }
                    int cnt = std::count_if(polygons.begin(), polygons.end(),
                        IsVertexCount(n));
                    std::cout << cnt << std::endl;
                } catch (...) {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                }
            }
        }
        else if (cmd == "MIN") {
            if (tokens.size() < 2) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            std::string sub = tokens[1];
            std::transform(sub.begin(), sub.end(), sub.begin(), ::toupper);
            if (sub == "AREA") {
                auto it = std::min_element(polygons.begin(), polygons.end(),
                    std::bind(std::less<double>(),
                        std::bind(AreaCalculator(), std::placeholders::_1),
                        std::bind(AreaCalculator(), std::placeholders::_2)));
                double val = AreaCalculator()(*it);
                std::cout << std::fixed << std::setprecision(1) << val << std::endl;
            }
            else if (sub == "VERTEXES") {
                auto it = std::min_element(polygons.begin(), polygons.end(),
                    [](const Polygon& a, const Polygon& b) {
                        return a.points.size() < b.points.size();
                    });
                std::cout << it->points.size() << std::endl;
            }
            else {
                std::cout << "<INVALID COMMAND>" << std::endl;
            }
        }
        else if (cmd == "MAX") {
            if (tokens.size() < 2) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>" << std::endl;
                continue;
            }
            std::string sub = tokens[1];
            std::transform(sub.begin(), sub.end(), sub.begin(), ::toupper);
            if (sub == "AREA") {
                auto it = std::max_element(polygons.begin(), polygons.end(),
                    std::bind(std::less<double>(),
                        std::bind(AreaCalculator(), std::placeholders::_1),
                        std::bind(AreaCalculator(), std::placeholders::_2)));
                double val = AreaCalculator()(*it);
                std::cout << std::fixed << std::setprecision(1) << val << std::endl;
            }
            else if (sub == "VERTEXES") {
                auto it = std::max_element(polygons.begin(), polygons.end(),
                    [](const Polygon& a, const Polygon& b) {
                        return a.points.size() < b.points.size();
                    });
                std::cout << it->points.size() << std::endl;
            }
            else {
                std::cout << "<INVALID COMMAND>" << std::endl;
            }
        }
        else {
            std::cout << "<INVALID COMMAND>" << std::endl;
        }
    }
    return 0;
}
//
