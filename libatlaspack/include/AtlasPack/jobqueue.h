/*
 * MIT License
 *
 * Copyright (c) 2017 Benjamin Zeller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ATLASPACK_JOBQUEUE_INCLUDED
#define ATLASPACK_JOBQUEUE_INCLUDED

#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <deque>
#include <functional>
#include <condition_variable>
#include <iostream>

#include <boost/core/noncopyable.hpp>

namespace AtlasPack {

template <typename T> class JobQueue : public boost::noncopyable{
    public:

    JobQueue(size_t threadPool = 0) {

        size_t reqThreads = threadPool > 0 ? threadPool : maxJobs();
        m_threadPool.reserve(reqThreads);

        for (size_t t = 0; t < reqThreads; t++ ) {
            std::shared_ptr<std::thread> daThread = std::make_shared<std::thread>(threadMain, this);
            m_threadPool.push_back(daThread);
        }
    }

    ~JobQueue() {
        m_stop.store(true);
        m_wakeup.notify_all();

        for (std::shared_ptr<std::thread> curr : m_threadPool) {
            curr->join();
        }
    }

    std::future<T> addTask (std::function<T()> &&fun) {
        std::packaged_task<T()> task(fun);
        std::future<T> fut = task.get_future();

        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_waitingTasks.push_back(std::move(task));
        }
        m_wakeup.notify_one();
        return fut;
   }

    void waitForAllRunningTasks () {
        std::unique_lock<std::mutex> lk(m_mutex);

        //check if there are thread running or tasks pending
        while (m_waitingTasks.size() != 0) {
            m_queue_empty.wait(lk);
        }
    }

    unsigned int maxJobs () const {
        unsigned int jobs = std::thread::hardware_concurrency();
        if (jobs == 0)
            return 2;
        return jobs;
    }


    private:

        static void threadMain (JobQueue<T> *queue) {

            while (!queue->m_stop.load()) {

                std::packaged_task<T()> task;
                {
                    std::unique_lock<std::mutex> lk(queue->m_mutex);

                    while (queue->m_waitingTasks.size() == 0) {
                        queue->m_wakeup.wait(lk);

                        if(queue->m_stop.load()) {

                            std::cout<<"Exit Exit Exit"<<std::endl;
                            return;
                        }
                    }

                    task = std::move(queue->m_waitingTasks.front());
                    queue->m_waitingTasks.pop_front();
                    queue->m_runningThreads++;
                }

                task();

                {
                    std::unique_lock<std::mutex> lk(queue->m_mutex);
                    queue->m_runningThreads--;
                    if (queue->m_waitingTasks.size() == 0 && queue->m_runningThreads == 0) {
                        queue->m_queue_empty.notify_all();
                    }
                }
            }

            std::cout<<"Exit Exit Exit 1"<<std::endl;
        }

        std::vector<std::shared_ptr<std::thread> > m_threadPool;
        std::deque<std::packaged_task<T()> > m_waitingTasks;
        std::size_t m_runningThreads = 0;

        std::atomic_bool m_stop{false};
        std::mutex m_mutex;
        std::condition_variable m_wakeup; //always notified when there are changes to the worker
        std::condition_variable m_queue_empty; //notifies if all tasks have been finished
};

}

#endif
