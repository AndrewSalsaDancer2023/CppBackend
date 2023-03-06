#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>
#include <iostream>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const;

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Выведите в поток все аргументы.
/*    template<class... Ts>
    void Log(const Ts&... args);
*/
void LogInfo() {
//	std::cout << std::endl;
	log_file_ << std::endl;
}

template <typename A, typename... B>
void LogInfo(A num, B... args) {
//	std::cout << num << " ";
	log_file_ << num << " ";
	LogInfo(args...);
}

template <typename A, typename... B>
void Log(A num, B... args) {
	std::lock_guard lg(m_);
	log_file_.open("/var/log/" + GetFileTimeStamp(), std::ios::app);
	log_file_ << GetTimeStamp() << ": ";
	LogInfo(num, args...);
	
	log_file_.close();
}


    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts);

private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::ofstream log_file_;
    std::mutex m_;
};
