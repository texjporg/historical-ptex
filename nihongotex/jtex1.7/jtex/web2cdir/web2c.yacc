%token	array_tok begin_tok case_tok const_tok do_tok downto_tok else_tok
	end_tok file_tok for_tok function_tok goto_tok if_tok label_tok
	of_tok procedure_tok program_tok record_tok repeat_tok then_tok
	to_tok type_tok until_tok var_tok while_tok integer_tok real_tok
	others_tok r_num_tok i_num_tok string_literal_tok single_char_tok
	assign_tok two_dots_tok unknown_tok undef_id_tok var_id_tok
	proc_id_tok proc_param_tok fun_id_tok fun_param_tok const_id_tok
	type_id_tok hhb0_tok hhb1_tok field_id_tok define_tok field_tok
	break_tok

%nonassoc '=' not_eq_tok '<' '>' less_eq_tok great_eq_tok 
%left '+' '-' or_tok
%right unary_plus_tok unary_minus_tok 
%left '*' '/' div_tok mod_tok and_tok
%right not_tok

%{
#include "web2c.h"

#ifdef SYSV
#include <string.h>
#define	rindex	strrchr
#define	index	strchr
#else
#include <strings.h>
#endif	/* SYSV */
#define	symbol(x)	sym_table[x].id
static char function_return_type[50], for_stack[300], control_var[50],
            relation[3];
static int last_type = -1;
char my_routine[100];	/* Name of routine being parsed, if any */
static char array_bounds[80], array_offset[80];
static int func_id, uses_mem, uses_eqtb, lower_sym, upper_sym;
extern char conditional[], temp[], *std_header;
extern int tex, mf, strict_for;
extern FILE *coerce;
extern char coerce_name[];
int doing_statements = FALSE, var_formals = FALSE, ids_typed, id_list[20];
long labs();
%}

%start PROGRAM

%%

PROGRAM:	DEFS
		PROGRAM_HEAD
			{block_level++;
			 printf("#include \"%s\"\n", std_header);}
		LABEL_DEC_PART CONST_DEC_PART TYPE_DEC_PART
		VAR_DEC_PART
			{printf("\n#include \"%s\"\n", coerce_name);}
		P_F_DEC_PART
		BODY
			{YYACCEPT;}
		;

DEFS:		/* empty */
		| DEFS DEF
		;

DEF:			define_tok field_tok undef_id_tok ';' 
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = field_id_tok;
			}
		|	define_tok function_tok undef_id_tok ';'
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = fun_id_tok;
			}
		|	define_tok function_tok undef_id_tok '(' ')' ';'
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = fun_param_tok;
			}
		|	define_tok procedure_tok undef_id_tok ';'
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = proc_id_tok;
			}
		|	define_tok procedure_tok undef_id_tok '(' ')' ';'
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = proc_param_tok;
			}
		|	define_tok type_tok undef_id_tok ';'
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = type_id_tok;
			}
		|	define_tok type_tok undef_id_tok '=' 
			SUBRANGE_TYPE ';'
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = type_id_tok;
			    sym_table[ii].val = lower_bound;
			    sym_table[ii].val_sym = lower_sym;
			    sym_table[ii].upper = upper_bound;
			    sym_table[ii].upper_sym = upper_sym;
			}
		|	define_tok var_tok undef_id_tok ';'
			{
			    ii = add_to_table(last_id); 
			    sym_table[ii].typ = var_id_tok;
			}
		;
		
PROGRAM_HEAD:	program_tok undef_id_tok ';'
		;

BLOCK:
		      {	if (block_level > 0) my_output("{");
			indent++; block_level++;
		      }
		  LABEL_DEC_PART
		  CONST_DEC_PART TYPE_DEC_PART
			{if (block_level == 2) {
			    if (function_return_type[0]) {
			      my_output("register");
			      my_output(function_return_type);
			      my_output("Result;");
			    }
			    if (tex) {
			      (void) sprintf(safe_string, "%s_regmem",
						my_routine);
			      my_output(safe_string);
			      indent_line();
			    }
			 }
			}
		  VAR_DEC_PART
			{if (block_level == 1)
				puts("\n#include \"coerce.h\"");
			 doing_statements = TRUE;
			}
		  STAT_PART
			{if (block_level == 2) {
			    if (function_return_type[0]) {
			      my_output("return(Result)");
			      semicolon();
			      function_return_type[0] = '\0';
			     }
			     if (tex) {
			       if (uses_mem && uses_eqtb)
				(void) fprintf(coerce,
	"#define %s_regmem register memoryword *mem=zmem, *eqtb=zeqtb;\n",
				   my_routine);
			       else if (uses_mem)
				(void) fprintf(coerce,
			"#define %s_regmem register memoryword *mem=zmem;\n",
				   my_routine);
			       else if (uses_eqtb)
				(void) fprintf(coerce,
			"#define %s_regmem register memoryword *eqtb=zeqtb;\n",
				   my_routine);
			       else
				(void) fprintf(coerce,
				   "#define %s_regmem\n",
				   my_routine);
			    }
			    my_routine[0] = '\0';
			 }
			 indent--; block_level--;
			 my_output("}"); new_line();
			 doing_statements = FALSE;
			}
		;

LABEL_DEC_PART:		/* empty */
		|	label_tok 
				{ my_output("/*"); }
			LABEL_LIST ';'
				{ my_output("*/"); }
		;

LABEL_LIST:		LABEL
		|	LABEL_LIST ',' LABEL
		;

LABEL:			i_num_tok
				{ my_output(temp); }
		;

CONST_DEC_PART:		/* empty */
		|	const_tok CONST_DEC_LIST
				{ indent_line(); }
		;

CONST_DEC_LIST:		CONST_DEC
		|	CONST_DEC_LIST CONST_DEC
		;

CONST_DEC:
				{ new_line(); my_output("#define"); }
			undef_id_tok
				{ ii=add_to_table(last_id);
				  sym_table[ii].typ = const_id_tok;
				  my_output(last_id);
				}
			'=' CONSTANT ';'
				{ sym_table[ii].val=last_i_num;
				  new_line(); }
		;

CONSTANT:		i_num_tok
				{
				  (void) sscanf(temp, "%ld", &last_i_num);
				  if (labs((long) last_i_num) > 32767)
				      (void) strcat(temp, "L");
				  my_output(temp);
				  $$ = ex_32;
				}
		|	r_num_tok
				{ my_output(temp);
				  $$ = ex_real;
				}
		|	STRING
				{ $$ = 0; }
		|	CONSTANT_ID
				{ $$ = ex_32; }
		;

STRING:			string_literal_tok
				{ int i, j; char s[132];
	  			  j = 1;
				  s[0] = '"';
	  			  for (i=1; yytext[i-1]!=0; i++) {
	  			    if (yytext[i] == '\\' || yytext[i] == '"')
					s[j++]='\\';
	    			    else if (yytext[i] == '\'') i++;
	    			    s[j++] = yytext[i];
				  }
	    			  s[j-1] = '"';
				  s[j] = 0;
				  my_output(s);
				}
		|	single_char_tok
	  			{ char s[5];
				  s[0]='\'';
	    			  if (yytext[1] == '\\' || yytext[1] == '\'') {
	  				s[2] = yytext[1];
					s[1] = '\\';
					s[3] = '\'';
					s[4] = '\0';
				  }
	  			  else {
					s[1] = yytext[1];
					s[2]='\'';
					s[3]='\0';
				  }
	  			  my_output(s);
				}
		;

CONSTANT_ID:		const_id_tok
				{ my_output(last_id); }
		;

TYPE_DEC_PART:		/* empty */
		|	type_tok TYPE_DEF_LIST
		;

TYPE_DEF_LIST:		TYPE_DEF
		|	TYPE_DEF_LIST TYPE_DEF
		;

TYPE_DEF: 
				{ my_output("typedef"); }
			undef_id_tok 
				{ ii = add_to_table(last_id);
				  sym_table[ii].typ = type_id_tok;
				  (void) strcpy(safe_string, last_id);
				  last_type = ii;
				}
			'=' 
				{
				  array_bounds[0] = 0;
				  array_offset[0] = 0;
				}
			TYPE ';'
				{ if (*array_offset) {
			fprintf(stderr, "Cannot typedef arrays with offsets\n");
					exit(1);
				  }
				  my_output(safe_string);
				  my_output(array_bounds);
				  semicolon();
				  last_type = -1;
				}
		;

TYPE:			SIMPLE_TYPE
		|	STRUCTURED_TYPE
		;

SIMPLE_TYPE:		SUBRANGE_TYPE
				{ if (last_type >= 0) {
					sym_table[ii].val = lower_bound;
					sym_table[ii].val_sym = lower_sym;
			 		sym_table[ii].upper = upper_bound;
					sym_table[ii].upper_sym = upper_sym;
					ii= -1;
				  }
/* The following code says: if the bounds are known at translation time
 * on an integral type, then we select the smallest type of data which
 * can represent it in ANSI C.  We only use unsigned types when necessary.
 */
				  if (lower_sym == -1 && upper_sym == -1) {
				    if (lower_bound>= -127 && upper_bound<=127)
					my_output("schar");
				    else if (lower_bound >= 0
				      && upper_bound <= 255)
					my_output("unsigned char");
	  			    else if (lower_bound >= -32767
				      && upper_bound <= 32767)
					my_output("short");
	  			    else if (lower_bound >= 0
				      && upper_bound <= 65535)
					my_output("unsigned short");
	  			    else my_output("integer");
				  }
				  else my_output("integer");
				}
		|	TYPE_ID
		;

SUBRANGE_TYPE:		SUB_CONSTANT two_dots_tok SUB_CONSTANT
		;

POSSIBLE_PLUS:		/* empty */
		|	unary_plus_tok
		;

SUB_CONSTANT:		POSSIBLE_PLUS i_num_tok
				{lower_bound = upper_bound;
				 lower_sym = upper_sym;
				 (void) sscanf(temp, "%d", &upper_bound);
				 upper_sym = -1; /* no sym table entry */
				}
		|	const_id_tok
				{ lower_bound = upper_bound;
				  lower_sym = upper_sym;
				  upper_bound = sym_table[l_s].val;
				  upper_sym = l_s;
				}
		;

TYPE_ID:		type_id_tok
	{if (last_type >= 0) {
	    sym_table[last_type].var_not_needed = sym_table[l_s].var_not_needed;
	    sym_table[last_type].upper = sym_table[l_s].upper;
	    sym_table[last_type].upper_sym = sym_table[l_s].upper_sym;
	    sym_table[last_type].val = sym_table[l_s].val;
	    sym_table[last_type].val_sym = sym_table[l_s].val_sym;
	 }
	 my_output(last_id); }
		;

STRUCTURED_TYPE:	ARRAY_TYPE
				{if (last_type >= 0)
				    sym_table[last_type].var_not_needed = TRUE;}
		|	RECORD_TYPE
		|	FILE_TYPE
				{if (last_type >= 0)
				    sym_table[last_type].var_not_needed = TRUE;}
		|	POINTER_TYPE
				{if (last_type >= 0)
				    sym_table[last_type].var_not_needed = TRUE;}
		;

POINTER_TYPE:		'^' type_id_tok
	{if (last_type >= 0) {
	    sym_table[last_type].var_not_needed = sym_table[l_s].var_not_needed;
	    sym_table[last_type].upper = sym_table[l_s].upper;
	    sym_table[last_type].upper_sym = sym_table[l_s].upper_sym;
	    sym_table[last_type].val = sym_table[l_s].val;
	    sym_table[last_type].val_sym = sym_table[l_s].val_sym;
	 }
	 my_output(last_id); my_output("*"); }
		;

ARRAY_TYPE:		array_tok '[' INDEX_TYPE ']' of_tok COMPONENT_TYPE
		|	array_tok '[' INDEX_TYPE ',' INDEX_TYPE ']' of_tok
			COMPONENT_TYPE
		;

INDEX_TYPE:		SUBRANGE_TYPE
				{ compute_array_bounds(); }
		|	type_id_tok
				{ int i, j;
				  lower_bound = sym_table[l_s].val;
				  lower_sym = sym_table[l_s].val_sym;
				  upper_bound = sym_table[l_s].upper;
				  upper_sym = sym_table[l_s].upper_sym;
				  compute_array_bounds();
				}
		;

COMPONENT_TYPE:		TYPE
		;


RECORD_TYPE:		record_tok
				{ my_output("struct"); my_output("{"); indent++;}
			FIELD_LIST end_tok
				{ indent--; my_output("}"); semicolon(); }
		;

FIELD_LIST:		RECORD_SECTION
		|	FIELD_LIST ';' RECORD_SECTION
		;

RECORD_SECTION:
				{ field_list[0] = 0; }
		  	FIELD_ID_LIST ':'
				{
				  /*array_bounds[0] = 0;
				  array_offset[0] = 0;*/
				}
			TYPE
				{ int i=0, j; char temp[80];
				  while(field_list[i++] == '!') {
					j = 0;
					while (field_list[i])
					    temp[j++] = field_list[i++];
					i++;
					if (field_list[i] == '!')
						temp[j++] = ',';
					temp[j] = 0;
					my_output(temp);
				  }
				  semicolon();
				}
		|	/* empty */
		;

FIELD_ID_LIST:		FIELD_ID
		|	FIELD_ID_LIST ',' FIELD_ID
		;

FIELD_ID:		undef_id_tok
				{ int i=0, j=0;
				  while (field_list[i] == '!')
					while(field_list[i++]);
				  ii = add_to_table(last_id);
				  sym_table[ii].typ = field_id_tok;
				  field_list[i++] = '!';
				  while (last_id[j])
					field_list[i++] = last_id[j++];
				  field_list[i++] = 0;
				  field_list[i++] = 0;
				}
		|	field_id_tok
				{ int i=0, j=0;
				  while (field_list[i] == '!')
					while(field_list[i++]);
				  field_list[i++] = '!';
				  while (last_id[j])
					field_list[i++] = last_id[j++];
				  field_list[i++] = 0;
				  field_list[i++] = 0;
				}
		;

FILE_TYPE:		file_tok of_tok 
				{ my_output("file_ptr /* of "); }
			TYPE
				{my_output("*/");}
		;

VAR_DEC_PART:		/* empty */
		|	var_tok VAR_DEC_LIST
		;

VAR_DEC_LIST:		VAR_DEC
		|	VAR_DEC_LIST VAR_DEC
		;

VAR_DEC: 
				{ var_list[0] = 0;
				  array_bounds[0] = 0;
				  array_offset[0] = 0;
				  var_formals = FALSE;
				  ids_typed = 0;
				}
			VAR_ID_DEC_LIST ':'
				{
				  array_bounds[0] = 0;	
				  array_offset[0] = 0;
				}
			TYPE ';'
				{fixup_var_list();}
		;

VAR_ID_DEC_LIST:	VAR_ID
		|	VAR_ID_DEC_LIST ',' VAR_ID
		;

VAR_ID:			undef_id_tok
				{ int i=0, j=0;
				  ii = add_to_table(last_id);
				  sym_table[ii].typ = var_id_tok;
				  sym_table[ii].var_formal = var_formals;
				  id_list[ids_typed++] = ii;
	  			  while (var_list[i] == '!')
					while(var_list[i++]);
				  var_list[i++] = '!';
				  while (last_id[j])
					var_list[i++] = last_id[j++];
	  			  var_list[i++] = 0;
				  var_list[i++] = 0;
				}
		|	var_id_tok
				{ int i=0, j=0;
				  ii = add_to_table(last_id);
	  			  sym_table[ii].typ = var_id_tok;
				  sym_table[ii].var_formal = var_formals;
				  id_list[ids_typed++] = ii;
	  			  while (var_list[i] == '!')
					while (var_list[i++]);
	  			  var_list[i++] = '!';
				  while (last_id[j])
					var_list[i++] = last_id[j++];
	  			  var_list[i++] = 0;
				  var_list[i++] = 0;
				}
		|	field_id_tok
				{ int i=0, j=0;
				  ii = add_to_table(last_id);
	  			  sym_table[ii].typ = var_id_tok;
	  			  while (var_list[i] == '!')
					while(var_list[i++]);
	  			  var_list[i++] = '!';
				  while (last_id[j])
					var_list[i++] = last_id[j++];
				  var_list[i++] = 0;
				  var_list[i++] = 0;
				}
		;

BODY:		/* empty */
		| 	begin_tok 
			    {my_output("main_body() {"); indent++; new_line();}
			STAT_LIST end_tok '.'
				{ indent--; my_output("}"); new_line(); }
		;

P_F_DEC_PART:		P_F_DEC
		|	P_F_DEC_PART P_F_DEC
		;

P_F_DEC:		PROCEDURE_DEC ';'
				{ indent_line(); remove_locals(); }
		|	FUNCTION_DEC ';'
				{ indent_line(); remove_locals(); }
		;

PROCEDURE_DEC:		PROCEDURE_HEAD BLOCK
		;

PROCEDURE_HEAD:		procedure_tok undef_id_tok 
				{ ii = add_to_table(last_id);
				  (void) fprintf(stderr, "%3d Procedure %s\n",
					pf_count++, last_id);
				  sym_table[ii].typ = proc_id_tok;
				  (void) strcpy(my_routine, last_id);
				  uses_eqtb = uses_mem = 0;
				}
			PARAM ';'
		|	procedure_tok proc_id_tok
				{ ii = l_s; 
				  (void) fprintf(stderr, "%3d Procedure %s\n",
					pf_count++, last_id);
				}
			PARAM ';'
		;

PARAM:			/* empty */
				{ (void) strcpy(z_id, last_id);
				  my_output(last_id); my_output("()");
				  mark();
				}
		|	'('
				{ (void) sprintf(z_id, "z%s", last_id);
				  my_output(z_id);
				  (void) fprintf(coerce, "#define %s(",
							 last_id);
				  field_list[0] = 0;
				  my_output("(");
				  sym_table[ii].typ++;
				  f_l = 0;
				  mark();
				  first_param = 0;
				}
			FORM_PAR_SEC_L ')'
				{ int i=0, jj;
				  field_list[f_l] = 0;
				  my_output(")");
				  indent_line();
				  (void) fprintf(coerce, ") %s(", z_id);
				  first_param = 0;
				  while (field_list[i++] == '.') {
					my_output(field_list+i);
					jj = i;
					var_formals =
					    (index(field_list+jj, '*') != 0);
					while (field_list[i++]);
					while(field_list[i] == '!') {
					    my_output(field_list+(++i)); 
					    if (first_param)
						(void) putc(',', coerce);
					    if (!var_formals)
					      (void) fprintf(coerce, "(%s)(%c)",
						field_list+jj,
						first_param++ +'a');
					    else
					      (void) fprintf(coerce, "(%s) &(%c)",
						field_list+jj,
						first_param++ +'a');
					    while (field_list[i++]); 
					    if (field_list[i] == '!') {
						my_output(",");
						if (var_formals)
						    (void) putchar('*');
					    }
					}
					semicolon();
				    }
				    (void) fprintf(coerce, ")\n");
				}
		;

FORM_PAR_SEC_L:		FORM_PAR_SEC
		|	FORM_PAR_SEC_L ';' FORM_PAR_SEC
		;

FORM_PAR_SEC1: 			{ var_list[0] = 0; ids_typed = 0;}
			VAR_ID_DEC_LIST ':' type_id_tok
				{ int i=0, id;
				  field_list[f_l++] = '.'; 
				  while (last_id[i])
					field_list[f_l++] = last_id[i++];
				  if (var_formals) {
					if (sym_table[l_s].var_not_needed) {
					    for (id=0; id<ids_typed; id++)
					      sym_table[id_list[id]].var_formal = FALSE;
					}
					else {
					    field_list[f_l++] = ' ';
					    field_list[f_l++] = '*';
					}
				  }
				  field_list[f_l++] = 0;
				  i = 0;
				  while (var_list[i] == '!') {
					if (first_param) {
					    my_output(",");
					    (void) putc(',', coerce);
					}
					(void) fprintf(coerce, "%c",
							first_param++ +'a');
					my_output(var_list+i+1);
					while (var_list[i])
					    field_list[f_l++] = var_list[i++];
					field_list[f_l++] = 0;
					i++;
				  }
				}
		;

FORM_PAR_SEC:		{var_formals = 0;} FORM_PAR_SEC1
		|	var_tok {var_formals = 1;} FORM_PAR_SEC1
		;

FUNCTION_DEC:		FUNCTION_HEAD BLOCK
		;

FUNCTION_HEAD:		function_tok undef_id_tok 
				{ push();
				  ii = add_to_table(last_id);
				  func_id = ii;
				  (void) fprintf(stderr, "%3d Function %s\n",
					pf_count++, last_id);
	  			  sym_table[ii].typ = fun_id_tok;
				  (void) strcpy(my_routine, last_id);
				  uses_eqtb = uses_mem = 0;
				}
			PARAM ':'
				{ normal();
				  array_bounds[0] = 0;
				  array_offset[0] = 0;
				}
			RESULT_TYPE
				{(void) strcpy(function_return_type, yytext);
				 (void) fprintf(coerce, "%s %s();\n",
					function_return_type,
					z_id);
				 pop();
				}
			';'
		;

RESULT_TYPE:		TYPE
		;

STAT_PART:		begin_tok STAT_LIST end_tok
		;

COMPOUND_STAT:		begin_tok 
				{my_output("{"); indent++; new_line();}
			STAT_LIST end_tok
				{ indent--; my_output("}"); new_line(); }
		;

STAT_LIST:		STATEMENT
		|	STAT_LIST ';' STATEMENT
		;

STATEMENT:		UNLAB_STAT
		|	S_LABEL ':'
			UNLAB_STAT
		;

S_LABEL:		i_num_tok
				{if (!doreturn(temp)) {
				    (void) sprintf(safe_string, "lab%s:",
					temp);
				    my_output(safe_string);
				 }
				}
		;

UNLAB_STAT:		SIMPLE_STAT
				{ semicolon(); }
		|	STRUCT_STAT
				{ semicolon(); }
		;

SIMPLE_STAT:		ASSIGN_STAT
		|	PROC_STAT
		|	GO_TO_STAT
		|	EMPTY_STAT
		|	break_tok
				{my_output("break");}
		;

ASSIGN_STAT:		VARIABLE assign_tok 
				{ my_output("="); }
			EXPRESS
		|	FUNC_ID_AS assign_tok
				{ my_output("Result ="); }
			EXPRESS
		;

VARIABLE:		var_id_tok
				{ if (strcmp(last_id, "mem") == 0)
					uses_mem = 1;
				  else if (strcmp(last_id, "eqtb") == 0)
					uses_eqtb = 1;
				  if (sym_table[l_s].var_formal)
					(void) putchar('*');
				  my_output(last_id);
				  $$ = ex_32;
				}
			VAR_DESIG_LIST
		|	var_id_tok
				{ if (sym_table[l_s].var_formal)
					(void) putchar('*');
				  my_output(last_id); $$ = ex_32; }
		;

FUNC_ID_AS:		fun_id_tok
				{ $$ = ex_32; }
		|	fun_param_tok
				{ $$ = ex_32; }
		;

VAR_DESIG_LIST:		VAR_DESIG
		|	VAR_DESIG_LIST VAR_DESIG
		;

VAR_DESIG:		'['
				{ my_output("["); }
			EXPRESS VAR_DESIG1
				{ my_output("]"); }
		|	'.' field_id_tok
				{if (tex || mf) {
				   if (strcmp(last_id, "int")==0)
					my_output(".cint");
				   else if (strcmp(last_id, "lh")==0)
					my_output(".v.LH");
				   else if (strcmp(last_id, "rh")==0)
					my_output(".v.RH");
				   else {
				     (void)sprintf(safe_string, ".%s", last_id);
				     my_output(safe_string);
				   }
				 }
				 else {
				    (void) sprintf(safe_string, ".%s", last_id);
				    my_output(safe_string);
				 }
				}
		|	'.' hhb0_tok
				{ my_output(".hh.b0");}
		|	'.' hhb1_tok
				{ my_output(".hh.b1");}
		;

VAR_DESIG1:		']'
		|	','
				{ my_output("][");}
			EXPRESS	']'
		;
		
EXPRESS:		UNARY_OP EXPRESS	%prec '*'
				{ $$ = $2; }
		|	EXPRESS '+' {my_output("+");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS '-' {my_output("-");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS '*' {my_output("*");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS div_tok {my_output("/");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS '=' {my_output("==");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS not_eq_tok {my_output("!=");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS mod_tok {my_output("%");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS '<' {my_output("<");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS '>' {my_output(">");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS less_eq_tok {my_output("<=");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS great_eq_tok {my_output(">=");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS and_tok {my_output("&&");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS or_tok {my_output("||");} EXPRESS
				{$$ = max($1, $4);}
		|	EXPRESS '/'
				{ my_output("/ ((double)"); }
			EXPRESS
				{$$ = max($1, $4); my_output(")"); }
		|	FACTOR
				{ $$ = $1; }
		;

UNARY_OP:		unary_plus_tok
		|	unary_minus_tok
			{ my_output("- (integer)"); }
		|	not_tok
				{ my_output("!"); }
		;

FACTOR:			'('
				{ my_output("("); }
			EXPRESS ')'
				{ my_output(")"); $$ = $1; }
		|	VARIABLE
		|	CONSTANT
		|	fun_id_tok
				{ my_output(last_id); my_output("()"); }
		|	fun_param_tok
				{ my_output(last_id); }
			PARAM_LIST
		;

PARAM_LIST:		'('
				{ my_output("("); }
			ACTUAL_PARAM_L ')'
				{ my_output(")"); }
		;

ACTUAL_PARAM_L:		ACTUAL_PARAM
		|	ACTUAL_PARAM_L ',' 
				{ my_output(","); }
			ACTUAL_PARAM
		;

ACTUAL_PARAM:
			EXPRESS WIDTH_FIELD
		; 

WIDTH_FIELD:		':' i_num_tok
		|	/* empty */
		;

PROC_STAT:		proc_id_tok
				{ my_output(last_id); my_output("()"); }
		|	undef_id_tok
				{ my_output(last_id);
				  ii = add_to_table(last_id);
	  			  sym_table[ii].typ = proc_id_tok;
				  my_output("()");
				}
		|	proc_param_tok
				{ my_output(last_id); }
			PARAM_LIST
		;

GO_TO_STAT:		goto_tok i_num_tok
				{if (doreturn(temp)) {
				    if (function_return_type[0])
					my_output("return(Result)");
				    else
					my_output("return");
				 } else {
				     (void) sprintf(safe_string, "goto lab%s",
					temp);
				     my_output(safe_string);
				 }
				}
		;

EMPTY_STAT:		/* empty */
		;

STRUCT_STAT:		COMPOUND_STAT
		|	CONDIT_STAT
		|	REPETIT_STAT
		;

CONDIT_STAT:		IF_STATEMENT
		|	CASE_STATEMENT
		;

IF_STATEMENT:		BEGIN_IF_STAT
		|	BEGIN_IF_STAT ELSE_STAT
		;

BEGIN_IF_STAT:		if_tok 
				{ my_output("if"); my_output("("); }
			EXPRESS 
				{ my_output(")"); new_line();}
			then_tok STATEMENT
		;

ELSE_STAT:		else_tok
				{ my_output("else"); }
			STATEMENT
		;

CASE_STATEMENT:		case_tok 
				{ my_output("switch"); my_output("("); }
			EXPRESS of_tok 
				{ my_output(")"); indent_line();
				  my_output("{"); indent++;
				}
			CASE_EL_LIST END_CASE
				{ indent--; my_output("}"); new_line(); }
		;

CASE_EL_LIST:		CASE_ELEMENT 
		|	CASE_EL_LIST ';' CASE_ELEMENT
		;

CASE_ELEMENT:		CASE_LAB_LIST ':' UNLAB_STAT
				{ my_output("break"); semicolon(); }
		;

CASE_LAB_LIST:		CASE_LAB
		|	CASE_LAB_LIST ',' CASE_LAB
		;

CASE_LAB:		i_num_tok
				{ my_output("case"); 
				  my_output(temp);
				  my_output(":"); indent_line();
				}
		|	others_tok
				{ my_output("default:"); indent_line(); }
		;

END_CASE:		end_tok
		|	';' end_tok
		;

REPETIT_STAT:		WHILE_STATEMENT
		|	REP_STATEMENT
		|	FOR_STATEMENT
		;

WHILE_STATEMENT:	while_tok 
				{ my_output("while");
				  my_output("(");
				}
			EXPRESS 
				{ my_output(")"); }
			do_tok STATEMENT
		;

REP_STATEMENT:		repeat_tok 
				{ my_output("do"); my_output("{"); indent++; }
			STAT_LIST until_tok 
				{ indent--; my_output("}"); 
				  my_output("while"); my_output("( ! (");
				}
			EXPRESS
				{ my_output(") )"); }
		;

FOR_STATEMENT:		for_tok 
				{
				  my_output("{");
				  my_output("register");
				  my_output("integer");
				  if (strict_for)
					my_output("for_begin,");
				  my_output("for_end;");
				 }
			CONTROL_VAR assign_tok 
				{ if (strict_for)
					my_output("for_begin");
				  else
					my_output(control_var);
				  my_output("="); }
			FOR_LIST do_tok
				{ my_output("; if (");
				  if (strict_for) my_output("for_begin");
				  else my_output(control_var);
				  my_output(relation);
				  my_output("for_end)");
				  if (strict_for) {
					my_output("{");
					my_output(control_var);
					my_output("=");
					my_output("for_begin");
					semicolon();
				  }
				  my_output("do"); 
				  indent++; 
				  new_line();}
			STATEMENT
				{
				  char *top = rindex(for_stack, '#');
				  indent--; new_line();
				  my_output("while"); 
				  my_output("("); 
				  my_output(top+1); 
				  my_output(")"); 
				  my_output(";");
				  my_output("}");
				  if (strict_for)
					my_output("}");
				  *top=0;
				  new_line();
				}
		;

CONTROL_VAR:		var_id_tok
				{(void) strcpy(control_var, last_id); }
		;

FOR_LIST:		EXPRESS 
				{ my_output(";"); }
			to_tok 
				{ 
				  (void) strcpy(relation, "<=");
				  my_output("for_end");
				  my_output("="); }
			EXPRESS
				{ 
				  (void) sprintf(for_stack + strlen(for_stack),
				    "#%s++ < for_end", control_var);
				}
		|	EXPRESS 
				{ my_output(";"); }
			downto_tok 
				{
				  (void) strcpy(relation, ">=");
				  my_output("for_end");
				  my_output("="); }
			EXPRESS
				{ 
				  (void) sprintf(for_stack + strlen(for_stack),
				    "#%s-- > for_end", control_var);
				}
		;
%%
compute_array_bounds()
{
    int lb;
    char tmp[200];

    if (lower_sym == -1) {	/* lower is a constant */
	lb = lower_bound - 1;
	if (lb==0) lb = -1;	/* Treat lower_bound==1 as if lower_bound==0 */
	if (upper_sym == -1)	/* both constants */
	    (void) sprintf(tmp, "[%d]", upper_bound - lb);
	else {			/* upper a symbol, lower constant */
	    if (lb < 0)
		(void) sprintf(tmp, "[%s + %d]",
				symbol(upper_sym), (-lb));
	    else
		(void) sprintf(tmp, "[%s - %d]",
				symbol(upper_sym), lb);
	}
	if (lower_bound < 0 || lower_bound > 1) {
	    if (*array_bounds) {
		fprintf(stderr, "Cannot handle offset in second dimension\n");
		exit(1);
	    }
	    if (lower_bound < 0) {
		(void) sprintf(array_offset, "+%d", -lower_bound);
	    } else {
		(void) sprintf(array_offset, "-%d", lower_bound);
	    }
	}
	(void) strcat(array_bounds, tmp);
    }
    else {			/* lower is a symbol */
	if (upper_sym != -1)	/* both are symbols */
	    (void) sprintf(tmp, "[%s - %s + 1]", symbol(upper_sym),
		symbol(lower_sym));
	else {			/* upper constant, lower symbol */
	    (void) sprintf(tmp, "[%d - %s]", upper_bound + 1,
		symbol(lower_sym));
	}
	if (*array_bounds) {
	    fprintf(stderr, "Cannot handle symbolic offset in second dimension\n");
	    exit(1);
	}
	(void) sprintf(array_offset, "- (int)(%s)", symbol(lower_sym));
	(void) strcat(array_bounds, tmp);
    }
}

fixup_var_list()
{
    int i, j;
    char output_string[100], real_symbol[100];

    for (i=0; var_list[i++] == '!'; ) {
	for (j=0; real_symbol[j++] = var_list[i++];);
	if (*array_offset) {
	    (void) fprintf(std, "\n#define %s (%s %s)\n  ",
	        real_symbol, next_temp, array_offset);
	    (void) strcpy(real_symbol, next_temp);
	    find_next_temp();
	}
	(void) sprintf(output_string, "%s%s%c",
	    real_symbol, array_bounds, (var_list[i]=='!' ? ',' : ' '));
	my_output(output_string);
    }
    semicolon();
}


/*
 * If we're not processing TeX, we return 0 (false).  Otherwise,
 * return 1 if the label is "10" and we're not in one of four TeX
 * routines where the line labeled "10" isn't the end of the routine.
 * Otherwise, return 0.
 */
doreturn(label)
char *label;
{
    if (!tex) return(0);
    if (strcmp(label, "10")) return(0);
    if (strcmp(my_routine, "macrocall") == 0) return(0);
    if (strcmp(my_routine, "hpack") == 0) return(0);
    if (strcmp(my_routine, "vpackage") == 0) return(0);
    if (strcmp(my_routine, "trybreak") == 0) return(0);
    return(1);
}


/* Return the absolute value of a long */
long labs(x)
long x;
{
    if (x < 0L) return(-x);
    return(x);
}
