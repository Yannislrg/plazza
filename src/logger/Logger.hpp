/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Logger
*/

#pragma once

#include <fstream>
#include <string>
#include "concurrency/Mutex.hpp"

class Logger {
 public:
  explicit Logger(const std::string& filename = "plazza.log");
  ~Logger() = default;

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

  void logPizzaDone(int kitchenId, const std::string& pizzaName);

 private:
  std::ofstream file_;
  Mutex mutex_;
};
