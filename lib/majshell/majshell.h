/*
    rej version 0.0.1 -> creation
*/

#ifndef MAJSHELL_H
#define MAJSHELL_H
/*-------------------------------------------------------------*/
/*      Includes and dependencies           */
/*-------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <Arduino.h>

/**
 * Defines the maximum number of commands that can be registered
 */
#define CONFIG_SHELL_MAX_COMMANDS       10
/**
 * Defines the maximum characters that the input buffer can accept
 */
#define CONFIG_SHELL_MAX_INPUT          40
/**
 * Configures the maximum number of arguments per command that can be accepted
 */
#define CONFIG_SHELL_MAX_COMMAND_ARGS       2

/**
 * Defines the buffer for formatted string output from program memory
 */
#define CONFIG_SHELL_FMT_BUFFER         40

/*-------------------------------------------------------------*/
/*      Macros and definitions              */
/*-------------------------------------------------------------*/
#define SHELL_ASCII_NUL             0x00
#define SHELL_ASCII_BEL             0x07
#define SHELL_ASCII_BS              0x08
#define SHELL_ASCII_HT              0x09
#define SHELL_ASCII_LF              0x0A
#define SHELL_ASCII_CR              0x0D
#define SHELL_ASCII_ESC             0x1B
#define SHELL_ASCII_DEL             0x7F
#define SHELL_ASCII_US              0x1F
#define SHELL_ASCII_SP              0x20
#define SHELL_VT100_ARROWUP         'A'
#define SHELL_VT100_ARROWDOWN       'B'
#define SHELL_VT100_ARROWRIGHT      'C'
#define SHELL_VT100_ARROWLEFT       'D'

#define SHELL_RET_SUCCESS           0
#define SHELL_RET_FAILURE           1
#define SHELL_RET_IOPENDING         -1

#define SHELL_VERSION_STRING        "maj_shell Version 0.0.1"

/*-------------------------------------------------------------*/
/*      Typedefs enums & structs            */
/*-------------------------------------------------------------*/

/**
 *  Type definition for all the programs invoked by the shell (function pointer)
 */
typedef int (*shell_program_t) (int, char **);

/*
 * Type definition for a function that sends a single character to the remote
 * terminal screen (function pointer)
 */
typedef void (*shell_writer_t) (char);

/*
 * Type definition for a function that reads a single character from the remote
 * terminal (function pointer)
 */
typedef int (*shell_reader_t) (char *);

/**
 * This enumeration defines the errors printed by the programs called by the
 * shell.
 */
enum shell_errors {
    /** There are missing arguments for the command */
    E_SHELL_ERR_ARGCOUNT = 0,
    /** The program received an argument that is out of range */
    E_SHELL_ERR_OUTOFRANGE,
    /** The program received an argument with a value different than expected */
    E_SHELL_ERR_VALUE,
    /** Invalid action requested for the current state */
    E_SHELL_ERR_ACTION,
    /** Cannot parse the user input */
    E_SHELL_ERR_PARSE,
    /** Cannot access storage device or memory device */
    E_SHELL_ERR_STORAGE,
    /** IO device error caused program interruption */
    E_SHELL_ERR_IO,
    /** Other kinds of errors */
    E_SHELL_ERROR_UNKNOWN,
};

/**
 * This structure holds the data for every command registered on the shell
 */
struct shell_command_entry {
    shell_program_t shell_program;
    const char * shell_desc_string;
    const char * shell_args_string;
    const char * shell_command_string;
};

/*-------------------------------------------------------------*/
/*      Function prototypes             */
/*-------------------------------------------------------------*/
#ifdef  __cplusplus
extern "C" {
#endif
    /**
     * @brief Prepares the command line interface
     *
     * Initializes the resources used by the command line interface. The main
     * 'program' for the command line interface where all the data is handled is
     * shell_task().
     *
     * @param reader The callback function used to get a character from the
     * stream.
     *
     * @param writer The callback function used to write a character to the
     * stream.
     *
     * @return Returns true if the shell was successfully initialized, returns
     * false otherwise.
     */
    bool shell_init(shell_reader_t reader, shell_writer_t writer, char* ver);

    /**
     * @brief Registers a command with the command line library
     *
     * Registers a command on the available command list. The name of the
     * command and the function that implements it's functionality should be
     * provided.
     *
     * Register command on alphabetical sort for print all command
     * @param program The type shell_program_t is a pointer to a function
     * that is executed when the written command matches the associated name
     *
     * @param string A string containing the name of the command to be
     * registered.
     *
     * @return Returns true if command was successfully added to the command
     * list, or returns false if something fails (no more commands can be
     * registered).
     */
    bool shell_register(shell_program_t program, const char * string_desc,
            const char * string_args, const char * string_cmd);

    /**
     * @brief Prints error messages to the terminal screen
     *
     * This function presents an alternative for displaying program errors in a
     * uniform format.
     *
     * @param error The code (ID) of the error to print
     *
     * @param field The name of the parameter or variable where the error was
     * detected.
     */
    void shell_print_error(int error, const char * field);

    /**
     * @brief Prints a null terminated string to the terminal
     *
     * Displays a string on the terminal. The string should be null terminated.
     *
     * @param string The string to send to the terminal
     */
    int shell_print(const char * string);

    /**
     * @brief Prints a string and moves the cursor to the next line
     *
     * Displays a string on the terminal and moves the cursor to the next line. The
     * string should be null terminated.
     *
     * @param string The string to send to the terminal
     */
    void shell_println(const char * string);

    /**
     * @brief Prints formatted text to the terminal
     *
     * Displays a string (fmt) on the terminal. If the string includes format
     * specifiers (subsequences beginning with '%'), the additional arguments
     * following format are formatted and inserted in the resulting string
     * replacing their respective specifiers.
     *
     * This function implements it�s own mechanism for text formatting. It
     * doesn�t rely on the native print functions.
     *
     * @param fmt The string to send to the terminal, the string can include
     * format specifiers in a similar fashion to printf standard function.
     *
     * @param ... Aditional arguments that are inserted on the string as text
     */
    void shell_printf(const char * fmt, ...);

    /**
     * @brief Main Shell processing loop
     *
     * This function implements the main functionality of the command line
     * interface this function should be called frequently so it can handle
     * the input from the data stream.
     */
    void shell_task();

    /**
     * @brief Prints formatted text to the terminal from flash
     *
     * Displays a string (fmt) on the terminal. If the string includes format
     * specifiers (subsequences beginning with '%'), the additional arguments
     * following format are formatted and inserted in the resulting string
     * replacing their respective specifiers.
     *
     * This function is designed to be used with strings stored in flash.
     *
     * This function implements it�s own mechanism for text formatting. It
     * doesn�t rely on the native print functions.
     *
     * @param fmt The string to send to the terminal, the string can include format
     * specifiers in a similar fashion to printf standard function.
     *
     * @param ... Aditional arguments that are inserted on the string as text
     */
    void shell_printf_pm(const char * fmt, ...);

#ifdef  __cplusplus
}
#endif

#endif
