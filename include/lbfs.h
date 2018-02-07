/* lbfs.c */
/* (Lexicographicl breadth first search via partition refinement ) 
 * Warning!! This program doesn't run in linear time.
 *
 * I should implement a liner time LBFS via following algorithms.
 *   
 *  * (LBFS)  https://www.irif.fr/~habib/Documents/Cours_implementation.pdf
 *  * (LBFS)  http://www.sciencedirect.com/science/article/pii/S0304397599001504
 *  * (LBFS★) https://www.dmtcs.org/dmtcs-ojs/index.php/dmtcs/article/viewArticle/2066.html
 *
 */

#ifndef __TANAKALAB_LBFS_H__
#define __TANAKALAB_LBFS_H__

#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __TANAKALAB_LIST_H__
#include "list.h"
#endif

#ifndef __TANAKALAB_SET_H__
#include "set.h"
#endif

#ifndef __TANAKALAB_GRAPH_H__
#include "graph.h"
#endif

/* 
   Not Interval graph ordering, 
   Unit Interval graph ordering, 
   Interval graph Ordering
*/
typedef enum { NI, UI, I } Ordering; 

struct PARTITION_CELL {
  list_unsigned* List;
  struct PARTITION_CELL* prev;
  struct PARTITION_CELL* next;
};
typedef struct PARTITION_CELL partition_cell;

struct PARTITION {
  partition_cell* head;
  partition_cell* last;
  unsigned size;
};
typedef struct PARTITION partition;

bool partition_is_empty(partition* P) {
  return (P->size == 0);
}

void partition_delete_sub(partition* P, partition_cell* x) {
  --(P->size);
  if (NULL != x->prev) { x->prev->next = x->next; }
  else { P->head = x->next; }
  if (NULL != x->next) { x->next->prev = x->prev; }
  /* else { P->last = x->prev; } */
}

void partition_insert_sub(partition* P, partition_cell* x) {
  ++(P->size);
  x->next = P->head;
  if (NULL != P->head) { P->head->prev = x; }
  P->head = x;
  x->prev = NULL;
}

void partition_insert(partition* P, list_unsigned* L) {
  partition_cell* new = (partition_cell*)calloc(1, sizeof(partition_cell));
  new->List = L;
  partition_insert_sub(P, new);
}

void partition_remove_head(partition* P) {
  list_unsigned_clear(P->head->List);
  partition_delete_sub(P, P->head);
}

list_unsigned* intersect(unsigned v, graph G, list_unsigned* L) {
  list_unsigned* I = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  list_unsigned_cell* p;
  unsigned u;
  /* for (p = (G.al)[v]->head; NULL != p; p = p->next) { */
  /*   u = p->key; */
  /*   if (list_unsigned_member(L, u)) { list_unsigned_insert_b(I, u); } */
  /* } */
  for (p = L->head; NULL != p; p = p->next) {
    u = p->key;
    if (list_unsigned_member((G.al)[v], u)) { list_unsigned_insert_b(I, u); }
  }
  return I;
}

void print_order(unsigned* sigma, const unsigned n) {
  unsigned i;
  const unsigned d = floor(log10(n)) + 1;
  printf("%*d", d, sigma[0]);
  for (i = 1; i < n; ++i) { printf(", %*d", d, sigma[i]); }
}

void show_partition(partition* P) {
  partition_cell* p;
  for (p = P->head; NULL != p; p = p->next) {
    list_unsigned_cell* l = p->List->head;
    bool flag = false;
    if (NULL != l) { printf("[%2d", l->key); l = l->next; flag = true; }
    for ( ; NULL != l; l = l->next) { printf(", %2d", l->key); }
    if (flag) printf("]");
  }
  putchar('\n');
}

unsigned* lbfs(graph G) {
  /**************************************** preparation process ****************************************/
  unsigned n = G.size;
  unsigned i, v;
  unsigned* sigma = (unsigned*)calloc(n, sizeof(unsigned));
  partition* P = (partition*)calloc(1, sizeof(partition));
  list_unsigned *L, *I;
  list_unsigned* init = (list_unsigned*)calloc(1, sizeof(list_unsigned));

  /******************************************** main process ********************************************/
  /* init Partition */
  /* for (i = 0; i < n; ++i) { list_unsigned_insert(init, n-i-1); } */
  for (i = 0; i < n; ++i) { list_unsigned_insert(init, i); }
  partition_insert(P,init);

  i = 0;
  partition_cell *p, *new, *pp;
  list_unsigned_cell* q;
  while (!partition_is_empty(P)) {
    /****** select node v ******/
    L = P->head->List;
    v = L->head->key;
    /* show_partition(P); */
    /* printf("P.size = %d, vertex = %d, index = %d\n", P->size, v, i); */
    sigma[i] = v;
    ++i;
    list_unsigned_remove_head(L);
    /* if L becomes empty list, then remove L from P */
    if (list_unsigned_is_empty(L)) { partition_remove_head(P); }
    if (partition_is_empty(P)) { break; }
    
    /****** refine Partion with N(v) ******/
    /* printf("========== refine with N(%d) ==========\n", v); */
    for (p = P->head; NULL != p; p = p->next) {
      L = p->List;
      I = intersect(v, G, L);
      if (0 != I->size && I->size < L->size) { /* Does I intersect strictly L? */
	/* remove elements from L i.e. L \ I */
	for (q = I->head; NULL != q; q = q->next) { list_unsigned_delete(L, q->key); }
	/* insert I to left of L */
	++(P->size);
	new = (partition_cell*)calloc(1, sizeof(partition_cell));
	new->List = I;
	pp = p->prev;
	if (P->head == p) { P->head = new; }
	if (NULL != pp) { pp->next = new; }
	new->prev = pp;
	new->next = p;
	p->prev = new;
      }
    }
    /* show_partition(P); */
  }
  /****************************** clean up the memory allocated by calloc *******************************/
  free(init);
  free(P);
    
  return sigma;
}

/* void sort(list_unsigned* al, unsigned* tau) { */
/*   unsigned i, n = al->size; */
/*   unsigned* array = calloc(n, sizeof(array)); */
/*   list_unsigned_cell* p; */
/*   for (p = al->head, i = 0; i < n; p = p->next, ++i) { array[i] = p->key; } */
/*   /\* for (i = 0; i < n; ++i) { printf("%d, ", array[i]); } *\/ */
/*   /\* putchar('\n'); *\/ */
  
/*   free(array); */
/* } */

unsigned* lbfs_plus(graph G, unsigned* tau) {
  /**************************************** preparation process ****************************************/
  unsigned n = G.size;
  unsigned i, v;
  unsigned* sigma = (unsigned*)calloc(n, sizeof(unsigned));
  partition* P = (partition*)calloc(1, sizeof(partition));
  list_unsigned *L, *I;
  list_unsigned* init = (list_unsigned*)calloc(1, sizeof(list_unsigned));

  /******************************************** main process ********************************************/
  /* init Partition */
  for (i = 0; i < n; ++i) { list_unsigned_insert(init, tau[i]); }
  partition_insert(P,init);
  /* print_graph(G); */
  
  /* sort adjacency list of G by tau */
  /* for (i = 0; i < n; ++i) { sort(G.al[i], tau); } */

  i = 0;
  partition_cell* p, *new, *pp;
  list_unsigned_cell* q;
  /* show_partition(P); */
  while (!partition_is_empty(P)) {
    /****** select node v ******/
    L = P->head->List;
    v = L->head->key;
    /* printf("P.size = %d, vertex = %d, index = %d\n", P->size, v, i); */
    /* show_partition(P); */
    sigma[i] = v;
    ++i;
    list_unsigned_remove_head(L);
    /* if L becomes empty list, then remove L from P */
    if (list_unsigned_is_empty(L)) { partition_remove_head(P); }
    if (partition_is_empty(P)) { break; }

    /****** refine Partion with N(v) ******/
    /* printf("========== refine with N(%d) ==========\n", v); */
    for (p = P->head; NULL != p; p = p->next) {
      L = p->List;
      I = intersect(v, G, L);
      // show_list_unsigned(I);
      if (0 != I->size && I->size < L->size) { /* Does I intersect strictly L? */
	/* remove elements from L i.e. L \ I */
	for (q = I->head; NULL != q; q = q->next) { list_unsigned_delete(L, q->key); }
	/* insert I to left of L */
	++(P->size);
	new = (partition_cell*)calloc(1, sizeof(partition_cell));
	new->List = I;
	pp = p->prev;
	if (P->head == p) { P->head = new; }
	if (NULL != pp) { pp->next = new; }
	new->prev = pp;
	new->next = p;
	p->prev = new;
      }
    }
    /* show_partition(P); */
  }
  /****************************** clean up the memory allocated by calloc *******************************/
  free(init);
  free(P);
    
  return sigma;
}

char* add_label_i(char* s, unsigned i, const unsigned n) { /* n-1 ; because starts from 0 not 1 */
  unsigned di;
  if (0 != i) { di = floor(log10((double)i)) + 1; }
  else { di = 1; }
  const unsigned dn = floor(log10((double)n)) + 1;

  unsigned len;
  if (NULL != s) { len = strlen(s); }
  else { len = 0; }
  const unsigned l = len + dn;
  char* new = (char*)calloc(l+1, sizeof(char));
  if (NULL != s) { strcpy(new, s); }

  unsigned j, c;
  for (j = 0; j < di; ++j) { 
    c = i % 10;
    new[l-1-j] = '0' + c;
    i /= 10;
  }
  for (j = 0; j < dn-di; ++j) { new[len+j] = '0'; }
  new[l] = '\0';

  if (NULL != s) { free(s); }
  return new;
}

void get_w_sub(bool* flag, unsigned* w, unsigned* f_sigma, unsigned_node* p, unsigned_node* nil) {
  if (nil == p || NULL == p) { return ; }
  get_w_sub(flag, w, f_sigma, p->left, nil);
  get_w_sub(flag, w, f_sigma, p->right, nil);
  unsigned fx = f_sigma[p->key];
  if (!(*flag)) { *flag = true; *w = p->key; }
  else if (f_sigma[*w] < fx) { *w = p->key; }
  /* printf("f_sigma[%d] = %d, f_sigma[%d] = %d\n", *w, f_sigma[*w], p->key, f_sigma[p->key]); */
}

unsigned get_w(unsigned* f_sigma, set_unsigned* S) {
  unsigned w;
  bool flag = false;
  get_w_sub(&flag, &w, f_sigma, S->root, S->nil);
  return w;
}

void get_beta_sub(bool* flag, unsigned* beta, unsigned* sigma_i, unsigned_node* p, unsigned_node* nil) {
  if (nil == p || NULL == p) { return; }
  get_beta_sub(flag, beta, sigma_i, p->left, nil);
  get_beta_sub(flag, beta, sigma_i, p->right, nil);
  unsigned j = sigma_i[p->key];
  if (!(*flag)) { *flag = true; *beta = j; }
  else if (*beta < j) { *beta = j; }
}

/* β = max{ j | σ(J) ∈ S } */
unsigned get_beta(unsigned* sigma_i, set_unsigned* S) {
  if (NULL == S) { fprintf(stderr, "ERROR: in get_alpha, S is null!!\n"); exit(1); }
  unsigned beta;
  bool flag = false;
  get_beta_sub(&flag, &beta, sigma_i, S->root, S->nil);
  return beta;
}

void get_alpha_sub(bool* flag, unsigned* alpha, unsigned* sigma_i, unsigned_node* p, unsigned_node* nil) {
  if (nil == p || NULL == p) { return; }
  get_alpha_sub(flag, alpha, sigma_i, p->left, nil);
  get_alpha_sub(flag, alpha, sigma_i, p->right, nil);
  unsigned j = sigma_i[p->key];
  if (!(*flag)) { *flag = true; *alpha = j; }
  else if (j < *alpha) { *alpha = j; }
}

/* α = min{ j | σ(J) ∈ S } */
unsigned get_alpha(unsigned* sigma_i, set_unsigned* S) {
  if (NULL == S) { fprintf(stderr, "ERROR: in get_alpha, S is null!!\n"); exit(1); }
  unsigned alpha;
  bool flag = false;
  get_alpha_sub(&flag, &alpha, sigma_i, S->root, S->nil);
  return alpha;
}

/* f_σ(u) = max{ j | σ(j) ∈ N_G[u] }*/
unsigned get_f_sigma(graph G, unsigned* sigma_i, unsigned u) {
  list_unsigned_cell* p;
  unsigned j = sigma_i[u];
  for (p = G.al[u]->head; NULL != p; p = p->next) if (j < sigma_i[p->key]) { j = sigma_i[p->key]; }
  return j;
}

/* L_G,σ^k(w) = N_G[w] ∩ σ[k] */
set_unsigned* getL(graph G, unsigned* sigma, const unsigned k, const unsigned w) {
  /* if (0 == k) { */
  /*   set_unsigned* Empty = (set_unsigned*)calloc(1, sizeof(set_unsigned)); */
  /*   set_unsigned_init(Empty); */
  /*   return Empty; */
  /* } */

  /* const unsigned n = G.size; */
  set_unsigned* N = (set_unsigned*)calloc(1, sizeof(set_unsigned));
  set_unsigned_init(N);
  set_unsigned_insert(N, w);
  list_unsigned_cell* p;
  for (p = G.al[w]->head; NULL != p; p = p->next) { set_unsigned_insert(N, p->key); }
  
  set_unsigned* S = (set_unsigned*)calloc(1, sizeof(set_unsigned));
  set_unsigned_init(S);
  unsigned i;
  for (i = 0; i <= k; ++i) { set_unsigned_insert(S, sigma[i]); }

  set_unsigned* L = set_unsigned_intersect(N, S);

  set_unsigned_clear(S);
  set_unsigned_clear(N);
  return L;
}

set_unsigned* getS(const unsigned n, char** label, bool* unvisited) {
  set_unsigned* S = (set_unsigned*)calloc(1, sizeof(set_unsigned));
  set_unsigned_init(S);
  char* s = NULL;
  unsigned i;
  for (i = 0; i < n; ++i) {
    if (NULL != label[i] && unvisited[i]) {
      if (NULL == s) { s = label[i]; }
      else if (strcmp(s, label[i]) < 0) { s = label[i]; }
    }
  }
  if (NULL != s) { for (i = 0; i < n; ++i) { if (unvisited[i] && NULL != label[i] && 0 == strcmp(s, label[i])) { set_unsigned_insert(S, i); } } }
  else { for (i = 0; i < n; ++i) if (unvisited[i]) { set_unsigned_insert(S, i); } }
  return S;
}

set_unsigned* get_rho_i(const unsigned i, const unsigned* rho) {
  set_unsigned* rho_i = (set_unsigned*)calloc(1, sizeof(set_unsigned));
  set_unsigned_init(rho_i);
  unsigned k;
  for (k = 0; k <= i; ++k) { set_unsigned_insert(rho_i, rho[k]); }
  return rho_i;
}

unsigned* lbfs_star(graph G, unsigned* sigma) {
  /**************************************** preparation process ****************************************/
  const unsigned n = G.size;
  unsigned* sigma_i = get_inverse(sigma, n);
  /* unsigned l; */

  unsigned* rho = (unsigned*)calloc(n, sizeof(unsigned));
  char** label = (char**)calloc(n, sizeof(char*));
  unsigned i; for (i = 0; i < n; ++i) { label[i] = NULL; }
  bool* unvisited = (bool*)calloc(n, sizeof(bool)); /* if true == unvisited[u] then the vertex u is unvisited */
  for (i = 0; i < n; ++i) { unvisited[i] = true; }

  set_unsigned** L_G_sigma = (set_unsigned**)calloc(n, sizeof(set_unsigned*));
  L_G_sigma[0] = (set_unsigned*)calloc(1,sizeof(set_unsigned)); set_unsigned_init(L_G_sigma[0]);
  for (i = 1; i < n; ++i) { L_G_sigma[i] = getL(G, sigma, i-1, sigma[i]); }
  
  unsigned* f_sigma = (unsigned*)calloc(n, sizeof(unsigned));
  for (i = 0; i < n; ++i) { f_sigma[i] = get_f_sigma(G, sigma_i, sigma[i]); }
  
  /* for (i = 0; i < n; ++i) { printf("f_sigma[%2d] = %2d, sigma[%2d] = %2d : ", i, f_sigma[i], i, sigma[i]); set_show_unsigned(L_G_sigma[i]); putchar('\n'); } */

  /******************************************** main process ********************************************/
  list_unsigned_cell* p;
  /* unsigned_node* s; */
  set_unsigned *S = NULL, *diff = NULL, *Rho = NULL;
  unsigned u, w = 0, alpha, beta;
  for (i = 0; i < n; ++i) {
    /* unsigned z; for (z = 0; z < n; ++z) { printf("%d", unvisited[z]); } putchar('\n'); */
    if (0 == i) { rho[i] = sigma[n-1]; }
    else {
      /* unsigned k; for (k = 0; k < n; ++k) { printf("label[%2d] : ", k); if (unvisited[k] && NULL != label[k]) printf("%s\n", label[k]); else putchar('\n'); } */
      S = getS(n, label, unvisited);
      /* printf("S    : "); set_show_unsigned(S); putchar('\n'); */
      alpha = get_alpha(sigma_i, S);
      beta = get_beta(sigma_i, S);
      w = get_w(f_sigma, S);
      /* printf("L_G  : "); set_show_unsigned(L_G_sigma[sigma[alpha]]); putchar('\n'); */
      diff = (set_unsigned*)calloc(1, sizeof(set_unsigned));
      set_unsigned_copy(diff, L_G_sigma[sigma[alpha]]);
      Rho = get_rho_i(i-1,rho);
      set_unsigned_diff(diff, Rho);
      /* printf("Rho  : "); set_show_unsigned(Rho); putchar('\n'); */
      /* printf("diff : "); set_show_unsigned(diff); putchar('\n'); */
      if (!set_unsigned_is_empty(diff)) { rho[i] = sigma[alpha]; }
      else if (beta < f_sigma[w]) { rho[i] = w; }
      else { rho[i] = sigma[beta]; }
      /* printf("rho[%2d] = %2d, alpha = %2d, beta = %2d, w = %2d\n", i, rho[i], alpha, beta, w); */
      set_unsigned_clear(Rho);
      set_unsigned_clear(diff);
      set_unsigned_clear(S);
    }

    unvisited[rho[i]] = false;
    for (p = G.al[rho[i]]->head; NULL != p; p = p->next) {
      u = p->key;
      if (unvisited[u]) { label[u] = add_label_i(label[u], n-i-1, n-1); } /* label(u) = label(u) ∪ {i}; */
    }
    /* unsigned z; for (z = 0; z < n; ++z) if (NULL != label[z]) printf("%d: %s\n", z, label[z]); */
  }

  /****************************** clean up the memory allocated by calloc *******************************/
  free(f_sigma);
  for (i = 0; i < n; ++i) { set_unsigned_clear(L_G_sigma[i]); }
  free(L_G_sigma);
  free(sigma_i);
  free(unvisited);
  free(label);
  
  return rho;
}

bool is_UI_ordering(unsigned* sigma, graph G) {
  const unsigned n = G.size;
  unsigned* sigma_i = get_inverse(sigma, n);

  unsigned si, sk, i, j, k;
  list_unsigned_cell *p, *q;
  for (si = 0; si < n; ++si)
    for (p = G.al[si]->head; NULL != p; p = p->next) {
      sk = p->key;
      i = sigma_i[si];
      k = sigma_i[sk];
      if (k < i) { unsigned tmp = k; k = i; i = tmp; }
      for (j = i+1; j <= k-1; ++j) {
	/* printf("i = %d, j = %d, k = %d, si = %d, sj = %d, sk = %d\n", i, j, k, sigma[i], sigma[j], sigma[k]); */
	bool flag1 = false, flag2 = false;
	for (q = G.al[sigma[j]]->head; NULL != q; q = q->next) { if (q->key == sigma[i]) { flag1 = true; break; } }
	for (q = G.al[sigma[j]]->head; NULL != q; q = q->next) { if (q->key == sigma[k]) { flag2 = true; break; } }
	if (!flag1 || !flag2) { /* printf("i = %d, j = %d, k = %d, si = %d, sj = %d, sk = %d\n", i, j, k, sigma[i], sigma[j], sigma[k]); */
	    return false; }
      }
    }
  
  free(sigma_i);
  return true;
}

bool is_I_ordering(unsigned* sigma, graph G) {
  const unsigned n = G.size;
  unsigned* sigma_i = get_inverse(sigma, n);

  /* print_order(sigma, n); putchar('\n'); */
  /* print_order(sigma_i, n); putchar('\n'); */
  unsigned si, sk, i, j, k;
  list_unsigned_cell *p, *q;
  for (si = 0; si < n; ++si)
    for (p = G.al[si]->head; NULL != p; p = p->next) {
      sk = p->key;
      i = sigma_i[si];
      k = sigma_i[sk];
      if (k < i) { unsigned tmp = k; k = i; i = tmp; }
      for (j = i+1; j <= k-1; ++j) {
	/* printf("i = %d, j = %d, k = %d, si = %d, sj = %d, sk = %d\n", i, j, k, sigma[i], sigma[j], sigma[k]); */
	bool flag = false;
	for (q = G.al[sigma[j]]->head; NULL != q; q = q->next) { if (q->key == sigma[i]) { flag = true; break; } }
	if (!flag) { /* printf("i = %d, j = %d, k = %d, si = %d, sj = %d, sk = %d\n", i, j, k, sigma[i], sigma[j], sigma[k]); */
	    return false; }
      }
    }
  
  free(sigma_i);
  return true;
}

Ordering Four_Sweep_LBFS(graph G) {
  /* const unsigned n = G.size; */
  /* const unsigned d = floor(log10(n)) + 1; */
  /* unsigned i; */
  unsigned* delta = lbfs(G);
  /* unsigned* delta = (unsigned*)calloc(n, sizeof(unsigned)); */
  /* delta[0] = 0; */
  /* delta[1] = 1; */
  /* delta[2] = 14; */
  /* delta[3] = 4; */
  /* delta[4] = 13; */
  /* delta[5] = 5; */
  /* delta[6] = 8; */
  /* delta[7] = 7; */
  /* delta[8] = 9; */
  /* delta[9] = 10; */
  /* delta[10] = 11; */
  /* delta[11] = 12; */
  /* delta[12] = 6; */
  /* delta[13] = 3; */
  /* delta[14] = 15; */
  /* delta[15] = 2; */
  /* delta[16] = 16; */
  
  /* printf("LexBFS  ordering : %*d", d, delta[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, delta[i]); } putchar('\n'); */
  
  unsigned* sigma = lbfs_plus(G, delta);
  /* printf("LexBFS+ ordering : %*d", d, sigma[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, sigma[i]); } putchar('\n'); */

  unsigned* rho = lbfs_star(G, sigma);
  /* printf("LexBFS★ ordering : %*d", d, rho[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, rho[i]); } putchar('\n'); */
  
  unsigned* tau = lbfs_plus(G, rho);
  /* printf("LexBFS+ ordering : %*d", d, tau[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, tau[i]); } putchar('\n'); */

  Ordering o = NI;
  if (is_UI_ordering(tau, G)) { /* printf("Input Graph is an Unit Interval Graph\n"); */
    o = UI; }  
  
  else if (is_I_ordering(tau, G)) { /* printf("Input Graph is an Interval Graph\n"); */
    o = I; }
  
  free(tau);
  free(rho);
  free(sigma);
  free(delta);

  return o;
}

unsigned* Four_Sweep_LBFS2(graph G) {
  /* const unsigned n = G.size; */
  /* const unsigned d = floor(log10(n)) + 1; */
  /* unsigned i; */

  unsigned* delta = lbfs(G);
  /* printf("LexBFS  ordering : %*d", d, delta[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, delta[i]); } putchar('\n'); */
  unsigned* sigma = lbfs_plus(G, delta);
  /* printf("LexBFS+ ordering : %*d", d, sigma[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, sigma[i]); } putchar('\n'); */
  unsigned* rho = lbfs_star(G, sigma);
  /* printf("LexBFS★ ordering : %*d", d, rho[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, rho[i]); } putchar('\n'); */
  unsigned* tau = lbfs_plus(G, rho);
  /* printf("LexBFS+ ordering : %*d", d, tau[0]); */
  /* for (i = 1; i < n; ++i) { printf(", %*d", d, tau[i]); } putchar('\n'); */

  /* Ordering o = NI; */
  /* if (is_UI_ordering(tau, G)) { /\* printf("Input Graph is an Unit Interval Graph\n"); *\/ */
  /*   o = UI; }   */
  /* else if (is_I_ordering(tau, G)) { /\* printf("Input Graph is an Interval Graph\n"); *\/ */
  /*   o = I; } */

  free(rho);
  free(sigma);
  free(delta);

  return tau;
}

#endif

  /* unsigned* tau_i = get_inverse(tau, n); */
  /* for (i = 0; i < n; ++i) { */
  /*   set_unsigned* L = getL(G, tau, i, tau[i]); */
  /*   set_show_unsigned(L); putchar('\n'); */
  /*   printf("f_σ(%u) = %u\n", i, get_f_sigma(G, tau_i, i)); */
  /*   set_unsigned_clear(L); */
  /* } */

  /* printf("inverse          : %*d", 2, sigma_i[0]); */
  /* for (l = 1; l < n; ++l) { printf(", %*d", 2, sigma_i[l]); } */
  /* printf("inverse          : %2d", sigma_i[0]); */
  /* for (l = 1; l < n; ++l) { printf(", %2d", sigma_i[l]); } */
  /* putchar('\n'); */
