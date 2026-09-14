//  Лабораторная работа №1
//
//  Условие: Разработать консольное приложение, имеющее функцию для ввода данных,
//  обработки и вывода. Структуру данных разместить в куче. Размерность структуры
//  задаётся во время выполнения программы.
//
//  Вариант 6: Поменять порядок строк матрицы на обратный.

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma execution_character_set("utf-8")

using namespace std;

int   readInt(const char* prompt, int minValue, int maxValue);
int** createMatrix(int rows, int cols);
void  freeMatrix(int**& matrix, int rows);
void  inputMatrix(int** matrix, int rows, int cols);
void  fillMatrixRandom(int** matrix, int rows, int cols);
void  printMatrix(int** matrix, int rows, int cols);
void  reverseRows(int** matrix, int rows);


int main()
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    srand(static_cast<unsigned>(time(nullptr)));

    cout << "Изменение порядка строк матрицы на обратный\n\n";

    int rows = readInt("Введите количество строк    (1..20): ", 1, 20);
    int cols = readInt("Введите количество столбцов (1..20): ", 1, 20);

    int** matrix = createMatrix(rows, cols);

    int mode = readInt("\nСпособ заполнения (1 - вручную, 2 - случайно): ", 1, 2);
    if (mode == 1)
        inputMatrix(matrix, rows, cols);
    else
        fillMatrixRandom(matrix, rows, cols);

    cout << "\nИсходная матрица:\n";
    printMatrix(matrix, rows, cols);

    reverseRows(matrix, rows);

    cout << "\nРезультат:\n";
    printMatrix(matrix, rows, cols);

    freeMatrix(matrix, rows);

    cout << "\nРабота завершена.\n";
    cout << "Нажмите Enter для выхода...";
    cin.get();
    return 0;
}


int readInt(const char* prompt, int minValue, int maxValue)
{
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000000, '\n');
            cout << "  Ошибка: нужно ввести целое число.\n";
            continue;
        }
        cin.ignore(1000000, '\n');

        if (value < minValue || value > maxValue) {
            cout << "  Ошибка: число должно быть в диапазоне от "
                << minValue << " до " << maxValue << ".\n";
            continue;
        }
        return value;
    }
}


int** createMatrix(int rows, int cols)
{
    int** matrix = new int* [rows];
    for (int i = 0; i < rows; i++)
        matrix[i] = new int[cols];
    return matrix;
}


void freeMatrix(int**& matrix, int rows)
{
    if (matrix == nullptr) return;
    for (int i = 0; i < rows; i++)
        delete[] matrix[i];
    delete[] matrix;
    matrix = nullptr;
}

void inputMatrix(int** matrix, int rows, int cols)
{
    cout << "\nВведите элементы матрицы:\n";
    char prompt[64];
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            sprintf_s(prompt, sizeof(prompt), "  a[%d][%d] = ", i, j);
            matrix[i][j] = readInt(prompt, -9999, 9999);
        }
}

void fillMatrixRandom(int** matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            matrix[i][j] = rand() % 199 - 99;
}

void printMatrix(int** matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            cout << setw(6) << matrix[i][j];
        cout << '\n';
    }
}

void reverseRows(int** matrix, int rows)
{
    for (int i = 0; i < rows / 2; i++) {
        int* temp = matrix[i];
        matrix[i] = matrix[rows - 1 - i];
        matrix[rows - 1 - i] = temp;
    }
}
