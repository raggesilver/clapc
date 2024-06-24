#pragma once
#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_CLAPC
#define CLAPC_PUBLIC __declspec(dllexport)
#else
#define CLAPC_PUBLIC __declspec(dllimport)
#endif
#else
#ifdef BUILDING_CLAPC
#define CLAPC_PUBLIC __attribute__((visibility("default")))
#else
#define CLAPC_PUBLIC
#endif
#endif

#include <assert.h>
#include <stdbool.h>

typedef enum {
  CLAP_ARG_TYPE_BOOL = 0,
  CLAP_ARG_TYPE_INT,
  CLAP_ARG_TYPE_FLOAT,
  CLAP_ARG_TYPE_STRING,
} CLAPC_PUBLIC e_clap_arg_type;

/**
 * Represents a command-line argument. This is used to define the arguments that
 * the user can provide to the program.
 */
typedef struct {
  /**
   * The name of the argument. This is the long name of the argument, e.g.
   * "--json"
   */
  const char* name;
  /**
   * The short name of the argument. This is the short name of the argument,
   * e.g. "-j"
   */
  const char short_name;
  /**
   * The type of the argument. This can be one of the following:
   *
   * - CLAP_ARG_TYPE_BOOL
   * - CLAP_ARG_TYPE_INT
   * - CLAP_ARG_TYPE_FLOAT
   * - CLAP_ARG_TYPE_STRING
   */
  const e_clap_arg_type type;
  /**
   * The value of the argument. This is a pointer to the value of the argument.
   * The type of the value is determined by the {@link type} field.
   */
  void* value;
  /**
   * The description of the argument. This is a human-readable description of
   * the argument. This is used to generate the help message for the argument.
   */
  const char* description;
  /**
   * Whether the argument is required. If this is true, the argument must be
   * provided by the user. If this is false, the argument is optional.
   */
  bool required;
} CLAPC_PUBLIC s_clap_arg;

/**
 * Gets the value of a boolean argument.
 *
 * @param arg The argument to get the value of
 * @return The value of the argument
 */
#define clap_arg_get_bool(arg)                                                 \
  ({                                                                           \
    assert((arg)->type == CLAP_ARG_TYPE_BOOL);                                 \
    assert((arg)->value != NULL);                                              \
    *(bool*)(arg)->value;                                                      \
  })

/**
 * Gets the value of an integer argument.
 * @param arg The argument to get the value of
 * @return The value of the argument
 */
#define clap_arg_get_int(arg)                                                  \
  ({                                                                           \
    assert((arg)->type == CLAP_ARG_TYPE_INT);                                  \
    assert((arg)->value != NULL);                                              \
    *(int*)(arg)->value;                                                       \
  })

/**
 * Gets the value of a float argument.
 * @param arg The argument to get the value of
 * @return The value of the argument
 */
#define clap_arg_get_float(arg)                                                \
  ({                                                                           \
    assert((arg)->type == CLAP_ARG_TYPE_FLOAT);                                \
    assert((arg)->value != NULL);                                              \
    *(float*)(arg)->value;                                                     \
  })

/**
 * Gets the value of a string argument.
 * @param arg The argument to get the value of
 * @return The value of the argument
 */
#define clap_arg_get_string(arg)                                               \
  ({                                                                           \
    assert((arg)->type == CLAP_ARG_TYPE_STRING);                               \
    (char*)(arg)->value;                                                       \
  })

/**
 * Parses the command-line arguments and populates the values of the arguments
 * in the {@link args} array.
 *
 * @param args The array of arguments to parse. This array should be
 * null-terminated
 * @param argv_ptr A pointer to the command-line arguments. This pointer will be
 * updated to point to the next argument after the parsed arguments.
 */
CLAPC_PUBLIC void clapc_parse(s_clap_arg* args[], char*** argv_ptr);
