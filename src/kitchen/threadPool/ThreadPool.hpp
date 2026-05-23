/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ThreadPool
*/

#pragma once

namespace kitchen {
class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

 private:
};
}  // namespace kitchen
