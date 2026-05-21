/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Mutex functional tests
*/

#include <gtest/gtest.h>
#include <atomic>
#include "concurrency/Mutex.hpp"
#include "concurrency/Thread.hpp"
#include "fixtures/SharedCounterFixture.hpp"

TEST(MutexConcurrencyTest, TrylockFailsWhenLockedByAnotherThread) {
  Mutex mutex;
  std::atomic<bool> trylockResult{true};

  mutex.lock();
  {
    Thread t([&mutex, &trylockResult] { trylockResult = mutex.trylock(); });
  }

  EXPECT_FALSE(trylockResult);
  mutex.unlock();
}

TEST_F(SharedCounterFixture, ProtectsSharedCounter) {
  {
    Thread t1([this] { incrementRoutine(); });
    Thread t2([this] { incrementRoutine(); });
    Thread t3([this] { incrementRoutine(); });
    Thread t4([this] { incrementRoutine(); });
  }

  EXPECT_EQ(_counter, 4 * kIterationsPerThread);
}
