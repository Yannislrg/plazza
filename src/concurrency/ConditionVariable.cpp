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
#include "utils/ErrorMessage.hpp"

ConditionVariable::ConditionVariable() {
  if (pthread_cond_init(&cond_, nullptr) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarInitFailed));
  }
}

ConditionVariable::~ConditionVariable() { pthread_cond_destroy(&cond_); }

void ConditionVariable::wait(Mutex& mutex) {
  if (pthread_cond_wait(&cond_, mutex.nativeHandle()) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarWaitFailed));
  }
}

void ConditionVariable::notifyOne() {
  if (pthread_cond_signal(&cond_) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarSignalFailed));
  }
}

void ConditionVariable::notifyAll() {
  if (pthread_cond_broadcast(&cond_) != 0) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kCondVarBroadcastFailed));
  }
}
