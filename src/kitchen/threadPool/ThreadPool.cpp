/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** ThreadPool
*/

#include "ThreadPool.hpp"
#include <stdexcept>

namespace kitchen {
ThreadPool::ThreadPool(std::size_t nCooks, double multiplier)
    : _maxCapacity(2 * nCooks)
    , _running(false)
    , _load(0)
    , _stock(nullptr)
    , _ipc(nullptr) {
  if (multiplier < 0.0 || multiplier > 1.0) {
    throw std::invalid_argument("multiplier must be in [0,1]");
  }
  for (std::size_t i = 0; i < nCooks; ++i) {
    _cooks.emplace_back(i, multiplier);
  }
}

bool ThreadPool::addPizza(PizzaRecipe pizza) {
  std::lock_guard lock(_queueMutex);
  if (isFull()) {
    return false;
  }
  _queue.push(std::move(pizza));
  ++_load;
  _conditionVariable.notifyOne();
  return true;
}

bool ThreadPool::isFull() const { return _load >= _maxCapacity; }

std::size_t ThreadPool::getLoad() const { return _load.load(); }

std::size_t ThreadPool::getCapacity() const noexcept { return _maxCapacity; }

std::vector<CookStatus> ThreadPool::getStatus() const {
  std::vector<CookStatus> statuses;
  statuses.reserve(_cooks.size());
  for (const auto& cook : _cooks) {
    statuses.push_back(cook.status());
  }
  return statuses;
}

void ThreadPool::start(IngredientStock& stock, MessageQueue& ipc) {
  _stock = &stock;
  _ipc = &ipc;
  _running = true;
  for (auto& cook : _cooks) {
    Cook* cookPtr = &cook;
    _threads.emplace_back([this, cookPtr] { workerLoop(*cookPtr); });
  }
}

void ThreadPool::stop() {
  {
    std::lock_guard lock(_queueMutex);
    _running = false;
  }
  _conditionVariable.notifyAll();
}

void ThreadPool::workerLoop(Cook& cook) {
  while (true) {
    PizzaRecipe pizza;
    {
      _queueMutex.lock();
      while (_queue.empty() && _running) {
        _conditionVariable.wait(_queueMutex);
      }
      if (!_running && _queue.empty()) {
        _queueMutex.unlock();
        break;
      }
      pizza = std::move(_queue.front());
      _queue.pop();
      _queueMutex.unlock();
    }
    if (_stock != nullptr && _ipc != nullptr) {
      cook.cookPizza(pizza, *_stock, *_ipc);
    }
    --_load;
  }
}

}  // namespace kitchen
