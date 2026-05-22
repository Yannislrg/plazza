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

    mutex_.lock();
    ready_ = true;
    mutex_.unlock();
    conditionVariable_.notifyOne();
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

    mutex_.lock();
    ready_ = true;
    mutex_.unlock();
    conditionVariable_.notifyAll();
  }

  EXPECT_EQ(notifiedCount, waiterCount);
}
