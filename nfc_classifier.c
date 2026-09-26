#include <furi.h>
#include <gui/gui.h>
#include "classifier.h"
#include "card_reader.h"  

// etiqueta para identificar los logs
#define TAG "NfcClassifier"  

//ifnormacio que tenim del xip actual
// struct con un solo campo de momento no me juzguen
typedef struct {
    CardType type;
    Verdict verdict;
} CardInfo;



// la funcion que la GUI llama para escribir en la pantalla
static void nfc_classifier_draw_callback(Canvas* canvas, void* context) {
    CardInfo* info = context; //recuperem les dades
    canvas_clear(canvas); //borrem tot el q hi havia
    canvas_set_font(canvas, FontPrimary); 

    canvas_draw_str(canvas, 2, 10, "NFC Classifier"); 

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "Tipus:");   // etiqueta fixa

    switch (info->type)
    {
    case CardTypeUltralight:
        canvas_draw_str(canvas, 27, 25, "Ultralight / NTAG");
        break;
    case CardTypeClassic1k:
        canvas_draw_str(canvas, 27, 25, "Classic1k");
        break;
    case CardTypeClassic4k:
        canvas_draw_str(canvas, 27, 25, "Classic4k");
        break;
    case CardTypeDesfire:
        canvas_draw_str(canvas, 27, 25, "Desfire");
        break;
    
    default:
        canvas_draw_str(canvas, 27, 25, "Desconeguda");
        break;
    }

    canvas_draw_str(canvas, 2, 35, "Estat:");   // etiqueta fixa
    switch(info->verdict) {
        case VerdictGreen:
            canvas_draw_str(canvas, 28, 35, "Clonable");
            break;

        case VerdictAmber:
            canvas_draw_str(canvas, 28, 35, "Condicional");
            break;

        case VerdictRed:
            canvas_draw_str(canvas, 28, 35, "Intocable");
            break;
            
        default:
            canvas_draw_str(canvas, 28, 35, "?");
        break;
    }


}

// cada vez que se pulse un boton la GUI llama a esta funcion
static void nfc_classifier_input_callback(InputEvent* event, void* context) {

    // el contexto es la cola que le hemos entregado antes
    FuriMessageQueue* cola = context;

    //ponemos el evento del boton a la cola
    furi_message_queue_put (cola, event, FuriWaitForever);
}

// TEMPORAL: de moment nomes registrem el SAK que llegim
static void on_card_found(uint8_t sak, bool iso4, void* context) {
    UNUSED(context);
    FURI_LOG_I(TAG, "targeta! sak=%02X iso4=%d", sak, iso4);
}

// funcion de entrada
int32_t nfc_classifier_app(void* p) {
    UNUSED(p);                 // para silenciar el warning 

    // dades falses per provar sense NFC
    uint8_t fake_sak = 0x08;
    bool fake_iso4 = false;
    // de moment fixem el tipus a mà, per provar sense NFC
    CardType t = classify(fake_sak, fake_iso4);
    CardInfo card = { .type = t, .verdict = assess(t) };  

    //bustia per rebre els esdeveniments de botons
    //pongo un 8 porque me apetece, un limite comodo de eventos acumulados en la cola,
    //es imposible que se llene, a menos que venga una mega rafaga
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    //pido prestado el servicio de la pantalla
    Gui* gui = furi_record_open(RECORD_GUI);

    //reservamos la "ventana"
    ViewPort* view_port= view_port_alloc();

    // i li donem la funcio de dibuixar
    view_port_draw_callback_set(view_port, nfc_classifier_draw_callback, &card); 

    // registramos el callback de input y se lo damos a la cola como context
    view_port_input_callback_set(view_port, nfc_classifier_input_callback, event_queue);


    // enganxem la finestra a la pantalla en pantalla completa
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // arrenquem el lector: li donem la nostra callback (l'app decideix que fer)
    CardReader* reader = card_reader_alloc();
    card_reader_start(reader, on_card_found, NULL); 

    //delay pa ver algo
    //furi_delay_ms(5000);
    //Ahora, en vez de un delay de 5 segundos, pongo un bucle de eventos
    //Basicamente con wait furi_message_queue_get es decirle que espere hasta que llegue algo al buzon.
    //El FuriWaitForever, dice que puede esperar sin ningun tipo de prisa
    //cuando un evento llega lo copia a la variable event y sigue. 
    // si la pulsacion es corta y además es de tirar para atras, salimos y la app se desmonta
    // si es cualquier otra tecla, no se cumplirá el if y por tanto el bucle se queda esperando otra tecla
    InputEvent event;
    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if (event.type == InputTypeShort && event.key == InputKeyBack) {
            break;
        }
    }

    // (just despres del while, abans de desmuntar la GUI)
    card_reader_stop(reader);
    card_reader_free(reader);

   
    //y dessecuestramos todo lo q nos habíamos robado, importante orden inverso
    gui_remove_view_port(gui, view_port);

    view_port_free(view_port);
    
    furi_record_close(RECORD_GUI);
    
    furi_message_queue_free(event_queue); 


    FURI_LOG_I(TAG, "app tancada");    // deixa constància que hem entrat
    return 0;                  // sortida neta
}

