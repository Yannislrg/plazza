/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** LoadBalancer
*/

#include "LoadBalancer.hpp"
#include <algorithm>
#include <string>
#include <thread>
#include <vector>
#include "concurrency/Process.hpp"
#include "ipc/MessageQueue.hpp"
#include "ipc/PizzaSerializer.hpp"
#include "kitchen/KitchenWorker.hpp"
#include "pizza/factory/PizzaFactory.hpp"
#include "reception/kitchenHandle/KitchenHandle.hpp"

namespace {

void spawnKitchen(std::vector<KitchenHandle>& kitchens,
                  std::atomic<int>& nextId, std::size_t nCooks,
                  std::size_t regenMs, double multiplier) {
  const int kitchenId = nextId++;
  const std::string orderPath = "order_queue_" + std::to_string(kitchenId);
  const std::string resultPath = "result_queue_" + std::to_string(kitchenId);

  KitchenHandle handle;
  handle.id = kitchenId;
  handle.load = 0;
  handle.capacity = 2 * nCooks;
  handle.alive = true;
  handle.status.id = kitchenId;
  handle.status.load = 0;
  handle.status.capacity = handle.capacity;
  handle.orderQueue =
      std::make_unique<MessageQueue>(orderPath, MessageQueue::Mode::Create);
  handle.resultQueue =
      std::make_unique<MessageQueue>(resultPath, MessageQueue::Mode::Create);
  handle.process = std::make_unique<Process>(
      [orderPath, resultPath, nCooks, regenMs, multiplier]() {
        MessageQueue orders(orderPath, MessageQueue::Mode::Open);
        MessageQueue results(resultPath, MessageQueue::Mode::Open);
        kitchen::KitchenWorker worker(nCooks, regenMs, multiplier, orders,
                                     results);
        worker.run();
      });
  kitchens.push_back(std::move(handle));
}

}  // namespace

LoadBalancer::LoadBalancer(PizzaFactory& factory, std::size_t nCooks,
                           std::size_t regenMs, double multiplier)
    : factory_(factory)
    , nCooks_(nCooks)
    , regenMs_(regenMs)
    , multiplier_(multiplier) {}

LoadBalancer::~LoadBalancer() { shutdown(); }

void LoadBalancer::setDoneCallback(
    std::function<void(int, PizzaType, PizzaSize)> callback) {
  doneCb_ = std::move(callback);
}

void LoadBalancer::dispatch(const std::vector<PizzaOrder>& orders) {
  updateKitchens();
  for (const auto& order : orders) {
    for (std::size_t i = 0; i < order.quantity; ++i) {
      KitchenHandle* best = nullptr;
      for (auto& kitchen : kitchens_) {
        if (kitchen.alive && kitchen.load < kitchen.capacity) {
          if (best == nullptr || kitchen.load < best->load) {
            best = &kitchen;
          }
        }
      }
      if (best == nullptr) {
        spawnKitchen(kitchens_, nextKitchenId_, nCooks_, regenMs_, multiplier_);
        best = &kitchens_.back();
      }
      plazza::Packet pkt{};
      pkt.type = plazza::MessageType::Pizza;
      pkt.pizza = pack({.type = order.type, .size = order.size});
      best->orderQueue->send(pkt);
      ++best->load;
    }
  }
}

std::vector<KitchenStatus> LoadBalancer::getStatus() {
  std::vector<int> pending;
  for (auto& kitchen : kitchens_) {
    if (kitchen.alive) {
      kitchen.pendingCooks.clear();
      plazza::Packet req{};
      req.type = plazza::MessageType::StatusRequest;
      kitchen.orderQueue->send(req);
      pending.push_back(kitchen.id);
    }
  }

  while (!pending.empty()) {
    for (auto it = pending.begin(); it != pending.end();) {
      auto kitchenIt = std::find_if(
          kitchens_.begin(), kitchens_.end(),
          [id = *it](const KitchenHandle& k) { return k.id == id; });
      if (kitchenIt == kitchens_.end() || !kitchenIt->alive) {
        it = pending.erase(it);
        continue;
      }
      plazza::Packet pkt{};
      bool gotReply = false;
      while (kitchenIt->resultQueue->receive(pkt)) {
        if (pkt.type == plazza::MessageType::StatusReply) {
          gotReply = true;
        }
        handlePacket(*kitchenIt, pkt);
      }
      if (gotReply || !kitchenIt->alive) {
        it = pending.erase(it);
      } else {
        ++it;
      }
    }
    if (!pending.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  std::vector<KitchenStatus> result;
  for (const auto& kitchen : kitchens_) {
    if (kitchen.alive) {
      result.push_back(kitchen.status);
    }
  }
  return result;
}

void LoadBalancer::handlePacket(KitchenHandle& kitchen,
                                const plazza::Packet& pkt) {
  switch (pkt.type) {
    case plazza::MessageType::Done: {
      if (kitchen.load > 0) {
        --kitchen.load;
      }
      const auto pizza = unpack(pkt.pizza);
      logger_.logPizzaDone(kitchen.id,
                           PizzaFactory::create(pizza.type, pizza.size).getName());
      if (doneCb_) {
        doneCb_(kitchen.id, pizza.type, pizza.size);
      }
      break;
    }
    case plazza::MessageType::CookStatus: {
      std::string pizzaName;
      if (static_cast<kitchen::CookState>(pkt.cook.state) ==
          kitchen::CookState::COOKING) {
        const auto pizzaType = static_cast<PizzaType>(pkt.cook.pizzaType);
        const auto pizzaSize = static_cast<PizzaSize>(pkt.cook.pizzaSize);
        pizzaName = PizzaFactory::create(pizzaType, pizzaSize).getName();
      }
      kitchen.pendingCooks.push_back(
          {.id = pkt.cook.id,
           .state = static_cast<kitchen::CookState>(pkt.cook.state),
           .currentPizza = std::move(pizzaName),
           .type = static_cast<PizzaType>(pkt.cook.pizzaType),
           .size = static_cast<PizzaSize>(pkt.cook.pizzaSize)});
      break;
    }
    case plazza::MessageType::StatusReply:
      kitchen.status.load = pkt.load;
      kitchen.status.capacity = pkt.capacity;
      kitchen.status.cooks = std::move(kitchen.pendingCooks);
      kitchen.pendingCooks.clear();
      for (std::size_t i = 0; i < 9; ++i) {
        kitchen.status.stock[static_cast<kitchen::Ingredient>(i)] =
            pkt.stock.quantities[i];
      }
      break;
    case plazza::MessageType::Shutdown:
      kitchen.alive = false;
      kitchen.process->wait();
      break;
    default:
      break;
  }
}

void LoadBalancer::updateKitchens() {
  for (auto& kitchen : kitchens_) {
    if (!kitchen.alive) {
      continue;
    }
    plazza::Packet pkt{};
    while (kitchen.resultQueue->receive(pkt)) {
      handlePacket(kitchen, pkt);
    }
  }
  kitchens_.erase(
      std::remove_if(kitchens_.begin(), kitchens_.end(),
                     [](const KitchenHandle& k) { return !k.alive; }),
      kitchens_.end());
}

void LoadBalancer::shutdown() {
  for (auto& kitchen : kitchens_) {
    if (kitchen.alive) {
      plazza::Packet pkt{};
      pkt.type = plazza::MessageType::Shutdown;
      kitchen.orderQueue->send(pkt);
      kitchen.process->wait();
      kitchen.alive = false;
    }
  }
}
