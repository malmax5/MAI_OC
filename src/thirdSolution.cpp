#include "../includes/Matrix.hpp"
#include "../includes/TaskQueue.hpp"
#include <cmath>
#include <iostream>

Matrix Matrix::DevideMatrixWithPids3(Matrix& first, Matrix& other) {
    if (first.rows != other.columns)
        throw std::invalid_argument("Can't divide these matrices");

    Matrix res(first.rows, other.columns);
    TaskQueue& taskQueue = TaskQueue::getInstance();
    taskQueue.start(maxThreads);
    
    std::vector<ThreadArgs> threadArgs(first.rows * other.columns);

    int threadId = 0;

    for (int i = 0; i < first.rows; i++) {
        for (int j = 0; j < other.columns; j++) {
            threadArgs[threadId] = {&res, &first, &other, i, j};

            taskQueue.addTask([threadId, threadArgs]() {
                ThreadArgs args = threadArgs[threadId];
                Complex resNum;
                for (int k = 0; k < args.first->GetSize().second; k++) {
                    resNum += args.first->mat[args.rowFirst][k] * args.other->mat[k][args.columnOther];
                }
                pthread_mutex_lock(&Matrix::mutex);
                args.res->mat[args.rowFirst][args.columnOther] = resNum;
                pthread_mutex_unlock(&Matrix::mutex);
            });

            threadId++;
        }
    }

    taskQueue.stop();
    
    return res;
}
