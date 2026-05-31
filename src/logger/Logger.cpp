/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Logger
*/

#include "logger/Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace {

std::string timestamp() {
  const auto now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const auto* localTm = std::localtime(&now);
  std::ostringstream oss;
  oss << std::put_time(localTm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

}  // namespace

Logger::Logger(const std::string& filename)
    : file_(filename, std::ios::app) {}

void Logger::logPizzaDone(int kitchenId, const std::string& pizzaName) {
  std::lock_guard<Mutex> lock(mutex_);
  file_ << "[" << timestamp() << "] Kitchen #" << kitchenId << " - "
        << pizzaName << " done\n";
  file_.flush();
}
