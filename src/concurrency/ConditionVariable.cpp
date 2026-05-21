/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ConditionVariable
*/

#include "concurrency/ConditionVariable.hpp"
#include <pthread.h>
#include <string>
#include "concurrency/Mutex.hpp"
#include "exceptions/Exception.hpp"
#include "utils/Constant.hpp"

ConditionVariable::ConditionVariable() {
  if (pthread_cond_init(&_cond, nullptr) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarInitFailed));
  }
}

ConditionVariable::~ConditionVariable() { pthread_cond_destroy(&_cond); }

void ConditionVariable::wait(Mutex& mutex) {
  if (pthread_cond_wait(&_cond, mutex.nativeHandle()) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarWaitFailed));
  }
}

void ConditionVariable::notifyOne() {
  if (pthread_cond_signal(&_cond) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarSignalFailed));
  }
}

void ConditionVariable::notifyAll() {
  if (pthread_cond_broadcast(&_cond) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarBroadcastFailed));
  }
}
