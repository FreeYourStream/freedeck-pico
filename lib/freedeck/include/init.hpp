#include <GFX.hpp>
#include <sd_card.h>
extern GFX *oled[];
extern sd_card_t *pSD;
extern FIL fil;
void init_sdcard();
void init_oleds();
void init_button();
void init_mux();