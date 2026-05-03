#pragma once

#include <memory>
#include <array>
#include <vector>
#include <limits>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

template<int ThreadCount>
class ThreadPool{
private:
    using Task = std::function<void()>;
    using TaskQueue = std::vector<Task>;

public:
    ThreadPool(){
        initialize_threads();
    };
    ~ThreadPool(){
        quit();
        stop_threads();
    }
    ThreadPool(const ThreadPool& other_queue) = delete;
    ThreadPool& operator=(const ThreadPool& other_queue) = delete;
    ThreadPool(ThreadPool&& other_queue) = delete;  //TODO сделать мув конструктор
    ThreadPool& operator=(ThreadPool&& other_queue) = delete;  //TODO сделать мув присваивание

public:
    void start(){
        std::unique_lock<std::mutex> lock{mutex};
        this->is_working = true;
        condition_variable.notify_one();
    }
    void stop(){
        std::unique_lock<std::mutex> lock{mutex};
        this->is_working = false;
        condition_variable.notify_one();
    }
    void quit(){
        std::unique_lock<std::mutex> lock{mutex};
        this->is_quit_required = true;
        condition_variable.notify_one();
    }

    template<typename Functor, typename ...Args>
    auto add_task(Functor functor, Args... args){
        using LambdaResultType = std::invoke_result_t<Functor, Args...>;
        auto binded_task = std::bind(std::forward<Functor>(functor), std::forward<Args>(args)...);
        auto packaged_task = std::packaged_task<LambdaResultType()>(std::move(binded_task));
        auto smart_pointer_packaged_task = std::make_shared<decltype(packaged_task)>(std::move(packaged_task));
        auto future = smart_pointer_packaged_task->get_future();
        std::unique_lock<std::mutex> lock(mutex);
        this->tasks.emplace_back([task = std::move(smart_pointer_packaged_task)](){(*task)();});
        condition_variable.notify_one();
        return future;
    }
private:
    void processing_loop(){
        while(true){
            Task task{};
            {
                std::unique_lock<std::mutex> lock{mutex};
                condition_variable.wait(lock,[this](){return this->is_quit_required || ( this->is_working && !this->tasks.empty());});
                // Если вышли из кондишна по окончанию работы - вываливаемся из цикла
                if(this->is_quit_required)
                    break;
                task = std::move(tasks.back());
                tasks.pop_back();
            }

            task();
        }
        std::unique_lock<std::mutex> lock{mutex};
        // Допроцессинг ивентов после выхода
        for(auto& task: tasks)
            task();
    }

    void initialize_threads(){
        tasks.reserve(max_tasks);
        for(auto& thread: thread_array){
            thread = std::thread(&ThreadPool::processing_loop,this);

        }
    }

    void stop_threads(){
        for(auto& thread: thread_array){
            if(thread.joinable())
                thread.join();
        }
    }


private:
    bool is_quit_required{false};
    bool is_working{false}; 
    std::mutex mutex{};
    std::condition_variable condition_variable{};

    TaskQueue tasks{};
    std::array<std::thread, ThreadCount> thread_array{};

private:
    uint8_t max_tasks{100};
};   