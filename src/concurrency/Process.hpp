/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Process
*/

#pragma once

#include <sys/types.h>
#include <functional>

class Process {
 public:
  explicit Process(const std::function<void()>& routine);
  ~Process();

  Process(const Process&) = delete;
  Process& operator=(const Process&) = delete;
  Process(Process&&) noexcept;
  Process& operator=(Process&&) noexcept;

  void wait();
  [[nodiscard]] pid_t pid() const noexcept;

 private:
  pid_t pid_{-1};
  bool done_{false};
};
