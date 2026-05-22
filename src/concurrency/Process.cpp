/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Process
*/

#include "concurrency/Process.hpp"
#include <sys/wait.h>
#include <unistd.h>
#include <functional>
#include <string>
#include "exceptions/Exception.hpp"
#include "utils/Constant.hpp"

Process::Process(const std::function<void()>& routine) {
  const pid_t forkedPid = fork();  // NOLINT(misc-include-cleaner)
  if (forkedPid < 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kForkFailed));
  }
  if (forkedPid == 0) {
    routine();
    _exit(0);
  }
  pid_ = forkedPid;
}

Process::~Process() {
  if (!done_ && pid_ > 0) {
    waitpid(pid_, nullptr, 0);
  }
}

void Process::wait() {
  if (!done_ && pid_ > 0) {
    waitpid(pid_, nullptr, 0);
    done_ = true;
  }
}

pid_t Process::pid() const noexcept { return pid_; }

Process::Process(Process&& other) noexcept
    : pid_(other.pid_)
    , done_(other.done_) {
  other.pid_ = -1;
  other.done_ = true;
}

Process& Process::operator=(Process&& other) noexcept {
  if (this != &other) {
    if (!done_ && pid_ > 0) {
      waitpid(pid_, nullptr, 0);
    }
    pid_ = other.pid_;
    done_ = other.done_;
    other.pid_ = -1;
    other.done_ = true;
  }
  return *this;
}
