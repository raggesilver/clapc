#include <clapc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctest.h"

// Tests =======================================================================

void explicit_long_boolean()
{
  s_clap_arg json_arg = {
    .name = "json",
    .type = CLAP_ARG_TYPE_BOOL,
    .description = "If true, output will be in JSON format",
    .required = true,
  };

  char* error;
  char* argv[] = { "clapc_test", "--json", "false", NULL };
  char** argv_ptr = argv;
  s_clap_arg* args[] = { &json_arg, NULL };
  bool result = clapc_parse_safe(args, &argv_ptr, &error);

  expect(error == NULL);
  expect(result);
  expect(clap_arg_get_bool(&json_arg) == false);

  clapc_args_free(args);
}

/**
 * Ensure that a boolean argument can be set implicitly. That is, if the
 * argument is present, even without a value, it is true.
 */
void implicit_boolean()
{
  s_clap_arg json_arg = {
    .name = "json",
    .short_name = 'j',
    .type = CLAP_ARG_TYPE_BOOL,
    .description = "If true, output will be in JSON format",
    .required = true,
  };

  s_clap_arg* args[] = { &json_arg, NULL };
  {
    char* error;
    char* argv[] = { "clapc_test", "--json", NULL };
    char** argv_ptr = argv;
    bool result = clapc_parse_safe(args, &argv_ptr, &error);

    expect(error == NULL);
    expect(result);
    expect(clap_arg_get_bool(&json_arg) == true);

    clapc_args_free(args);
  }

  {
    char* error;
    char* argv[] = { "clapc_test", "-j", NULL };
    char** argv_ptr = argv;
    bool result = clapc_parse_safe(args, &argv_ptr, &error);

    expect(error == NULL);
    expect(result);
    expect(clap_arg_get_bool(&json_arg) == true);

    clapc_args_free(args);
  }
}

void string_arguments(void)
{
  s_clap_arg extensions_arg = {
    .name = "extensions",
    .type = CLAP_ARG_TYPE_STRING,
    .description = "A comma-separated list of file extensions to include",
    .required = true,
  };

  s_clap_arg* args[] = { &extensions_arg, NULL };

  {
    char* error;
    char* argv[] = { "clapc_test", "--extensions", "c,h", NULL };
    char** argv_ptr = argv;
    bool result = clapc_parse_safe(args, &argv_ptr, &error);

    expect(error == NULL);
    expect(result);
    expect(strcmp(clap_arg_get_string(&extensions_arg), "c,h") == 0);

    clapc_args_free(args);
  }
}

int main(void)
{
  begin_suite();

  test(explicit_long_boolean);
  test(implicit_boolean);

  test(string_arguments);

  return end_suite();
}
