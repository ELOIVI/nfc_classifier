#include <furi.h>

// etiqueta para identificar los logs
#define TAG "NfcClassifier"  

// funcion de entrada
int32_t nfc_classifier_app(void* p) {
    UNUSED(p);                 // para silenciar el warning?
    FURI_LOG_I(TAG, "missatge");    // deixa constància que hem entrat
    return 0;                  // sortida neta
}