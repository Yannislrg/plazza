/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** main
*/

#include <iostream>
#include <string>
#include <vector>
#include "Pizza.hpp"
#include "display/Display.hpp"
#include "pizza/factory/PizzaFactory.hpp"
#include "reception/loadBalancer/LoadBalancer.hpp"
#include "shell/Shell.hpp"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
              << " <multiplier> <cooksPerKitchen> <regenTimeMs>\n";
    return 84;
  }
  try {
    const double multiplier = std::stod(argv[1]);
    const std::size_t nCooks = std::stoull(argv[2]);
    const std::size_t regenMs = std::stoull(argv[3]);

    if (multiplier <= 0 || nCooks == 0 || regenMs == 0) {
      std::cerr << "Arguments must be strictly positive\n";
      return 84;
    }

    PizzaFactory factory;
    LoadBalancer loadBalancer(factory, nCooks, regenMs, multiplier);
    loadBalancer.setDoneCallback(display::notifyPizzaDone);

    Shell shell;
    shell.setOrderCallback([&loadBalancer](const std::vector<PizzaOrder>& orders) {
      loadBalancer.dispatch(orders);
    });
    shell.setStatusCallback(
        [&loadBalancer]() { display::printStatus(loadBalancer.getStatus()); });

    shell.run();
  } catch (const std::exception& exc) {
    std::cerr << "Error: " << exc.what() << "\n";
    return 84;
  }
  return 0;
}
