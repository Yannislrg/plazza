/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaSerializer
*/

#include "ipc/PizzaSerializer.hpp"
#include "Pizza.hpp"

static_assert(Regina == 1 && Margarita == 2 && Americana == 4 && Fantasia == 8,
              "PizzaType values must match the IPC protocol");
static_assert(S == 1 && M == 2 && L == 4 && XL == 8 && XXL == 16,
              "PizzaSize values must match the IPC protocol");

PackedPizza pack(const Pizza& pizza) {
  return {static_cast<char>(pizza.type), static_cast<char>(pizza.size)};
}

Pizza unpack(const PackedPizza& data) {
  return {.type = static_cast<PizzaType>(data[0]),
          .size = static_cast<PizzaSize>(data[1])};
}
