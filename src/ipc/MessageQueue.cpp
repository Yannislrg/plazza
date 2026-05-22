/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** MessageQueue
*/

#include "ipc/MessageQueue.hpp"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include "Pizza.hpp"
#include "exceptions/Exception.hpp"
#include "ipc/PizzaSerializer.hpp"
#include "utils/Constant.hpp"

namespace {

// NOLINTBEGIN(google-runtime-int,cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays,altera-struct-pack-align)
struct SysVMessage {
  long messageType;
  char data[sizeof(PackedPizza)];
};
// NOLINTEND(google-runtime-int,cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays,altera-struct-pack-align)

// NOLINTNEXTLINE(google-runtime-int)
constexpr long kMessageType = 1;
constexpr int kProjectId = 1;
constexpr int kQueuePermissions = 0600;

}  // namespace

void MessageQueue::createAnchorFile() const {
  const std::unique_ptr<FILE, decltype(&std::fclose)> file(
      std::fopen(path_.c_str(), "ae"), std::fclose);
  if (!file) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMqOpenFailed));
  }
}

int MessageQueue::resolveKey() const {
  const auto key = ftok(path_.c_str(), kProjectId);
  if (key == -1) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMqOpenFailed));
  }
  return key;
}

void MessageQueue::openQueue(int key) {
  if (owner_) {
    const int staleId = msgget(key, 0);
    if (staleId != -1) {
      // NOLINTNEXTLINE(cert-err33-c)
      msgctl(staleId, IPC_RMID, nullptr);
    }
    queueId_ = msgget(key, IPC_CREAT | IPC_EXCL | kQueuePermissions);
  } else {
    queueId_ = msgget(key, 0);
  }
  if (queueId_ == -1) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMqOpenFailed));
  }
}

MessageQueue::MessageQueue(std::string path, Mode mode)
    : path_(std::move(path))
    , owner_(mode == Mode::Create) {
  if (owner_) {
    createAnchorFile();
  }
  openQueue(resolveKey());
}

MessageQueue::~MessageQueue() {
  if (owner_ && queueId_ != -1) {
    // NOLINTNEXTLINE(cert-err33-c)
    msgctl(queueId_, IPC_RMID, nullptr);
    // NOLINTNEXTLINE(cert-err33-c)
    std::remove(path_.c_str());
  }
}

MessageQueue::MessageQueue(MessageQueue&& other) noexcept
    : queueId_(other.queueId_)
    , path_(std::move(other.path_))
    , owner_(other.owner_) {
  other.queueId_ = -1;
  other.owner_ = false;
}

MessageQueue& MessageQueue::operator=(MessageQueue&& other) noexcept {
  if (this != &other) {
    if (owner_ && queueId_ != -1) {
      // NOLINTNEXTLINE(cert-err33-c)
      msgctl(queueId_, IPC_RMID, nullptr);
      // NOLINTNEXTLINE(cert-err33-c)
      std::remove(path_.c_str());
    }
    queueId_ = other.queueId_;
    path_ = std::move(other.path_);
    owner_ = other.owner_;
    other.queueId_ = -1;
    other.owner_ = false;
  }
  return *this;
}

MessageQueue& MessageQueue::operator<<(const Pizza& pizza) {
  SysVMessage msg{};
  msg.messageType = kMessageType;
  const PackedPizza packed = pack(pizza);
  std::ranges::copy(packed, msg.data);
  if (msgsnd(queueId_, &msg, sizeof(msg.data), 0) == -1) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMqSendFailed));
  }
  return *this;
}

bool MessageQueue::operator>>(Pizza& pizza) const {
  SysVMessage msg{};
  const auto received =
      msgrcv(queueId_, &msg, sizeof(msg.data), kMessageType, IPC_NOWAIT);
  if (received == -1) {
    if (errno == ENOMSG || errno == EAGAIN) {
      return false;
    }
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMqReceiveFailed));
  }
  const PackedPizza packed{msg.data[0], msg.data[1]};
  pizza = unpack(packed);
  return true;
}
