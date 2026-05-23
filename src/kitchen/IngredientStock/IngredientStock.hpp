/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** IngredientStock
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
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
  IngredientStock();
  explicit IngredientStock(std::size_t initialStock);
  ~IngredientStock();

  IngredientStock(const IngredientStock&) = delete;
  IngredientStock& operator=(const IngredientStock&) = delete;
  IngredientStock(IngredientStock&&) = delete;
  IngredientStock& operator=(IngredientStock&&) = delete;

  bool hasIngredients(const std::size_t* ingredients, std::size_t count) const;
  void waitAndConsumeIngredients(
      const std::vector<Ingredient>& ingredientsNeeded);
  [[nodiscard]] std::map<Ingredient, std::size_t> stock() const;

 private:
  std::map<Ingredient, std::size_t> _stock;
  std::size_t _regenDelayMs;
  mutable Mutex _mutex;
  ConditionVariable _cv;
  Thread _regenThread;
  bool _running;
};
}  // namespace kitchen
