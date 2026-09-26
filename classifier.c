#include "classifier.h"

// els SAK surten de l'estàndard ISO 14443-3; la taula de cada xip la té NXP
// a la nota AN10833. de moment comparo el byte sencer (0x00, 0x08, 0x18), que
// és el que fa el propi Flipper i ja va bé per a targetes NXP normals.
// la DESFire la trec del flag iso4 i no del SAK a posta: el bit d'ISO14443-4
// és el que de veritat diu si és de capa 4, i a més la EV3 pot portar
// qualsevol SAK, així que per byte no la pillaria.
CardType classify(uint8_t sak, bool iso4) {
    CardType tipus;
    if (iso4) {
        tipus = CardTypeDesfire;
    }
    else {
        switch (sak)
        {
        case (0x00):
            tipus = CardTypeUltralight;
            break;
        
        case (0x08):
            tipus = CardTypeClassic1k;
            break;

        case (0x18):
            tipus = CardTypeClassic4k;
            break;
        
        default:
            tipus = CardTypeUnknown;
            break;
        }
    }
    return tipus;
}