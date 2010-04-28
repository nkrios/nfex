/*
 * main.c - main program driver
 *
 * 2009, 2010 Mike Schiffman <mschiffm@cisco.com> 
 *
 * Copyright (c) 2010 by Cisco Systems, Inc.
 * All rights reserved.
 * Based off of tcpxtract by Nicholas Harbour.
 */


#include "nfex.h"
#include "sessionlist.h"
#include "config.h"
#include "util.h"
#include "confy.h"
#include "search.h"

int
main(int argc, char *argv[])
{
    int c;
    ncc_t *ncc;
    char *device;
    u_int16_t flags;
    char capfname[128];
    char yyinfname[128];
#if (HAVE_GEOIP)
    char geoip_data[128];
#endif /** HAVE_GEOIP */
    char output_dir[128];
    char errbuf[PCAP_ERRBUF_SIZE];

    if (argc == 1)
    {
        usage(argv[0]);
    }

    flags = 0;
    device = NULL;
    memset(capfname,   0, sizeof (capfname));
    memset(yyinfname,  0, sizeof (yyinfname));
    memset(output_dir, 0, sizeof (output_dir));
#if (HAVE_GEOIP)
    memset(geoip_data, 0, sizeof (geoip_data));
#endif /** HAVE_GEOIP */
    while ((c = getopt(argc, argv, "c:d:G:gf:o:hVv")) != EOF)
    {
        switch (c)
        {
            case 'f':
                strncpy(capfname, optarg, 127);
                break;
            case 'd':
                device = strdup(optarg);
                break;
            case 'c':
                strncpy(yyinfname, optarg, 127);
                break;
#if (HAVE_GEOIP)
            case 'G':
                strncpy(geoip_data, optarg, 127);
                break;
            case 'g':
                flags |= NFEX_GEOIP;
                break;
#endif /** HAVE_GEOIP */
            case 'o':
                if (optarg[strlen(optarg) - 1] != '/')
                {
                    strncpy(output_dir, optarg, 126);
                    output_dir[strlen(optarg)] = '/';
                    output_dir[strlen(optarg) + 1] = '\0';
                }
                else
                {
                    strncpy(output_dir, optarg, 127); 
                }
                break;
            case 'h':
                usage(argv[0]);
                break;
            case 'V':
                flags |= NFEX_VERBOSE;
                break;
            case 'v':
                printf("%s v%s\n", PACKAGE, VERSION);
                return (EXIT_SUCCESS);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }

    if (optind < argc)
    {
        if (!device)
        {
            device = argv[optind];
        }
    }

    printf("nfex - realtime network file extraction engine\n");
#if (HAVE_GEOIP)
    ncc = control_context_init(output_dir, yyinfname, device, capfname, 
            geoip_data, flags, errbuf);
#else
    ncc = control_context_init(output_dir, yyinfname, device, capfname, 
            NULL, flags, errbuf);
#endif /** HAVE_GEOIP */

    if (ncc == NULL)
    {
        fprintf(stderr, "can't initialize program.\n");
        return (EXIT_FAILURE);
    }

    printf("program initialized, now the game can start...\n");

    the_game(ncc);
    stats(ncc);
    control_context_destroy(ncc);
    printf("program completed, normal exit\n");

    return(EXIT_SUCCESS);
}

void
usage(char *progname)
{
    printf("Usage: %s [OPTIONS] [[-d <DEVICE>] || [-f <FILE>]]\n"
           "  -f <FILE>       specify an input capture file\n"
           "  -d <DEVICE>     to specify a network device\n"
           "  -c <FILE>       specify config file\n"
#if (HAVE_GEOIP)
           "  -G              specify path to MaxMind geoIP database\n"
           "  -g              toggle geoIP mode on\n"
#endif /** HAVE_GEOIP */
           "  -o <DIRECTORY>  dump files here instead of cwd\n"
           "  -V              toggle verbose mode on\n"
           "  -v              display the version number\n"
           "  -h              this\n", progname);
    exit(1);    
}

/** EOF */
