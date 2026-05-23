/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ThreadPool
*/

#pragma once

#include <atomic>
#include <cstddef>
#include <deque>
#include <queue>
#include <vector>
#include "concurrency/ConditionVariable.hpp"
#include "concurrency/Mutex.hpp"
#include "concurrency/Thread.hpp"
#include "ipc/MessageQueue.hpp"
#include "kitchen/IngredientStock/IngredientStock.hpp"
#include "kitchen/cook/Cook.hpp"
#include "pizza/Pizza.hpp"

namespace kitchen {
class ThreadPool {
 public:
  ~ThreadPool() = default;

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  explicit ThreadPool(std::size_t nCooks);

  bool addPizza(PizzaRecipe pizza);
  bool isFull() const;
  std::size_t getLoad() const;
  std::vector<CookStatus> getStatus() const;
  void start(IngredientStock& stock, MessageQueue& ipc);
  void stop();

 private:
  std::size_t _nCooks;
  std::size_t _maxCapacity;
  std::deque<Cook> _cooks;
  std::vector<Thread> _threads;
  std::queue<PizzaRecipe> _queue;
  Mutex _queueMutex;
  ConditionVariable _conditionVariable;
  bool _running;
  std::atomic<std::size_t> _load;
  IngredientStock* _stock;
  MessageQueue* _ipc;

  void workerLoop(Cook& cook);
};
}  // namespace kitchen
