/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Mutex unit tests
*/

#include <gtest/gtest.h>
#include "concurrency/Mutex.hpp"

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
