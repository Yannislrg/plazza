/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** KitchenWorker
*/

#include "kitchen/KitchenWorker.hpp"
#include <chrono>
#include <cstddef>
#include <cstring>
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
  bool running = true;

  while (running) {
    plazza::Packet packet{};
    if (orderQueue_.receive(packet)) {
      if (packet.type == plazza::MessageType::Shutdown) {
        running = false;
      } else {
        handlePacket(packet, lastActiveTime);
      }
    }

    const auto now = std::chrono::steady_clock::now();
    if (isIdle()) {
      if (now - lastActiveTime >= kIdleTimeout) {
        running = false;
      }
    } else {
      lastActiveTime = now;
    }
    std::this_thread::sleep_for(kPollInterval);
  }
  pool_->stop();
  pool_.reset();

  plazza::Packet shutdownPkt{};
  shutdownPkt.type = plazza::MessageType::Shutdown;
  resultQueue_.send(shutdownPkt);
}

void KitchenWorker::handlePacket(
    const plazza::Packet& packet,
    std::chrono::steady_clock::time_point& lastActiveTime) {
  if (packet.type == plazza::MessageType::Pizza) {
    PizzaRecipe recipe =
        PizzaFactory::create(static_cast<PizzaType>(packet.pizzaType),
                             static_cast<PizzaSize>(packet.pizzaSize));
    pool_->addPizza(std::move(recipe));
    lastActiveTime = std::chrono::steady_clock::now();
  } else if (packet.type == plazza::MessageType::StatusRequest) {
    sendStatusResponse();
  }
}

void KitchenWorker::sendStatusResponse() const {
  plazza::Packet reply{};
  reply.type = plazza::MessageType::StatusReply;
  reply.load = static_cast<uint8_t>(pool_->getLoad());
  reply.capacity = static_cast<uint8_t>(pool_->getCapacity());

  const auto currentStock = stock_.stock();
  for (std::size_t i = 0; i < 9; ++i) {
    reply.stock.quantities[i] = static_cast<uint8_t>(
        currentStock.at(IngredientStock::kAllIngredients[i].value));
  }

  const auto cookStatuses = pool_->getStatus();
  reply.nCooks = static_cast<uint8_t>(cookStatuses.size());
  for (std::size_t i = 0;
       i < std::min(cookStatuses.size(), static_cast<std::size_t>(8)); ++i) {
    reply.cooks[i].id = static_cast<uint8_t>(cookStatuses[i].id);
    reply.cooks[i].state = static_cast<uint8_t>(cookStatuses[i].state);
    reply.cooks[i].pizzaType = static_cast<uint8_t>(cookStatuses[i].type);
    reply.cooks[i].pizzaSize = static_cast<uint8_t>(cookStatuses[i].size);
  }
  resultQueue_.send(reply);
}

bool KitchenWorker::isIdle() const noexcept { return pool_->getLoad() == 0; }

}  // namespace kitchen
