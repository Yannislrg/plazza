/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Mutex unit tests
*/

#include <gtest/gtest.h>
#include <atomic>
#include "concurrency/Mutex.hpp"
#include "concurrency/Thread.hpp"

TEST(MutexTest, LockAndUnlockDoNotThrow) {
  Mutex mutex;

  EXPECT_NO_THROW(mutex.lock());
  EXPECT_NO_THROW(mutex.unlock());
}

TEST(MutexTest, TrylockSucceedsWhenFree) {
  Mutex mutex;

  EXPECT_TRUE(mutex.trylock());
  mutex.unlock();
}

TEST(MutexTest, TrylockFailsWhenLockedByAnotherThread) {
  Mutex mutex;
  std::atomic<bool> trylockResult{true};

  mutex.lock();
  {
    Thread t([&mutex, &trylockResult] { trylockResult = mutex.trylock(); });
  }

  EXPECT_FALSE(trylockResult);
  mutex.unlock();
}

TEST(MutexTest, ProtectsSharedCounter) {
  Mutex mutex;
  int counter = 0;
  constexpr int iterationsPerThread = 1000;

  auto increment = [&mutex, &counter]() {
    for (int i = 0; i < iterationsPerThread; ++i) {
      mutex.lock();
      ++counter;
      mutex.unlock();
    }
  };

  {
    Thread t1(increment);
    Thread t2(increment);
    Thread t3(increment);
    Thread t4(increment);
  }

  EXPECT_EQ(counter, 4 * iterationsPerThread);
}
