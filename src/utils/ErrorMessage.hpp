/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** Error messages
*/

#pragma once

#include <string_view>

namespace plazza::constants {

constexpr std::string_view kEmptyOrderSegment = "empty order segment";
constexpr std::string_view kEmptyOrder = "empty order";
constexpr std::string_view kInvalidToken =
    "invalid token: expected TYPE SIZE NUMBER";
constexpr std::string_view kUnknownPizzaType = "unknown pizza type: ";
constexpr std::string_view kUnknownPizzaSize = "unknown pizza size: ";
constexpr std::string_view kInvalidNumberFormatExpected =
    "invalid number format (expected x<n>): ";
constexpr std::string_view kQuantityMustBePositive = "quantity must be >= 1: ";
constexpr std::string_view kInvalidNumberFormat = "invalid number format: ";

constexpr std::string_view kThreadCreationFailed = "Failed to create thread";
constexpr std::string_view kMutexInitFailed = "Failed to initialize mutex";
constexpr std::string_view kMutexLockFailed = "Failed to lock mutex";
constexpr std::string_view kMutexTrylockFailed = "Failed to trylock mutex";
constexpr std::string_view kMutexUnlockFailed = "Failed to unlock mutex";
constexpr std::string_view kCondVarInitFailed =
    "Failed to initialize condition variable";
constexpr std::string_view kCondVarWaitFailed =
    "Failed to wait on condition variable";
constexpr std::string_view kCondVarSignalFailed =
    "Failed to signal condition variable";
constexpr std::string_view kCondVarBroadcastFailed =
    "Failed to broadcast condition variable";
constexpr std::string_view kForkFailed = "Failed to fork process";

constexpr std::string_view kMqOpenFailed = "Failed to open message queue";
constexpr std::string_view kMqSendFailed = "Failed to send on message queue";
constexpr std::string_view kMqReceiveFailed =
    "Failed to receive from message queue";

}  // namespace plazza::constants
