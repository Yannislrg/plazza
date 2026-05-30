/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** IngredientStock
*/

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string_view>
#include <vector>
#include "concurrency/ConditionVariable.hpp"
#include "concurrency/Mutex.hpp"
#include "concurrency/Thread.hpp"

namespace kitchen {

enum class Ingredient : std::uint8_t {
  Dough,
  Tomato,
  Gruyere,
  Ham,
  Mushrooms,
  Steak,
  Eggplant,
  GoatCheese,
  ChiefLove,
};

class IngredientStock {
 public:
  struct IngredientEntry {
    std::string_view name;
    Ingredient value;
  };

  static constexpr std::array<IngredientEntry, 9> kAllIngredients{{
      {.name = "dough", .value = Ingredient::Dough},
      {.name = "tomato", .value = Ingredient::Tomato},
      {.name = "gruyere", .value = Ingredient::Gruyere},
      {.name = "ham", .value = Ingredient::Ham},
      {.name = "mushrooms", .value = Ingredient::Mushrooms},
      {.name = "steak", .value = Ingredient::Steak},
      {.name = "eggplant", .value = Ingredient::Eggplant},
      {.name = "goat cheese", .value = Ingredient::GoatCheese},
      {.name = "chief love", .value = Ingredient::ChiefLove},
  }};

  explicit IngredientStock(std::size_t regenDelayMs,
                           std::size_t initialStock = 5);
  ~IngredientStock();

  IngredientStock(const IngredientStock&) = delete;
  IngredientStock& operator=(const IngredientStock&) = delete;
  IngredientStock(IngredientStock&&) = delete;
  IngredientStock& operator=(IngredientStock&&) = delete;

  [[nodiscard]] bool waitAndConsumeIngredients(
      const std::vector<Ingredient>& ingredientsNeeded);
  [[nodiscard]] std::map<Ingredient, std::size_t> stock() const;

 private:
  [[nodiscard]] bool allAvailable(
      const std::vector<Ingredient>& ingredients) const;
  void regenLoop();

  std::map<Ingredient, std::size_t> stock_;
  std::size_t regenDelayMs_;
  mutable Mutex mutex_;
  ConditionVariable cv_;
  bool running_{false};
  std::unique_ptr<Thread> regenThread_;
};

}  // namespace kitchen
