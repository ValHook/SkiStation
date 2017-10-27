/*
 *  libsaferesource.c
 *  Useful C Libraries
 *
 *  Created by Valentin on 14/11/2013.
 *  Copyright (c) 2013 Valentin Mercier. All rights reserved.
 */

#include "libsaferesource.h"

/* Reference counting variables, initialized at 0 at compile time */
static int nb_alloc;
static int nb_files;


/* Safer resource functions
 * ! Do not redefine malloc etc with these names, because this may cause
 * an unexpected behavior */

 /* Sha1 checker */
 void sha1_file_check(const char *filename, unsigned char checksum[20])
 {
     unsigned char sha1_digest[20];
     sha1_file(filename, sha1_digest);
     if (memcmp(checksum, sha1_digest,sizeof(sha1_digest)) != 0)
        die("Checksum failed");
 }

/* MEMORY MANGEMENT FUNCTIONS */
void *malloc_ex(size_t length)
{
        void *ptr = malloc(length);

        if (!ptr)
                die("Bad allocation");

        nb_alloc ++;
        return ptr;
}

void *calloc_ex(size_t num, size_t size)
{
        void *ptr = calloc(num, size);

        if (!ptr)
                die("Bad allocation");

        nb_alloc ++;
        return ptr;
}

void free_ex(void *ptr)
{
        if (!ptr)
                die("Cannot free NULL pointer");

        free(ptr);
        nb_alloc--;
}
char *strdup_ex(const char *str)
{
        char *r_str;
        r_str = strdup(str);

        if (!r_str)
                die("Bad allocation");

        nb_alloc ++;
        return r_str;
}



/* FILE FUNCTIONS */
FILE *fopen_ex(const char *path, const char *flags)
{
        FILE *fp = fopen(path, flags);

        if (!fp)
                die("Could not open file %s", path);

        nb_files++;
        return fp;
}

/* Rigorous fscanf */
int fscanf_ex(int expected, FILE *fp, const char *fmt, ...)
{
        va_list ap;
	int ret;

        if (!fp)
                die("Cannot read invalid file");

	va_start(ap, fmt);
	ret = vfscanf(fp, fmt, ap);
	va_end(ap);

        if (ret != expected) {
                die("Invalid file format");
        }

        return ret;
}


int fclose_ex(FILE *fp)
{
        if (!fp)
                die("Could not close invalid file");

        nb_files--;
        return fclose(fp);
}



/* MISC FUNCTIONS */
/* Reference couter overview */
void show_refcounter(void)
{
        printf("Memory allocation result: %d\n", nb_alloc);
        printf("File opening result: %d\n", nb_files);
}

/* Exit function with error message */
void die(const char *error, ...)
{
        va_list ap;
        va_start(ap, error);
	vfprintf(stderr, error, ap);
	va_end(ap);
	getchar();
        exit(EXIT_FAILURE);
}
