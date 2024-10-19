#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include <queue>
#include <pthread.h>
#include <functional>
#include <vector>
#include <stdexcept>

class TaskQueue {
public:
    static TaskQueue& getInstance() {
        static TaskQueue instance;
        return instance;
    }

    void start(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            pthread_t worker;
            if (pthread_create(&worker, NULL, &TaskQueue::workerThread, this) != 0) {
                throw std::runtime_error("Failed to create thread");
            }
            workers.push_back(worker);
        }
    }

    void stop() {
        {
            pthread_mutex_lock(&mutex);
            stopFlag = true;
            pthread_cond_broadcast(&condVar);
            pthread_mutex_unlock(&mutex);
        }
        for (pthread_t& worker : workers) {
            pthread_join(worker, nullptr);
        }
    }

    void addTask(std::function<void()> task) {
        pthread_mutex_lock(&mutex);
        tasks.push(std::move(task));
        pthread_cond_signal(&condVar);
        pthread_mutex_unlock(&mutex);
    }

private:
    TaskQueue() : stopFlag(false) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&condVar, nullptr);
    }

    ~TaskQueue() {
        stop();
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&condVar);
    }

    static void* workerThread(void* arg) {
        TaskQueue* queue = static_cast<TaskQueue*>(arg);
        while (true) {
            std::function<void()> task;

            pthread_mutex_lock(&queue->mutex);
            while (!queue->stopFlag && queue->tasks.empty()) {
                pthread_cond_wait(&queue->condVar, &queue->mutex);
            }
            if (queue->stopFlag && queue->tasks.empty()) {
                pthread_mutex_unlock(&queue->mutex);
                break;
            }

            task = std::move(queue->tasks.front());
            queue->tasks.pop();
            pthread_mutex_unlock(&queue->mutex);

            task();
        }
        return nullptr;
    }

    std::vector<pthread_t> workers;
    std::queue<std::function<void()>> tasks;
    pthread_mutex_t mutex;
    pthread_cond_t condVar;
    bool stopFlag;
};

#endif
