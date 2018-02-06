/* graph.h */
/*
 * array of linked list of unsigned
 * vertex is represented by unsigned
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef __TANAKALAB_GRAPH_H__
#define __TANAKALAB_GRAPH_H__

#ifndef __TANAKALAB_LIST_H__
#include "list.h"
#endif

#ifndef __TANAKALAB_SET_H__
#include "set.h"
#endif

struct MATRIX {
  unsigned m;
  unsigned n;
  char**   b;
};
typedef struct MATRIX matrix;

struct GRAPH {
  unsigned size; /* number of vertices */
  list_unsigned** al; /* adjacency list representing edges */
};
typedef struct GRAPH graph;

void print_graph(graph G) {
  const unsigned d = floor(log10(G.size)) + 1;
  unsigned i;
  for (i = 0; i < G.size; ++i) { printf("al[%*d] : ", d, i); list_unsigned_print(G.al[i]); putchar('\n'); }
}

unsigned* get_inverse(const unsigned* s, const unsigned n) {
  unsigned* inv = (unsigned*)calloc(n, sizeof(unsigned));
  unsigned i;
  for (i = 0; i < n; ++i) { inv[s[i]] = i; }
  return inv;
}

list_pair_unsigned* permutation_to_transpositions(unsigned* sigma, const unsigned n) {
  list_pair_unsigned* ts = (list_pair_unsigned*)calloc(1, sizeof(list_pair_unsigned));
  unsigned* tau = (unsigned*)calloc(1, sizeof(unsigned));
  unsigned i;
  for (i = 0; i < n; ++i) { tau[i] = sigma[i]; }
  unsigned* inv = get_inverse(tau, n);

  printf("tau : %*d", 2, tau[0]);
  unsigned k; for (k = 1; k < n; ++k) { printf(", %*d", 2, tau[k]); } putchar('\n');
  printf("inv : %*d", 2, inv[0]);
  for (k = 1; k < n; ++k) { printf(", %*d", 2, inv[k]); } putchar('\n');
  putchar('\n');

  for (i = 0; i < n; ++i) {
    unsigned j = n-1-i, tmp, ij;
    ij = inv[j];
    printf("j = %d, ij = %d, inv[%d] = %d, tau[%d] = %d, tau[ij = %d] = %d\n", j, ij, j, ij, j, tau[j], ij, tau[ij]);
    tmp = inv[tau[j]];
    inv[tau[j]] = inv[tau[ij]];
    inv[tau[ij]] = tmp;
    printf("inv : %*d", 2, inv[0]);
    for (k = 1; k < n; ++k) { printf(", %*d", 2, inv[k]); } putchar('\n');

    tmp = tau[j];
    tau[j] = tau[ij];
    tau[ij] = tmp;
    
    printf("tau : %*d", 2, tau[0]);
    for (k = 1; k < n; ++k) { printf(", %*d", 2, tau[k]); } putchar('\n');
    printf("inv : %*d", 2, inv[0]);
    for (k = 1; k < n; ++k) { printf(", %*d", 2, inv[k]); } putchar('\n');
    putchar('\n');
  }
  
  printf("tau : %*d", 2, tau[0]);
  for (i = 1; i < n; ++i) { printf(", %*d", 2, tau[i]); } putchar('\n');
  free(tau);
  free(inv);
  return ts;
}

void print_matrix(matrix* M) {
  unsigned i, j;
  for (i = 0; i < M->m; ++i) {
    for (j = 0; j < M->n; ++j) putchar(M->b[i][j]);
    putchar('\n');
  }
}

matrix* permute_matrix2(matrix* M, unsigned* sigma) {
  matrix* N = (matrix*)calloc(1, sizeof(matrix));
  N->m = M->m;
  N->n = M->n;
  N->b = (char**)calloc(N->m, sizeof(char*));
  unsigned i, j;
  for (i = 0; i < N->m; ++i) { N->b[i] = (char*)calloc(N->n, sizeof(char)); }
  for (j = 0; j < N->n; ++j) { for (i = 0; i < N->m; ++i) { N->b[i][sigma[j]] = M->b[i][j]; } }

  return N;
}

void permute_matrix(matrix* M, unsigned* sigma) {
  unsigned i, j;
  char tmp;
  for (j = 0; j < M->n; ++j)
    for (i = 0; i < M->m; ++i) {
      /* tmp = M->b[i][sigma[j]]; */
      /* M->b[i][sigma[j]] = M->b[i][j]; */
      /* M->b[i][j] = tmp; */
      ;
    }
}

set_unsigned** c1p_matrix_to_clique(matrix* M) {
  set_unsigned** clique = (set_unsigned**)calloc(M->n, sizeof(set_unsigned*));
  unsigned i, j;
  for (j = 0; j < M->n; ++j) {
    clique[j] = (set_unsigned*)calloc(1, sizeof(set_unsigned));
    set_unsigned_init(clique[j]);
    for (i = 0; i < M->m; ++i) { if ('1' == M->b[i][j]) { set_unsigned_insert(clique[j], i); } }
  }
  return clique;
}

matrix* add_UnitMatrix(matrix* M) {
  matrix* N = (matrix*)calloc(1, sizeof(matrix));
  unsigned i, j;
  N->b = (char**)calloc((M->m)+(M->n), sizeof(char*));
  N->m = M->m + M->n;
  N->n = M->n;
  for (i = 0; i < N->m; ++i) { N->b[i] = (char*)calloc(M->n, sizeof(char)); }
  for (i = 0; i < M->m; ++i) { for (j = 0; j < M->n; ++j) { N->b[i][j] = M->b[i][j]; } }
  for (i = M->m; i < N->m; ++i) { for (j = 0; j < N->n; ++j) { N->b[i][j] = '0'; } }
  for (i = 0; i < (N->m)-(M->m); ++i) { N->b[M->m+i][i] = '1'; }

  return N;
}
  
matrix* read_matrix(char* filename) {
   FILE* fp;
  if (NULL == (fp = fopen(filename,"r"))) {
    fprintf(stderr,"ERROR: Can't read the graph file.\n");
    exit(1);
  }
  char* line = NULL;
  size_t len = 0;
  unsigned m = 0;
  unsigned n;
  if (-1 != getline(&line, &len, fp)) { ++m, n = strlen(line)-1; }
  while (-1 != getline(&line, &len, fp)) { ++m; }

  matrix* M = (matrix*)calloc(1, sizeof(matrix));
  M->b = (char**)calloc(m,sizeof(char*));
  M->m = m;
  M->n = n;
  
  unsigned i, j;
  rewind(fp);
  for (i = 0; getline(&line, &len, fp) != -1; ++i) {
    M->b[i] = (char*)calloc(n, sizeof(char));
    for (j = 0; j < n; ++j) { M->b[i][j] = line[j]; }
  }
  fclose(fp);
  return M;
}

graph* matrix_to_graph(matrix* M) {
  graph* g = (graph*)calloc(1,sizeof(graph));
  bool* flag = (bool*)calloc(M->m, sizeof(bool));
  g->size = M->m;
  g->al = (list_unsigned**)calloc(M->m, sizeof(list_unsigned*));
  unsigned i, j, k;
  for (i = 0; i < M->m; ++i) {
    g->al[i] = (list_unsigned*)calloc(1,sizeof(list_unsigned));
    for (j = 0; j < M->m; ++j) { flag[j] = false; }
    for (j = 0; j < M->n; ++j) { if ('1' == M->b[i][j]) { for (k = 0; k < M->m; ++k) { if ('1' == M->b[k][j]) { flag[k] = true; } } } }
    for (j = 0; j < M->m; ++j) { if (i != j && flag[j]) { list_unsigned_insert(g->al[i], j); } }
  }
  free(flag);
  return g;
}

graph* read_graph(char* filename) {
  FILE* fp;
  if (NULL == (fp = fopen(filename,"r"))) {
    fprintf(stderr,"ERROR: Can't read the graph file.\n");
    exit(1);
  }
  char* line = NULL;
  size_t len = 0;
  unsigned n = 0;
  while (getline(&line, &len, fp) != -1) { ++n; }

  graph* G = (graph*)calloc(1, sizeof(graph));
  G->size = n;
  G->al = (list_unsigned**)calloc(n, sizeof(list_unsigned*));
  unsigned i;
  for (i = 0; i < n; ++i) { G->al[i] = (list_unsigned*)calloc(1,sizeof(list_unsigned)); }
  rewind(fp);
  unsigned j, k, l;
  char s[255];
  for (i = 0; getline(&line, &len, fp) != -1; ++i) {
    /* printf("=====] %s", line); */
    for (j = 0; (line[j] != '\n' && line[j] != '\0') && line[j] != ' '; ++j) ;
    if (line[j] != '\n' && line[j] != '\0') {
      for (l = 0, k = j+1; k < strlen(line); ++k) {
	if (line[k] == ',') { 
	  s[l] = '\0'; l = 0; /* putchar('\n'); */
	  /* printf("%d\n", atoi(s)); */
	  list_unsigned_insert(G->al[i], atoi(s));
	}
	else { s[l] = line[k], ++l; /* putchar(line[k]); */ }
      }
      /* printf("%d\n", atoi(s)); */
      list_unsigned_insert(G->al[i], atoi(s));
    }
  }
  fclose(fp);
  return G;
}

#endif
