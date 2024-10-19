#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "includes/Matrix.hpp"
#include "includes/TaskQueue.hpp"

Complex randomComplex() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(-10.0, 10.0);
    return Complex(dis(gen), dis(gen));
}

void fillMatrix(Matrix& matrix) {
    std::pair<int, int> size = matrix.GetSize();
    for (int i = 0; i < size.first; i++) {
        for (int j = 0; j < size.second; j++) {
            matrix[i][j] = randomComplex();
        }
    }
}

int main(int argc, char* argv[]) 
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads>" << std::endl;
        return 1;
    }

    Matrix::SetMaxThreads(std::stoi(argv[1]));

    const int size = 1000;

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

    std::cout << "--Divide with threads(third try)--" << "\n";
    start = std::chrono::high_resolution_clock::now();
    Matrix result4 = Matrix::DevideMatrixWithPids3(m1, m2);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration4 = end - start;
    std::cout << "Time for threaded multiplication: " << duration4.count() << " seconds" << "\n--End--\n";

    // Проверка корректности
    std::cout << "Testing results for equality...\n";
    bool equal1 = (result1 == result2);
    bool equal2 = (result1 == result3);
    bool equal3 = (result1 == result4);

    if (equal1 && equal2 && equal3) {
        std::cout << "All results are equal.\n";
    } else {
        std::cout << "Results differ:\n";
        if (!equal1) {
            std::cout << "Result1 is not equal to Result2.\n";
        }
        if (!equal2) {
            std::cout << "Result1 is not equal to Result3.\n";
        }
        if (!equal3) {
            std::cout << "Result1 is not equal to Result3.\n";
        }
    }

    return 0;
}