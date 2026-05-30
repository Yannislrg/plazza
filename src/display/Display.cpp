/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Display
*/

#include "display/Display.hpp"
#include <iostream>
#include <vector>
#include "kitchen/IngredientStock/IngredientStock.hpp"
#include "kitchen/cook/Cook.hpp"
#include "reception/kitchenHandle/KitchenHandle.hpp"

namespace display {

void Display::printStatus(const std::vector<KitchenStatus>& statuses) {
  if (statuses.empty()) {
    std::cout << "No active kitchen.\n";
    return;
  }
  for (const auto& status : statuses) {
    std::cout << "=== Kitchen #" << status.id << " [" << status.load << "/"
              << status.capacity << "] ===\n";
    std::cout << "  Cooks:\n";
    for (const auto& cook : status.cooks) {
      std::cout << "    Cook " << cook.id << ": ";
      if (cook.state == kitchen::CookState::IDLE) {
        std::cout << "idle\n";
      } else {
        std::cout << "cooking " << cook.currentPizza << "\n";
      }
    }
    std::cout << "  Stock:\n";
    for (const auto& entry : kitchen::IngredientStock::kAllIngredients) {
      const auto stockIt = status.stock.find(entry.value);
      const std::size_t qty =
          (stockIt != status.stock.end()) ? stockIt->second : 0;
      std::cout << "    " << entry.name << ": " << qty << "\n";
    }
  }
}

void Display::notifyOrderReady(int orderId) {
  std::cout << "\nOrder #" << orderId << " ready!\n> " << std::flush;
}

}  // namespace display
