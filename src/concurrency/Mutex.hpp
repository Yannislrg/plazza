/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Mutex
*/

#pragma once

#include <pthread.h>

class Mutex {
 public:
  Mutex();
  ~Mutex();

  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

  void lock();
  void unlock();
  bool trylock();

  [[nodiscard]] pthread_mutex_t* nativeHandle() noexcept;

 private:
  pthread_mutex_t _mutex{};
};
