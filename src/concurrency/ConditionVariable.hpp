/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ConditionVariable
*/

#pragma once

#include <pthread.h>
#include "concurrency/Mutex.hpp"

class ConditionVariable {
 public:
  ConditionVariable();
  ~ConditionVariable();

  ConditionVariable(const ConditionVariable&) = delete;
  ConditionVariable& operator=(const ConditionVariable&) = delete;
  ConditionVariable(ConditionVariable&&) = delete;
  ConditionVariable& operator=(ConditionVariable&&) = delete;

  void wait(Mutex& mutex);
  void notifyOne();
  void notifyAll();

 private:
  pthread_cond_t _cond{};
};
