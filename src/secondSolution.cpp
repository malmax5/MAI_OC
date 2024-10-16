#include "../includes/Matrix.hpp"
#include "cmath"

Matrix Matrix::DevideMatrixWithPids2(Matrix& first, Matrix& other)
{
    if (first.rows != other.columns)
        throw std::invalid_argument("Can't devide this matrixes");
    
    std::vector<pthread_t> threads(maxThreads + 1);
    std::vector<ThreadArgs2> threadArgs(maxThreads + 1);

    Matrix res(first.rows, other.columns);
    int threadId = 0;
    int k = (static_cast<int>((other.columns + other.columns % maxThreads) / maxThreads) == 0 ? 1 : static_cast<int>((other.columns + other.columns % maxThreads) / maxThreads));

    int j = 0;
    for (int i = 0; i < first.rows; i++)
    {
        for (j = k; j < other.columns; j += k)
        {
            threadArgs[threadId] = {&res, &first, &other, i, j - k, j};
            
            if (pthread_create(&threads[threadId], NULL, DevideRowColumnByIndexis2, &threadArgs[threadId]) != 0)
                throw std::runtime_error("Can't create a thread");
            threadId++;
        }

        if (j > other.columns)
        {
            threadArgs[threadId] = {&res, &first, &other, i, j - k, other.columns};
            
            if (pthread_create(&threads[threadId], NULL, DevideRowColumnByIndexis2, &threadArgs[threadId]) != 0)
                throw std::runtime_error("Can't create a thread");
            threadId++;
        }

        for (int joinId = 0; joinId < threadId; joinId++)
        {
            if (pthread_join(threads[joinId], NULL) != 0)
                throw std::runtime_error("Can't join a thread");
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