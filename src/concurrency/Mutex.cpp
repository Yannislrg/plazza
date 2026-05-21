/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Mutex
*/

#include "concurrency/Mutex.hpp"
#include "exceptions/Exception.hpp"
#include "utils/Constant.hpp"

Mutex::Mutex() {
  if (pthread_mutex_init(&_mutex, nullptr) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::mutexInitFailed));
  }
}

Mutex::~Mutex() { pthread_mutex_destroy(&_mutex); }

void Mutex::lock() {
  if (pthread_mutex_lock(&_mutex) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::mutexLockFailed));
  }
}

void Mutex::unlock() {
  if (pthread_mutex_unlock(&_mutex) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::mutexUnlockFailed));
  }
}

bool Mutex::trylock() { return pthread_mutex_trylock(&_mutex) == 0; }

pthread_mutex_t* Mutex::nativeHandle() noexcept { return &_mutex; }
