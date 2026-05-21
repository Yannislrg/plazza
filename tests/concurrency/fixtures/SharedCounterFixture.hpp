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

  Mutex _mutex;
  int _counter{0};

  void incrementRoutine() {
    for (int i = 0; i < kIterationsPerThread; ++i) {
      _mutex.lock();
      ++_counter;
      _mutex.unlock();
    }
  }
};
