/*
** EPITECH PROJECT, 2026
** raytracer
** File description:
** Parser
*/

#include "Parser.hpp"
#include <cctype>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>
#include "Pizza.hpp"
#include "exceptions/Exception.hpp"

namespace {
std::string toLower(const std::string& str) {
  std::string low;
  for (char character : str) {
    char lowerC = static_cast<char>(std::tolower(character));
    low.push_back(lowerC);
  }
  return low;
}
}  // namespace

using plazza::exceptions::Exception;

std::vector<PizzaOrder> Parser::parse(const std::string& line) {
  std::vector<PizzaOrder> orders;
  std::istringstream iss(line);
  std::string segment;
  while (std::getline(iss, segment, ';')) {
    if (segment.find_first_not_of(" \t\r\n") == std::string::npos) {
      continue;
    }
    orders.push_back(parseLine(segment));
  }
  if (orders.empty()) {
    Exception::thrownError("empty order");
  }
  return orders;
}

PizzaOrder Parser::parseLine(const std::string& line) {
  std::istringstream iss(line);
  std::string type_token;
  std::string size_token;
  std::string quantity_token;

  if (!(iss >> type_token >> size_token >> quantity_token)) {
    Exception::thrownError("invalid token: expected TYPE SIZE NUMBER");
  }

  std::string extra;
  if (iss >> extra) {
    Exception::thrownError("unexpected token: " + extra);
  }

  PizzaOrder order = {};
  order.type = parseType(type_token);
  order.size = parseSize(size_token);
  order.quantity = parseNumber(quantity_token);
  return order;
}

PizzaType Parser::parseType(const std::string& token) {
  std::string low = toLower(token);

  if (low == "regina") {
    return PizzaType::Regina;
  }
  if (low == "margarita") {
    return PizzaType::Margarita;
  }
  if (low == "americana") {
    return PizzaType::Americana;
  }
  if (low == "fantasia") {
    return PizzaType::Fantasia;
  }
  Exception::thrownError("unknown pizza type: " + token);
}

PizzaSize Parser::parseSize(const std::string& token) {
  std::string low = toLower(token);

  if (low == "s") {
    return PizzaSize::S;
  }
  if (low == "m") {
    return PizzaSize::M;
  }
  if (low == "l") {
    return PizzaSize::L;
  }
  if (low == "xl") {
    return PizzaSize::XL;
  }
  if (low == "xxl") {
    return PizzaSize::XXL;
  }
  Exception::thrownError("unknown pizza size: " + token);
}

std::size_t Parser::parseNumber(const std::string& token) {
  if (token.empty() || token[0] != 'x') {
    Exception::thrownError("invalid number format (expected x<n>): " + token);
  }
  if (token.size() < 2 || token[1] < '1' || token[1] > '9') {
    Exception::thrownError("quantity must be >= 1: " + token);
  }
  for (std::size_t i = 2; i < token.size(); ++i) {
    if (std::isdigit(static_cast<unsigned char>(token[i])) == 0) {
      Exception::thrownError("invalid number format: " + token);
    }
  }
  return std::stoul(token.substr(1));
}
