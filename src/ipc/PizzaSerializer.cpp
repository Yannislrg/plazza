/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaSerializer
*/

#include "ipc/PizzaSerializer.hpp"
#include "Pizza.hpp"

PackedPizza pack(const Pizza& pizza) {
  return {static_cast<char>(pizza.type), static_cast<char>(pizza.size)};
}

Pizza unpack(const PackedPizza& data) {
  return {.type = static_cast<PizzaType>(data[0]),
          .size = static_cast<PizzaSize>(data[1])};
}
