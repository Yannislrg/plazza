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
std::vector<std::pair<PizzaType, PizzaRecipe>>& registry() {
  static std::vector<std::pair<PizzaType, PizzaRecipe>> instance;
  return instance;
}

void registerBuiltinPizzas() {
  static bool alreadyRegistered = false;
  if (alreadyRegistered) {
    return;
  }

  alreadyRegistered = true;
  PizzaFactory::registerPizza(PizzaType::Margarita,
                              PizzaRecipe(PizzaType::Margarita, PizzaSize::M,
                                          {"dough", "tomato", "gruyere"}, 1));
  PizzaFactory::registerPizza(
      PizzaType::Regina,
      PizzaRecipe(PizzaType::Regina, PizzaSize::M,
                  {"dough", "tomato", "gruyere", "ham", "mushrooms"}, 2));
  PizzaFactory::registerPizza(
      PizzaType::Americana,
      PizzaRecipe(PizzaType::Americana, PizzaSize::M,
                  {"dough", "tomato", "gruyere", "steak"}, 2));
  PizzaFactory::registerPizza(
      PizzaType::Fantasia,
      PizzaRecipe(PizzaType::Fantasia, PizzaSize::M,
                  {"dough", "tomato", "eggplant", "goat cheese", "chef love"},
                  4));
}
}  // namespace

void PizzaFactory::registerPizza(PizzaType type, const PizzaRecipe& prototype) {
  auto existingPizza =
      std::find_if(registry().begin(), registry().end(),
                   [type](const auto& entry) { return entry.first == type; });
  if (existingPizza == registry().end()) {
    registry().emplace_back(type, prototype);
  }
}

PizzaRecipe PizzaFactory::create(PizzaType type, PizzaSize size) {
  registerBuiltinPizzas();
  auto matchingPizza =
      std::find_if(registry().begin(), registry().end(),
                   [type](const auto& entry) { return entry.first == type; });
  if (matchingPizza == registry().end()) {
    throw std::runtime_error("Unknown pizza type");
  }
  return matchingPizza->second.withSize(size);
}
