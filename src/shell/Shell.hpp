/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Shell
*/
#pragma once

#include <functional>
#include <string>
#include <vector>
#include "Pizza.hpp"

class Shell {
 public:
  explicit Shell();

  void run();
  void stop();

  void setOrderCallback(
      std::function<void(const std::vector<PizzaOrder>&)> callback);
  void setStatusCallback(std::function<void()> callback);

 private:
  bool running_;
  std::function<void(const std::vector<PizzaOrder>&)> orderCb_;
  std::function<void()> statusCb_;

  void handleLine(const std::string& line);
  static void printPrompt();
};
