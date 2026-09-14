//  Лабораторная работа №3
//
//  Условие: Реализовать шаблон класса (функции), принимающий элементы любого
//  типа и поддерживающий базовые операции в соответствии со своим назначением.
//  В качестве внутренней структуры хранения данных можно использовать, например,
//  динамический массив. Использование шаблона должно осуществляться из функции
//  main() и содержать примеры работы как минимум с двумя разными типами данных
//  (например, int и string).
//
//  Вариант 6: Очередь.

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <limits>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

using namespace std;

template <typename T>
class Queue {
private:
    T*     data;
    size_t capacity;
    size_t count;
    size_t head;

    size_t indexOf(size_t i) const { return (head + i) % capacity; }

    void grow(size_t newCapacity) {
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < count; i++)
            newData[i] = data[indexOf(i)];
        delete[] data;
        data     = newData;
        capacity = newCapacity;
        head     = 0;
    }

public:
    explicit Queue(size_t initialCapacity = 4)
        : data(nullptr), capacity(initialCapacity), count(0), head(0) {
        if (capacity < 1) capacity = 1;
        data = new T[capacity];
    }

    Queue(const Queue& other)
        : data(nullptr), capacity(other.capacity), count(other.count), head(0) {
        data = new T[capacity];
        for (size_t i = 0; i < count; i++)
            data[i] = other.data[other.indexOf(i)];
    }

    Queue& operator=(const Queue& other) {
        if (this == &other) return *this;
        delete[] data;
        capacity = other.capacity;
        count    = other.count;
        head     = 0;
        data     = new T[capacity];
        for (size_t i = 0; i < count; i++)
            data[i] = other.data[other.indexOf(i)];
        return *this;
    }

    ~Queue() { delete[] data; }

    void push(const T& value) {
        if (count == capacity) grow(capacity * 2);
        data[indexOf(count)] = value;
        count++;
    }

    T pop() {
        if (empty())
            throw runtime_error("Очередь пуста: извлекать нечего");
        T value = data[head];
        head = (head + 1) % capacity;
        count--;
        return value;
    }

    const T& front() const {
        if (empty()) throw runtime_error("Очередь пуста: головы нет");
        return data[head];
    }

    const T& back() const {
        if (empty()) throw runtime_error("Очередь пуста: хвоста нет");
        return data[indexOf(count - 1)];
    }

    const T& at(size_t i) const {
        if (i >= count) throw out_of_range("Нет такого места в очереди");
        return data[indexOf(i)];
    }

    bool   empty()       const { return count == 0; }
    size_t size()        const { return count; }
    size_t getCapacity() const { return capacity; }

    void clear() { count = 0; head = 0; }
};

size_t utf8Length(const string& s) {
    size_t n = 0;
    for (size_t i = 0; i < s.size(); i++)
        if ((static_cast<unsigned char>(s[i]) & 0xC0) != 0x80) n++;
    return n;
}

string padRight(const string& s, size_t width) {
    size_t len = utf8Length(s);
    return (len >= width) ? s : s + string(width - len, ' ');
}

struct Student {
    string name;
    int    grade;

    Student() : name("-"), grade(0) {}
    Student(const string& n, int g) : name(n), grade(g) {}
};

string toText(int value) {
    return to_string(value);
}

string toText(double value) {
    ostringstream stream;
    stream << value;
    return stream.str();
}

string toText(const string& value) {
    return value;
}

string toText(const Student& s) {
    return s.name + "(" + to_string(s.grade) + ")";
}

template <typename T>
string queueToText(const Queue<T>& q) {
    string result = "[";
    for (size_t i = 0; i < q.size(); i++) {
        if (i > 0) result += ", ";
        result += toText(q.at(i));
    }
    return result + "]";
}

template <typename T>
void printInfo(const string& title, const Queue<T>& q) {
    cout << "  " << padRight(title, 24) << queueToText(q)
         << "   (размер " << q.size() << ", ёмкость " << q.getCapacity() << ")\n";
}

void demoInt() {
    cout << "\n--- Демонстрация 1: Queue<int> ---\n";
    Queue<int> q(2);
    printInfo("пустая очередь:", q);

    for (int i = 10; i <= 60; i += 10) {
        q.push(i);
        cout << "  push(" << setw(2) << i << ") -> " << queueToText(q)
             << "   ёмкость " << q.getCapacity() << "\n";
    }
    cout << "\n  Рост ёмкости 2 -> 4 -> 8 происходит автоматически:\n";
    cout << "  когда массив заполнен, выделяется вдвое больший.\n\n";

    cout << "  front() = " << q.front() << ", back() = " << q.back() << "\n";
    cout << "  pop() -> " << q.pop() << ";  pop() -> " << q.pop() << "\n";
    printInfo("после двух pop():", q);
    cout << "  Обратите внимание: ёмкость не уменьшилась, а голова\n"
         << "  сместилась внутри того же массива — это и есть кольцевой буфер.\n";
}

void demoString() {
    cout << "\n--- Демонстрация 2: Queue<string> ---\n";
    Queue<string> queueA;
    queueA.push("Анна");
    queueA.push("Борис");
    queueA.push("Виктор");
    printInfo("очередь A:", queueA);

    Queue<string> queueB = queueA;
    queueB.pop();
    queueB.push("Галина");

    cout << "\n  Скопировали A в B, затем в B сделали pop() и push(\"Галина\"):\n";
    printInfo("очередь A:", queueA);
    printInfo("очередь B:", queueB);
    cout << "  A не изменилась — значит, копия получила собственный массив,\n"
         << "  а не указатель на чужой. Это работа конструктора копирования.\n";

    Queue<string> queueC;
    queueC.push("будет затёрто");
    queueC = queueA;
    cout << "\n";
    printInfo("очередь C = A:", queueC);
}

void demoOtherTypes() {
    cout << "\n--- Демонстрация 3: Queue<double> и Queue<Student> ---\n";

    Queue<double> prices;
    prices.push(19.99);
    prices.push(5.50);
    prices.push(120.0);
    printInfo("Queue<double>:", prices);

    Queue<Student> group;
    group.push(Student("Иванов", 5));
    group.push(Student("Петров", 4));
    group.push(Student("Сидоров", 3));
    printInfo("Queue<Student>:", group);

    cout << "  Первым сдаёт: " << toText(group.front()) << "\n";
    Student passed = group.pop();
    cout << "  " << passed.name << " сдал на " << passed.grade << "\n";
    printInfo("осталось в группе:", group);

    cout << "\n  Один и тот же шаблон обслужил int, string, double и Student —\n"
         << "  компилятор сгенерировал по отдельному классу для каждого типа.\n";
}

void demoErrors() {
    cout << "\n--- Демонстрация 4: обработка ошибок ---\n";
    Queue<int> q;
    try {
        cout << "  Пробуем pop() из пустой очереди...\n";
        q.pop();
    }
    catch (const runtime_error& e) {
        cout << "  Перехвачено исключение: " << e.what() << "\n";
    }

    q.push(1);
    try {
        cout << "  Пробуем обратиться к месту №5, когда в очереди 1 элемент...\n";
        cout << q.at(5);
    }
    catch (const out_of_range& e) {
        cout << "  Перехвачено исключение: " << e.what() << "\n";
    }
    cout << "  Программа продолжает работу.\n";
}

int readInt(const char* prompt, int minValue, int maxValue) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Ошибка: нужно ввести целое число.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (value < minValue || value > maxValue) {
            cout << "  Ошибка: допустим диапазон от " << minValue
                 << " до " << maxValue << ".\n";
            continue;
        }
        return value;
    }
}

template <typename T>
void interactiveMode(const string& typeName) {
    Queue<T> q;
    cout << "\n=== Ручной режим: Queue<" << typeName << "> ===\n";

    while (true) {
        cout << "\n  Очередь: " << queueToText(q)
             << "  (размер " << q.size() << ", ёмкость " << q.getCapacity() << ")\n";
        cout << "   1 - push (добавить в хвост)\n"
             << "   2 - pop  (извлечь из головы)\n"
             << "   3 - показать голову и хвост\n"
             << "   4 - очистить\n"
             << "   0 - назад\n";
        int cmd = readInt("  Команда: ", 0, 4);

        if (cmd == 0) break;

        try {
            if (cmd == 1) {
                T value;
                cout << "  Значение (" << typeName << "): ";
                cin >> value;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "  Ошибка: значение не подходит под тип "
                         << typeName << ".\n";
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                q.push(value);
                cout << "  Добавлено.\n";
            }
            else if (cmd == 2) {
                cout << "  Извлечено: " << toText(q.pop()) << "\n";
            }
            else if (cmd == 3) {
                cout << "  Голова: " << toText(q.front())
                     << ",  хвост: " << toText(q.back()) << "\n";
            }
            else {
                q.clear();
                cout << "  Очередь очищена.\n";
            }
        }
        catch (const exception& e) {
            cout << "  Ошибка: " << e.what() << "\n";
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    cout << "=== Лабораторная работа №3. Вариант 6: шаблон класса «Очередь» ===\n";

    while (true) {
        cout << "\n--- Меню ---\n"
             << " 1 - Демонстрация: Queue<int>\n"
             << " 2 - Демонстрация: Queue<string> и копирование\n"
             << " 3 - Демонстрация: Queue<double> и Queue<Student>\n"
             << " 4 - Демонстрация: обработка ошибок\n"
             << " 5 - Все демонстрации подряд\n"
             << " 6 - Ручной режим с Queue<int>\n"
             << " 7 - Ручной режим с Queue<string>\n"
             << " 0 - Выход\n";
        int choice = readInt("Ваш выбор: ", 0, 7);

        if      (choice == 0) break;
        else if (choice == 1) demoInt();
        else if (choice == 2) demoString();
        else if (choice == 3) demoOtherTypes();
        else if (choice == 4) demoErrors();
        else if (choice == 5) { demoInt(); demoString(); demoOtherTypes(); demoErrors(); }
        else if (choice == 6) interactiveMode<int>("int");
        else                  interactiveMode<string>("string");
    }

    cout << "\nРабота завершена.\n";
    return 0;
}
