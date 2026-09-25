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

// funcion de entrada
int32_t nfc_classifier_app(void* p) {
    UNUSED(p);                 // para silenciar el warning?
    
    //pido prestado el servicio de la pantalla
    Gui* gui = furi_record_open(RECORD_GUI);

    //reservamos la "ventana"
    ViewPort* view_port= view_port_alloc();

    // i li donem la funcio de dibuixar
    view_port_draw_callback_set(view_port, nfc_classifier_draw_callback, NULL); 

    // enganxem la finestra a la pantalla en pantalla completa
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    //delay pa ver algo
    furi_delay_ms(5000);

    //y dessecuestramos todo lo q nos habíamos robado, importante orden inverso
    gui_remove_view_port(gui, view_port);

    view_port_free(view_port);
    
    furi_record_close(RECORD_GUI);

    FURI_LOG_I(TAG, "missatge");    // deixa constància que hem entrat
    return 0;                  // sortida neta
}

