/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** MessageQueue
*/

#pragma once

#include <cstdint>
#include <string>
#include "Pizza.hpp"

class MessageQueue {
 public:
  enum class Mode : std::uint8_t { Create, Open };

  MessageQueue(std::string path, Mode mode);
  ~MessageQueue();

  MessageQueue(const MessageQueue&) = delete;
  MessageQueue& operator=(const MessageQueue&) = delete;
  MessageQueue(MessageQueue&&) noexcept;
  MessageQueue& operator=(MessageQueue&&) noexcept;

  MessageQueue& operator<<(const Pizza& pizza);
  bool operator>>(Pizza& pizza) const;

 private:
  void createAnchorFile() const;
  [[nodiscard]] int resolveKey() const;
  void openQueue(int key);

  int queueId_{-1};
  std::string path_;
  bool owner_{false};
};
