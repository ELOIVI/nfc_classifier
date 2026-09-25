#include <furi.h>
#include <gui/gui.h>

// etiqueta para identificar los logs
#define TAG "NfcClassifier"  


// la funcion que la GUI llama para escribir en la pantalla
static void nfc_classifier_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas); //borrem tot el q hi havia
    canvas_set_font(canvas, FontPrimary); 
    canvas_draw_str(canvas, 2, 12, "NFC Classifier"); //suposo que x=2 i y=12??
}

// cada vez que se pulse un boton la GUI llama a esta funcion
static void nfc_classifier_input_callback(InputEvent* event, void* context) {

    // el contexto es la cola que le hemos entregado antes
    FuriMessageQueue* cola = context;

    //ponemos el evento del boton a la cola
    furi_message_queue_put (cola, event, FuriWaitForever);
}


// funcion de entrada
int32_t nfc_classifier_app(void* p) {
    UNUSED(p);                 // para silenciar el warning?

    //bustia per rebre els esdeveniments de botons
    //pongo un 8 porque no es fisicamente posible pulsar mas de 8 botones a la vez (?)
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    //pido prestado el servicio de la pantalla
    Gui* gui = furi_record_open(RECORD_GUI);

    //reservamos la "ventana"
    ViewPort* view_port= view_port_alloc();

    // i li donem la funcio de dibuixar
    view_port_draw_callback_set(view_port, nfc_classifier_draw_callback, NULL); 

    // registramos el callback de input y se lo damos a la cola como context
    view_port_input_callback_set(view_port, nfc_classifier_input_callback, event_queue);


    // enganxem la finestra a la pantalla en pantalla completa
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

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

    //y dessecuestramos todo lo q nos habíamos robado, importante orden inverso
    gui_remove_view_port(gui, view_port);

    view_port_free(view_port);
    
    furi_record_close(RECORD_GUI);
    
    furi_message_queue_free(event_queue); 


    FURI_LOG_I(TAG, "missatge");    // deixa constància que hem entrat
    return 0;                  // sortida neta
}

