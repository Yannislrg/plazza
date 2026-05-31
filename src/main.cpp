/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** main
*/

#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include "Pizza.hpp"
#include "display/Display.hpp"
#include "pizza/factory/PizzaFactory.hpp"
#include "reception/loadBalancer/LoadBalancer.hpp"
#include "shell/Shell.hpp"

namespace {

void installDoneCallback(LoadBalancer& loadBalancer,
                         std::queue<std::pair<int, int>>& pendingOrders) {
  loadBalancer.setDoneCallback(
      [&](int /*kitchenId*/, PizzaType /*type*/, PizzaSize /*size*/) {
        if (pendingOrders.empty()) {
          return;
        }
        if (--pendingOrders.front().second == 0) {
          display::Display::notifyOrderReady(pendingOrders.front().first);
          pendingOrders.pop();
        }
      });
}

void configureShellCallbacks(Shell& shell, LoadBalancer& loadBalancer,
                             std::queue<std::pair<int, int>>& pendingOrders,
                             int& nextOrderId) {
  shell.setOrderCallback([&](const std::vector<PizzaOrder>& orders) {
    int total = 0;
    for (const auto& order : orders) {
      total += static_cast<int>(order.quantity);
    }
    if (total > 0) {
      pendingOrders.emplace(++nextOrderId, total);
    }
    loadBalancer.dispatch(orders);
  });
  shell.setStatusCallback([&loadBalancer]() {
    display::Display::printStatus(loadBalancer.getStatus());
  });
  shell.setPollCallback([&loadBalancer]() { loadBalancer.updateKitchens(); });
}

}  // namespace

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
      std::cerr << "multiplier must be > 0, cooks and regenTime must be "
                   "strictly positive\n";
      return 84;
    }

    PizzaFactory factory;
    LoadBalancer loadBalancer(factory, nCooks, regenMs, multiplier);

    int nextOrderId = 0;
    std::queue<std::pair<int, int>> pendingOrders;

    installDoneCallback(loadBalancer, pendingOrders);

    Shell shell;
    configureShellCallbacks(shell, loadBalancer, pendingOrders, nextOrderId);

    shell.run();
  } catch (const std::exception& exc) {
    std::cerr << "Error: " << exc.what() << "\n";
    return 84;
  }
  return 0;
}
