
/* ------------------------------------------------------------------------ */
/**
   @file    ts3w.c
   @author  F. Indot
   @brief   Implements an easy to use teamspeak3 server wrapper.

   This utility short circuit the teamspeak3 server software default
   startscript by exposing a standard command line interface. Arguments
   are not passed directly to the executalbe, instead, those are fed to the
   ini configuration file.
*/
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <dirent.h>
#include <errno.h>

#include "../iniparser/src/dictionary.h"
#include "../iniparser/src/iniparser.h"

/* -------------------------------------------------------------------------
                                   Constants
   ------------------------------------------------------------------------- */

static char doc[]                   = "A simple teamspeak3 server wrapper.";
static char args_doc[]              = "";

static int INITIALIZE               = 0;
static int SAVE                     = 0;
static int VANILLA                  = 1;
static char configuration_path[]    = "/etc/teamspeak/conf.ini";
static char BINARY_PATH[]           = "/usr/local/share/teamspeak/ts3server";

/* -------------------------------------------------------------------------
                                   Functions
   ------------------------------------------------------------------------- */

int checkdir(char *path)
{
    DIR *dir = opendir(path);
    int retval = 0;

    if(dir)
    {
        closedir(dir);
        retval = 1;
    }

    return retval;
}

void init_default_conf()
{
    FILE *default_conf, *conf;
    char ch;

    printf("Initializing default configuration... ");

    default_conf = fopen("/tmp/ts3_conf.ini", "r");
    conf = fopen(configuration_path, "w");

    while((ch = fgetc(default_conf)) != EOF)
        fputc(ch, conf);

    fclose(default_conf);
    fclose(conf);

    printf("Done.\n");
}

void start_vanilla(int argc, char **argv)
{
    int cpt;
    int shift;
    char **command;

    shift = 0;
    for(cpt = 0; cpt < argc; cpt++)
    {
        if(shift)
        {
            argv[cpt] = argv[cpt + 1];
        }

        else if(strcmp(argv[cpt], "-v") || strcmp(argv[cpt], "--vanilla"))
        {
            argv[cpt] = argv[cpt + 1];
            shift = 1;
        }
    }
    argc--;

    command = malloc((argc) * sizeof(char *));
    command[0] = malloc(sizeof(BINARY_PATH));
    command[0] = BINARY_PATH;
    for(cpt = 1; cpt <= argc; cpt++)
    {
        command[cpt] = malloc(sizeof(argv[cpt]));
        command[cpt] = argv[cpt];
    }

    if(!execv(BINARY_PATH, command))
    {
        fprintf(stderr, "ERROR %d : %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

/* -------------------------------------------------------------------------
                               Arguments parsing
   ------------------------------------------------------------------------- */

static struct argp_option options[] =
{
    { "initialize", 'i', 0, 0,
      "Initialize the server configuration and database" },
    { "log-path", 'l', "PATH", 0, "Specify the logging directory" },
    { "backend", 'b', "SQL_BACKEND", 0,
      "Specify which database backend to use, default to sqlite" },

    { 0 }
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    dictionary* rtconf = state->input;

    switch(key)
    {
        case 'i':
            INITIALIZE = 1;
            break;

        case 'b':
            iniparser_set(rtconf, ":dbplugin", arg);
            break;

        case 'l':
            if(!checkdir(arg))
                argp_failure(state, 1, 0, "Unable to access \"%s\"", arg);
            iniparser_set(rtconf, ":logpath", arg);
            break;

        case 's':
            SAVE = 1;
            break;

        case 'v':
            VANILLA = 1;
            return 0;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

/* -------------------------------------------------------------------------
                                      Main
   ------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
    dictionary* configuration;
    FILE* configuration_file;

    configuration = iniparser_load(configuration_path);
    if(!configuration)
    {
        init_default_conf();
        configuration = iniparser_load(configuration_path);
    }

    argp_parse(&argp, argc, argv, 0, 0, configuration);

    if(VANILLA)
        start_vanilla(argc, argv);

    if(SAVE)
    {
        configuration_file = fopen(configuration_path, "w");
        iniparser_dump_ini(configuration, configuration_file);
        fclose(configuration_file);
    }

    iniparser_freedict(configuration);

    return EXIT_SUCCESS;
}
