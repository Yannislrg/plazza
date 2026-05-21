/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ConditionVariable
*/

#include "concurrency/ConditionVariable.hpp"
#include "exceptions/Exception.hpp"
#include "utils/Constant.hpp"

ConditionVariable::ConditionVariable() {
  if (pthread_cond_init(&_cond, nullptr) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::condVarInitFailed));
  }
}

ConditionVariable::~ConditionVariable() { pthread_cond_destroy(&_cond); }

void ConditionVariable::wait(Mutex& mutex) {
  if (pthread_cond_wait(&_cond, mutex.nativeHandle()) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::condVarWaitFailed));
  }
}

void ConditionVariable::notifyOne() {
  if (pthread_cond_signal(&_cond) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::condVarSignalFailed));
  }
}

void ConditionVariable::notifyAll() {
  if (pthread_cond_broadcast(&_cond) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::condVarBroadcastFailed));
  }
}
