#pragma once

#include <cstdint>

namespace networkProtocol {
    enum Type : uint8_t {
        Song,
        Text,
        Call,
        EndCall,
        Contacts
    };

    /**
     *  Structure that represents a Header network protocol
     */
    struct HeaderProtocol {
        uint32_t magicValue; // 0x42dead42
        uint8_t id; // Chaque message a un identifiant. Une réponse ou une erreur aura le même identifiant que l'appel correspondant.
        uint32_t size;
    };
}