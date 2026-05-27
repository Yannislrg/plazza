/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** KitchenHandle
*/

#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <vector>
#include "concurrency/Process.hpp"
#include "ipc/MessageQueue.hpp"
#include "kitchen/IngredientStock/IngredientStock.hpp"
#include "kitchen/cook/Cook.hpp"

struct KitchenStatus {
  int id;
  std::size_t load;
  std::size_t capacity;
  std::vector<kitchen::CookStatus> cooks;
  std::map<kitchen::Ingredient, std::size_t> stock;
};

struct KitchenHandle {
  int id;
  pid_t pid;
  std::unique_ptr<Process> process;
  std::unique_ptr<MessageQueue> orderQueue;
  std::unique_ptr<MessageQueue> resultQueue;
  std::size_t load;
  std::size_t capacity;
  bool alive;
  KitchenStatus status;
  std::vector<kitchen::CookStatus> pendingCooks;
};
