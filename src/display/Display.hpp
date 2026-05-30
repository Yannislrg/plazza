/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Display
*/

#pragma once

#include <vector>
#include "reception/kitchenHandle/KitchenHandle.hpp"

namespace display {

class Display {
 public:
  static void printStatus(const std::vector<KitchenStatus>& statuses);
  static void notifyPizzaReady(const std::string& pizzaName);
  static void notifyOrderReady(int orderId);
};

}  // namespace display
