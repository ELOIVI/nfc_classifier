#include "card_reader.h"
#include <furi.h>
#include <nfc/nfc.h>   // nfc_alloc, nfc_free, tipus Nfc
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>              // Iso14443_3aData
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller_sync.h>  // la lectura sincrona


// la definicio real del tipus opac, nomes viu aqui
struct CardReader {
    Nfc* nfc;                    // instancia del hardware NFC
    FuriThread* thread;          // el nostre fil de treball (l,escaneig)
    CardFoundCallback callback;  // la funcio de l,app a qui avisem
    void* context;               // dades de l,app per a la callback
    volatile bool running;       // bandera per dir-li al fil que pari, la fiquem volatile pq es pot canviar fora del fil
                                 // daquesta manera volatile li diu que sempre miri la info de mem i no d,un reg
};

CardReader* card_reader_alloc(void) {
    CardReader* reader = malloc(sizeof(CardReader));
    reader->nfc = nfc_alloc();
    reader->thread = NULL;
    reader->callback = NULL;
    reader->context = NULL;
    reader->running = false;

    //inicializamos todos a cero aunq estoy basttante seguro de que el flipper envuelve el malloc
    //para q sea todo 0, a diferencia del malloc de C estandar q devuelve basura
    // de hecho tampoco hace falta comprobar x ejemplo si el malloc da null, el firmware ya deberia parar con un error
    return reader;
}

void card_reader_free(CardReader* reader) {
    nfc_free(reader->nfc);   // primero el contenido
    free(reader);            // despres la caixa
    // importante hacerlo asi pq si se hace al reves no podriamos leer lo de dentro
}

// aquesta funcio corre al fil de treball: escaneja en bucle fins que parem
static int32_t card_reader_worker(void* context) {
    CardReader* reader = context;   // el context del fil es el propi reader

    while(reader->running) {
        Iso14443_3aData data = {};   // caixa buida per als resultats

        // intent de lectura sincrona: bloqueja fins llegir o rendir-se
        Iso14443_3aError error = iso14443_3a_poller_sync_read(reader->nfc, &data);

        if(error == Iso14443_3aErrorNone) {
            // targeta llegida. traiem el flag iso4 del bit 0x20 del SAK
            bool iso4 = (data.sak & 0x20) != 0;
            // avisem l'app amb el SAK i el flag
            reader->callback(data.sak, iso4, reader->context);
        }

        furi_delay_ms(100);   // petita pausa pq descansi
    }

    return 0;
}

void card_reader_start(CardReader* reader, CardFoundCallback callback, void* context) {
    reader->callback = callback;   // guardem a qui hem d'avisar
    reader->context = context;     // i el seu sobre de dades
    reader->running = true;        // aixequem la bandera abans d'arrencar

    // creem el fil: nom, mida de pila, funcio a executar, i context (el reader)
    reader->thread = furi_thread_alloc_ex("CardReaderWorker", 2048, card_reader_worker, reader);
    furi_thread_start(reader->thread);
}

void card_reader_stop(CardReader* reader) {
    reader->running = false;         // demanem al fil que pari
    furi_thread_join(reader->thread); // esperem que acabi de debo
    furi_thread_free(reader->thread); // alliberem el fil
    reader->thread = NULL;
}

