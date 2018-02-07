/* c1p.h */
/* This is a test program for the lbfs.c */

#ifndef __TANAKALAB_C1P_H__
#define __TANAKALAB_C1P_H__

#ifndef __TANAKALAB_LIST_H__
#include "list.h"
#endif

#ifndef __TANAKALAB_SET_H__
#include "set.h"
#endif

#ifndef __TANAKALAB_GRAPH_H__
#include "graph.h"
#endif

#ifndef __TANAKALAB_LBFS_H__
#include "lbfs.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <math.h>

struct LIST_SET_UNSIGNED_CELL {
  set_unsigned* key;
  struct LIST_SET_UNSIGNED_CELL* prev;
  struct LIST_SET_UNSIGNED_CELL* next;
};
typedef struct LIST_SET_UNSIGNED_CELL list_set_unsigned_cell;

struct LIST_SET_UNSIGNED {
  list_set_unsigned_cell* head;
  list_set_unsigned_cell* last;
  unsigned size;
};
typedef struct LIST_SET_UNSIGNED list_set_unsigned;

struct CC_PARTITION_CELL {
  list_set_unsigned* list;
  bool state;
  struct CC_PARTITION_CELL* prev;
  struct CC_PARTITION_CELL* next;
};
typedef struct CC_PARTITION_CELL cc_partition_cell;

struct CC_PARTITION { /* partition for clique chain */
  cc_partition_cell* head;
  cc_partition_cell* last;
  unsigned size;
};
typedef struct CC_PARTITION cc_partition;

void show_cc_partition(cc_partition* P) {
  cc_partition_cell* p;
  for (p = P->head; NULL != p; p = p->next) {
    list_set_unsigned_cell* s = p->list->head;
    bool flag = false;
    if (NULL != s) { printf("["); putchar('{'); set_unsigned_print(s->key); putchar('}'); flag = true; }
    for (s = s->next ; NULL != s; s = s->next) { printf(", "); putchar('{'); set_unsigned_print(s->key); putchar('}'); }
    if (flag) printf("]");
  }
}

void list_set_unsigned_show(list_set_unsigned* L) {
  if (NULL == L) { return; }
  list_set_unsigned_cell* x = L->head;
  if (NULL != x) { printf("["); putchar('{'); set_unsigned_print(x->key); putchar('}');
    for (x = x->next; NULL != x; x = x->next) { printf(", "); putchar('{'); set_unsigned_print(x->key); putchar('}'); }
    printf("]");
  }
}

list_set_unsigned_cell* list_set_unsigned_search(list_set_unsigned* L, set_unsigned* s) {
  list_set_unsigned_cell* x = L->head;
  /* printf("s: "); set_show_unsigned(s); putchar('\n'); */
  /* printf("x: "); set_show_unsigned(x->key); putchar('\n'); */
  while (NULL != x && !set_unsigned_equal(x->key, s)) { x = x->next; }
  return x;
}

void list_set_unsigned_insert_b_sub(list_set_unsigned* L, list_set_unsigned_cell* x) {
  x->prev = L->last;
  if (NULL != L->last) { L->last->next = x; }
  else { L->head = x; }
  L->last = x;
  x->next = NULL;
}

void list_set_unsigned_insert_b(list_set_unsigned* L, set_unsigned* s) {
  list_set_unsigned_cell* new = (list_set_unsigned_cell*)calloc(1, sizeof(list_set_unsigned_cell));
  L->size = L->size + 1;
  new->key = s;
  list_set_unsigned_insert_b_sub(L, new);
}

void list_set_unsigned_insert_sub(list_set_unsigned* L, list_set_unsigned_cell* x) {
  x->next = L->head;
  if (NULL != L->head) { L->head->prev = x; }
  else { L->last = x; }
  L->head = x;
  x->prev = NULL;
}

void list_set_unsigned_insert(list_set_unsigned* L, set_unsigned* s) {
  list_set_unsigned_cell* new = (list_set_unsigned_cell*)calloc(1, sizeof(list_set_unsigned_cell));
  L->size = L->size + 1;
  new->key = s;
  list_set_unsigned_insert_sub(L, new);
}

void list_set_unsigned_delete_sub(list_set_unsigned* L, list_set_unsigned_cell* x) {
  L->size = L->size-1;
  if (NULL != x->prev) { x->prev->next = x->next; }
  else { L->head = x->next; }
  if (NULL != x->next) { x->next->prev = x->prev; }
  else { L->last = x->prev; }
  /* set_unsigned_clear(x->key); */
}

void list_set_unsigned_delete(list_set_unsigned* L, set_unsigned* s) {
  list_set_unsigned_cell* x = list_set_unsigned_search(L, s);
  if (NULL != x) { list_set_unsigned_delete_sub(L, x); }
}

void cc_partition_insert_sub(cc_partition* P, cc_partition_cell* x) {
  ++(P->size);
  x->next = P->head;
  if (NULL != P->head) { P->head->prev = x; }
  P->head = x;
  x->prev = NULL;
}

void cc_partition_insert(cc_partition* P, list_set_unsigned* L) {
  cc_partition_cell* new = (cc_partition_cell*)calloc(1, sizeof(cc_partition_cell));
  new->list = L;
  cc_partition_insert_sub(P, new);
}

list_set_unsigned* cc_intersect(unsigned v, list_set_unsigned* L) {
  list_set_unsigned* I = (list_set_unsigned*)calloc(1, sizeof(list_set_unsigned));
  list_set_unsigned_cell* p;
  for (p = L->head; NULL != p; p = p->next) {
    set_unsigned* s = p->key;
    if (set_unsigned_member(s, v)) { list_set_unsigned_insert_b(I,s); }
  }
  return I;
}

bool lt_clique(unsigned* sigma, const unsigned n, set_unsigned* S, set_unsigned* T) {
  if (set_unsigned_is_empty(S) && !set_unsigned_is_empty(T)) { return true; }
  if (!set_unsigned_is_empty(S) && set_unsigned_is_empty(T)) { return false; }
  set_unsigned* R = (set_unsigned*)calloc(1, sizeof(set_unsigned));
  set_unsigned_copy(R, S);
  set_unsigned* U = (set_unsigned*)calloc(1, sizeof(set_unsigned));
  set_unsigned_copy(U, T);
  bool flag = false;
  /* printf("S : "); set_show_unsigned(R); putchar('\n'); */
  /* printf("T : "); set_show_unsigned(U); putchar('\n'); */

  unsigned i;
  for (i = 0; i < n; ++i) {
    bool s = false, t = false;
    if (set_unsigned_member(R, sigma[i])) { s = true; }
    if (set_unsigned_member(U, sigma[i])) { t = true; }
    if (s && !t) { flag = true; break; }
    if (!s && t) { break; }
    set_unsigned_delete(R, sigma[i]);
    set_unsigned_delete(U, sigma[i]);
    if (set_unsigned_is_empty(R) && !set_unsigned_is_empty(U)) { flag = true; break; }
    if (!set_unsigned_is_empty(R) && set_unsigned_is_empty(U)) { break; }
  }

  set_unsigned_clear(U);  
  set_unsigned_clear(R);
  return flag;
}

void quick_sort_for_clique(unsigned* sigma, const unsigned n, set_unsigned** clique, unsigned* cc, int l, int r) {
  /* printf("n = %d, l = %d, r = %d\n", n, l, r); */
  /* unsigned k; for (k = l; k <= r; ++k) { printf("%d : ", k); set_show_unsigned(clique[k]); putchar('\n'); } */
  /* unsigned k; for (k = l; k <= r; ++k) { printf("%d", cc[k]); } putchar('\n');  */
  if (l < r) {
    int i = l, j = r, p = (int)floor((l+r)/2), t;
    set_unsigned *tmp, *P = clique[p];
    while (true) {
      while (i < r && lt_clique(sigma, n, clique[i], P)) ++i;
      while (l < j && lt_clique(sigma, n, P, clique[j])) --j;
      if (i >= j) break;
      tmp = clique[i]; clique[i] = clique[j]; clique[j] = tmp;
      t = cc[i]; cc[i] = cc[j]; cc[j] = t;
      ++i; --j;
    }
    quick_sort_for_clique(sigma, n, clique, cc, l, i-1);
    quick_sort_for_clique(sigma, n, clique, cc, j+1, r);
  }
}

unsigned* get_clique_chain(unsigned* sigma, const unsigned m, set_unsigned** clique, const unsigned n) {
  /* sort a clique (a set of unsigned) by a sigma */
  unsigned i;
  /* set_unsigned *S, *T; */
  
  /* set clique chain (c1p order) */
  unsigned* cc = (unsigned*)calloc(n, sizeof(unsigned));
  /* for (i = 0; i < n; ++i) { set_show_unsigned(clique[i]); putchar('\n'); } */
  for (i = 0; i < n; ++i) { cc[i] = i; }

  quick_sort_for_clique(sigma, m, clique, cc, 0, n-1);
  
  return cc;
}

unsigned* get_clique_chain2(unsigned* sigma, const unsigned m, set_unsigned** clique, const unsigned n) {
  unsigned i;
  /* for (i = 0; i < n; ++i) { set_show_unsigned(clique[i]); putchar('\n'); } */
  
  /* initialize clique partition */
  cc_partition* P = (cc_partition*)calloc(1, sizeof(cc_partition));
  list_set_unsigned* init = (list_set_unsigned*)calloc(1, sizeof(list_set_unsigned));
  for (i = 0; i < n; ++i) { list_set_unsigned_insert(init, clique[i]); }
  cc_partition_insert(P, init);
  show_cc_partition(P); putchar('\n');
  
  /* refine the partition by I-ordering (sigma) */
  list_set_unsigned *L, *I;
  cc_partition_cell *p, *new, *pp, *nn;
  list_set_unsigned_cell* q;
  for (i = 0; i < m; ++i) {
    for (p = P->head; NULL != p; p = p->next) {
      L = p->list;
      /* printf("L : "); list_set_unsigned_show(L); putchar('\n'); */
      I = cc_intersect(sigma[i], L);
      /* printf("sigma[%d] = %d, I : ", i, sigma[i]); list_set_unsigned_show(I); putchar('\n'); */
      if (0 != I->size && I->size < L->size) {
	for (q = I->head; NULL != q; q = q->next) { list_set_unsigned_delete(L, q->key); }
	/* printf("D : "); show_cc_partition(P); putchar('\n'); */
	++(P->size);
	new = (cc_partition_cell*)calloc(1, sizeof(cc_partition_cell));
	new->list = I;
	if (true) { /* insert new previous of p */
	  pp = p->prev;
	  if (P->head == p) { P->head = new; }
	  if (NULL != pp) { pp->next = new; }
	  new->prev = pp;
	  new->next = p;
	  p->prev = new;
	}
	else { /* insert new next of p */
	  nn = p->next;
	  if (NULL != nn) { nn->prev = new; }
	  new->next = nn;
	  new->prev = p;
	  p->next = new;
	}
      }
    }
    show_cc_partition(P); putchar('\n');
  }
  show_cc_partition(P); putchar('\n');

  /* set clique chain (c1p order) from partition */
  unsigned* cc = (unsigned*)calloc(n, sizeof(unsigned));
  unsigned j;
  set_unsigned *S, *T;
  for (i = 0; i < n; ++i) {
    S = clique[i];
    for (p = P->head, j = 0; NULL != p; p = p->next, ++j) {
      T = p->list->head->key;
      if (set_unsigned_equal(S,T)) { cc[i] = j; break; }
    }
  }
  /* printf("cc : %*d", 2, cc[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", 2, cc[i]); } putchar('\n'); */
  return cc;
}

unsigned* remove_unit_portion(unsigned* sigma, const unsigned m, const unsigned n) {
  unsigned* tau = (unsigned*)calloc(m, sizeof(unsigned));
  unsigned i, j;
  for (i = 0, j = 0; i < n; ++i) { if (sigma[i] < m) { tau[j] = sigma[i]; ++j; } }
  return tau;
}

// if M is non-C1P then return NULL
unsigned* get_c1p_order(matrix* M) {
  matrix* N = add_UnitMatrix(M);
  /* print_matrix(N); */
  /* printf("N->m = %d, N->n = %d\n", N->m, N->n); */
  graph* G = matrix_to_graph(N);
  /* printf("G->size = %d\n", G->size); */
  /* print_graph(*G); */
  
  unsigned i;
  /* for (i = 0; i < M->m; ++i) { for (j = 0; j < M->n; ++j) { putchar(M->b[i][j]); } putchar('\n'); } */

  unsigned* sigma = Four_Sweep_LBFS2(*G);

  unsigned* c1p_order = NULL;
  bool flag = is_I_ordering(sigma, *G);

  if (flag) {
    unsigned* tau = remove_unit_portion(sigma, M->m, N->m);
    /* printf("sigma : %*d", 2, sigma[0]); */
    /* for (i = 1; i < N->m; ++i) { printf(", %*d", 2, sigma[i]); } putchar('\n'); */
    /* printf("tau   : %*d", 2, tau[0]); */
    /* for (i = 1; i < M->m; ++i) { printf(", %*d", 2, tau[i]); } putchar('\n'); */

    set_unsigned** clique = c1p_matrix_to_clique(M);
    /* for (i = 0; i < M->n; ++i) { printf("clique[%d] : ", i); set_show_unsigned(clique[i]); putchar('\n'); } */
    /* printf("M->m = %d, M->n = %d\n", M->m, M->n); */

    /* for (i = 0; i < (M->n)-1; ++i) { */
    /*   for (j = i+1; j < M->n; ++j) { */
    /* 	if (lt_clique(tau, M->n, clique[i], clique[j])) { printf("cc[%d] < cc[%d]\n", i, j); } */
    /* 	else { printf("cc[%d] > cc[%d]\n", i, j); } */
    /*   } */
    /* } */
    
    unsigned* cc = get_clique_chain(tau, M->m, clique, M->n);
    /* printf("clique chain : %d", cc[0]); for (i = 1; i < M->n; ++i) { printf(", %d", cc[i]); } putchar('\n'); */

    c1p_order = (unsigned*)calloc(M->n, sizeof(unsigned));
    for (i = 0; i < M->n; ++i) { c1p_order[cc[i]] = i; }
	
    free(tau);
    for (i = 0; i < M->n; ++i) { set_unsigned_clear(clique[i]); }

    free(cc);
    free(clique);
  }

  free(sigma);
  for (i = 0; i < N->m; ++i) { free(N->b[i]); }
  free(N->b);
  free(N);
  for (i = 0; i < G->size; ++i) { free(G->al[i]); }
  free(G->al);
  free(G);
  
  if (!flag) { return NULL; }
      
  return c1p_order;
}

bool is_c1p(matrix* M) {
  matrix* N = add_UnitMatrix(M);
  graph* G = matrix_to_graph(N);

  /* for (i = 0; i < M->m; ++i) { for (j = 0; j < M->n; ++j) { putchar(M->b[i][j]); } putchar('\n'); } */
  /* for (i = 0; i < N->m; ++i) { for (j = 0; j < N->n; ++j) { putchar(N->b[i][j]); } putchar('\n'); } */
  /* for (i = 0; i < G->size; ++i) { printf("%d : ", i); show_list_unsigned(G->al[i]); putchar('\n'); } */
  
  Ordering o = Four_Sweep_LBFS(*G);

  unsigned i;
  for (i = 0; i < N->m; ++i) { free(N->b[i]); }
  free(N->b);
  free(N);
  for (i = 0; i < G->size; ++i) { free(G->al[i]); }
  free(G->al);
  free(G);
  if (UI == o || I == o) { return true; }
  return false;
}

#endif
  // matrix* M = read_matrix(argv[1]);
  /* for (i = 0; i < M->m; ++i) { free(M->b[i]); } */
  /* free(M->b); */
  /* free(M); */
