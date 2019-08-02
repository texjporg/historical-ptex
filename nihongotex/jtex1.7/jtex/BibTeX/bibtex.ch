% Change file for BibTeX for Berkeley UNIX, by H. Trickey
% History:
% 5/28/84	Initial implementation, version 0.41 of BibTeX
% 7/1/84	Version 0.41a of BibTeX.
% 12/17/84	Version 0.97c of BibTeX.
% 2/12/85	Version 0.98c of BibTeX.
% 2/25/85	Newer Version 0.98c of BibTeX
% 3/25/85	Version 0.98f of BibTeX
% 5/23/85	Version 0.98i of BibTeX
% 2/11/88       Version 0.99b of BibTeX
% 4/ 4/88	Version 0.99c; converted for use with web2c (ETM).
% 8/ 3/88	Separated BIBPATH from TEXINPUTS in input file searching
%                         ^^^^^^^ -> BIBINPUTS !!! in bibext.c !!!!!
%                                    
%  5/ 2/89 	Version 0.20 of JBiBTeX by Shouichi Matsui
% 12/25/90	Version 0.30 of JBibTeX by Shouichi Matsui
%  1/ 1/91      last update for JBibTeX 0.30 by Shouichi Matsui
% 
%
@x only print chnages
\def\title{\BibTeX\ }
@y
\let\maybe=\iffalse
\def\title{J\BibTeX\ Changes for C Version \BibTeX\ }
@z

@x
 \def\titlepage{F}
 \centerline{\:\titlefont The {\:\ttitlefont \BibTeX} preprocessor}
 \vskip 15pt \centerline{(Version 0.99c---\today)} \vfill}
@y
 \def\titlepage{F}
 \centerline{\:\titlefont The {\:\ttitlefont J\BibTeX} preprocessor}
 \vskip 15pt \centerline{(Version 0.30 base on C Version \BibTeX 0.99c---\today)} \vfill}
@z

@x banner
@d banner=='This is BibTeX, Version 0.99c' {printed when the program starts}
@y
@d banner=='This is JBibTeX 0.30 based on C Version BibTeX 0.99c' {printed when the program starts}
@z

@x terminal
@d term_out == tty
@d term_in == tty
@y
@d term_out == stdout
@d term_in == stdin
@z

@x debug..gubed, stat..tats, trace..ecart
@d debug == @{		{ remove the `|@{|' when debugging }
@d gubed == @t@>@}	{ remove the `|@}|' when debugging }
@f debug == begin
@f gubed == end
@#
@d stat == @{		{ remove the `|@{|' when keeping statistics }
@d tats == @t@>@}	{ remove the `|@}|' when keeping statistics }
@f stat == begin
@f tats == end
@#
@d trace == @{		{ remove the `|@{|' when in |trace| mode }
@d ecart == @t@>@}	{ remove the `|@}|' when in |trace| mode }
@f trace == begin
@f ecart == end
@y
@d debug == ifdef('DEBUG')
@d gubed == endif('DEBUG')
@f debug == begin
@f gubed == end
@#
@d stat == ifdef('STAT')
@d tats == endif('STAT')
@f stat==begin
@f tats==end
@#
@d trace == ifdef@&('TRACE')
@d ecart == endif@&('TRACE')
@f trace == begin
@f ecart == end
@z

@x
@d incr(#) == #:=#+1	{increase a variable by unity}
@d decr(#) == #:=#-1	{decrease a variable by unity}
@y
{These are defined as C macros}
@z

@x compiler directives
@{@&$C-,A+,D-@}	 {no range check, catch arithmetic overflow, no debug overhead}
@!debug @{@&$C+,D+@}@+ gubed		{but turn everything on when debugging}
@y
{Don't need 'em for C}
@z

@x
    goto exit_program;
@y
    uexit(0);
@z

@x Change system constants by Shouichi Matsui
@<Constants in the outer block@>=
@!buf_size=1000; {maximum number of characters in an input line (or string)}
@!min_print_line=3; {minimum \.{.bbl} line length: must be |>=3|}
@!max_print_line=79; {the maximum: must be |>min_print_line| and |<buf_size|}
@!aux_stack_size=20; {maximum number of simultaneous open \.{.aux} files}
@!max_bib_files=20; {maximum number of \.{.bib} files allowed}
@!pool_size=65000; {maximum number of characters in strings}
@!max_strings=4000; {maximum number of strings, including pre-defined;
							must be |<=hash_size|}
@!max_cites=750; {maximum number of distinct cite keys; must be
							|<=max_strings|}
@!min_crossrefs=2; {minimum number of cross-refs required for automatic
							|cite_list| inclusion}
@!wiz_fn_space=3000; {maximum amount of |wiz_defined|-function space}
@!single_fn_space=100; {maximum amount for a single |wiz_defined|-function}
@!max_ent_ints=3000; {maximum number of |int_entry_var|s
					(entries $\times$ |int_entry_var|s)}
@!max_ent_strs=3000; {maximum number of |str_entry_var|s
					(entries $\times$ |str_entry_var|s)}
@!ent_str_size=100; {maximum size of a |str_entry_var|; must be |<=buf_size|}
@!glob_str_size=1000; {maximum size of a |str_global_var|;
							must be |<=buf_size|}
@!max_fields=17250; {maximum number of fields (entries $\times$ fields,
					about |23*max_cites| for consistency)}
@!lit_stk_size=100; {maximum number of literal functions on the stack}
@y
@<Constants in the outer block@>=
@!buf_size=3000; {maximum number of characters in an input line (or string)}
@!min_print_line=3; {minimum \.{.bbl} line length: must be |>=3|}
@!max_print_line=79; {the maximum: must be |>min_print_line| and |<buf_size|}
@!aux_stack_size=20; {maximum number of simultaneous open \.{.aux} files}
@!max_bib_files=20; {maximum number of \.{.bib} files allowed}
@!pool_size=126000; {maximum number of characters in strings}
@!max_strings=6000; {maximum number of strings, including pre-defined;
							must be |<=hash_size|}
@!max_cites=750; {maximum number of distinct cite keys; must be
							|<=max_strings|}
@!min_crossrefs=2; {minimum number of cross-refs required for automatic
							|cite_list| inclusion}
@!wiz_fn_space=3400; {maximum amount of |wiz_defined|-function space}
@!single_fn_space=100; {maximum amount for a single |wiz_defined|-function}
@!max_ent_ints=3000; {maximum number of |int_entry_var|s
					(entries $\times$ |int_entry_var|s)}
@!max_ent_strs=3000; {maximum number of |str_entry_var|s
					(entries $\times$ |str_entry_var|s)}
@!ent_str_size=100; {maximum size of a |str_entry_var|; must be |<=buf_size|}
@!glob_str_size=1000; {maximum size of a |str_global_var|;
							must be |<=buf_size|}
@!max_fields=18000; {maximum number of fields (entries $\times$ fields,
					about |24*max_cites| for consistency)}
@!lit_stk_size=100; {maximum number of literal functions on the stack}
@z

@x increase file_name_size
@d hash_size=5000	{must be |>= max_strings| and |>= hash_prime|}
@d hash_prime=4253	{a prime number about 85\% of |hash_size| and |>= 128|
						and |< @t$2^{14}-2^6$@>|}
@d file_name_size=40	{file names shouldn't be longer than this}
@y
@d hash_size=7000	{must be |>= max_strings| and |>= hash_prime|}
@d hash_prime=5903	{a prime number about 85\% of |hash_size| and |>= 128|
						and |< @t$2^{14}-2^6$@>|}
@d file_name_size=1024	{file names shouldn't be longer than this}
@z

@x Changes for JBibTeX by Shouichi Matsui [22]
Characters of text that have been converted to \TeX's internal form
are said to be of type |ASCII_code|, which is a subrange of the integers.

@<Types in the outer block@>=
@!ASCII_code=0..127;	{seven-bit numbers}
@y
Characters of text that have been converted to \TeX's internal form
are said to be of type |ASCII_code|, which is a subrange of the integers.

@<Types in the outer block@>=
@!ASCII_code=0..255;	{eight-bit numbers}
@z

@x Changes for JBibTeX by Shouichi Matsui [23]
@d text_char == char	{the data type of characters in text files}
@d first_text_char=0	{ordinal number of the smallest element of |text_char|}
@d last_text_char=127	{ordinal number of the largest element of |text_char|}
@y
@d text_char == char	{the data type of characters in text files}
@d first_text_char=0	{ordinal number of the smallest element of |text_char|}
@d last_text_char=255	{ordinal number of the largest element of |text_char|}
@z

@x Changes for JBibTeX by Shouichi Matsui [24]
@!xord: array [text_char] of ASCII_code;
  {specifies conversion of input characters}
@!xchr: array [ASCII_code] of text_char;
  {specifies conversion of output characters}
@y
@!xord: array [ASCII_code] of ASCII_code;
  {specifies conversion of input characters}
@!xchr: array [ASCII_code] of ASCII_code;
  {specifies conversion of output characters}
@z

@x Changes for JBibTeX by Shouichi Matsui [27]
for i:=1 to @'37 do xchr[i]:=' ';
xchr[tab]:=chr(tab);
@y
for i:=1 to @'37 do xchr[i]:=' ';
xchr[tab]:=chr(tab);
for i:=@'200 to @'237 do xchr[i]:=chr(i-@'200);
for i:=@'240 to 255 do xchr[i]:=chr(i);
xchr[@'33]:=chr(@'33); {escape char}
@z

@x Changes for JBibTeX by Shouichi Matsui [28]
@<Set initial values of key variables@>=
for i:=first_text_char to last_text_char do xord[chr(i)]:=invalid_code;
for i:=1 to @'176 do xord[xchr[i]]:=i;
@y
@<Set initial values of key variables@>=
for i:=first_text_char to last_text_char do xord[i]:=invalid_code;
for i:=1 to @'176 do xord[i]:= i;
for i:=@'200 to @'237 do xord[i]:= i-@'200;
for i:=@'240 to 255 do xord[i]:=i;
@z
@x Changes for JBibTeX by Shouichi Matsui  for ver. 0.30
@d period = "."			{these are three}
@d question_mark = "?"		{string-ending characters}
@d exclamation_mark = "!"	{of interest in \.{add.period\$}}
@y
@d period = "."			{these are three}
@d question_mark = "?"		{string-ending characters}
@d exclamation_mark = "!"	{of interest in \.{add.period\$}}
@d zen_pun_first = 161		{Zenkaku punctuation first byte; in EUC}
@d zen_space = 161		{Zenkaku space first, second byte; in EUC}
@d zen_kuten = 162		{Zenkaku kuten second byte; in EUC}
@d zen_ten = 163		{Zenkaku ten second byte; in EUC}
@d zen_comma = 164		{Zenkaku comman second byte; in EUC}
@d zen_period = 165		{Zenkaku period second byte; in EUC}
@d zen_question = 169		{Zenkaku question mark second byte; in EUC}
@d zen_exclamation = 170	{Zenkaku exclamation mark second byte; in EUC}
@z
@x Changes for JBibTeX by Shouichi Matsui [32]
@<Set initial values of key variables@>=
for i:=0 to @'177 do lex_class[i] := other_lex;
for i:=0 to @'37 do lex_class[i] := illegal;
lex_class[invalid_code] := illegal;
lex_class[tab] := white_space;
lex_class[space] := white_space;
lex_class[tie] := sep_char;
lex_class[hyphen] := sep_char;
for i:=@'60 to @'71 do lex_class[i] := numeric;
for i:=@'101 to @'132 do lex_class[i] := alpha;
for i:=@'141 to @'172 do lex_class[i] := alpha;
@y
@<Set initial values of key variables@>=
for i:=0 to @'377 do lex_class[i] := other_lex;
for i:=0 to @'37 do lex_class[i] := illegal;
lex_class[invalid_code] := illegal;
lex_class[tab] := white_space;
lex_class[space] := white_space;
lex_class[tie] := sep_char;
lex_class[hyphen] := sep_char;
for i:=@'60 to @'71 do lex_class[i] := numeric;
for i:=@'101 to @'132 do lex_class[i] := alpha;
for i:=@'141 to @'172 do lex_class[i] := alpha;
for i:=@'200 to @'237 do lex_class[i] := illegal;
for i:=@'240 to 255 do lex_class[i] := alpha;
lex_class[@'33]:=alpha;
@z

@x Changes for JBibTeX by Shouichi Matsui [33]
@<Set initial values of key variables@>=
for i:=0 to @'177 do id_class[i] := legal_id_char;
for i:=0 to @'37 do id_class[i] := illegal_id_char;
id_class[space] := illegal_id_char;
id_class[tab] := illegal_id_char;
id_class[double_quote] := illegal_id_char;
id_class[number_sign] := illegal_id_char;
id_class[comment] := illegal_id_char;
id_class[single_quote] := illegal_id_char;
id_class[left_paren] := illegal_id_char;
id_class[right_paren] := illegal_id_char;
id_class[comma] := illegal_id_char;
id_class[equals_sign] := illegal_id_char;
id_class[left_brace] := illegal_id_char;
id_class[right_brace] := illegal_id_char;
@y
@<Set initial values of key variables@>=
for i:=0 to @'377 do id_class[i] := legal_id_char;
for i:=0 to @'37 do id_class[i] := illegal_id_char;
for i:=@'200 to @'237 do id_class[i] := illegal_id_char;
id_class[@'33] := legal_id_char;
id_class[space] := illegal_id_char;
id_class[tab] := illegal_id_char;
id_class[double_quote] := illegal_id_char;
id_class[number_sign] := illegal_id_char;
id_class[comment] := illegal_id_char;
id_class[single_quote] := illegal_id_char;
id_class[left_paren] := illegal_id_char;
id_class[right_paren] := illegal_id_char;
id_class[comma] := illegal_id_char;
id_class[equals_sign] := illegal_id_char;
id_class[left_brace] := illegal_id_char;
id_class[right_brace] := illegal_id_char;
@z

@x Changes for JBibTeX by Shouichi Matsui [35]
char_width[@'167] := 722;
char_width[@'170] := 528;
char_width[@'171] := 528;
char_width[@'172] := 444;
char_width[@'173] := 500;
char_width[@'174] :=1000;
char_width[@'175] := 500;
char_width[@'176] := 500;
@y
char_width[@'167] := 722;
char_width[@'170] := 528;
char_width[@'171] := 528;
char_width[@'172] := 444;
char_width[@'173] := 500;
char_width[@'174] :=1000;
char_width[@'175] := 500;
char_width[@'176] := 500;
for i:=@'240 to 254 do char_width[i]:=514;
@z

@x declare real_name_of_file
Most of what we need to do with respect to input and output can be handled
by the I/O facilities that are standard in \PASCAL, i.e., the routines
called |get|, |put|, |eof|, and so on. But
standard \PASCAL\ does not allow file variables to be associated with file
names that are determined at run time, so it cannot be used to implement
\BibTeX; some sort of extension to \PASCAL's ordinary |reset| and |rewrite|
is crucial for our purposes. We shall assume that |name_of_file| is a variable
of an appropriate type such that the \PASCAL\ run-time system being used to
implement \BibTeX\ can open a file whose external name is specified by
|name_of_file|. \BibTeX\ does no case conversion for file names.

@<Globals in the outer block@>=
@!name_of_file:packed array[1..file_name_size] of char;
			 {on some systems this is a \&{record} variable}
@y
Most of what we need to do with respect to input and output can be handled
by the I/O facilities that are standard in \PASCAL, i.e., the routines
called |get|, |put|, |eof|, and so on. But
standard \PASCAL\ does not allow file variables to be associated with file
names that are determined at run time, so it cannot be used to implement
\BibTeX; some sort of extension to \PASCAL's ordinary |reset| and |rewrite|
is crucial for our purposes. We shall assume that |name_of_file| is a variable
of an appropriate type such that the \PASCAL\ run-time system being used to
implement \BibTeX\ can open a file whose external name is specified by
|name_of_file|. \BibTeX\ does no case conversion for file names.

The C version of BibTeX uses search paths to look for files to open.
We use |real_name_of_file| to hold the |name_of_file| with a directory name
from the path in front of it.

@<Globals in the outer block@>=
@!name_of_file,@!real_name_of_file:packed array[1..file_name_size] of char;
@z

@x opening files
The \ph\ compiler with which the present version of \TeX\ was prepared has
extended the rules of \PASCAL\ in a very convenient way. To open file~|f|,
we can write
$$\vbox{\halign{#\hfil\qquad&#\hfil\cr
|reset(f,@t\\{name}@>,'/O')|&for input;\cr
|rewrite(f,@t\\{name}@>,'/O')|&for output.\cr}}$$
The `\\{name}' parameter, which is of type `\ignorespaces|packed
array[@t\<\\{any}>@>] of text_char|', stands for the name of
the external file that is being opened for input or output.
Blank spaces that might appear in \\{name} are ignored.

The `\.{/O}' parameter tells the operating system not to issue its own
error messages if something goes wrong. If a file of the specified name
cannot be found, or if such a file cannot be opened for some other reason
(e.g., someone may already be trying to write the same file), we will have
|@!erstat(f)<>0| after an unsuccessful |reset| or |rewrite|.  This allows
\TeX\ to undertake appropriate corrective action.

\TeX's file-opening procedures return |false| if no file identified by
|name_of_file| could be opened.

@d reset_OK(#)==erstat(#)=0
@d rewrite_OK(#)==erstat(#)=0

@<Procedures and functions for file-system interacting@>=
function erstat(var f:file):integer; extern;	{in the runtime library}
@#@t\2@>
function a_open_in(var f:alpha_file):boolean;	{open a text file for input}
begin reset(f,name_of_file,'/O'); a_open_in:=reset_OK(f);
end;
@#
function a_open_out(var f:alpha_file):boolean;	{open a text file for output}
begin rewrite(f,name_of_file,'/O'); a_open_out:=rewrite_OK(f);
end;
@y
@ The \ph\ compiler with which the present version of \TeX\ was prepared has
extended the rules of \PASCAL\ in a very convenient way for file opening.
Berkeley {\mc UNIX} \PASCAL\ isn't nearly as nice as \ph.
Normally, it bombs out if a file open fails.
An external C procedure, |test_access| is used to check whether or not the
open will work.  It is declared in the ``ext.h'' include file, and it returns
|true| or |false|. The |name_of_file| global holds the file name whose access
is to be tested.
The first parameter for |test_access| is the access mode,
one of |read_access_mode| or |write_access_mode|.

We also implement path searching in |test_access|:  its second parameter is
one of the ``file path'' constants defined below.  If |name_of_file|
doesn't start with |'/'| then |test_access| tries prepending pathnames
from the appropriate path list until success or the end of path list
is reached.
On return, |real_name_of_file| contains the original name with the path
that succeeded (if any) prepended.  It is the name used in the various
open procedures.

Path searching is not done for output files.

@d read_access_mode=4  {``read'' mode for |test_access|}
@d write_access_mode=2 {``write'' mode for |test_access|}

@d no_file_path=0    {no path searching should be done}
@d input_file_path=1 {path specifier for input files}
@d bib_file_path=2   {path specifier for .bib files}

@<Procedures and functions for file-system interacting@>=
function a_open_in(var f:palpha_file; pathspec:integer):boolean;
  {open a text file for input}
var @!ok:boolean;
begin
if test_access(read_access_mode,pathspec) then
    begin reset(f,real_name_of_file); ok:=true@+end
else
    ok:=false;
a_open_in:=ok;
end;
@#
function a_open_out(var f:palpha_file):boolean;
  {open a text file for output}
var @!ok:boolean;
begin
if test_access(write_access_mode,no_file_path) then
    begin rewrite(f,real_name_of_file); ok:=true @+end
else ok:=false;
a_open_out:=ok;
end;
@z

@x
@<Procedures and functions for file-system interacting@>=
procedure a_close(var f:alpha_file);		{close a text file}
begin close(f);
end;
@y
{aclose will be defined as a C macro}
@z

%%%%% overflow and confusion go here 
@x faster input_ln
Standard \PASCAL\ says that a file should have |eoln| immediately
before |eof|, but \BibTeX\ needs only a weaker restriction: If |eof|
occurs in the middle of a line, the system function |eoln| should return
a |true| result (even though |f^| will be undefined).

@<Procedures and functions for all file I/O, error messages, and such@>=
function input_ln(var f:alpha_file) : boolean;
				{inputs the next line or returns |false|}
label loop_exit;
begin
last:=0;
if (eof(f)) then input_ln:=false
else
  begin
  while (not eoln(f)) do
    begin
    if (last >= buf_size) then
	buffer_overflow;
    buffer[last]:=xord[f^];
    get(f); incr(last);
    end;
  get(f);
  while (last > 0) do		{remove trailing |white_space|}
    if (lex_class[buffer[last-1]] = white_space) then
      decr(last)
     else
      goto loop_exit;
loop_exit:
  input_ln:=true;
  end;
end;
@y
With Berkeley {\mc UNIX} we call an external C procedure, |line_read|.
That routine fills |buffer| from |0| onwards with the |xord|'ed values
of the next line, setting |last| appropriately.  It will stop if
|last=buf_size|, and the following will cause an ``overflow'' abort.

@<Procedures and functions for all file I/O, error messages, and such@>=
function input_ln(var f:alpha_file) : boolean;
  {inputs the next line or returns |false|}
label loop_exit;
begin
last:=0;
if eof(f) then input_ln:=false
else
  begin
  line_read(f,buf_size);
  if last>=buf_size then
	overflow('buffer size ',buf_size);
  while (last > 0) do		{remove trailing |white_space|}
    if lex_class[buffer[last-1]] = white_space then
      decr(last)
     else
      goto loop_exit;
loop_exit:
  input_ln:=true;
  end;
end;
@z

@x
if (length(file_name) > file_name_size) then
    begin
    print ('File=');
    print_pool_str (file_name);
    print_ln (',');
    file_nm_size_overflow;
    end;
name_ptr := 1;
@y
if (length(file_name) > file_name_size) then
    begin
    print ('File=');
    print_pool_str (file_name);
    print_ln (',');
    file_nm_size_overflow;
    end;
name_ptr := 0;
@z

@x
name_ptr := name_length + 1;
p_ptr := str_start[ext];
while (p_ptr < str_start[ext+1]) do
    begin
    name_of_file[name_ptr] := chr (str_pool[p_ptr]);
    incr(name_ptr); incr(p_ptr);
    end;
name_length := name_length + length(ext);
name_ptr := name_length+1;
while (name_ptr <= file_name_size) do	{pad with blanks}
    begin
    name_of_file[name_ptr] := ' ';
    incr(name_ptr);
    end;
@y
name_ptr := name_length;
p_ptr := str_start[ext];
while (p_ptr < str_start[ext+1]) do
    begin
    name_of_file[name_ptr] := chr (str_pool[p_ptr]);
    incr(name_ptr); incr(p_ptr);
    end;
name_length := name_length + length(ext);
name_of_file[name_length] := ' ';
@z

@x
    print_pool_str (area); print (name_of_file,',');
    file_nm_size_overflow;
    end;
name_ptr := name_length;
while (name_ptr > 0) do		{shift up name}
    begin
    name_of_file[name_ptr+length(area)] := name_of_file[name_ptr];
    decr(name_ptr);
    end;
name_ptr := 1;
p_ptr := str_start[area];
while (p_ptr < str_start[area+1]) do
@y
    print_pool_str (area); print_str (name_of_file,',');
    file_nm_size_overflow;
    end;
name_ptr := name_length;
while (name_ptr > 0) do		{shift up name}
    begin
    name_of_file[name_ptr+length(area)] := name_of_file[name_ptr];
    decr(name_ptr);
    end;
name_ptr := 0;
p_ptr := str_start[area];
while (p_ptr < str_start[area+1]) do
@z

@x
for i:=1 to len do
    buffer[i] := xord[pds[i]];
@y
for i:=1 to len do
    buffer[i] := xord[pds[i-1]];
@z

@x
procedure sam_too_long_file_name_print;
begin
write (term_out,'File name `');
name_ptr := 1;
while (name_ptr <= aux_name_length) do
    begin
    write (term_out,name_of_file[name_ptr]);
@y
procedure sam_too_long_file_name_print;
begin
write (term_out,'File name `');
name_ptr := 0;
while (name_ptr < aux_name_length) do
    begin
    write (term_out,name_of_file[name_ptr]);
@z

@x
procedure sam_wrong_file_name_print;
begin
write (term_out,'I couldn''t open file name `');
name_ptr := 1;
while (name_ptr <= name_length) do
    begin
    write (term_out,name_of_file[name_ptr]);
    incr(name_ptr);
    end;
write_ln (term_out,'''');
end;
@y
procedure sam_wrong_file_name_print;
begin
write (term_out,'I couldn''t open file name `');
name_ptr := 0;
while (name_ptr < name_length) do
    begin
    write (term_out,name_of_file[name_ptr]);
    incr(name_ptr);
    end;
write_ln (term_out,'''');
end;
@z

@x reading the command line
This procedure consists of a loop that reads and processes a (nonnull)
\.{.aux} file name.  It's this module and the next two that must be
changed on those systems using command-line arguments.  Note: The
|term_out| and |term_in| files are system dependent.

@<Procedures and functions for the reading and processing of input files@>=
procedure get_the_top_level_aux_file_name;
label aux_found,@!aux_not_found;
var @<Variables for possible command-line processing@>@/
begin
check_cmnd_line := false;			{many systems will change this}
loop
    begin
    if (check_cmnd_line) then
	@<Process a possible command line@>
      else
	begin
	write (term_out,'Please type input file name (no extension)--');
	if (eoln(term_in)) then			{so the first |read| works}
	    read_ln (term_in);
	aux_name_length := 0;
	while (not eoln(term_in)) do
	    begin
	    if (aux_name_length = file_name_size) then
		begin
		while (not eoln(term_in)) do	{discard the rest of the line}
		    get(term_in);
		sam_you_made_the_file_name_too_long;
		end;
	    incr(aux_name_length);
	    name_of_file[aux_name_length] := term_in^;
	    get(term_in);
	    end;
	end;
    @<Handle this \.{.aux} name@>;
aux_not_found:
    check_cmnd_line := false;
    end;
aux_found:			{now we're ready to read the \.{.aux} file}
end;
@y
@<Procedures and functions for the reading and processing of input files@>=
procedure get_the_top_level_aux_file_name;
label aux_found,@!aux_not_found;
begin
loop
    begin
    if (gargc > 1) then
	@<Process a possible command line@>
      else begin
	write (term_out,'Please type input file name (no extension)--');
	aux_name_length := 0;
	while (not eoln(term_in)) do begin
	    if (aux_name_length = file_name_size) then begin
		readln(term_in);
		sam_you_made_the_file_name_too_long;
	    end;
	    name_of_file[aux_name_length] := getc(term_in);
	    incr(aux_name_length);
	end;
	if (eof(term_in)) then begin
	    writeln(term_out);
	    writeln(term_out,'Unexpected end of file on terminal---giving up!');
	    uexit(1);
	end;
	readln(term_in);
      end;
    @<Handle this \.{.aux} name@>;
aux_not_found:
    gargc := 0;
    end;
aux_found:			{now we're ready to read the \.{.aux} file}
end;
@z

@x
@<Variables for possible command-line processing@>=
@!check_cmnd_line : boolean;	{|true| if we're to check the command line}
@y
@z

@x
@<Process a possible command line@>=
begin
do_nothing;		{the ``default system'' doesn't use the command line}
end
@y
@<Process a possible command line@>=
aux_name_length := get_cmd_line(name_of_file, file_name_size)
@z

@x
if (not a_open_in(cur_aux_file)) then
    sam_you_made_the_file_name_wrong;
@y
if (not a_open_in(cur_aux_file,no_file_path)) then
    sam_you_made_the_file_name_wrong;
@z

@x
while (name_ptr <= name_length) do
    begin
    buffer[name_ptr] := xord[name_of_file[name_ptr]];
    incr(name_ptr);
    end;
@y
while (name_ptr <= name_length) do
    begin
    buffer[name_ptr] := xord[name_of_file[name_ptr-1]];
    incr(name_ptr);
    end;
@z

% Handle path searching on opening files
@x
if (not a_open_in(cur_bib_file)) then
    begin
    add_area (s_bib_area);
    if (not a_open_in(cur_bib_file)) then
	open_bibdata_aux_err ('I couldn''t open database file ');
    end;
@y
if (not a_open_in(cur_bib_file,bib_file_path)) then
	open_bibdata_aux_err ('I couldn''t open database file ');
@z

@x
add_extension (s_bst_extension);
if (not a_open_in(bst_file)) then
    begin
    add_area (s_bst_area);
    if (not a_open_in(bst_file)) then
	begin
	print ('I couldn''t open style file ');
	print_bst_name;@/
	bst_str := 0;				{mark as unused again}
	aux_err_return;
	end;
    end;
@y
add_extension (s_bst_extension);
if (not a_open_in(bst_file,input_file_path)) then
    begin
	print ('I couldn''t open style file ');
	print_bst_name;@/
	bst_str := 0;				{mark as unused again}
	aux_err_return;
    end;
@z

@x
name_ptr := name_length+1;
@y
name_ptr := name_length;
@z

% More search path stuff
@x
if (not a_open_in(cur_aux_file)) then
@y
if (not a_open_in(cur_aux_file, no_file_path)) then
@z

@x
buf_ptr2 := last;	{to get the first input line}
loop
    begin
    if (not eat_bst_white_space) then	{the end of the \.{.bst} file}
	goto bst_done;
    get_bst_command_and_process;
    end;
bst_done: a_close (bst_file);
@y
buf_ptr2 := last;	{to get the first input line}
hack1;
    begin
    if (not eat_bst_white_space) then	{the end of the \.{.bst} file}
	hack2;
    get_bst_command_and_process;
    end;
bst_done: a_close (bst_file);
@z
@x Changes for JBibTeX by Shouichi Matsui [322]
procedure add_out_pool (@!p_str : str_number);
var break_ptr : buf_pointer;	{the first character following the line break}
@!end_ptr : buf_pointer;	{temporary end-of-buffer pointer}
@y
procedure add_out_pool (@!p_str : str_number);
var break_ptr : buf_pointer;	{the first character following the line break}
@!end_ptr : buf_pointer;	{temporary end-of-buffer pointer}
@!in_mid_kanji:boolean; {flag if |max_print_line|-1 is in mid kanji}
@!last_char_ptr : buf_pointer;
@!t : buf_pointer;
@z

@x Changes for JBibTeX by Shouichi Matsui [324]
@<Break that unbreakable line@>=
begin
out_buf[end_ptr] := out_buf[max_print_line-1];	{save this character}
out_buf[max_print_line-1] := comment;		{so \TeX\ does the thing right}
out_buf_length := max_print_line;
break_ptr := out_buf_length - 1;	{the `|-1|' allows for the restoration}
output_bbl_line;				{output what we can,}
out_buf[max_print_line-1] := out_buf[end_ptr];	{restore this character}
out_buf_ptr := 0;
tmp_ptr := break_ptr;
while (tmp_ptr < end_ptr) do			{and slide the rest down}
    begin
    out_buf[out_buf_ptr] := out_buf[tmp_ptr];
    incr(out_buf_ptr);
    incr(tmp_ptr);
    end;
out_buf_length := end_ptr - break_ptr;
end
@y
@<Break that unbreakable line@>=
begin
@<Check if |max_print_line|-1 is in mid kanji@>;
if in_mid_kanji then last_char_ptr:=max_print_line-1
                else last_char_ptr:=max_print_line;
out_buf[end_ptr] := out_buf[last_char_ptr-1];	{save this character}
out_buf[last_char_ptr-1] := comment;		{so \TeX\ does the thing right}
out_buf_length := last_char_ptr;
break_ptr := out_buf_length - 1;	{the `|-1|' allows for the restoration}
output_bbl_line;				{output what we can,}
out_buf[last_char_ptr-1] := out_buf[end_ptr];	{restore this character}
out_buf_ptr := 0;
tmp_ptr := break_ptr;
while (tmp_ptr < end_ptr) do			{and slide the rest down}
    begin
    out_buf[out_buf_ptr] := out_buf[tmp_ptr];
    incr(out_buf_ptr);
    incr(tmp_ptr);
    end;
out_buf_length := end_ptr - break_ptr;
end
@z

@x Changes for JBibTeX by Shouichi Matsui [332]
@!b_while : hash_loc;		{\.{while\$}}
@!b_width : hash_loc;		{\.{width\$}}
@!b_write : hash_loc;		{\.{write\$}}
@!b_default : hash_loc;		{either \.{skip\$} or \.{default.type}}
@y
@!b_while : hash_loc;		{\.{while\$}}
@!b_width : hash_loc;		{\.{width\$}}
@!b_write : hash_loc;		{\.{write\$}}
@!b_default : hash_loc;		{either \.{skip\$} or \.{default.type}}
@!b_is_kanji_str : hash_loc;    {\.{is.kanji.str\$}}
@z

@x Changes for JBibTeX by Shouichi Matsui [334]
@d n_while = 34		{\.{while\$}}
@d n_width = 35		{\.{width\$}}
@d n_write = 36		{\.{write\$}}

@<Constants in the outer block@>=
@!num_blt_in_fns = 37;	{one more than the previous number}
@y
@d n_while = 34		{\.{while\$}}
@d n_width = 35		{\.{width\$}}
@d n_write = 36		{\.{write\$}}
@d n_is_kanji_str = 37  {\.{is.kanji.str\$}}

@<Constants in the outer block@>=
@!num_blt_in_fns = 38;	{one more than the previous number}
@z

@x Changes for JBibTeX by Shouichi Matsui [335]
build_in('width$      ',6,b_width,n_width);
build_in('while$      ',6,b_while,n_while);
build_in('width$      ',6,b_width,n_width);
build_in('write$      ',6,b_write,n_write);
@y
build_in('while$      ',6,b_while,n_while);
build_in('width$      ',6,b_width,n_width);
build_in('write$      ',6,b_write,n_write);
build_in('is.kanji.str$',13,b_is_kanji_str,n_is_kanji_str);
@z

@x Changes for JBibTeX by Shouichi Matsui [342]
    n_width :		x_width;
    n_write :		x_write;
    othercases confusion ('Unknown built-in function')
endcases;
end
@y
    n_width :		x_width;
    n_write :		x_write;
    n_is_kanji_str:     x_is_kanji_str;
    othercases confusion ('Unknown built-in function')
endcases;
end
@z

@x Changes for JBibTeX by Shouichi Matsui [343]
@<|execute_fn|({\.{width\$}})@>@;
@<|execute_fn|({\.{write\$}})@>@;
@<|execute_fn| itself@>
@y
@<|execute_fn|({\.{width\$}})@>@;
@<|execute_fn|({\.{write\$}})@>@;
@<|execute_fn|({\.{is.kanji.str\$}})@>@;
@<|execute_fn| itself@>
@z

@x Changes for JBibTeX by Shouichi Matsui   --- add.period$ for ver. 0.30
case (str_pool[sp_ptr]) of
    period,
    question_mark,
    exclamation_mark :
	repush_string;
    othercases
	@<Add the |period| (it's necessary) and push@>
@y
case (str_pool[sp_ptr]) of
    period,
    question_mark,
    exclamation_mark :
	repush_string;
    zen_ten,
    zen_period,
    zen_question,
    zen_exclamation:
        if( str_pool[sp_ptr-1] = zen_pun_first ) then
	    repush_string
        else
            @<Add the |period| (it's necessary) and push@>;
    othercases
	@<Add the |period| (it's necessary) and push@>
@z

@x Changes for JBibTeX by Shouichi Matsui [377]
else if (length(pop_lit1) <> 1) then
    begin
    print ('"');
    print_pool_str (pop_lit1);
    bst_ex_warn ('" isn''t a single character');
    push_lit_stk (0, stk_int);
    end
@y
else if (length(pop_lit1) <> 1) then
    if(str_pool[str_start[pop_lit1]]>127) then { a KANJI char is 2byte long }
	push_lit_stk(str_pool[str_start[pop_lit1]],stk_int)
    else begin
	print ('"');
	print_pool_str (pop_lit1);
	bst_ex_warn ('" isn''t a single character');
	push_lit_stk (0, stk_int);
    end
@z


@x by Shouichi Matsui for Zenkaku comma
if (ex_buf_ptr < ex_buf_length) then		{remove the ``and''}
    ex_buf_ptr := ex_buf_ptr - 4;
@y
if (ex_buf_ptr < ex_buf_length) then	{remove the ``and'', or Zenkau comma}
 begin
    if( (ex_buf[ex_buf_ptr-1]=zen_comma) or (ex_buf[ex_buf_ptr-1]=zen_kuten) )
    then ex_buf_ptr := ex_buf_ptr - 2
    else ex_buf_ptr := ex_buf_ptr - 4;
 end;
@z

@x Changes for JBibTeX by Shouichi Matsui for Zenkaku comma
    "a", "A" :
	begin
	incr(ex_buf_ptr);
	if (preceding_white) then
	    @<See if we have an ``and''@>;	{if so, |and_found := true|}
	preceding_white := false;
	end;
@y
    "a", "A" :
	begin
	incr(ex_buf_ptr);
	if (preceding_white) then
	    @<See if we have an ``and''@>;	{if so, |and_found := true|}
	preceding_white := false;
	end;
     zen_pun_first:
	begin
	  if((ex_buf[ex_buf_ptr+1]=zen_comma) or 
	     (ex_buf[ex_buf_ptr+1]=zen_kuten) ) then
		begin
		  preceding_white := false;
		  and_found  := true
		end
	  else if(ex_buf[ex_buf_ptr+1]=zen_space) then
	       begin
		  ex_buf[ex_buf_ptr]   := space;
		  ex_buf[ex_buf_ptr+1] := space;
		  preceding_white := true;
	       end;
	  ex_buf_ptr := ex_buf_ptr + 2;
	end;
@z

@x Changes for JBibTeX by Shouichi Matsui [415]
    if (lex_class[name_buf[name_bf_ptr]] = alpha) then
	begin
	append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
	goto loop_exit;
	end
    else if ((name_buf[name_bf_ptr] = left_brace) and
@y
    if (lex_class[name_buf[name_bf_ptr]] = alpha) then
	begin
	    if name_buf[name_bf_ptr]>127 then begin
		append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
		incr(name_bf_ptr);
		append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
	    end
	    else
		append_ex_buf_char_and_check (name_buf[name_bf_ptr]);
	goto loop_exit;
	end
    else if ((name_buf[name_bf_ptr] = left_brace) and
@z

@x Changes for JBibTeX by Shouichi Matsui [437]
@<|execute_fn|({\.{substring\$}})@>=
procedure x_substring;
label exit;
begin
@y
@<|execute_fn|({\.{substring\$}})@>=
procedure x_substring;
label exit;
var tps,tpe:pool_pointer; {temporary pointer}
begin
@z

@x Changes for JBibTeX by Shouichi Matsui [438]
while (sp_ptr < sp_end) do			{shift the substring}
    begin
    append_char (str_pool[sp_ptr]);
    incr(sp_ptr);
    end;
@y
{ 2 bytes Kanji code break check }
tps:=str_start[pop_lit3];
while (tps < sp_ptr ) do
    if str_pool[tps] > 127 
    then tps := tps + 2
    else incr(tps);
tpe:=tps;
while (tpe < sp_end ) do begin
    if str_pool[tpe] > 127 
    then tpe := tpe+2
    else incr(tpe);
end;
if tps<>sp_ptr then begin
    if tps>str_start[pop_lit3]
    then decr(sp_ptr)
    else incr(sp_ptr);
end;
if tpe<>sp_end then begin
    if tpe<str_start[pop_lit3+1]
    then incr(sp_end)
    else decr(sp_end);
end;

while (sp_ptr < sp_end) do			{shift the substring}
    begin
    if str_pool[sp_ptr] >127 then begin
    	 append_char (str_pool[sp_ptr]); incr(sp_ptr);
   	 append_char (str_pool[sp_ptr]); incr(sp_ptr);
	 end
    else begin
         append_char (str_pool[sp_ptr]); incr(sp_ptr); end;
    end;
@z

@x Changes for JBibTeX by Shouichi Matsui [445]
    else if (str_pool[sp_xptr1-1] = right_brace) then
	begin
	if (sp_brace_level > 0) then
	    decr(sp_brace_level);
	end
    else
	incr(num_text_chars);
    end;
@y
    else if (str_pool[sp_xptr1-1] = right_brace) then
	begin
	if (sp_brace_level > 0) then
	    decr(sp_brace_level);
	end
    else if (str_pool[sp_xptr1-1]>127) then begin {kanji char}
	    incr(sp_xptr1); num_text_chars:=num_text_chars+2;
         end
    else
	incr(num_text_chars);
    end;
@z

@x system dependencies by Shouichi Matsui
@* System-dependent changes.
@^system dependencies@>
This section should be replaced, if necessary, by changes to the program
that are necessary to make \BibTeX\ work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@y
@* System-dependent changes.
@^system dependencies@>
This section should be replaced, if necessary, by changes to the program
that are necessary to make \BibTeX\ work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.

@
@<Check if |max_print_line|-1 is in mid kanji@>=
t:=0;
while(t<max_print_line-1) do 
   if out_buf[t]>127 then t:=t+2 else t:=t+1;
in_mid_kanji:= t>=max_print_line;

@
@<|execute_fn|({\.{is.kanji.str\$}})@>=
procedure x_is_kanji_str;
label exit;
begin
    pop_lit_stk(pop_lit1, pop_typ1);
    if pop_typ1<> stk_str then
    begin print_wrong_stk_lit(pop_lit1,pop_typ1,stk_str);
	  push_lit_stk(0,stk_int);
    end else begin
	sp_ptr := str_start[pop_lit1];
        sp_end := str_start[pop_lit1+1];
	while sp_ptr<sp_end do begin
	    if str_pool[sp_ptr]>127 then begin
		push_lit_stk(1,stk_int);
		return;
	    end else begin
		incr(sp_ptr);
	    end;
        end;
	push_lit_stk(0,stk_int);
    end;
exit:end;  
@z
