#include "clapc.h"
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Print an error message and exit the program.
 * @param status The exit status.
 * @param fmt The format string.
 */
__attribute__((format(printf, 2, 3), noreturn)) static void die(
  int status, const char* fmt, ...)
{
  va_list args;
  va_start(args);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(status);
}

void clapc_parse(s_clap_arg* args[], char*** argv_ptr)
{
  char** argv =
    // Skip first argument (it's always the executable name)
    (*argv_ptr + 1);
  char* arg = *argv;

  while (arg && *arg) {
    // NOTE: right now we don't support out of order arguments. This means that
    // no flags can be parsed after the first positional agurment. In the future
    // we may want to put all positional arguments in a separate array and keep
    // parsing flags.
    //
    // Left-over positional argument.
    if (*arg != '-') {
      break;
    }

    bool is_long = false;

    if (arg[1] == '-') {
      // Found "--", skip parsing
      if (arg[2] == '\0') {
        break;
      }
      is_long = true;
    }

    // This is the name of the arg
    arg = arg + (is_long ? 2 : 1);

    auto clap_arg = ({
      s_clap_arg* clap_arg = NULL;
      if (is_long) {
        for (int i = 0; args[i] != NULL; i++) {
          if (args[i]->name && strcmp(args[i]->name, arg) == 0) {
            clap_arg = args[i];
            break;
          }
        }
      } else {
        for (int i = 0; args[i] != NULL; i++) {
          if (args[i]->short_name == *arg) {
            clap_arg = args[i];
            break;
          }
        }
      }
      clap_arg;
    });

    if (clap_arg == NULL) {
      die(1, "Invalid argument '%s'\n", arg);
    }

    if (clap_arg->type != CLAP_ARG_TYPE_BOOL) {
      // We need to consume the next arg
      if (!argv[1]) {
        die(1, "Missing positional argument for '%s'\n", *argv);
      }

      // TODO: real implementation (parse value according to type)

      switch (clap_arg->type) {
      case CLAP_ARG_TYPE_INT: {
        clap_arg->value = malloc(sizeof(int));
        long int value = strtol(argv[1], NULL, 10);
        if (value > INT_MAX || value < INT_MIN) {
          die(1, "Value out of range for argument '%s'\n", *argv);
        }
        *(int*)clap_arg->value = value;
        break;
      }
      case CLAP_ARG_TYPE_FLOAT: {
        clap_arg->value = malloc(sizeof(float));
        float fvalue = strtof(argv[1], NULL);
        *(float*)clap_arg->value = fvalue;
        break;
      }
      case CLAP_ARG_TYPE_STRING: {
        clap_arg->value = strdup(argv[1]);
        break;
      }
      default: {
        die(1, "Invalid argument type\n");
      }
      }

      argv++;
    } else {
      clap_arg->value = malloc(sizeof(bool));
      // If we are parsing a boolean argument, consume the next arg if it is
      // either "true" or "false"
      bool is_true = argv[1] && strcmp(argv[1], "true") == 0;
      bool is_false = !is_true && argv[1] && strcmp(argv[1], "false") == 0;

      if (is_true || is_false) {
        *(bool*)clap_arg->value = is_true;
        argv++;
      } else {
        *(bool*)clap_arg->value = true;
      }
    }

    // TODO: go through all args and check if required args are present

    arg = *++argv;
  }
}
