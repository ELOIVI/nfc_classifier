#pragma once

#include <stdint.h>   // per a uint8_t (el SAK es un byte)
#include <stdbool.h>  // per a bool (el flag ISO14443-4)

//los tipos de targeta
typedef enum {
    CardTypeUnknown,     // per defecte, encara no classificada. La ponemos primero pq al no asignarle valor el campo numerico 
                         //se queda a 0, por lo tanto una sin clasifucar y una desconocida son equivalentes
    CardTypeUltralight,  // Ultralight / NTAG
    CardTypeClassic1k,
    CardTypeClassic4k,
    CardTypeDesfire,
} CardType;


// que se puede hacer con la targeta
typedef enum {
    VerdictUnknown,    // no classificada o desconeguda
    VerdictGreen,      // clonable sencera (sense xifrat real)
    VerdictAmber,      // condicional (Crypto1 trencat, depen de les claus)
    VerdictRed,        // intocable a la practica (xifrat fort)
} Verdict;

// funcio del tipus al veredicte
Verdict assess(CardType type);

// a partir del SAK i del flag ISO14443-4, decideix el tipus
// no depen d,altres arguments externs aixi q la podem falsejar sense hardware
CardType classify(uint8_t sak, bool iso4);