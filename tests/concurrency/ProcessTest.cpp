/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Process unit tests
*/

#include <gtest/gtest.h>
#include "concurrency/Process.hpp"

TEST(ProcessTest, PidIsPositiveAfterFork) {
  Process p([] {});

  EXPECT_GT(p.pid(), 0);
}

TEST(ProcessTest, WaitCompletesWithoutHanging) {
  bool completed = false;
  Process p([] {});

  p.wait();
  completed = true;

  EXPECT_TRUE(completed);
}
