/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** MessageQueue
*/

#include "ipc/MessageQueue.hpp"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include "exceptions/Exception.hpp"
#include "utils/ErrorMessage.hpp"

namespace {
struct SysVMessage {
  long mtype;
  plazza::Packet packet;
};

constexpr int kProjectId = 1;
constexpr int kQueuePermissions = 0600;
}  // namespace

void MessageQueue::createAnchorFile() const {
  const std::unique_ptr<FILE, int (*)(FILE*)> file(
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

void MessageQueue::send(const plazza::Packet& packet) {
  SysVMessage msg{.mtype = 1, .packet = packet};
  if (msgsnd(queueId_, &msg, sizeof(plazza::Packet), 0) == -1) {
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMqSendFailed));
  }
}

bool MessageQueue::receive(plazza::Packet& packet, bool blocking) const {
  SysVMessage msg{};
  const int flags = blocking ? 0 : IPC_NOWAIT;
  const ssize_t received =
      msgrcv(queueId_, &msg, sizeof(plazza::Packet), 0, flags);

  if (received == -1) {
    if (errno == ENOMSG || errno == EAGAIN) {
      return false;
    }
    throw plazza::exceptions::Exception(
        std::string(plazza::constants::kMqReceiveFailed));
  }
  packet = msg.packet;
  return true;
}
