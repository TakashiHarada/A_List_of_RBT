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

/************************* Arbitraly Mask Rule **************************/
struct RULE {
  unsigned num;
  char* cond;
};
typedef struct RULE rule;

bool rule_eq(rule*, rule*);
void rule_print(rule*);
  
bool rule_eq(rule* r1, rule* r2) {
  return (r1->num == r2->num && !strcmp(r1->cond, r2->cond));
}

void rule_print(rule* r) { printf("r[%d] = %s", r->num, r->cond); }

struct LIST_RULE_CELL {
  rule* key;
  struct LIST_RULE_CELL* prev;
  struct LIST_RULE_CELL* next;
};
typedef struct LIST_RULE_CELL list_rule_cell;

struct LIST_RULE {
  unsigned* sigma; /* order of column */
  list_rule_cell* head;
  list_rule_cell* last;
  unsigned size;
};
typedef struct LIST_RULE list_rule;

rule* list_rule_head(list_rule*);
bool list_rule_eq(list_rule*, list_rule*);
bool list_rule_is_empty(list_rule*);
list_rule_cell* list_rule_search(list_rule*, rule*);
void list_rule_insert(list_rule*, rule*);
void list_rule_insert_sub(list_rule*, list_rule_cell*);
void list_rule_delete(list_rule*, rule*);
void list_rule_delete_sub(list_rule*, list_rule_cell*);
void list_rules_concat(list_rule*, list_rule*);
void list_rule_clear(list_rule*);
void list_rule_print(list_rule*);
void list_rule_print2(list_rule*);
list_rule* list_rule_copy(list_rule*);

list_rule* read_rule_list(char*);


rule* list_rule_head(list_rule* L) {
  list_rule_cell* h = L->head;
  rule* r = (rule*)malloc(sizeof(rule));
  r->num = h->key->num;
  const unsigned l = strlen(h->key->cond);
  r->cond = (char*)malloc((l+1)*sizeof(char));
  strcpy(r->cond, h->key->cond);
  return r;
}

bool list_rule_eq(list_rule* L1, list_rule* L2) {
  if (L1->size != L2->size) { return false; }
  list_rule_cell *p1, *p2;
  for (p1 = L1->head, p2 = L2->head; NULL != p1; p1 = p1->next, p2 = p2->next)
    if (!rule_eq(p1->key, p2->key)) { return false; }
  return true;
}

bool list_rule_is_empty(list_rule* L) { return (0 == L->size); }

list_rule_cell* list_rule_search(list_rule* L, rule* r) {
  list_rule_cell* x = L->head;
  while (NULL != x && x->key->num == r->num && strcmp(x->key->cond, r->cond)) { x = x->next; }
  return x;
}

void list_rule_insert(list_rule* L, rule* r) {
  list_rule_cell* new = (list_rule_cell*)malloc(sizeof(list_rule_cell));
  L->size = L->size + 1;
  const unsigned l = strlen(r->cond);
  new->key = (rule*)malloc(sizeof(rule));
  new->key->cond = (char*)malloc((l+1)*sizeof(char));
  strcpy(new->key->cond, r->cond);
  new->key->num = r->num;
  list_rule_insert_sub(L, new);
}

void list_rule_insert_sub(list_rule* L, list_rule_cell* x) {
  x->next = L->head;
  if (NULL != L->head) { L->head->prev = x; }
  else { L->last = x; }
  L->head = x;
  x->prev = NULL;
}

void list_rule_delete(list_rule* L, rule* r) {
  list_rule_cell* x = list_rule_search(L, r);
  if (NULL != x) { list_rule_delete_sub(L, x); }
}

void list_rule_delete_sub(list_rule* L, list_rule_cell* x) {
  L->size = L->size - 1;
  if (NULL != x->prev) { x->prev->next = x->next; }
  else { L->head = x->next; }
  if (NULL != x->next) { x->next->prev = x->prev; }
  else { L->last = x->prev; }
  free(x->key->cond);
  free(x->key);
  free(x);
}

void list_rules_concat(list_rule* L1, list_rule* L2) {
  if (NULL == L1 || NULL == L1->head) { L1 = L2; return ; }
  L1->last->next = L2->head;
  if (NULL != L2 && NULL != L2->head) { L2->head->prev = L1->last; }
  L1->size = L1->size + L2->size;
}

void list_rule_clear(list_rule* L) {
  if (NULL == L) { return ; }
  list_rule_cell* p, *q;
  for (p = L->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q->key->cond);
    free(q->key);
    free(q);
  }
  if (NULL != L->sigma) { free(L->sigma); }
  L->size = 0;
  L = NULL;
}

void list_rule_print(list_rule* L) {
  list_rule_cell* p;
  const unsigned d = floor(log10(L->size)) + 1;
  for (p = L->head; NULL != p; p = p->next) 
    printf("r[%*d] : %s", d, p->key->num, p->key->cond);
}

void list_rule_print2(list_rule* L) {
  list_rule_cell* p = L->head;
  const unsigned d = floor(log10(L->size)) + 1;
  const unsigned n = strlen(p->key->cond)-1;
  char* rule = (char*)malloc(n*sizeof(char));
  for ( ; NULL != p; p = p->next) { 
    printf("r[%*d] : ", d, p->key->num);
    unsigned i;
    for (i = 0; i < n; ++i) { rule[L->sigma[i]] = p->key->cond[i]; }
    rule[n] = '\0';
    printf("%s\n", rule);
  }
  free(rule);
}

list_rule* list_rule_copy(list_rule* L) {
  list_rule* L2 = (list_rule*)malloc(sizeof(list_rule));
  list_rule_cell* p;
  for (p = L->last; NULL != p; p = p->prev)
    list_rule_insert(L2, p->key);
  return L2;
}

list_rule* read_rule_list(char* rule_file_name) {
  FILE *fp;
  if (NULL == (fp = fopen(rule_file_name, "r"))) {
    fprintf(stderr, "ERROR: can't open the rule file.\n");
    exit(1);
  }

  list_rule* rulelist = (list_rule*)malloc(sizeof(list_rule));

  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  unsigned i;
  
  for (i = 1; -1 != (read = getline(&line, &len, fp)); ++i) {
    rule* r = (rule*)malloc(sizeof(rule));
    r->num = i;
    r->cond = (char*)malloc(strlen(line)*sizeof(char));
    strcpy(r->cond, line);
    r->cond[strlen(line)] = '\0';
    list_rule_insert(rulelist, r);
  }
  fclose(fp);
  
  return rulelist;
}


/************************* list of Rule List **************************/
struct LIST_RULELIST_CELL {
  list_rule* key;
  struct LIST_RULELIST_CELL* prev;
  struct LIST_RULELIST_CELL* next;
};
typedef struct LIST_RULELIST_CELL list_rulelist_cell;

struct LIST_RULELIST {
  list_rulelist_cell* head;
  list_rulelist_cell* last;
  unsigned size;
};
typedef struct LIST_RULELIST list_rulelist;

list_rule* list_rulelist_head(list_rulelist*);
bool list_rulelist_is_empty(list_rulelist*);
list_rulelist_cell* list_rulelist_search(list_rulelist*, list_rule*);
void list_rulelist_insert(list_rulelist*, list_rule*);
void list_rulelist_insert_sub(list_rulelist*, list_rulelist_cell*);
void list_rulelist_delete(list_rulelist*, list_rule*);
void list_rulelist_delete_sub(list_rulelist*, list_rulelist_cell*);
void list_rulelists_concat(list_rulelist*, list_rulelist*);
void list_rulelist_clear(list_rulelist*);
void list_rulelist_print(list_rulelist*);


list_rule* list_rulelist_head(list_rulelist* LL) {
  list_rulelist_cell* h = LL->head;
  list_rule* L = list_rule_copy(h->key);
  return L;
}

bool list_rulelist_is_empty(list_rulelist* L) { return (0 == L->size); }

list_rulelist_cell* list_rulelist_search(list_rulelist* LL, list_rule* L) {
  list_rulelist_cell* x = LL->head;
  while (NULL != x && !list_rule_eq(L, x->key)) { x = x->next; }
  return x;
}

void list_rulelist_insert(list_rulelist* LL, list_rule* L) {
  LL->size = LL->size + 1;
  list_rulelist_cell* new = (list_rulelist_cell*)malloc(sizeof(list_rulelist_cell));
  new->key = list_rule_copy(L);
  list_rulelist_insert_sub(LL, new);
}

void list_rulelist_insert_sub(list_rulelist* L, list_rulelist_cell* x) {
  x->next = L->head;
  if (NULL != L->head) { L->head->prev = x; }
  else { L->last = x; }
  L->head = x;
  x->prev = NULL;
}

void list_rulelist_delete(list_rulelist* LL, list_rule* L) {
  list_rulelist_cell* x = list_rulelist_search(LL, L);
  if (NULL != x) { list_rulelist_delete_sub(LL, x); }
}

void list_rulelist_delete_sub(list_rulelist* L, list_rulelist_cell* x) {
  L->size = L->size - 1;
  if (NULL != x->prev) { x->prev->next = x->next; }
  else { L->head = x->next; }
  if (NULL != x->next) { x->next->prev = x->prev; }
  else { L->last = x->prev; }
  list_rule_clear(x->key);
  free(x);
}

void list_rulelists_concat(list_rulelist* L1, list_rulelist* L2) {
  if (NULL == L1 || NULL == L1->head) { L1 = L2; return ; }
  L1->last->next = L2->head;
  if (NULL != L2 && NULL != L2->head) { L2->head->prev = L1->last; }
  L1->size = L1->size + L2->size;
}

void list_rulelist_clear(list_rulelist* L) {
  if (NULL == L) { return ; }
  list_rulelist_cell* p, *q;
  for (p = L->head; NULL != p; ) {
    q = p;
    p = p->next;
    list_rule_clear(q->key);
    free(q);
  }
}

void list_rulelist_print(list_rulelist* L) {
  list_rulelist_cell* p = L->head;
  list_rule_print(p->key); putchar('\n');
  for (p = p->next; NULL != p; p = p->next) {
    list_rule_print(p->key); putchar('\n');
  }
}

/*************************** Class Bench Rule ***************************/
#define HEADER_LENGTH 104

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
