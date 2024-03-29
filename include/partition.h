/* partition.h */

#ifndef __TANAKALAB_PARTITION_H__
#define __TANAKALAB_PARTITION_H__


#ifndef __TANAKALAB_C1P_H__
#include "c1p.h"
#endif

#ifndef __TANAKALAB_RULE_H__
#include "rule.h"
#endif

list_rulelist* list_rule_to_list_rulelist(list_rule*);
matrix* rulelist_to_matrix(list_rule*);
bool list_rule_isC1P(list_rule*);
void set_C1Porder_to_rulelist(list_rule*);

list_rulelist* list_rule_to_list_rulelist(list_rule* L) {
  if (NULL == L || 0 == L->size) { return NULL; }
  list_rulelist* LL = (list_rulelist*)calloc(1, sizeof(list_rulelist));
  if (L->size < 3) { list_rulelist_insert(LL, L); return LL; }

  /* add the rul list containing the first and second rule of L to a list of rulelist LL */
  list_rule_cell* p;
  unsigned i;
  list_rule* R = (list_rule*)calloc(1, sizeof(list_rule));
  for (p = L->head, i = 0; i < 2; p = p->next, ++i) { list_rule_insert(R, p->key); }
  /* printf("List Size = %d\n", R->size); */
  list_rulelist_insert(LL, R);
  /* printf("List Size = %d\n", LL->last->key->size); */
  list_rule_clear(R);

  /* partition */
  list_rulelist_cell* q;
  for ( ; NULL != p; p = p->next) {
     // printf("w = %u, ", strlen(p->key->cond)); rule_print(p->key); putchar('\n');
    for (q = LL->last; NULL != q; q = q->prev) {
      list_rule_insert(q->key, p->key);
      if (list_rule_isC1P(q->key)) { break; }
      list_rule_remove_head(q->key);
    }
    // rule_print(p->key); putchar('\n');
    if (NULL == q) {
      list_rule* S = mk_new_list_rule(p->key);
      list_rulelist_insert(LL, S);
    }
    /* rule_print(p->key); */
  }

  /* set order */
  for (q = LL->head; NULL != q; q = q->next) { set_C1Porder_to_rulelist(q->key); }
       
  return LL;
}

void set_C1Porder_to_rulelist(list_rule* R) {
  matrix* M = rulelist_to_matrix(R);
  R->sigma = get_c1p_order(M);
  /* print_matrix(M); */
  /* unsigned i; */
  /* printf("%d", R->sigma[0]); */
  /* for (i = 1; i < M->n; ++i) { printf(", %d", R->sigma[i]); } putchar('\n'); */
  matrix_clear(M);
}

matrix* rulelist_to_matrix(list_rule* L) {
  list_rule_cell* p = L->head;
  const unsigned m = L->size;
  const unsigned n = strlen(p->key->cond)-1;
  matrix* M = (matrix*)calloc(1, sizeof(matrix));
  M->m = m;
  M->n = n;
  /* printf("L->size = %d, M->m = %d, M->n = %d\n", L->size, m, n); */
  M->b = (char**)calloc(m, sizeof(char*));
  unsigned i, j;
  for (i = 0; i < m; ++i) { M->b[i] = (char*)calloc(n, sizeof(char)); }

  for (i = 0; NULL != p; p = p->next, ++i)
    for (j = 0; j < n; ++j)
      if ('*' == p->key->cond[j]) { M->b[i][j] = '0'; }
      else { M->b[i][j] = '1'; }
  return M;
}

bool list_rule_isC1P(list_rule* L) {
  matrix* M = rulelist_to_matrix(L);
  /* L->sigma = get_c1p_order(M); */
  /* if (NULL != L->sigma) { */
    /* list_rule_print(L); */
    /* putchar('\n'); */
    /* unsigned i; */
    /* printf("%d ", L->sigma[0]); */
    /* for (i = 1; i < M->n; ++i) printf(", %d", L->sigma[i]); */
    /* putchar('\n'); */
    /* list_rule_print2(L); */
  /* } */
  
  bool flag = is_c1p(M);
  matrix_clear(M);
  return flag;
}

#endif
