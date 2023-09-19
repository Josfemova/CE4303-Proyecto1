#include "parse.h"

#include <argp.h>
#include <stdlib.h>
#include <string.h>

static char doc[] = "verificar_filtro -- verificación del filtro laplaciano y "
                    "sincronización de procesos entre procesadores";

static char args_doc[] = "";

static struct argp_option options[] = {
    {"cortex", 'c', "PORCENTAJE", 0,
     "Especificar qué porcentaje de la imagen ejecutar en el \"procesador "
     "Cortex\" (default: 0).",
     0},
    {0},
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 'c':;
    char *endptr;

    float percent = strtof(arg, &endptr);
    if (arg == endptr) {
      argp_error(state, "Porcentaje de opción -c no es un número válido");
    }
    if (percent < 0 || percent > 1) {
      argp_error(state, "Porcentaje debe ser un número entre 0-1");
    }

    arguments->percent_cortex = percent;
    arguments->percent_qsys = 1 - percent;
    break;

  case ARGP_KEY_ARG:
    argp_usage(state);
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

// Esta función puede llamar exit()
struct arguments parse_args(int argc, char **argv) {
  struct arguments arguments;

  /* Default values. */
  arguments.percent_qsys = 1;
  arguments.percent_cortex = 0;

  error_t ret = argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (ret != 0) {
    fprintf(stderr, "Error leyendo argumentos: %s\n", strerror(ret));
    exit(ret);
  }

  return arguments;
}
