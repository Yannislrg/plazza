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

  KitchenHandle NewKitchenHandle;
  NewKitchenHandle.id = kitchenId;
  NewKitchenHandle.load = 0;
  NewKitchenHandle.capacity = 2 * nCooks;
  NewKitchenHandle.alive = true;
  NewKitchenHandle.status.id = kitchenId;
  NewKitchenHandle.status.load = 0;
  NewKitchenHandle.status.capacity = NewKitchenHandle.capacity;
  NewKitchenHandle.orderQueue =
      std::make_unique<MessageQueue>(orderPath, MessageQueue::Mode::Create);
  NewKitchenHandle.resultQueue =
      std::make_unique<MessageQueue>(resultPath, MessageQueue::Mode::Create);
  NewKitchenHandle.process = std::make_unique<Process>(
      [orderPath, resultPath, nCooks, regenMs, multiplier]() {
        MessageQueue orders(orderPath, MessageQueue::Mode::Open);
        MessageQueue results(resultPath, MessageQueue::Mode::Open);
        kitchen::KitchenWorker worker(nCooks, regenMs, multiplier, orders,
                                      results);
        worker.run();
      });
  kitchens.push_back(std::move(NewKitchenHandle));
}

using HandlePacketFn =
    std::function<void(KitchenHandle&, const plazza::Packet&)>;

void requestStatusForAll(std::vector<KitchenHandle>& kitchens,
                         std::vector<int>& pendingIDs) {
  for (auto& kitchen : kitchens) {
    if (kitchen.alive) {
      kitchen.pendingCooks.clear();
      plazza::Packet request{};
      request.type = plazza::MessageType::StatusRequest;
      kitchen.orderQueue->send(request);
      pendingIDs.push_back(kitchen.id);
    }
  }
}

bool processPendingKitchen(std::vector<KitchenHandle>& kitchens, int id,
                           HandlePacketFn handlePacket) {
  auto kitchenIt =
      std::find_if(kitchens.begin(), kitchens.end(),
                   [id](const KitchenHandle& kh) { return kh.id == id; });
  if (kitchenIt == kitchens.end() || !kitchenIt->alive) {
    return true;
  }
  plazza::Packet pkt{};
  bool gotReply = false;
  while (kitchenIt->resultQueue->receive(pkt)) {
    if (pkt.type == plazza::MessageType::StatusReply) {
      gotReply = true;
    }
    handlePacket(*kitchenIt, pkt);
  }
  return gotReply || !kitchenIt->alive;
}

void handleDonePacket(KitchenHandle& kitchen, const plazza::Packet& packet,
                      PizzaFactory& factory, Logger& logger,
                      std::function<void(int, PizzaType, PizzaSize)>& doneCb) {
  if (kitchen.load > 0) {
    --kitchen.load;
  }
  const auto pizza = unpack(packet.pizza);
  logger.logPizzaDone(kitchen.id,
                      factory.create(pizza.type, pizza.size).getName());
  if (doneCb) {
    doneCb(kitchen.id, pizza.type, pizza.size);
  }
}

void handleCookStatusPacket(KitchenHandle& kitchen,
                            const plazza::Packet& packet,
                            PizzaFactory& factory) {
  std::string pizzaName;
  if (static_cast<kitchen::CookState>(packet.cook.state) ==
      kitchen::CookState::COOKING) {
    const auto pizzaType = static_cast<PizzaType>(packet.cook.pizzaType);
    const auto pizzaSize = static_cast<PizzaSize>(packet.cook.pizzaSize);
    pizzaName = factory.create(pizzaType, pizzaSize).getName();
  }
  kitchen.pendingCooks.push_back(
      {.id = packet.cook.id,
       .state = static_cast<kitchen::CookState>(packet.cook.state),
       .currentPizza = std::move(pizzaName),
       .type = static_cast<PizzaType>(packet.cook.pizzaType),
       .size = static_cast<PizzaSize>(packet.cook.pizzaSize)});
}

void handleStatusReplyPacket(KitchenHandle& kitchen,
                             const plazza::Packet& packet) {
  kitchen.status.load = packet.load;
  kitchen.status.capacity = packet.capacity;
  kitchen.status.cooks = std::move(kitchen.pendingCooks);
  kitchen.pendingCooks.clear();
  for (std::size_t i = 0; i < 9; ++i) {
    kitchen.status.stock[static_cast<kitchen::Ingredient>(i)] =
        packet.stock.quantities[i];
  }
}

void handleShutdownPacket(KitchenHandle& kitchen) {
  kitchen.alive = false;
  kitchen.process->wait();
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
      plazza::Packet PizzaPacket{};
      PizzaPacket.type = plazza::MessageType::Pizza;
      PizzaPacket.pizza = pack({.type = order.type, .size = order.size});
      best->orderQueue->send(PizzaPacket);
      ++best->load;
    }
  }
}

std::vector<KitchenStatus> LoadBalancer::getStatus() {
  std::vector<int> pendingIDs;
  requestStatusForAll(kitchens_, pendingIDs);

  while (!pendingIDs.empty()) {
    for (auto it = pendingIDs.begin(); it != pendingIDs.end();) {
      const int id = *it;
      const bool remove = processPendingKitchen(
          kitchens_, id, [this](KitchenHandle& kh, const plazza::Packet& pkt) {
            handlePacket(kh, pkt);
          });
      if (remove) {
        it = pendingIDs.erase(it);
      } else {
        ++it;
      }
    }
    if (!pendingIDs.empty()) {
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
                                const plazza::Packet& packet) {
  switch (packet.type) {
    case plazza::MessageType::Done:
      handleDonePacket(kitchen, packet, factory_, logger_, doneCb_);
      break;
    case plazza::MessageType::CookStatus:
      handleCookStatusPacket(kitchen, packet, factory_);
      break;
    case plazza::MessageType::StatusReply:
      handleStatusReplyPacket(kitchen, packet);
      break;
    case plazza::MessageType::Shutdown:
      handleShutdownPacket(kitchen);
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
    plazza::Packet packet{};
    while (kitchen.resultQueue->receive(packet)) {
      handlePacket(kitchen, packet);
    }
  }
  kitchens_.erase(std::remove_if(kitchens_.begin(), kitchens_.end(),
                                 [](const KitchenHandle& kitchenHandle) {
                                   return !kitchenHandle.alive;
                                 }),
                  kitchens_.end());
}

void LoadBalancer::shutdown() {
  for (auto& kitchen : kitchens_) {
    if (kitchen.alive) {
      plazza::Packet packet{};
      packet.type = plazza::MessageType::Shutdown;
      kitchen.orderQueue->send(packet);
      kitchen.process->wait();
      kitchen.alive = false;
    }
  }
}
