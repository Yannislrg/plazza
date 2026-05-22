/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Thread unit tests
*/

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>
#include "concurrency/Thread.hpp"

TEST(ThreadTest, RoutineExecutes) {
  std::atomic<bool> ran{false};

  {
    Thread t([&ran] { ran = true; });
  }

  EXPECT_TRUE(ran);
}

TEST(ThreadTest, DestructorJoinsBeforeReturning) {
  std::atomic<bool> done{false};

  {
    Thread t([&done] {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      done = true;
    });
  }

  EXPECT_TRUE(done);
}

TEST(ThreadTest, MoveConstructorPreservesRoutine) {
  std::atomic<bool> ran{false};

  {
    Thread first([&ran] { ran = true; });
    Thread second(std::move(first));
  }

  EXPECT_TRUE(ran);
}

TEST(ThreadTest, MoveAssignmentPreservesRoutine) {
  std::atomic<bool> firstRan{false};
  std::atomic<bool> secondRan{false};

  {
    Thread first([&firstRan] { firstRan = true; });
    Thread second([&secondRan] { secondRan = true; });
    second = std::move(first);
  }

  EXPECT_TRUE(firstRan);
}
