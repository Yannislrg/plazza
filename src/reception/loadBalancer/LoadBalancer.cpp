/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** LoadBalancer
*/

#include "LoadBalancer.hpp"
#include <thread>
#include "ipc/MessageQueue.hpp"
#include "kitchen/KitchenWorker.hpp"

namespace {
void createNewKitchen(std::vector<KitchenHandle>& kitchens,
                      std::atomic<int>& nextKitchenId, std::size_t nCooks,
                      std::size_t regenMs, double multiplier) {
  int id = nextKitchenId++;
  std::string orderPath = "/tmp/orderQueue" + std::to_string(id);
  std::string resultPath = "/tmp/resultQueue" + std::to_string(id);

  KitchenHandle newKitchen;
  newKitchen.id = id;
  newKitchen.orderQueue = std::make_unique<MessageQueue>(orderPath, MessageQueue::Mode::Create);
  newKitchen.resultQueue = std::make_unique<MessageQueue>(resultPath, MessageQueue::Mode::Create);
  
  newKitchen.process = std::make_unique<Process>([orderPath, resultPath, nCooks, regenMs, multiplier] {
    MessageQueue orders(orderPath, MessageQueue::Mode::Open);
    MessageQueue results(resultPath, MessageQueue::Mode::Open);
    kitchen::KitchenWorker worker(nCooks, regenMs, orders, results, multiplier);
    worker.run();
  });

  newKitchen.load = 0;
  newKitchen.capacity = 2 * nCooks;
  newKitchen.alive = true;
  newKitchen.status.id = newKitchen.id;
  newKitchen.status.load = 0;
  newKitchen.status.capacity = newKitchen.capacity;
  kitchens.push_back(std::move(newKitchen));
}
}  // namespace

LoadBalancer::LoadBalancer(PizzaFactory& factory, std::size_t nCooks,
                           std::size_t regenMs, double multiplier)
    : factory_(factory)
    , nextKitchenId_(0)
    , nCooks_(nCooks)
    , regenMs_(regenMs)
    , multiplier_(multiplier) {
  if (multiplier_ < 0.0 || multiplier_ > 1.0) {
    throw std::invalid_argument("multiplier must be in [0, 1]");
  }
}

LoadBalancer::~LoadBalancer() { shutdown(); }

void LoadBalancer::shutdown() {
  for (auto& kitchen : kitchens_) {
    if (kitchen.alive) {
      plazza::Packet shutdownPacket{};
      shutdownPacket.type = plazza::MessageType::Shutdown;
      kitchen.orderQueue->send(shutdownPacket);
      kitchen.process->wait();
      kitchen.alive = false;
    }
  }
}

std::vector<KitchenStatus> LoadBalancer::getStatus() {
  std::vector<int> pendingIds;
  for (auto& kitchen : kitchens_) {
    if (kitchen.alive) {
      plazza::Packet request{};
      request.type = plazza::MessageType::StatusRequest;
      kitchen.orderQueue->send(request);
      pendingIds.push_back(kitchen.id);
    }
  }

  while (!pendingIds.empty()) {
    for (auto it = pendingIds.begin(); it != pendingIds.end();) {
      auto kitchenIt = std::find_if(
          kitchens_.begin(), kitchens_.end(),
          [id = *it](const KitchenHandle& k) { return k.id == id; });

      if (kitchenIt == kitchens_.end() || !kitchenIt->alive) {
        it = pendingIds.erase(it);
        continue;
      }

      plazza::Packet packet{};
      bool receivedReply = false;
      while (kitchenIt->resultQueue->receive(packet, false)) {
        if (packet.type == plazza::MessageType::StatusReply) {
          receivedReply = true;
        }
        handlePacket(*kitchenIt, packet);
      }

      if (receivedReply || !kitchenIt->alive) {
        it = pendingIds.erase(it);
      } else {
        ++it;
      }
    }
    if (!pendingIds.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  std::vector<KitchenStatus> statuses;
  for (const auto& kitchen : kitchens_) {
    if (kitchen.alive) {
      statuses.push_back(kitchen.status);
    }
  }
  return statuses;
}

void LoadBalancer::handlePacket(KitchenHandle& kitchen,
                                const plazza::Packet& packet) {
  switch (packet.type) {
    case plazza::MessageType::Done:
      if (kitchen.load > 0) {
        --kitchen.load;
      }
      break;
    case plazza::MessageType::CookStatus:
      kitchen.pendingCooks.push_back(
          {.id = packet.cook.id,
           .state = static_cast<kitchen::CookState>(packet.cook.state),
           .currentPizza = "",
           .type = static_cast<PizzaType>(packet.cook.pizzaType),
           .size = static_cast<PizzaSize>(packet.cook.pizzaSize)});
      break;
    case plazza::MessageType::StatusReply:
      kitchen.load = packet.load;
      kitchen.status.load = packet.load;
      kitchen.status.capacity = packet.capacity;
      kitchen.status.cooks = std::move(kitchen.pendingCooks);
      kitchen.pendingCooks.clear();
      for (std::size_t i = 0; i < 9; ++i) {
        kitchen.status.stock[static_cast<kitchen::Ingredient>(i)] =
            packet.stock.quantities[i];
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

void LoadBalancer::dispatch(const std::vector<PizzaOrder>& orders) {
  updateKitchens();
  for (const auto& order : orders) {
    for (std::size_t i = 0; i < order.quantity; ++i) {
      KitchenHandle* bestKitchen = nullptr;
      for (auto& kitchen : kitchens_) {
        if (kitchen.alive && kitchen.load < kitchen.capacity) {
          if (bestKitchen == nullptr || kitchen.load < bestKitchen->load) {
            bestKitchen = &kitchen;
          }
        }
      }
      if (bestKitchen == nullptr) {
        createNewKitchen(kitchens_, nextKitchenId_, nCooks_, regenMs_,
                         multiplier_);
        bestKitchen = &kitchens_.back();
      }
      plazza::Packet pizzaPacket{};
      pizzaPacket.type = plazza::MessageType::Pizza;
      pizzaPacket.pizza = pack({.type = order.type, .size = order.size});
      bestKitchen->orderQueue->send(pizzaPacket);
      ++bestKitchen->load;
    }
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
                                 [](const KitchenHandle& kitchen) {
                                   return !kitchen.alive;
                                 }),
                  kitchens_.end());
}
