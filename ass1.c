/* Extended precision integer calculator program
 *
 * Skeleton program written by Alistair Moffat, August 2013
 *
 * Modifications by Xichang Zhao , September 2013
 * 
 *Algorithms Are Fun!! 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define INTSIZE	    100 /* max number of digits per integer value */
#define LINELEN	    103 /* maximum length of any input line */
#define NVARS	     26	/* number of different variables */
#define INT_ONE      1  /* standard length of a number */
#define MAX_SINGLE_D 9  /* the largest single-digit number */
#define DECIMAL_BASE 10 /* base of the decimal numeral system */
#define C_INTERVAL   3  /* interval of each comma in a number */


#define CH_BLANK ' '    /* character blank */
#define CH_ZERO  '0'    /* character zero */
#define CH_A     'a'    /* character 'a', first variable name */
#define CH_ONE   '1'    /* character one */

#define ERROR	(-1)	/* error return value from some functions */
#define NULL_BYTE '\0'  /* null-byte that ends a string */

#define PRINT	'?'	/* the print operator */
#define ASSIGN	'='	/* the assignment operator */
#define PLUS	'+'	/* the addition operator */
#define TIMES   '*'     /* the multiplication operator */
#define POWER   '^'     /* the exponent operator */
#define ALLOPS  "?=+*^"   /* list of all valid operators */
#define SGNCHRS "+-"    /* the two sign characters */
#define NUMCHRS "0123456789"
			/* list of characters legal within numbers */


#define NEGATIVE '-'    /* negative sign */
#define POSITIVE '+'    /* positive sign */
#define COMMA    ','    /* comma character */


typedef struct {
	int numdig;
	char sign;
	char value[INTSIZE+1];
	
} longint_t;

			/* one extended-precision "variable" */

/****************************************************************/

/* function prototypes */

void print_prompt(void);
int read_line(char *line, int maxlen);
void process_line(longint_t vars[], char *line);
int to_varnum(char ident);
int get_second_value(longint_t vars[], char *rhsarg,
		longint_t *second_value);
int to_int(char digit);
char to_digit(int number);
void do_print(longint_t *var);
void do_assign(longint_t *var1, longint_t *var2);
void do_plus(longint_t *var1, longint_t *var2);
void zero_vars(longint_t vars[]);
void longintlize(longint_t *var,char *value,char sign,int numdig);
void plus_operation(longint_t big,longint_t small,longint_t *final);
void minus_operation(longint_t big,longint_t small,longint_t *final);
void zero_final(longint_t *final, int range);
void erase_zero(longint_t *final, int range, int *valid);
void comma(char str[], longint_t var);
void do_times(longint_t *var1, longint_t *var2);
void cmp_len(longint_t var1,longint_t var2,longint_t *big,longint_t *small);
void times_digit(longint_t big, int single, longint_t *product);
void plus_zero(longint_t *product, int rank_d);
void do_power(longint_t *var1, longint_t *var2);
void one_final(longint_t *final, int range);

/****************************************************************/

/* main program controls all the action
 */
int
main(int argc, char **argv) {
	char line[LINELEN+1];
	longint_t vars[NVARS];

	zero_vars(vars);
	print_prompt();
	while (read_line(line, LINELEN)) {
		if (strlen(line)>0) {
			/* non empty line, so process it */
			process_line(vars, line);
		}
		print_prompt();
	}

	/* all done, so pack up and go home */
	printf("\n");
	return 0;
}

/****************************************************************/

/* prints the prompt indicating ready for input
 */
void
print_prompt(void) {
	printf("> ");
}

/****************************************************************/

/* read a line of input into the array passed as argument
 * returns false if there is no input available
 * all whitespace characters are removed
 */
int
read_line(char *line, int maxlen) {
	int i=0, c;
	while (((c=getchar())!=EOF) && (c!='\n')) {
		if (i<maxlen && !isspace(c)) {
			line[i++] = c;
		}
	}
	if (i==maxlen) {
		printf("Input is oversized, a maximum of ");
		printf("%d digits are stored\n",INTSIZE);
	}
	line[i] = NULL_BYTE;
	return ((i>0) || (c!=EOF));
}

/****************************************************************/

/* process a command by parsing the input line into parts
 */
void
process_line(longint_t vars[], char *line) {
	int varnum, optype, status;
	longint_t second_value;

	/* determine the LHS variable, it
	 * must be first character in line
	 */
	varnum = to_varnum(line[0]);
	if (varnum==ERROR) {
		printf("Invalid LHS variable\n");
		return;
	}

	/* more testing for validity 
	 */
	if (strlen(line)<2) {
		printf("No operator supplied\n");
		return;
	}

	/* determine the operation to be performed, it
	 * must be second character in line
	 */
	optype = line[1];
	if (strchr(ALLOPS, optype) == NULL) {
		printf("Unknown operator\n");
		return;
	}

	/* determine the RHS argument (if one is required),
	 * it must start in third character of line
	 */
	if (optype != PRINT) {
		if (strlen(line)<3) {
			printf("No RHS supplied\n");
			return;
		}
		status = get_second_value(vars, line+2, &second_value);
		if (status==ERROR) {
			printf("RHS argument is invalid\n");
			return;
		}
	}

	/* finally, do the actual operation
	 */
	if (optype == PRINT) {
		do_print(vars+varnum);
	} else if (optype == ASSIGN) {
		do_assign(vars+varnum, &second_value);
	} else if (optype == PLUS) {
		do_plus(vars+varnum, &second_value);
	} else if (optype == TIMES) {
		do_times(vars+varnum, &second_value);
	} else if (optype == POWER) {
		do_power(vars+varnum, &second_value);
	}
	return;
}

/****************************************************************/

/* convert a character variable identifier to a variable number
 */
int
to_varnum(char ident) {
	int varnum;
	varnum = ident - CH_A;
	if (0<=varnum && varnum<NVARS) {
		return varnum;
	} else {
		return ERROR;
	}
}

/****************************************************************/

/* process the input line to extract the RHS argument, which
 * should start at the pointer that is passed
 */
int
get_second_value(longint_t vars[], char *rhsarg,
			longint_t *second_value) {
	char *p;
	int varnum2, i=0, numdig=0, x=0;
	char sign;
	char tmp[INTSIZE+1];
	
	while (*rhsarg) {
		if (*rhsarg!=COMMA) {
			tmp[i]=*rhsarg;
			i++;
		}
		rhsarg++;
	}
	tmp[i]=NULL_BYTE;
	rhsarg=tmp;
		
	if (strchr(NUMCHRS, *rhsarg) != NULL ||
				strchr(SGNCHRS, *rhsarg) != NULL) {
		/* first character is a digit or a sign, so RHS 
		 should be a number */
		 
	        if (*rhsarg==NEGATIVE) {
	        	sign=NEGATIVE;
	        }
	        else if (*rhsarg==POSITIVE) {
	        	sign=POSITIVE;
	        }
	        else {
	        	numdig=1;  /*initial number of digit is 1
	        	           if no sign is attached*/
	        	sign=POSITIVE;
	        }
		p = rhsarg+1;
		
		/*store the sign and number of digits 
		for later assignment in struct*/
		
		while (*p) {
			if (strchr(NUMCHRS, *p) == NULL) {
				/* nope, found an illegal character */
				return ERROR;
			}
			numdig+=1;
			p++;
		}
		while (numdig==101) {
			if (*(rhsarg+x)==NULL_BYTE) {
				*(rhsarg+x-1)=NULL_BYTE;
				numdig--;
				break;
			}
			x++;
		}
		/* A loop that makes sure a positive number is max 100 digits
		long. Due to the LINELEN allowance of 103 digits,when a number
		is typed in with a sign (NEGATIVE or POSITIVE),the max digits
		the read_line function can read is 100; if the number is
		without the sign (default to POSITIVE), max of 101 digits can
		be tolerated.*/
			
		longintlize(second_value,rhsarg,sign,numdig);
		
		return !ERROR;
	} 
	else {
		/* argument is not a number, so might be a variable */
		varnum2 = to_varnum(*rhsarg);
		if (varnum2==ERROR || strlen(rhsarg)!=1) {
			/* nope, not a variable either */
			return ERROR;
		}
		/* is a variable, so can use its value to assign to
		 * second_value
		 */
		*second_value = vars[varnum2];
		return !ERROR;
	}
	return ERROR;
}

/****************************************************************/

/* convert a character digit to the int equivalent, but null bytes
 * stay as zero integers
 */
int
to_int(char digit) {
	if (digit != NULL_BYTE) {
		return digit - CH_ZERO;
	} else {
		return 0;
	}
}

/****************************************************************/

/* and back again to a digit */
char
to_digit(int number) {
	return number + CH_ZERO;
}


/****************************************************************/

/* print out a longint value
 */
void
do_print(longint_t *var) {
	int i=0;
	char tmp[INTSIZE+INTSIZE/C_INTERVAL+1];
	
	if (var->sign==NEGATIVE) {
		while (*(var->value+i)) {
			if (*(var->value+i)!=CH_ZERO) {
				printf("%c",var->sign);
				break;
			}
			i++;
		}
	}
	/*if the number is negative and is not zero, print the '-' sign */
	
	comma(tmp,*var);
	
	printf("%s\n", tmp);

	return;
}

/****************************************************************/

/* update the indicated variable var1 by doing an assignment
 * using var2
 */
void
do_assign(longint_t *var1, longint_t *var2) {

	*var1 = *var2;
}

/****************************************************************/

/* update the indicated variable var1 by doing an addition*/
void
do_plus(longint_t *var1, longint_t *var2) {
	
	longint_t big,small,tmp,final;
	int difference,z,x,i;
	char mint,lint;
	
	cmp_len(*var1,*var2,&big,&small);
	
	difference = big.numdig - small.numdig;
	
	
	if (!difference) {
		for (z=0;z<big.numdig;z++) {
			mint=to_int(big.value[z]);
			lint=to_int(small.value[z]);
			if (mint<lint) {
				tmp=big;
				big=small;
				small=tmp;
				break;
			}
			else if (mint>lint) {
				break;
			}
		}
	}
	/*if two integers are equal length, determine which is larger*/
	
	else {
		tmp.sign=small.sign;
		tmp.numdig=small.numdig;
		for (x=0;x<difference;x++) {
			tmp.value[x]=CH_ZERO;
		}
		for (i=difference;i<big.numdig;i++) {
			tmp.value[i]=small.value[i-difference];
		}
		tmp.value[i]=NULL_BYTE;
		small=tmp;
	}
	/*add zeros to the front of the small number to match 
	with the number of digits of the big number, in this way, two
	numbers are easier to be operated.  BY THE WAY: 
	
	algorithms are fun......
	
	*/
	
	final.sign=big.sign;
	
	if (big.sign==small.sign) {
		plus_operation(big,small,&final);
	}
	else {
		minus_operation(big,small,&final);
	}
	
	/*sign of the final sum/difference is always determined by the
	bigger number. if both numbers are positive/negative, they are
	added together; otherwise, they are subtracted together.*/
	
        *var1=final; 
}
	


/****************************************************************/

/* make a number-string to a longint-t type struct */

void 
longintlize(longint_t *var,char *value,char sign,int numdig) {
	int i=0,valid=numdig-1;
	if (strchr(SGNCHRS, *value) != NULL) {
		value++;
	}
	/*if the first character is a sign, 
	ignore it, move the pointer by one*/
	while (*value) {
		var->value[i]=*value;
		i++;
		value++;
	}
	var->value[i]=NULL_BYTE;
	var->sign=sign;
	var->numdig=numdig;
	
	erase_zero(var,var->numdig,&valid);
	var->numdig-=valid;
}


/****************************************************************/


/* set the vars array to all zero values
 */
void
zero_vars(longint_t vars[]) {
	int i;
	
	for (i=0; i<NVARS; i++) {
		(vars+i)->value[0] = CH_ZERO;
		(vars+i)->value[1] = NULL_BYTE;
		(vars+i)->numdig = INT_ONE;
		(vars+i)->sign = POSITIVE;
	}
	return;
}



/****************************************************************/

/* add two numbers,assign the sum to final */

void
plus_operation(longint_t big,longint_t small,longint_t *final) {
	int i,sum=0,valid=big.numdig;
	/*valid is initialized to be the digits of big number, in case
	all the digits of the final sum are 0*/
	
	char *finv=final->value;
	
	zero_final(final,big.numdig+1);
	
	/*initialize all elements in final to zero, final is 1 character
	longer than big number, in case the last digit of the big number
	is greater than 10 when added with the small number, thus the digit 
	is carried forward by one*/
	
	for (i=big.numdig-1;i>=0;i--) {
		sum=to_int(big.value[i])+to_int(small.value[i]);
		sum+=to_int(*(finv+i+1));
		
		/*plus any value there is in final to sum,
		in case there is "1" carried forward from the addition of the
		previous digit*/
		
		if (sum>MAX_SINGLE_D) {
			*(finv+i+1)=to_digit(sum-DECIMAL_BASE);
			/* sum-10 to obtain a single digit number */
			
			*(finv+i)=CH_ONE;
			/*1 is carried forward to the next digit*/
		}
		else {
			*(finv+i+1)=to_digit(sum);
		}	
	}
	
	erase_zero(final,big.numdig+1,&valid);
	
	final->numdig=big.numdig+1-valid;
	
	assert(final->numdig <= INTSIZE);
	
}

/****************************************************************/

/* subtract two numbers, assign the difference to final */

void
minus_operation(longint_t big,longint_t small,longint_t *final) {
	int i,dif=0,valid=big.numdig-1,base=0;
	char *finv=final->value;
	
	zero_final(final,big.numdig);
	
	for (i=big.numdig-1;i>=0;i--) {
		base=to_int(*(finv+i));
		/*any value borrowed from the previous digit operation,
		either one or zero*/
		
		if (to_int(big.value[i])-base<to_int(small.value[i])) {
		        
			dif=to_int(big.value[i])+DECIMAL_BASE
		                            -to_int(small.value[i]);
		/* borrow a ten from the next digit to do subtraction */
		        
		        *(finv+i-1)=CH_ONE;
		}
		else {
			dif=to_int(big.value[i])-to_int(small.value[i]);
		}
		dif-=base;
		*(finv+i)=to_digit(dif);
	}
	
	erase_zero(final,big.numdig,&valid);
	
	final->numdig=big.numdig-valid;
	
	
}

/****************************************************************/

/* set the array to all 0 values */

void
zero_final(longint_t *final, int range) {
	int i;
	final->numdig=range;
	for (i=0;i<range;i++) {
		final->value[i]=CH_ZERO;
	}
	final->value[i]=NULL_BYTE;
}

/****************************************************************/

/*determine the position where the first non-zero element exists,
and erase the zeros before it */

void
erase_zero(longint_t *final, int range, int *valid) {
	int i,y;
	for (i=0;i<range;i++) {
		if (final->value[i]!=CH_ZERO) {
			*valid=i;
			break;
		}
	}
	
	for (y=0;y<range-*valid;y++) {
		final->value[y]=final->value[y+*valid];
	}
	
	final->value[y]=NULL_BYTE;

}

/****************************************************************/

/*insert commas between every three digits of a number*/

void
comma(char str[], longint_t var) {
	int i,x=var.numdig-1,count=0;
	if (var.numdig % C_INTERVAL) {
		i=var.numdig-1+(var.numdig)/C_INTERVAL;
	}
	else {
		i=var.numdig-1+(var.numdig)/C_INTERVAL-1;
	}
	/* determine the numbers of commas in total needed to be inserted,
	add the value with the numdig of the number to get a total length
	of the string the final number would be (with commas)*/
	
	*(str+i+1)=NULL_BYTE;
	while (x>=0) {
		if (count==0) {
			*(str+i)=var.value[x];
			count+=1;
			x-=1;
		}
		else if (count%C_INTERVAL) {
			*(str+i)=var.value[x];
			x-=1;
			count+=1;
		}
		else {
			*(str+i)=COMMA;
			count=0;
		}
		i--;
	}
	
}

/****************************************************************/

/* compare the size of two values by their digits numbers, assign them
to the corresponding variables */

void
cmp_len(longint_t var1,longint_t var2,longint_t *big,longint_t *small) {
	if ((var1.numdig)>(var2.numdig)) {
		*big=var1;
		*small=var2;
	}
	else {
		*big=var2;
		*small=var1;
	}
}

/****************************************************************/

/*Assigning two integers separately to big and small according
to their digits*/

void
do_times(longint_t *var1, longint_t *var2) {
	longint_t big,small,final,product;
	int i,rank_d=0;
	
	product.sign=POSITIVE;
	
	final.sign=POSITIVE;
	
	cmp_len(*var1,*var2,&big,&small);
	
	zero_final(&final,big.numdig+1);
	/*The range of final is assumed to be size of the big number plus 1,
	since a n digits 9 (9999 when n-4) times 9 is always n+1 digits long, 
	this is the maxmium size the final can be each time in iteration*/
	
	for (i=small.numdig-1;i>=0;i--) {
		times_digit(big,to_int(small.value[i]),&product);
		assert(product.numdig+rank_d <= INTSIZE);
		plus_zero(&product,rank_d);
		do_plus(&final,&product);
		rank_d++;
	}
	
	if (var1->sign==var2->sign) {
		final.sign=POSITIVE;
	}
	else {
		final.sign=NEGATIVE;
	}

	*var1=final;
}

/****************************************************************/

/* multiply a single digit number with a longint type number */

void
times_digit(longint_t big, int single, longint_t *product) {
	int i,tmp_pro,carry,valid;
	
	zero_final(product,big.numdig+1);
	product->sign=POSITIVE;
	valid=product->numdig-1;
	/* initialized to be this for the case if all digits in product
	are zero*/
	
	for (i=big.numdig-1;i>=0;i--) {
		carry=0;
		/* how many to be carried forward each time */
		tmp_pro=0;
		/* the product of two single digits multiplication */
		tmp_pro=to_int(big.value[i])*single;
		tmp_pro+=to_int(product->value[i+1]);
		/* plus any value that was carried forward 
		from last operation */
		
		while (tmp_pro>=DECIMAL_BASE) {
			tmp_pro-=DECIMAL_BASE;
			carry+=1;
		}
		/* get a single digit from tmp_pro */
		
		product->value[i+1]=to_digit(tmp_pro);
		product->value[i]=to_digit(carry);
	}
	
	erase_zero(product,big.numdig+1,&valid);
	
	product->numdig-=valid;
	
	assert(product->numdig <= INTSIZE);
}
			
		
	


/****************************************************************/

/* plus rank_d many zeros to the end of the number to form a proper
size product after each times_digit operation */

void
plus_zero(longint_t *product, int rank_d) {
	int i;
	for (i=0;i<rank_d;i++) {
		product->value[product->numdig]=CH_ZERO;
		product->value[product->numdig+1]=NULL_BYTE;
		product->numdig+=1;
	}
}

/****************************************************************/

/* set all values in the array to 1 */

void
one_final(longint_t *final, int range) {
	int i;
	final->numdig=range;
	for (i=0;i<range;i++) {
		final->value[i]=CH_ONE;
	}
	final->value[i]=NULL_BYTE;
}

/****************************************************************/

/* times number var1 var2 times,cannot tolerate negative exponents */

void
do_power(longint_t *var1, longint_t *var2) {
	longint_t i,final,one;
	i.sign=POSITIVE;
	final.sign=POSITIVE;
	one.sign=POSITIVE;
	
	
	zero_final(&i,INT_ONE);
	one_final(&final,INT_ONE);
	one_final(&one,INT_ONE);
	
	if (var2->sign==NEGATIVE && strcmp(var2->value,i.value)) {
			printf("power operation failed, only non-negative ");
			printf("exponent value should be supplied\n");
			exit(EXIT_FAILURE);
	}
	/* testing if the exponent is a negative number, and print out
	the error message and exit the program if so */
	
	while (strncmp(i.value,var2->value,var2->numdig)) {
	/* if i is not equal(smaller) to the range(var2,which is the
	exponent)*/
		do_times(&final,var1);
		do_plus(&i,&one);
	}
	
	/* This loop is proceeding based on 
	the operation of longint_t number */
	
	
	assert(final.numdig <= INTSIZE);
	*var1=final;
}
		
