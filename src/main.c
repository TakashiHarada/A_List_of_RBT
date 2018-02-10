#include <main.h>
#include <unistd.h>

int main(int argc, char** argv) {
  if (3 != argc && 4 != argc) {
    fprintf(stderr, "ERROR!!\nUsage: $ ./filter [-c] <rule list> <header list>\n");
    exit(1);
  }

  headerlist* H = read_header_list(argv[2]);
  /* header_list_print(H); putchar('\n'); */

  list_rule* R = read_rule_list(argv[1]);
  /* list_rule_print(R); */

	struct timespec l_start, l_end, srbt_start, srbt_end;
	long l_nsec, srbt_nsec;

	clock_getres(CLOCK_REALTIME, &l_start);
  do_linear_search(R, H);
	clock_getres(CLOCK_REALTIME, &l_end);
	srbt_nsec = l_end.tv_nsec - l_start.tv_nsec;
	printf("Linear Seach Time = %09ld\n", srbt_nsec);
  
	list_rulelist* RR = list_rule_to_list_rulelist(R);

  /* list_rulelist_print(RR); */
  /* list_rulelist_print2(RR); */

  srbt*** S = mk_srbt_list(RR, R->size);
  /* list_srbt_print(S, RR->size, strlen(R->head->key->cond)-1); */
  
	clock_getres(CLOCK_REALTIME, &srbt_start);
  do_list_srbt_search(S, RR->size, R->size, H);
	clock_getres(CLOCK_REALTIME, &srbt_end);
	l_nsec = srbt_end.tv_nsec - srbt_start.tv_nsec;
	printf("Linear Seach Time = %09ld\n", l_nsec);
  /* free_srbt_list(S, RR->size, strlen(R->head->key->cond)-1); */

	/*
  list_rulelist_clear(RR);
  list_rule_clear(R);
  free_header_list(H);
  */

  /* c_rulelist* c_rulelist = read_class_bench_rule_list(argv[1]); */
  /* c_rulelist_print(c_rulelist); */
  /* free_class_bench_rule_list(c_rulelist); */
  
  return 0;
}
