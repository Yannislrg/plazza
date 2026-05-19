/*
** EPITECH PROJECT, 2026
** raytracer
** File description:
** Parser
*/

#pragma once

#include <string>
#include <vector>
#include "Pizza.hpp"

class Parser {
 public:
  static std::vector<PizzaOrder> parse(const std::string& line);

 private:
  static PizzaOrder parseLine(const std::string& line);
  static PizzaType parseType(const std::string& token);
  static PizzaSize parseSize(const std::string& token);
  static std::size_t parseNumber(const std::string& token);
};
