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

bool clapc_parse_safe(s_clap_arg* args[], char*** argv_ptr, char** error)
{
  *error = NULL;

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
        argv++;
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
      asprintf(error, "Invalid argument '%s'\n", arg);
      return false;
    }

    if (clap_arg->type != CLAP_ARG_TYPE_BOOL) {
      // We need to consume the next arg
      if (!argv[1]) {
        asprintf(error, "Missing positional argument for '%s'\n", *argv);
        return false;
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
        asprintf(error, "Invalid argument type\n");
        return false;
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

  *argv_ptr = argv;
  return true;
}

void clapc_parse(s_clap_arg* args[], char*** argv_ptr)
{
  char* error = NULL;
  if (!clapc_parse_safe(args, argv_ptr, &error)) {
    die(1, "%s", error);
  }
}

void clapc_print_help(
  const char* program_name, const char* description, s_clap_arg* args[])
{
  int largest_name = 2;

  for (int i = 0; args[i] != NULL; i++) {
    s_clap_arg* arg = args[i];
    if (arg->name == NULL) {
      continue;
    }
    int name_len = strlen(arg->name) + 2;
    int short_name_len = arg->short_name == 0 ? 0 : 2;

    int total_length = name_len + short_name_len +
      // If both a short and long name are present, add ", " between them
      (short_name_len > 0 ? 2 : 0);

    // TODO: in the future we may want to include <value> for non-bool args

    if (total_length > largest_name) {
      largest_name = total_length;
    }
  }

  printf("%s\n\n%s\n\n", program_name, description);

  printf("Options:\n");

  char buffer[1024];

  for (int i = 0; args[i] != NULL; i++) {
    s_clap_arg* arg = args[i];
    bool has_long_name = arg->name != NULL;
    bool has_short_name = arg->short_name != 0;

    sprintf(buffer, "%s%s%s%c", has_long_name ? "--" : "",
      has_long_name ? arg->name : "",
      has_long_name && has_short_name ? ", -" : "",
      has_short_name ? arg->short_name : 0);

    printf("  %-*s  %s\n", largest_name, buffer, arg->description);
  }

  printf("\n");
}

void clapc_arg_free(s_clap_arg* arg)
{
  if (arg->value) {
    free(arg->value);
    arg->value = NULL;
  }
}

void clapc_args_free(s_clap_arg* args[])
{
  for (int i = 0; args[i] != NULL; i++) {
    clapc_arg_free(args[i]);
  }
}
