/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** NotifyFixture
*/

#pragma once

#include <gtest/gtest.h>
#include <atomic>
#include "concurrency/ConditionVariable.hpp"
#include "concurrency/Mutex.hpp"

class NotifyFixture : public ::testing::Test {
 protected:
  Mutex mutex_;
  ConditionVariable conditionVariable_;
  bool ready_{false};

  void waiterRoutine(std::atomic<int>& counter) {
    mutex_.lock();
    while (!ready_) {
      conditionVariable_.wait(mutex_);
    }
    ++counter;
    mutex_.unlock();
  }
};
