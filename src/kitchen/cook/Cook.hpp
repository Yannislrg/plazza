/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Cook
*/

#pragma once

#include <cstdint>
#include "Pizza.hpp"
#include "concurrency/Mutex.hpp"
#include "ipc/MessageQueue.hpp"
#include "kitchen/IngredientStock/IngredientStock.hpp"
#include "pizza/Pizza.hpp"

namespace kitchen {

enum class CookState : std::uint8_t {
  IDLE,
  COOKING,
};

struct CookStatus {
  int id;
  CookState state;
  PizzaType currentPizza;
};

class Cook {
 public:
  Cook();
  // NOLINTNEXTLINE (readability-identifier-length)
  explicit Cook(int id);
  ~Cook();

  Cook(const Cook&) = delete;
  Cook& operator=(const Cook&) = delete;
  Cook(Cook&&) = delete;
  Cook& operator=(Cook&&) = delete;

  [[nodiscard]] CookStatus status() const noexcept;
  [[nodiscard]] int id() const noexcept;
  [[nodiscard]] CookState state() const noexcept;

  void cookPizza(PizzaRecipe pizza, IngredientStock& stock,
                 MessageQueue& messageQueue);

 private:
  int id_;
  CookState state_;
  Mutex mutex_;
  std::string currentPizzaName_;
};
}  // namespace kitchen
