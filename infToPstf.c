#include <stdlib.h>
#include <stdio.h>
#include "infToPstf.h"
#include "peepcc.h"
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define ADD     0x10    // Add a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define SUB     0x11    // Subtract a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define MUL     0x12    // Multiple a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define DIV     0x13    // Divide a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define MOD     0x14    // Compute the integer remainder when dividing a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)

// ACC-MEM Logical Instructions
#define AND     0x20    // Compute the binary AND of a specific memory location and the accumulator (leave the result in the accumulator)
#define ORR     0x21    // Compute the binary OR of a specific memory location and the accumulator (leave the result in the accumulator)
#define NOT     0x22    // Compute the logical NOT of the accumulator (leave the result in the accumulator)
#define XOR     0x23    // Compute the binary XOR of a specific memory location and the accumulator (leave the result in the accumulator)
#define LSR     0x24    // Compute the binary logical shift right (1-bit) of the accumulator (leave the result in the accumulator)
#define ASR     0x25    // Compute the binary arithmetic shift right (1-bit) of the accumulator (leave the result in the accumulator)
#define LSL     0x26    // Compute the binary logical shift left of the accumulator (leave the result in the accumulator)

// Branch Instructions
#define B       0X30    // Branch to a specific location in memory
#define BNEG    0X31    // Branch to a specific location in memory if the accumulator is negative
#define BPOS    0X32    // Branch to a specific location in memory if the accumulator is positive
#define BZRO    0X33    // Branch to a specific location in memory if the accumulator is zero

// ACC-MEM Load/Store Instructions
#define LOAD    0X40    // Load a word from a specific location in memory into the accumulator
#define STOR    0X41    // Store a word from the accumulator into a specific memory location

// I/O (Standard) Instructions
#define READ    0X50    // Read a word from the terminal into a specific location in memory
#define WRTE    0X51    // Write a word from a specific location in memory to the terminal

// HALT Instruction
#define HALT    0XFF    // Halt, i.e. the program has completed its task



/*
***********************************
* Function declarations
***********************************
*/
void convertToPostfix(char infix[], char postfix[]);

int isOperator(char c);

int precedence(char operator1, char operator2);

Stack_t* newStack();

bool push(Stack_t *s, int value);

int pop(Stack_t *s);

int stackTop(Stack_t *s);

int isEmpty(Stack_t *s);

void printStack(StackNodePtr topPtr);

int evaluatePostfixExpression(PeepCompiler *compiler, char *expr);

int calculate(int op1, int op2, char operator);

/*
*************************************
* Function Implementations
*************************************
*/
Stack_t* newStack()
{
	Stack_t *s = malloc(sizeof(Stack_t));
	if (s == NULL)
	{
		return NULL;
	}
	s->top = NULL;
	s->size = 0;
	return s;
}

bool push(Stack_t *s, int value)
{
	if (s == NULL)
	{
		return false;
	}
	StackNodePtr newTop = malloc(sizeof(StackNode));
	if (newTop == NULL)
	{
		return false;
	}
	newTop->data = value;
	newTop->nextPtr = s->top;
	s->top = newTop;
	s->size += 1;
	return true;
}

int pop(Stack_t *s)
{
	if (s == NULL)
	{
		return '\0';
	}
	StackNodePtr oldTop;
	oldTop = s->top;
	s->top = s->top->nextPtr;
	s->size -= 1;
	int val = oldTop->data;
	free(oldTop);
	return val;
}

int stackTop(Stack_t *s)
{
	if (s == NULL)
	{
		return '\0';
	}
	StackNodePtr t;
	t = s->top;
	return t->data;
}

int isEmpty(Stack_t* s)
{
	if (s->size == 0)
	{
		return 1;
	}
	return 0;
}

void printStack(StackNodePtr topPtr)
{
	StackNode *current_node = topPtr;
	printf("%c ", '[');
   	while ( current_node != NULL) {
        printf("%d ", current_node->data);
        current_node = current_node->nextPtr;
    }
	printf("%c", ']');
	printf("\n");
}

int isOperator(char c)
{
	if (c == '/' || c == '*' || c == '+' || c == '-')
	{
		return 1;
	}
	return 0;
}

int precedence(char operator1, char operator2)
{
	if (operator1 == '*')
	{
		if(operator2 == '*')
		{
			return 0;
		}
		return 1;
	}
	else if (operator1 == '/')
	{
		if(operator2 == '*')
		{
			return -1;
		}
		else if(operator2 == '/')
		{
			return 0;
		}
		return 1;
	}
	else if (operator1 == '+')
	{
		if(operator2 == '*' || operator2 == '/')
		{
			return -1;
		}
		else if(operator2 == '+')
		{
			return 0;
		}
		return 1;
	}
	else if(operator1 == '-')
	{
		if(operator2 == '*' || operator2 == '/' || operator2 == '+')
		{
			return -1;
		}
		else if(operator2 == '-')
		{
			return 0;
		}
	}
	else
	{
		printf("%s", "Not an operator");
		return 0;
	}
}

int calculate(int op1, int op2, char operator)
{
	if (operator == '+')
	{
		return op1 + op2;
	}
	else if(operator == '-')
	{
		return op1 - op2;
	}
	else if(operator == '/')
	{
		return op1 / op2;
	}
	else if(operator == '*')
	{
		return op1 * op2;
	}
	return 0;
}

void infixToPostfix(char infix_exp[], char postfix_exp[])
{
	int i, j;
	char item;
	char x;
	Stack_t *stack = newStack();
	push(stack, '(');
	strcat(infix_exp,")");

	i=0;
	j=0;
	item=infix_exp[i];

	while(item != '\0')
	{
		if(item == '(')
		{
			push(stack, item);
		}
		else if( isdigit(item) || isalpha(item))
		{
			postfix_exp[j] = item;              
			j++;
		}
		else if(isOperator(item) == 1)       
		{
			x=pop(stack);
			while(isOperator(x) == 1 && (precedence(x, item)== 1 || precedence(x, item) == 0))
			{
				postfix_exp[j] = x;                
				j++;
				x = pop(stack);                       
			}
			push(stack, x);

			push(stack, item);
		}
		else if(item == ')')       
		{
			x = pop(stack);                   /* pop and keep popping until */
			while(x != '(')                /* '(' encounterd */
			{
				postfix_exp[j] = x;
				j++;
				x = pop(stack);
			}
		}
		else
		{ /* if current symbol is neither operand not '(' nor ')' and nor
			operator */
			printf("\nInvalid infix Expression.\n");        /* the it is illegeal  symbol */
			getchar();
			exit(1);
		}
		i++;


		item = infix_exp[i]; /* go to next symbol of infix expression */
	} /* while loop ends here */

	postfix_exp[j] = '\0'; /* add sentinel else puts() fucntion */
	/* will print entire postfix[] array upto SIZE */

}

int evaluatePostfixExpression(struct pCompiler *compiler, char *expr)
{
    Stack_t *stack = newStack();
    
 
    // Check if stack was created successfully
    if (!stack) return -1;
 
    // Scan all characters one by one
    for (int i = 0; i < strlen(expr) - 1; i++)
    {
		
        // If the read character is an operand (number here), push it to the stack.
        if (isdigit(expr[i]))
        {
				char str[2];
				str[0] = expr[i];
				TableEntry entry = getSymbol(atoi(str), compiler->symTab);
				push(stack, entry.location);
				printf("%d \n", stackTop(stack));
				printStack(stack->top);
        }
		// If the read character is a variabe, then push its location to the stack.
		else if (isalpha(expr[i]))
		{
			if(lookUpSymbol(expr[i], compiler->symTab))
			{
				char str[2];
				str[0] = expr[i];
				TableEntry entry = getSymbol(expr[i], compiler->symTab);
				push(stack, entry.location);
			}
			else
			{
				fprintf(stderr, "%s", "Undeclared variable!\n");
			}
		} 
        // If the read character is an operator, pop two elements from stack apply the operator
        else
        {
			if (i < 2)
			{
				fprintf(stderr, "%s", "Invalid expression!\n");
			}
			// printStack(stack->top);
			// printMem(compiler->hml);
            int val2 = pop(stack);
            int val1 = pop(stack);
			
			int absVal1 = compiler->hml[val1];
			int absVal2 = compiler->hml[val2];
			
			
			short unsigned int result = 0;
			compiler->hml[compiler->inscount++] = LOAD * MEMSIZE + val1;
			
			

            switch (expr[i])
            {
            case '+': 
				compiler->hml[compiler->inscount++] = ADD * MEMSIZE + val2;
				result = absVal1 + absVal2; 
				break;
            case '-': 
				compiler->hml[compiler->inscount++] = SUB * MEMSIZE + val2;
				result = absVal1 - absVal2; 
				break;
            case '*': 
				compiler->hml[compiler->inscount++] = MUL * MEMSIZE + val2;
				result = absVal1 * absVal2;
				
				break;
            case '/': 
				compiler->hml[compiler->inscount++] = DIV * MEMSIZE + val2;
				result = absVal1 / absVal2;
				break;
			default:
				break;
            }
			
			compiler->hml[compiler->inscount++] = STOR * MEMSIZE + compiler->datacount;
			compiler->hml[compiler->datacount] = result;
			TableEntry entry;
			entry.location = compiler->datacount;
			entry.symbol = result;
			entry.type = 'C';
			compiler->symTab[compiler->symSize++] = entry;
			push(stack, compiler->datacount--);
        }
		
    }
	
	if(stack->size != 1)
	{
        fprintf(stderr, "%s", "Invalid expression!\n");
	}

    return pop(stack);
}


/*
* Do not delete commented code below, it might be need to text infix-to-postfix or evaluate-expression.
*/

// int main(int argc, char** argv)
// {
// 	// char in[12] = "(6+2)*5-8/4";
// 	// char post[10];
// 	// infixToPostfix(in, post);
// 	// for (int i = 0; i<10; ++i)
// 	// {
// 	// 	printf("%c ", post[i]);
// 	// }
// 	// printf("\n");
//     // //printf("Result: %d", evaluatePostfixExpression(post));
// 	// printf("\n");
// 	return 0;
// }
