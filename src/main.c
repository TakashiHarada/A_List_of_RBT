#include <main.h>

int main(int argc, char** argv) {
  if (3 != argc && 4 != argc) {
    fprintf(stderr, "ERROR!!\nUsage: $ ./filter [-c] <rule list> <header list>\n");
    exit(1);
  }
  /* list_rulelist* RL = (list_rulelist*)malloc(sizeof(list_rulelist)); */

  list_rule* rulelist = read_rule_list(argv[1]);
  if (list_rule_isC1P(rulelist)) { printf("isC1P\n"); }
  /* list_rule_print(rulelist);   putchar('\n'); */
  /* list_rule* copy = list_rule_copy(rulelist); */

  /* list_rulelist_insert(RL, rulelist); */
  /* list_rulelist_insert(RL, copy); */
  /* list_rulelist_print(RL); */
  
  /* list_rule_print(copy); */
  /* list_rule_clear(copy); */
  /* if (list_rule_eq(rulelist, copy)) { printf("equal!\n"); } */
  /* else { printf("hoge\n"); } */
      
  list_rule_clear(rulelist);
  /* list_rulelist_clear(RL); */
  
  /* c_rulelist* c_rulelist = read_class_bench_rule_list(argv[1]); */
  /* c_rulelist_print(c_rulelist); */
  /* free_class_bench_rule_list(c_rulelist); */
  
  return 0;
}
