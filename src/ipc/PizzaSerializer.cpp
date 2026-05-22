/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaSerializer
*/

#include "ipc/PizzaSerializer.hpp"
#include "Pizza.hpp"

namespace {

constexpr int kReginaWireValue = 1;
constexpr int kMargaritaWireValue = 2;
constexpr int kAmericanaWireValue = 4;
constexpr int kFantasiaWireValue = 8;

constexpr int kSWireValue = 1;
constexpr int kMWireValue = 2;
constexpr int kLWireValue = 4;
constexpr int kXLWireValue = 8;
constexpr int kXXLWireValue = 16;

}  // namespace

static_assert(Regina == kReginaWireValue && Margarita == kMargaritaWireValue &&
                  Americana == kAmericanaWireValue &&
                  Fantasia == kFantasiaWireValue,
              "PizzaType values must match the IPC protocol");
static_assert(S == kSWireValue && M == kMWireValue && L == kLWireValue &&
                  XL == kXLWireValue && XXL == kXXLWireValue,
              "PizzaSize values must match the IPC protocol");

PackedPizza pack(const Pizza& pizza) {
  return {static_cast<char>(pizza.type), static_cast<char>(pizza.size)};
}

Pizza unpack(const PackedPizza& data) {
  return {.type = static_cast<PizzaType>(data[0]),
          .size = static_cast<PizzaSize>(data[1])};
}
