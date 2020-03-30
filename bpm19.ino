/*****************************************************************************
 *  @file     bpm19.ino
 *  @author   marrej
 *  @brief    Pressure management software for mask
 *  @version  0.2.C
 *  @history
 *    24/03/2019  0.2.A   marrej  Add PWM
 *    22/06/2019  0.2.B   marrej  Screen modification
 *    15/11/2019  0.2.C   marrej  Initialisation modification
 *  @todo
 *    Change var name
 *    Change Function name
 *    Change layout of the code
 *    Save different Kp, Ki and Kd for different mode or hardware
 *    The calibration menu on special menu
 *****************************************************************************/
/*****************************************************************************
 * Config Kx 24/04/2019:
 * Kp = 10
 * Ki = 1
 * Kd = 0
 *****************************************************************************/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>

//#include <TouchScreen_kbv.h> //GUI Slice
//#include <Adafruit_MCP4725.h>

#include <Adafruit_GFX.h> //Adafruit_gfx libraries
#include <MCUFRIEND_kbv.h> //MCUFRIEND_kbv
#include <Keypad.h> // Keypad
#include <TimerOne.h> // TimerOne

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

//#define _REJ_AFFICH_25
#define _REJ_AFFICH_35
#include <affich.h>
#include <majshell.h>
#include <print_scr.h>
#include <majMCP4725.h>

/*  SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MOSI - pin 51 on Arduino Mega
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 50 on Arduino Mega
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 52 on Arduino Mega
 ** CS - pin 10 on Arduino Uno/Duemilanove/Diecimila
 ** CS - pin 53 on Arduino Mega
*/

#define PORTRAIT  0
#define LANDSCAPE 1
#define ORIENTATION  LANDSCAPE

// petit écran 450 MINPRESSURE
// grand écran 200 MINPRESSURE
#define MINPRESSURE 450
#define MAXPRESSURE 1000

#define NOERR     false
#define WIERR     true

#define NR_BTN_TS 10

#define NRMES 10 //nombre de mesures pour faire la moyenne

#define PRINCIPAL 1
#define MCONSIGNE 2
#define MCONFKX   3
#define CFG_KP    4
#define CFG_KI    5
#define CFG_KD    6
#define MMODE     7
#define MCONFIG   8
#define MFREQ     9
#define CFG_FREQ  10
#define CFG_OFF   11

byte mode = MODEC;

#define PWM       45
byte cde_ret;

// Version
#define TITRE "BPM19"
#define VERSION "V0.2.C"
char info[20];
char ver_aff[10];

//Écran
MCUFRIEND_kbv tft;
int dispx;
int dispy;

//Tactile 2,4"
//const int XP = 9;
//const int YP = A2;
//const int XM = A3;
//const int YM = 8;
//Tactile 3,2"
#define XP 6
#define YP A1
#define XM A2
#define YM 7
TouchScreen_kbv ts(XP, YP, XM, YM, 300);
TSPoint_kbv tp;
bool btn_press;
unsigned long pos_x;
unsigned long pos_y;
int x[NR_BTN_TS];
int y[NR_BTN_TS];

//Boutons
rej_affich_bp btn_1;
rej_affich_bp btn_2;
rej_affich_bp btn_3;
rej_affich_bp btn_4;
//rej_affich_bp btn_5;

rej_affich_bp clv_1;
rej_affich_bp clv_2;
rej_affich_bp clv_3;
rej_affich_bp clv_4;
rej_affich_bp clv_5;
rej_affich_bp clv_6;
rej_affich_bp clv_7;
rej_affich_bp clv_8;
rej_affich_bp clv_9;
rej_affich_bp clv_c;
rej_affich_bp clv_0;
rej_affich_bp clv_v;

// Définition du clavier
#define ROWS 4
#define COLS 3

#define L1 46
#define L2 44
#define L3 42
#define L4 40
#define C1 38
#define C2 36
#define C3 34

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rows_pin[ROWS] = {L1, L2, L3, L4};
byte cols_pin[COLS] = {C1, C2, C3};

char touche;

Keypad clavier(makeKeymap(keys), rows_pin, cols_pin, ROWS, COLS);

int menu;

//Control du moteur
int kx_tp;
int kp = 10; //750
int ki = 1; //1
int kd = 1; //5
int consigne = 0;
int consignem = 0;
int consigne_off = 0;
int aff_consigne = 0;
int out_dac = 0;
int aff_dac = 0;
bool regul;
float erreur = 0;
float s_erreur = 0;
float d_erreur = 0;
float p_erreur = 0;

#define NR_TIMER1_1_MIN   1200
bool m_freq_tier = true;
int m_freq = 400;
int modef_nr_timer = (int) (NR_TIMER1_1_MIN / m_freq);
int modef_nr_off = modef_nr_timer * 2 / 3;
int modef_dec_timer = modef_nr_timer;
int mode_c = 0;

// Gestion des erreurs
char last_err[255] = "";
char k_aff[255] = "";

//constantes entrees analogiques
#define ANA_IN    A14
#define ANA_TRIG  A15

// position dans le tableau
byte index_mesure = 0;

//Structure de mesures
struct mes {
    unsigned int mesure;
    unsigned long temps_mesure;
};

//tableau de mesures
struct mes val_in[NRMES];
struct mes val_trig[NRMES];

//résultat en mm/H2O
long mm_h2o_in;
long mm_h2o_trig;
long aff_in;
long aff_trig;

//calibration du zero
long zero_in = 0;
long zero_trig = 0;

// Pilotage en tension
majMCP4725 dac_jma;
//Adafruit_MCP4725 dac;
byte code_ret;

//interval entre 2 affichage
unsigned long interval = 500;

// Affichage des mesures
bool affich = false;
char ligne1[32];
unsigned long tps_affich = 0;
unsigned long tps_IHM1 = 0;
unsigned long tps_IHM2 = 0;
unsigned long tps_vie = 0;

//SD
bool save = false;

//état de la vanne
unsigned int vanne_pos = 0;

// Affichage sur l'écran
print_scr screen;

void setup()
{
  // réglage de la fréquence du PWM sur la broche 45 (Timer5)
  TCCR5B = (TCCR5B & 0xF8) | 0x03; // diviseur par 8 soit 3.921KHz
  // diviseur par 1 soit 31.372KHz
  // diviseur par 8 soit 3.921KHz
  // diviseur par 64 soit 490Hz défault
  // diviseur par 256 soit 122.5KHz
  // diviseur par 1024 soit 30.63KHz

  // Initialisation du DAC
  // initialisation de l'I2C par defaut
  Wire.begin();
  dac_jma.begin(0x60);
  out_dac = 0;
  pinMode(PWM, OUTPUT);
  analogWrite(PWM, 0);

  //dac.setVoltage(out_dac, false);
  dac_jma.setVoltage(out_dac, &code_ret);

  // Initialisation de la LS
  Serial.begin(115200);

  // Initialisation du shell
  sprintf(info, "%s", TITRE);
  sprintf(info, "%s %s", info, VERSION);
  shell_init(shell_reader, shell_writer, info);

  // Add commands to the shell
  shell_register(cmd_affich, "start or stop mesure", "0-1", "affich");
  shell_register(cmd_cal, "calibrate 0", "NA", "calib");
  shell_register(cmd_tps, "time to print", "100-10000", "temps");
  shell_register(cmd_vanne, "tension de pilotage de la vanne", "0-5000", "vanne");
  shell_register(cmd_regul, "pression de consigne (cm/H2O)", "0-50", "consigne");
  shell_register(cmd_save, "Écriture sur la SD", "0-1", "save");
  shell_register(cmd_kp, "changement de Kp, ki, kd", "0-4000", "kp");
  shell_register(cmd_ki, "changement de Kp, ki, kd", "0-4000", "ki");
  shell_register(cmd_kd, "changement de Kp, ki, kd", "0-4000", "kd");
  shell_register(cmd_modef, "Affichage des paramètre modeF", "", "modef");

  // Initialisation de l'écran
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(ORIENTATION);
  dispx = tft.width();
  dispy = tft.height();

  // Initialisation de l'affichage
  sprintf(info, "%s", TITRE);
  sprintf(ver_aff, "%s", VERSION);
  screen.pscr_init(&tft, info, ver_aff, dispx, mode, k_aff, last_err);
  affich_menu();
  k_print();

  // Initialisation de la carte µSD
  if (!SD.begin()) {
    sprintf(last_err,"Impossible d'initialiser la carte SD");
    screen.pscr_err(&tft, last_err);
  }

  //Initialisation des variables du TS
  init_var();

  //initialisation des valeurs mesurées
  do {
    mesure();
  } while (index_mesure != 0);

  // réglage du timer 1 toute les 50ms
  Timer1.initialize(50000L);
  Timer1.attachInterrupt(timer_1);
  Timer1.start();

  //calibration du zero
  cal();

}

void mesure()
{

  val_trig[index_mesure].mesure = analogRead(ANA_TRIG);
  val_trig[index_mesure].temps_mesure = millis();
  val_trig[index_mesure].mesure = analogRead(ANA_TRIG);
  val_trig[index_mesure].temps_mesure = millis();
  val_in[index_mesure].mesure = analogRead(ANA_IN);
  val_trig[index_mesure].temps_mesure = millis();
  val_in[index_mesure].mesure = analogRead(ANA_IN);
  val_trig[index_mesure].temps_mesure = millis();
  index_mesure++;

  if (index_mesure == NRMES){
    index_mesure = 0;
  }
}

/******************************************************************************
 *  @brief
 *    Calcul de l'erreur
 *  @param[in]
 *    void
 * @param[out]
 *    void
 *  @todo
 *    void
******************************************************************************/
void calc_err(void){

  if (mm_h2o_trig < 0){
    erreur = consigne * 10 - mm_h2o_trig;
  } else {
    erreur = consigne * 10 - mm_h2o_in;
  }

  if (erreur == 0){
    mode_c++;
  } else {
    mode_c = 0;
  }
  /*if (mm_h2o_in < (consigne * 5)){
    s_erreur = 0;
    p_erreur = 0;
  }*/

  s_erreur += erreur;
  //s_erreur += erreur;
  d_erreur = erreur - p_erreur;
  p_erreur = erreur;

  //calcul de la commande
  erreur = kp * erreur + ki * s_erreur + kd * d_erreur;

  // Vérification de la commande
  if (erreur < 10){
    out_dac = 10;
  } else {
    if (erreur > 3800){
      out_dac = 3800;
    } else {
      out_dac = (int) erreur;
    }
  }
}

/******************************************************************************
 *  @brief
 *    Tache régulière du timer toutes les 50ms
 *  @param[in]
 *    void
 * @param[out]
 *    void
******************************************************************************/
void timer_1(void){

  digitalWrite(23, LOW);
  interrupts();
  mesure();
  noInterrupts();
  moyenne();

  if (regul == false){
    return;
  }

  if (mode == MODEF){
    // Voir à modifier les Kx²
    modef_dec_timer --;

    if (modef_dec_timer == modef_nr_off){
      consigne = consigne_off;
      digitalWrite(22, LOW);
    }

    if (modef_dec_timer < 1){
      modef_dec_timer = modef_nr_timer;
      consigne = consignem;
      digitalWrite(22, HIGH);
    }
  }

  calc_err();

  if ((mode == MODEC) && (mode_c >= 10)){
    regul = false;
  }

  interrupts();
  //if (mode != MODER){
    dac_jma.setVoltage(out_dac, &code_ret);
    analogWrite(PWM, out_dac / 16);
  //}
  digitalWrite(23, HIGH);
}

/******************************************************************************
 *  @brief
 *    Fonction de la boucle principale du programme Arduino
 *  @param[in]
 *    void
 * @param[out]
 *    void
******************************************************************************/
void loop(void){
  // tache du shell
  shell_task();

  // tache de l'IHM
  ihm_task();

  // tache d'affichage
  if(true == affiche_task()){
    sprintf(last_err,"Problème d'affichage");
    screen.pscr_err(&tft, last_err);
  }
}

/**
 * Function to read data from serial port
 * Functions to read from physical media should use this prototype:
 * int my_reader_function(char * data)
 */
int shell_reader(char * data)
{
  // Wrapper for Serial.read() method
  if (Serial.available()) {
    *data = Serial.read();
    return 1;
  }
  return 0;
}

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype:
 * void my_writer_function(char data)
 */
void shell_writer(char data)
{
  // Wrapper for Serial.write() method
  Serial.write(data);
}

/******************************************************************************
 *  @brief
 *   Mise en forme de l'information Kx pour affichage
 *  @param[in]
 *    void
 *  @param[out]
 *    void
******************************************************************************/
void k_print(void){
  sprintf(k_aff, "Kp:%d,Ki:%d,Kd:%d,size:%dx%d,Freq:%dcp/min,Alt:", kp, ki, kd, dispx, dispy, m_freq);
  if (m_freq_tier == true){
    sprintf(k_aff, "%s1/3 ", k_aff);
  } else {
    sprintf(k_aff, "%s1/4 ", k_aff);
  }
  screen.pscr_k(&tft, k_aff);
}

// Lecture de la dalle tactile
bool readResistiveTouch(void)
{
  int index_var;
  bool sortie;

  index_var = 0;
  sortie = false;

  do{
    tp = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);  //because TFT control pins
    digitalWrite(XM, HIGH);
    //Grand Écran
    if (ORIENTATION == PORTRAIT){
      x[index_var] = map(tp.x, 136, 907, 0, tft.width());
      y[index_var] = map(tp.y, 128, 934, 0, tft.height());
    } else {
      x[index_var] = map(tp.x, 934, 128, 0, 480);
      y[index_var] = map(tp.y, 136, 907, 0, 320);
    }
    /*//Petit Écran
    if (ORIENTATION == PORTRAIT){
      x[index_var] = map(tp.x, 164, 928, 0, tft.width());
      y[index_var] = map(tp.y, 198, 934, 0, tft.height());
    } else {
      x[index_var] = map(tp.x, 927, 202, 0, 320);
      y[index_var] = map(tp.y, 931, 163, 0, 240);
    }*/
    if ((tp.z < MINPRESSURE) || (tp.z > MAXPRESSURE)){
      return(false);
    }
    index_var++;
    if (index_var == NR_BTN_TS){
      sortie = true;
    }
  }while(sortie == false);
  pos_x = 0;
  pos_y = 0;
  for (index_var = 0; index_var < NR_BTN_TS; index_var++){
    pos_x = pos_x + x[index_var];
    pos_y = pos_y + y[index_var];
    x[index_var] = 0;
    y[index_var] = 0;
  }
  pos_x = pos_x / NR_BTN_TS;
  pos_y = pos_y / NR_BTN_TS;
  screen.pscr_ts(&tft, pos_x, pos_y);
  return(true);
}

//Affichage des boutons config
void kx_bp()
{
  char var[4];
  sprintf(var,"KP");
  btn_1.bp_menu(&tft, 1, var);
  sprintf(var,"KI");
  btn_2.bp_menu(&tft, 2, var);
  sprintf(var,"KD");
  btn_3.bp_menu(&tft, 3, var);
  sprintf(var,"Exit");
  btn_4.bp_menu(&tft, 4, var, TFT_GREEN, false);
}

//Affichage des boutons config
void config_bp()
{
  char var[4];
  sprintf(var,"Kx");
  btn_1.bp_menu(&tft, 1, var);
  sprintf(var,"Mode");
  btn_2.bp_menu(&tft, 2, var);
  sprintf(var,"Freq");
  btn_3.bp_menu(&tft, 3, var);
  sprintf(var,"Exit");
  btn_4.bp_menu(&tft, 4, var, TFT_GREEN, false);
}

//Affichage des boutons +-ok
void affich_bp()
{
  char var[2];
  sprintf(var,"-");
  btn_2.bp_menu(&tft, 2, var, TFT_GREEN, false);
  sprintf(var,"+");
  btn_3.bp_menu(&tft, 3, var, TFT_GREEN, false);
  sprintf(var,"OK");
  btn_4.bp_menu(&tft, 4, var, TFT_GREEN, false);
}

//Affichage des boutons bouttons menus
void affich_menu()
{
  char var[7];
  sprintf(var,"Cons");
  btn_1.bp_menu(&tft, 1, var);
  sprintf(var,"Conf");
  btn_2.bp_menu(&tft, 2, var);
  sprintf(var,"Cal");
  btn_3.bp_menu(&tft, 3, var, TFT_GREEN, false);
  if (regul == false){
    sprintf(var,"ON");
    btn_4.bp_menu(&tft, 4, var, TFT_GREEN, false);
  } else {
    sprintf(var,"OFF");
    btn_4.bp_menu(&tft, 4, var,TFT_RED, false);
  }
  //sprintf(var, "Mode");
  //btn_5.bp_menu(&tft, 5, var);
}

//Affichage des boutons modes
void mode_bp()
{
  char var[7];
  sprintf(var,"Cont");
  btn_1.bp_menu(&tft, 1, var);
  sprintf(var,"Regul");
  btn_2.bp_menu(&tft, 2, var);
  sprintf(var,"Freq");
  btn_3.bp_menu(&tft, 3, var);
  sprintf(var,"Exit");
  btn_4.bp_menu(&tft, 4, var, TFT_GREEN, false);
  //sprintf(var, "Mode");
  //btn_5.bp_menu(&tft, 5, var);
}

/******************************************************************************
 *  @brief
 *    Affichage des boutons de configuration du mode fréquence
 *  @param[in]
 *    void
 * @param[out]
 *    bool  retour de la fonction pour indiqué une erreur.
 *  @todo
 *    Faire un code de retour dans les autres fonctions...
 *    Voir pour mettre cette fonctions ainsi que celles d'affichage dans un
 *    fichier C séparé.
******************************************************************************/
bool freq_bp(void){
  char var[7];

  sprintf(var,"Cp/min");
  btn_1.bp_menu(&tft, 1, var);

  if(true == m_freq_tier){
    sprintf(var,"Quart");
  } else {
    sprintf(var,"Tier");
  }
  btn_2.bp_menu(&tft, 2, var);

  sprintf(var,"OFF");
  btn_3.bp_menu(&tft, 3, var);

  sprintf(var,"Exit");
  btn_4.bp_menu(&tft, 4, var, TFT_GREEN, false);

  return(NOERR);
}

// initialisation des variables
void init_var()
{
  int index_var;

  for(index_var = 0; index_var < NR_BTN_TS; index_var++){
    x[index_var] = 0;
    y[index_var] = 0;
  }

  affich = false;
  save = false;
  regul = false;
  tps_affich = millis() + interval;

  zero_in = 0;
  zero_trig = 0;
  for (index_var = 0; index_var < NRMES; index_var++){
    zero_in = zero_in + val_in[index_var].mesure;
    zero_trig = zero_trig + val_trig[index_var].mesure;
  }
  zero_in = (int)(zero_in / NRMES);
  zero_trig = (int)(zero_trig / NRMES);

  menu = PRINCIPAL;
}

//réglage de la vanne
int cmd_vanne(int argc, char** argv)
{
  byte i;
  long inter_tmp = 0;

  if (argc != 1) {
    shell_printf("%d\n", vanne_pos);
    return SHELL_RET_SUCCESS;
  } else {
    for (i = 0; i < strlen(argv[1]); i++){
      if ((argv[1][i] < 0x30) | (argv[1][i] > 0x39)) {
        shell_print_error(E_SHELL_ERR_VALUE, "vanne");
        return SHELL_RET_FAILURE;
      }
      inter_tmp = inter_tmp * 10 + (argv[1][i] - 0x30);
    }
    if ((inter_tmp < 0) | (inter_tmp > 5000)){
        shell_printf("%d\n", inter_tmp);
        shell_print_error(E_SHELL_ERR_OUTOFRANGE, "vanne");
        return SHELL_RET_FAILURE;
    }
    vanne_pos = inter_tmp * 12 / 10;
    shell_printf("%d\n", vanne_pos);
    //dac.setVoltage(vanne_pos, false);
    dac_jma.setVoltage(vanne_pos, &code_ret);
    analogWrite(PWM, vanne_pos / 16);
    return SHELL_RET_SUCCESS;
  }
}

//Consigne de réglage
int cmd_regul(int argc, char** argv)
{
  byte i;
  long consigne_tmp = 0;

  regul = false;
  //dac.setVoltage(0, false);
  dac_jma.setVoltage(0, &code_ret);
  analogWrite(PWM, 0);

  if (argc != 1) {
    shell_printf("%d\n", consigne);
    return SHELL_RET_SUCCESS;
  }

  for (i = 0; i < strlen(argv[1]); i++){
    if ((argv[1][i] < 0x30) | (argv[1][i] > 0x39)) {
      shell_print_error(E_SHELL_ERR_VALUE, "consigne");
      return SHELL_RET_FAILURE;
    }
    consigne_tmp = consigne_tmp * 10 + (argv[1][i] - 0x30);
  }

  if ((consigne_tmp < 0) | (consigne_tmp > 50)){
      shell_printf("%d\n", consigne_tmp);
      shell_print_error(E_SHELL_ERR_OUTOFRANGE, "regul");
      return SHELL_RET_FAILURE;
  }

  noInterrupts();
  consigne = consigne_tmp;
  consignem = consigne_tmp;
  interrupts();
  aff_consigne = consigne_tmp;

  regul = true;
  shell_printf("%d\n", aff_consigne);
  return SHELL_RET_SUCCESS;
}

//Changement de Kp
int cmd_kp(int argc, char** argv)
{
  int i;
  int k = 0;

  if (argc != 1) {
    shell_printf("%d\n", kp);
    return SHELL_RET_SUCCESS;
  }

  for (i = 0; i < strlen(argv[1]); i++){
    if ((argv[1][i] < 0x30) | (argv[1][i] > 0x39)) {
      shell_print_error(E_SHELL_ERR_VALUE, "k");
      return SHELL_RET_FAILURE;
    }
    k = k * 10 + (argv[1][i] - 0x30);
  }

  if ((k < 0) | (k > 4000)){
      shell_printf("%d\n", k);
      shell_print_error(E_SHELL_ERR_OUTOFRANGE, "k");
      return SHELL_RET_FAILURE;
  }

  kp = k;
  shell_printf("%d\n", kp);
  return SHELL_RET_SUCCESS;
}

//Changement de Ki
int cmd_ki(int argc, char** argv)
{
  int i;
  int k = 0;

  if (argc != 1) {
    shell_printf("%d\n", ki);
    return SHELL_RET_SUCCESS;
  }

  for (i = 0; i < strlen(argv[1]); i++){
    if ((argv[1][i] < 0x30) | (argv[1][i] > 0x39)) {
      shell_print_error(E_SHELL_ERR_VALUE, "k");
      return SHELL_RET_FAILURE;
    }
    k = k * 10 + (argv[1][i] - 0x30);
  }

  if ((k < 0) | (k > 4000)){
      shell_printf("%d\n", k);
      shell_print_error(E_SHELL_ERR_OUTOFRANGE, "k");
      return SHELL_RET_FAILURE;
  }
  ki = k;
  shell_printf("%d\n", ki);
  return SHELL_RET_SUCCESS;
}

//Changement de Kd
int cmd_kd(int argc, char** argv)
{
  int i;
  int k = 0;

  if (argc != 1) {
    shell_printf("%d\n", kd);
    return SHELL_RET_SUCCESS;
  }

  for (i = 0; i < strlen(argv[1]); i++){
    if ((argv[1][i] < 0x30) | (argv[1][i] > 0x39)) {
      shell_print_error(E_SHELL_ERR_VALUE, "k");
      return SHELL_RET_FAILURE;
    }
    k = k * 10 + (argv[1][i] - 0x30);
  }

  if ((k < 0) | (k > 4000)){
      shell_printf("%d\n", k);
      shell_print_error(E_SHELL_ERR_OUTOFRANGE, "k");
      return SHELL_RET_FAILURE;
  }
  kd = k;
  shell_printf("%d\n", kd);
  return SHELL_RET_SUCCESS;
}

//Changement de Kd
int cmd_modef(int argc, char** argv)
{
  int i;
  int k = 0;

  if (argc != 1) {
    shell_printf("dec:%d\n\r", modef_dec_timer);
    shell_printf("off:%d\n\r", modef_nr_off);
    shell_printf("nr:%d\n\r", modef_nr_timer);
    shell_printf("freq:%d\n\r", m_freq);
    return SHELL_RET_SUCCESS;
  }

  shell_print_error(E_SHELL_ERR_VALUE, "modef");
  return SHELL_RET_FAILURE;
}

// Commande shell d'affichage
int cmd_affich(int argc, char** argv)
{
  if (argc != 1) {
    affich = false;
    shell_print_error(E_SHELL_ERR_ARGCOUNT, "affich");
    return SHELL_RET_FAILURE;
  }

  if (strcmp(argv[1], "1") != 0){
    affich = false;
    return SHELL_RET_SUCCESS;
  }

  shell_print("lancement affich");
  affich = true;
  tps_affich = millis() + interval;
  return SHELL_RET_SUCCESS;
}

// Commande save
int cmd_save(int argc, char** argv)
{
  if (argc != 1) {
    save = false;
    shell_print_error(E_SHELL_ERR_ARGCOUNT, "save");
    return SHELL_RET_FAILURE;
  }

  if (strcmp(argv[1], "1") != 0){
    affich = false;
    return SHELL_RET_SUCCESS;
  }

  shell_print("lancement écriture SD");
  save = true;
  return SHELL_RET_SUCCESS;
}

/******************************************************************************
 * Fonction de calibration du zero des capteurs de pressions
******************************************************************************/
void cal(){
  byte i;

  zero_in = 0;
  zero_trig = 0;
  noInterrupts();

  for (i = 0; i < NRMES; i++){
    zero_in = zero_in + val_in[i].mesure;
    zero_trig = zero_trig + val_trig[i].mesure;
  }

  interrupts();
  zero_in = (int)(zero_in / NRMES);
  zero_trig = (int)(zero_trig / NRMES);
}

/******************************************************************************
 * Commande shell de calibration du zero
******************************************************************************/
int cmd_cal(int argc, char** argv){
  cal();
  shell_println("calib OK");
  shell_printf("zero_in=%d\r\n", zero_in);
  shell_printf("zero_trig=%d\r\n", zero_trig);
  return SHELL_RET_SUCCESS;
}

/******************************************************************************
 * modifie ou affiche le temps entre 2 impressions sur le port série
******************************************************************************/
int cmd_tps(int argc, char** argv){
  int i;
  long inter_tmp = 0;

  // Si pas d'argument affichage de la valeur
  if (argc == 0) {
    Serial.println(interval);
    return SHELL_RET_SUCCESS;
  } else if (argc > 1) {
    // Si trop d'argument retour erreur
    return SHELL_RET_FAILURE;
  }

  // Sinon traitement de la demande
  for (i = 0; i < strlen(argv[1]); i++){

    // Vérification que ce soit des nombres sinon retour erreur
    if ((argv[1][i] < 0x30) | (argv[1][i] > 0x39)) {
      shell_print_error(E_SHELL_ERR_VALUE, "interval");
      return SHELL_RET_FAILURE;
    }

    // Conversion un entier
    inter_tmp = inter_tmp * 10 + (argv[1][i] - 0x30);
  }

  // Vérification de la valeur par rapport à l'interval
  if ((inter_tmp > 10000) | (inter_tmp < 100)){
      shell_printf("%d\n", inter_tmp);
      shell_print_error(E_SHELL_ERR_OUTOFRANGE, "interval");
      return SHELL_RET_FAILURE;
  }

  // Modification de la valeur
  interval = inter_tmp;
  shell_printf("%d\n", interval);
  return SHELL_RET_SUCCESS;
}

/******************************************************************************
 * Calcul de la moyenne des mesures
******************************************************************************/
void moyenne(){
  byte index_moy;

  mm_h2o_in = 0;
  mm_h2o_trig = 0;

  for (index_moy = 0; index_moy < NRMES; index_moy++){
    mm_h2o_in = mm_h2o_in + val_in[index_moy].mesure;// - zero_ana1;
    mm_h2o_trig = mm_h2o_trig + val_trig[index_moy].mesure;// - zero_ana2;
  }

  mm_h2o_in = mm_h2o_in / NRMES;
  mm_h2o_trig = mm_h2o_trig / NRMES;

  mm_h2o_in = mm_h2o_in - zero_in;
//  mm_h2o_trig = mm_h2o_trig - zero_trig;
  mm_h2o_trig = zero_trig - mm_h2o_trig;

  mm_h2o_in = mm_h2o_in * 49;
  mm_h2o_trig = mm_h2o_trig * 49;

  mm_h2o_in = (int)(mm_h2o_in / 75);
  mm_h2o_trig = (int)(mm_h2o_trig / 75);
}

/******************************************************************************
 * Affichage sur l'écran tactile
******************************************************************************/
void affich_ecr_g(void){
  screen.pscr_aff_val(&tft, aff_in, PSCR_IN);
  screen.pscr_aff_val(&tft, aff_trig, PSCR_TRIG);
  screen.pscr_aff_val(&tft, consigne, PSCR_CONS);
  //screen.pscr_aff_val(&tft, aff_dac, PSCR_DAC);
}

/******************************************************************************
 * Affichage de la mesure sur la LS
******************************************************************************/
void affiche_ls(void){
  shell_printf("IN=%dmm-H2O, TRIG=%dmm-H2O, dac=%d", aff_in, aff_trig, aff_dac);
  shell_println("");
}

/******************************************************************************
 * Écriture des données dans la SD
******************************************************************************/
void save_sd(void){
  String datastring = "";

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {

    //todo, if faudra ajouter Consigne, dac
    datastring += "Time;";
    datastring += String(millis());
    datastring += ";IN;";
    datastring += String(aff_in);
    datastring += ";TRIG;";
    datastring += String(aff_trig);

    dataFile.println(datastring);
    dataFile.close();
    // print to the serial port too:
    // Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    shell_println("error opening datalog.txt");
  }
}

/******************************************************************************
 * Tache de lecture du clavier
******************************************************************************/
void kbd_task(){
  //Lecture du clavier
  touche = clavier.getKey();

  //Lecture du tactile
  btn_press = readResistiveTouch();
  btn_1.press(btn_press && btn_1.contains(pos_x, pos_y));
  btn_2.press(btn_press && btn_2.contains(pos_x, pos_y));
  btn_3.press(btn_press && btn_3.contains(pos_x, pos_y));
  btn_4.press(btn_press && btn_4.contains(pos_x, pos_y));
  //btn_5.press(btn_press && btn_5.contains(pos_x, pos_y));

  clv_0.press(btn_press && clv_0.contains(pos_x, pos_y));
  clv_1.press(btn_press && clv_1.contains(pos_x, pos_y));
  clv_2.press(btn_press && clv_2.contains(pos_x, pos_y));
  clv_3.press(btn_press && clv_3.contains(pos_x, pos_y));
  clv_4.press(btn_press && clv_4.contains(pos_x, pos_y));
  clv_5.press(btn_press && clv_5.contains(pos_x, pos_y));
  clv_6.press(btn_press && clv_6.contains(pos_x, pos_y));
  clv_7.press(btn_press && clv_7.contains(pos_x, pos_y));
  clv_8.press(btn_press && clv_8.contains(pos_x, pos_y));
  clv_9.press(btn_press && clv_9.contains(pos_x, pos_y));
  clv_c.press(btn_press && clv_c.contains(pos_x, pos_y));
  clv_v.press(btn_press && clv_v.contains(pos_x, pos_y));
}

/******************************************************************************
 * Gestion du pavé numérique dans les menu de configuration
******************************************************************************/
bool gestion_pave_num(int *org){
  int save;

  save = kx_tp;

  if (kx_tp > 99){
    kx_tp = kx_tp / 10;
  }

  kx_tp = kx_tp * 10;

  // Appui sur la touche 1
  if (clv_1.justReleased() || touche == '1'){
    kx_tp = kx_tp + 1;
    return(false);
  }

  // Appui sur la touche 2
  if (clv_2.justReleased() || touche == '2'){
    kx_tp = kx_tp + 2;
    return(false);
  }

  // Appui sur la touche 3
  if (clv_3.justReleased() || touche == '3'){
    kx_tp = kx_tp + 3;
    return(false);
  }

  // Appui sur la touche 4
  if (clv_4.justReleased() || touche == '4'){
    kx_tp = kx_tp + 4;
    return(false);
  }

  // Appui sur la touche 5
  if (clv_5.justReleased() || touche == '5'){
    kx_tp = kx_tp + 5;
    return(false);
  }

  // Appui sur la touche 6
  if (clv_6.justReleased() || touche == '6'){
    kx_tp = kx_tp + 6;
    return(false);
  }

  // Appui sur la touche 7
  if (clv_7.justReleased() || touche == '7'){
    kx_tp = kx_tp + 7;
    return(false);
  }

  // Appui sur la touche 8
  if (clv_8.justReleased() || touche == '8'){
    kx_tp = kx_tp + 8;
    return(false);
  }

  // Appui sur la touche 9
  if (clv_9.justReleased() || touche == '9'){
    kx_tp = kx_tp + 9;
    return(false);
  }

  // Appui sur la touche 0
  if (clv_0.justReleased() || touche == '0'){
    return(false);
  }

  // Appui sur la touche * (correction)
  if (clv_c.justReleased() || touche == '*'){
    if (kx_tp > 9){
      kx_tp = kx_tp / 10;
    } else {
      kx_tp = 0;
    }
    return(false);
  }

  // Appui sur la touche # (Validation)
  if (clv_v.justReleased() || touche == '#'){
    tft.writeFillRect(0, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
    if (menu == CFG_FREQ){
      modef_nr_timer = (int) (NR_TIMER1_1_MIN / save);

      if (m_freq_tier == true){
        modef_nr_off = modef_nr_timer * 2 / 3;
      } else {
        modef_nr_off = modef_nr_timer * 3 / 4;
      }
      modef_dec_timer = modef_nr_timer;
      shell_printf("nr timer: %d", modef_nr_timer);
    }
    affich_menu();
    menu = PRINCIPAL;
    *org = save;
    k_print();
    return(true);
  }

  kx_tp = save;
  return(false);
}

/******************************************************************************
 *  @brief
 *    Tache de gestion de l'affichage
 *  @param[in]
 *    void
 * @param[out]
 *    void
 *  @todo
 *    Faire un code de retour plus significatif
 *    Voir pour mettre cette fonctions ainsi que celles d'affichage dans un
 *    fichier C séparé.
******************************************************************************/
void ihm_task(void){
  char ihm_task_bp_name[10];

  if (tps_vie < millis()){
    if (regul == true){
      screen.pscr_life(&tft, PSCR_REGUL);
    } else {
      screen.pscr_life(&tft, PSCR_INFO);
    }
    tps_vie = millis() + 500;
  }

  if (regul == false){
    sprintf(ihm_task_bp_name, "ON");
    btn_4.initButtonUL(&tft, 365, 200, 110, 78, TFT_DARKGREY, TFT_BLACK, TFT_GREEN , ihm_task_bp_name,2);
    erreur = 0;
    s_erreur = 0;
    d_erreur = 0;
    p_erreur = 0;
  }

  kbd_task();

  // État dans le menu principal
  if (menu == PRINCIPAL){

    // Appui sur le bouton cons
    if (btn_1.justReleased() || touche == '1'){
      tft.writeFastVLine(A_REGL_CLI,A_REGL_Y,A_REGL_HI,TFT_GREEN);
      tps_IHM1 = millis() + 1000;
      tps_IHM2 = millis() + 500;
      screen.pscr_mcons(&tft, aff_consigne);
      affich_bp();
      menu = MCONSIGNE;
      return;
    }

    // Appui sur le bouton conf
    if (btn_2.justReleased() || touche == '2'){
      config_bp();
      menu = MCONFIG;
      return;
    }

    // Appui sur le bouton cal
    if (btn_3.justReleased() || touche == '3'){
      cal();
      return;
    }

    // Appui sur le bouton de régulation
    if (btn_4.justReleased() || touche == '4'){

      if ((regul == true) || ((regul == false) && (out_dac != 0))){
        sprintf(ihm_task_bp_name, "ON");
        btn_4.initButtonUL(&tft, 365, 200, 110, 78, TFT_DARKGREY, TFT_BLACK, TFT_GREEN , ihm_task_bp_name,2);
        regul = false;
        out_dac = 0;
        //dac.setVoltage(out_dac, false);
        dac_jma.setVoltage(out_dac, &code_ret);
        analogWrite(PWM, out_dac);
        mesure();
        if (mode == MODEF){
          consigne = consignem;
        }
      } else {
        sprintf(ihm_task_bp_name, "OFF");
        btn_4.initButtonUL(&tft, 365, 200, 110, 78, TFT_DARKGREY, TFT_BLACK, TFT_RED , ihm_task_bp_name,2);

        float erreur = 0;
        float s_erreur = 0;
        float d_erreur = 0;
        float p_erreur = 0;
        consignem = consigne;

        regul = true;
      }

      btn_4.drawButton(false);
      return;
    }

    /*if (btn_5.justReleased() || touche == '5'){
        mode_bp();
        menu = MMODE;
    }*/
  }

  // État dans le menu consigne
  if (menu == MCONSIGNE){
    menu_consigne();
    return;
  }

  // État dans le menu de configuration
  if (menu == MCONFIG){
    menu = menu_config();
    return;
  }

  // État dans le menu  de configuration des variables K
  if (menu == MCONFKX){
    menu_kx();
    return;
  }

  // État dans le menu de configuration de la variable KP
  if (menu == CFG_KP){

    // Sortie du menu de configuration
    if (btn_4.justReleased() || touche == '4'){
      tft.writeFillRect(0, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
      tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
      affich_menu();
      menu = PRINCIPAL;
      return;
    }

    // Affichage de la variable
    if (true != gestion_pave_num(&kp)){
      tft.setTextSize(A_ECR2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(A_REGL_X,A_REGL_Y);
      tft.print(String(kx_tp));
      tft.print("  ");
    }
    return;
  }

  // État dans le menu de configuration de la variable KI
  if (menu == CFG_KI){

    // Sortie du menu de configuration
    if (btn_4.justReleased() || touche == '4'){
      tft.writeFillRect(0, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
      tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
      affich_menu();
      menu = PRINCIPAL;
      return;
    }

    // Affichage de la variable
    if (true != gestion_pave_num(&ki)){
      tft.setTextSize(A_ECR2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(A_REGL_X,A_REGL_Y);
      tft.print(String(kx_tp));
      tft.print("  ");
    }
    return;
  }

  // État dans le menu de configuration de la variable KD
  if (menu == CFG_KD){

    // Sortie du menu de configuration
    if (btn_4.justReleased() || touche == '4'){
      tft.writeFillRect(0, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
      tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
      affich_menu();
      menu = PRINCIPAL;
      return;
    }

    // Affichage de la variable
    if (true != gestion_pave_num(&kd)){
      tft.setTextSize(A_ECR2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(A_REGL_X,A_REGL_Y);
      tft.print(String(kx_tp));
      tft.print("  ");
    }
    return;
  }

  // État dans le menu de configuration du mode
  if (menu == MMODE){
      menu_mode();
      return;
  }

  // État dans le menu de configuration du mode de fréquence
  if (menu == MFREQ){
      menu_freq();
      return;
  }

  // État dans le menu de configuration de la variable m_freq
  if (menu == CFG_FREQ){

    // Sortie du menu de configuration
    if (btn_4.justReleased() || touche == '4'){
      tft.writeFillRect(0, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
      tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
      affich_menu();
      menu = PRINCIPAL;
      return;
    }

    // Affichage de la variable
    if (true != gestion_pave_num(&m_freq)){
      tft.setTextSize(A_ECR2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(A_REGL_X,A_REGL_Y);
      tft.print(String(kx_tp));
      tft.print("  ");
      return;
    }

    return;
  }

  // État dans le menu de configuration de la variable consigne_off
  if (menu == CFG_OFF){

    // Sortie du menu de configuration
    if (btn_4.justReleased() || touche == '4'){
      tft.writeFillRect(0, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
      tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
      affich_menu();
      menu = PRINCIPAL;
      return;
    }
    // Affichage de la variable
    if (true != gestion_pave_num(&consigne_off)){
      tft.setTextSize(A_ECR2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(A_REGL_X,A_REGL_Y);
      tft.print(String(kx_tp));
      tft.print("  ");
      return;
    }
    return;
  }

  return;
}

/******************************************************************************
 * Gestion et affichage de du menu de configuration du mode
******************************************************************************/
void menu_mode(){
  // Sortie du menu de  configuration
  if (btn_4.justReleased() || touche == '4'){
    affich_menu();
    menu = PRINCIPAL;
    return;
  }

  // Choix du mode Constant
  if (btn_1.justReleased() || touche == '1'){
    mode = MODEC;
    menu = PRINCIPAL;
    screen.pscr_mode(&tft, mode);
    affich_menu();
    return;
  }

  // Choix du mode régulé
  if (btn_2.justReleased() || touche == '2'){
    mode = MODER;
    menu = PRINCIPAL;
    screen.pscr_mode(&tft, mode);
    affich_menu();
    return;
  }

  // Choix du mode fréquence
  if (btn_3.justReleased() || touche == '3'){
    mode = MODEF;
    menu = PRINCIPAL;
    screen.pscr_mode(&tft, mode);
    affich_menu();
    return;
  }
}

/******************************************************************************
 *  @brief
 *    Gestion et affichage du menu de configuration du mode fréquence
 *  @param[in]
 *    void
 * @param[out]
 *    bool  presence d'erreurs.
 *  @todo
 *    Faire un code de retour plus significatif
 *    Voir pour mettre cette fonctions ainsi que celles d'affichage dans un
 *    fichier C séparé.
******************************************************************************/
bool menu_freq(void){
  char freq_bp_name[10];

  // Nouvelle valeur pour la variable coups/min
  if (btn_1.justReleased() || touche == '1'){
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BTN2_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    tft.writeFillRect(A_BTN3_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    pave_bp();
    menu = CFG_FREQ;
    kx_tp = 0;
    return(NOERR);
  }

  // Mode 1/3 ou 1/4
  if (btn_2.justReleased() || touche == '2'){
    if (true == m_freq_tier){
      m_freq_tier = false;
      sprintf(freq_bp_name,"Tier");
      btn_2.bp_menu(&tft, 2, freq_bp_name);
    } else {
      m_freq_tier = true;
      sprintf(freq_bp_name,"Quart");
      btn_2.bp_menu(&tft, 2, freq_bp_name);
    }

    k_print();
    return(NOERR);
  }

  // Consigned OFF
  if (btn_3.justReleased() || touche == '3'){
    sprintf(freq_bp_name,"OFF");
    btn_1.bp_menu(&tft, 1, freq_bp_name);
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BTN2_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    tft.writeFillRect(A_BTN3_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    pave_bp();
    menu = CFG_OFF;
    kx_tp = 0;
    return(NOERR);
  }

  // Sortie du menu de  configuration
  if (btn_4.justReleased() || touche == '4'){
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
    tft.writeFillRect(A_REGL_X,A_REGL_Y,A_BTN_LG*3,A_BTN_HI,TFT_BLACK);
    affich_menu();
    menu = PRINCIPAL;
  }

  // Aucun boutons appuyé
  return(NOERR);
}

/******************************************************************************
 * Affichage du pavé numérique
******************************************************************************/
void pave_bp(){
  clv_1.bp_num(&tft, 1);
  clv_4.bp_num(&tft, 4);
  clv_7.bp_num(&tft, 7);
  clv_c.bp_num(&tft, 10);
  clv_2.bp_num(&tft, 2);
  clv_5.bp_num(&tft, 5);
  clv_8.bp_num(&tft, 8);
  clv_0.bp_num(&tft, 0);
  clv_3.bp_num(&tft, 3);
  clv_6.bp_num(&tft, 6);
  clv_9.bp_num(&tft, 9);
  clv_v.bp_num(&tft, 11);
  return;
}

/******************************************************************************
 * Gestion et affichage du menu Kx
******************************************************************************/
void menu_kx(void){
  char menu_kx_bp_name[10];

  // Configuration de Kp
  if (btn_1.justReleased() || touche == '1'){
    sprintf(menu_kx_bp_name, "KP");
    btn_1.bp_menu(&tft, 1, menu_kx_bp_name);
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BTN2_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    tft.writeFillRect(A_BTN3_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    pave_bp();
    menu = CFG_KP;
    kx_tp = 0;
    return;
  }

  // Configuration de Ki
  if (btn_2.justReleased() || touche == '2'){
    sprintf(menu_kx_bp_name, "KI");
    btn_1.bp_menu(&tft, 1, menu_kx_bp_name);
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BTN2_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    tft.writeFillRect(A_BTN3_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    pave_bp();
    menu = CFG_KI;
    kx_tp = 0;
    return;
  }

  // Configuration de Kd
  if (btn_3.justReleased() || touche == '3'){
    sprintf(menu_kx_bp_name, "KD");
    btn_1.bp_menu(&tft, 1, menu_kx_bp_name);
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BTN2_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    tft.writeFillRect(A_BTN3_X,A_BTN_Y,A_BTN_LG,A_BTN_HI,TFT_BLACK);
    pave_bp();
    menu = CFG_KD;
    kx_tp = 0;
    return;
  }

  // sortie du menu de configuration
  if (btn_4.justReleased() || touche == '4'){
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
    tft.writeFillRect(A_REGL_X,A_REGL_Y,A_BTN_LG*3,A_BTN_HI,TFT_BLACK);
    affich_menu();
    menu = PRINCIPAL;
  }
}

/******************************************************************************
 *  @brief
 *    Gestion et affichage du menu de configuration
 *  @param[in]
 *    void
 * @param[out]
 *    int  retour du menu en cours.
 *  @todo
 *    Faire un code de retour plus significatif
 *    Voir pour mettre cette fonctions ainsi que celles d'affichage dans un
 *    fichier C séparé.
******************************************************************************/
int menu_config(void){

  // Configuration des variables Kx
  if (btn_1.justReleased() || touche == '1'){
    kx_bp();
    menu = MCONFKX;
    return(MCONFKX);
  }

  // Configuration du mode de fonctionnement
  if (btn_2.justReleased() || touche == '2'){
    mode_bp();
    menu = MMODE;
    return(MMODE);
  }

  // Configuration de la fréquence du mode fréquence
  if (btn_3.justReleased() || touche == '3'){
    if (false == freq_bp()){
      menu = MFREQ;
      return(MFREQ);
    }
    sprintf(last_err,"Impossible d'entrer dans le menu fréquence");
    screen.pscr_err(&tft, last_err);
    return(MCONFIG);
  }

  // Sortie du menu de configuration
  if (btn_4.justReleased() || touche == '4'){
    tft.writeFillRect(A_BTN1_X, A_BTN5_Y, A_BP_LG * 2, A_BP_HI * 2, TFT_BLACK);
    tft.writeFillRect(A_BP_COL1,A_BP_ROW1,A_PV_LG,A_PV_HI,TFT_BLACK);
    tft.writeFillRect(A_REGL_X,A_REGL_Y,A_BTN_LG*3,A_BTN_HI,TFT_BLACK);
    affich_menu();
    menu = PRINCIPAL;
    return(PRINCIPAL);
  }

  // pas d'appui sur un boutton
  return(MCONFIG);
}

/******************************************************************************
 *  @brief
 *    Gestion et affichage de la consigne pour modification
 *  @param[in]
 *    void
 * @param[out]
 *    bool  presence d'erreurs.
 *  @todo
 *    Faire un code de retour plus significatif
******************************************************************************/
bool menu_consigne(void){

  //Clignotement du curseur (Noir)
  if (tps_IHM1 < millis()){
    tps_IHM1 = millis() + 1000;
    tft.writeFastVLine(A_REGL_CLI,A_REGL_Y,A_REGL_HI,TFT_BLACK);
  }

  //Clignotement du curseur (Vert)
  if (tps_IHM2 < millis()){
    tps_IHM2 = millis() + 1000;
    tft.writeFastVLine(A_REGL_CLI,A_REGL_Y,A_REGL_HI,TFT_GREEN);
  }

  // Augmentation de la valeur de la consigne
  if ((touche == '3') || btn_3.justReleased()){

    if (aff_consigne < 25) {
      aff_consigne++;
      tft.setTextSize(A_ECR2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(A_REGL_X_VAL,A_REGL_Y);
      tft.print(String(aff_consigne));
      tft.print("  ");
      return(false);
    }
  }

  // Diminution de la valeur de la consigne
  if ((touche == '2') || btn_2.justReleased()){

    if (aff_consigne > 0){
      aff_consigne--;
      tft.setTextSize(A_ECR2);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.setCursor(A_REGL_X_VAL,A_REGL_Y);
      tft.print(String(aff_consigne));
      tft.print("  ");
      return(false);
    }
  }

  // Validation de la valeur de la consigne et retour sur le menu principal
  if ((touche == '4') || btn_4.justReleased()){
    noInterrupts();
    consigne = aff_consigne;
    consignem = aff_consigne;
    interrupts();
    tft.writeFillRect(A_REGL_X,A_REGL_Y,A_BTN_LG*3,A_BTN_HI,TFT_BLACK);
    affich_menu();
    menu = PRINCIPAL;
    return(false);
  }
  return(false);

}

/******************************************************************************
 *  @brief
 *    Tache d'affichage des valeurs sur l'écran LCD, sauvegarde sur carte SD
 *    et envoi sur le port série
 *  @param[in]
 *    void
 * @param[out]
 *    bool  presence d'erreurs.
 *  @todo
 *    Faire un code de retour plus significatif
******************************************************************************/
bool affiche_task(){

  if (tps_affich < millis()){
    //Enregistrement des valeurs courantes de mesures
    noInterrupts();
    aff_in = mm_h2o_in;
    aff_trig = mm_h2o_trig;
    aff_dac = out_dac;
    interrupts();

    tps_affich = millis() + interval;
    affich_ecr_g();

    if (affich == true) {
      affiche_ls();
    }

    if (save == true){
      save_sd();
    }

    return(NOERR);
  }

  return(NOERR);
}
