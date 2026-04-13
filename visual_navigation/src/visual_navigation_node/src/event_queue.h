#include <atomic>
#include <memory>
#include <array>
#include <vector>
#include <limits>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace EventSystem{

    enum class EventType: uint16_t{
        UNKNOWN = 0,
        CAMERA_INITIALIZED,
        IMU_INITIALIZED,
        RANGE_FINDER_INTIALIZED,
        UAV_ARMED,
        UAV_READY,
        CUSTOM_EVENT_MIN,
        CUSTOM_EVENT_MAX = std::numeric_limits<std::uint16_t>::max()
    };


    class BaseEvent{
    public:
        BaseEvent(EventType event_type):event_type(event_type){};

    public:
        EventType get_event_type() const {
            return this->event_type;
        }

    private:
        EventType event_type{EventType::UNKNOWN};
    };

    class EventQueue{
    private:
        using EventVector = std::vector<std::unique_ptr<BaseEvent>>;
    public:
        EventQueue(){
            events.reserve(max_events);
            this->processing_thread = std::thread(&processing_method, this);
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

    private:
        void processing_method(){
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
                auto event = local_events.begin();
                for(auto& event: local_events)
                    base_events_processing(event);
                local_events.clear();
            }

            std::unique_lock<std::mutex> lock{mutex};
            // Допроцессинг ивентов после выхода
            for(auto& event: events)
                    base_events_processing(event);  
        }

    private:
        void base_events_processing(const std::unique_ptr<BaseEvent>& event){
            auto event_type = event.get()->get_event_type();
            switch (event_type)
            {
            case EventType::UNKNOWN:
                // Ничего не делаем, поскольку ивент неизвестный
                break;
            
            case EventType::CAMERA_INITIALIZED:
                // Тут какая то логика при инициализации камеры
                break;

            case EventType::IMU_INITIALIZED:
                // Тут какая-то логика при инициализации гироскопа
                break;

            case EventType::UAV_ARMED:
                // Тут какая-то логика при успешном арминге
                break;

            case EventType::UAV_READY:
                // Тут какая-то логика когда дрон полностью прошел процедуру инициализации
                break;

            default:
                // Тут обработка оставшихся ивентов. Потом перенесу.
            }

        }


    private:
        bool is_quit_required{false};
        bool is_working{false};
        std::mutex mutex{};
        std::condition_variable condition_variable{};

        std::thread processing_thread{};
        EventVector events{};

    private:
        uint8_t max_events{100};
    };   

}