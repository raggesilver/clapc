#include <clapc.h>
#include <stdio.h>
#include <stdlib.h>

#define bool_to_str(b) ((b) ? "true" : "false")

int main([[maybe_unused]] int _argc, char** argv)
{
  s_clap_arg help_arg = {
    .name = "help",
    .short_name = 'h',
    .type = CLAP_ARG_TYPE_BOOL,
    .description = "Prints this help message",
    .required = false,
  };

  s_clap_arg json_arg = {
    .name = "json",
    .type = CLAP_ARG_TYPE_BOOL,
    .description = "If true, output will be in JSON format",
    .required = true,
  };

  s_clap_arg extensions_arg = {
    .name = "extensions",
    .short_name = 'e',
    .type = CLAP_ARG_TYPE_STRING,
    .description
    = "Comma-separated list of extensions to include in the output",
    .required = false,
  };

  s_clap_arg precision_arg = {
    .name = "precision",
    .short_name = 'p',
    .type = CLAP_ARG_TYPE_INT,
    .description = "The number of decimal places to include in the output",
    .required = false,
  };

  s_clap_arg threshold_arg = {
    .name = "threshold",
    .short_name = 't',
    .type = CLAP_ARG_TYPE_FLOAT,
    .description = "The threshold for including a value in the output",
    .required = false,
  };

  s_clap_arg* args[] = {
    &json_arg,
    &extensions_arg,
    &precision_arg,
    &threshold_arg,
    NULL,
  };

  clapc_parse(args, &argv);

  bool show_help = clap_arg_get_bool(&help_arg);

  if (show_help) {
    // clapc_print_help(args);
    return 0;
  }

  bool is_json = clap_arg_get_bool(&json_arg);
  char* extensions = clap_arg_get_string(&extensions_arg);

  printf("Json: %s\n", bool_to_str(is_json));
  printf("Extensions: %s\n", extensions);

  if (precision_arg.value) {
    printf("Precision: %d\n", clap_arg_get_int(&precision_arg));
  }

  if (threshold_arg.value) {
    printf("Threshold: %f\n", clap_arg_get_float(&threshold_arg));
  }

  void* to_free[]
    = { extensions, precision_arg.value, threshold_arg.value, json_arg.value };

  for (size_t i = 0; i < sizeof(to_free) / sizeof(void*); i++) {
    if (to_free[i]) {
      free(to_free[i]);
    }
  }

  return 0;
}
