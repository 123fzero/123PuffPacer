#include <furi.h>
#include <gui/gui.h>

int32_t puff_pacer_app(void* p) {
    UNUSED(p);
    FURI_LOG_I("PuffPacer", "Hello from PuffPacer!");
    furi_delay_ms(2000);
    return 0;
}
