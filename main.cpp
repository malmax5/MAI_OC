#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "includes/Matrix.hpp"

// Функция для генерации случайных комплексных чисел
Complex randomComplex() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(-10.0, 10.0);
    return Complex(dis(gen), dis(gen));
}

// Функция для заполнения матрицы случайными комплексными числами
void fillMatrix(Matrix& matrix) {
    std::pair<int, int> size = matrix.GetSize();
    for (int i = 0; i < size.first; i++) {
        for (int j = 0; j < size.second; j++) {
            matrix[i][j] = randomComplex();
        }
    }
}

int main() {
    const int size = 100;

    Matrix m1(size, size);
    fillMatrix(m1);

    Matrix m2(size, size);
    fillMatrix(m2);

    std::cout << "--Divide without threads--" << "\n";
    auto start = std::chrono::high_resolution_clock::now();
    Matrix result1 = Matrix::DevideMatrix(m1, m2);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end - start;
    std::cout << "Time for regular multiplication: " << duration1.count() << " seconds" << "\n--End--\n";

    std::cout << "--Divide with threads(first try)--" << "\n";
    start = std::chrono::high_resolution_clock::now();
    Matrix result2 = Matrix::DevideMatrixWithPids(m1, m2);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = end - start;
    std::cout << "Time for threaded multiplication: " << duration2.count() << " seconds" << "\n--End--\n";

    std::cout << "--Divide with threads(second try)--" << "\n";
    start = std::chrono::high_resolution_clock::now();
    Matrix result3 = Matrix::DevideMatrixWithPids2(m1, m2);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration3 = end - start;
    std::cout << "Time for threaded multiplication: " << duration3.count() << " seconds" << "\n--End--\n";

    // std::cout << result1 << "\n";
    // std::cout << result2 << "\n";
    // std::cout << result3 << "\n";

    return 0;
}