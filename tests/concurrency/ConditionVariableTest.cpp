/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ConditionVariable functional tests
*/

#include <gtest/gtest.h>
#include <atomic>
#include "concurrency/Thread.hpp"
#include "fixtures/NotifyFixture.hpp"

TEST_F(NotifyFixture, NotifyOneWakesWaiter) {
  std::atomic<int> notifiedCount{0};

  {
    Thread waiter([this, &notifiedCount] { waiterRoutine(notifiedCount); });

    _mutex.lock();
    _ready = true;
    _mutex.unlock();
    _conditionVariable.notifyOne();
  }

  EXPECT_EQ(notifiedCount, 1);
}

TEST_F(NotifyFixture, NotifyAllWakesAllWaiters) {
  constexpr int waiterCount = 3;
  std::atomic<int> notifiedCount{0};

  {
    Thread w1([this, &notifiedCount] { waiterRoutine(notifiedCount); });
    Thread w2([this, &notifiedCount] { waiterRoutine(notifiedCount); });
    Thread w3([this, &notifiedCount] { waiterRoutine(notifiedCount); });

    _mutex.lock();
    _ready = true;
    _mutex.unlock();
    _conditionVariable.notifyAll();
  }

  EXPECT_EQ(notifiedCount, waiterCount);
}
