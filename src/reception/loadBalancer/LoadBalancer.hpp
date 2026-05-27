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
#include "logger/Logger.hpp"
#include "pizza/factory/PizzaFactory.hpp"
#include "reception/kitchenHandle/KitchenHandle.hpp"

class LoadBalancer {
 public:
  LoadBalancer(PizzaFactory& factory, std::size_t nCooks, std::size_t regenMs);
  ~LoadBalancer();

  LoadBalancer(const LoadBalancer&) = delete;
  LoadBalancer& operator=(const LoadBalancer&) = delete;
  LoadBalancer(LoadBalancer&&) = delete;
  LoadBalancer& operator=(LoadBalancer&&) = delete;

  void dispatch(const std::vector<PizzaOrder>& orders);
  std::vector<KitchenStatus> getStatus();
  void setDoneCallback(std::function<void(int, PizzaType, PizzaSize)> callback);

 private:
  PizzaFactory& _factory;
  std::size_t _nCooks;
  std::size_t _regenMs;
  double _multiplier;
  int _nextId;
  std::vector<KitchenHandle> _kitchens;
  Mutex _mutex;
  std::atomic<bool> _running{false};
  Thread _listenerThread;
  std::vector<KitchenStatus> _kitchenStatuses;
  int _pendingStatusReplies = 0;
  ConditionVariable _statusCv;
  std::function<void(int, PizzaType, PizzaSize)> _doneCb;
  Logger _logger;

  KitchenHandle* selectKitchen();
  KitchenHandle& spawnKitchen();

  static void sendPizza(KitchenHandle& kitchen, const PizzaRecipe& pizza);
  void listenLoop();
  void processKitchenPacket(KitchenHandle& kitchen);
  void updateKitchenStatus(int kitchenId, const plazza::Packet& response);
  void removeKitchen(int kitchenId);
};
