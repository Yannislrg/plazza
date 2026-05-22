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
  if (pthread_create(&thread_, nullptr, &Thread::entryPoint, callbackArg) !=
      0) {
    delete callbackArg;  // NOLINT(cppcoreguidelines-owning-memory)
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kThreadCreationFailed));
  }
  launched_ = true;
}

Thread::~Thread() {
  if (launched_ && pthread_join(thread_, nullptr) != 0) {
    std::abort();
  }
}

Thread::Thread(Thread&& other) noexcept
    : thread_(other.thread_)
    , launched_(other.launched_) {
  other.launched_ = false;
}

Thread& Thread::operator=(Thread&& other) noexcept {
  if (this != &other) {
    if (launched_ && pthread_join(thread_, nullptr) != 0) {
      std::abort();
    }
    thread_ = other.thread_;
    launched_ = other.launched_;
    other.launched_ = false;
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
