#pragma once

#include <stdint.h>   // uint8_t per al SAK
#include <stdbool.h>  // bool per al flag iso4

// tipo opaco pq el resto de la app no sabe lo que tiene x dentro
// la definicio real (amb Nfc*, NfcScanner*...) viu al .c, amagada.
typedef struct CardReader CardReader;

// molde de la funcion que el reader llama
typedef void (*CardFoundCallback)(uint8_t sak, bool iso4, void* context);

// reservar i alliberar el lector (crea i destrueix les seves tripes NFC)
CardReader* card_reader_alloc(void);
void card_reader_free(CardReader* reader);

// arrencar i parar el sondeig. En arrencar li donem la callback i el context.
void card_reader_start(CardReader* reader, CardFoundCallback callback, void* context);
void card_reader_stop(CardReader* reader);