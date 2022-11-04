#include <stdlib.h>
#include <stdio.h>
#include "peepcc.h"
bool lookUpSymbol(int symbol, TableEntry *table);
TableEntry getSymbol(int symbol, TableEntry *table);

int evaluatePostfixExpression(PeepCompiler *compiler, char *expr);

struct stackNode {
    int data;
    struct stackNode *nextPtr;
};
typedef struct stackNode StackNode;
typedef StackNode *StackNodePtr;

typedef struct {
    StackNodePtr top;  /* Linked list of elements */
  /*
      You will need to add more fields to this structure
      to efficiently implement q_size and q_insert_tail
    */
	int size;
} Stack_t;
static const int MEMSIZE = 256;
