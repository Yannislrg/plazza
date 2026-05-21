/*
** EPITECH PROJECT, 2026
** plazza
** File description:
** NotifyFixture
*/

#pragma once

#include <gtest/gtest.h>
#include "concurrency/ConditionVariable.hpp"
#include "concurrency/Mutex.hpp"

class NotifyFixture : public ::testing::Test {
 protected:
  Mutex _mutex;
  ConditionVariable _conditionVariable;
  bool _ready{false};
};
