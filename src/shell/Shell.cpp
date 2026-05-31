/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Shell
*/

#include "Shell.hpp"
#include <sys/select.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "exceptions/Exception.hpp"
#include "parser/Parser.hpp"

Shell::Shell() : running_(false) {}

void Shell::run() {
  running_ = true;
  printPrompt();
  while (running_) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv{0, 100000};
    const int ret = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
    if (ret > 0) {
      std::string line;
      if (!std::getline(std::cin, line)) {
        break;
      }
      handleLine(line);
      printPrompt();
    } else if (ret == 0 && pollCb_) {
      pollCb_();
    }
  }
}

void Shell::stop() { running_ = false; }

void Shell::setOrderCallback(
    std::function<void(const std::vector<PizzaOrder>&)> callback) {
  orderCb_ = std::move(callback);
}

void Shell::setStatusCallback(std::function<void()> callback) {
  statusCb_ = std::move(callback);
}

void Shell::setPollCallback(std::function<void()> callback) {
  pollCb_ = std::move(callback);
}

void Shell::handleLine(const std::string& line) {
  std::string trimmed = line;
  while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\r' ||
                              trimmed.back() == '\t')) {
    trimmed.pop_back();
  }
  if (trimmed.empty()) {
    return;
  }
  if (trimmed == "status") {
    if (statusCb_) {
      statusCb_();
    }
    return;
  }
  try {
    auto orders = Parser::parse(trimmed);
    if (orderCb_) {
      orderCb_(orders);
    }
  } catch (const std::exception& e) {
    plazza::exceptions::Exception::thrownError(std::string("error: ") +
                                               e.what());
  }
}

void Shell::printPrompt() { std::cout << "> " << std::flush; }
