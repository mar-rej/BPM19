/*****************************************************************************
 *  @file     affich.cpp
 *  @author   marrej
 *  @brief    Affichage sur l'écran
 *  @version  0.0.B
 *  @history
 *    24/03/2019  0.0.A   Première version
 *    08/04/2019  0.0.B   Ajout des fonctions d'affichage simple
*****************************************************************************/

/******************************************************************************
 * Variables Arduino Mega/Uno
 *  char          8 bits  unsigned
 *  byte          8 bits  unsigned
 *  int           16 bits signed
 *  unsigned int  16 bits unsigned
 *  long          32 bits signed
 *  unsigned long 32 bits unsigned
 *  Action de calcul (math) avec des long penser à L
 *  float         32 bits avec decimals
 *  double        32 identique float
 *  uint8_t       8 bits unsigned
 *  int8_t        8 bits signed
 *  uint16_t      16 bits unsigned
 *  int16_t       16 bits signed
 *  uint32_t      32 bits unsigned
 *  int32_t       32 bits signed
******************************************************************************/

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#define _REJ_AFFICH_35
#include "affich.h"


/******************************************************************************
 *  @brief 
 *    Affichage des boutons 
 *  @param[in]
 *    void
 *  @param[out]
 *    void
******************************************************************************/
void rej_affich_bp::bp_menu(Adafruit_GFX *gfx, int position, char *texte,
                            uint16_t couleur, boolean invert){
    char tempo[20];

    sprintf(tempo, "%d:%s", position, texte);
    switch (position){
        case 1:
            initButtonUL(gfx, A_BTN1_X, A_BTN_Y, A_BTN_LG, A_BTN_HI,
                         TFT_DARKGREY, TFT_BLACK, couleur, tempo, A_ECR2);
        break;
        case 2:
            initButtonUL(gfx, A_BTN2_X, A_BTN_Y, A_BTN_LG, A_BTN_HI,
                         TFT_DARKGREY, TFT_BLACK, couleur, tempo, A_ECR2);
        break;
        case 3:
            initButtonUL(gfx, A_BTN3_X, A_BTN_Y, A_BTN_LG, A_BTN_HI,
                         TFT_DARKGREY, TFT_BLACK, couleur, tempo, A_ECR2);
        break;
        case 4:
            initButtonUL(gfx, A_BTN4_X, A_BTN_Y, A_BTN_LG, A_BTN_HI,
                         TFT_DARKGREY, TFT_BLACK, couleur, tempo, A_ECR2);
        break;
        case 5:
            initButtonUL(gfx, A_BTN1_X, A_BTN5_Y, A_BTN_LG, A_BTN_HI,
                         TFT_DARKGREY, TFT_BLACK, couleur, tempo, A_ECR2);

    }
    drawButton(invert);
}

/******************************************************************************
 *  @brief 
 *    Affichage des boutons 
 *  @param[in]
 *    void
 *  @param[out]
 *    void
******************************************************************************/
void rej_affich_bp::bp_num(Adafruit_GFX *gfx, int position){
    char chif[3];
    sprintf(chif,"%d",position);
    switch (position){
        case 1:
            initButtonUL(gfx, A_BP_COL1, A_BP_ROW1, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 4:
            initButtonUL(gfx, A_BP_COL1, A_BP_ROW2, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 7:
            initButtonUL(gfx, A_BP_COL1, A_BP_ROW3, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 10:
            sprintf(chif, "C");
            initButtonUL(gfx, A_BP_COL1, A_BP_ROW4, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_RED, chif, A_ECR2);
            drawButton();
        break;
        case 2:
            initButtonUL(gfx, A_BP_COL2, A_BP_ROW1, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 5:
            initButtonUL(gfx, A_BP_COL2, A_BP_ROW2, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 8:
            initButtonUL(gfx, A_BP_COL2, A_BP_ROW3, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 0:
            initButtonUL(gfx, A_BP_COL2, A_BP_ROW4, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 3:
            initButtonUL(gfx, A_BP_COL3, A_BP_ROW1, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 6:
            initButtonUL(gfx, A_BP_COL3, A_BP_ROW2, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 9:
            initButtonUL(gfx, A_BP_COL3, A_BP_ROW3, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_LIGHTGREY, chif, A_ECR2);
            drawButton(true);
        break;
        case 11:
            sprintf(chif,"V");
            initButtonUL(gfx, A_BP_COL3, A_BP_ROW4, A_BP_LG, A_BP_HI,
                         TFT_DARKGREY, TFT_BLACK, TFT_GREEN, chif,A_ECR2);
            drawButton();
        break;
    }
}
