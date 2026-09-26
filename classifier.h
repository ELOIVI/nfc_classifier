#pragma once

#include <stdint.h>   // per a uint8_t (el SAK es un byte)
#include <stdbool.h>  // per a bool (el flag ISO14443-4)

typedef enum {
    CardTypeUnknown,
    CardTypeUltralight,
    CardTypeClassic1k,
    CardTypeClassic4k,
    CardTypeDesfire,
} CardType;

// a partir del SAK i del flag ISO14443-4, decideix el tipus
// no depen d,altres arguments externs aixi q la podem falsejar sense hardware
CardType classify(uint8_t sak, bool iso4);