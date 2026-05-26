/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Cook
*/

#include "Cook.hpp"
#include <chrono>
#include <stdexcept>
#include <thread>
#include <unordered_map>

namespace {

const std::unordered_map<std::string, kitchen::Ingredient>& ingredientMap() {
  static const std::unordered_map<std::string, kitchen::Ingredient> map = [] {
    std::unordered_map<std::string, kitchen::Ingredient> result;
    for (const auto& entry : kitchen::IngredientStock::kAllIngredients) {
      result.emplace(entry.name, entry.value);
    }
    return result;
  }();
  return map;
}

kitchen::Ingredient toIngredient(const std::string& ingredientName) {
  const auto& ingredients = ingredientMap();
  if (const auto ingredientIt = ingredients.find(ingredientName);
      ingredientIt != ingredients.end()) {
    return ingredientIt->second;
  }
  throw std::invalid_argument("Unknown ingredient: " + ingredientName);
}

std::vector<kitchen::Ingredient> toIngredients(
    const std::vector<std::string>& ingredientNames) {
  std::vector<kitchen::Ingredient> ingredients;
  ingredients.reserve(ingredientNames.size());
  for (const auto& ingredientName : ingredientNames) {
    ingredients.push_back(toIngredient(ingredientName));
  }
  return ingredients;
}

}  // namespace

namespace kitchen {
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
Cook::Cook(int cookId, double multiplier)
    : id_(cookId)
    , state_(CookState::IDLE)
    , multiplier_(multiplier) {}

CookStatus Cook::status() const noexcept {
  std::lock_guard lock(mutex_);
  return CookStatus{
      .id = id_, .state = state_, .currentPizza = currentPizzaName_};
}

int Cook::id() const noexcept { return id_; }

CookState Cook::state() const noexcept {
  std::lock_guard lock(mutex_);
  return state_;
}

void Cook::cookPizza(const PizzaRecipe& pizza, IngredientStock& stock,
                     MessageQueue& messageQueue) {
  {
    std::lock_guard lock(mutex_);
    state_ = CookState::COOKING;
    currentPizzaName_ = pizza.getName();
  }
  try {
    if (!stock.waitAndConsumeIngredients(toIngredients(pizza.ingredients()))) {
      std::lock_guard lock(mutex_);
      state_ = CookState::IDLE;
      currentPizzaName_.clear();
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(
        static_cast<int>(pizza.cookingTime(multiplier_) * 1000)));
    messageQueue << Pizza{.type = pizza.type(), .size = pizza.size()};
  } catch (...) {
    std::lock_guard lock(mutex_);
    state_ = CookState::IDLE;
    currentPizzaName_.clear();
    throw;
  }
  std::lock_guard lock(mutex_);
  state_ = CookState::IDLE;
  currentPizzaName_.clear();
}
}  // namespace kitchen
