#include <main.h>

int main(int argc, char** argv) {
  if (3 != argc && 4 != argc) {
    fprintf(stderr, "ERROR!!\nUsage: $ ./filter [-c] <rule list> <header list>\n");
    exit(1);
  }

  headerlist* H = read_header_list(argv[2]);
  /* header_list_print(H); putchar('\n'); */

  list_rule* R = read_rule_list(argv[1]);
  list_rule_print(R);
  
  do_linear_search(R, H);

  
  list_rulelist* RR = list_rule_to_list_rulelist(R);
  /* list_rulelist_print(RR); */
  /* list_rulelist_print2(RR); */
  /* if (does_match_header(RR->head->key->head->key, H->h[0])) { printf("match !!\n"); } */
  
  srbt** s1 = mk_srbt(RR->head->key, R->size);
  srbts_print(s1, 120);
  srbt** s2 = mk_srbt(RR->head->next->key, R->size);
  srbts_print(s2, 120);
  free_srbts(s1, 120);
  free_srbts(s2, 120);
  
  list_rulelist_clear(RR);

  
  /* if (list_rule_isC1P(rulelist)) { printf("isC1P\n"); } */
  
  /* list_rule_print(rulelist);   putchar('\n'); */
  /* list_rule* copy = list_rule_copy(rulelist); */

  /* list_rulelist_insert(RL, rulelist); */
  /* list_rulelist_insert(RL, copy); */
  /* list_rulelist_print(RL); */
  
  /* list_rule_print(copy); */
  /* list_rule_clear(copy); */
  /* if (list_rule_eq(rulelist, copy)) { printf("equal!\n"); } */
  /* else { printf("hoge\n"); } */
      
  list_rule_clear(R);
  free_header_list(H);
  
  /* list_rulelist_clear(RL); */
  
  /* c_rulelist* c_rulelist = read_class_bench_rule_list(argv[1]); */
  /* c_rulelist_print(c_rulelist); */
  /* free_class_bench_rule_list(c_rulelist); */
  
  return 0;
}
