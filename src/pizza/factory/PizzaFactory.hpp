/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaFactory
*/

#pragma once

#include "pizza/Pizza.hpp"

class PizzaFactory {
 public:
  static void registerPizza(PizzaType type, const PizzaRecipe& prototype);
  static PizzaRecipe create(PizzaType type, PizzaSize size);
};
