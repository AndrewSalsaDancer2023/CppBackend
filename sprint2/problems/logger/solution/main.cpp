#include <iostream>
#include <thread>
#include <filesystem>
#include "my_logger.h"

int main(int argc, const char* argv[]) {
	try{
		LOG("Hello world from logger!!!", 1, 2, 3, 4 ,5);
	}catch(const std::exception& ex)
	{
		std::cout << ex.what();
	}
return 0;
}

