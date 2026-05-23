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

namespace {

kitchen::Ingredient toIngredient(const std::string& ingredientName) {
  if (ingredientName == "dough") {
    return kitchen::Ingredient::Dough;
  }
  if (ingredientName == "tomato") {
    return kitchen::Ingredient::Tomato;
  }
  if (ingredientName == "gruyere") {
    return kitchen::Ingredient::Gruyere;
  }
  if (ingredientName == "ham") {
    return kitchen::Ingredient::Ham;
  }
  if (ingredientName == "mushrooms") {
    return kitchen::Ingredient::Mushrooms;
  }
  if (ingredientName == "steak") {
    return kitchen::Ingredient::Steak;
  }
  if (ingredientName == "eggplant") {
    return kitchen::Ingredient::Eggplant;
  }
  if (ingredientName == "goat cheese") {
    return kitchen::Ingredient::GoatCheese;
  }
  if (ingredientName == "chef love") {
    return kitchen::Ingredient::ChiefLove;
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
  stock.waitAndConsumeIngredients(toIngredients(pizza.ingredients()));
  std::this_thread::sleep_for(std::chrono::milliseconds(
      static_cast<int>(pizza.cookingTime(multiplier_) * 1000)));
  messageQueue << Pizza{.type = pizza.type(), .size = pizza.size()};
  {
    std::lock_guard lock(mutex_);
    state_ = CookState::IDLE;
    currentPizzaName_.clear();
  }
}
}  // namespace kitchen
