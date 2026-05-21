/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Process unit tests
*/

#include <gtest/gtest.h>
#include <unistd.h>
#include <array>
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

TEST(ProcessTest, RoutineExecutesInChild) {
  std::array<int, 2> pipefd{};
  ASSERT_EQ(pipe(pipefd.data()), 0);

  {
    Process p([pipefd] {
      close(pipefd[0]);
      const char sentinel = 42;
      write(pipefd[1], &sentinel, sizeof(sentinel));
      close(pipefd[1]);
    });
    p.wait();
  }

  close(pipefd[1]);
  char received = 0;
  const ssize_t bytesRead = read(pipefd[0], &received, sizeof(received));
  close(pipefd[0]);

  EXPECT_EQ(bytesRead, 1);
  EXPECT_EQ(received, 42);
}

TEST(ProcessTest, MoveConstructorTransfersOwnership) {
  std::array<int, 2> pipefd{};
  ASSERT_EQ(pipe(pipefd.data()), 0);

  {
    Process first([pipefd] {
      close(pipefd[0]);
      const char sentinel = 7;
      write(pipefd[1], &sentinel, sizeof(sentinel));
      close(pipefd[1]);
    });
    Process second(std::move(first));
    second.wait();
  }

  close(pipefd[1]);
  char received = 0;
  const ssize_t bytesRead = read(pipefd[0], &received, sizeof(received));
  close(pipefd[0]);

  EXPECT_EQ(bytesRead, 1);
  EXPECT_EQ(received, 7);
}
