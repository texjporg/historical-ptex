% Change file for BibTeX in C, originally by Howard Trickey, for
% Berkeley Unix.
% 
% History:
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
% 03/09/90      `int' is a bad variable name.
% (more recent changes in ../ChangeLog.W2C)
%                                    
%  5/ 2/89      Version 0.20 of JBiBTeX by Shouichi Matsui
% 12/25/90      Version 0.30 of JBibTeX by Shouichi Matsui
%  1/ 1/91      last update for JBibTeX 0.30 by Shouichi Matsui
% 10/30/92      last update for JBibTeX 0.31 for bug fix by Shouichi Matsui
% 11/02/94      Version 0.32 for use with web2c 6.1, by Takafumi Sakurai
% 
%
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
@d banner=='This is JBibTeX 0.32 based on BibTeX 0.99c' {printed when the program starts}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [2] `term_in' and `term_out' are standard input and output.  But
% there is a complication: BibTeX passes `term_out' to some routines as
% a var parameter.  web2c turns a var parameter f into &f at the calling
% side -- and stdout is sometimes implemented as `&_iob[1]' or some
% such.  An address of an address is invalid.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@d term_out == tty
@d term_in == tty
@y
@d term_out == standard_output
@d term_in == standard_input
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [4] Turn debug..gubed et al. into #ifdef's.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x 
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
@d incr(#) == #:=#+1    {increase a variable by unity}
@d decr(#) == #:=#-1    {decrease a variable by unity}
@y
{These are defined as C macros}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [10] Don't print the banner unless verbose. JBibTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
initialize;
print_ln(banner);@/
@y
init_kanji;
initialize;
if verbose then begin
  print (banner);
  print_ln (version_string);
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [10] Possibly exit with bad status.  It doesn't seem worth it to move
% the definitions of the |history| values to above this module; hence
% the 1.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
exit_program:
end.
@y
exit_program:
if (history > 1) then uexit (history);
end.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [11] Remove compiler directives.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@{@&$C-,A+,D-@}  {no range check, catch arithmetic overflow, no debug overhead}
@!debug @{@&$C+,D+@}@+ gubed            {but turn everything on when debugging}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [13] Remove nonlocal goto.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
    goto exit_program;
@y
    uexit (1);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [14] Increase some constants.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@!buf_size=1000; {maximum number of characters in an input line (or string)}
@y
@!buf_size=3000; {maximum number of characters in an input line (or string)}
@z
@x
@!max_bib_files=20; {maximum number of \.{.bib} files allowed}
@y
@!max_bib_files=1000; {maximum number of \.{.bib} files allowed}
@z
@x
@!pool_size=65000; {maximum number of characters in strings}
@y
@!pool_size=512000; {maximum number of characters in strings}
@z
@x
@!max_strings=4000; {maximum number of strings, including pre-defined;
                                                        must be |<=hash_size|}
@y
@!max_strings=20000; {maximum number of strings, including pre-defined;
                                                        must be |<=hash_size|}
@z
@x
@!max_cites=750; {maximum number of distinct cite keys; must be
                                                        |<=max_strings|}
@y
@!max_cites=3000; {maximum number of distinct cite keys; must be
                                                        |<=max_strings|}
@z
@x
@!min_crossrefs=2; {minimum number of cross-refs required for automatic
                                                        |cite_list| inclusion}
@y
{|min_crossrefs| can be set at runtime now.}
@z
@x changes for JBibTeX
@!wiz_fn_space=3000; {maximum amount of |wiz_defined|-function space}
@y
@!wiz_fn_space=3400; {maximum amount of |wiz_defined|-function space}
@z
@x
@!max_ent_ints=3000; {maximum number of |int_entry_var|s
                                        (entries $\times$ |int_entry_var|s)}
@y
@!max_ent_ints=25000; {maximum number of |int_entry_var|s
                                        (entries $\times$ |int_entry_var|s)}
@z
@x
@!max_ent_strs=3000; {maximum number of |str_entry_var|s
                                        (entries $\times$ |str_entry_var|s)}
@y
@!max_ent_strs=10000; {maximum number of |str_entry_var|s
                                        (entries $\times$ |str_entry_var|s)}
@z
@x
@!max_fields=17250; {maximum number of fields (entries $\times$ fields,
                                        about |23*max_cites| for consistency)}
@y
@!max_fields=69000; {maximum number of fields (entries $\times$ fields,
                                        about |23*max_cites| for consistency)}
@z
@x
@d hash_size=5000       {must be |>= max_strings| and |>= hash_prime|}
@y
@d hash_size=21000       {must be |>= max_strings| and |>= hash_prime|}
@z
@x
@d hash_prime=4253      {a prime number about 85\% of |hash_size| and |>= 128|
                                                and |< @t$2^{14}-2^6$@>|}
@y
@d hash_prime=16319      {a prime number about 85\% of |hash_size| and |>= 128|
                                                and |< @t$2^{14}-2^6$@>|}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [15] Use the system constant.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@d file_name_size=40    {file names shouldn't be longer than this}
@y
@d file_name_size == PATH_MAX
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [22, 23, 27, 28] Allow any character as input.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x [22]
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36] Define `alpha_file' in C.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@!alpha_file=packed file of text_char;  {files that contain textual data}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [37] Can't do arithmetic with |file_name_size| any longer, sigh.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@!name_ptr:0..file_name_size+1;         {index variable into |name_of_file|}
@y
@!name_ptr:integer;         {index variable into |name_of_file|}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [38] File opening.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [39] Do file closing in C.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [77] The predefined string array starts at zero instead of one.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
for i:=1 to len do
    buffer[i] := xord[pds[i]];
@y
for i:=1 to len do
    buffer[i] := xord[pds[i-1]];
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [97] Can't do this tangle-time arithmetic.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@!aux_name_length : 0..file_name_size+1;        {\.{.aux} name sans extension}
@y
@!aux_name_length : integer;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [100] Reading the aux file name and command-line processing.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
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
var @<Variables for possible command-line processing@>@/
begin
loop
    begin
    {initialize the path variables}
    set_paths (BIB_INPUT_PATH_BIT + BST_INPUT_PATH_BIT);
    if (argc > 1) then
        @<Process a possible command line@>
      else begin
        write (term_out,'Please type input file name (no extension)--');
        aux_name_length := 0;
        while (not eoln(term_in)) do begin
            if (aux_name_length = file_name_size) then begin
                readln(term_in);
                sam_you_made_the_file_name_too_long;
            end;
            name_of_file[aux_name_length+1] := getc(term_in);
            incr(aux_name_length);
        end;
        if (eof(term_in)) then begin
            writeln(term_out);
            writeln(term_out,
                    'Unexpected end of file on terminal---giving up!');
            uexit(1);
        end;
        readln(term_in);
      end;
    @<Handle this \.{.aux} name@>;
aux_not_found:
    argc := 0;
    end;
aux_found:                      {now we're ready to read the \.{.aux} file}
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [101] Don't need this variable; we use argc to check if we have a
% command line.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@<Variables for possible command-line processing@>=
@!check_cmnd_line : boolean;    {|true| if we're to check the command line}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [102] Get the aux file name from the command line.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@<Process a possible command line@>=
begin
do_nothing;             {the ``default system'' doesn't use the command line}
end
@y
@<Process a possible command line@>=
begin
  if argc > n_options + arg_options + 3
  then begin
    print ('Usage: bibtex ');
    print ('[-verbose] ');
    print ('[-min-crossrefs=<number>] ');
    print_ln ('[<aux file>].');
@.Usage: ...@>
    uexit (1);
  end;

  @<Initialize the option variables@>;
  @<Parse arguments@>;

  argv (optind, name_of_file);
  aux_name_length := 1;
  while name_of_file[aux_name_length] <> ' '
    do incr (aux_name_length);
  decr (aux_name_length);
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [106] Don't use a path to find the aux file.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
if (not a_open_in(cur_aux_file)) then
    sam_you_made_the_file_name_wrong;
@y
if (not a_open_in(cur_aux_file,no_file_path)) then
    sam_you_made_the_file_name_wrong;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [110] Be silent unless verbose.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
print ('The top-level auxiliary file: ');
print_aux_name;
@y
if verbose then begin
  print ('The top-level auxiliary file: ');
  print_aux_name;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [123] Use BIBINPUTS to search for the .bib file.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
if (not a_open_in(cur_bib_file)) then
    begin
    add_area (s_bib_area);
    if (not a_open_in(cur_bib_file)) then
        open_bibdata_aux_err ('I couldn''t open database file ');
    end;
@y
if (not a_open_in(cur_bib_file,BIB_INPUT_PATH)) then
        open_bibdata_aux_err ('I couldn''t open database file ');
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [127] Use BSTINPUTS/TEXINPUTS to search for .bst files.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
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
add_extension (s_bst_extension);
if (not a_open_in(bst_file,BST_INPUT_PATH)) then
    begin
        print ('I couldn''t open style file ');
        print_bst_name;@/
        bst_str := 0;                           {mark as unused again}
        aux_err_return;
    end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [127] Be silent unless verbose.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
print ('The style file: ');
print_bst_name;
@y
if verbose then begin
  print ('The style file: ');
  print_bst_name;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [141] Don't use a path to search for subsidiary aux files, either.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
if (not a_open_in(cur_aux_file)) then
@y
if (not a_open_in(cur_aux_file, no_file_path)) then
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [151] This goto gets turned into a setjmp/longjmp by ./convert --
% unfortunately, it is a nonlocal goto.  ekrell@ulysses@att.com
% implemented the conversion.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [198] A variable named `int' is no good in C.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@<Procedures and functions for handling numbers, characters, and strings@>=
@y
@d int == the_int

@<Procedures and functions for handling numbers, characters, and strings@>=
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [223] Be silent unless verbose.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
    print ('Database file #',bib_ptr+1:0,': ');
    print_bib_name;@/
@y
    if verbose then begin
      print ('Database file #',bib_ptr+1:0,': ');
      print_bib_name;
    end;
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [388] bibtex.web has mutually exclusive tests here; Oren said he
% doesn't want to fix it until 1.0, since it's obviously of no practical
% import (or someone would have found it before GCC 2 did).  Changing
% the second `and' to an `or' makes all but the last of multiple authors
% be omitted in the bbl file, so I simply removed the statement.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
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

This macro tests if its argument is the current option, as represented
by the index variable |option_index|.

@d argument_is (#) == (strcmp (long_options[option_index].name, #) = 0)

@<Parse arguments@> =
begin
  @<Define the option table@>;
  repeat
    getopt_return_val := getopt_long_only (argc, gargv, '', long_options,
                                           address_of_int (option_index));
    if getopt_return_val <> -1
    then begin
      if getopt_return_val = "?"
      then uexit (1); {|getopt| has already given an error message.}

      if argument_is ('min-crossrefs')
      then begin
        min_crossrefs := atoi (optarg);
      end
      
      else
        {It was just a flag; |getopt| has already done the assignment.}
        do_nothing;

    end;
  until getopt_return_val = -1;

  {Now |optind| is the index of first non-option on the command line.}
end


@ The array of information we pass in.  The type |getopt_struct| is
defined in C, to avoid type clashes.  We also need to know the return
value from getopt, and the index of the current option.

@<Variables for possible command-line...@> =
@!long_options: array[0..n_options] of getopt_struct;
@!getopt_return_val: integer;
@!option_index: c_int_type;
@!current_option: 0..n_options;

@ Here is the first of the options we allow.
@.-verbose@>

@<Define the option...@> =
current_option := 0;
long_options[0].name := 'verbose';
long_options[0].has_arg := 0;
long_options[0].flag := address_of_int (verbose);
long_options[0].val := 1;
incr (current_option);

@ The global variable |verbose| determines whether or not we print
progress information.

@<Glob...@> =
@!verbose: c_int_type;

@ It starts off |false|.

@<Initialize the option...@> =
verbose := false;


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


@ An element with all zeros always ends the list.

@<Define the option...@> =
long_options[current_option].name := 0;
long_options[current_option].has_arg := 0;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;


@ Pascal compilers won't count the number of elements in an array
constant for us.  This doesn't include the zero-element at the end,
because this array starts at index zero.

@<Constants...@> =
@!n_options = 2;
@!arg_options = 1;


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
