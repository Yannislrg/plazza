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
  _pid = forkedPid;
}

Process::~Process() {
  if (!_done && _pid > 0) {
    waitpid(_pid, nullptr, 0);
  }
}

void Process::wait() {
  if (!_done && _pid > 0) {
    waitpid(_pid, nullptr, 0);
    _done = true;
  }
}

pid_t Process::pid() const noexcept { return _pid; }

Process::Process(Process&& other) noexcept
    : _pid(other._pid)
    , _done(other._done) {
  other._pid = -1;
  other._done = true;
}

Process& Process::operator=(Process&& other) noexcept {
  if (this != &other) {
    if (!_done && _pid > 0) {
      waitpid(_pid, nullptr, 0);
    }
    _pid = other._pid;
    _done = other._done;
    other._pid = -1;
    other._done = true;
  }
  return *this;
}
