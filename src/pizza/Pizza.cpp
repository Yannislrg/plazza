/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Pizza
*/

#include "Pizza.hpp"
#include <utility>

PizzaRecipe::PizzaRecipe(std::string type, PizzaSize size,
                         std::vector<std::string> ingredients,
                         int baseCookSeconds) noexcept
    : _type(std::move(type)),
      _size(size),
      _ingredients(std::move(ingredients)),
      _baseCookSeconds(baseCookSeconds) {}

const std::string& PizzaRecipe::type() const noexcept { return _type; }

PizzaSize PizzaRecipe::size() const noexcept { return _size; }

const std::vector<std::string>& PizzaRecipe::ingredients() const noexcept {
  return _ingredients;
}

int PizzaRecipe::cookingTime(int multiplier) const noexcept {
  return _baseCookSeconds * multiplier;
}

PizzaRecipe PizzaRecipe::withSize(PizzaSize newSize) const noexcept {
  PizzaRecipe copy = *this;
  copy._size = newSize;
  return copy;
}
