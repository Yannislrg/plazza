/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Thread
*/

#pragma once

#include <pthread.h>
#include <functional>

class Thread {
 public:
  explicit Thread(std::function<void()> routine);
  ~Thread();

  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;
  Thread(Thread&&) noexcept;
  Thread& operator=(Thread&&) noexcept;

 private:
  static void* entryPoint(void* arg);

  pthread_t _thread{};
  bool _launched{false};
};
