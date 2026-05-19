/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Shell
*/

#include "Shell.hpp"
#include <iostream>
#include <string>
#include "exceptions/Exception.hpp"
#include "parser/Parser.hpp"

Shell::Shell() : running_(false) {}

void Shell::run() {
  running_ = true;
  while (running_) {
    printPrompt();
    std::string line;
    if (!std::getline(std::cin, line)) {
      break;
    }
    handleLine(line);
  }
}

void Shell::stop() { running_ = false; }

void Shell::setOrderCallback(
    std::function<void(std::vector<PizzaOrder>)> callback) {
  orderCb_ = std::move(callback);
}

void Shell::setStatusCallback(std::function<void()> callback) {
  statusCb_ = std::move(callback);
}

void Shell::handleLine(const std::string& line) {
  if (line == "status") {
    if (statusCb_) {
      statusCb_();
    }
    return;
  }
  try {
    auto orders = Parser::parse(line);
    if (orderCb_) {
      orderCb_(orders);
    }
  } catch (const std::exception& e) {
    plazza::exceptions::Exception::thrownError(std::string("error: ") +
                                               e.what());
  }
}

void Shell::printPrompt() { std::cout << "> "; }
