/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Constants for error messages
*/

#pragma once

#include <string>

namespace plazza::constants {

constexpr std::string_view kEmptyOrderSegment = "empty order segment";
constexpr std::string_view kEmptyOrder = "empty order";
constexpr std::string_view kInvalidToken =
    "invalid token: expected TYPE SIZE NUMBER";
constexpr std::string_view kUnknownPizzaType = "unknown pizza type: ";
constexpr std::string_view kUnknownPizzaSize = "unknown pizza size: ";
constexpr std::string_view kInvalidNumberFormatExpected =
    "invalid number format (expected x<n>): ";
constexpr std::string_view kQuantityMustBePositive = "quantity must be >= 1: ";
constexpr std::string_view kInvalidNumberFormat = "invalid number format: ";

}  // namespace plazza::constants
