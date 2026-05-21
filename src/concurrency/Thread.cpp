/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Thread
*/

#include "concurrency/Thread.hpp"
#include <pthread.h>
#include <cstdlib>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include "exceptions/Exception.hpp"
#include "utils/Constant.hpp"

Thread::Thread(std::function<void()> routine) {
  auto* const callbackArg =  // NOLINT(cppcoreguidelines-owning-memory)
      new std::function<void()>(std::move(routine));
  if (pthread_create(&_thread, nullptr, &Thread::entryPoint, callbackArg) !=
      0) {
    delete callbackArg;  // NOLINT(cppcoreguidelines-owning-memory)
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kThreadCreationFailed));
  }
  _launched = true;
}

Thread::~Thread() {
  if (_launched && pthread_join(_thread, nullptr) != 0) {
    std::abort();
  }
}

Thread::Thread(Thread&& other) noexcept
    : _thread(other._thread)
    , _launched(other._launched) {
  other._launched = false;
}

Thread& Thread::operator=(Thread&& other) noexcept {
  if (this != &other) {
    if (_launched && pthread_join(_thread, nullptr) != 0) {
      std::abort();
    }
    _thread = other._thread;
    _launched = other._launched;
    other._launched = false;
  }
  return *this;
}

void* Thread::entryPoint(void* arg) {
  const std::unique_ptr<std::function<void()>>
      pendingCallback(  // NOLINT(cppcoreguidelines-owning-memory)
          static_cast<std::function<void()>*>(arg));
  try {
    (*pendingCallback)();
  } catch (...) {
    std::terminate();
  }
  return nullptr;
}
