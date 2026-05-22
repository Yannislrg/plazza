/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaSerializer
*/

#pragma once

#include <array>
#include "Pizza.hpp"

using PackedPizza = std::array<char, 2>;

PackedPizza pack(const Pizza& pizza);

Pizza unpack(const PackedPizza& data);
