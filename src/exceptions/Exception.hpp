/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Common exceptions
*/

#pragma once

#include <exception>
#include <string>

namespace plazza::exceptions {

class Exception : public std::exception {
 public:
  explicit Exception(std::string message) : message_(std::move(message)) {}
  [[nodiscard]] const char* what() const noexcept override {
    return message_.c_str();
  }
  [[noreturn]] static void thrownError(std::string message) {
    throw Exception(std::move(message));
  }

 private:
  std::string message_;
};

}  // namespace plazza::exceptions
