/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Pizza
*/

#include "Pizza.hpp"
#include <utility>

PizzaRecipe::PizzaRecipe(PizzaType type, PizzaSize size,
                         std::vector<std::string> ingredients,
                         double baseCookSeconds) noexcept
    : _type(type)
    , _size(size)
    , _ingredients(std::move(ingredients))
    , _baseCookSeconds(baseCookSeconds) {}

PizzaType PizzaRecipe::type() const noexcept { return _type; }

PizzaSize PizzaRecipe::size() const noexcept { return _size; }

const std::vector<std::string>& PizzaRecipe::ingredients() const noexcept {
  return _ingredients;
}

double PizzaRecipe::cookingTime(double multiplier) const noexcept {
  return _baseCookSeconds * multiplier;
}

PizzaRecipe PizzaRecipe::withSize(PizzaSize newSize) const noexcept {
  PizzaRecipe copy = *this;
  copy._size = newSize;
  return copy;
}

std::string PizzaRecipe::getName() const noexcept {
  switch (_type) {
    case PizzaType::Margarita:
      return "Margarita";
    case PizzaType::Regina:
      return "Regina";
    case PizzaType::Americana:
      return "Americana";
    case PizzaType::Fantasia:
      return "Fantasia";
    default:
      return "Unknown";
  }
}
