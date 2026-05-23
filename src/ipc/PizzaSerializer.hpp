/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** PizzaSerializer
*/

#pragma once

#include <array>
#include "Pizza.hpp"

struct PackedPizza {
  std::array<char, 2> bytes{};
};

/**
 * @brief Serializes a Pizza into its opaque wire representation.
 *
 * @param[in] pizza  Pizza to serialize.
 * @returns A PackedPizza encoding the type and size.
 */
PackedPizza pack(const Pizza& pizza);

/**
 * @brief Deserializes a PackedPizza back into a Pizza.
 *
 * @param[in] data  Buffer produced by pack().
 * @returns The reconstructed Pizza.
 */
Pizza unpack(const PackedPizza& data);
