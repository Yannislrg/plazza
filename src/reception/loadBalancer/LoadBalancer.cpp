/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** LoadBalancer
*/

#include "LoadBalancer.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "Pizza.hpp"
#include "concurrency/Process.hpp"
#include "ipc/MessageQueue.hpp"
#include "kitchen/KitchenWorker.hpp"
#include "pizza/factory/PizzaFactory.hpp"
#include "reception/kitchenHandle/KitchenHandle.hpp"

namespace {
constexpr std::chrono::milliseconds kListenerPollDelay{50};
}  // namespace

LoadBalancer::LoadBalancer(PizzaFactory& factory, std::size_t nCooks,
                           std::size_t regenMs)
    : _factory(factory)
    , _nCooks(nCooks)
    , _regenMs(regenMs)
    , _nextId(1)
    , _running(true)
    , _listenerThread([this]() { this->listenLoop(); }) {}

LoadBalancer::~LoadBalancer() { _running = false; }

void LoadBalancer::dispatch(const std::vector<PizzaOrder>& orders) {
  for (const auto& order : orders) {
    auto recipe = PizzaFactory::create(order.type, order.size);
    const std::lock_guard lock(_mutex);
    auto* kitchen = selectKitchen();
    if (kitchen == nullptr) {
      kitchen = &spawnKitchen();
    }
    sendPizza(*kitchen, recipe);
  }
}

std::vector<KitchenStatus> LoadBalancer::getStatus() {
  const std::lock_guard lock(_mutex);
  for (auto& kitchen : _kitchens) {
    if (kitchen.alive) {
      plazza::Packet request{.type = plazza::MessageType::StatusRequest};
      kitchen.orderQueue->send(request);
    }
  }
  return _kitchenStatuses;
}

KitchenHandle* LoadBalancer::selectKitchen() {
  KitchenHandle* selected = nullptr;
  std::size_t minLoad = std::numeric_limits<std::size_t>::max();
  for (auto& kitchen : _kitchens) {
    if (kitchen.alive && kitchen.load < kitchen.capacity &&
        kitchen.load < minLoad) {
      minLoad = kitchen.load;
      selected = &kitchen;
    }
  }
  return selected;
}

KitchenHandle& LoadBalancer::spawnKitchen() {
  const int kitchenId = _nextId++;
  auto orderQueue = std::make_unique<MessageQueue>(
      "order_queue_" + std::to_string(kitchenId), MessageQueue::Mode::Create);
  auto resultQueue = std::make_unique<MessageQueue>(
      "result_queue_" + std::to_string(kitchenId), MessageQueue::Mode::Create);

  auto process = std::make_unique<Process>(
      [kitchenId, nCooks = _nCooks, regenMs = _regenMs]() {
        MessageQueue orders("order_queue_" + std::to_string(kitchenId),
                            MessageQueue::Mode::Open);
        MessageQueue results("result_queue_" + std::to_string(kitchenId),
                             MessageQueue::Mode::Open);
        kitchen::KitchenWorker worker(nCooks, regenMs, orders, results);
        worker.run();
      });

  _kitchens.push_back(KitchenHandle{
      .id = kitchenId,
      .pid = process->pid(),
      .process = std::move(process),
      .orderQueue = std::move(orderQueue),
      .resultQueue = std::move(resultQueue),
      .load = 0,
      .capacity = 2 * _nCooks,
      .alive = true,
  });
  _kitchenStatuses.push_back(KitchenStatus{.id = kitchenId,
                                           .load = 0,
                                           .capacity = _nCooks,
                                           .cooks = {},
                                           .stock = {}});
  return _kitchens.back();
}

void LoadBalancer::sendPizza(KitchenHandle& kitchen, const PizzaRecipe& pizza) {
  plazza::Packet pkt{.type = plazza::MessageType::Pizza,
                     .pizzaType = static_cast<uint8_t>(pizza.type()),
                     .pizzaSize = static_cast<uint8_t>(pizza.size())};
  kitchen.orderQueue->send(pkt);
  kitchen.load++;
}

void LoadBalancer::listenLoop() {
  while (_running) {
    {
      const std::lock_guard lock(_mutex);
      for (auto& kitchen : _kitchens) {
        if (kitchen.alive) {
          processKitchenPacket(kitchen);
        }
      }
    }
    std::this_thread::sleep_for(kListenerPollDelay);
  }
}

void LoadBalancer::processKitchenPacket(KitchenHandle& kitchen) {
  plazza::Packet response{};
  if (kitchen.resultQueue->receive(response)) {
    if (response.type == plazza::MessageType::Done) {
      if (kitchen.load > 0) {
        kitchen.load--;
      }
    } else if (response.type == plazza::MessageType::StatusReply) {
      updateKitchenStatus(kitchen.id, response);
    } else if (response.type == plazza::MessageType::Shutdown) {
      kitchen.alive = false;
    }
  }
}

void LoadBalancer::updateKitchenStatus(int kitchenId,
                                       const plazza::Packet& response) {
  auto it = std::find_if(_kitchenStatuses.begin(), _kitchenStatuses.end(),
                         [kitchenId](const auto& kitchenStatus) {
                           return kitchenStatus.id == kitchenId;
                         });
  if (it == _kitchenStatuses.end()) {
    return;
  }
  it->load = response.load;
  it->capacity = response.capacity;
  it->stock.clear();
  for (size_t i = 0; i < 9; ++i) {
    it->stock[static_cast<kitchen::Ingredient>(i)] =
        response.stock.quantities[i];
  }
  it->cooks.clear();
  for (size_t i = 0; i < response.nCooks; ++i) {
    it->cooks.push_back(
        {.id = response.cooks[i].id,
         .state = static_cast<kitchen::CookState>(response.cooks[i].state),
         .currentPizza = ""});
  }
}

void LoadBalancer::removeKitchen(int kitchenId) {
  const std::lock_guard lock(_mutex);
  const auto [first, last] = std::ranges::remove_if(
      _kitchens,
      [kitchenId](const auto& kitchen) { return kitchen.id == kitchenId; });
  _kitchens.erase(first, last);
}
