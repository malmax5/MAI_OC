#include "../includes/Matrix.hpp"

int Matrix::maxThreads = 1;
pthread_mutex_t Matrix::mutex = PTHREAD_MUTEX_INITIALIZER;

Matrix::Matrix() : rows(0), columns(0) {}
Matrix::Matrix(int rowsNum, int columnsNum) 
    : rows(rowsNum), columns(columnsNum)
{
    mat.resize(rows, std::vector<Complex>(columns));
}
Matrix::Matrix(std::vector<std::vector<Complex>> matrix)
    : mat(matrix), rows(mat.size()), columns(mat[0].size()) {}
Matrix::Matrix(const Matrix& other)
    : mat(other.mat), rows(other.rows), columns(other.columns) {}
Matrix::Matrix(const Matrix&& other)
    : mat(std::move(other.mat)),
      rows(std::move(other.rows)),
      columns(std::move(other.columns)) {}
Matrix::~Matrix() {}

std::pair<int, int> Matrix::GetSize() const
{
    return std::make_pair(this->rows, this->columns);
}

Matrix Matrix::DevideMatrix(Matrix& first, Matrix& other)
{
    if (first.rows != other.columns)
        throw std::invalid_argument("Can't devide this matrixes");
    
    Matrix res(first.rows, other.columns);
    for (int i = 0; i < first.rows; i++)
        for (int j = 0; j < other.columns; j++)
            for (int k = 0; k < first.columns; k++)
                res.mat[i][j] += first.mat[i][k] * other.mat[k][j];
    
    return res;
}

Matrix& Matrix::operator=(Matrix& other)
{
    if (this != &other)
    {
        rows = other.rows;
        columns = other.columns;
        mat = other.mat;
    }

    return *this;
}

Matrix& Matrix::operator=(Matrix&& other)
{
    if (this != &other)
    {
        rows = std::move(other.rows);
        columns = std::move(other.columns);
        mat = std::move(other.mat);
    }

    return *this;
}

Matrix Matrix::operator*(Matrix& other)
{
    return DevideMatrixWithPids(*this, other);
}

std::vector<Complex>& Matrix::operator[](int index) {
    return mat[index];
}

const std::vector<Complex>& Matrix::operator[](int index) const {
    return mat[index];
}

std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
    std::pair<int, int> size = mat.GetSize();
    for (int i = 0; i < size.first; i++)
    {
        for (int j = 0; j < size.second; j++)
        {
            os << mat.mat[i][j] << " ";
        }
        os << "\n";
    }
    return os;
}
