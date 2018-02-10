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

	struct timespec s, e;

	clock_gettime(CLOCK_REALTIME, &s);
  do_linear_search(R, H);
	clock_gettime(CLOCK_REALTIME, &e);

	if (e.tv_nsec < s.tv_nsec) {
		printf("Linear Seach Time = %10ld.%09ld\n", e.tv_sec - s.tv_sec - 1, e.tv_nsec + 1000000000 - s.tv_nsec);
	} else {
		printf("Linear Seach Time = %10ld.%09ld\n", e.tv_sec - s.tv_sec, e.tv_nsec - s.tv_nsec);
	}
  
	list_rulelist* RR = list_rule_to_list_rulelist(R);
  /* list_rulelist_print(RR); */
  /* list_rulelist_print2(RR); */

  srbt*** S = mk_srbt_list(RR, R->size);
  /* list_srbt_print(S, RR->size, strlen(R->head->key->cond)-1); */
  
	clock_gettime(CLOCK_REALTIME, &s);
  do_list_srbt_search(S, RR->size, R->size, H);
	clock_gettime(CLOCK_REALTIME, &e);

	if (e.tv_nsec < s.tv_nsec) {
		printf("LSRBT Seach Time  = %10ld.%09ld\n", e.tv_sec - s.tv_sec - 1, e.tv_nsec + 1000000000 - s.tv_nsec);
	} else {
		printf("LSRBT Seach Time  = %10ld.%09ld\n", e.tv_sec - s.tv_sec, e.tv_nsec - s.tv_nsec);
	}
	printf("LSRBT size = %d\n", RR->size);

	/*
  free_srbt_list(S, RR->size, strlen(R->head->key->cond)-1);

  list_rulelist_clear(RR);
  list_rule_clear(R);
  free_header_list(H);
	*/

  /* c_rulelist* c_rulelist = read_class_bench_rule_list(argv[1]); */
  /* c_rulelist_print(c_rulelist); */
  /* free_class_bench_rule_list(c_rulelist); */
  
  return 0;
}
