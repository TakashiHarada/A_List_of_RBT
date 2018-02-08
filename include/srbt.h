/* srbt.h (Single Run-Based Trie) */

#ifndef __TANAKALAB_SRBT_H__
#define __TANAKALAB_SRBT_H__

#ifndef __TANAKALAB_RULE_H__
#include "rule.h"
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

srbt** mk_srbt(list_rule*);
srbt** mk_backbone_rbt(list_rule*);
void sub_mk_srbt(srbt*,srbt*);
void set_roots(srbt**, unsigned, unsigned, unsigned*);
srbt* mk_srbt_node(int, unsigned, unsigned);

unsigned get_start_point(char*, unsigned);
void modify_tj(srbt**, unsigned, rule*);

void srbt_print(srbt*);
void srbts_print(srbt**, unsigned);
/* void post_order(srbt*, unsigned, char*); */

void free_srbts(srbt**, unsigned);
void free_srbt(unsigned, srbt*);


srbt** mk_srbt(list_rule* R) {
  srbt** srbt = mk_backbone_rbt(R);

  const unsigned w = strlen(R->head->key->cond);
  const unsigned n = R->size;
  
  int i;
  for(i = w-1; 1 <= i; i--){
    if(srbt[i-1]->left == NULL){
      if(srbt[i]->candidate_rule != n+1){
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
      if(srbt[i]->candidate_rule != n+1){
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

srbt** mk_backbone_rbt(list_rule* R) {
  const unsigned w = strlen(R->head->key->cond);
  const unsigned n = R->size;
  srbt** rbt = (srbt**)malloc(w*sizeof(srbt*));
  set_roots(rbt, w, n, R->sigma);

  list_rule_cell* p;
  for (p = R->head; NULL != p; p = p->next) {
    unsigned j = get_start_point(p->key->cond, w);
    modify_tj(rbt, j, p->key);
  }
  
  return rbt;
}

/* modify a T[j] */
void modify_tj(srbt** rbt, unsigned j, rule* r) {
  srbt* t = rbt[j];
  int trie_num = j;
  /* if there is no rule on the T[j] 
      then the candidate rule number of the root on T[j] is a rule number of r */
  if (r->num < t->candidate_rule) { t->candidate_rule = r->num; }

  for ( ; '*' != r->cond[j] && '\0' != r->cond[j]; ++j) {
    /* if the node pointed by t has a rule then the rule r is redundant */
    if (t->rule != 0) { return ; }
    if ('0' == r->cond[j]) {
      if (NULL == t->left) { t->left = mk_srbt_node(j+1,trie_num,r->num); }
      t = t->left;
    } else {
      if (NULL == t->right) { t->right = mk_srbt_node(j+1,trie_num,r->num); }
      t = t->right;
    }
  }
  
  /* add the rule to the node pointed by the pointer t */
  if(t->rule == 0) { t->rule = r->num; }
}

unsigned get_start_point(char* cond, unsigned w) {
  unsigned i;
  for (i = 0; i < w; ++i)
    if ('*' != cond[i]) { break; }
  return i;
}

void set_roots(srbt** rbt, unsigned w, unsigned n, unsigned* sigma) {
  unsigned i;
  unsigned* sigma_i = (unsigned*)malloc(w*sizeof(unsigned));
  for (i = 0; i < w; ++i) { sigma_i[sigma[i]] = i; }
  for (i = 0; i < w; ++i) { rbt[i] = mk_srbt_node(sigma_i[i],i,n+1); }
  free(sigma_i);
}

srbt* mk_srbt_node(int var, unsigned tn, unsigned candidate_rule) {
  srbt* new = (srbt*)malloc(sizeof(srbt));
  new->var = var;
  new->tn = tn;
  new->rule = 0;
  new->candidate_rule = candidate_rule;
  new->left = new->right = NULL;
  
  return new;
}

void free_srbts(srbt** srbt, unsigned w) {
  unsigned i;
  for (i = 0; i < w; ++i) { free_srbt(i,srbt[i]); }
}

void free_srbt(unsigned i,srbt* ptr) {
  if (NULL == ptr) { return; }
  if(ptr->tn != i){return;}
  free_srbt(i,ptr->left);
  free_srbt(i,ptr->right);
  free(ptr);
}

void srbt_print(srbt* ptr){
  if(ptr == NULL){
    printf("_ ");
    return;
  }
  printf("{tn=%d, v=%d, r=%d, cr=%d ",ptr->tn,ptr->var,ptr->rule,ptr->candidate_rule);
  srbt_print(ptr->left);
  srbt_print(ptr->right);
  printf("}");
}

void srbts_print(srbt** srbt, unsigned w) {
  unsigned i;
  for (i = 0; i < w; ++i) { srbt_print(srbt[i]); putchar('\n'); }
}

#endif
