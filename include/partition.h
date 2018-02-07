/* partition.h */

#ifndef __TANAKALAB_PARTITION_H__
#define __TANAKALAB_PARTITION_H__


#ifndef __TANAKALAB_C1P_H__
#include "c1p.h"
#endif

#ifndef __TANAKALAB_RULE_H__
#include "rule.h"
#endif

matrix* rulelist_to_matrix(list_rule*);
bool list_rule_isC1P(list_rule*);


matrix* rulelist_to_matrix(list_rule* L) {
  list_rule_cell* p = L->head;
  const unsigned m = L->size;
  const unsigned n = strlen(p->key->cond)-1;
  matrix* M = (matrix*)malloc(sizeof(matrix));
  M->m = m;
  M->n = n;
  M->b = (char**)malloc(m*sizeof(char*));
  unsigned i, j;
  for (i = 0; i < m; ++i) { M->b[i] = (char*)malloc(n*sizeof(char)); }

  for (i = 0; NULL != p; p = p->next, ++i)
    for (j = 0; j < n; ++j)
      if ('*' == p->key->cond[j]) { M->b[i][j] = '0'; }
      else { M->b[i][j] = '1'; }
  return M;
}

bool list_rule_isC1P(list_rule* L) {
  matrix* M = rulelist_to_matrix(L);
  L->sigma = get_c1p_order(M);
  if (NULL != L->sigma) {
    list_rule_print(L);
    putchar('\n');
    unsigned i;
    printf("%d ", L->sigma[0]);
    for (i = 1; i < M->n; ++i) printf(", %d", L->sigma[i]);
    putchar('\n');
    list_rule_print2(L);
  }
  
  bool flag = is_c1p(M);
  matrix_clear(M);
  return flag;
}

#endif
