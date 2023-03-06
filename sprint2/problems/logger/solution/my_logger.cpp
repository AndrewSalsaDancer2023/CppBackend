#include "my_logger.h"
#include <ctime>

std::string ToString(int n)
{
  if(n < 10)	
	return '0'+ std::to_string(n);

  return std::to_string(n);
}

std::string Logger::GetFileTimeStamp() const
{
  const auto now = GetTime();
  const auto tnow = std::chrono::system_clock::to_time_t(now);
  std::tm* pnow = std::localtime(&tnow);
 
  return "sample_log_" + std::to_string(pnow->tm_year + 1900) + "_" + 
  	  ToString(pnow->tm_mon + 1) + "_" + ToString(pnow->tm_mday) + ".log";
}

void Logger::SetTimestamp(std::chrono::system_clock::time_point ts)
{
	std::lock_guard lg(m_);
	manual_ts_ = ts;
}
