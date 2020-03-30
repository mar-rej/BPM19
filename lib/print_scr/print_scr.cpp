/*****************************************************************************
 *  @file     print_scr.cpp
 *  @author   marrej
 *  @brief    Affichage sur l'écran
 *  @version  0.0.A
 *  @history
 *    24/03/2019  0.0.A   Première version
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

#include "print_scr.h"

/******************************************************************************
 *  @brief 
 *    Configuration des couleur de l'écriture
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    uint8_t         Couleur
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_col(MCUFRIEND_kbv *pscr_c_tft, uint8_t pscr_c_type){
  switch (pscr_c_type){
    case PSCR_ERR:
      pscr_c_tft->setTextColor(TFT_WHITE, TFT_RED);
      pscr_c_tft->setTextSize(A_ECR1);
      break;
    case PSCR_ATR:
      pscr_c_tft->setTextColor(TFT_GREEN, TFT_BLACK);
      pscr_c_tft->setTextSize(A_ECR2);
      break;
    case PSCR_WHI2:
      pscr_c_tft->setTextColor(TFT_WHITE, TFT_BLACK);
      pscr_c_tft->setTextSize(A_ECR2);
      break;
    case PSCR_WHI3:
      pscr_c_tft->setTextColor(TFT_WHITE, TFT_BLACK);
      pscr_c_tft->setTextSize(A_ECR3);
      break;
    case PSCR_REGUL:
      pscr_c_tft->setTextColor(TFT_GREEN, TFT_BLACK);
      pscr_c_tft->setTextSize(A_ECR1);
      break;
    default:
      pscr_c_tft->setTextColor(TFT_DARKGREY, TFT_BLACK);
      pscr_c_tft->setTextSize(A_ECR1);
      break;
  }
}

/******************************************************************************
 *  @brief 
 *    Affichage d'une erreur sur l'écran
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    char *          pointeur sur le message de l'erreur
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_err(MCUFRIEND_kbv *pscr_err_tft, char *pscr_err_mess){
  pscr_col(pscr_err_tft, PSCR_ERR);
  pscr_err_tft->setCursor(A_ERR_VAL_X, A_ERR_Y);
  pscr_err_tft->print(pscr_err_mess);
}

/******************************************************************************
 *  @brief 
 *    Affichage des informations dans la zone K
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    cahr *          Message de a afficher
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_k(MCUFRIEND_kbv *pscr_k_tft, char *pscr_k_mess){
  pscr_col(pscr_k_tft, PSCR_INFO);
  pscr_k_tft->setCursor(A_DBG_X, A_DBG_Y);
  pscr_k_tft->print(pscr_k_mess);
}

/******************************************************************************
 *  @brief 
 *    Affichage des coordonées de l'appui sur l'écran tactile
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    uint16_t        coordonées en X
 *    uint16_t        coordonées en Y
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_ts(MCUFRIEND_kbv *pscr_k_tft, uint32_t pscr_pr_ts_x, uint32_t pscr_pr_ts_y){
  pscr_col(pscr_k_tft, PSCR_INFO);
  pscr_k_tft->setCursor(A_TS_X,A_DBG_Y);
  pscr_k_tft->print("x: ");
  pscr_k_tft->print(String(pscr_pr_ts_x));
  pscr_k_tft->print(", y: ");
  pscr_k_tft->print(String(pscr_pr_ts_y));
  pscr_k_tft->print("  ");
}

/******************************************************************************
 *  @brief 
 *   Affichage de la consigne
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    uint16_t        Consigne à afficher
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_mcons(MCUFRIEND_kbv *pscr_mc_tft, uint16_t pscr_mc_cons){
  pscr_mc_tft->writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
  pscr_col(pscr_mc_tft, PSCR_ATR);
  pscr_mc_tft->setCursor(A_REGL_X,A_REGL_Y);
  pscr_mc_tft->print("Consigne=");
  pscr_mc_tft->setCursor(A_REGL_X_VAL,A_REGL_Y);
  pscr_mc_tft->print(String(pscr_mc_cons));
  pscr_mc_tft->print("  ");
}

/******************************************************************************
 *  @brief 
 *   Affichage du mode de fonctionnement
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    char            Mode de fonctionnement
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_mode(MCUFRIEND_kbv *pscr_mode_tft, char pscr_mode_mode){
  pscr_col(pscr_mode_tft, PSCR_ATR);
  pscr_mode_tft->setCursor(A_MODE,5);
  switch (pscr_mode_mode) {
    case MODEF:
      pscr_mode_tft->print("Mode: F");
      break;
    case MODER:
      pscr_mode_tft->print("Mode: R");
      break;
    default:
      pscr_mode_tft->print("Mode: C");
      break;
  }
}

/******************************************************************************
 *  @brief 
 *   Initialisation de l'affichage
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    char          * Version à afficher
 *    uint8_t         Largeur de l'écran
 *    char            Mode de fonctionnement
 *    char          * Message K
 *    char          * Message Erreur
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_init(MCUFRIEND_kbv *pscr_init_tft,
                          char * pscr_init_title,
                          char * pscr_init_ver,
                          uint16_t pscr_init_width,
                          char pscr_mode_mode,
                          char * pscr_init_k_mes,
                          char * pscr_init_err_mes){

  pscr_init_tft->fillScreen(TFT_BLACK);
  
  pscr_mode(pscr_init_tft, pscr_mode_mode);

  pscr_col(pscr_init_tft, PSCR_WHI3);
  pscr_init_tft->setCursor(A_TITRE,0);
  pscr_init_tft->print(pscr_init_title);
  pscr_col(pscr_init_tft, PSCR_INFO);
  pscr_init_tft->setCursor(A_VERSION,5);
  pscr_init_tft->print(pscr_init_ver);
  pscr_init_tft->writeFastHLine(0,A_BARRETITRE,pscr_init_width,TFT_WHITE);

  pscr_col(pscr_init_tft, PSCR_WHI2);
  pscr_init_tft->setCursor(A_CONS_X,A_TEXT_Y);
  pscr_init_tft->print("CONSIGNE: ");

  pscr_init_tft->setCursor(A_CONS_X,A_UNIT_Y);
  pscr_init_tft->print("cm/H2O");

  pscr_init_tft->setCursor(A_ENTR_X,A_TEXT_Y);
  pscr_init_tft->print("ENTREE:");

  pscr_init_tft->setCursor(A_ENTR_X,A_UNIT_Y);
  pscr_init_tft->print("mm/H2O");

  pscr_init_tft->setCursor(A_TRIG_X,A_TEXT_Y);
  pscr_init_tft->print("TRIGGER:");

  pscr_init_tft->setCursor(A_TRIG_X,A_UNIT_Y);
  pscr_init_tft->print("mm/H2O");

  pscr_init_tft->writeFastHLine(0,A_BARREDBG,pscr_init_width,TFT_DARKGREY);
  
  pscr_k(pscr_init_tft, pscr_init_k_mes);
  
  pscr_init_tft->writeFastVLine(A_BARREDBGVERT_X,A_BARREDBG,A_BARREDBGVERT,TFT_DARKGREY);

  pscr_init_tft->writeFastHLine(0,A_BARREERR,pscr_init_width,TFT_WHITE);
  pscr_init_tft->setTextColor(TFT_WHITE, TFT_BLACK);
  pscr_init_tft->setTextSize(A_ECR1);
  pscr_init_tft->setCursor(A_ERR_X,A_ERR_Y);
  pscr_init_tft->print("Erreurs:");
  
  pscr_err(pscr_init_tft, pscr_init_err_mes);
}

/******************************************************************************
 *  @brief 
 *   Affichage d'une valeurs sur l'écran
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *    uint_t          Mode de fonctionnement
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_aff_val(MCUFRIEND_kbv *pscr_av_tft, int32_t pscr_av_val, uint8_t pscr_av_iedent){
  pscr_col(pscr_av_tft, PSCR_WHI3);
  
  switch (pscr_av_iedent){
    case PSCR_IN:
      pscr_av_tft->setCursor(A_VAL_X_ENTR, A_VAL_Y);
      break;
    case PSCR_TRIG:
      pscr_av_tft->setCursor(A_VAL_X_TRIG, A_VAL_Y);
      break;
    case PSCR_CONS:
      pscr_av_tft->setCursor(A_VAL_X_CONS, A_VAL_Y);
      break;
    case PSCR_DAC:
      pscr_av_tft->setCursor(A_VAL_X_TRIG, A_VAL_Y * 2);
      break;
  }
  
  pscr_av_tft->print(pscr_av_val);
  pscr_av_tft->print("   ");
}

/******************************************************************************
 *  @brief 
 *   Affichage du point de vie 
 *  @param[in]
 *    MCUFRIEND_kbv * Pointeur sur l'ecran
 *  @param[out]
 *    void
******************************************************************************/
void print_scr::pscr_life(MCUFRIEND_kbv *pscr_l_tft, uint8_t pscr_l_type){
  
  pscr_col(pscr_l_tft, pscr_l_type);
  pscr_l_tft->setCursor(A_DBG_VIE, A_DBG_Y);
  if (pscr_vie == true){
    pscr_vie = false;
    pscr_l_tft->print(" ");
  } else {
    pscr_vie = true;
    pscr_l_tft->print(".");
  }
}
