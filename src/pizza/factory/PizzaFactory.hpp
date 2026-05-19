/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaFactory
*/

#pragma once

#include <string>
#include "pizza/Pizza.hpp"

class PizzaFactory {
 public:
  static void registerPizza(const std::string& typeName,
                            const PizzaRecipe& prototype);
  static PizzaRecipe create(const std::string& typeName, PizzaSize size);
};
