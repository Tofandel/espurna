/*

RELAY MODULE

Copyright (C) 2016-2019 by Xose Pérez <xose dot perez at gmail dot com>

*/

#pragma once

#include <bitset>
#include "rpc.h"
#include "utils.h"

constexpr size_t RELAYS_MAX = 32;

struct RelayMask {

    explicit RelayMask(const String& string) :
        as_string(string),
        as_u32(u32fromString(string))
    {}

    explicit RelayMask(String&& string) :
        as_string(std::move(string)),
        as_u32(u32fromString(as_string))
    {}

    explicit RelayMask(uint32_t value) :
        as_string(std::move(u32toString(value, 2))),
        as_u32(value)
    {}

    explicit RelayMask(std::bitset<RELAYS_MAX> bitset) :
        RelayMask(bitset.to_ulong())
    {}

    RelayMask(String&& string, uint32_t value) :
        as_string(std::move(string)),
        as_u32(value)
    {}

    const String as_string;
    uint32_t as_u32;

};

PayloadStatus relayParsePayload(const char * payload);

bool relayStatus(unsigned char id, bool status, bool report, bool group_report);
bool relayStatus(unsigned char id, bool status);
bool relayStatus(unsigned char id);

void relayToggle(unsigned char id, bool report, bool group_report);
void relayToggle(unsigned char id);

unsigned char relayCount();

constexpr const String& relayPayloadOn();
constexpr const String& relayPayloadOff();
constexpr const String& relayPayloadToggle();

const char* relayPayload(PayloadStatus status);

void relaySetupDummy(size_t size, bool reconfigure = false);

