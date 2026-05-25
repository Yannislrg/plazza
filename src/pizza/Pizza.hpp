/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaRecipe
*/

#pragma once

#include <Pizza.hpp>
#include <string>
#include <vector>

class PizzaRecipe {
 public:
  PizzaRecipe() = default;
  PizzaRecipe(PizzaType type, PizzaSize size,
              std::vector<std::string> ingredients,
              double baseCookSeconds) noexcept;
  ~PizzaRecipe() = default;

  PizzaRecipe(const PizzaRecipe&) = default;
  PizzaRecipe& operator=(const PizzaRecipe&) = default;
  PizzaRecipe(PizzaRecipe&&) noexcept = default;
  PizzaRecipe& operator=(PizzaRecipe&&) noexcept = default;

  [[nodiscard]] PizzaType type() const noexcept;
  [[nodiscard]] PizzaSize size() const noexcept;
  [[nodiscard]] const std::vector<std::string>& ingredients() const noexcept;
  [[nodiscard]] double cookingTime(double multiplier) const noexcept;

  [[nodiscard]] PizzaRecipe withSize(PizzaSize newSize) const noexcept;

  [[nodiscard]] std::string getName() const noexcept;

 private:
  PizzaType _type{PizzaType::Margarita};
  PizzaSize _size{PizzaSize::M};
  std::vector<std::string> _ingredients;
  double _baseCookSeconds = 0;
};
