/*
** EPITECH PROJECT, 2026
** raytracer
** File description:
** Parser
*/

#include "Parser.hpp"
#include <string>
#include <vector>
#include "Pizza.hpp"

std::vector<PizzaOrder> Parser::parse(const std::string& line) {
  (void)line;
  return {};
}

PizzaType Parser::parseType(const std::string& token) {
  (void)token;
  return Regina;
}

PizzaSize Parser::parseSize(const std::string& token) {
  (void)token;
  return S;
}

std::size_t Parser::parseNumber(const std::string& token) {
  (void)token;
  return 0;
}
