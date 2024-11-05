#include <fstream>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <cstdlib>

#include "commonUtils.h"

std::string Logger::logFilePath;

std::string Logger::getLogFilePath()
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "logs/%d%m%Y-%H%M%S.log");
  return ss.str();
}

std::string Logger::getCurrentDateTime(const std::string &format)
{
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), format.c_str());
  return ss.str();
}

void Logger::deleteOldLogs()
{
  // Ensure the logs/ directory exists
  int mkdirResult = system("mkdir -p logs");
  if (mkdirResult != 0)
  {
    std::cerr << "Failed to create or verify the logs / directory." << std::endl;
    // Handle error or return if critical
  }
  else
  {
    // Command to keep the latest 10 log files, deleting any beyond that
    int rmResult = system("ls -t logs/*.log | tail -n +11 | xargs -r rm --");
    if (rmResult != 0)
    {
      std::cerr << "Error occurred while deleting old log files." << std::endl;
      // Handle error or return if critical
    }
    logFilePath = getLogFilePath();
    if (logFilePath.empty())
      std::cerr << "Error occurred while creating log file." << std::endl;
  }
}

void Logger::logMessage(const std::string &message)
{
  std::ofstream logFile(logFilePath, std::ios::app);
  if (!logFile.is_open())
  {
    std::cerr << "[Error] Failed to open log file: " << logFilePath << std::endl;
    exit(1);
    return;
  }

  std::string timestamp = getCurrentDateTime("[%Y-%m-%d %H:%M:%S] ");
  logFile << timestamp << message << std::endl;
  logFile.close();
}