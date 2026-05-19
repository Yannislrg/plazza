/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaFactory
*/

#include "PizzaFactory.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
std::vector<std::pair<std::string, PizzaRecipe>>& registry() {
  static std::vector<std::pair<std::string, PizzaRecipe>> instance;
  return instance;
}

void registerBuiltinPizzas() {
  static bool alreadyRegistered = false;
  if (alreadyRegistered) {
    return;
  }

  alreadyRegistered = true;
  PizzaFactory::registerPizza("Margarita",
                              PizzaRecipe("Margarita", PizzaSize::M,
                                          {"dough", "tomato", "gruyere"}, 1));
  PizzaFactory::registerPizza(
      "Regina",
      PizzaRecipe("Regina", PizzaSize::M,
                  {"dough", "tomato", "gruyere", "ham", "mushrooms"}, 2));
  PizzaFactory::registerPizza(
      "Americana", PizzaRecipe("Americana", PizzaSize::M,
                               {"dough", "tomato", "gruyere", "steak"}, 2));
  PizzaFactory::registerPizza(
      "Fantasia",
      PizzaRecipe("Fantasia", PizzaSize::M,
                  {"dough", "tomato", "eggplant", "goat cheese", "chef love"},
                  4));
}
}  // namespace

void PizzaFactory::registerPizza(const std::string& typeName,
                                 const PizzaRecipe& prototype) {
  auto existingPizza = std::find_if(
      registry().begin(), registry().end(),
      [&typeName](const auto& entry) { return entry.first == typeName; });
  if (existingPizza == registry().end()) {
    registry().emplace_back(typeName, prototype);
  }
}

PizzaRecipe PizzaFactory::create(const std::string& typeName, PizzaSize size) {
  registerBuiltinPizzas();
  auto matchingPizza = std::find_if(
      registry().begin(), registry().end(),
      [&typeName](const auto& entry) { return entry.first == typeName; });
  if (matchingPizza == registry().end()) {
    throw std::runtime_error("Unknown pizza type: " + typeName);
  }
  return matchingPizza->second.withSize(size);
}
