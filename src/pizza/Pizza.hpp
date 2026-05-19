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
  PizzaRecipe(std::string type, PizzaSize size,
              std::vector<std::string> ingredients,
              int baseCookSeconds) noexcept;
  ~PizzaRecipe() = default;

  PizzaRecipe(const PizzaRecipe&) = default;
  PizzaRecipe& operator=(const PizzaRecipe&) = default;
  PizzaRecipe(PizzaRecipe&&) noexcept = default;
  PizzaRecipe& operator=(PizzaRecipe&&) noexcept = default;

  [[nodiscard]] const std::string& type() const noexcept;
  [[nodiscard]] PizzaSize size() const noexcept;
  [[nodiscard]] const std::vector<std::string>& ingredients() const noexcept;
  [[nodiscard]] int cookingTime(int multiplier) const noexcept;

  [[nodiscard]] PizzaRecipe withSize(PizzaSize newSize) const noexcept;

 private:
  std::string _type;
  PizzaSize _size{PizzaSize::M};
  std::vector<std::string> _ingredients;
  int _baseCookSeconds{0};
};
