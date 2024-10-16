#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "Complex.hpp"

#include <vector>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

class Matrix
{
public:
    Matrix();
    Matrix(int rows, int columns);
    Matrix(std::vector<std::vector<Complex>> matrix);
    Matrix(const Matrix& other);
    Matrix(const Matrix&& other);
    ~Matrix();

    void Print();
    std::pair<int, int> GetSize() const;
    static Matrix DevideMatrix(Matrix& first, Matrix& other);

    // first try
    static Matrix DevideMatrixWithPids(Matrix& first, Matrix& other);
    static void* DevideRowColumnByIndexis(void* arg);

    // second try
    static Matrix DevideMatrixWithPids2(Matrix& first, Matrix& other);
    static void* DevideRowColumnByIndexis2(void* arg);

    Matrix& operator=(Matrix& other);
    Matrix& operator=(Matrix&& other);
    
    Matrix operator*(Matrix& other);

    std::vector<Complex>& operator[](int index);
    const std::vector<Complex>& operator[](int index) const;

    friend std::ostream& operator<<(std::ostream& os, const Matrix& c);

    static void SetMaxThreads(int numThreads)
    {
        int num_processors = sysconf(_SC_NPROCESSORS_ONLN);
        maxThreads = numThreads < num_processors ? numThreads : num_processors;
    }

private:
    static int maxThreads;
    static pthread_mutex_t mutex;

    std::vector<std::vector<Complex>> mat;
    int rows;
    int columns;


    struct ThreadArgs
    {
        Matrix* res;
        Matrix* first;
        Matrix* other;
        int rowFirst;
        int columnOther;
    };

    struct ThreadArgs2
    {
        Matrix* res;
        Matrix* first;
        Matrix* other;
        int rowFirst;
        int startColumn;
        int endColumn;
    };
};

#endif