/*

SYSTEM MODULE

Copyright (C) 2019 by Xose Pérez <xose dot perez at gmail dot com>

*/

#pragma once

#include <cstdint>

uint32_t systemResetReason();
uint8_t systemStabilityCounter();
void systemStabilityCounter(uint8_t count);

void systemCheck(bool stable);
bool systemCheck();

uint32_t systemResetReason();
unsigned char customResetReason();
void customResetReason(unsigned char reason);

void deferredReset(unsigned long delay, unsigned char reason);
bool checkNeedsReset();
