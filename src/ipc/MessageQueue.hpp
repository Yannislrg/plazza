/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** MessageQueue
*/

#pragma once

#include <cstdint>
#include <string>
#include "ipc/PizzaSerializer.hpp"

namespace plazza {

enum class MessageType : uint8_t {
  Pizza = 1,
  Done = 2,
  StatusRequest = 3,
  StatusReply = 4,
  Ready = 5,
  Shutdown = 6,
  Full = 7,
  CookStatus = 8
};

struct PackedCook {
  uint8_t id;
  uint8_t state;
  uint8_t pizzaType;
  uint8_t pizzaSize;
};

struct PackedStock {
  uint32_t quantities[9];
};

struct Packet {
  MessageType type;
  uint8_t kitchenId;
  PackedPizza pizza;
  uint8_t load;
  uint8_t capacity;
  uint8_t nCooks;
  PackedStock stock;
  PackedCook cook;
};

}  // namespace plazza

class MessageQueue {
 public:
  enum class Mode : std::uint8_t { Create, Open };

  MessageQueue(std::string path, Mode mode);
  ~MessageQueue();

  MessageQueue(const MessageQueue&) = delete;
  MessageQueue& operator=(const MessageQueue&) = delete;
  MessageQueue(MessageQueue&&) noexcept;
  MessageQueue& operator=(MessageQueue&&) noexcept;

  void send(const plazza::Packet& packet);
  bool receive(plazza::Packet& packet, bool blocking = false) const;

 private:
  void createAnchorFile() const;
  [[nodiscard]] int resolveKey() const;
  void openQueue(int key);

  int queueId_{-1};
  std::string path_;
  bool owner_{false};
};
