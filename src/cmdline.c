//----------------------------------------------------------------------
//----------------------------------------------------------------------
//
// bridge_hla 
// Implementation for IEEE HLA bridges between federations (Compliant with CERTI RTI)
// 
// Copyright (C) 2023  ISAE-SUPAERO
//
// Authors: Benoit Breholée
//          Jean-Baptiste Chaudron
//
// email:   jean-baptiste.chaudron@isae-supaero.fr
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
/* Check for configure's getopt check result.  */
#ifndef HAVE_GETOPT_LONG
#include "getopt.h"
#else
#include <getopt.h>
#endif

#ifndef HAVE_STRDUP
#define strdup gengetopt_strdup
#endif /* HAVE_STRDUP */

#include "cmdline.h"


void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n", PACKAGE, VERSION);
}

void
cmdline_parser_print_help (void)
{
  cmdline_parser_print_version ();
  printf("\n"
  "Usage: %s [OPTIONS]...\n", PACKAGE);
  printf("   -h         --help           Print help and exit\n");
  printf("   -V         --version        Print version and exit\n");
  printf("   -cSTRING   --config=STRING  configuration file\n");
  printf("   -tINT      --timer=INT      timer\n");
  printf("   -v         --verbose        verbose mode (default=off)\n");
}


#ifndef HAVE_STRDUP
/* gengetopt_strdup(): automatically generated from strdup.c. */
/* strdup.c replacement of strdup, which is not standard */
static char *
gengetopt_strdup (const char *s)
{
  char *result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
} 
#endif /* HAVE_STRDUP */

int
cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info)
{
  int c;	/* Character of the parsed option.  */
  int missing_required_options = 0;	

  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->config_given = 0 ;
  args_info->timer_given = 0 ;
  args_info->verbose_given = 0 ;
#define clear_args() { \
  args_info->config_arg = NULL; \
  args_info->verbose_flag = 0;\
}

  clear_args();

  optarg = 0;
  optind = 1;
  opterr = 1;
  optopt = '?';

  while (1)
    {
      int option_index = 0;
      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "config",	1, NULL, 'c' },
        { "timer",	1, NULL, 't' },
        { "verbose",	0, NULL, 'v' },
        { NULL,	0, NULL, 0 }
      };

      c = getopt_long (argc, argv, "hVc:t:v", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          clear_args ();
          cmdline_parser_print_help ();
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          clear_args ();
          cmdline_parser_print_version ();
          exit (EXIT_SUCCESS);

        case 'c':	/* configuration file.  */
          if (args_info->config_given)
            {
              fprintf (stderr, "%s: `--config' (`-c') option given more than once\n", PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->config_given = 1;
          args_info->config_arg = strdup (optarg);
          break;

        case 't':	/* timer.  */
          if (args_info->timer_given)
            {
              fprintf (stderr, "%s: `--timer' (`-t') option given more than once\n", PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->timer_given = 1;
          args_info->timer_arg = atoi (optarg);
          break;

        case 'v':	/* verbose mode.  */
          if (args_info->verbose_given)
            {
              fprintf (stderr, "%s: `--verbose' (`-v') option given more than once\n", PACKAGE);
              clear_args ();
              exit (EXIT_FAILURE);
            }
          args_info->verbose_given = 1;
          args_info->verbose_flag = !(args_info->verbose_flag);
          break;


        case 0:	/* Long option with no short option */

        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          exit (EXIT_FAILURE);

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c\n", PACKAGE, c);
          abort ();
        } /* switch */
    } /* while */

  if (! args_info->config_given)
    {
      fprintf (stderr, "%s: '--config' ('-c') option required\n", PACKAGE);
      missing_required_options = 1;
    }
  
  if ( missing_required_options )
    exit (EXIT_FAILURE);

  return 0;
}
