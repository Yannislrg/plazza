/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** IngredientStock
*/

#include "kitchen/IngredientStock/IngredientStock.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "concurrency/Mutex.hpp"
#include "concurrency/Thread.hpp"

namespace kitchen {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
IngredientStock::IngredientStock(std::size_t regenDelayMs,
                                 std::size_t initialStock)
    : regenDelayMs_(regenDelayMs)
    , running_(true) {
  for (const auto& entry : kAllIngredients) {
    stock_[entry.value] = initialStock;
  }
  regenThread_ = std::make_unique<Thread>([this] { regenLoop(); });
}

IngredientStock::~IngredientStock() {
  {
    const std::scoped_lock<Mutex> guard(mutex_);
    running_ = false;
  }
  cv_.notifyAll();
}

bool IngredientStock::waitAndConsumeIngredients(
    const std::vector<Ingredient>& needed) {
  mutex_.lock();
  while (running_ && !allAvailable(needed)) {
    cv_.wait(mutex_);
  }
  if (!running_) {
    mutex_.unlock();
    return false;
  }
  for (const auto ingredient : needed) {
    --stock_[ingredient];
  }
  mutex_.unlock();
  return true;
}

std::map<Ingredient, std::size_t> IngredientStock::stock() const {
  const std::scoped_lock<Mutex> guard(mutex_);
  return stock_;
}

bool IngredientStock::allAvailable(
    const std::vector<Ingredient>& ingredients) const {
  return std::ranges::all_of(ingredients, [this](const Ingredient ingredient) {
    const auto entry = stock_.find(ingredient);
    return entry != stock_.end() && entry->second > 0;
  });
}

void IngredientStock::regenLoop() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(regenDelayMs_));
    const std::scoped_lock<Mutex> guard(mutex_);
    if (!running_) {
      return;
    }
    for (auto& [ingredient, count] : stock_) {
      ++count;
    }
    cv_.notifyAll();
  }
}

}  // namespace kitchen
