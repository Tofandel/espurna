/*

RFM69 MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if RFM69_SUPPORT

#include "mqtt.h"
#include "rfm69.h"
#include "ws.h"

#define RFM69_PACKET_SEPARATOR ':'

// -----------------------------------------------------------------------------
// Locals
// -----------------------------------------------------------------------------

RFM69Wrap * _rfm69_radio;

struct _node_t {
    unsigned long count = 0;
    unsigned long missing = 0;
    unsigned long duplicates = 0;
    unsigned char lastPacketID = 0;
};

_node_t _rfm69_node_info[RFM69_MAX_NODES];
unsigned char _rfm69_node_count;
unsigned long _rfm69_packet_count;

// -----------------------------------------------------------------------------
// WEB
// -----------------------------------------------------------------------------

#if WEB_SUPPORT

void _rfm69WebSocketOnVisible(JsonObject& root) {
    JsonObject& modules = root["_modules"];
    modules["rfm69"] = 1;
}

void _rfm69WebSocketOnConnected(JsonObject& root) {
    JsonObject& rfm69 = root.createNestedObject("rfm69");

    rfm69["topic"] = getSetting("rfm69Topic", RFM69_DEFAULT_TOPIC);
    rfm69["_packetCount"] = _rfm69_packet_count;
    rfm69["_nodeCount"] = _rfm69_node_count;
    rfm69["_max"] = RFM69_MAX_TOPICS;

    JsonArray& schema = rfm69.createNestedArray("_schema");

    schema.add("node");
    schema.add("key");
    schema.add("topic");

    JsonArray& list = rfm69.createNestedArray("list");

    for (unsigned char i=0; i<RFM69_MAX_TOPICS; i++) {
        auto node = getSetting({"rfm69node", i}, 0);
        if (0 == node) break;
        JsonObject& node = list.createNestedArray();
        node.add(node);
        node.add(getSetting({"rfm69Key", i}));
        node.add(getSetting({"rfm69topic", i}));
    }

}

bool _rfm69WebSocketOnKeyCheck(const char * key, JsonVariant& value) {
    if (strncmp(key, "rfm69", 5) == 0) return true;
//    if (strncmp(key, "node", 4) == 0) return true;
//    if (strncmp(key, "key", 3) == 0) return true;
//    if (strncmp(key, "topic", 5) == 0) return true;
    return false;
}

uint8_t _rfm69WebSocketOnAction(uint32_t client_id, const char * action, JsonObject& data, JsonObject& res) {
    if (strcmp(action, "clear-counts") == 0) {
        _rfm69Clear();
        return 1;
    }
    return 0;
}


#endif // WEB_SUPPORT

void _rfm69CleanNodes(unsigned char num) {

    // Look for the last defined node
    unsigned char id = 0;
    while (id < num) {
        if (0 == getSetting({"rfm69Node", id}, 0)) break;
        if (!getSetting({"rfm69Key", id}).length()) break;
        if (!getSetting({"rfm69Topic", id}).length()) break;
        ++id;
    }

    // Delete all other settings
    while (id < SETTINGS_MAX_LIST_COUNT) {
        delSetting({"rfm69Node", id});
        delSetting({"rfm69Key", id});
        delSetting({"rfm69Topic", id});
        ++id;
    }

}

void _rfm69Configure() {
    _rfm69CleanNodes(RFM69_MAX_TOPICS);
}

// -----------------------------------------------------------------------------
// Radio
// -----------------------------------------------------------------------------

void _rfm69Debug(const char * level, packet_t * data) {

    DEBUG_MSG_P(
        PSTR("[RFM69] %s: messageID:%05d senderID:%03d targetID:%03d packetID:%03d rssi:%-04d key:%s value:%s\n"),
        level,
        data->messageID,
        data->senderID,
        data->targetID,
        data->packetID,
        data->rssi,
        data->key,
        data->value
    );

}

void _rfm69Process(packet_t * data) {

    // Is node beyond RFM69_MAX_NODES?
    if (data->senderID >= RFM69_MAX_NODES) return;

    // Count seen nodes
    if (_rfm69_node_info[data->senderID].count == 0) ++_rfm69_node_count;

    // Detect duplicates and missing packets
    // packetID==0 means device is not sending packetID info
    if (data->packetID > 0) {
        if (_rfm69_node_info[data->senderID].count > 0) {

            unsigned char gap = data->packetID - _rfm69_node_info[data->senderID].lastPacketID;

            if (gap == 0) {
                _rfm69_node_info[data->senderID].duplicates = _rfm69_node_info[data->senderID].duplicates + 1;
                //_rfm69Debug("DUP", data);
                return;
            }

            if ((gap > 1) && (data->packetID > 1)) {
                _rfm69_node_info[data->senderID].missing = _rfm69_node_info[data->senderID].missing + gap - 1;
                DEBUG_MSG_P(PSTR("[RFM69] %u missing packets detected\n"), gap - 1);
            }
        }

    }

    _rfm69Debug("OK ", data);

    _rfm69_node_info[data->senderID].lastPacketID = data->packetID;
    _rfm69_node_info[data->senderID].count = _rfm69_node_info[data->senderID].count + 1;

    // Send info to websocket clients
    {
        char buffer[200];
        snprintf_P(
            buffer,
            sizeof(buffer) - 1,
            PSTR("{\"nodeCount\": %d, \"packetCount\": %lu, \"packet\": {\"senderID\": %u, \"targetID\": %u, \"packetID\": %u, \"key\": \"%s\", \"value\": \"%s\", \"rssi\": %d, \"duplicates\": %d, \"missing\": %d}}"),
            _rfm69_node_count, _rfm69_packet_count,
            data->senderID, data->targetID, data->packetID, data->key, data->value, data->rssi,
            _rfm69_node_info[data->senderID].duplicates , _rfm69_node_info[data->senderID].missing);
        wsSend(buffer);
    }

    // If we are the target of the message, forward it via MQTT, otherwise quit
    if (!RFM69_PROMISCUOUS_SENDS && (RFM69_GATEWAY_ID != data->targetID)) return;

    // Try to find a matching mapping
    for (unsigned char i=0; i<RFM69_MAX_TOPICS; i++) {
        auto node = getSetting({"node", i}, 0);
        if (0 == node) break;
        if ((node == data->senderID) && (getSetting({"rfm69Key", i}).equals(data->key))) {
            mqttSendRaw((char *) getSetting({"rfm69Topic", i}).c_str(), (char *) String(data->value).c_str());
            return;
        }
    }

    // Mapping not found, use default topic
    String topic = getSetting("rfm69Topic", RFM69_DEFAULT_TOPIC);
    if (topic.length() > 0) {
        topic.replace("{node}", String(data->senderID));
        topic.replace("{key}", String(data->key));
        mqttSendRaw((char *) topic.c_str(), (char *) String(data->value).c_str());
    }

}

void _rfm69Loop() {

    if (_rfm69_radio->receiveDone()) {

        uint8_t senderID = _rfm69_radio->SENDERID;
        uint8_t targetID = _rfm69_radio->TARGETID;
        int16_t rssi = _rfm69_radio->RSSI;
        uint8_t length = _rfm69_radio->DATALEN;
        char buffer[length + 1];
        strncpy(buffer, (const char *) _rfm69_radio->DATA, length);
        buffer[length] = 0;

        // Do not send ACKs in promiscuous mode,
        // we want to listen without being heard
        if (!RFM69_PROMISCUOUS) {
            if (_rfm69_radio->ACKRequested()) _rfm69_radio->sendACK();
        }

        uint8_t parts = 1;
        for (uint8_t i=0; i<length; i++) {
            if (buffer[i] == RFM69_PACKET_SEPARATOR) ++parts;
        }

        if (parts > 1) {

            char sep[2] = {RFM69_PACKET_SEPARATOR, 0};

            uint8_t packetID = 0;
            char * key = strtok(buffer, sep);
            char * value = strtok(NULL, sep);
            if (parts > 2) {
                char * packet = strtok(NULL, sep);
                packetID = atoi(packet);
            }

            packet_t message;

            message.messageID = ++_rfm69_packet_count;
            message.packetID = packetID;
            message.senderID = senderID;
            message.targetID = targetID;
            message.key = key;
            message.value = value;
            message.rssi = rssi;

            _rfm69Process(&message);

        }

    }

}

void _rfm69Clear() {
    for(unsigned int i=0; i<RFM69_MAX_NODES; i++) {
        _rfm69_node_info[i].duplicates = 0;
        _rfm69_node_info[i].missing = 0;
        _rfm69_node_info[i].count = 0;
    }
    _rfm69_node_count = 0;
    _rfm69_packet_count = 0;
}

// -----------------------------------------------------------------------------
// RFM69
// -----------------------------------------------------------------------------

void rfm69Setup() {

    delay(10);

    _rfm69Configure();

    _rfm69_radio = new RFM69Wrap(RFM69_CS_PIN, RFM69_IRQ_PIN, RFM69_IS_RFM69HW, digitalPinToInterrupt(RFM69_IRQ_PIN));
    _rfm69_radio->initialize(RFM69_FREQUENCY, RFM69_NODE_ID, RFM69_NETWORK_ID);
    _rfm69_radio->encrypt(RFM69_ENCRYPTKEY);
    _rfm69_radio->promiscuous(RFM69_PROMISCUOUS);
    _rfm69_radio->enableAutoPower(0);
    if (RFM69_IS_RFM69HW) _rfm69_radio->setHighPower();

    DEBUG_MSG_P(PSTR("[RFM69] Worning at %u MHz\n"), RFM69_FREQUENCY == RF69_433MHZ ? 433 : RFM69_FREQUENCY == RF69_868MHZ ? 868 : 915);
    DEBUG_MSG_P(PSTR("[RFM69] Node %u\n"), RFM69_NODE_ID);
    DEBUG_MSG_P(PSTR("[RFM69] Network %u\n"), RFM69_NETWORK_ID);
    DEBUG_MSG_P(PSTR("[RFM69] Promiscuous mode %s\n"), RFM69_PROMISCUOUS ? "ON" : "OFF");

    #if WEB_SUPPORT
        wsRegister()
            .onVisible(_rfm69WebSocketOnVisible)
            .onConnected(_rfm69WebSocketOnConnected)
            .onAction(_rfm69WebSocketOnAction)
            .onKeyCheck(_rfm69WebSocketOnKeyCheck);
    #endif

    // Main callbacks
    espurnaRegisterLoop(_rfm69Loop);
    espurnaRegisterReload(_rfm69Configure);

}

#endif // RFM69_SUPPORT
