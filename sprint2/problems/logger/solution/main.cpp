#include <iostream>
#include <thread>
#include <filesystem>
#include "my_logger.h"

int main(int argc, const char* argv[]) {
	try{
//		LOG("Hello world from logger!!!", 1, 2, 3, 4 ,5);
/*		static const int attempts = 100000;
	    for(int i = 0; i < attempts; ++i) {
        	Logger::GetInstance().SetTimestamp(std::chrono::system_clock::time_point(std::chrono::seconds(10000000 + i * 100)));
	        LOG("Logging attempt ", i, ". ", "I Love it");*/
	    }
	}catch(const std::exception& ex)
	{
		std::cout << ex.what();
	}
return 0;
}

