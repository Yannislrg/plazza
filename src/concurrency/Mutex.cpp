/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Mutex
*/

#include "concurrency/Mutex.hpp"
#include <pthread.h>
#include <cerrno>
#include <string>
#include "exceptions/Exception.hpp"
#include "utils/ErrorMessage.hpp"

Mutex::Mutex() {
  if (pthread_mutex_init(&mutex_, nullptr) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMutexInitFailed));
  }
}

Mutex::~Mutex() { pthread_mutex_destroy(&mutex_); }

void Mutex::lock() {
  if (pthread_mutex_lock(&mutex_) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMutexLockFailed));
  }
}

void Mutex::unlock() {
  if (pthread_mutex_unlock(&mutex_) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMutexUnlockFailed));
  }
}

bool Mutex::trylock() {
  const int result = pthread_mutex_trylock(&mutex_);
  if (result == 0) {
    return true;
  }
  if (result == EBUSY) {
    return false;
  }
  throw plazza::exceptions::Exception(
      std::string(plazza::constants::kMutexTrylockFailed));
}

pthread_mutex_t*
Mutex::nativeHandle() noexcept {  // NOLINT(misc-include-cleaner)
  return &mutex_;
}
