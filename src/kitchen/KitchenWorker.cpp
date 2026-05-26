/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** KitchenWorker
*/

#include "kitchen/KitchenWorker.hpp"
#include <chrono>
#include <cstddef>
#include <memory>
#include <thread>
#include <utility>
#include "Pizza.hpp"
#include "ipc/MessageQueue.hpp"
#include "kitchen/threadPool/ThreadPool.hpp"
#include "pizza/Pizza.hpp"
#include "pizza/factory/PizzaFactory.hpp"

namespace kitchen {

namespace {
constexpr std::chrono::milliseconds kPollInterval{50};
}  // namespace

KitchenWorker::KitchenWorker(std::size_t nCooks, std::size_t regenDelayMs,
                             MessageQueue& orderQueue,
                             MessageQueue& resultQueue)
    : stock_(regenDelayMs)
    , pool_(std::make_unique<ThreadPool>(nCooks))
    , orderQueue_(orderQueue)
    , resultQueue_(resultQueue) {}

void KitchenWorker::run() {
  pool_->start(stock_, resultQueue_);
  auto lastActiveTime = std::chrono::steady_clock::now();

  while (true) {
    if (!pool_->isFull()) {
      Pizza order{};
      if (orderQueue_ >> order) {
        // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
        if (order.type == static_cast<PizzaType>(0) &&
            order.size == static_cast<PizzaSize>(0)) {
          break;
        }
        // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)
        PizzaRecipe recipe = PizzaFactory::create(order.type, order.size);
        pool_->addPizza(std::move(recipe));
        lastActiveTime = std::chrono::steady_clock::now();
      }
    }

    const auto now = std::chrono::steady_clock::now();
    if (isIdle()) {
      if (now - lastActiveTime >= kIdleTimeout) {
        break;
      }
    } else {
      lastActiveTime = now;
    }

    std::this_thread::sleep_for(kPollInterval);
  }

  pool_->stop();
  pool_.reset();

  // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
  const Pizza closeSentinel{.type = static_cast<PizzaType>(0),
                            .size = static_cast<PizzaSize>(0)};
  // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)
  resultQueue_ << closeSentinel;
}

bool KitchenWorker::isIdle() const noexcept { return pool_->getLoad() == 0; }

}  // namespace kitchen
