/* tools.h */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __TANAKALAB_TOOLS_H__
#define __TANAKALAB_TOOLS_H__

#ifndef __TANAKALAB_LIST_H__
#include "list.h"
#endif

/* for ipv4 */
#define LOW 0
#define HIGH 65535
#define BIT_LENGTH 16

list_string* range_to_01m_strings(unsigned, unsigned, unsigned, unsigned);
list_string* int_pair_to_01m_string(unsigned, unsigned, unsigned);


/* LOW (=0) and HIGH (=65535) are predefined in the tool.h
     a-b  => 010101..10**, 111101..1111, ...
e.g. 3-17 => 0000000000000011,00000000000001**,0000000000001***,000000000001000*
   
*/

list_string* range_to_01m_strings(unsigned a, unsigned b, unsigned low, unsigned high)
{
  unsigned m = (low+high-1)/2;
  unsigned n = (low+high+1)/2;
  list_string* X;
  list_string* Y;
  /* printf("a = %d, b = %d, low = %d, high = %d, m = %d, n = %d\n", a, b, low, high, m, n); */

  if (a == low && b == high) { return int_pair_to_01m_string(a,b,BIT_LENGTH); }
  else if (b <= m) { return range_to_01m_strings(a,b,low,m); }
  else if (a <= m && n <= b) { 
    Y = range_to_01m_strings(a,m,low,m);
    X = range_to_01m_strings(n,b,n,high);
    list_strings_concat(X,Y);
  }
  else { return range_to_01m_strings(a,b,n,high); }

  return X;
}

list_string* int_pair_to_01m_string(unsigned a, unsigned b, unsigned w) {
  char* bin = (char*)malloc((w+1)*sizeof(char));

  double a1 = a, b1 = b, l;
  int d;
  l = log2(b1-a1+1);
  d = (int)l;

  /* printf("a = %d, b = %d, d = %d\n", a, b, d); */
  int i;
  for (i = w-1; i>=0; --i) {
    // printf("%d", (a >> i) & 1);
    // putchar(48+((a >> i) & 1));
    bin[w-i-1] = 48 + ((a>>i)&1);
  }
  bin[w] = '\0';

  for (i = 0; i < d; ++i) { bin[w-i-1] = '*'; }
  //putchar('\n');
  /* printf("bin = %s\n",bin); */

  list_string* L = (list_string*)calloc(1, sizeof(list_string));
  L->head = L->last = NULL;
  list_string_insert(L, bin);
  free(bin);

  return L;
}
  
#endif
