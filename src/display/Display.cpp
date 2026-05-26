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
#include "pizza/factory/PizzaFactory.hpp"
#include "reception/kitchenHandle/KitchenHandle.hpp"

namespace display {

void printStatus(const std::vector<KitchenStatus>& statuses) {
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

void notifyPizzaDone(int kitchenId, PizzaType type, PizzaSize size) {
  const auto recipe = PizzaFactory::create(type, size);
  std::cout << "\n[Kitchen #" << kitchenId << "] " << recipe.getName()
            << " ready!\n> " << std::flush;
}

}  // namespace display
