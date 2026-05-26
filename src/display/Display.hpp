/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Display
*/

#pragma once

#include <vector>
#include "Pizza.hpp"
#include "reception/kitchenHandle/KitchenHandle.hpp"

namespace display {

void printStatus(const std::vector<KitchenStatus>& statuses);
void notifyPizzaDone(int kitchenId, PizzaType type, PizzaSize size);

}  // namespace display
