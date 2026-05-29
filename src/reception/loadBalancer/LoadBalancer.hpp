/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** LoadBalancer
*/

#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <vector>
#include "Pizza.hpp"
#include "logger/Logger.hpp"
#include "pizza/factory/PizzaFactory.hpp"
#include "reception/kitchenHandle/KitchenHandle.hpp"

class LoadBalancer {
 public:
  LoadBalancer(PizzaFactory& factory, std::size_t nCooks, std::size_t regenMs,
               double multiplier = 1.0);
  ~LoadBalancer();

  LoadBalancer(const LoadBalancer&) = delete;
  LoadBalancer& operator=(const LoadBalancer&) = delete;
  LoadBalancer(LoadBalancer&&) = delete;
  LoadBalancer& operator=(LoadBalancer&&) = delete;

  void dispatch(const std::vector<PizzaOrder>& orders);
  [[nodiscard]] std::vector<KitchenStatus> getStatus();
  void setDoneCallback(std::function<void(int, PizzaType, PizzaSize)> callback);

 private:
  void handlePacket(KitchenHandle& kitchen, const plazza::Packet& packet);
  void updateKitchens();
  void shutdown();

  PizzaFactory& factory_;
  std::size_t nCooks_;
  std::size_t regenMs_;
  double multiplier_;
  std::atomic<int> nextKitchenId_{0};
  std::vector<KitchenHandle> kitchens_;
  std::function<void(int, PizzaType, PizzaSize)> doneCb_;
  Logger logger_;
};
