/* hash.h (static set) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __TANAKALAB_HASH_H__
#define __TANAKALAB_HASH_H__

#ifndef __TANAKALAB_BOOL_H__
#include <bool.h>
#endif

#ifndef __TANAKALAB_PAIR_H__
#include <pair.h>
#endif

typedef enum { occupied, empty, deleted } oed;

struct HASH_UNSIGNED_CELL {
  unsigned v;
  oed state;
};
typedef struct HASH_UNSIGNED_CELL hash_unsigned_cell;

struct HASH_UNSIGNED {
  unsigned B; /* budget size */
  hash_unsigned_cell* A;
};
typedef struct HASH_UNSIGNED hash_unsigned;

void hash_init(hash_unsigned*, unsigned);
bool member(unsigned, hash_unsigned*);
bool member_sub(unsigned, hash_unsigned_cell*, unsigned);
void hash_unsigned_delete(unsigned, hash_unsigned*);
void hash_unsigned_delete_sub(unsigned, hash_unsigned_cell*, unsigned);
void hash_unsigned_insert(unsigned, hash_unsigned*);
void hash_unsigned_insert_sub(unsigned, hash_unsigned_cell*, unsigned);
void hash_unsigned_clear(hash_unsigned*);
int h(unsigned, unsigned);

void hash_init(hash_unsigned* D, unsigned n) {
  D->A = (hash_unsigned_cell*)calloc(n,sizeof(hash_unsigned_cell));
  D->B = n;
  unsigned i;
  for (i = 0; i < n; ++i) { (D->A)[i].state = empty; }
}

bool member(unsigned x, hash_unsigned* D) {
  if (NULL == D->A) { return false; }
  return member_sub(x, D->A, D->B);
}

bool member_sub(unsigned x, hash_unsigned_cell* A, unsigned B) {
  int i, k;
  oed cstate;

  k = i = h(x, B);
  do {
    cstate = A[k].state;
    if (cstate == occupied) { if (x == A[k].v) { return true; } }
    k = (k+1) % B;
  } while (cstate != occupied && k != i);
  return false;
}

void hash_unsigned_delete(unsigned x, hash_unsigned* D) {
  if (NULL == D->A) { return; }
  hash_unsigned_delete_sub(x, D->A, D->B);
}

void hash_unsigned_delete_sub(unsigned x, hash_unsigned_cell* A, unsigned B) {
  int i, k;
  oed cstate;

  k = i = h(x, B);
  do {
    cstate = A[k].state;
    if (cstate == occupied) { if (x == A[k].v) { A[k].state = deleted; return; } }
    k = (k+1) % B;
  } while (cstate != empty && k != i);
}

void hash_unsigned_insert(unsigned x, hash_unsigned* D) {
  if (NULL == D->A) {
    printf("Error: Dictionary is not initialized.\n");
    exit(1);
  }
  hash_unsigned_insert_sub(x, D->A, D->B);
}

void hash_unsigned_insert_sub(unsigned x, hash_unsigned_cell* A, unsigned B) {

  int i, k, found = -1;
  oed cstate;

  k = i = h(x, B);
  do {
    cstate = A[k].state;
    if (cstate == empty || cstate == deleted) { if (found < 0) found = k; } /* there is an empty cell */
    else { if (A[k].v == x) return; } /* x is already existed */
    k = (k+1) % B;
  } while (cstate != empty && k != i);
  if (found < 0) {
    printf("Error: Dictionary is full.\n");
    exit(1);
  }
  A[found].v = x;
  A[found].state = occupied;
}

void hash_unsigned_clear(hash_unsigned* H) { free(H->A); }

int h(unsigned x, unsigned B) { /* hash function */
  return x%B;
}

#endif
