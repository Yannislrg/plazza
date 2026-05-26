/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** KitchenWorker
*/

#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include "ipc/MessageQueue.hpp"
#include "kitchen/IngredientStock/IngredientStock.hpp"
#include "kitchen/threadPool/ThreadPool.hpp"

namespace kitchen {

class KitchenWorker {
 public:
  KitchenWorker(std::size_t nCooks, std::size_t regenDelayMs,
                MessageQueue& orderQueue, MessageQueue& resultQueue);
  ~KitchenWorker() = default;

  KitchenWorker(const KitchenWorker&) = delete;
  KitchenWorker& operator=(const KitchenWorker&) = delete;
  KitchenWorker(KitchenWorker&&) = delete;
  KitchenWorker& operator=(KitchenWorker&&) = delete;

  void run();

 private:
  [[nodiscard]] bool isIdle() const noexcept;

  IngredientStock stock_;
  std::unique_ptr<ThreadPool> pool_;
  MessageQueue& orderQueue_;
  MessageQueue& resultQueue_;

  static constexpr std::chrono::seconds kIdleTimeout{5};
};

}  // namespace kitchen
