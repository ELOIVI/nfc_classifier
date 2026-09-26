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

Verdict assess(CardType type) {
    Verdict veredicte;
    switch (type)
    {
    // Ultralight -> VerdictGreen  (sense xifrat, clonable sencera)
    case CardTypeUltralight:
        veredicte = VerdictGreen ;
        break;
    
    // Classic 1K i 4K -> VerdictAmber  (Crypto1 trencat, depèn de les claus)
    //els apilo pq donen el mateix comportament i molo molt
    case CardTypeClassic1k:
    case CardTypeClassic4k:
        veredicte = VerdictAmber;
        break;
    
    // DESFire -> VerdictRed    (AES + auth mútua, no clonable)
    case CardTypeDesfire:
        veredicte = VerdictRed;
        break;

    // qualsevol altre -> VerdictUnknown
    default:
    veredicte = VerdictUnknown;
        break;
    }
    return veredicte;
}