/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Pizza
*/

#pragma once

#include <cstddef>
#include <cstdint>

enum PizzaType : std::uint8_t {
  Regina = 1,
  Margarita = 2,
  Americana = 4,
  Fantasia = 8
};

enum PizzaSize : std::uint8_t { S = 1, M = 2, L = 4, XL = 8, XXL = 16 };

struct Pizza {
  PizzaType type;
  PizzaSize size;
};

struct PizzaOrder {
  PizzaType type;
  PizzaSize size;
  std::size_t quantity;
};
