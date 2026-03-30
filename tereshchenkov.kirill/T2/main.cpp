#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <limits>
#include <sstream>

struct Datastruct {
    double key1;
    unsigned long long key2;
    std::string key3;
};

// Вспомогательная функция для пропуска ожидаемых символов
std::istream& skip_char(std::istream& in, char expected) {
    char c;
    if (!(in >> c) || c != expected) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, Datastruct& ds) {
    std::istream::sentry guard(in);
    if (!guard) return in;

    char c;
    // Начинаем парсинг (:
    if (!(in >> c) || c != '(') return in;
    if (!(in >> c) || c != ':') return in;

    while (in && in.peek() != ')') {
        std::string key;
        // Читаем имя ключа до пробела, проверяя EOF
        while (in && in.peek() != ' ' && in.peek() != ':') {
            key += static_cast<char>(in.get());
        }

        if (key == "key1") {
            if (!(in >> ds.key1)) break;
        }
        else if (key == "key2") {
            char b;
            // Проверяем формат 0b
            if (!(in >> c >> b) || c != '0' || (b != 'b' && b != 'B')) {
                in.setstate(std::ios::failbit);
                break;
            }
            if (!(in >> ds.key2)) break;
        }
        else if (key == "key3") {
            in >> std::ws; // Пропускаем пробелы перед кавычкой
            if (in.get() != '"') {
                in.setstate(std::ios::failbit);
                break;
            }
            std::getline(in, ds.key3, '"');
        }

        // Пропускаем возможные разделители (пробелы и двоеточия)
        in >> std::ws;
        if (in.peek() == ':') in.get();
    }

    if (!(in >> c) || c != ')') {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::ostream& operator<<(std::ostream& ost, const Datastruct& src) {
    std::ostream::sentry guard(ost);
    if (!guard) return ost;
    ost << "(:key1 " << std::fixed << std::setprecision(1) << src.key1
        << " :key2 0b" << src.key2
        << " :key3 \"" << src.key3 << "\" :)";
    return ost;
}

bool compare_data(const Datastruct& a, const Datastruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3.length() < b.key3.length();
}

int main() {
    std::string input =
        "(:key1 1.2 :key2 0b20 :key3 \"longer\" :)\n"
        "(:key1 0.5 :key2 0b100 :key3 \"data\" :)\n"
        "(:key1 1.2 :key2 0b100 :key3 \"abc\" :)\n"
        "(:key1 1.2 :key2 0b20 :key3 \"short\" :)";

    // ИСПРАВЛЕНИЕ: Читаем из iss, а не из std::cin
    std::istringstream iss(input);
    std::vector<Datastruct> vec;

    std::copy(std::istream_iterator<Datastruct>(iss),
              std::istream_iterator<Datastruct>(),
              std::back_inserter(vec));

    std::sort(vec.begin(), vec.end(), compare_data);

    std::cout << "Sorted results:\n";
    std::copy(vec.begin(), vec.end(),
              std::ostream_iterator<Datastruct>(std::cout, "\n"));

    return 0;
}