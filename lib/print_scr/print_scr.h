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

#ifndef _PRINT_SCR
#define _PRINT_SCR

#include <Adafruit_GFX.h>
#define _REJ_AFFICH_35
#include <affich.h>
#include <MCUFRIEND_kbv.h>


// Indentation de l'affichage des valeur
#define PSCR_IN     1
#define PSCR_TRIG   2
#define PSCR_CONS   3
#define PSCR_DAC    4

// type de couleur d'affichage
#define PSCR_INFO   1
#define PSCR_ERR    2
#define PSCR_ATR    3
#define PSCR_WHI2   4
#define PSCR_WHI3   5
#define PSCR_REGUL  6

// Mode de fonctionnement
#define MODEC     1
#define MODEF     2
#define MODER     3

class print_scr 
{

  public:
    void pscr_err(MCUFRIEND_kbv *pscr_err_tft, char *pscr_err_mess);
    void pscr_k(MCUFRIEND_kbv *pscr_k_tft, char *pscr_k_mess);
    void pscr_ts(MCUFRIEND_kbv *pscr_k_tft, uint32_t pscr_pr_ts_x, uint32_t pscr_pr_ts_y);
    void pscr_mcons(MCUFRIEND_kbv *pscr_mc_tft, uint16_t pscr_mc_cons);
    void pscr_mode(MCUFRIEND_kbv *pscr_mode_tft, char pscr_mode_mode);
    void pscr_init(MCUFRIEND_kbv *pscr_init_tft, char * pscr_init_title, char * pscr_init_ver, uint16_t pscr_init_width,
                   char pscr_mode_mode, char * pscr_init_k_mes, char * pscr_init_err_mes);
    void pscr_aff_val(MCUFRIEND_kbv *pscr_av_tft, int32_t pscr_av_val, uint8_t pscr_av_iedent);
    void pscr_life(MCUFRIEND_kbv *pscr_l_tft, uint8_t pscr_l_type);

  private:
    void pscr_col(MCUFRIEND_kbv *pscr_c_tft, uint8_t pscr_c_type);
    bool pscr_vie = true;

};

#endif
