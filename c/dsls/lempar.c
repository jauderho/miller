/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
#undef NDEBUG // xxx temp
%%
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/*
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands.
**
** Each symbol here is a terminal symbol in the grammar.
*/
%%
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into pparser
**    ParseARG_FETCH     Code to extract %extra_argument from pparser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
%%
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
%%
#define YY_SZ_ACTTAB (sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
**
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
%%
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
	int stateno;       /* The state-number */
	int major;         /* The major token value.  This is the code
										 ** number for the token at this stack level */
	YYMINORTYPE minor; /* The user-supplied minor token value.  This
										 ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure. */
struct _lemon_parser_t {
	int yyidx;                    /* Index of top element in stack */
	int yyerrcnt;                 /* Shifts left before out of the error */
	ParseARG_SDECL                /* A place to hold %extra_argument */
	yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
};
typedef struct _lemon_parser_t lemon_parser_t;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE   = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/*
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt) {
	yyTraceFILE = TraceFILE;
	yyTracePrompt = zTracePrompt;
	if (yyTraceFILE==0)
		yyTracePrompt = 0;
	else if (yyTracePrompt==0)
		yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *yyTokenName[] = {
%%
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
%%
};
#endif /* NDEBUG */

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *ParseTokenName(int tokenType) {
#ifndef NDEBUG
	if (tokenType>0 && tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0]))) {
		return yyTokenName[tokenType];
	} else {
		return "Unknown";
	}
#else
	return "";
#endif
}

/*
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and lemon_parser_free.
*/
void *lemon_parser_alloc(void *(*mallocProc)(size_t)){
	lemon_parser_t *pParser;
	pParser = (lemon_parser_t*)(*mallocProc) ((size_t)sizeof(lemon_parser_t)) ;
	if (pParser) {
		pParser->yyidx = -1;
	}
	return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
	switch (yymajor) {
		/* Here is inserted the actions which take place when a
		** terminal or non-terminal is destroyed.  This can happen
		** when the symbol is popped from the stack during a
		** reduce or during error processing or when a parser is
		** being destroyed before it is finished parsing.
		**
		** Note: during a reduce, the only symbols destroyed are those
		** which appear on the RHS of the rule, but which are not used
		** inside the C code.
		*/
%%
		default:  break;   /* If no destructor action specified: do nothing */
	}
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(lemon_parser_t *pParser){
	YYCODETYPE yymajor;
	yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

	if (pParser->yyidx<0)  return 0;
#ifndef NDEBUG
	if (yyTraceFILE && pParser->yyidx>=0) {
		fprintf(yyTraceFILE,"%sPopping %s\n", yyTracePrompt, yyTokenName[yytos->major]);
	}
#endif
	yymajor = yytos->major;
	yy_destructor (yymajor, &yytos->minor);
	pParser->yyidx--;
	return yymajor;
}

/*
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from lemon_parser_alloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void lemon_parser_free(
	void *pvparser,             /* The parser to be deleted */
	void (*freeProc)(void*))    /* Function used to reclaim memory */
{
	lemon_parser_t *pparser = (lemon_parser_t*)pvparser;
	if (pparser==0)  return;
	while (pparser->yyidx>=0)  yy_pop_parser_stack(pparser);
	(*freeProc)((void*)pparser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
	lemon_parser_t *pparser,        /* The parser */
	int iLookAhead)           /* The look-ahead token */
{
	int i;
	int stateno = pparser->yystack[pparser->yyidx].stateno;

	/* if (pparser->yyidx<0)  return YY_NO_ACTION;  */
	i = yy_shift_ofst[stateno];
	if (i==YY_SHIFT_USE_DFLT) {
		return yy_default[stateno];
	}
	if (iLookAhead==YYNOCODE) {
		return YY_NO_ACTION;
	}
	i += iLookAhead;
	if (i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead) {
#ifdef YYFALLBACK
		int iFallback;            /* Fallback token */
		if (iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
					 && (iFallback = yyFallback[iLookAhead])!=0) {
#ifndef NDEBUG
			if (yyTraceFILE) {
				fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
					 yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
			}
#endif
			return yy_find_shift_action(pparser, iFallback);
		}
#endif
		return yy_default[stateno];
	} else {
		return yy_action[i];
	}
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
	lemon_parser_t *pparser,        /* The parser */
	int iLookAhead)           /* The look-ahead token */
{
	int i;
	int stateno = pparser->yystack[pparser->yyidx].stateno;

	i = yy_reduce_ofst[stateno];
	if (i==YY_REDUCE_USE_DFLT) {
		return yy_default[stateno];
	}
	if (iLookAhead==YYNOCODE) {
		return YY_NO_ACTION;
	}
	i += iLookAhead;
	if (i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead) {
		return yy_default[stateno];
	} else {
		return yy_action[i];
	}
}

/*
** Perform a shift action.
*/
static void yy_shift(
	lemon_parser_t *pparser,          /* The parser to be shifted */
	int yyNewState,               /* The new state to shift in */
	int yyMajor,                  /* The major token to shift in */
	YYMINORTYPE *yypMinor)        /* Pointer ot the minor token to shift in */
{
	yyStackEntry *yytos;
	pparser->yyidx++;
	if (pparser->yyidx>=YYSTACKDEPTH) {
		 ParseARG_FETCH;
		 pparser->yyidx--;
#ifndef NDEBUG
		 if (yyTraceFILE) {
			 fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
		 }
#endif
		 while (pparser->yyidx>=0)  yy_pop_parser_stack(pparser);
		 /* Here code is inserted which will execute if the parser
		 ** stack every overflows */
%%
		 ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
		 return;
	}
	yytos = &pparser->yystack[pparser->yyidx];
	yytos->stateno = yyNewState;
	yytos->major = yyMajor;
	yytos->minor = *yypMinor;
#ifndef NDEBUG
	if (yyTraceFILE && pparser->yyidx>0) {
		int i;
		fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
		fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
		for(i=1; i<=pparser->yyidx; i++)
			fprintf(yyTraceFILE," %s", yyTokenName[pparser->yystack[i].major]);
		fprintf(yyTraceFILE,"\n");
	}
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static struct {
	YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
	unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
%%
};

static void yy_accept(lemon_parser_t*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
	lemon_parser_t *pparser,         /* The parser */
	int yyruleno)                /* Number of the rule by which to reduce */
{
	//printf("-- %s %d\n", __FILE__, __LINE__);
	int yygoto = -999 ;                     /* The next state */
	int yyact = -999;                      /* The next action */
	YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
	// xxx
	//printf("PRE MEMSET\n");
	//memset(&yygotominor, 0, sizeof(yygotominor));
	//printf("POST MEMSET\n");
	yyStackEntry *yymsp;            /* The top of the parser's stack */
	int yysize;                     /* Amount to pop the stack */
	// xxx remove x all
	//printf("-- %s %d\n", __FILE__, __LINE__);
	ParseARG_FETCH;
	//printf("-- %s %d\n", __FILE__, __LINE__);
	yymsp = &pparser->yystack[pparser->yyidx];
	//printf("-- %s %d\n", __FILE__, __LINE__);
#ifndef NDEBUG
	if (yyTraceFILE && yyruleno>=0
				&& yyruleno<sizeof(yyRuleName)/sizeof(yyRuleName[0])) {
		fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
			yyRuleName[yyruleno]);
	}
#endif /* NDEBUG */

	//printf("-- %s %d (%d)\n", __FILE__, __LINE__, yyruleno);
	switch (yyruleno) {
	//printf("-- %s %d\n", __FILE__, __LINE__);
	/* Beginning here are the reduction cases.  A typical example
	** follows:
	**   case 0:
	**  #line <lineno> <grammarfile>
	**     { ... }           // User supplied code
	**  #line <lineno> <thisfile>
	**     break;
	*/
%%
	//printf("-- %s %d\n", __FILE__, __LINE__);
	};
	//printf("-- %s %d\n", __FILE__, __LINE__);
	yygoto = yyRuleInfo[yyruleno].lhs;
	//printf("-- %s %d\n", __FILE__, __LINE__);
	yysize = yyRuleInfo[yyruleno].nrhs;
	//printf("-- %s %d\n", __FILE__, __LINE__);
	pparser->yyidx -= yysize;
	//printf("-- %s %d\n", __FILE__, __LINE__);
	yyact = yy_find_reduce_action(pparser,yygoto);
	if (yyact < YYNSTATE) {
	//printf("-- %s %d\n", __FILE__, __LINE__);
		yy_shift(pparser,yyact,yygoto,&yygotominor);
	} else if (yyact == YYNSTATE + YYNRULE + 1) {
	//printf("-- %s %d\n", __FILE__, __LINE__);
		yy_accept(pparser);
	}
	//printf("-- %s %d\n", __FILE__, __LINE__);
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(lemon_parser_t *pparser) {
	ParseARG_FETCH;
#ifndef NDEBUG
	if (yyTraceFILE) {
		fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
	}
#endif
	while (pparser->yyidx >= 0)
		yy_pop_parser_stack(pparser);
	/* Here code is inserted which will be executed whenever the
	** parser fails */
%%
	ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
	lemon_parser_t *pparser,           /* The parser */
	int yymajor,                   /* The major type of the error token */
	YYMINORTYPE yyminor            /* The minor type of the error token */
){
	ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
%%
	ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(lemon_parser_t *pparser) {
	ParseARG_FETCH;
#ifndef NDEBUG
	if (yyTraceFILE) {
		fprintf(yyTraceFILE,"%sAccept!\n", yyTracePrompt);
	}
#endif
	while (pparser->yyidx>=0)
		yy_pop_parser_stack(pparser);
	/* Here code is inserted which will be executed whenever the
	** parser accepts */
%%
	ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "lemon_parser_alloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure).
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/

int lemon_parser_parse_token(
	void *pvparser,              /* The parser */
	int yymajor,                 /* The major token code number */
	ParseTOKENTYPE yyminor       /* The value for the token */
	ParseARG_PDECL)              /* Optional %extra_argument parameter */
{
	YYMINORTYPE yyminorunion;
	int yyact;            /* The parser action. */
	int yyendofinput;     /* True if we are at the end of input */
	int yyerrorhit = 0;   /* True if yymajor has invoked an error */
	lemon_parser_t *pparser;  /* The parser */

	int parse_code = 1;

	/* (re)initialize the parser, if necessary */
	pparser = (lemon_parser_t*)pvparser;
	if (pparser->yyidx<0) {
		if (yymajor==0)  return 1;
		pparser->yyidx = 0;
		pparser->yyerrcnt = -1;
		pparser->yystack[0].stateno = 0;
		pparser->yystack[0].major = 0;
	}
	yyminorunion.yy0 = yyminor;
	yyendofinput = (yymajor==0);
	ParseARG_STORE;

#ifndef NDEBUG
	if (yyTraceFILE) {
		fprintf(yyTraceFILE,"%sInput %s\n", yyTracePrompt, yyTokenName[yymajor]);
	}
#endif

	do {
		yyact = yy_find_shift_action(pparser,yymajor);
		if (yyact<YYNSTATE) {
			yy_shift(pparser,yyact,yymajor,&yyminorunion);
			pparser->yyerrcnt--;
			if (yyendofinput && pparser->yyidx>=0) {
				yymajor = 0;
			} else {
				yymajor = YYNOCODE;
			}
		} else if (yyact < YYNSTATE + YYNRULE) {
			yy_reduce(pparser,yyact-YYNSTATE);
		} else if (yyact == YY_ERROR_ACTION) {
			int yymx;
#ifndef NDEBUG
			if (yyTraceFILE) {
				fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
			}
#endif
#ifdef YYERRORSYMBOL
			/* A syntax error has occurred.
			** The response to an error depends upon whether or not the
			** grammar defines an error token "ERROR".
			**
			** This is what we do if the grammar does define ERROR:
			**
			**  * Call the %syntax_error function.
			**
			**  * Begin popping the stack until we enter a state where
			**    it is legal to shift the error symbol, then shift
			**    the error symbol.
			**
			**  * Set the error count to three.
			**
			**  * Begin accepting and shifting new tokens.  No new error
			**    processing will occur until three tokens have been
			**    shifted successfully.
			**
			*/
			if (pparser->yyerrcnt<0) {
				yy_syntax_error(pparser, yymajor, yyminorunion);
				return 0;
			}
			yymx = pparser->yystack[pparser->yyidx].major;
			if (yymx==YYERRORSYMBOL || yyerrorhit) {
#ifndef NDEBUG
				if (yyTraceFILE) {
					fprintf(yyTraceFILE,"%sDiscard input token %s\n",
						 yyTracePrompt, yyTokenName[yymajor]);
				}
#endif
				yy_destructor(yymajor,&yyminorunion);
				yymajor = YYNOCODE;
			} else {
				 while(
					pparser->yyidx >= 0 &&
					yymx != YYERRORSYMBOL &&
					(yyact = yy_find_shift_action(pparser,YYERRORSYMBOL)) >= YYNSTATE
				){
					yy_pop_parser_stack(pparser);
				}
				if (pparser->yyidx < 0 || yymajor==0) {
					yy_destructor(yymajor,&yyminorunion);
					yy_parse_failed(pparser);
					parse_code = 0;
					yymajor = YYNOCODE;
				} else if (yymx!=YYERRORSYMBOL) {
					YYMINORTYPE u2;
					u2.YYERRSYMDT = 0;
					yy_shift(pparser,yyact,YYERRORSYMBOL,&u2);
				}
			}
			pparser->yyerrcnt = 3;
			yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
			/* This is what we do if the grammar does not define ERROR:
			**
			**  * Report an error message, and throw away the input token.
			**
			**  * If the input token is $, then fail the parse.
			**
			** As before, subsequent error messages are suppressed until
			** three input tokens have been successfully shifted.
			*/
			if (pparser->yyerrcnt<=0) {
				yy_syntax_error(pparser, yymajor, yyminorunion);
				return 0;
			}
			pparser->yyerrcnt = 3;
			yy_destructor(yymajor,&yyminorunion);
			if (yyendofinput) {
				yy_parse_failed(pparser);
				parse_code = 0;
			}
			yymajor = YYNOCODE;
#endif
		} else {
			yy_accept(pparser);
			yymajor = YYNOCODE;
		}
	} while (yymajor!=YYNOCODE && pparser->yyidx>=0) ;
	return parse_code;
}
