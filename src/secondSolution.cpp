#include "../includes/Matrix.hpp"
#include "cmath"

Matrix Matrix::DevideMatrixWithPids2(Matrix& first, Matrix& other) {
    if (first.rows != other.columns)
        throw std::invalid_argument("Can't divide these matrices");

    Matrix res(first.rows, other.columns);
    std::vector<pthread_t> threads(maxThreads);
    std::vector<ThreadArgs2> threadArgs(maxThreads);

    int threadId = 0;

    int columnsPerThread = (other.columns + maxThreads - 1) / maxThreads;

    for (int i = 0; i < first.rows; i++) {
        for (int j = 0; j < other.columns; j += columnsPerThread) {
            int endColumn = std::min(j + columnsPerThread, other.columns);
            threadArgs[threadId] = {&res, &first, &other, i, j, endColumn};

            if (pthread_create(&threads[threadId], nullptr, DevideRowColumnByIndexis2, &threadArgs[threadId]) != 0) {
                throw std::runtime_error("Can't create a thread");
            }
            threadId++;
        }

        for (int joinId = 0; joinId < threadId; joinId++) {
            if (pthread_join(threads[joinId], nullptr) != 0) {
                throw std::runtime_error("Can't join a thread");
            }
        }

        threadId = 0;
    }

    return res;
}


void* Matrix::DevideRowColumnByIndexis2(void* arg)
{
    // Matrix& res, Matrix& first, Matrix& other, int rowFirst, int startColumn, int endColumn
    ThreadArgs2* args = static_cast<ThreadArgs2*>(arg);
    std::pair<int, int> size = args->res->GetSize();
    for(int j = args->startColumn; j < args->endColumn; j++)
    {
        Complex resNum;
        for (int i = 0; i < size.second; i++)
            resNum += (args->first->mat[args->rowFirst][i] * args->other->mat[i][j]);
        
        pthread_mutex_lock(&mutex);
        args->res->mat[args->rowFirst][j] = resNum;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}