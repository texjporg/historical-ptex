% Change file for BibTeX in C, originally by Howard Trickey.
% 
% 05/28/84      Initial implementation, version 0.41 of BibTeX
% 07/01/84      Version 0.41a of BibTeX.
% 12/17/84      Version 0.97c of BibTeX.
% 02/12/85      Version 0.98c of BibTeX.
% 02/25/85      Newer version 0.98c of BibTeX.
% 03/25/85      Version 0.98f of BibTeX
% 05/23/85      Version 0.98i of BibTeX
% 02/11/88      Version 0.99b of BibTeX
% 04/04/88      Version 0.99c; converted for use with web2c (ETM).
% 11/30/89      Use FILENAMESIZE instead of 1024 (KB).
% 03/09/90	`int' is a bad variable name for C.
% (more recent changes in the ChangeLog)
%                                    
%  5/ 2/89      Version 0.20 of JBiBTeX by Shouichi Matsui
% 12/25/90      Version 0.30 of JBibTeX by Shouichi Matsui
%  1/ 1/91      last update for JBibTeX 0.30 by Shouichi Matsui
% 10/30/92      last update for JBibTeX 0.31 for bug fix by Shouichi Matsui
% 11/02/94      Version 0.32 for use with web2c 6.1, by Takafumi Sakurai
% 
%

% [0] Let bibtex.tex work with latest webmac (which defines \ET, hence
% making E active loses).
@x
\catcode`E=13 \uppercase{\def E{e}}
\def\\#1{\hbox{\let E=\drop\it#1\/\kern.05em}} % italic type for identifiers
@y
\let\maybe = \iffalse % process only changed sections
@z

@x only print chnages
\def\title{\BibTeX\ }
@y
\let\maybe=\iffalse
\def\title{J\BibTeX\ 0.32 Changes for C Version \BibTeX\ }
@z

@x
 \def\titlepage{F}
 \centerline{\:\titlefont The {\:\ttitlefont \BibTeX} preprocessor}
 \vskip 15pt \centerline{(Version 0.99c---\today)} \vfill}
@y
 \def\titlepage{F}
 \centerline{\:\titlefont The {\:\ttitlefont J\BibTeX} preprocessor}
 \vskip 15pt \centerline{(Version 0.32 base on C Version \BibTeX 0.99c---\today)} \vfill}
@z
  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1] banner
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@d banner=='This is BibTeX, Version 0.99c' {printed when the program starts}
@y
@d banner=='This is JBibTeX, Version 0.99c-j0.33'
  {printed when the program starts}
@z

% [2] `term_in' and `term_out' are standard input and output.  But
% there is a complication: BibTeX passes `term_out' to some routines as
% a var parameter.  web2c turns a var parameter f into &f at the calling
% side -- and stdout is sometimes implemented as `&_iob[1]' or some
% such.  An address of an address is invalid. Therefore, we define
% variables `standardinput' and `standardoutput' in openinout.c.
@x
@d term_out == tty
@d term_in == tty
@y
@d term_out == standard_output
@d term_in == standard_input

@<Globals in the outer block@>=
standard_input, standard_output: text;
@z

@x [4] Turn debug..gubed et al. into #ifdef's.
@d debug == @{          { remove the `|@{|' when debugging }
@d gubed == @t@>@}      { remove the `|@}|' when debugging }
@f debug == begin
@f gubed == end
@#
@d stat == @{           { remove the `|@{|' when keeping statistics }
@d tats == @t@>@}       { remove the `|@}|' when keeping statistics }
@f stat == begin
@f tats == end
@#
@d trace == @{          { remove the `|@{|' when in |trace| mode }
@d ecart == @t@>@}      { remove the `|@}|' when in |trace| mode }
@f trace == begin
@f ecart == end
@y
@d debug == ifdef('TEXMF_DEBUG')
@d gubed == endif('TEXMF_DEBUG')
@f debug == begin
@f gubed == end
@#
@d stat == ifndef('NO_BIBTEX_STAT')
@d tats == endifn('NO_BIBTEX_STAT')
@f stat==begin
@f tats==end
@#
@d trace == ifdef@&('TRACE')
@d ecart == endif@&('TRACE')
@f trace == begin
@f ecart == end
@z

@x [10] Don't print the banner unless verbose, and initialize dynamic arrays.
begin
initialize;
print_ln(banner);@/
@y
@<Define |parse_arguments|@>
begin
standard_input := stdin;
standard_output := stdout;
@#
max_ent_ints := MAX_ENT_INTS;
max_ent_strs := MAX_ENT_STRS;
pool_size := POOL_SIZE;
max_bib_files := MAX_BIB_FILES;
max_fields := MAX_FIELDS;
@#
{Add one to the sizes because the Pascal arrays start at 1, not 0.}
bib_file   := XTALLOC (max_bib_files + 1, alpha_file);
bib_list   := XTALLOC (max_bib_files + 1, str_number);
entry_ints := XTALLOC (max_ent_ints + 1, integer);
entry_strs := XTALLOC ((max_ent_strs + 1) * (ent_str_size + 1), ASCII_code);
wiz_functions := XTALLOC (wiz_fn_space + 1, hash_ptr2);
field_info := XTALLOC (max_fields + 1, str_number);
s_preamble := XTALLOC (max_bib_files + 1, str_number);
str_pool   := XTALLOC (pool_size + 1, ASCII_code);
@#
initialize;
if verbose then begin
  print (banner);
  print_ln (version_string);
end;
@z

% [10] Possibly exit with bad status.  It doesn't seem worth it to move
% the definitions of the |history| values to above this module; hence the 1.
@x
exit_program:
end.
@y
exit_program:
if (history > 1) then uexit (history);
end.
@z

@x [13] Remove nonlocal goto.
    goto exit_program;
@y
    uexit (1);
@z

@x [14] Increase some constants, and uppercase others for dynamic arrays.
@<Constants in the outer block@>=
@y
@<Constants in the outer block@>=
@!hash_prime = 30011;   {a prime number about 85\% of |hash_size| and |>= 128|}
@!hash_size = 35307;    {must be |>= max_strings| and |>= hash_prime|}
@!hash_base = empty + 1;                {lowest numbered hash-table location}
@!hash_max = hash_base + hash_size - 1; {highest numbered hash-table location}
@!hash_maxp1 = hash_max + 1;         {because we need a scalar constant later}
@!max_hash_value = hash_prime + hash_prime - 2 + 127; {|h|'s maximum value}
@!quote_next_fn = hash_base - 1;  {special marker used in defining functions}
@!end_of_def = hash_max + 1;      {another such special marker}
@!undefined = hash_max + 1;       {a special marker used for |type_list|}
@z

@x [still 14]
@!buf_size=1000; {maximum number of characters in an input line (or string)}
@y
@!buf_size=3000; {maximum number of characters in an input line (or string)}
@z

@x [still 14]
@!max_bib_files=20; {maximum number of \.{.bib} files allowed}
@!pool_size=65000; {maximum number of characters in strings}
@!max_strings=4000; {maximum number of strings, including pre-defined;
                                                        must be |<=hash_size|}
@!max_cites=750; {maximum number of distinct cite keys; must be
                                                        |<=max_strings|}
@!min_crossrefs=2; {minimum number of cross-refs required for automatic
                                                        |cite_list| inclusion}
@!wiz_fn_space=3000; {maximum amount of |wiz_defined|-function space}
@y [still 14]
@!MAX_BIB_FILES=20; {initial number of \.{.bib} files allowed}
@!POOL_SIZE=65000; {initial number of characters in strings}
@!max_strings=35000; {maximum number of strings, including pre-defined;
                                                        must be |<=hash_size|}
@!max_cites=5000; {maximum number of distinct cite keys; must be
                                                        |<=max_strings|}
@!WIZ_FN_SPACE=3400; {initial amount of |wiz_defined|-function space}
{|min_crossrefs| can be set at runtime now.}
@z

@x [still 14] handle long citation strings
@!max_ent_ints=3000; {maximum number of |int_entry_var|s
                                        (entries $\times$ |int_entry_var|s)}
@!max_ent_strs=3000; {maximum number of |str_entry_var|s
                                        (entries $\times$ |str_entry_var|s)}
@!ent_str_size=100; {maximum size of a |str_entry_var|; must be |<=buf_size|}
@!glob_str_size=1000; {maximum size of a |str_global_var|;
                                                        must be |<=buf_size|}
@!max_fields=17250; {maximum number of fields (entries $\times$ fields,
@y
@!MAX_ENT_INTS=3000; {initial number of |int_entry_var|s
                                        (entries $\times$ |int_entry_var|s)}
@!MAX_ENT_STRS=3000; {initial number of |str_entry_var|s
                                        (entries $\times$ |str_entry_var|s)}
@!ent_str_size=250; {maximum size of a |str_entry_var|; must be |<=buf_size|}
@!glob_str_size=3000; {maximum  size of a |str_global_var|;
                                                        must be |<=buf_size|}
@!MAX_FIELDS=5000; {initial number of fields (entries $\times$ fields,
@z

@x [15] Increase more constants in the web defines.
@d hash_size=5000       {must be |>= max_strings| and |>= hash_prime|}
@d hash_prime=4253      {a prime number about 85\% of |hash_size| and |>= 128|
                                                and |< @t$2^{14}-2^6$@>|}
@d file_name_size=40    {file names shouldn't be longer than this}
@d max_glob_strs=10     {maximum number of |str_global_var| names}
@y
{|hash_size| and |hash_prime| are |const| constants now.}
@d max_glob_strs=20     {maximum number of |str_global_var| names
                         \.{James.Ashton@keating.anu.edu.au} says his
                         indxcite package needs at least 15 here.}
@#
@d file_name_size==maxint {file names have no arbitrary maximum length}
@#
{For dynamic allocation.}
@d x_entry_strs_tail(#) == (#)]
@d x_entry_strs(#) == entry_strs[(#) * (ent_str_size+1) + x_entry_strs_tail
@z

@x [16] Add new variables-that-used-to-be-constants for dynamic arrays.
@<Globals in the outer block@>=
@y
@<Globals in the outer block@>=
@!max_ent_ints: integer;
@!max_ent_strs: integer;
@!pool_size: integer;
@!max_bib_files: integer;
@!wiz_fn_space: integer;
@!max_fields: integer;
@z

@x [17] Remove painfully small upper bound on hash_prime
if (hash_prime >= (16384-64)) then              bad:=10*bad+6;
@y
@z

@x [22, 23, 27, 28] Allow any character as input. [22]
@!ASCII_code=0..127;    {seven-bit numbers}
@y
@!ASCII_code=0..255;    {eight-bit numbers}
@z

@x [23]
@d text_char == char    {the data type of characters in text files}
@d first_text_char=0    {ordinal number of the smallest element of |text_char|}
@d last_text_char=127   {ordinal number of the largest element of |text_char|}

@<Local variables for initialization@>=
i:0..last_text_char;    {this is the first one declared}
@y
@d text_char == ASCII_code    {the data type of characters in text files}
@d first_text_char=0    {ordinal number of the smallest element of |text_char|}
@d last_text_char=255   {ordinal number of the largest element of |text_char|}

@<Local variables for initialization@>=
i:integer;
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
@d period = "."                 {these are three}
@d question_mark = "?"          {string-ending characters}
@d exclamation_mark = "!"       {of interest in \.{add.period\$}}
@y
@d period = "."                 {these are three}
@d question_mark = "?"          {string-ending characters}
@d exclamation_mark = "!"       {of interest in \.{add.period\$}}
@d zen_pun_first = 161          {Zenkaku punctuation first byte; in EUC}
@d zen_space = 161              {Zenkaku space first, second byte; in EUC}
@d zen_kuten = 162              {Zenkaku kuten second byte; in EUC}
@d zen_ten = 163                {Zenkaku ten second byte; in EUC}
@d zen_comma = 164              {Zenkaku comman second byte; in EUC}
@d zen_period = 165             {Zenkaku period second byte; in EUC}
@d zen_question = 169           {Zenkaku question mark second byte; in EUC}
@d zen_exclamation = 170        {Zenkaku exclamation mark second byte; in EUC}
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
lex_class[13] := white_space;
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

% [37] file_name_size no longer exists.  See comments in tex.ch for why
% we change the element type to text_char.
@x
@!name_of_file:packed array[1..file_name_size] of char;
                         {on some systems this is a \&{record} variable}
@!name_length:0..file_name_size;
  {this many characters are relevant in |name_of_file| (the rest are blank)}
@!name_ptr:0..file_name_size+1;         {index variable into |name_of_file|}
@y
@!name_of_file:^text_char;
@!name_length:integer;
  {this many characters are relevant in |name_of_file| }
@!name_ptr:integer;         {index variable into |name_of_file|}
@z

@x [38] File opening.
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
function erstat(var f:file):integer; extern;    {in the runtime library}
@#@t\2@>
function a_open_in(var f:alpha_file):boolean;   {open a text file for input}
begin reset(f,name_of_file,'/O'); a_open_in:=reset_OK(f);
end;
@#
function a_open_out(var f:alpha_file):boolean;  {open a text file for output}
begin rewrite(f,name_of_file,'/O'); a_open_out:=rewrite_OK(f);
end;
@y
@ File opening will be done in C.
@d no_file_path = -1
@z

@x [39] Do file closing in C.
@<Procedures and functions for file-system interacting@>=
procedure a_close(var f:alpha_file);            {close a text file}
begin close(f);
end;
@y
File closing will be done in C, too.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47] input for JBibTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
  while (not eoln(f)) do
    begin
    if (last >= buf_size) then
        buffer_overflow;
    buffer[last]:=xord[f^];
    get(f); incr(last);
    end;
  get(f);
@y
  if (not input_line(f)) then
    buffer_overflow;
@z

@x [48] Dynamically allocate str_pool.
@!str_pool : packed array[pool_pointer] of ASCII_code;  {the characters}
@y
@!str_pool : ^ASCII_code;  {the characters}
@z

@x [49] pool_size is a variable now, so can't be used as a constant.
@!pool_pointer = 0..pool_size;  {for variables that point into |str_pool|}
@y
@!pool_pointer = integer;  {for variables that point into |str_pool|}
@z

@x [53] Reallocate str_pool.
overflow('pool size ',pool_size);
@y
BIB_XRETALLOC ('str_pool', str_pool, ASCII_code, pool_size,
               pool_size + POOL_SIZE);
@z

% [58] (start_name) reallocate name_of_file for the new name and
% terminate with null.
@x
name_ptr := 1;
@y
name_ptr := 1;
free (name_of_file);
name_of_file := xmalloc (length (file_name) + 2);
@z

@x
name_length := length(file_name);
@y
name_length := length(file_name);
name_of_file[name_length + 1] := 0;
@z

% [60] (add_extension) Don't pad name_of_file with blanks, terminate
% with null. And junk the overflow check, since Web2c can't translate
% the print statement properly and it can never happen, anyway.
@x
if (name_length + length(ext) > file_name_size) then
    begin
    print ('File=',name_of_file,', extension=');
    print_pool_str (ext); print_ln (',');
    file_nm_size_overflow;
    end;
@y
@z
@x
name_ptr := name_length+1;
while (name_ptr <= file_name_size) do   {pad with blanks}
    begin
    name_of_file[name_ptr] := ' ';
    incr(name_ptr);
    end;
@y
name_of_file[name_length + 1] := 0;
@z

@x [61] (add_area) Delete this print of name_of_file as well.
if (name_length + length(area) > file_name_size) then
    begin
    print ('File=');
    print_pool_str (area); print (name_of_file,',');
    file_nm_size_overflow;
    end;
@y
@z

@x [65] hash_base and hash_max are now Pascal consts, instead of web macros.
@d hash_base = empty + 1                {lowest numbered hash-table location}
@d hash_max = hash_base + hash_size - 1 {highest numbered hash-table location}
@y
@z
@x
@!hash_used : hash_base..hash_max+1;    {allocation pointer for hash table}
@y
@!hash_used : hash_base..hash_maxp1;    {allocation pointer for hash table}
@z

@x [68] This is const now.
@d max_hash_value = hash_prime+hash_prime-2+127         {|h|'s maximum value}
@y
@z

@x [77] The predefined string array starts at zero instead of one.
for i:=1 to len do
    buffer[i] := xord[pds[i]];
@y
for i:=1 to len do
    buffer[i] := xord[pds[i-1]];
@z

@x [97] Can't do this tangle-time arithmetic with file_name_size.
@!aux_name_length : 0..file_name_size+1;        {\.{.aux} name sans extension}
@y
@!aux_name_length : integer;
@z

@x [100] Reading the aux file name and command-line processing.
This procedure consists of a loop that reads and processes a (nonnull)
\.{.aux} file name.  It's this module and the next two that must be
changed on those systems using command-line arguments.  Note: The
|term_out| and |term_in| files are system dependent.

@<Procedures and functions for the reading and processing of input files@>=
procedure get_the_top_level_aux_file_name;
label aux_found,@!aux_not_found;
var @<Variables for possible command-line processing@>@/
begin
check_cmnd_line := false;                       {many systems will change this}
loop
    begin
    if (check_cmnd_line) then
        @<Process a possible command line@>
      else
        begin
        write (term_out,'Please type input file name (no extension)--');
        if (eoln(term_in)) then                 {so the first |read| works}
            read_ln (term_in);
        aux_name_length := 0;
        while (not eoln(term_in)) do
            begin
            if (aux_name_length = file_name_size) then
                begin
                while (not eoln(term_in)) do    {discard the rest of the line}
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
aux_found:                      {now we're ready to read the \.{.aux} file}
end;
@y
@<Procedures and functions for the reading and processing of input files@>=
procedure get_the_top_level_aux_file_name;
label aux_found,@!aux_not_found;
begin
  @<Process a possible command line@>
  {Leave room for the \.., the extension, the junk byte at the
   beginning, and the null byte at the end.}
  init_kanji;
  name_of_file := xmalloc (strlen (cmdline (optind)) + 4 + 2);
  strcpy (name_of_file + 1, cmdline (optind));
  aux_name_length := strlen (name_of_file + 1);
  @<Handle this \.{.aux} name@>;
aux_not_found:  uexit (1);
aux_found:                      {now we're ready to read the \.{.aux} file}
end;
@z

% [101] Don't need this variable; we use argc to check if we have a
% command line.
@x
@<Variables for possible command-line processing@>=
@!check_cmnd_line : boolean;    {|true| if we're to check the command line}
@y
@z

@x [102] Get the aux file name from the command line.
@<Process a possible command line@>=
begin
do_nothing;             {the ``default system'' doesn't use the command line}
end
@y
@<Process a possible command line@>=
kpse_set_progname (argv[0]);
parse_arguments;
@z

% [106] Don't use a path to find the aux file, and don't add the
% extension if it's already there.
@x
add_extension (s_aux_extension);        {this also sets |name_length|}
aux_ptr := 0;                           {initialize the \.{.aux} file stack}
if (not a_open_in(cur_aux_file)) then
    sam_you_made_the_file_name_wrong;
@y
if strcmp (name_of_file + 1 + name_length - 3, 'aux') <> 0 then
  add_extension (s_aux_extension);        {this also sets |name_length|}
aux_ptr := 0;                           {initialize the \.{.aux} file stack}
if (not a_open_in(cur_aux_file,no_file_path)) then
    sam_you_made_the_file_name_wrong;
@z

@x [110] Be silent unless verbose.
print ('The top-level auxiliary file: ');
print_aux_name;
@y
if verbose then begin
  print ('The top-level auxiliary file: ');
  print_aux_name;
end;
@z

@x [117] bib_list is dynamically allocated.
@!bib_list : array[bib_number] of str_number;   {the \.{.bib} file list}
@y
@!bib_list : ^str_number;   {the \.{.bib} file list}
@z
@x [still 117] bib_file also.
@!bib_file : array[bib_number] of alpha_file; {corresponding |file| variables}
@y
@!bib_file : ^alpha_file; {corresponding |file| variables}
@z

@x [118] max_bib_files is a variable now, so can't be used as a const.
@!bib_number = 0..max_bib_files;        {gives the |bib_list| range}
@y
@!bib_number = integer;        {gives the |bib_list| range}
@z

@x [123] Reallocate when we run out of bib files.
    overflow('number of database files ',max_bib_files);
@y
begin
  BIB_XRETALLOC ('bib_list', bib_list, str_number, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
  {Already increased |max_bib_files|, so don't need to do it again.}
  BIB_XRETALLOC ('bib_file', bib_file, alpha_file, max_bib_files,
                 max_bib_files);
  BIB_XRETALLOC ('s_preamble', s_preamble, str_number, max_bib_files,
                 max_bib_files);
end;
@z

@x [still 123] Use BIBINPUTS to search for the .bib file.
add_extension (s_bib_extension);
if (not a_open_in(cur_bib_file)) then
    begin
    add_area (s_bib_area);
    if (not a_open_in(cur_bib_file)) then
        open_bibdata_aux_err ('I couldn''t open database file ');
    end;
@y
if (not a_open_in(cur_bib_file, kpse_bib_format)) then
        open_bibdata_aux_err ('I couldn''t open database file ');
@z

@x [127] Use BSTINPUTS/TEXINPUTS to search for .bst files.
add_extension (s_bst_extension);
if (not a_open_in(bst_file)) then
    begin
    add_area (s_bst_area);
    if (not a_open_in(bst_file)) then
        begin
        print ('I couldn''t open style file ');
        print_bst_name;@/
        bst_str := 0;                           {mark as unused again}
        aux_err_return;
        end;
    end;
@y
if (not a_open_in(bst_file, kpse_bst_format)) then
    begin
        print ('I couldn''t open style file ');
        print_bst_name;@/
        bst_str := 0;                           {mark as unused again}
        aux_err_return;
    end;
@z

@x [127] Be silent unless verbose.
print ('The style file: ');
print_bst_name;
@y
if verbose then begin
  print ('The style file: ');
  print_bst_name;
end;
@z

% [141] Don't pad with blanks.
% Don't use a path to search for subsidiary aux files, either.
@x
while (name_ptr <= file_name_size) do   {pad with blanks}
    begin
    name_of_file[name_ptr] := ' ';
    incr(name_ptr);
    end;
if (not a_open_in(cur_aux_file)) then
@y
name_of_file[name_ptr] := 0;
if (not a_open_in(cur_aux_file, no_file_path)) then
@z

% [151] This goto gets turned into a setjmp/longjmp by ./convert --
% unfortunately, it is a nonlocal goto.  ekrell@ulysses.att.com
% implemented the conversion.
@x
buf_ptr2 := last;       {to get the first input line}
loop
    begin
    if (not eat_bst_white_space) then   {the end of the \.{.bst} file}
        goto bst_done;
    get_bst_command_and_process;
    end;
bst_done: a_close (bst_file);
@y
buf_ptr2 := last;       {to get the first input line}
hack1;
    begin
    if (not eat_bst_white_space) then   {the end of the \.{.bst} file}
        hack2;
    get_bst_command_and_process;
    end;
bst_done: a_close (bst_file);
@z

% max_ent_ints, max_ent_strs, max_fields are no longer const.
@x [160] quote_next_fn and end_of_def are Pascal consts, instead of web macros.
@d quote_next_fn = hash_base - 1  {special marker used in defining functions}
@d end_of_def = hash_max + 1      {another such special marker}

@<Types in the outer block@>=
@!fn_class = 0..last_fn_class;          {the \.{.bst} function classes}
@!wiz_fn_loc = 0..wiz_fn_space;  {|wiz_defined|-function storage locations}
@!int_ent_loc = 0..max_ent_ints;        {|int_entry_var| storage locations}
@!str_ent_loc = 0..max_ent_strs;        {|str_entry_var| storage locations}
@!str_glob_loc = 0..max_glb_str_minus_1; {|str_global_var| storage locations}
@!field_loc = 0..max_fields;            {individual field storage locations}
@y
@<Types in the outer block@>=
@!fn_class = 0..last_fn_class;          {the \.{.bst} function classes}
@!wiz_fn_loc = integer;         {|wiz_defined|-function storage locations}
@!int_ent_loc = integer;        {|int_entry_var| storage locations}
@!str_ent_loc = integer;        {|str_entry_var| storage locations}
@!str_glob_loc = 0..max_glb_str_minus_1; {|str_global_var| storage locations}
@!field_loc = integer;            {individual field storage locations}
@z

@x Dynamically allocate wiz_functions.
@!wiz_functions : packed array[wiz_fn_loc] of hash_ptr2;
@y
@!wiz_functions : ^hash_ptr2;
@z

% [still 161] Convert entry_ints and entry_strs to dynamically-allocated
% one-dimensional arrays; too bad C and Pascal lag Fortran in supporting
% run-time dimensioning of multidimensional arrays.  Other changes that
% follow this one will convert every reference to entry_strs[p][q] to
% x_entry_strs(p)(q), the equivalent of entry_strs[p*(ent_str_size+1) +
% q], but hidden inside a macro to mask the addressing computation.
% Although WEB does not have multi-argument macros, webman.tex shows how
% to get the equivalent effect.
@x
@!entry_ints : array[int_ent_loc] of integer;
@!num_ent_ints : int_ent_loc;   {the number of distinct |int_entry_var| names}
@!str_ent_ptr : str_ent_loc;    {general |str_entry_var| location}
@!entry_strs : array[str_ent_loc] of
                                packed array[0..ent_str_size] of ASCII_code;
@y
@!entry_ints : ^integer; {dynamically-allocated array}
@!num_ent_ints : int_ent_loc;   {the number of distinct |int_entry_var| names}
@!str_ent_ptr : str_ent_loc;    {general |str_entry_var| location}
@!entry_strs : ^ASCII_code; {dynamically-allocated array}
@z

@x [still 161] Dynamically allocate field_info.
@!field_info : packed array[field_loc] of str_number;
@y
@!field_info : ^str_number;
@z

@x [198] A variable named `int' is no good in C.
@<Procedures and functions for handling numbers, characters, and strings@>=
@y
@d int == the_int
@<Procedures and functions for handling numbers, characters, and strings@>=
@z

@x [200] Reallocate if out of wizard space.
if (single_ptr + wiz_def_ptr > wiz_fn_space) then
    begin
    print (single_ptr + wiz_def_ptr : 0,': ');
    overflow('wizard-defined function space ',wiz_fn_space);
    end;
@y
if (single_ptr + wiz_def_ptr > wiz_fn_space) then
    begin
    BIB_XRETALLOC ('wiz_functions', wiz_functions, hash_ptr2,
                    wiz_fn_space, wiz_fn_space + WIZ_FN_SPACE);
    end;
@z

@x [220] undefined is now a Pascal const, instead of a web macro
@d undefined = hash_max + 1     {a special marker used for |type_list|}
@y
@z

@x [223] Be silent unless verbose.
    print ('Database file #',bib_ptr+1:0,': ');
    print_bib_name;@/
@y
    if verbose then begin
      print ('Database file #',bib_ptr+1:0,': ');
      print_bib_name;
    end;
@z

@x [226] Reallocate if out of fields.
procedure check_field_overflow (@!total_fields : integer);
begin
if (total_fields > max_fields) then
    begin
    print_ln (total_fields:0,' fields:');
    overflow('total number of fields ',max_fields);
@y
procedure check_field_overflow (@!total_fields : integer);
var @!f_ptr: field_loc;
    @!start_fields: field_loc;
begin
if (total_fields > max_fields) then
    begin
    start_fields := max_fields;
    BIB_XRETALLOC ('field_info', field_info, str_number, max_fields,
                   total_fields + MAX_FIELDS);
    {Initialize to |missing|.}
    for f_ptr := start_fields to max_fields do begin
      field_info[f_ptr] := missing;
    end;
@z

@x [242] Reallocate when we run out of s_preamble's.
    bib_err ('You''ve exceeded ',max_bib_files:0,' preamble commands');
@y
begin
  BIB_XRETALLOC ('bib_list', bib_list, str_number, max_bib_files,
                 max_bib_files + MAX_BIB_FILES);
  {Already increased |max_bib_files|, so don't need to do it again.}
  BIB_XRETALLOC ('bib_file', bib_file, alpha_file, max_bib_files,
                 max_bib_files);
  BIB_XRETALLOC ('s_preamble', s_preamble, str_number, max_bib_files,
                 max_bib_files);
end;
@z

@x [264] Add check for fieldinfo[] overflow.
field_ptr := entry_cite_ptr * num_fields + fn_info[field_name_loc];
@y
field_ptr := entry_cite_ptr * num_fields + fn_info[field_name_loc];
check_field_overflow(field_ptr);
@z

@x [278] Add check for fieldinfo[] overflow.
@<Add cross-reference information@>=
begin
@y
@<Add cross-reference information@>=
begin
check_field_overflow((num_cites - 1) * num_fields + crossref_num);
@z

@x [280] Add check for fieldinfo[] overflow.
@<Subtract cross-reference information@>=
begin
@y
@<Subtract cross-reference information@>=
begin
check_field_overflow((num_cites - 1) * num_fields + crossref_num);
@z

@x [286] Add check for fieldinfo[] overflow.
@<Slide this cite key down to its permanent spot@>=
begin
@y
@<Slide this cite key down to its permanent spot@>=
begin
check_field_overflow((cite_xptr + 1) * num_fields);
@z

@x [287] Reallocate on overflow.
if (num_ent_ints*num_cites > max_ent_ints) then
    begin
    print (num_ent_ints*num_cites,': ');
    overflow('total number of integer entry-variables ',max_ent_ints);
    end;
@y
if (num_ent_ints*num_cites > max_ent_ints) then
  BIB_XRETALLOC ('entry_ints', entry_ints, integer, max_ent_ints,
                 (num_ent_ints + 1) * (num_cites + 1));
@z

@x [288] Reallocate entry_strs.
if (num_ent_strs*num_cites > max_ent_strs) then
    begin
    print (num_ent_strs*num_cites,': ');
    overflow('total number of string entry-variables ',max_ent_strs);
    end;
@y
{Have to include the maximum size of each string in the reallocation,
 unfortunately, since we're faking a two-dimensional array. And then
 decrease |max_ent_strs| again, because it's the number of strings, not
 the number of characters (which is what we're allocating.)}
if (num_ent_strs * num_cites > max_ent_strs) then begin
  BIB_XRETALLOC ('entry_strs', entry_strs, ASCII_code, max_ent_strs,
                 (num_ent_strs + 1) * (num_cites + 1) * (ent_str_size + 1));
  max_ent_strs := num_ent_strs * num_cites;
  {The new values are initialized in the next few statements from
  \.{bibtex.web}.}
end;
@z

@x [289] Macroize entry_strs[][].
    entry_strs[str_ent_ptr][0] := end_of_string;
@y
    x_entry_strs(str_ent_ptr)(0) := end_of_string;
@z

@x [302] Macroize entry_strs[][].
    char1 := entry_strs[ptr1][char_ptr];
    char2 := entry_strs[ptr2][char_ptr];
@y
    char1 := x_entry_strs(ptr1)(char_ptr);
    char2 := x_entry_strs(ptr2)(char_ptr);
@z

@x Changes for JBibTeX by Shouichi Matsui [322]
procedure add_out_pool (@!p_str : str_number);
var break_ptr : buf_pointer;    {the first character following the line break}
@!end_ptr : buf_pointer;        {temporary end-of-buffer pointer}
@y
procedure add_out_pool (@!p_str : str_number);
var break_ptr : buf_pointer;    {the first character following the line break}
@!end_ptr : buf_pointer;        {temporary end-of-buffer pointer}
@!in_mid_kanji:boolean; {flag if |max_print_line|-1 is in mid kanji}
@!last_char_ptr : buf_pointer;
@!t : buf_pointer;
@z

@x Changes for JBibTeX by Shouichi Matsui [324]
@<Break that unbreakable line@>=
begin
out_buf[end_ptr] := out_buf[max_print_line-1];  {save this character}
out_buf[max_print_line-1] := comment;           {so \TeX\ does the thing right}
out_buf_length := max_print_line;
break_ptr := out_buf_length - 1;        {the `|-1|' allows for the restoration}
output_bbl_line;                                {output what we can,}
out_buf[max_print_line-1] := out_buf[end_ptr];  {restore this character}
out_buf_ptr := 0;
tmp_ptr := break_ptr;
while (tmp_ptr < end_ptr) do                    {and slide the rest down}
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
out_buf[end_ptr] := out_buf[last_char_ptr-1];   {save this character}
out_buf[last_char_ptr-1] := comment;            {so \TeX\ does the thing right}
out_buf_length := last_char_ptr;
break_ptr := out_buf_length - 1;        {the `|-1|' allows for the restoration}
output_bbl_line;                                {output what we can,}
out_buf[last_char_ptr-1] := out_buf[end_ptr];   {restore this character}
out_buf_ptr := 0;
tmp_ptr := break_ptr;
while (tmp_ptr < end_ptr) do                    {and slide the rest down}
    begin
    out_buf[out_buf_ptr] := out_buf[tmp_ptr];
    incr(out_buf_ptr);
    incr(tmp_ptr);
    end;
out_buf_length := end_ptr - break_ptr;
end
@z

@x [328] Add check for fieldinfo[] overflow.
    field_ptr := cite_ptr*num_fields + fn_info[ex_fn_loc];
@y
    field_ptr := cite_ptr*num_fields + fn_info[ex_fn_loc];
    check_field_overflow(field_ptr);
@z

@x [330] Macroize entry_strs[][]
    while (entry_strs[str_ent_ptr][ex_buf_ptr] <> end_of_string) do
                                        {copy characters into the buffer}
        append_ex_buf_char (entry_strs[str_ent_ptr][ex_buf_ptr]);
@y
    while (x_entry_strs(str_ent_ptr)(ex_buf_ptr) <> end_of_string) do
                                        {copy characters into the buffer}
        append_ex_buf_char (x_entry_strs(str_ent_ptr)(ex_buf_ptr));
@z

@x Changes for JBibTeX by Shouichi Matsui [332]
@!b_while : hash_loc;           {\.{while\$}}
@!b_width : hash_loc;           {\.{width\$}}
@!b_write : hash_loc;           {\.{write\$}}
@!b_default : hash_loc;         {either \.{skip\$} or \.{default.type}}
@y
@!b_while : hash_loc;           {\.{while\$}}
@!b_width : hash_loc;           {\.{width\$}}
@!b_write : hash_loc;           {\.{write\$}}
@!b_default : hash_loc;         {either \.{skip\$} or \.{default.type}}
@!b_is_kanji_str : hash_loc;    {\.{is.kanji.str\$}}
@z

@x Changes for JBibTeX by Shouichi Matsui [334]
@d n_while = 34         {\.{while\$}}
@d n_width = 35         {\.{width\$}}
@d n_write = 36         {\.{write\$}}

@<Constants in the outer block@>=
@!num_blt_in_fns = 37;  {one more than the previous number}
@y
@d n_while = 34         {\.{while\$}}
@d n_width = 35         {\.{width\$}}
@d n_write = 36         {\.{write\$}}
@d n_is_kanji_str = 37  {\.{is.kanji.str\$}}

@<Constants in the outer block@>=
@!num_blt_in_fns = 38;  {one more than the previous number}
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

@x [337] s_preamble is dynamically allocated.
@!s_preamble : array[bib_number] of str_number;
@y
@!s_preamble : ^str_number;
@z

@x Changes for JBibTeX by Shouichi Matsui [342]
    n_width :           x_width;
    n_write :           x_write;
    othercases confusion ('Unknown built-in function')
endcases;
end
@y
    n_width :           x_width;
    n_write :           x_write;
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

@x [358] Macroize entry_strs[][].
    while (sp_ptr < sp_xptr1) do
        begin                   {copy characters into |entry_strs|}
        entry_strs[str_ent_ptr][ent_chr_ptr] := str_pool[sp_ptr];
        incr(ent_chr_ptr);
        incr(sp_ptr);
        end;
    entry_strs[str_ent_ptr][ent_chr_ptr] := end_of_string;
@y
    while (sp_ptr < sp_xptr1) do
        begin                   {copy characters into |entry_strs|}
        x_entry_strs(str_ent_ptr)(ent_chr_ptr) := str_pool[sp_ptr];
        incr(ent_chr_ptr);
        incr(sp_ptr);
        end;
    x_entry_strs(str_ent_ptr)(ent_chr_ptr) := end_of_string;
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
if (ex_buf_ptr < ex_buf_length) then            {remove the ``and''}
    ex_buf_ptr := ex_buf_ptr - 4;
@y
if (ex_buf_ptr < ex_buf_length) then    {remove the ``and'', or Zenkau comma}
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
            @<See if we have an ``and''@>;      {if so, |and_found := true|}
        preceding_white := false;
        end;
@y
    "a", "A" :
        begin
        incr(ex_buf_ptr);
        if (preceding_white) then
            @<See if we have an ``and''@>;      {if so, |and_found := true|}
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
@x Changes for JBibTeX by Shouichi Matsui for Zenkaku comma[385]
    othercases
        if (lex_class[ex_buf[ex_buf_ptr]] = white_space) then
            begin
            incr(ex_buf_ptr);
            preceding_white := true;
            end
        else
            begin
            incr(ex_buf_ptr);
            preceding_white := false;
            end
  endcases;
check_brace_level (pop_lit_var);
end;
@y
    othercases
        if (lex_class[ex_buf[ex_buf_ptr]] = white_space) then
            begin
            incr(ex_buf_ptr);
            preceding_white := true;
            end
        else
            begin
                if( ex_buf[ex_buf_ptr] > 127 ) then
                        ex_buf_ptr := ex_buf_ptr +2
                else
                        incr(ex_buf_ptr);
            preceding_white := false;
            end
  endcases;
check_brace_level (pop_lit_var);
end;
@z

% [388] bibtex.web has mutually exclusive tests here; Oren said he
% doesn't want to fix it until 1.0, since it's obviously of no practical
% import (or someone would have found it before GCC 2 did).  Changing
% the second `and' to an `or' makes all but the last of multiple authors
% be omitted in the bbl file, so I simply removed the statement.
@x
while ((ex_buf_xptr < ex_buf_ptr) and
                        (lex_class[ex_buf[ex_buf_ptr]] = white_space) and
                        (lex_class[ex_buf[ex_buf_ptr]] = sep_char)) do
        incr(ex_buf_xptr);                      {this removes leading stuff}
@y
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
while (sp_ptr < sp_end) do                      {shift the substring}
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

while (sp_ptr < sp_end) do                      {shift the substring}
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

% [460] Eliminate unreferenced statement label, because `undefined' is
% now a constant expression that is not evaluated at the Web level. If
% this label were ever required, it could be replaced by the constant
% 9997, which is not used as a statement label in BibTeX.
@x
    undefined : trace_pr ('unknown')
@y
    trace_pr ('unknown')
@z

@x [461] Macroize entry_strs[][].
        while (entry_strs[str_ent_ptr][ent_chr_ptr] <> end_of_string) do
            begin
            trace_pr (xchr[entry_strs[str_ent_ptr][ent_chr_ptr]]);
            incr(ent_chr_ptr);
            end;
@y
        while (x_entry_strs(str_ent_ptr)(ent_chr_ptr) <> end_of_string) do
            begin
            trace_pr (xchr[x_entry_strs(str_ent_ptr)(ent_chr_ptr)]);
            incr(ent_chr_ptr);
            end;
@z

@x [463] Add check for fieldinfo[] overflow.
@<Print fields@>=
begin
if (not read_performed) then
    trace_pr_ln ('    uninitialized')
  else
    begin
    field_ptr := cite_ptr * num_fields;
    field_end_ptr := field_ptr + num_fields;
@y
@<Print fields@>=
begin
if (not read_performed) then
    trace_pr_ln ('    uninitialized')
  else
    begin
    field_ptr := cite_ptr * num_fields;
    field_end_ptr := field_ptr + num_fields;
    check_field_overflow(field_end_ptr);
@z

@x [467] System-dependent changes.
This section should be replaced, if necessary, by changes to the program
that are necessary to make \BibTeX\ work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@y
@d argument_is (#) == (strcmp (long_options[option_index].name, #) = 0)

@<Define |parse_arguments|@> =
procedure parse_arguments;
const n_options = 5; {Pascal won't count array lengths for us.}
var @!long_options: array[0..n_options] of getopt_struct;
    @!getopt_return_val: integer;
    @!option_index: c_int_type;
    @!current_option: 0..n_options;
begin
  @<Initialize the option variables@>;
  @<Define the option table@>;
  repeat
    getopt_return_val := getopt_long_only (argc, argv, '', long_options,
                                           address_of (option_index));
    if getopt_return_val = -1 then begin
      {End of arguments; we exit the loop below.} ;

    end else if getopt_return_val = "?" then begin
      usage ('jbibtex');

    end else if argument_is ('min-crossrefs') then begin
      min_crossrefs := atoi (optarg);
      
    end else if argument_is ('help') then begin
      usage_help (JBIBTEX_HELP);

    end else if argument_is ('version') then begin
      print_version_and_exit (banner, 'Oren Patashnik', nil);

    end else if argument_is ('kanji') then begin
      @<Set process kanji code@>;

    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;

  {Now |optind| is the index of first non-option on the command line.
   We must have one remaining argument.}
  if (optind + 1 <> argc) then begin
    write_ln (stderr, 'jbibtex: Need exactly one file argument.');
    usage ('jbibtex');
  end;
end;

@ Here is the first of the options we allow.
@.-terse@>

@<Define the option...@> =
current_option := 0;
long_options[0].name := 'terse';
long_options[0].has_arg := 0;
long_options[0].flag := address_of (verbose);
long_options[0].val := 0;
incr (current_option);

@ The global variable |verbose| determines whether or not we print
progress information.

@<Glob...@> =
@!verbose: c_int_type;

@ Start off |true|, to match the default behavior.

@<Initialize the option...@> =
verbose := true;

@ Here is an option to change the minimum number of cross-refs required
for automatic |cite_list| inclusion.
@.-min-crossrefs@>

@<Define the option...@> =
long_options[current_option].name := 'min-crossrefs';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@
@<Glob...@> =
@!min_crossrefs: integer;

@ Set |min_crossrefs| to two by default, so we match the
documentation (\.{btxdoc.tex}).

@<Initialize the option...@> =
min_crossrefs := 2;

@ One of the standard options.
@.-help@>

@<Define the option...@> =
long_options[current_option].name := 'help';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Another of the standard options.
@.-version@>

@<Define the option...@> =
long_options[current_option].name := 'version';
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr (current_option);

@ Kanji option.
@.-kanji@>

@<Define the option...@> =
long_options[current_option].name := 'kanji';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr(current_option);

@ An element with all zeros always ends the list.

@<Define the option...@> =
long_options[current_option].name := 0;
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;


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

@ kanji code.

@d jis_enc==0
@d euc_enc==1
@d sjis_enc==2

@ @<Glob...@>=
@!proc_kanji_code:jis_enc..sjis_enc;

@ @<Initialize the option...@> =
ifdef('OUTJIS')  proc_kanji_code:=jis_enc; endif('OUTJIS')@/
ifdef('OUTEUC')  proc_kanji_code:=euc_enc; endif('OUTEUC')@/
ifdef('OUTSJIS') proc_kanji_code:=sjis_enc; endif('OUTSJIS')@/

@ @<Set process kanji code@>=
  if strcmp(optarg, 'jis') = 0 then
    proc_kanji_code:=jis_enc
  else if strcmp(optarg, 'euc') = 0 then
    proc_kanji_code:=euc_enc
  else if strcmp(optarg, 'sjis') = 0 then
    proc_kanji_code:=sjis_enc
  else
    print_ln('Bad kanjicode encoding', optarg, '.');
@z
