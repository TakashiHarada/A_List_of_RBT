/* srbt.h (Single Run-Based Trie) */

#ifndef __TANAKALAB_SRBT_H__
#define __TANAKALAB_SRBT_H__

#ifndef __TANAKALAB_RULE_H__
#include "rule.h"
#endif

#ifndef __TANAKALAB_HEADER_H__
#include "header.h"
#endif

struct SRBT {
  int var;
  struct SRBT* left;
  struct SRBT* right;
  unsigned tn;
  unsigned rule;
  unsigned candidate_rule;
};
typedef struct SRBT srbt;

srbt*** mk_srbt_list(list_rulelist*, unsigned);
srbt** mk_srbt(list_rule*, unsigned);
srbt** mk_backbone_rbt(list_rule*, unsigned);
void sub_mk_srbt(srbt*, srbt*);
void set_roots(srbt**, unsigned, unsigned, unsigned*);
srbt* mk_srbt_node(int, unsigned, unsigned);
unsigned get_start_point(char*, unsigned, unsigned*);
void modify_tj(srbt**, unsigned, rule*, unsigned*, unsigned);
void srbt_print(unsigned, srbt*);
void srbts_print(srbt**, unsigned);
void list_srbt_print(srbt***, unsigned, unsigned);

void free_srbt_list(srbt***, unsigned, unsigned);
void free_srbts(srbt**, unsigned);
void free_srbt(unsigned, srbt*);

void do_list_srbt_search(srbt***, unsigned, unsigned, headerlist*);
unsigned list_srbt_search(srbt***, unsigned, unsigned, header);
unsigned srbt_search(srbt*, unsigned, header);

void do_list_srbt_search(srbt*** S, unsigned size, unsigned N, headerlist* H) {
  // printf("==================== SRBT Search ====================\n");
  unsigned i;
  // for (i = 0; i < H->n; ++i) { printf("h[%d] = %s ---> %d\n", H->h[i].num, H->h[i].string, list_srbt_search(S, size, N, H->h[i])); }
  for (i = 0; i < H->n; ++i) { list_srbt_search(S, size, N, H->h[i]); }
}

unsigned list_srbt_search(srbt*** S, unsigned size, unsigned N, header h) {
  unsigned i, m, candidate = N+1;
  for (i = 0; i < size; ++i) {
    /* printf("=== %d ===\n", i); */
    m = srbt_search(S[i][0], candidate, h);
    if (m < candidate) { candidate = m; }
  }
  return candidate;
}

unsigned srbt_search(srbt* ptr, unsigned N, header h) {
  srbt* t = ptr;
  unsigned rn = N;

  do {
    if (0 != t->rule && t->rule < rn) { rn = t->rule; }
    /* printf("t->var = %d\n", t->var); */
    if ('0' == h.string[t->var]) { t = t->left; }
    else { t = t->right; }
  } while (NULL != t);

  return rn;
}


srbt*** mk_srbt_list(list_rulelist* RR, unsigned N) {
  list_rulelist_cell* p;
  srbt*** S = (srbt***)calloc(RR->size, sizeof(srbt**));
  unsigned i;
  for (i = 0, p = RR->head; NULL != p; p = p->next, ++i) { S[i] = mk_srbt(p->key, N); }
  return S;
}

srbt** mk_srbt(list_rule* R, unsigned N) {
  srbt** srbt = mk_backbone_rbt(R, N);
  const unsigned w = strlen(R->head->key->cond)-1;
    
  int i;
  for(i = w-1; 1 <= i; i--){
    if(srbt[i-1]->left == NULL){
      if(srbt[i]->candidate_rule != N+1){
  	srbt[i-1]->left = srbt[i];
  	if(/*srbt[i-1]->candidate_rule == 0 ||*/ srbt[i-1]->candidate_rule > srbt[i]->candidate_rule)
  	  srbt[i-1]->candidate_rule = srbt[i]->candidate_rule;
      }
    }
    else{
      sub_mk_srbt(srbt[i-1]->left,srbt[i]);
      if(srbt[i-1]->candidate_rule > srbt[i-1]->left->candidate_rule)
  	srbt[i-1]->candidate_rule = srbt[i-1]->left->candidate_rule;
    }
    if(NULL == srbt[i-1]->right) {
      if(srbt[i]->candidate_rule != N+1){
  	srbt[i-1]->right = srbt[i];
  	if(/*srbt[i-1]->candidate_rule == 0 || */srbt[i-1]->candidate_rule > srbt[i]->candidate_rule)
  	  srbt[i-1]->candidate_rule = srbt[i]->candidate_rule;
      }
    }
    else{
      sub_mk_srbt(srbt[i-1]->right,srbt[i]);
      if(srbt[i-1]->candidate_rule > srbt[i-1]->right->candidate_rule) { srbt[i-1]->candidate_rule = srbt[i-1]->right->candidate_rule; }
    }
  }
  return srbt;
}

void sub_mk_srbt(srbt* y_srbt, srbt* o_srbt) {
  if (0 != o_srbt->rule) {
    if(0 == y_srbt->rule) {
      y_srbt->rule = o_srbt->rule;
      if (o_srbt->rule < y_srbt->candidate_rule) { y_srbt->candidate_rule = o_srbt->rule; }
    }
    else if(o_srbt->rule < y_srbt->rule){
      y_srbt->rule = o_srbt->rule;
      if (o_srbt->rule < y_srbt->candidate_rule) { y_srbt->candidate_rule = o_srbt->rule; }
    }
  }
  if (NULL != o_srbt->left) {
    if(NULL == y_srbt->left) {
      if(y_srbt->rule == 0 || o_srbt->left->candidate_rule < y_srbt->rule) {
	y_srbt->left = o_srbt->left;
	if(y_srbt->candidate_rule > o_srbt->left->candidate_rule) { y_srbt->candidate_rule = o_srbt->left->candidate_rule; }
      }
    }
    else{
      sub_mk_srbt(y_srbt->left,o_srbt->left);
      if(y_srbt->left->candidate_rule < y_srbt->candidate_rule) { y_srbt->candidate_rule = y_srbt->left->candidate_rule; }
    }
  }
  if(NULL != o_srbt->right) {
    if(NULL == y_srbt->right) {
      if(y_srbt->rule == 0 || o_srbt->right->candidate_rule < y_srbt->rule) {
	y_srbt->right = o_srbt->right;
	if(y_srbt->candidate_rule > o_srbt->right->candidate_rule) { y_srbt->candidate_rule = o_srbt->right->candidate_rule; }
      }
    }
    else {
      sub_mk_srbt(y_srbt->right,o_srbt->right);
      if (y_srbt->right->candidate_rule < y_srbt->candidate_rule) { y_srbt->candidate_rule = y_srbt->right->candidate_rule; }
    }
  }
}

srbt** mk_backbone_rbt(list_rule* R, unsigned N) {
  const unsigned w = strlen(R->head->key->cond)-1;
  /* const unsigned n = R->size; */
  srbt** rbt = (srbt**)calloc(w, sizeof(srbt*));

  unsigned* sigma_i = (unsigned*)calloc(w, sizeof(unsigned));
  unsigned i; for (i = 0; i < w; ++i) { sigma_i[R->sigma[i]] = i; }
  set_roots(rbt, w, N, sigma_i);
  
  list_rule_cell* p;
  for (p = R->head; NULL != p; p = p->next) {
    unsigned j = get_start_point(p->key->cond, w, sigma_i);
    modify_tj(rbt, j, p->key, sigma_i, w);
  }

  free(sigma_i);
  return rbt;
}

/* modify a T[j] */
void modify_tj(srbt** rbt, unsigned j, rule* r, unsigned* sigma, unsigned w) {
  srbt* t = rbt[j];
  int trie_num = j;
  /* if there is no rule on the T[j] 
      then the candidate rule number of the root on T[j] is a rule number of r */
  if (r->num < t->candidate_rule) { t->candidate_rule = r->num; }
  for ( ; '*' != r->cond[sigma[j]] && '\0' != r->cond[sigma[j]] && j < w; ++j) {
    /* if the node pointed by t has a rule then the rule r is redundant */
    if (t->rule != 0) { return ; }
    if ('0' == r->cond[sigma[j]]) {
      if (NULL == t->left) { t->left = mk_srbt_node(sigma[j+1],trie_num,r->num); }
      t = t->left;
    } else {
      if (NULL == t->right) { t->right = mk_srbt_node(sigma[j+1],trie_num,r->num); }
      t = t->right;
    }
  }
  
  /* add the rule to the node pointed by the pointer t */
  if(t->rule == 0) { t->rule = r->num; }
}

unsigned get_start_point(char* cond, unsigned w, unsigned* sigma) {
  unsigned i;
  for (i = 0; i < w; ++i)
    if ('*' != cond[sigma[i]]) { break; }
  return i;
}

void set_roots(srbt** rbt, unsigned w, unsigned n, unsigned* sigma) {
  unsigned i;
  for (i = 0; i < w; ++i) { rbt[i] = mk_srbt_node(sigma[i],i,n+1); }
}

srbt* mk_srbt_node(int var, unsigned tn, unsigned candidate_rule) {
  srbt* new = (srbt*)calloc(1, sizeof(srbt));
  new->var = var;
  new->tn = tn;
  new->rule = 0;
  new->candidate_rule = candidate_rule;
  new->left = new->right = NULL;
  
  return new;
}

void free_srbt_list(srbt*** s, unsigned size, unsigned w) {
  unsigned i, j;
  for (i = 0; i < size; ++i) { for (j = 0; j < w; ++j) { free_srbt(j, s[i][j]); } }
}

void free_srbts(srbt** srbt, unsigned w) {
  unsigned i;
  for (i = 0; i < w; ++i) { free_srbt(i, srbt[i]); }
}

void free_srbt(unsigned i, srbt* ptr) {
  if (NULL == ptr) { return; }
  if(ptr->tn != i){ return; }
  free_srbt(i, ptr->left);
  free_srbt(i, ptr->right);
  free(ptr);
}

void srbt_print(unsigned i, srbt* ptr) {
  if(ptr == NULL || ptr->tn != i) { return ; }
  printf("tn=%d, v=%d, r=%d, cr=%d\n", ptr->tn, ptr->var, ptr->rule, ptr->candidate_rule);
  srbt_print(i, ptr->left);
  srbt_print(i, ptr->right);
}

void srbts_print(srbt** srbt, unsigned w) {
  unsigned i;
  for (i = 0; i < w; ++i) { printf("=== SRBT[%d] ===\n", i); srbt_print(i, srbt[i]); putchar('\n'); }
}

void list_srbt_print(srbt*** S, unsigned size, unsigned w) {
  unsigned i;
  for (i = 0; i < size; ++i) { printf("===== SS[%d] =====\n", i); srbts_print(S[i], w); putchar('\n'); }
}

#endif
