#pragma once

#include <cstdint>

namespace networkProtocol {
    enum Type : uint8_t {
        Error,
        Song,
        Video,
        Text,
        Reply
    };

    /**
     *  Structure that represents a Header network protocol
     */
    struct HeaderProtocol {
        uint32_t magicValue; // 0x42dead42
        uint32_t id; // Chaque message a un identifiant. Une réponse ou une erreur aura le même identifiant que l'appel correspondant.
        uint16_t version;
        uint16_t type;
        uint32_t payloadSize;
        uint32_t action;
        // ... 
    };
}