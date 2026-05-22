/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** SharedCounterFixture
*/

#pragma once

#include <gtest/gtest.h>
#include "concurrency/Mutex.hpp"

class SharedCounterFixture : public ::testing::Test {
 protected:
  static constexpr int kIterationsPerThread = 1000;

  Mutex mutex_;
  int counter_{0};

  void incrementRoutine() {
    for (int i = 0; i < kIterationsPerThread; ++i) {
      mutex_.lock();
      ++counter_;
      mutex_.unlock();
    }
  }
};
