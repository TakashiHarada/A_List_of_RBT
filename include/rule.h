/* rule.h 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __TANAKALAB_RULE_H__
#define __TANAKALAB_RULE_H__

#ifndef __TANAKALAB_BOOL_H__
#include "bool.h"
#endif

#ifndef __TANAKALAB_LIST_H__
#include "list.h"
#endif

#ifndef __TANAKALAB_TOOLS_H__
#include "tools.h"
#endif

#define HEADER_LENGTH 104

/*************************** Class Bench Rule ***************************/
struct C_RULE {
  unsigned num;
  char* sa;
  char* da;
  list_string* sp;
  list_string* dp;
  char* prot;
};
typedef struct C_RULE c_rule;

struct C_RULELIST {
  unsigned n;
  c_rule* r;
};
typedef struct C_RULELIST c_rulelist;

struct C_HEADERLIST {
  unsigned n;
  char** h;
};
typedef struct C_HEADERLIST c_headerlist;

void c_rulelist_print(c_rulelist*);
c_rulelist* read_class_bench_rule_list(char*);
void free_class_bench_rule_list(c_rulelist*);

/* char** read_header_list(char*); */

/* void free_header_list(char**); */

void c_rulelist_print(c_rulelist* rulelist) {
  unsigned i, j;
  unsigned d = floor(log10(rulelist->n)) + 1;
  for (i = 0; i < rulelist->n; ++i) {
    printf("R[%*d] : SA   = %s\n", d, rulelist->r[i].num, rulelist->r[i].sa) ;
    for (j = 0; j < d+6; ++j) { putchar(' '); } printf("DA   = %s\n", rulelist->r[i].da); 
    for (j = 0; j < d+6; ++j) { putchar(' '); } printf("SP   = "); list_string_print(rulelist->r[i].sp); putchar('\n');
    for (j = 0; j < d+6; ++j) { putchar(' '); } printf("DP   = "); list_string_print(rulelist->r[i].dp); putchar('\n');
    for (j = 0; j < d+6; ++j) { putchar(' '); } printf("PROT = %s\n", rulelist->r[i].prot);
  }
}

c_rulelist* read_class_bench_rule_list(char* rule_file_name) {
  FILE *fp;
  if (NULL == (fp = fopen(rule_file_name, "r"))) {
    fprintf(stderr, "ERROR: can't open the rule file.\n");
    exit(1);
  }

  c_rulelist* rulelist = (c_rulelist*)malloc(sizeof(c_rulelist));
  
  unsigned n = 0;
  { /* count the number of rules */
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while (-1 != (read = getline(&line, &len, fp))) ++n;
  }

  rulelist->n = n;
  
  char SA[33], DA[33], PROT[9];
  unsigned SPL, SPH, DPL, DPH;

  c_rule* rs = (c_rule*)malloc(n*sizeof(c_rule));
  unsigned i;
  rewind(fp);
  for (i = 0; EOF != fscanf(fp,"%s %s %u-%u %u-%u %s",SA,DA,&SPL,&SPH,&DPL,&DPH,PROT); ++i) {
    /* printf("%s %s %u : %u %u : %u %s\n", SA, DA, SPL, SPH, DPL, DPH, PROT); */
    rs[i].num = i+1;
    rs[i].sa = (char*)malloc(33*sizeof(char));
    rs[i].da = (char*)malloc(33*sizeof(char));
    rs[i].prot = (char*)malloc(9*sizeof(char));
    strcpy(rs[i].sa, SA); rs[i].sa[32] = '\0';
    strcpy(rs[i].da, DA); rs[i].da[32] = '\0';
    strcpy(rs[i].prot, PROT); rs[i].prot[8] = '\0';
    rs[i].sp = range_to_01m_strings(SPL,SPH,LOW,HIGH);
    rs[i].dp = range_to_01m_strings(DPL,DPH,LOW,HIGH);
  }
  rulelist->r = rs;

  fclose(fp);
  
  return rulelist;
}

void free_class_bench_rule_list(c_rulelist* rulelist) {
  unsigned i;
  for (i = 0; i < rulelist->n; ++i) {
    free(rulelist->r[i].sa);
    free(rulelist->r[i].da);
    free(rulelist->r[i].prot);
    list_string_clear(rulelist->r[i].sp);
    list_string_clear(rulelist->r[i].dp);
  }
  free(rulelist->r);
}

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
