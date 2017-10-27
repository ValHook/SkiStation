/*
 *  libsaferesource.h
 *  Useful C Libraries
 *
 *  Created by Valentin on 14/11/2013.
 *  Copyright (c) 2013 Valentin Mercier. All rights reserved.
 *
 *
 * Standard resource functions re-implemented with a success check
 * and a reference counter for debugging purposes */

#ifndef libsaferesource_h
#define libsaferesource_h

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "sha1.h"

/* Checksum checker */
void sha1_file_check(const char *, unsigned char[20]);

/* Safer standard resource functions prototypes */
void    *malloc_ex(size_t);
void    *calloc_ex(size_t, size_t);
void    free_ex(void *);
FILE    *fopen_ex(const char *, const char *);
int     fclose_ex(FILE *);
char    *strdup_ex(const char *);

/* Reference couter overview */
void    show_refcounter(void);

/* Rigorous fscanf */
int     fscanf_ex(int expected, FILE *, const char *, ...);

/* Exit function with error message */
void    die(const char *error, ...);

#endif
