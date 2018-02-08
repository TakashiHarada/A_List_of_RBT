/* header.h */

#ifndef __TANAKALAB_HEADER_H__
#define __TANAKALAB_HEADER_H__

#include <string.h>

#ifndef __TANAKALAB_BOOL_H__
#include "bool.h"
#endif

struct HEADER {
  unsigned num;
  char* string;
};
typedef struct HEADER header;

struct HEADERLIST {
  unsigned n;
  header* h;
};
typedef struct HEADERLIST headerlist;

headerlist* read_header_list(char*);
void header_list_print(headerlist*);
void free_header_list(headerlist*);

void header_list_print(headerlist* L) {
  unsigned i;
  if (NULL == L || 0 == L->n) { return; }
  printf("h[%d] : %s\n", L->h[0].num, L->h[0].string);
  for (i = 1; i < L->n; ++i) { printf("h[%d] : %s\n", L->h[i].num, L->h[i].string); }
}

void free_header_list(headerlist* L) {
  unsigned i = 0;
  while (i < L->n) {
    free(L->h[i].string);
    ++i;
  }
}

headerlist* read_header_list(char* header_file_name) {
  FILE* fp;
  
  if (NULL == (fp = fopen(header_file_name,"r"))) {
    fprintf(stderr,"ERROR: Can't read the header list file.\n");
    exit(1);
  }

  /* get a length of the rule; and check the lengths of rule and header */
  char* line = NULL;
  size_t len = 0;

  int w = 0;
  w = getline(&line, &len, fp);
  if (-1 == w) {
    fprintf(stderr,"ERROR: Can't read the rule list file.\n");
    fclose(fp);
    exit(1);
  }
  /* if ((unsigned)w-1 != w) { */
  /*   fprintf(stderr,"ERROR: header length doesn't match the length of rule.\n"); */
  /*   fclose(fp); */
  /*   exit(1); */
  /* } */

  headerlist* HL = (headerlist*)malloc(sizeof(headerlist));
  
  /* count a number of headers in the header list */
  unsigned n = 1; // already read the first line
  while (getline(&line, &len, fp) != -1) { ++n; }
  HL->n = n;
  HL->h = (header*)malloc(n*sizeof(header));
  
  /* read headers */
  rewind(fp);
  unsigned i = 0;
  while (getline(&line, &len, fp) != -1) {
    w = strlen(line);
    HL->h[i].num = i+1;
    HL->h[i].string = (char*)malloc(w*sizeof(char));
    strcpy(HL->h[i].string, line);
    HL->h[i].string[w-1] = '\0';
    line = NULL;
    ++i;
  }
  
  return HL;
}

/* Class Bench Header List */

#define HEADER_LENGTH 104

struct C_HEADERLIST {
  unsigned n;
  char** h;
};
typedef struct C_HEADERLIST c_headerlist;

/* char** read_header_list(char*); */
/* void free_header_list(char**); */

/* char** read_header_list(char* header_file_name) { */
/*   FILE *fp; */
/*   if (NULL == (fp = fopen(header_file_name, "r"))) { */
/*     fprintf(stderr, "ERROR: can't open the header file.\n"); */
/*     exit(1); */
/*   } */

/*   unsigned hn = 0; */
/*   { /\* count the number of rules *\/ */
/*     char* line = NULL; */
/*     size_t len = 0; */
/*     ssize_t read; */
/*     while (-1 != (read = getline(&line, &len, fp))) ++hn; */
/*   } */

/*   char** headerlist = (char**)malloc(hn*sizeof(char*)); */
/*   unsigned i; */
/*   rewind(fp); */
  
/*   char SA[33], DA[33], SP[17], DP[17], PROT[9]; */
/*   for (i = 0; EOF != fscanf(fp,"%s %s %s %s %s",SA,DA,SP,DP,PROT); ++i) { */
/*     headerlist[i] = (char*)malloc((HEADER_LENGTH+1)*sizeof(char)); */
/*     unsigned j; */
/*     for (j = 0; j < 32; ++j) { */
/*       headerlist[i][31-j] = SA[j]; /\* reverse the string of SA *\/ */
/*       headerlist[i][32+j] = DA[j]; */
/*     } */
/*     for (j = 0; j < 16; ++j) { */
/*       headerlist[i][64+j] = SP[j]; */
/*       headerlist[i][80+j] = DP[j]; */
/*     } */
/*     for (j = 0; j < 8; ++j) { headerlist[i][96+j] = PROT[j]; } */
/*   } */
/*   fclose(fp); */
  
/*   return headerlist; */
/* } */

/* void free_header_list(char** headerlist) { */
/*   unsigned i; */
/*   for (i = 0; i < hn; ++i) { free(headerlist[i]); } */
/*   free(headerlist); */
/* } */

#endif
