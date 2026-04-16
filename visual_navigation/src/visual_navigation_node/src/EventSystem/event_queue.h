#pragma once

#include <memory>
#include <array>
#include <vector>
#include <limits>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "Events/base_event.h"

namespace EventSystem{

    class EventQueue{
    public:
        using Event = std::unique_ptr<BaseEvent>;
        using EventVector = std::vector<Event>; 
        using ProcessingMethod = std::function<void(const Event&)>;

    public:
        EventQueue(){
            events.reserve(max_events);
            this->processing_thread = std::thread(&EventQueue::processing_loop, this);
        };

        ~EventQueue(){
            quit();
            if(this->processing_thread.joinable())
                this->processing_thread.join();
        }

        EventQueue(const EventQueue& other_queue) = delete;
        EventQueue& operator=(const EventQueue& other_queue) = delete;

        EventQueue(EventQueue&& other_queue) = delete;  //TODO сделать мув конструктор
        EventQueue& operator=(EventQueue&& other_queue) = delete;  //TODO сделать мув присваивание


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

        void push_event(std::unique_ptr<BaseEvent> event){
            std::unique_lock<std::mutex> lock(mutex);
            this->events.push_back(std::move(event));
            condition_variable.notify_one();
        }

        void set_processing_method(ProcessingMethod method){
            this->processing_method = std::move(method);
        }

    private:
        void processing_loop(){
            EventVector local_events{};
            local_events.reserve(max_events);
            while(true){
                // Лочим мьютекс и отдаем его в кондишн чтобы, ждать пока мы полностью не выйдем или пока не в очереди не появится ивент и работа не будет запущена
                std::unique_lock<std::mutex> lock{mutex};
                {
                    condition_variable.wait(lock,[this](){return this->is_quit_required || ( this->is_working && !this->events.empty());});
                    // Если вышли из кондишна по окончанию работы - вываливаемся из цикла
                    if(this->is_quit_required)
                        break;
                    // Если нет - значит, очередь запустили и там есть ивенты. Под мьютексом муваем вектор в локалку
                    local_events.swap(events);
                }
                for(auto& event: local_events)
                    processing_method(event);
                local_events.clear();
            }

            std::unique_lock<std::mutex> lock{mutex};
            // Допроцессинг ивентов после выхода
            for(auto& event: events)
                processing_method(event);
        }

    private:
        bool is_quit_required{false};
        bool is_working{false}; 
        std::mutex mutex{};
        std::condition_variable condition_variable{};
        std::thread processing_thread{};

        EventVector events{};
        ProcessingMethod processing_method{};

    private:
        uint8_t max_events{100};
    };   

}