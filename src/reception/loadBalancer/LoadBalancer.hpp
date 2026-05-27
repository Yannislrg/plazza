/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** LoadBalancer
*/

#pragma once

#include <atomic>
#include <cstddef>
#include <vector>
#include "Pizza.hpp"
#include "concurrency/ConditionVariable.hpp"
#include "concurrency/Mutex.hpp"
#include "concurrency/Thread.hpp"
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
  [[nodiscard]] std::vector<KitchenStatus> getStatus() const;
  void shutdown();
  void updateKitchens();

 private:
  PizzaFactory& factory_;
  std::vector<KitchenHandle> kitchens_;
  std::atomic<int> nextKitchenId_;
  std::size_t nCooks_;
  std::size_t regenMs_;
  double multiplier_;
};
