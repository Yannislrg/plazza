/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Thread
*/

#include "concurrency/Thread.hpp"
#include "exceptions/Exception.hpp"
#include "utils/Constant.hpp"

Thread::Thread(std::function<void()> routine) {
  auto* fn = new std::function<void()>(std::move(routine));
  if (pthread_create(&_thread, nullptr, &Thread::entryPoint, fn) != 0) {
    delete fn;
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::threadCreationFailed));
  }
  _launched = true;
}

Thread::~Thread() {
  if (_launched) {
    pthread_join(_thread, nullptr);
  }
}

Thread::Thread(Thread&& other) noexcept
    : _thread(other._thread)
    , _launched(other._launched) {
  other._launched = false;
}

Thread& Thread::operator=(Thread&& other) noexcept {
  if (this != &other) {
    if (_launched) {
      pthread_join(_thread, nullptr);
    }
    _thread = other._thread;
    _launched = other._launched;
    other._launched = false;
  }
  return *this;
}

void* Thread::entryPoint(void* arg) {
  auto* fn = static_cast<std::function<void()>*>(arg);
  (*fn)();
  delete fn;
  return nullptr;
}
