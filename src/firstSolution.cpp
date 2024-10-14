#include "../includes/Matrix.hpp"

Matrix Matrix::DevideMatrixWithPids(Matrix& first, Matrix& other)
{
    if (first.rows != other.columns)
        throw std::invalid_argument("Can't devide this matrixes");
    
    std::vector<pthread_t> threads(maxThreads);
    std::vector<ThreadArgs> threadArgs(maxThreads);

    Matrix res(first.rows, other.columns);
    long unsigned int threadId = 0;

    for (int i = 0; i < first.rows; i++)
    {
        for (int j = 0; j < other.columns; j++)
        {
            if (threadId == maxThreads)
            {
                for (long unsigned int joinId = 0; joinId < threadId; joinId++)
                {
                    pthread_join(threads[joinId], NULL);
                }
                threadId = 0;
            }

            threadArgs[threadId] = {&res, &first, &other, i, j};
            pthread_create(&threads[threadId], NULL, DevideRowColumnByIndexis, &threadArgs[threadId]);
            threadId++;
        }
    }

    if (threadId != 0)
    {
        for (long unsigned int joinId = 0; joinId < threadId; joinId++)
        {
            pthread_join(threads[joinId], NULL);
        }
        threadId = 0;
    }

    return res;
}

void* Matrix::DevideRowColumnByIndexis(void* arg)
{
    // Matrix& res, Matrix& first, Matrix& other, int rowFirst, int columnOther
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);
    std::pair<int, int> size = args->res->GetSize();
    Complex resNum;
    for (int i = 0; i < size.second; i++)
        resNum += (args->first->mat[args->rowFirst][i] * args->other->mat[i][args->columnOther]);
    args->res->mat[args->rowFirst][args->columnOther] = resNum;
    pthread_exit(0);
}