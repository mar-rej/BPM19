/*
    rej version 0.0.1 -> creation

    TODO: pb avec fleche haut/bas
*/
#include "majshell.h"

/**
 * This structure array contains the available commands and they associated
 * function entry point, other data required by the commands may be added to
 * this structure
 */
struct shell_command_entry list[CONFIG_SHELL_MAX_COMMANDS];

/**
 * This array of pointers to characters holds the addresses of the begining of
 * the parameter strings passed to the programs
 */
char * argv_list[CONFIG_SHELL_MAX_COMMAND_ARGS];

/**
 * This is the main buffer to store received characters
 */
char shellbuf[CONFIG_SHELL_MAX_COMMANDS][CONFIG_SHELL_MAX_INPUT];

/**
 * This is the buffer for formatted strings
 */
char shellfmtbuf[CONFIG_SHELL_FMT_BUFFER];

shell_reader_t shell_reader = 0;
shell_writer_t shell_writer = 0;

bool initialized = false;

int index_hist = 0;

/**
 * Parses the string and finds all the substrings (arguments)
 *
 * @param buf The buffer containing the original string
 *
 * @param argv Pointer to char * array to place the pointers to substrings
 *
 * @param maxargs The maximum number of pointers that the previous array can hold
 *
 * @return The total of args parsed
 */
static int shell_parse(char * buf, char** argv, unsigned short maxargs);

/**
 *  Prints the command shell prompt
 */
static void shell_prompt();

/**
 * Helper function for formatting text in shell_printf and shell_printf_pm
 *
 * @param fmt
 *
 * @param va
 */
static void shell_format(const char * fmt, va_list va);

// initialise les variables des commandes shell
static void shell_unregister_all();

// Affiche la liste des commandes shell
static void shell_print_commands(bool tout,
    bool select[CONFIG_SHELL_MAX_COMMANDS]);

// Initialisation du shell
bool shell_init(shell_reader_t reader, shell_writer_t writer, char* ver)
{
    for (index_hist = 0; index_hist < CONFIG_SHELL_MAX_COMMANDS; index_hist++){
        sprintf(shellbuf[index_hist], "");
    }

    index_hist = 0;
    if (reader == 0 || writer == 0) {
        return false;
    }

    shell_unregister_all();
    shell_reader = reader;
    shell_writer = writer;
    initialized = true;
    shell_println((const char *) SHELL_VERSION_STRING);
    shell_println((const char *) ver);
    shell_prompt();
    return true;
}

//enregistrement des commandes
bool shell_register(shell_program_t program, const char * string_desc,
                    const char * string_args,  const char * string_cmd)
{
    unsigned char i, j, nrcmd;

    nrcmd = 0;

    if (list[0].shell_program == 0){
        list[0].shell_program = program;
        list[0].shell_desc_string = string_desc;
        list[0].shell_args_string = string_args;
        list[0].shell_command_string = string_cmd;
        return true;
    }

    for (i = 1; i < CONFIG_SHELL_MAX_COMMANDS; i++) {
        if (list[i].shell_program == 0){
            nrcmd = i;
            i = CONFIG_SHELL_MAX_COMMANDS;
        }
    }

    for (i = 0; i < nrcmd ; i++) {
        if (strcmp (list[i].shell_command_string, string_cmd) > 0){
            for (j = nrcmd; j > i; j--){
                list[j].shell_program = list[j-1].shell_program;
                list[j].shell_desc_string = list[j-1].shell_desc_string;
                list[j].shell_args_string = list[j-1].shell_args_string;
                list[j].shell_command_string = list[j-1].shell_command_string;
            }

            list[i].shell_program = program;
            list[i].shell_desc_string = string_desc;
            list[i].shell_args_string = string_args;
            list[i].shell_command_string = string_cmd;
            return true;
        } else {
            if (list[i+1].shell_program == 0) {
                list[i+1].shell_program = program;
                list[i+1].shell_desc_string = string_desc;
                list[i+1].shell_args_string = string_args;
                list[i+1].shell_command_string = string_cmd;
                return true;
            }
        }
    }

    return false;
}

// Initialisation des commandes shell
void shell_unregister_all()
{
    unsigned char i;

    for (i = 0; i < CONFIG_SHELL_MAX_COMMANDS; i++) {
        list[i].shell_program = 0;
        list[i].shell_desc_string = 0;
        list[i].shell_args_string = 0;
        list[i].shell_command_string = 0;
    }
}

// Affiche la liste des commandes disponible
void shell_print_commands(bool tout, bool select[CONFIG_SHELL_MAX_COMMANDS])
{
    unsigned char i, j, deb;

    if (tout == true) {
        for (i = 0; i < CONFIG_SHELL_MAX_COMMANDS; i++){
            select[i] = true;
        }
    }

    shell_print("\r\n");
    for (i = 0; i < CONFIG_SHELL_MAX_COMMANDS; i++) {
        if (list[i].shell_command_string == 0){
            shell_print("\r\n");
            shell_prompt();
            return;
        }

        if (i == 4){
            shell_print("\r\n");
        }

        if (select[i] == true){
            deb = shell_print(list[i].shell_command_string);
            for (j = deb; j < 20; j++) {
                shell_print(" ");
            }
        }
    }

    shell_print("\r\n");
    shell_print("\r\n");
    shell_prompt();
}

// Affichage de l'erreur
void shell_print_error(int error, const char * field)
{
    if (field != 0) {
        shell_print(field);
        shell_print(" - ");
    }

    switch (error) {
    case E_SHELL_ERR_ARGCOUNT:
        shell_print((const char *)("ARG_COUNT"));
        break;
    case E_SHELL_ERR_OUTOFRANGE:
        shell_print((const char *)("OUT_OF_RANGE"));
        break;
    case E_SHELL_ERR_VALUE:
        shell_print((const char *)("INVALID_VALUE"));
        break;
    case E_SHELL_ERR_ACTION:
        shell_print((const char *)("INVALID_ACTION"));
        break;
    case E_SHELL_ERR_PARSE:
        shell_print((const char *)("PARSING"));
        break;
    case E_SHELL_ERR_STORAGE:
        shell_print((const char *)("STORAGE"));
        break;
    case E_SHELL_ERR_IO:
        shell_print((const char *)("IO"));
        break;
    default:
        shell_print((const char *)("Unknown"));
    }
}

// Affichage d'une chaine de carract�re depuis la ram
int shell_print(const char * string)
{
    int ret = 0;

    while (* string != '\0') {
        shell_writer(* string);
        string++;
        ret++;
    }

    return ret;
}

// Idem sauf avec saut de ligne
void shell_println(const char * string)
{
    shell_print(string);
    shell_print((const char *) "\r\n");
}

// Formatage d'une chaine de caract�re
void shell_printf(const char * fmt, ...)
{
    va_list argl;
    va_start(argl, fmt);
    shell_format(fmt, argl);
    va_end(argl);
}

//traitement de la reception de caract�res
void shell_task()
{
    bool finished = false;
    bool car_ide = false;
    bool Choix [CONFIG_SHELL_MAX_COMMANDS];
    char rxchar = 0;
    char tempo[CONFIG_SHELL_MAX_INPUT];
    unsigned int i = 0;
    unsigned int retval = 0;
    unsigned int j = 0;
    int argc = 0;
    int nbChoix = 0;
    int lastChoix = 0;

    // Number of characters written to buffer (this should be static var)
    static unsigned short count = 0;

    if (!initialized) {
        return;
    }

    // Process each one of the received characters
    if (shell_reader(&rxchar)) {
        switch (rxchar) {
        case SHELL_VT100_ARROWUP:
            if (index_hist != 0){
                index_hist--;
            } else {
                if (strcmp(shellbuf[CONFIG_SHELL_MAX_COMMANDS - 1], "") != 0){
                    index_hist = CONFIG_SHELL_MAX_COMMANDS - 1;
                } else {
                    break;
                }
            }

            shell_println("");
            shell_prompt();
            shell_print(shellbuf[index_hist]);
            count = strlen(shellbuf[index_hist]);
            break;

        case SHELL_VT100_ARROWDOWN:
            if (index_hist != CONFIG_SHELL_MAX_COMMANDS - 1){
                if (strcmp(shellbuf[index_hist + 1], "") != 0){
                    index_hist++;
                } else {
                    break;
                }
            } else {
                index_hist = 0;
            }

            shell_println("");
            shell_prompt();
            shell_print(shellbuf[index_hist]);
            count = strlen(shellbuf[index_hist]);
            break;

        case SHELL_ASCII_ESC: // For VT100 escape sequences
        case '[':
            // Process escape sequences: maybe later
            break;

        case SHELL_ASCII_HT: //TAB
            //shell_writer(SHELL_ASCII_BEL);
            if (count > 0) {
                nbChoix = 0;
                for (i = 0; i < CONFIG_SHELL_MAX_COMMANDS; i++) {
                    if (list[i].shell_program == 0) {
                        Choix[i] = false;
                        continue;
                    }

                    // If string matches one on the list
                    if (strncmp(shellbuf[index_hist],
                        list[i].shell_command_string,
                        count) == 0) {
                        nbChoix++;
                        Choix[i] = true;
                        lastChoix = i;
                    } else {
                        Choix[i] = false;
                    }
                }

                switch (nbChoix){
                case 1 :
                    shell_print("\r\n");
                    strcpy (shellbuf[index_hist],
                    list[lastChoix].shell_command_string);
                    shell_prompt();
                    shell_print(shellbuf[index_hist]);
                    count = strlen(shellbuf[index_hist]);
                    break;

                case 0 :
                    shell_print_commands(true, Choix);
                    shell_print(shellbuf[index_hist]);
                    break;

                default :
                    for (i = count;
                        i < strlen(list[lastChoix].shell_command_string);
                        i++){
                        rxchar = 0;
                        car_ide = false;
                        for (j = 0; j < CONFIG_SHELL_MAX_COMMANDS; j++){
                            if (list[j].shell_program == 0) {
                                continue;
                            }

                            if (Choix[j] != true){
                                continue;
                            }

                            if (rxchar == 0){
                                rxchar = list[j].shell_command_string[i];
                            } else {
                                if (rxchar == list[j].shell_command_string[i]){
                                    car_ide = true;
                                } else {
                                    car_ide = false;
                                    i = strlen(list[lastChoix].shell_command_string);
                                }
                            }
                        }

                        if (car_ide == true) {
                            shellbuf[index_hist][i] = rxchar;
                            count++;
                            shell_writer(rxchar);
                        }
                    }

                    //count--;
                    shell_print_commands(false, Choix);
                    shell_print(shellbuf[index_hist]);
                    break;
                }

            } else {
                shell_print_commands(true, Choix);
            }

            break;

        case SHELL_ASCII_CR: // Enter key pressed
            shellbuf[index_hist][count] = '\0';
            shell_println("");
            finished = true;
            break;

        case SHELL_ASCII_BS: // Backspace pressed
        case SHELL_ASCII_DEL:
            if (count > 0) {
                count--;
                shell_writer(SHELL_ASCII_BS);
                shell_writer(SHELL_ASCII_SP);
                shell_writer(SHELL_ASCII_BS);
            } else {
                //shell_writer(SHELL_ASCII_BEL);
            }

            break;

        default:
            // Process printable characters, but ignore other ASCII chars
            if (count < CONFIG_SHELL_MAX_INPUT && rxchar >= 0x20 &&
                rxchar < 0x7F) {
                shellbuf[index_hist][count] = rxchar;
                shell_writer(rxchar);
                count++;
            }
        }

        // Check if a full command is available on the buffer to process
        if (finished == true) {
            strcpy(tempo, shellbuf[index_hist]);
            argc = shell_parse(tempo, argv_list, CONFIG_SHELL_MAX_COMMAND_ARGS);

            // sequential search on command table
            for (i = 0; i < CONFIG_SHELL_MAX_COMMANDS; i++) {
                if (list[i].shell_program == 0){
                    continue;
                }

                // If string matches one on the list
                if (strcmp(argv_list[0], list[i].shell_command_string) == 0) {
                    // Run the appropiate function
                    if (strcmp(argv_list[1], "?") != 0){
                        retval = list[i].shell_program(argc, argv_list);
                    } else {
                        retval = SHELL_RET_FAILURE;
                    }

                    if (retval != SHELL_RET_SUCCESS) {
                        shell_println("");
                        shell_println(list[i].shell_desc_string);
                        shell_print(list[i].shell_args_string);
                    }

                finished = false;
                }
            }

            // If no command found and buffer not empty
            if (finished != false && count != 0) {
                shell_println((const char *)("Command NOT found."));
            }

            count = 0;
            shell_println("");
            shell_prompt();
            index_hist++;
            if (index_hist == CONFIG_SHELL_MAX_COMMANDS){
                index_hist = 0;
            }
        }
    }
}

/*-------------------------------------------------------------*/
/*      Internal functions              */

/*-------------------------------------------------------------*/
static int shell_parse(char * buf, char ** argv, unsigned short maxargs)
{
    int i = 0;
    int argc = 0;
    int length = strlen(buf) + 1; //String lenght to parse = strlen + 1
    bool toggle = false;

    argv[argc] = &buf[0];
    for (i = 0; i < length && argc < maxargs; i++) {
        switch (buf[i]) {
            // String terminator means at least one arg
        case '\0':
            i = length;
            //argc++;
            break;
            // Check for double quotes for strings as parameters
        case '\"':
            if (toggle == false) {
                toggle = true;
                buf[i] = '\0';
                argv[argc] = &buf[i + 1];
            } else {
                toggle = false;
                buf[i] = '\0';
            }
            break;
        case ' ':
            if (toggle == false) {
                buf[i] = '\0';
                argc++;
                argv[argc] = &buf[i + 1];
            }
            break;
        }
    }
    return argc;
}

static void shell_prompt()
{
    shell_print((const char *) "majShell>");
}

/*-------------------------------------------------------------*/
/*      Shell formatted print support           */
/*-------------------------------------------------------------*/
static void uli2a(unsigned long int num, unsigned int base, int uc, char * bf)
{
    int n = 0;
    unsigned int d = 1;
    while (num / d >= base)
        d *= base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}

static void li2a(long num, char * bf)
{
    if (num < 0) {
        num = -num;
        *bf++ = '-';
    }
    uli2a(num, 10, 0, bf);
}

static void ui2a(unsigned int num, unsigned int base, int uc, char * bf)
{
    int n = 0;
    unsigned int d = 1;
    while (num / d >= base)
        d *= base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}

static void i2a(int num, char * bf)
{
    if (num < 0) {
        num = -num;
        *bf++ = '-';
    }
    ui2a(num, 10, 0, bf);
}

static int a2d(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    else return -1;
}

static char a2i(char ch, const char** src, int base, int* nump)
{
    const char* p = *src;
    int num = 0;
    int digit;
    while ((digit = a2d(ch)) >= 0) {
        if (digit > base) break;
        num = num * base + digit;
        ch = *p++;
    }
    *src = p;
    *nump = num;
    return ch;
}

static void putchw(int n, char z, char* bf)
{
    char fc = z ? '0' : ' ';
    char ch;
    char* p = bf;
    while (*p++ && n > 0)
        n--;
    while (n-- > 0)
        shell_writer(fc);
    while ((ch = *bf++))
        shell_writer(ch);
}

static void shell_format(const char * fmt, va_list va)
{
    char bf[12];
    char ch;
    char lz;
    char lng;
    int w;

    //rej - doute sur le while
    while ((ch = *(fmt++))) {
        if (ch != '%') {
            shell_writer(ch);
        } else {
            lz = 0;
            lng = 0;
            w = 0;
            ch = *(fmt++);
            if (ch == '0') {
                ch = *(fmt++);
                lz = 1;
            }

            if (ch >= '0' && ch <= '9') {
                ch = a2i(ch, &fmt, 10, &w);
            }

            if (ch == 'l') {
                ch = *(fmt++);
                lng = 1;
            }

            switch (ch) {
            case 0:
                goto abort;
            case 'u':
            {
                if (lng)
                    uli2a(va_arg(va, unsigned long), 10, 0, bf);
                else
                    ui2a(va_arg(va, unsigned int), 10, 0, bf);
                putchw(w, lz, bf);
                break;
            }
            case 'd':
            {
                if (lng)
                    li2a(va_arg(va, unsigned long), bf);
                else
                    i2a(va_arg(va, int), bf);
                putchw(w, lz, bf);
                break;
            }
            case 'x': case 'X':
                if (lng)
                    uli2a(va_arg(va, unsigned long), 16, (ch == 'X'), bf);
                else
                    ui2a(va_arg(va, unsigned int), 16, (ch == 'X'), bf);
                putchw(w, lz, bf);
                break;
            case 'c':
                shell_writer((char) (va_arg(va, int)));
                break;
            case 's':
                putchw(w, 0, va_arg(va, char*));
                break;
            case '%':
                shell_writer(ch);
            default:
                break;
            }
        }
    }
abort:
    ;
}
