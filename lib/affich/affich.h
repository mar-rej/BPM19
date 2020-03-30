#ifndef _REJ_AFFICH
#define _REJ_AFFICH

#include <MCUFRIEND_kbv.h>

#ifdef _REJ_AFFICH_25//320x240

#define A_ECR1 1
#define A_ECR2 1
#define A_ECR3 2

#define A_BARRETITRE 20
#define A_TITRE 5

#define A_ENTR_X 25
#define A_TEXT_Y 25
#define A_CONS_X 140
#define A_TRIG_X 260
#define A_UNIT_Y 65
#define A_VAL_X_ENTR 35
#define A_VAL_X_CONS 150
#define A_VAL_X_TRIG 270
#define A_VAL_Y 40

#define A_BTN1_X 5
#define A_BTN2_X 85
#define A_BTN3_X 165
#define A_BTN4_X 245
#define A_BTN_Y 140
#define A_BTN5_Y 20
#define A_BTN_LG 70
#define A_BTN_HI 60

#define A_BP_COL1 80
#define A_BP_COL2 135
#define A_BP_COL3 190
#define A_BP_ROW1 75
#define A_BP_ROW2 110
#define A_BP_ROW3 145
#define A_BP_ROW4 180
#define A_BP_LG 50
#define A_BP_HI 30

#define A_PV_LG 160
#define A_PV_HI 135

#define A_REGL_X 2
#define A_REGL_X_VAL 100
#define A_REGL_Y 100
#define A_REGL_CLI 115
#define A_REGL_HI 10

#define A_BARREERR 210
#define A_BARREDBG 225
#define A_BARREDBGVERT 15
#define A_BARREDBGVERT_X 220
#define A_ERR_X 2
#define A_DBG_X 2
#define A_DBG_Y 230
#define A_ERR_VAL_X 54
#define A_ERR_Y 215
#define A_TS_X 225

#endif

#ifdef _REJ_AFFICH_35

#define A_ECR1 1
#define A_ECR2 2
#define A_ECR3 3

#define A_BARRETITRE 30
#define A_TITRE 200
#define A_VERSION 380
#define A_MODE 5
#define A_CONSIGNE 250

#define A_ENTR_X 25
#define A_TEXT_Y 35
#define A_CONS_X 200
#define A_TRIG_X 350
#define A_UNIT_Y 100
#define A_VAL_X_ENTR 50
#define A_VAL_X_CONS 230
#define A_VAL_X_TRIG 380
#define A_VAL_Y 60

#define A_BTN1_X 5
#define A_BTN2_X 125
#define A_BTN3_X 245
#define A_BTN4_X 365
#define A_BTN_Y 200
#define A_BTN5_Y 120
#define A_BTN_LG 110
#define A_BTN_HI 78

#define A_BP_COL1 125
#define A_BP_COL2 205
#define A_BP_COL3 285
#define A_BP_ROW1 120
#define A_BP_ROW2 160
#define A_BP_ROW3 200
#define A_BP_ROW4 240
#define A_BP_LG 80
#define A_BP_HI 40

#define A_PV_LG 240
#define A_PV_HI 159

#define A_REGL_X 2
#define A_REGL_X_VAL 120
#define A_REGL_Y 150
#define A_REGL_CLI 130
#define A_REGL_HI 15

#define A_BARREERR 280
#define A_BARREDBG 300
#define A_BARREDBGVERT 20
#define A_BARREDBGVERT_X 360
#define A_TS_X 370
#define A_DBG_VIE 470
#define A_DBG_X 2
#define A_DBG_Y 305
#define A_ERR_X 2
#define A_ERR_VAL_X 54
#define A_ERR_Y 285

#endif

class rej_affich_bp : public Adafruit_GFX_Button{

    public:
        void bp_menu(Adafruit_GFX *gfx, int position, char *texte,
                     uint16_t couleur = TFT_LIGHTGREY, boolean invert = true);
        void bp_num(Adafruit_GFX *gfx, int position);

};

#endif
