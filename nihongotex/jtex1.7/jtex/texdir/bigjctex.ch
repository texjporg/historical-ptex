% JapaneseTeX change file for ASCII Kanji UNIX. (Shift JIS code only)
%	by Ryoichi Kurasawa (ryoi-k@ascii.co.jp)
%	   Hisato  Hamano (hisato-h@ascii.co.jp), ASCII Corporation.
%	
% Change file for TeX in C, derived from various other change files
% by Tim Morgan, UC Irvine ICS Department, and many others
%

% Modification History:
%
% (5 /28/86) ETM Started with TeX 2.0
% (7 /3/87)  ETM Brought up to TeX 2.2
% (9 /26/87) ETM Brought up to TeX 2.3
% (10/1/87)  ETM Brought up to TeX 2.5
% (12/21/87) ETM Brought up to TeX 2.7
% (1 /14/88) ETM Brought up to TeX 2.9
% (2 /20/88) PAM Revised format and module numbers
% (3 /1 /88) ETM Eliminated some unused variables and unnecesary tests

% NOTE: the module numbers in this change file refer to the published
%	text in TeX, the Program, Volume B. 1986

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [0] WEAVE: only print changes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x Tell WEAVE to print only the changes:
  \def\?##1]{\hbox to 1in{\hfil##1.\ }}
  }
@y
  \def\?##1]{\hbox{Changes to \hbox to 1em{\hfil##1}.\ }}
  }
\let\maybe=\iffalse
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1.2] banner line
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d banner=='This is TeX, Version 2.99' {printed when \TeX\ starts}
@y
@d banner=='This is BigTeX, C Version 2.99 - j1.7' {printed when \TeX\ starts}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1.4] program header
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
Actually the heading shown here is not quite normal: The |program| line
does not mention any |output| file, because \ph\ would ask the \TeX\ user
to specify a file name if |output| were specified here.
@^system dependencies@>

@d mtype==t@&y@&p@&e {this is a \.{WEB} coding trick:}
@f mtype==type {`\&{mtype}' will be equivalent to `\&{type}'}
@f type==true {but `|type|' will not be treated as a reserved word}

@p @t\4@>@<Compiler directives@>@/
program TEX; {all file names are defined dynamically}
label @<Labels in the outer block@>@/
@y

@d mtype==t@&y@&p@&e {this is a \.{WEB} coding trick:}
@d standard_input==i@&n@&p@&u@&t {and another}
@f mtype==type {`\&{mtype}' will be equivalent to `\&{type}'}
@f type==true {but `|type|' will not be treated as a reserved word}

@p @t\4@>@<Compiler directives@>@/
program TEX; {all file names are defined dynamically}
@z

@x
@<Labels in the out...@>=
start_of_TEX@t\hskip-2pt@>, end_of_TEX@t\hskip-2pt@>,@,final_end;
  {key control points}
@y
@<Labels in the outer block@>=
start_of_TEX@t\hskip-2pt@>, end_of_TEX@t\hskip-2pt@>,@,final_end;
  {key control points}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1.7] debug..gubed, stat..tats
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Here we change these WEB symbols, which are used much as #ifdef's
% in C, into something which will get translated into actual #ifdef's.
@x
@d debug==@{ {change this to `$\\{debug}\equiv\null$' when debugging}
@d gubed==@t@>@} {change this to `$\\{gubed}\equiv\null$' when debugging}
@y
@d debug==ifdef('DEBUG')
@d gubed==endif('DEBUG')
@z
@x
@d stat==@{ {change this to `$\\{stat}\equiv\null$' when gathering
  usage statistics}
@d tats==@t@>@} {change this to `$\\{tats}\equiv\null$' when gathering
  usage statistics}
@y
@d stat==ifdef('STAT')
@d tats==endif('STAT')
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1.8] init..tini
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d init== {change this to `$\\{init}\equiv\.{@@\{}$' in the production version}
@d tini== {change this to `$\\{tini}\equiv\.{@@\}}$' in the production version}
@y
@d init==ifdef('INITEX')
@d tini==endif('INITEX')
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1.9] compiler directives
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@{@&$C-,A+,D-@} {no range check, catch arithmetic overflow, no debug overhead}
@!debug @{@&$C+,D+@}@+ gubed {but turn everything on when debugging}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1.11] compile-time constants, use logical names
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% doubled mem_max for C since UCI people are memory hogs
% made dvi_buf_size=2*8192 for better BSD Unix I/O performance
% enlarged various other datastructures
@x
@!mem_max=30000; {greatest index in \TeX's internal |mem| array;
  must be strictly less than |max_halfword|;
  must be equal to |mem_top| in \.{INITEX}, otherwise |>=mem_top|}
@!mem_min=0; {smallest index in \TeX's internal |mem| array;
  must be |min_halfword| or more;
  must be equal to |mem_bot| in \.{INITEX}, otherwise |<=mem_bot|}
@!buf_size=500; {maximum number of characters simultaneously present in
  current lines of open files and in control sequences between
  \.{\\csname} and \.{\\endcsname}; must not exceed |max_halfword|}
@!error_line=72; {width of context lines on terminal error messages}
@!half_error_line=42; {width of first lines of contexts in terminal
  error messages; should be between 30 and |error_line-15|}
@!max_print_line=79; {width of longest text lines output; should be at least 60}
@!stack_size=200; {maximum number of simultaneous input sources}
@!max_in_open=6; {maximum number of input files and error insertions that
  can be going on simultaneously}
@!font_max=75; {maximum internal font number; must not exceed |max_quarterword|
  and must be at most |font_base+256|}
@!font_mem_size=20000; {number of words of |font_info| for all fonts}
@!param_size=60; {maximum number of simultaneous macro parameters}
@!nest_size=40; {maximum number of semantic levels simultaneously active}
@!max_strings=3000; {maximum number of strings; must not exceed |max_halfword|}
@!string_vacancies=8000; {the minimum number of characters that should be
  available for the user's control sequences and font names,
  after \TeX's own error messages are stored}
@!pool_size=32000; {maximum number of characters in strings, including all
  error messages and help texts, and the names of all fonts and
  control sequences; must exceed |string_vacancies| by the total
  length of \TeX's own strings, which is currently about 23000}
@!save_size=600; {space for saving values outside of current group; must be
  at most |max_halfword|}
@!trie_size=8000; {space for hyphenation patterns; should be larger for
  \.{INITEX} than it is in production versions of \TeX}
@!dvi_buf_size=800; {size of the output buffer; must be a multiple of 8}
@!file_name_size=40; {file names shouldn't be longer than this}
@!pool_name='TeXformats:TEX.POOL                     ';
  {string of length |file_name_size|; tells where the string pool appears}
@y
@!mem_max=327670; {greatest index in \TeX's internal |mem| array;
  must be strictly less than |max_halfword|;
  must be equal to |mem_top| in \.{INITEX}, otherwise |>=mem_top|}
@!mem_min=0; {smallest index in \TeX's internal |mem| array;
  must be |min_halfword| or more;
  must be equal to |mem_bot| in \.{INITEX}, otherwise |<=mem_bot|}
@!buf_size=4096; {maximum number of characters simultaneously present in
  current lines of open files and in control sequences between
  \.{\\csname} and \.{\\endcsname}; must not exceed |max_halfword|}
@!error_line=79; {width of context lines on terminal error messages}
@!half_error_line=50; {width of first lines of contexts in terminal
  error messages; should be between 30 and |error_line-15|}
@!max_print_line=79; {width of longest text lines output; should be at least 60}
@!stack_size=300; {maximum number of simultaneous input sources}
@!max_in_open=15; {maximum number of input files and error insertions that
  can be going on simultaneously}
@!font_max=255; {maximum internal font number; must not exceed |max_quarterword|
  and must be at most |font_base+256|}
@!font_mem_size=72000; {number of words of |font_info| for all fonts}
@!param_size=60; {maximum number of simultaneous macro parameters}
@!nest_size=40; {maximum number of semantic levels simultaneously active}
@!max_strings=6600; {maximum number of strings; must not exceed |max_halfword|}
@!string_vacancies=25000; {the minimum number of characters that should be
  available for the user's control sequences and font names,
  after \TeX's own error messages are stored}
@!pool_size=75000; {maximum number of characters in strings, including all
  error messages and help texts, and the names of all fonts and
  control sequences; must exceed |string_vacancies| by the total
  length of \TeX's own strings, which is currently about 23000}
@!save_size=4000; {space for saving values outside of current group; must be
  at most |max_halfword|}
@!trie_size=8000; {space for hyphenation patterns; should be larger for
  \.{INITEX} than it is in production versions of \TeX}
@!dvi_buf_size=16384; {size of the output buffer; must be a multiple of 8}
@!file_name_size=1024; {file names can't be longer than this in 4.2BSD}
@!pool_name='jtex.pool';
  {string of length |file_name_size|; tells where the string pool appears}
@!mem_top=327670; {largest index in the |mem| array dumped by \.{INITEX};
  must be substantially larger than |mem_bot|
  and not greater than |mem_max|}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1.12] sensitive compile-time constants
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% For C we change mem_base and hi_mem_base to reflect our use of
% 0-origin vs pc's use of negative-origin.  Despite the fact that
% these are "sensitive" constants, we're going to make mem_top a
% #define in the C code for readability and ease of modification
% (it's up above).  Beware!
@x
@d mem_bot=0 {smallest index in the |mem| array dumped by \.{INITEX};
  must not be less than |mem_min|}
@d mem_top==30000 {largest index in the |mem| array dumped by \.{INITEX};
  must be substantially larger than |mem_bot|
  and not greater than |mem_max|}
@d font_base=0 {smallest internal font number; must not be less
  than |min_quarterword|}
@d hash_size=2100 {maximum number of control sequences; it should be at most
  about |(mem_max-mem_min)/10|, but 2100 is already quite generous}
@d hash_prime=1777 {a prime number equal to about 85\% of |hash_size|}
@d hyph_size=307 {another prime; the number of \.{\\hyphenation} exceptions}
@y
@d mem_bot=0 {smallest index in the |mem| array dumped by \.{INITEX};
  must not be less than |mem_min|}
@d font_base=0 {smallest internal font number; must not be less
  than |min_quarterword|}
@d hash_size=9500 {maximum number of control sequences; it should be at most
  about |(mem_max-mem_min)/10|, so we can be really generous}
@d hash_prime=7919 {The thousandth in a list of 1000 primes.  Run the primes
  program in LiterateProgramming to find out.  It is reasonably close to
  85\% of a |hash_size| of 9500}
@d hyph_size=607 {another prime; the number of \.{\\hyphenation} exceptions}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Use C macros for incr()/decr()
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% By not defining these, they slip through into the C code, where we
% may be able to generate more efficient code.
@x
@d incr(#) == #:=#+1 {increase a variable by unity}
@d decr(#) == #:=#-1 {decrease a variable by unity}
@y
@d incr2(#) == #:=#+2
@d decr2(#) == #:=#-2
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [2.18] ASCII code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Unix really deals in 8-bit characters
@x
@ Characters of text that have been converted to \TeX's internal form
are said to be of type |ASCII_code|, which is a subrange of the integers.

@<Types...@>=
@!ASCII_code=0..127; {seven-bit numbers}

@ The original \PASCAL\ compiler was designed in the late 60s, when six-bit
@y
@ Characters of text that have been converted to \TeX's internal form
are said to be of type |ASCII_code|, which is a subrange of the integers.

@<Types...@>=
@!ASCII_code=0..255; {eight-bit numbers}
@!xchar=ASCII_code;

@ The original \PASCAL\ compiler was designed in the late 60s, when six-bit
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [2.19] first and last text char
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d text_char == char {the data type of characters in text files}
@d first_text_char=0 {ordinal number of the smallest element of |text_char|}
@d last_text_char=127 {ordinal number of the largest element of |text_char|}
@y
@d text_char == xchar {the data type of characters in text files}
@d first_text_char=0 {ordinal number of the smallest element of |text_char|}
@d last_text_char=255 {ordinal number of the largest element of |text_char|}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [2.20][2.21] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Glob...@>=
@!xord: array [text_char] of ASCII_code;
  {specifies conversion of input characters}
@!xchr: array [ASCII_code] of text_char;
  {specifies conversion of output characters}
@y
@<Glob...@>=
@z

@x
@<Set init...@>=
xchr[@'40]:=' ';
xchr[@'41]:='!';
xchr[@'42]:='"';
xchr[@'43]:='#';
xchr[@'44]:='$';
xchr[@'45]:='%';
xchr[@'46]:='&';
xchr[@'47]:='''';@/
xchr[@'50]:='(';
xchr[@'51]:=')';
xchr[@'52]:='*';
xchr[@'53]:='+';
xchr[@'54]:=',';
xchr[@'55]:='-';
xchr[@'56]:='.';
xchr[@'57]:='/';@/
xchr[@'60]:='0';
xchr[@'61]:='1';
xchr[@'62]:='2';
xchr[@'63]:='3';
xchr[@'64]:='4';
xchr[@'65]:='5';
xchr[@'66]:='6';
xchr[@'67]:='7';@/
xchr[@'70]:='8';
xchr[@'71]:='9';
xchr[@'72]:=':';
xchr[@'73]:=';';
xchr[@'74]:='<';
xchr[@'75]:='=';
xchr[@'76]:='>';
xchr[@'77]:='?';@/
xchr[@'100]:='@@';
xchr[@'101]:='A';
xchr[@'102]:='B';
xchr[@'103]:='C';
xchr[@'104]:='D';
xchr[@'105]:='E';
xchr[@'106]:='F';
xchr[@'107]:='G';@/
xchr[@'110]:='H';
xchr[@'111]:='I';
xchr[@'112]:='J';
xchr[@'113]:='K';
xchr[@'114]:='L';
xchr[@'115]:='M';
xchr[@'116]:='N';
xchr[@'117]:='O';@/
xchr[@'120]:='P';
xchr[@'121]:='Q';
xchr[@'122]:='R';
xchr[@'123]:='S';
xchr[@'124]:='T';
xchr[@'125]:='U';
xchr[@'126]:='V';
xchr[@'127]:='W';@/
xchr[@'130]:='X';
xchr[@'131]:='Y';
xchr[@'132]:='Z';
xchr[@'133]:='[';
xchr[@'134]:='\';
xchr[@'135]:=']';
xchr[@'136]:='^';
xchr[@'137]:='_';@/
xchr[@'140]:='`';
xchr[@'141]:='a';
xchr[@'142]:='b';
xchr[@'143]:='c';
xchr[@'144]:='d';
xchr[@'145]:='e';
xchr[@'146]:='f';
xchr[@'147]:='g';@/
xchr[@'150]:='h';
xchr[@'151]:='i';
xchr[@'152]:='j';
xchr[@'153]:='k';
xchr[@'154]:='l';
xchr[@'155]:='m';
xchr[@'156]:='n';
xchr[@'157]:='o';@/
xchr[@'160]:='p';
xchr[@'161]:='q';
xchr[@'162]:='r';
xchr[@'163]:='s';
xchr[@'164]:='t';
xchr[@'165]:='u';
xchr[@'166]:='v';
xchr[@'167]:='w';@/
xchr[@'170]:='x';
xchr[@'171]:='y';
xchr[@'172]:='z';
xchr[@'173]:='{';
xchr[@'174]:='|';
xchr[@'175]:='}';
xchr[@'176]:='~';@/
xchr[0]:=' '; xchr[@'177]:=' ';
  {ASCII codes 0 and |@'177| do not appear in text}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [2.23][2.24] form-feed and tab are legal characters
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@^character set dependencies@>
@^system dependencies@>

@<Set init...@>=
for i:=1 to @'37 do xchr[i]:=' ';
@y
@^character set dependencies@>
@^system dependencies@>

@<Set init...@>=
@z

@x
@<Set init...@>=
for i:=first_text_char to last_text_char do xord[chr(i)]:=invalid_code;
for i:=1 to @'176 do xord[xchr[i]]:=i;
@y
@<Set init...@>=
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.25] file types
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
The program actually makes use also of a third kind of file, called a
|word_file|, when dumping and reloading base information for its own
initialization.  We shall define a word file later; but it will be possible
for us to specify simple operations on word files before they are defined.

@y
I/O in C is done using standard I/O.  We will define the path numbers
in an include file for C which are used in searching for files to be
read.  We'll define all the file types in C also.
@z

@x
@!alpha_file=packed file of text_char; {files that contain textual data}
@!byte_file=packed file of eight_bits; {files that contain binary data}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.23] add real_name_of_file array
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
is crucial for our purposes. We shall assume that |name_of_file| is a variable
of an appropriate type such that the \PASCAL\ run-time system being used to
implement \TeX\ can open a file whose external name is specified by
|name_of_file|.
@^system dependencies@>

@<Glob...@>=
@!name_of_file:packed array[1..file_name_size] of char;@;@/
  {on some systems this may be a \&{record} variable}
@!name_length:0..file_name_size;@/{this many characters are actually
  relevant in |name_of_file| (the rest are blank)}
@y
is crucial for our purposes. We shall assume that |name_of_file| is a variable
of an appropriate type such that the \PASCAL\ run-time system being used to
implement \TeX\ can open a file whose external name is specified by
|name_of_file|.
The Berkeley {\mc UNIX} version uses search paths to look for files to open.
We use |real_name_of_file| to hold the |name_of_file| with a directory
name from the path in front of it.
@^system dependencies@>

@<Glob...@>=
@!name_of_file,@!real_name_of_file:packed array[1..file_name_size] of char;
@;@/
@!name_length:0..file_name_size;@/{this many characters are actually
  relevant in |name_of_file| (the rest are blank)}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.27] file opening
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The \ph\ compiler with which the present version of \TeX\ was prepared has
extended the rules of \PASCAL\ in a very convenient way. To open file~|f|,
we can write
$$\vbox{\halign{#\hfil\qquad&#\hfil\cr
|reset(f,@t\\{name}@>,'/O')|&for input;\cr
|rewrite(f,@t\\{name}@>,'/O')|&for output.\cr}}$$
The `\\{name}' parameter, which is of type `{\bf packed array
$[\langle\\{any}\rangle]$ of \\{char}}', stands for the name of
the external file that is being opened for input or output.
Blank spaces that might appear in \\{name} are ignored.

The `\.{/O}' parameter tells the operating system not to issue its own
error messages if something goes wrong. If a file of the specified name
cannot be found, or if such a file cannot be opened for some other reason
(e.g., someone may already be trying to write the same file), we will have
|@!erstat(f)<>0| after an unsuccessful |reset| or |rewrite|.  This allows
\TeX\ to undertake appropriate corrective action.
@:PASCAL H}{\ph@>
@^system dependencies@>

\TeX's file-opening procedures return |false| if no file identified by
|name_of_file| could be opened.

@d reset_OK(#)==erstat(#)=0
@d rewrite_OK(#)==erstat(#)=0

@p function a_open_in(var f:alpha_file):boolean;
  {open a text file for input}
begin reset(f,name_of_file,'/O'); a_open_in:=reset_OK(f);
end;
@#
function a_open_out(var f:alpha_file):boolean;
  {open a text file for output}
begin rewrite(f,name_of_file,'/O'); a_open_out:=rewrite_OK(f);
end;
@#
function b_open_in(var f:byte_file):boolean;
  {open a binary file for input}
begin reset(f,name_of_file,'/O'); b_open_in:=reset_OK(f);
end;
@#
function b_open_out(var f:byte_file):boolean;
  {open a binary file for output}
begin rewrite(f,name_of_file,'/O'); b_open_out:=rewrite_OK(f);
end;
@#
function w_open_in(var f:word_file):boolean;
  {open a word file for input}
begin reset(f,name_of_file,'/O'); w_open_in:=reset_OK(f);
end;
@#
function w_open_out(var f:word_file):boolean;
  {open a word file for output}
begin rewrite(f,name_of_file,'/O'); w_open_out:=rewrite_OK(f);
end;
@y
{All of these functions will be defined as macros in C.}
@z

@x
@p procedure a_close(var f:alpha_file); {close a text file}
begin close(f);
end;
@#
procedure b_close(var f:byte_file); {close a binary file}
begin close(f);
end;
@#
procedure w_close(var f:word_file); {close a word file}
begin close(f);
end;
@y
{And all of these as well.}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.31]Make input_ln an external C routine
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function input_ln(var f:alpha_file;@!bypass_eoln:boolean):boolean;
  {inputs the next line or returns |false|}
var last_nonblank:0..buf_size; {|last| with trailing blanks removed}
begin if bypass_eoln then if not eof(f) then get(f);
  {input the first character of the line into |f^|}
last:=first; {cf.\ Matthew 19\thinspace:\thinspace30}
if eof(f) then input_ln:=false
else  begin last_nonblank:=first;
  while not eoln(f) do
    begin if last>=max_buf_stack then
      begin max_buf_stack:=last+1;
      if max_buf_stack=buf_size then
        overflow("buffer size",buf_size);
@:TeX capacity exceeded buffer size}{\quad buffer size@>
      end;
    buffer[last]:=xord[f^]; get(f); incr(last);
    if buffer[last-1]<>" " then last_nonblank:=last;
    end;
  last:=last_nonblank; input_ln:=true;
  end;
end;
@y
We'll get inputln from an external C module, coded for efficiency
directly in C.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.32] term_in/out are input,output
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The user's terminal acts essentially like other files of text, except
that it is used both for input and for output. When the terminal is
considered an input file, the file variable is called |term_in|, and when it
is considered an output file the file variable is |term_out|.
@^system dependencies@>

@<Glob...@>=
@!term_in:alpha_file; {the terminal as an input file}
@!term_out:alpha_file; {the terminal as an output file}
@y
@ The user's terminal acts essentially like other files of text, except
that it is used both for input and for output. When the terminal is
considered an input file, the file is called |term_in|, and when it
is considered an output file the file is |term_out|.  In C, these
files will be defined as ``stdin'' and ``stdout,'' respectively.

@d term_in==stdin {the terminal as an input file}
@d term_out==stdout {the terminal as an output file}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.33] don't need to open terminal files
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ Here is how to open the terminal files
in \ph. The `\.{/I}' switch suppresses the first |get|.
@^system dependencies@>

@d t_open_in==reset(term_in,'TTY:','/O/I') {open the terminal for text input}
@d t_open_out==rewrite(term_out,'TTY:','/O') {open the terminal for text output}
@y
@ Here is how to open the terminal files.  t_open_out does nothing.
t_open_in, on the other hand, does the work of "rescanning", or getting
any command line arguments the user has provided.  It's coded in C
externally.

@d t_open_out == {output already open for text output}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.34] flushing output
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
these operations can be specified in \ph:
@^system dependencies@>

@d update_terminal == break(term_out) {empty the terminal output buffer}
@d clear_terminal == break_in(term_in,true) {clear the terminal input buffer}
@d wake_up_terminal == do_nothing {cancel the user's cancellation of output}
@y
these operations can be specified with {\mc UNIX}.  Update_terminal
does an fflush, since that's easy.  wake_up_terminal and clear_terminal
are specified in external C routines, if desired.  We call fflush()
termflush() here, and fix it with a macro in C, so we can cast it to (void)
to keep lint quiet.
@^system dependencies@>

@d update_terminal == termflush(term_out)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3.37] rescanning the command line 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The following program does the required initialization
without retrieving a possible command line.
It should be clear how to modify this routine to deal with command lines,
if the system permits them.
@^system dependencies@>

@p function init_terminal:boolean; {gets the terminal input started}
label exit;
begin t_open_in;
loop@+begin wake_up_terminal; write(term_out,'**'); update_terminal;
@.**@>
  if not input_ln(term_in,true) then {this shouldn't happen}
    begin write_ln(term_out);
    write(term_out,'! End of file on the terminal... why?');
@.End of file on the terminal@>
    init_terminal:=false; return;
    end;
  loc:=first;
  while (loc<last)and(buffer[loc]=" ") do incr(loc);
  if loc<last then
    begin init_terminal:=true;
    return; {return unless the line was all blank}
    end;
  write_ln(term_out,'Please type the name of your input file.');
  end;
exit:end;
@y
@ The following program does the required initialization.
Iff anything has been specified on the command line, then t_open_in
will return with last > first.
@^system dependencies@>

@p
function init_terminal:boolean; {gets the terminal input started}
label exit;

var
    i, j : integer;

begin
    t_open_in;
    if last > first then begin
        loc := first;
        while (loc < last) and (buffer[loc]=' ') do
	    incr(loc);
        if loc < last then begin
            init_terminal := true;
            goto exit;
        end;
    end;
    loop@+begin
        wake_up_terminal; write(term_out, '**'); update_terminal;
@.**@>
        if not input_ln(term_in,true) then begin {this shouldn't happen}
            write_ln(term_out);
            write(term_out, '! End of file on the terminal... why?');
@.End of file on the terminal@>
            init_terminal:=false;
	    return;
        end;

        loc:=first;
        while (loc<last)and(buffer[loc]=" ") do
            incr(loc);

        if loc<last then begin
           init_terminal:=true;
           return; {return unless the line was all blank}
        end;
        write_ln(term_out, 'Please type the name of your input file.');
    end;
exit:
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [4.47][4.48][4.49]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
label done,exit;
var k,@!l:0..127; {small indices or counters}
@y
label done,exit;
var k,@!l:halfword; {small indices or counters}
@z

@x
@<Make the first 128 strings@>;
@y
@<Make the first 256 strings@>;
@z


@x
@ @<Make the first 128...@>=
for k:=0 to 127 do
@y
@ @<Make the first 256...@>=
for k:=0 to 255 do
@z

@x
@<Character |k| cannot be printed@>=
  (k<" ")or(k>"~")
@y
@<Character |k| cannot be printed@>=
  (k<" ")or(k>252)or(k=127)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [4.51] a_open_in of pool file needs path specifier
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
name_of_file:=pool_name; {we needn't set |name_length|}
if a_open_in(pool_file) then
@y
vstrcpy(name_of_file+1, pool_name); {this is how you copy strings in C}
if a_open_in(pool_file,pool_path_spec) then
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [4.51,52,53] make TEX.POOL lowercase in messages
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else  bad_pool('! I can''t read TEX.POOL.')
@y
else begin
   {Like"bad_pool, but must not close file if we never opened it}
   wake_up_terminal; write_ln(term_out, '! I can''t read jtex.pool.');
   get_strings_started:= false; return;
end
@z

@x
@ @<Read one string...@>=
begin if eof(pool_file) then bad_pool('! TEX.POOL has no check sum.');
@.TEX.POOL has no check sum@>
read(pool_file,m,n); {read two digits of string length}
if m='*' then @<Check the pool check sum@>
else  begin if (xord[m]<"0")or(xord[m]>"9")or@|
      (xord[n]<"0")or(xord[n]>"9") then
    bad_pool('! TEX.POOL line doesn''t begin with two digits.');
@.TEX.POOL line doesn't...@>
  l:=xord[m]*10+xord[n]-"0"*11; {compute the length}
  if pool_ptr+l+string_vacancies>pool_size then
    bad_pool('! You have to increase POOLSIZE.');
@.You have to increase POOLSIZE@>
  for k:=1 to l do
    begin if eoln(pool_file) then m:=' '@+else read(pool_file,m);
    append_char(xord[m]);
    end;
  read_ln(pool_file); g:=make_string;
  end;
end

@ The \.{WEB} operation \.{@@\$} denotes the value that should be at the
end of this \.{TEX.POOL} file; any other value means that the wrong pool
file has been loaded.
@^check sum@>

@<Check the pool check sum@>=
begin a:=0; k:=1;
loop@+  begin if (xord[n]<"0")or(xord[n]>"9") then
  bad_pool('! TEX.POOL check sum doesn''t have nine digits.');
@.TEX.POOL check sum...@>
  a:=10*a+xord[n]-"0";
  if k=9 then goto done;
  incr(k); read(pool_file,n);
  end;
done: if a<>@$ then bad_pool('! TEX.POOL doesn''t match; TANGLE me again.');
@.TEX.POOL doesn't match@>
c:=true;
end
@y
@ @<Read one string...@>=
begin if eof(pool_file) then bad_pool('! jtex.pool has no check sum.');
@.TEX.POOL has no check sum@>
read(pool_file,m); read(pool_file,n); {read two digits of string length}
if m='*' then @<Check the pool check sum@>
else  begin if (xord(m)<"0")or(xord(m)>"9")or@|
      (xord(n)<"0")or(xord(n)>"9") then
    bad_pool('! jtex.pool line doesn''t begin with two digits.');
@.TEX.POOL line doesn't...@>
  l:=xord(m)*10+xord(n)-"0"*11; {compute the length}
  if pool_ptr+l+string_vacancies>pool_size then
    bad_pool('! You have to increase POOLSIZE.');
@.You have to increase POOLSIZE@>
  for k:=1 to l do
    begin if eoln(pool_file) then m:=' '@+else read(pool_file,m);
    append_char(xord(m));
    end;
  read_ln(pool_file); g:=make_string;
  end;
end

@ The \.{WEB} operation \.{@@\$} denotes the value that should be at the
end of this \.{TEX.POOL} file; any other value means that the wrong pool
file has been loaded.
@^check sum@>

@<Check the pool check sum@>=
begin a:=0; k:=1;
loop@+  begin if (xord(n)<"0")or(xord(n)>"9") then
  bad_pool('! jtex.pool check sum doesn''t have nine digits.');
@.TEX.POOL check sum...@>
  a:=10*a+xord(n)-"0";
  if k=9 then goto done;
  incr(k); read(pool_file,n);
  end;
done: if a<>@$ then bad_pool('! jtex.pool doesn''t match; TANGLE me again.');
@.TEX.POOL doesn't match@>
c:=true;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5.54][5.55] trick_buf
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!trick_count: integer; {threshold for pseudoprinting, explained later}
@!first_count: integer; {another variable for pseudoprinting}

@ @<Initialize the output routines@>=
selector:=term_only; tally:=0; term_offset:=0; file_offset:=0;
@y
@!trick_count: integer; {threshold for pseudoprinting, explained later}
@!first_count: integer; {another variable for pseudoprinting}
@!trick_buf2: array[0..error_line] of 0..2;
@!kcode_pos: 0..2;

@ @<Initialize the output routines@>=
selector:=term_only; tally:=0; term_offset:=0; file_offset:=0;
kcode_pos:=0;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5.57] kcode_pos
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% トークンリストを表示するときなどに, 漢字の 1 バイト目と 2 バイト目の間で
% 改行しないようにする.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
procedure print_ln; {prints an end-of-line}
begin case selector of
term_and_log: begin wterm_cr; wlog_cr;
  term_offset:=0; file_offset:=0;
  end;
log_only: begin wlog_cr; file_offset:=0;
  end;
term_only: begin wterm_cr; term_offset:=0;
  end;
no_print,pseudo,new_string: do_nothing;
othercases write_ln(write_file[selector])
endcases;@/
end; {|tally| is not affected}
@y
procedure print_ln; {prints an end-of-line}
begin
  case selector of
  term_and_log: begin
      if kcode_pos=1 then begin wterm(' '); wlog(' '); end;
      wterm_cr; wlog_cr; term_offset:=0; file_offset:=0;
    end;
  log_only: begin
      if kcode_pos=1 then wlog(' ');
      wlog_cr; file_offset:=0;
    end;
  term_only: begin
      if kcode_pos=1 then wterm(' ');
      wterm_cr; term_offset:=0;
    end;
  no_print,pseudo,new_string: do_nothing;
  othercases write_ln(write_file[selector])
  endcases;@/
  kcode_pos:=0;
end; {|tally| is not affected}
@z


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5.58] pseudo print
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%term_and_log: begin wterm(xchr[s]); wlog(xchr[s]);
%@y
%term_and_log: begin wterm(xchr(s)); wlog(xchr(s));
%@z
%@x
%log_only: begin wlog(xchr[s]); incr(file_offset);
%@y
%log_only: begin wlog(xchr(s)); incr(file_offset);
%@z
%@x
%term_only: begin wterm(xchr[s]); incr(term_offset);
%@y
%term_only: begin wterm(xchr(s)); incr(term_offset);
%@z
%
%@x
%pseudo: if tally<trick_count then trick_buf[tally mod error_line]:=s;
%@y
%pseudo: begin
%  if kcode_pos=0 then begin if iskanji(s) then kcode_pos:=1; end
%    else if kcode_pos=1 then begin kcode_pos:=2; end
%      else kcode_pos:=0;
%  if tally<trick_count then begin
%    trick_buf[tally mod error_line]:=s;
%    trick_buf2[tally mod error_line]:=kcode_pos;
%    end;
%  end;
%@z
%
%@x
%othercases write(write_file[selector],xchr[s])
%@y
%othercases write(write_file[selector],xchr(s))
%@z
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5.58] pseudo print
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% トークンリストを表示するときなどに, 漢字の 1 バイト目と 2 バイト目の間で
% 改行しないようにする.
% kcode_pos, trick_buf2[] の値の計算.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
case selector of
term_and_log: begin wterm(xchr[s]); wlog(xchr[s]);
  incr(term_offset); incr(file_offset);
  if term_offset=max_print_line then
    begin wterm_cr; term_offset:=0;
    end;
  if file_offset=max_print_line then
    begin wlog_cr; file_offset:=0;
    end;
  end;
log_only: begin wlog(xchr[s]); incr(file_offset);
  if file_offset=max_print_line then print_ln;
  end;
term_only: begin wterm(xchr[s]); incr(term_offset);
  if term_offset=max_print_line then print_ln;
  end;
no_print: do_nothing;
pseudo: if tally<trick_count then trick_buf[tally mod error_line]:=s;
new_string: begin if pool_ptr<pool_size then append_char(s);
  end; {we drop characters if the string space is full}
othercases write(write_file[selector],xchr[s])
endcases;@/
@y
if kcode_pos=1 then kcode_pos:=2
 else if iskanji(s) then kcode_pos:=1
 else kcode_pos:=0;
case selector of
term_and_log: begin
    if kcode_pos=1 then begin
      if term_offset>=max_print_line-1 then begin wterm_cr; term_offset:=0; end;
      if file_offset>=max_print_line-1 then begin wlog_cr; file_offset:=0; end;
    end;
    wterm(xchr(s)); incr(term_offset);
    if term_offset=max_print_line then begin wterm_cr; term_offset:=0; end;
    wlog(xchr(s)); incr(file_offset);
    if file_offset=max_print_line then begin wlog_cr; file_offset:=0; end;
  end;
log_only: begin
    if (kcode_pos=1)and(file_offset>=max_print_line-1) then begin
      wlog_cr; file_offset:=0;
    end;
    wlog(xchr(s)); incr(file_offset);
    if file_offset=max_print_line then print_ln;
  end;
term_only: begin
    if (kcode_pos=1)and(term_offset>=max_print_line-1) then begin
      wterm_cr; term_offset:=0;
    end;
    wterm(xchr(s)); incr(term_offset);
    if term_offset=max_print_line then print_ln;
  end;
no_print: do_nothing;
pseudo:
  if tally<trick_count then begin
    trick_buf[tally mod error_line]:=s;
    trick_buf2[tally mod error_line]:=kcode_pos;
  end;
new_string: begin
    if pool_ptr<pool_size then append_char(s);
  end; {we drop characters if the string space is full}
othercases
  write(write_file[selector],xchr(s))
endcases;@/
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5.59][5.60] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else if s<128 then
  if s<0 then s:="???" {can't happen}
  else if (@<Character |s| is the current new-line character@>) then
    if selector<pseudo then
@y
else if s<256 then
  if s<0 then s:="???" {can't happen}
  else if (@<Character |s| is the current new-line character@>) then
    if selector<pseudo then
@z

@x
else if s<128 then
  if s<0 then s:="???" {can't happen}
  else if (@<Character |s| is the current new-line character@>) then
    if selector<pseudo then
      begin print_ln; return;
@y
else if s<256 then
  if s<0 then s:="???" {can't happen}
  else if (@<Character |s| is the current new-line character@>) then
    if selector<pseudo then
      begin print_ln; return;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [6.61] opening message
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%wterm(banner);
%if format_ident=0 then wterm_ln(' (no format preloaded)')
%else  begin print(format_ident); print_ln;
%  end;
%@y
%if (format_ident=" (INITEX)") or (format_ident=0) then wterm(banner)
%else wterm_ln(banner);
%if format_ident=0 then wterm_ln(' (no format preloaded)')
%else  begin print(format_ident); print_ln;
%  end;
%@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [6.81] Eliminate nonlocal goto
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% C cannot do non-local goto's.
% Plus, it's nicer just to do an exit with the appropriate status code
% under Unix.  We call it "uexit" because there's a WEB symbol called
% "exit" already.  We use a C macro to change uexit() back to exit().
@x
@<Error hand...@>=
procedure jump_out;
begin goto end_of_TEX;
end;
@y
@d do_final_end==begin
   update_terminal;
   ready_already:=0;
   if (history <> spotless) and (history <> warning_issued) then
       uexit(1)
   else
       uexit(0);
   end
@<Error hand...@>=
procedure jump_out;
begin
close_files_and_terminate;
do_final_end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [6.84] switch-to-editor option
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
line ready to be edited. But such an extension requires some system
wizardry, so the present implementation simply types out what file should be
edited and the relevant line number.
@^system dependencies@>

There is a secret `\.D' option available when the debugging routines have
not been commented out.
@^debugging@>
@y
line ready to be edited.
We do this by calling the external procedure |calledit| with a pointer to
the filename, its length, and the line number.
However, here we just set up the variables that will be used as arguments,
since we don't want to do the switch-to-editor until after TeX has closed
its files.
@^system dependencies@>

There is a secret `\.D' option available when the debugging routines have
not been commented out.
@^debugging@>
@d edit_file==input_stack[base_ptr]
@z
@x
"E": if base_ptr>0 then
  begin print_nl("You want to edit file ");
@.You want to edit file x@>
  print(input_stack[base_ptr].name_field);
  print(" at line "); print_int(line);
  interaction:=scroll_mode; jump_out;
@y
"E": if base_ptr>0 then
    begin
    edit_name_start:=str_start[edit_file.name_field];
    edit_name_length:=str_start[edit_file.name_field+1] -
    		      str_start[edit_file.name_field];
    edit_line:=line;
    jump_out;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [7.109]Handle definition of glue_ratio in an include file as a typedef
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!glue_ratio=real; {one-word representation of a glue expansion factor}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [8.110]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d min_quarterword=0 {smallest allowable value in a |quarterword|}
@d max_quarterword=255 {largest allowable value in a |quarterword|}
@d min_halfword==0 {smallest allowable value in a |halfword|}
@d max_halfword==65535 {largest allowable value in a |halfword|}
@y
@d min_quarterword=0 {smallest allowable value in a |quarterword|}
@d max_quarterword=255 {largest allowable value in a |quarterword|}
@d min_halfword==0 {smallest allowable value in a |halfword|}
@d max_halfword==327679{largest allowable value in a |halfword|}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [8.112] Efficiency!
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d qi(#)==#+min_quarterword
  {to put an |eight_bits| item into a quarterword}
@d qo(#)==#-min_quarterword
  {to take an |eight_bits| item out of a quarterword}
@d hi(#)==#+min_halfword
  {to put a sixteen-bit item into a halfword}
@d ho(#)==#-min_halfword
  {to take a sixteen-bit item from a halfword}
@y
@d qi(#)==#
@d qo(#)==#
@d hi(#)==#
@d ho(#)==#
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [8.113] Hand-code some more things in C
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!quarterword = min_quarterword..max_quarterword; {1/4 of a word}
@!halfword=min_halfword..max_halfword; {1/2 of a word}
@!two_choices = 1..2; {used when there are two variants in a record}
@!four_choices = 1..4; {used when there are four variants in a record}
@!two_halves = packed record@;@/
  @!rh:halfword;
  case two_choices of
  1: (@!lh:halfword);
  2: (@!b0:quarterword; @!b1:quarterword);
  end;
@!four_quarters = packed record@;@/
  @!b0:quarterword;
  @!b1:quarterword;
  @!b2:quarterword;
  @!b3:quarterword;
  end;
@!memory_word = record@;@/
  case four_choices of
  1: (@!int:integer);
  2: (@!gr:glue_ratio);
  3: (@!hh:two_halves);
  4: (@!qqqq:four_quarters);
  end;
@!word_file = file of memory_word;
@y
@!quarterword=min_quarterword..max_quarterword;
@!halfword=min_halfword..max_halfword;
@!two_choices = 1..2; {used when there are two variants in a record}
@!four_choices = 1..4; {used when there are four variants in a record}
@!KANJI_code=halfword;
@=#include "memory.h";@>

@ KANJI_code handling

@d @!KANJI(#)==#
@d @!hbyte==Hi(cur_jchr)
@d @!lbyte==Lo(cur_jchr)
@d @!Hi_code(#)==mem[#].hh.b0
@d @!Lo_code(#)==mem[#].hh.b1
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [9.116] Change mem to zmem
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% We do this so that we can define mem to be a register pointer to the
% memory array for speed.
@x
@!mem : array[mem_min..mem_max] of memory_word; {the big dynamic storage area}
@y
@!zmem : array[mem_min..mem_max] of memory_word; {the big dynamic storage area}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [9.127] Fix casting problem in C
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% There are several of these.  They come from the rules C uses for
% comparing signed and unsigned quantities.  Just doing the comparison
% can result in incorrect evaluation wrt the way Pascal would do it.
@x
if r>p+1 then @<Allocate from the top of node |p| and |goto found|@>;
@y
if r>toint(p+1) then @<Allocate from the top of node |p| and |goto found|@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [10.135] box node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d hlist_node=0 {|type| of hlist nodes}
@d box_node_size=7 {number of words to allocate for a box node}
@d width_offset=1 {position of |width| field in a box node}
@d depth_offset=2 {position of |depth| field in a box node}
@d height_offset=3 {position of |height| field in a box node}
@d width(#) == mem[#+width_offset].sc {width of the box, in sp}
@d depth(#) == mem[#+depth_offset].sc {depth of the box, in sp}
@d height(#) == mem[#+height_offset].sc {height of the box, in sp}
@d shift_amount(#) == mem[#+4].sc {repositioning distance, in sp}
@d list_offset=5 {position of |list_ptr| field in a box node}
@d list_ptr(#) == link(#+list_offset) {beginning of the list inside the box}
@d glue_order(#) == subtype(#+list_offset) {applicable order of infinity}
@d glue_sign(#) == type(#+list_offset) {stretching or shrinking}
@d normal=0 {the most common case when several cases are named}
@d stretching = 1 {glue setting applies to the stretch components}
@d shrinking = 2 {glue setting applies to the shrink components}
@d glue_offset = 6 {position of |glue_set| in a box node}
@d glue_set(#) == mem[#+glue_offset].gr
  {a word of type |glue_ratio| for glue setting}
@y
@d hlist_node=0 {|type| of hlist nodes}
@d box_node_size=8 {number of words to allocate for a box node}
@d width_offset=1 {position of |width| field in a box node}
@d depth_offset=2 {position of |depth| field in a box node}
@d height_offset=3 {position of |height| field in a box node}
@d width(#) == mem[#+width_offset].sc {width of the box, in sp}
@d depth(#) == mem[#+depth_offset].sc {depth of the box, in sp}
@d height(#) == mem[#+height_offset].sc {height of the box, in sp}
@d shift_amount(#) == mem[#+4].sc {repositioning distance, in sp}
@d list_offset=5 {position of |list_ptr| field in a box node}
@d list_ptr(#) == link(#+list_offset) {beginning of the list inside the box}
@d glue_order(#) == subtype(#+list_offset) {applicable order of infinity}
@d glue_sign(#) == type(#+list_offset) {stretching or shrinking}
@d normal=0 {the most common case when several cases are named}
@d stretching = 1 {glue setting applies to the stretch components}
@d shrinking = 2 {glue setting applies to the shrink components}
@d glue_offset = 6 {position of |glue_set| in a box node}
@d glue_set(#) == mem[#+glue_offset].gr
  {a word of type |glue_ratio| for glue setting}
@d space_offset = 7 {position of |glue_set| in a box node}
@d space_ptr(#) == link(#+space_offset)
@d xspace_ptr(#) == info(#+space_offset)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [10.135] new box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function new_null_box:pointer; {creates a new box node}
var p:pointer; {the new node}
begin p:=get_node(box_node_size); type(p):=hlist_node;
subtype(p):=min_quarterword;
width(p):=0; depth(p):=0; height(p):=0; shift_amount(p):=0; list_ptr(p):=null;
glue_sign(p):=normal; glue_order(p):=normal; set_glue_ratio_zero(glue_set(p));
new_null_box:=p;
@y
@p function new_null_box:pointer; {creates a new box node}
var p:pointer; {the new node}
begin p:=get_node(box_node_size); type(p):=hlist_node;
subtype(p):=min_quarterword;
width(p):=0; depth(p):=0; height(p):=0; shift_amount(p):=0; list_ptr(p):=null;
glue_sign(p):=normal; glue_order(p):=normal; set_glue_ratio_zero(glue_set(p));
@<set zero glue@>; new_null_box:=p;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [11.165] fix the word "free" so that it doesn't conflict with the C routine
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
been included. (You may want to decrease the size of |mem| while you
@^debugging@>
are debugging.)
@y
been included. (You may want to decrease the size of |mem| while you
@^debugging@>
are debugging.)

@d free==free_arr
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [12.174] Eliminate some unsigned comparisons to zero
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%        begin if (font(p)<font_base)or(font(p)>font_max) then
%          print_char("*")
%@y
%        begin if (font(p)>font_max) then
%          print_char("*")
%@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [12.174] display box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure short_display(@!p:integer); {prints highlights of list |p|}
var n:integer; {for replacement counts}
begin while p>mem_min do
  begin if is_char_node(p) then
    begin if p<=mem_end then
      begin if font(p)<>font_in_short_display then
        begin if (font(p)<font_base)or(font(p)>font_max) then
          print_char("*")
@.*\relax@>
        else @<Print the font identifier for |font(p)|@>;
        print_char(" "); font_in_short_display:=font(p);
        end;
      print_ASCII(qo(character(p)));
      end;
    end
  else @<Print a short indication of the contents of node |p|@>;
  p:=link(p);
  end;
end;
@y
@p procedure short_display(@!p:integer); {prints highlights of list |p|}
var n:integer; {for replacement counts}
    f:internal_font_number;
    jc:KANJI_code;
begin while p>mem_min do
  begin if is_char_node(p) then
    begin if p<=mem_end then
      begin f:=font(p); if f<>font_in_short_display then
        begin if (font(p)>font_max) then
          print_char("*")
@.*\relax@>
        else @<Print the font identifier for |font(p)|@>;
        print_char(" "); font_in_short_display:=f;
        end;
      if font_jtype[f] then
        begin
        p:=link(p); jc:=info(p);
        print_char(Hi(jc)); print_char(Lo(jc));
        end
      else print_ASCII(qo(character(p)));
      end;
    end
  else @<Print a short indication of the contents of node |p|@>;
  p:=link(p);
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [12.176] Eliminate some unsigned comparisons to zero (continued)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%@p procedure print_font_and_char(@!p:integer); {prints |char_node| data}
%begin if p>mem_end then print_esc("CLOBBERED.")
%else  begin if (font(p)<font_base)or(font(p)>font_max) then print_char("*")
%@y
%@p procedure print_font_and_char(@!p:integer); {prints |char_node| data}
%begin if p>mem_end then print_esc("CLOBBERED.")
%else  begin if (font(p)>font_max) then print_char("*")
%@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [12.176] print font and char
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure print_font_and_char(@!p:integer); {prints |char_node| data}
begin if p>mem_end then print_esc("CLOBBERED.")
else  begin if (font(p)<font_base)or(font(p)>font_max) then print_char("*")
@.*\relax@>
  else @<Print the font identifier for |font(p)|@>;
  print_char(" "); print_ASCII(qo(character(p)));
  end;
end;
@y
@p procedure print_font_and_char(@!p:integer); {prints |char_node| data}
var f:internal_font_number;
    jc:KANJI_code;
begin if p>mem_end then print_esc("CLOBBERED.")
else begin f:=font(p);
  if (f>font_max) then print_char("*")
@.*\relax@>
  else @<Print the font identifier for |font(p)|@>;
  print_char(" ");
  if font_jtype[f] then begin
    p:=link(p);jc:=info(p);
    print_char(Hi(jc)); print_char(Lo(jc));
  end else
    print_ASCII(qo(character(p)));
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [12.183] display node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Display node |p|@>=
if is_char_node(p) then print_font_and_char(p)
else  case type(p) of
  hlist_node,vlist_node,unset_node: @<Display box |p|@>;
@y
@ @<Display node |p|@>=
if is_char_node(p) then
  begin print_font_and_char(p);
    if font_jtype[font(p)] then p:=link(p)
  end
else  case type(p) of
  hlist_node,vlist_node,unset_node: @<Display box |p|@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [12.184] display box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
    begin print(", shifted "); print_scaled(shift_amount(p));
    end;
  end;
node_list_display(list_ptr(p)); {recursive call}
@y
    begin print(", shifted "); print_scaled(shift_amount(p));
    end;
  end;
{if type(p)=hlist_node then begin

end;}
node_list_display(list_ptr(p)); {recursive call}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [12.186] glue_ratio fix
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
floating point underflow on the author's computer.
@^system dependencies@>
@^dirty \PASCAL@>

@<Display the value of |glue_set(p)|@>=
g:=float(glue_set(p));
if (g<>float_constant(0))and(glue_sign(p)<>normal) then
  begin print(", glue set ");
  if glue_sign(p)=shrinking then print("- ");
  if abs(mem[p+glue_offset].int)<@'4000000 then print("?.?")
  else if abs(g)>float_constant(20000) then
    begin if g>float_constant(0) then print_char(">")
    else print("< -");
    print_glue(20000*unity,glue_order(p),0);
    end
  else print_glue(round(unity*g),glue_order(p),0);
@^real multiplication@>
  end
@y
floating point underflow on the author's computer.
@^system dependencies@>
@^dirty \PASCAL@>

@<Display the value of |glue_set(p)|@>=
g:=float(glue_set(p));
if (g<>float_constant(0))and(glue_sign(p)<>normal) then
  begin print(", glue set ");
  if glue_sign(p)=shrinking then print("- ");
  { The Unix "pc" folks removed this restriction with a remark that
    invalid bit patterns were vanishingly improbable, so we follow
    their example without really understanding it.
  if abs(mem[p+glue_offset].int)<@'4000000 then print('?.?')
  else }
  if fabs(g)>float_constant(20000) then
    begin if g>float_constant(0) then print_char(">")
    else print("< -");
    print_glue(20000*unity,glue_order(p),0);
    end
  else print_glue(round(unity*g),glue_order(p),0);
@^real multiplication@>
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [13.202] free box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
    hlist_node,vlist_node,unset_node: begin flush_node_list(list_ptr(p));
      free_node(p,box_node_size); goto done;
      end;
@y
    hlist_node,vlist_node,unset_node: begin flush_node_list(list_ptr(p));
      fast_delete_glue_ref(space_ptr(p));
      fast_delete_glue_ref(xspace_ptr(p));
      free_node(p,box_node_size); goto done;
      end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [14.206] copy box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
hlist_node,vlist_node,unset_node: begin r:=get_node(box_node_size);
  mem[r+6]:=mem[p+6]; mem[r+5]:=mem[p+5]; {copy the last two words}
  list_ptr(r):=copy_node_list(list_ptr(p)); {this affects |mem[r+5]|}
  words:=5;
@y
hlist_node,vlist_node,unset_node: begin r:=get_node(box_node_size);
  mem[r+7]:=mem[p+7];
  mem[r+6]:=mem[p+6]; mem[r+5]:=mem[p+5]; {copy the last three words}
  add_glue_ref(space_ptr(r)); add_glue_ref(xspace_ptr(r));
  list_ptr(r):=copy_node_list(list_ptr(p)); {this affects |mem[r+5]|}
  words:=5;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [15.207][15.208][15.209] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d escape=0 {escape delimiter (called \.\\ in {\sl The \TeX book\/})}
@:TeXbook}{\sl The \TeX book@>
@d relax=0 {do nothing ( \.{\\relax} )}
@d left_brace=1 {beginning of a group ( \.\{ )}
@d right_brace=2 {ending of a group ( \.\} )}
@d math_shift=3 {mathematics shift character ( \.\$ )}
@d tab_mark=4 {alignment delimiter ( \.\&, \.{\\span} )}
@d car_ret=5 {end of line ( |carriage_return|, \.{\\cr}, \.{\\crcr} )}
@d out_param=5 {output a macro parameter}
@d mac_param=6 {macro parameter symbol ( \.\# )}
@d sup_mark=7 {superscript ( \.{\char'136} )}
@d sub_mark=8 {subscript ( \.{\char'137} )}
@d ignore=9 {characters to ignore ( \.{\^\^J} )}
@d endv=9 {end of \<v_j> list in alignment template}
@d spacer=10 {characters equivalent to blank space ( \.{\ } )}
@d letter=11 {characters regarded as letters ( \.{A..Z}, \.{a..z} )}
@d other_char=12 {none of the special character types}
@d active_char=13 {characters that invoke macros ( \.{\^\^[} )}
@d par_end=13 {end of paragraph ( \.{\\par} )}
@d match=13 {match a macro parameter}
@d comment=14 {characters that introduce comments ( \.\% )}
@d end_match=14 {end of parameters to macro}
@d stop=14 {end of job ( \.{\\end}, \.{\\dump} )}
@d invalid_char=15 {characters that shouldn't appear ( \.{\^\^?} )}
@d delim_num=15 {specify delimiter numerically ( \.{\\delimiter} )}
@d max_char_code=15 {largest catcode for individual characters}

@ Next are the ordinary run-of-the-mill command codes.  Codes that are
|min_internal| or more represent internal quantities that might be
expanded by `\.{\\the}'.

@d char_num=16 {character specified numerically ( \.{\\char} )}
@d math_char_num=17 {explicit math code ( \.{\\mathchar} )}
@d mark=18 {mark definition ( \.{\\mark} )}
@d xray=19 {peek inside of \TeX\ ( \.{\\show}, \.{\\showbox}, etc.~)}
@d make_box=20 {make a box ( \.{\\box}, \.{\\copy}, \.{\\hbox}, etc.~)}
@d hmove=21 {horizontal motion ( \.{\\moveleft}, \.{\\moveright} )}
@d vmove=22 {vertical motion ( \.{\\raise}, \.{\\lower} )}
@d un_hbox=23 {unglue a box ( \.{\\unhbox}, \.{\\unhcopy} )}
@d un_vbox=24 {unglue a box ( \.{\\unvbox}, \.{\\unvcopy} )}
@d remove_item=25 {nullify last item ( \.{\\unpenalty},
  \.{\\unkern}, \.{\\unskip} )}
@d hskip=26 {horizontal glue ( \.{\\hskip}, \.{\\hfil}, etc.~)}
@d vskip=27 {vertical glue ( \.{\\vskip}, \.{\\vfil}, etc.~)}
@d mskip=28 {math glue ( \.{\\mskip} )}
@d kern=29 {fixed space ( \.{\\kern})}
@d mkern=30 {math kern ( \.{\\mkern} )}
@d leader_ship=31 {use a box ( \.{\\shipout}, \.{\\leaders}, etc.~)}
@d halign=32 {horizontal table alignment ( \.{\\halign} )}
@d valign=33 {vertical table alignment ( \.{\\valign} )}
@d no_align=34 {temporary escape from alignment ( \.{\\noalign} )}
@d vrule=35 {vertical rule ( \.{\\vrule} )}
@d hrule=36 {horizontal rule ( \.{\\hrule} )}
@d insert=37 {vlist inserted in box ( \.{\\insert} )}
@d vadjust=38 {vlist inserted in enclosing paragraph ( \.{\\vadjust} )}
@d ignore_spaces=39 {gobble |spacer| tokens ( \.{\\ignorespaces} )}
@d after_assignment=40 {save till assignment is done ( \.{\\afterassignment} )}
@d after_group=41 {save till group is done ( \.{\\aftergroup} )}
@d break_penalty=42 {additional badness ( \.{\\penalty} )}
@d start_par=43 {begin paragraph ( \.{\\indent}, \.{\\noindent} )}
@d ital_corr=44 {italic correction ( \.{\\/} )}
@d accent=45 {attach accent in text ( \.{\\accent} )}
@d math_accent=46 {attach accent in math ( \.{\\mathaccent} )}
@d discretionary=47 {discretionary texts ( \.{\\-}, \.{\\discretionary} )}
@d eq_no=48 {equation number ( \.{\\eqno}, \.{\\leqno} )}
@d left_right=49 {variable delimiter ( \.{\\left}, \.{\\right} )}
@d math_comp=50 {component of formula ( \.{\\mathbin}, etc.~)}
@d limit_switch=51 {diddle limit conventions ( \.{\\displaylimits}, etc.~)}
@d above=52 {generalized fraction ( \.{\\above}, \.{\\atop}, etc.~)}
@d math_style=53 {style specification ( \.{\\displaystyle}, etc.~)}
@d math_choice=54 {choice specification ( \.{\\mathchoice} )}
@d non_script=55 {conditional math glue ( \.{\\nonscript} )}
@d vcenter=56 {vertically center a vbox ( \.{\\vcenter} )}
@d case_shift=57 {force specific case ( \.{\\lowercase}, \.{\\uppercase}~)}
@d message=58 {send to user ( \.{\\message}, \.{\\errmessage} )}
@d extension=59 {extensions to \TeX\ ( \.{\\write}, \.{\\special}, etc.~)}
@d in_stream=60 {files for reading ( \.{\\openin}, \.{\\closein} )}
@d begin_group=61 {begin local grouping ( \.{\\begingroup} )}
@d end_group=62 {end local grouping ( \.{\\endgroup} )}
@d omit=63 {omit alignment template ( \.{\\omit} )}
@d ex_space=64 {explicit space ( \.{\\\ } )}
@d radical=65 {square root and similar signs ( \.{\\radical} )}
@d end_cs_name=66 {end control sequence ( \.{\\endcsname} )}
@d min_internal=67 {the smallest code that can follow \.{\\the}}
@d char_given=67 {character code defined by \.{\\chardef}}
@d math_given=68 {math code defined by \.{\\mathchardef}}
@d last_item=69 {most recent item ( \.{\\lastpenalty},
  \.{\\lastkern}, \.{\\lastskip} )}
@d max_non_prefixed_command=69 {largest command code that can't be \.{\\global}}

@ The next codes are special; they all relate to mode-independent
assignment of values to \TeX's internal registers or tables.
Codes that are |max_internal| or less represent internal quantities
that might be expanded by `\.{\\the}'.

@d toks_register=70 {token list register ( \.{\\toks} )}
@d assign_toks=71 {special token list ( \.{\\output}, \.{\\everypar}, etc.~)}
@d assign_int=72 {user-defined integer ( \.{\\tolerance}, \.{\\day}, etc.~)}
@d assign_dimen=73 {user-defined length ( \.{\\hsize}, etc.~)}
@d assign_glue=74 {user-defined glue ( \.{\\baselineskip}, etc.~)}
@d assign_mu_glue=75 {user-defined muglue ( \.{\\thinmuskip}, etc.~)}
@d assign_font_dimen=76 {user-defined font dimension ( \.{\\fontdimen} )}
@d assign_font_int=77 {user-defined font integer ( \.{\\hyphenchar},
  \.{\\skewchar} )}
@d set_aux=78 {specify state info ( \.{\\spacefactor}, \.{\\prevdepth} )}
@d set_prev_graf=79 {specify state info ( \.{\\prevgraf} )}
@d set_page_dimen=80 {specify state info ( \.{\\pagegoal}, etc.~)}
@d set_page_int=81 {specify state info ( \.{\\deadcycles},
  \.{\\insertpenalties} )}
@d set_box_dimen=82 {change dimension of box ( \.{\\wd}, \.{\\ht}, \.{\\dp} )}
@d set_shape=83 {specify fancy paragraph shape ( \.{\\parshape} )}
@d def_code=84 {define a character code ( \.{\\catcode}, etc.~)}
@d def_family=85 {declare math fonts ( \.{\\textfont}, etc.~)}
@d set_font=86 {set current font ( font identifiers )}
@d def_font=87 {define a font file ( \.{\\font} )}
@d register=88 {internal register ( \.{\\count}, \.{\\dimen}, etc.~)}
@d max_internal=88 {the largest code that can follow \.{\\the}}
@d advance=89 {advance a register or parameter ( \.{\\advance} )}
@d multiply=90 {multiply a register or parameter ( \.{\\multiply} )}
@d divide=91 {divide a register or parameter ( \.{\\divide} )}
@d prefix=92 {qualify a definition ( \.{\\global}, \.{\\long}, \.{\\outer} )}
@d let=93 {assign a command code ( \.{\\let}, \.{\\futurelet} )}
@d shorthand_def=94 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
@d read_to_cs=95 {read into a control sequence ( \.{\\read} )}
@d def=96 {macro definition ( \.{\\def}, \.{\\gdef}, \.{\\xdef}, \.{\\edef} )}
@d set_box=97 {set a box ( \.{\\setbox} )}
@d hyph_data=98 {hyphenation data ( \.{\\hyphenation}, \.{\\patterns} )}
@d set_interaction=99 {define level of interaction ( \.{\\batchmode}, etc.~)}
@d max_command=99 {the largest command code seen at |big_switch|}
@y
@d escape=0 {escape delimiter (called \.\\ in {\sl The \TeX book\/})}
@:TeXbook}{\sl The \TeX book@>
@d relax=0 {do nothing ( \.{\\relax} )}
@d left_brace=1 {beginning of a group ( \.\{ )}
@d right_brace=2 {ending of a group ( \.\} )}
@d math_shift=3 {mathematics shift character ( \.\$ )}
@d tab_mark=4 {alignment delimiter ( \.\&, \.{\\span} )}
@d car_ret=5 {end of line ( |carriage_return|, \.{\\cr}, \.{\\crcr} )}
@d out_param=5 {output a macro parameter}
@d mac_param=6 {macro parameter symbol ( \.\# )}
@d sup_mark=7 {superscript ( \.{\char'136} )}
@d sub_mark=8 {subscript ( \.{\char'137} )}
@d ignore=9 {characters to ignore ( \.{\^\^J} )}
@d endv=9 {end of \<v_j> list in alignment template}
@d spacer=10 {characters equivalent to blank space ( \.{\ } )}
@d letter=11 {characters regarded as letters ( \.{A..Z}, \.{a..z} )}
@d other_char=12 {none of the special character types}
@d active_char=13 {characters that invoke macros ( \.{\^\^[} )}
@d par_end=13 {end of paragraph ( \.{\\par} )}
@d match=13 {match a macro parameter}
@d comment=14 {characters that introduce comments ( \.\% )}
@d end_match=14 {end of parameters to macro}
@d stop=14 {end of job ( \.{\\end}, \.{\\dump} )}
@d invalid_char=15 {characters that shouldn't appear ( \.{\^\^?} )}
@d delim_num=15 {specify delimiter numerically ( \.{\\delimiter} )}
@d kanji=16 {kanji}
@d kana=17 {hiragana, katakana, alphabet}
@d other_kchar=18 {kanji codes}
@d max_char_code=18 {largest catcode for individual characters}

@ Next are the ordinary run-of-the-mill command codes.  Codes that are
|min_internal| or more represent internal quantities that might be
expanded by `\.{\\the}'.

@d char_num=max_char_code+1 {character specified numerically ( \.{\\char} )}
@d math_char_num=max_char_code+2 {explicit math code ( \.{\\mathchar} )}
@d mark=max_char_code+3 {mark definition ( \.{\\mark} )}
@d xray=max_char_code+4 {peek inside of \TeX\ ( \.{\\show}, \.{\\showbox}, etc.~)}
@d make_box=max_char_code+5 {make a box ( \.{\\box}, \.{\\copy}, \.{\\hbox}, etc.~)}
@d hmove=max_char_code+6 {horizontal motion ( \.{\\moveleft}, \.{\\moveright} )}
@d vmove=max_char_code+7 {vertical motion ( \.{\\raise}, \.{\\lower} )}
@d un_hbox=max_char_code+8 {unglue a box ( \.{\\unhbox}, \.{\\unhcopy} )}
@d un_vbox=max_char_code+9 {unglue a box ( \.{\\unvbox}, \.{\\unvcopy} )}
@d remove_item=max_char_code+10 {nullify last item ( \.{\\unpenalty},
  \.{\\unkern}, \.{\\unskip} )}
@d hskip=max_char_code+11 {horizontal glue ( \.{\\hskip}, \.{\\hfil}, etc.~)}
@d vskip=max_char_code+12 {vertical glue ( \.{\\vskip}, \.{\\vfil}, etc.~)}
@d mskip=max_char_code+13 {math glue ( \.{\\mskip} )}
@d kern=max_char_code+14 {fixed space ( \.{\\kern})}
@d mkern=max_char_code+15 {math kern ( \.{\\mkern} )}
@d leader_ship=max_char_code+16 {use a box ( \.{\\shipout}, \.{\\leaders}, etc.~)}
@d halign=max_char_code+17 {horizontal table alignment ( \.{\\halign} )}
@d valign=max_char_code+18 {vertical table alignment ( \.{\\valign} )}
@d no_align=max_char_code+19 {temporary escape from alignment ( \.{\\noalign} )}
@d vrule=max_char_code+20 {vertical rule ( \.{\\vrule} )}
@d hrule=max_char_code+21 {horizontal rule ( \.{\\hrule} )}
@d insert=max_char_code+22 {vlist inserted in box ( \.{\\insert} )}
@d vadjust=max_char_code+23 {vlist inserted in enclosing paragraph ( \.{\\vadjust} )}
@d ignore_spaces=max_char_code+24 {gobble |spacer| tokens ( \.{\\ignorespaces} )}
@d after_assignment=max_char_code+25 {save till assignment is done ( \.{\\afterassignment} )}
@d after_group=max_char_code+26 {save till group is done ( \.{\\aftergroup} )}
@d break_penalty=max_char_code+27 {additional badness ( \.{\\penalty} )}
@d start_par=max_char_code+28 {begin paragraph ( \.{\\indent}, \.{\\noindent} )}
@d ital_corr=max_char_code+29 {italic correction ( \.{\\/} )}
@d accent=max_char_code+30 {attach accent in text ( \.{\\accent} )}
@d math_accent=max_char_code+31 {attach accent in math ( \.{\\mathaccent} )}
@d discretionary=max_char_code+32 {discretionary texts ( \.{\\-}, \.{\\discretionary} )}
@d eq_no=max_char_code+33 {equation number ( \.{\\eqno}, \.{\\leqno} )}
@d left_right=max_char_code+34 {variable delimiter ( \.{\\left}, \.{\\right} )}
@d math_comp=max_char_code+35 {component of formula ( \.{\\mathbin}, etc.~)}
@d limit_switch=max_char_code+36 {diddle limit conventions ( \.{\\displaylimits}, etc.~)}
@d above=max_char_code+37 {generalized fraction ( \.{\\above}, \.{\\atop}, etc.~)}
@d math_style=max_char_code+38 {style specification ( \.{\\displaystyle}, etc.~)}
@d math_choice=max_char_code+39 {choice specification ( \.{\\mathchoice} )}
@d non_script=max_char_code+40 {conditional math glue ( \.{\\nonscript} )}
@d vcenter=max_char_code+41 {vertically center a vbox ( \.{\\vcenter} )}
@d case_shift=max_char_code+42 {force specific case ( \.{\\lowercase}, \.{\\uppercase}~)}
@d message=max_char_code+43 {send to user ( \.{\\message}, \.{\\errmessage} )}
@d extension=max_char_code+44 {extensions to \TeX\ ( \.{\\write}, \.{\\special}, etc.~)}
@d in_stream=max_char_code+45 {files for reading ( \.{\\openin}, \.{\\closein} )}
@d begin_group=max_char_code+46 {begin local grouping ( \.{\\begingroup} )}
@d end_group=max_char_code+47 {end local grouping ( \.{\\endgroup} )}
@d omit=max_char_code+48 {omit alignment template ( \.{\\omit} )}
@d ex_space=max_char_code+49 {explicit space ( \.{\\\ } )}
@d radical=max_char_code+50 {square root and similar signs ( \.{\\radical} )}
@d end_cs_name=max_char_code+51 {end control sequence ( \.{\\endcsname} )}
@d min_internal=max_char_code+52 {the smallest code that can follow \.{\\the}}
@d char_given=max_char_code+52 {character code defined by \.{\\chardef}}
@d math_given=max_char_code+53 {math code defined by \.{\\mathchardef}}
@d last_item=max_char_code+54 {most recent item ( \.{\\lastpenalty},
  \.{\\lastkern}, \.{\\lastskip} )}
@d inhibit_glue=max_char_code+55 {inhibit adjust glue ( \.{\\inhibitglue} )}
@d max_non_prefixed_command=max_char_code+55 {largest command code that can't be \.{\\global}}

@ The next codes are special; they all relate to mode-independent
assignment of values to \TeX's internal registers or tables.
Codes that are |max_internal| or less represent internal quantities
that might be expanded by `\.{\\the}'.

@d toks_register=max_non_prefixed_command+1 {token list register ( \.{\\toks} )}
@d assign_toks=toks_register+1 {special token list ( \.{\\output}, \.{\\everypar}, etc.~)}
@d assign_int=assign_toks+1 {user-defined integer ( \.{\\tolerance}, \.{\\day}, etc.~)}
@d assign_dimen=assign_int+1 {user-defined length ( \.{\\hsize}, etc.~)}
@d assign_glue=assign_dimen+1 {user-defined glue ( \.{\\baselineskip}, etc.~)}
@d assign_mu_glue=assign_glue+1 {user-defined muglue ( \.{\\thinmuskip}, etc.~)}
@d assign_font_dimen=assign_mu_glue+1 {user-defined font dimension ( \.{\\fontdimen} )}
@d assign_font_int=assign_font_dimen+1
{user-defined font integer ( \.{\\hyphenchar}, \.{\\skewchar} )}
@d assign_kinsoku=assign_font_int+1
{user-defined kinsoku character ( \.{\\prebreakpenalty}, \.{\\postbreakpenalty} )}
@d assign_inhibit_xsp_code=assign_kinsoku+1
{user-defined inhibit xsp character ( \.{\\inhibitxspcode} )}
@d set_aux=assign_inhibit_xsp_code+1 {specify state info ( \.{\\spacefactor}, \.{\\prevdepth} )}
@d set_prev_graf=set_aux+1 {specify state info ( \.{\\prevgraf} )}
@d set_page_dimen=set_prev_graf+1 {specify state info ( \.{\\pagegoal}, etc.~)}
@d set_page_int=set_page_dimen+1 {specify state info ( \.{\\deadcycles},
  \.{\\insertpenalties} )}
@d set_box_dimen=set_page_int+1 {change dimension of box ( \.{\\wd}, \.{\\ht}, \.{\\dp} )}
@d set_shape=set_box_dimen+1 {specify fancy paragraph shape ( \.{\\parshape} )}
@d def_code=set_shape+1 {define a character code ( \.{\\catcode}, etc.~)}
@d def_family=def_code+1 {declare math fonts ( \.{\\textfont}, etc.~)}
@d set_font=def_family+1 {set current font ( font identifiers )}
@d def_font=set_font+1 {define a font file ( \.{\\font} )}
@d def_jfont=def_font+1 {define a font file ( \.{\\jfont} )}
@d register=def_jfont+1 {internal register ( \.{\\count}, \.{\\dimen}, etc.~)}
@d max_internal=register {the largest code that can follow \.{\\the}}
@d advance=max_internal+1 {advance a register or parameter ( \.{\\advance} )}
@d multiply=advance+1 {multiply a register or parameter ( \.{\\multiply} )}
@d divide=multiply+1 {divide a register or parameter ( \.{\\divide} )}
@d prefix=divide+1 {qualify a definition ( \.{\\global}, \.{\\long}, \.{\\outer} )}
@d let=prefix+1 {assign a command code ( \.{\\let}, \.{\\futurelet} )}
@d shorthand_def=let+1 {code definition ( \.{\\chardef}, \.{\\countdef}, etc.~)}
@d read_to_cs=shorthand_def+1 {read into a control sequence ( \.{\\read} )}
@d def=read_to_cs+1 {macro definition ( \.{\\def}, \.{\\gdef}, \.{\\xdef}, \.{\\edef} )}
@d set_box=def+1 {set a box ( \.{\\setbox} )}
@d hyph_data=set_box+1 {hyphenation data ( \.{\\hyphenation}, \.{\\patterns} )}
@d set_interaction=hyph_data+1 {define level of interaction ( \.{\\batchmode}, etc.~)}
@d set_auto_spacing=set_interaction+1 {set auto spaceing mode
  ( \.{\\autospacing}, \.{\\noautospacing}, ( \.{\\autospacing}, \.{\\noautospacing} )}
@d max_command=set_auto_spacing {the largest command code seen at |big_switch|}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [15.210] hihibit_glue
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%@d undefined_cs=max_command+1 {initial state of most |eq_type| fields}
%@d expand_after=max_command+2 {special expansion ( \.{\\expandafter} )}
%@d no_expand=max_command+3 {special nonexpansion ( \.{\\noexpand} )}
%@d input=max_command+4 {input a source file ( \.{\\input}, \.{\\endinput} )}
%@d if_test=max_command+5 {conditional text ( \.{\\if}, \.{\\ifcase}, etc.~)}
%@d fi_or_else=max_command+6 {delimiters for conditionals ( \.{\\else}, etc.~)}
%@d cs_name=max_command+7 {make a control sequence from tokens ( \.{\\csname} )}
%@d convert=max_command+8 {convert to text ( \.{\\number}, \.{\\string}, etc.~)}
%@d the=max_command+9 {expand an internal quantity ( \.{\\the} )}
%@d top_bot_mark=max_command+10 {inserted mark ( \.{\\topmark}, etc.~)}
%@d call=max_command+11 {non-long, non-outer control sequence}
%@d long_call=max_command+12 {long, non-outer control sequence}
%@d outer_call=max_command+13 {non-long, outer control sequence}
%@d long_outer_call=max_command+14 {long, outer control sequence}
%@d end_template=max_command+15 {end of an alignment template}
%@d dont_expand=max_command+16 {the following token was marked by \.{\\noexpand}}
%@d glue_ref=max_command+17 {the equivalent points to a glue specification}
%@d shape_ref=max_command+18 {the equivalent points to a parshape specification}
%@d box_ref=max_command+19 {the equivalent points to a box node, or is |null|}
%@d data=max_command+20 {the equivalent is simply a halfword number}
%@y
%@d undefined_cs=max_command+1 {initial state of most |eq_type| fields}
%@d expand_after=max_command+2 {special expansion ( \.{\\expandafter} )}
%@d no_expand=max_command+3 {special nonexpansion ( \.{\\noexpand} )}
%@d input=max_command+4 {input a source file ( \.{\\input}, \.{\\endinput} )}
%@d if_test=max_command+5 {conditional text ( \.{\\if}, \.{\\ifcase}, etc.~)}
%@d fi_or_else=max_command+6 {delimiters for conditionals ( \.{\\else}, etc.~)}
%@d cs_name=max_command+7 {make a control sequence from tokens ( \.{\\csname} )}
%@d convert=max_command+8 {convert to text ( \.{\\number}, \.{\\string}, etc.~)}
%@d the=max_command+9 {expand an internal quantity ( \.{\\the} )}
%@d top_bot_mark=max_command+10 {inserted mark ( \.{\\topmark}, etc.~)}
%@d inhibit_glue=max_command+11 {inhibit adjust glue ( \.{\\inhibitglue} )}
%@d call=max_command+12 {non-long, non-outer control sequence}
%@d long_call=max_command+13 {long, non-outer control sequence}
%@d outer_call=max_command+14 {non-long, outer control sequence}
%@d long_outer_call=max_command+15 {long, outer control sequence}
%@d end_template=max_command+16 {end of an alignment template}
%@d dont_expand=max_command+17 {the following token was marked by \.{\\noexpand}}
%@d glue_ref=max_command+18 {the equivalent points to a glue specification}
%@d shape_ref=max_command+19 {the equivalent points to a parshape specification}
%@d box_ref=max_command+20 {the equivalent points to a box node, or is |null|}
%@d data=max_command+21 {the equivalent is simply a halfword number}
%@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [16.212][16.213] last_jchr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  @!head_field,@!tail_field: pointer;
@y
  @!head_field,@!tail_field,@!last_jchr_field: pointer;
@z

@x
@d tail==cur_list.tail_field {final node on current list}
@y
@d tail==cur_list.tail_field {final node on current list}
@d last_jchr==cur_list.last_jchr_field {final jchar node on current list}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [16.215][16.216][16.217] push_nest pop_nest last_jchr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
mode:=vmode; head:=contrib_head; tail:=contrib_head;
@y
mode:=vmode; head:=contrib_head; tail:=contrib_head; last_jchr:=null;
@z

@x
incr(nest_ptr); head:=get_avail; tail:=head; prev_graf:=0; mode_line:=line;
@y
incr(nest_ptr); head:=new_null_box; tail:=head; prev_graf:=0; mode_line:=line;
last_jchr:=null;
@z

@x
@p procedure pop_nest; {leave a semantic level, re-enter the old}
begin free_avail(head); decr(nest_ptr); cur_list:=nest[nest_ptr];
end;
@y
@p procedure pop_nest; {leave a semantic level, re-enter the old}
begin fast_delete_glue_ref(space_ptr(head));
fast_delete_glue_ref(xspace_ptr(head));
free_node(head,box_node_size); decr(nest_ptr); cur_list:=nest[nest_ptr];
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.224] kanji_skip_code xkanji_skip_code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d thin_mu_skip_code=15 {thin space in math formula}
@d med_mu_skip_code=16 {medium space in math formula}
@d thick_mu_skip_code=17 {thick space in math formula}
@d glue_pars=18 {total number of glue parameters}
@y
@d kanji_skip_code=15
@d xkanji_skip_code=16
@d thin_mu_skip_code=17 {thin space in math formula}
@d med_mu_skip_code=18 {medium space in math formula}
@d thick_mu_skip_code=19 {thick space in math formula}
@d glue_pars=20 {total number of glue parameters}
@d jfm_skip=20
@z

@x
@d par_fill_skip==glue_par(par_fill_skip_code)
@d thin_mu_skip==glue_par(thin_mu_skip_code)
@d med_mu_skip==glue_par(med_mu_skip_code)
@d thick_mu_skip==glue_par(thick_mu_skip_code)
@y
@d par_fill_skip==glue_par(par_fill_skip_code)
@d thin_mu_skip==glue_par(thin_mu_skip_code)
@d med_mu_skip==glue_par(med_mu_skip_code)
@d thick_mu_skip==glue_par(thick_mu_skip_code)
@d kanji_skip==glue_par(kanji_skip_code)
@d xkanji_skip==glue_par(xkanji_skip_code)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.225] kanji_skip_code xkanji_skip_code 2
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
med_mu_skip_code: print_esc("medmuskip");
thick_mu_skip_code: print_esc("thickmuskip");
othercases print("[unknown glue parameter!]")
@y
med_mu_skip_code: print_esc("medmuskip");
thick_mu_skip_code: print_esc("thickmuskip");
kanji_skip_code: print_esc("kanjiskip");
xkanji_skip_code: print_esc("xkanjiskip");
jfm_skip: print("refer from jfm");
othercases print("[unknown glue parameter!]")
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.226] kanji_skip_code xkanji_skip_code 3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
primitive("medmuskip",assign_mu_glue,glue_base+med_mu_skip_code);@/
@!@:med_mu_skip_}{\.{\\medmuskip} primitive@>
primitive("thickmuskip",assign_mu_glue,glue_base+thick_mu_skip_code);@/
@!@:thick_mu_skip_}{\.{\\thickmuskip} primitive@>
@y
primitive("medmuskip",assign_mu_glue,glue_base+med_mu_skip_code);@/
@!@:med_mu_skip_}{\.{\\medmuskip} primitive@>
primitive("thickmuskip",assign_mu_glue,glue_base+thick_mu_skip_code);@/
@!@:thick_mu_skip_}{\.{\\thickmuskip} primitive@>
primitive("kanjiskip",assign_glue,glue_base+kanji_skip_code);@/
@!@:kanji_skip_}{\.{\\kanjiskip} primitive@>
primitive("xkanjiskip",assign_glue,glue_base+xkanji_skip_code);@/
@!@:xkanji_skip_}{\.{\\xkanjiskip} primitive@>
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.230] cat_code_size cur_jfont_loc auto_spacing auto_xspacing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d cat_code_base=math_font_base+48
  {table of 128 command codes (the ``catcodes'')}
@d lc_code_base=cat_code_base+128 {table of 128 lowercase mappings}
@d uc_code_base=lc_code_base+128 {table of 128 uppercase mappings}
@d sf_code_base=uc_code_base+128 {table of 128 spacefactor mappings}
@d math_code_base=sf_code_base+128 {table of 128 math mode mappings}
@d int_base=math_code_base+128 {beginning of region 5}
@y
@d cur_jfont_loc=math_font_base+48
@d auto_spacing_code=cur_jfont_loc+1
@d auto_xspacing_code=auto_spacing_code+1
@d cat_code_base=auto_xspacing_code+1
  {table of 256 command codes (the ``catcodes'')}
@d auto_xsp_code_base=cat_code_base+256 {table of 128 auto spacer flag}
@d inhibit_xsp_code_base=auto_xsp_code_base+128
@d lc_code_base=inhibit_xsp_code_base+256 {table of 128 lowercase mappings}
@d uc_code_base=lc_code_base+128 {table of 128 uppercase mappings}
@d sf_code_base=uc_code_base+128 {table of 128 spacefactor mappings}
@d math_code_base=sf_code_base+128 {table of 128 math mode mappings}
@d int_base=math_code_base+128 {beginning of region 5}
@d cur_jfont==equiv(cur_jfont_loc)
@d auto_xsp_code(#)==equiv(auto_xsp_code_base+#)
@d inhibit_xsp_type(#)==eq_type(inhibit_xsp_code_base+#)
@d inhibit_xsp_code(#)==equiv(inhibit_xsp_code_base+#)
@d auto_spacing==equiv(auto_spacing_code)
@d auto_xspacing==equiv(auto_xspacing_code)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.232] initialize of cat_code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ We initialize most things to null or undefined values. An undefined font
is represented by the internal code |font_base|.

However, the character code tables are given initial values based on the
conventional interpretation of ASCII code. These initial values should
not be changed when \TeX\ is adapted for use with non-English languages;
all changes to the initialization conventions should be made in format
packages, not in \TeX\ itself, so that global interchange of formats is
possible.

@d null_font==font_base
@d var_code==@'70000 {math code meaning ``use the current family''}

@<Initialize table entries...@>=
par_shape_ptr:=null; eq_type(par_shape_loc):=shape_ref;
eq_level(par_shape_loc):=level_one;@/
for k:=output_routine_loc to toks_base+255 do
  eqtb[k]:=eqtb[undefined_control_sequence];
box(0):=null; eq_type(box_base):=box_ref; eq_level(box_base):=level_one;
for k:=box_base+1 to box_base+255 do eqtb[k]:=eqtb[box_base];
cur_font:=null_font; eq_type(cur_font_loc):=data;
eq_level(cur_font_loc):=level_one;@/
for k:=math_font_base to math_font_base+47 do eqtb[k]:=eqtb[cur_font_loc];
equiv(cat_code_base):=0; eq_type(cat_code_base):=data;
eq_level(cat_code_base):=level_one;@/
for k:=cat_code_base+1 to int_base-1 do eqtb[k]:=eqtb[cat_code_base];
for k:=0 to 127 do
  begin cat_code(k):=other_char; math_code(k):=hi(k); sf_code(k):=1000;
  end;
cat_code(carriage_return):=car_ret; cat_code(" "):=spacer;
cat_code("\"):=escape; cat_code("%"):=comment;
cat_code(invalid_code):=invalid_char; cat_code(null_code):=ignore;
for k:="0" to "9" do math_code(k):=hi(k+var_code);
for k:="A" to "Z" do
  begin cat_code(k):=letter; cat_code(k+"a"-"A"):=letter;@/
  math_code(k):=hi(k+var_code+@"100);
  math_code(k+"a"-"A"):=hi(k+"a"-"A"+var_code+@"100);@/
  lc_code(k):=k+"a"-"A"; lc_code(k+"a"-"A"):=k+"a"-"A";@/
  uc_code(k):=k; uc_code(k+"a"-"A"):=k;@/
  sf_code(k):=999;
  end;
@y
@ We initialize most things to null or undefined values. An undefined font
is represented by the internal code |font_base|.

However, the character code tables are given initial values based on the
conventional interpretation of ASCII code. These initial values should
not be changed when \TeX\ is adapted for use with non-English languages;
all changes to the initialization conventions should be made in format
packages, not in \TeX\ itself, so that global interchange of formats is
possible.

@d null_font==font_base
@d var_code==@'70000 {math code meaning ``use the current family''}

@<Initialize table entries...@>=
eq_level(par_shape_loc):=level_one;@/
for k:=output_routine_loc to toks_base+255 do
  eqtb[k]:=eqtb[undefined_control_sequence];
box(0):=null; eq_type(box_base):=box_ref; eq_level(box_base):=level_one;
for k:=box_base+1 to box_base+255 do eqtb[k]:=eqtb[box_base];
cur_font:=null_font; eq_type(cur_font_loc):=data;
eq_level(cur_font_loc):=level_one;@/
cur_jfont:=null_font; eq_type(cur_jfont_loc):=data;
eq_level(cur_jfont_loc):=level_one;@/
for k:=math_font_base to math_font_base+47 do eqtb[k]:=eqtb[cur_font_loc];
equiv(cat_code_base):=0; eq_type(cat_code_base):=data;
eq_level(cat_code_base):=level_one;@/
for k:=cat_code_base+1 to int_base-1 do eqtb[k]:=eqtb[cat_code_base];
eqtb[auto_spacing_code]:=eqtb[cat_code_base];
eqtb[auto_xspacing_code]:=eqtb[cat_code_base];
par_shape_ptr:=null; eq_type(par_shape_loc):=shape_ref;
for k:=0 to 127 do
  begin cat_code(k):=other_char; math_code(k):=hi(k);
  sf_code(k):=1000;
  end;
for k:=0 to 255 do begin inhibit_xsp_code(k):=0; inhibit_xsp_type(k):=0; end;
cat_code(carriage_return):=car_ret; cat_code(" "):=spacer;
cat_code("\"):=escape; cat_code("%"):=comment;
cat_code(invalid_code):=invalid_char; cat_code(null_code):=ignore;
for k:="0" to "9" do 
  begin math_code(k):=hi(k+var_code);
  auto_xsp_code(k):=3;
  end;
for k:="A" to "Z" do
  begin cat_code(k):=letter; cat_code(k+"a"-"A"):=letter;@/
  math_code(k):=hi(k+var_code+@"100);
  math_code(k+"a"-"A"):=hi(k+"a"-"A"+var_code+@"100);@/
  lc_code(k):=k+"a"-"A"; lc_code(k+"a"-"A"):=k+"a"-"A";@/
  uc_code(k):=k; uc_code(k+"a"-"A"):=k;@/
  auto_xsp_code(k):=3; auto_xsp_code(k+"a"-"A"):=3;@/
  sf_code(k):=999;
  end;
for k:=128 to 255 do cat_code(k):=invalid_char;
cat_code(129):=other_kchar; cat_code(130):=kana;
cat_code(131):=kana; cat_code(132):=kana;
for k:=136 to 159 do cat_code(k):=kanji;
for k:=224 to 234 do cat_code(k):=kanji;
jflag:=0;
{kanji_skip:=zero_glue;
xkanji_skip:=zero_glue;}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.236][17.237][17.238] cur_jfam_code jchar_widow_penalty
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d cur_fam_code=44 {current family}
@d escape_char_code=45 {escape character for token output}
@d default_hyphen_char_code=46 {value of \.{\\hyphenchar} when a font is loaded}
@d default_skew_char_code=47 {value of \.{\\skewchar} when a font is loaded}
@d end_line_char_code=48 {character placed at the right end of the buffer}
@d new_line_char_code=49 {character that prints as |print_ln|}
@d int_pars=50 {total number of integer parameters}
@d count_base=int_base+int_pars {256 user \.{\\count} registers}
@y
@d cur_fam_code=44 {current family}
@d cur_jfam_code=45 {current kanji family}
@d escape_char_code=46 {escape character for token output}
@d default_hyphen_char_code=47 {value of \.{\\hyphenchar} when a font is loaded}
@d default_skew_char_code=48 {value of \.{\\skewchar} when a font is loaded}
@d end_line_char_code=49 {character placed at the right end of the buffer}
@d new_line_char_code=50 {character that prints as |print_ln|}
@d jchr_widow_penalty_code=51
			{penalty for creating a widow KANJI character line}
@d int_pars=52 {total number of integer parameters}
@d count_base=int_base+int_pars {256 user \.{\\count} registers}
@z

@x
@d global_defs==int_par(global_defs_code)
@d cur_fam==int_par(cur_fam_code)
@d escape_char==int_par(escape_char_code)
@y
@d global_defs==int_par(global_defs_code)
@d cur_fam==int_par(cur_fam_code)
@d cur_jfam==int_par(cur_jfam_code)
@d escape_char==int_par(escape_char_code)
@d jchr_widow_penalty==int_par(jchr_widow_penalty_code)
@z

@x
new_line_char_code:print_esc("newlinechar");
@y
new_line_char_code:print_esc("newlinechar");
cur_jfam_code:print_esc("jfam");
jchr_widow_penalty_code:print_esc("jcharwidowpenalty");
@z

@x
primitive("newlinechar",assign_int,int_base+new_line_char_code);@/
@!@:new_line_char_}{\.{\\newlinechar} primitive@>
@y
primitive("newlinechar",assign_int,int_base+new_line_char_code);@/
@!@:new_line_char_}{\.{\\newlinechar} primitive@>
primitive("jfam",assign_int,int_base+cur_jfam_code);@/
@!@:cur_jfam_}{\.{\\jfam} primitive@>
primitive("jcharwidowpenalty",assign_int,int_base+jchr_widow_penalty_code);@/
@!@:jchr_widow_penalty}{\.{\\jcharwidowpenalty} primitive@>
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.241] fix_date_and_time
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The following procedure, which is called just before \TeX\ initializes its
input and output, establishes the initial values of the date and time.
@^system dependencies@>
Since standard \PASCAL\ cannot provide such information, something special
is needed. The program here simply specifies July 4, 1776, at noon; but
users probably want a better approximation to the truth.

@p procedure fix_date_and_time;
begin time:=12*60; {minutes since midnight}
day:=4; {fourth day of the month}
month:=7; {seventh month of the year}
year:=1776; {Anno Domini}
end;
@y
@ The following procedure, which is called just before \TeX\ initializes its
input and output, establishes the initial values of the date and time.
It calls a macro-defined |date_and_time| routine.  Date_and_time
("dateandtime") in turn is a C macro, which calls get_date_and_time, passing
it the addresses of the day, month, etc., so they can be set by the
routine.  Get_date_and_time also sets up interrupt catching if that
is conditionally compiled in the C code.
@^system dependencies@>

@d fix_date_and_time==date_and_time(time,day,month,year)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.247] kinsoku
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d scaled_base=dimen_base+dimen_pars
  {table of 256 user-defined \.{\\dimen} registers}
@d eqtb_size=scaled_base+255 {largest subscript of |eqtb|}
@#
@d dimen(#)==eqtb[scaled_base+#].sc
@d dimen_par(#)==eqtb[dimen_base+#].sc {a scaled quantity}
@y
@d scaled_base=dimen_base+dimen_pars
  {table of 256 user-defined \.{\\dimen} registers}
@d kinsoku_base=scaled_base+256 {table of 256-2word kinsoku registers}
@d eqtb_size=kinsoku_base+511 {largest subscript of |eqtb|}
@#
@d kinsoku_type(#)==eq_type(kinsoku_base+#)
@d kinsoku_code(#)==equiv(kinsoku_base+#)
@d kinsoku(#)==eqtb[kinsoku_base+1+#].int
@d dimen(#)==eqtb[scaled_base+#].sc
@d dimen_par(#)==eqtb[dimen_base+#].sc {a scaled quantity}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.250] init eqtb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
for k:=dimen_base to eqtb_size do eqtb[k].sc:=0;
@y
for k:=dimen_base to eqtb_size do eqtb[k].sc:=0;
k:=kinsoku_base;
while k < eqtb_size do begin eq_level(k):=level_one; incr2(k); end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.252] show_eqtb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else if n<=eqtb_size then @<Show equivalent |n|, in region 6@>
else print_char("?"); {this can't happen either}
end;
tats
@y
else if n<kinsoku_base then @<Show equivalent |n|, in region 6@>
else if n<=eqtb_size then @<Show equivalent |n|, in region 7@>
else print_char("?"); {this can't happen either}
end;
tats

@ @<Show equivalent |n|, in region 7@>=
print("kinsoku")
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17.253] xeq_level & init eqtb
% Change eqtb to zeqtb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!eqtb:array[active_base..eqtb_size] of memory_word;
@!xeq_level:array[int_base..eqtb_size] of quarterword;

@ @<Set init...@>=
for k:=int_base to eqtb_size do xeq_level[k]:=level_one;
@y
@!zeqtb:array[active_base..eqtb_size] of memory_word;
@!xeq_level:array[int_base..kinsoku_base-1] of quarterword;

@ @<Set init...@>=
for k:=int_base to kinsoku_base-1 do xeq_level[k]:=level_one;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [18.262] More useless unsigned comparisons to zero
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else if (text(p)<0)or(text(p)>=str_ptr) then print_esc("NONEXISTENT.")
@y
else if (text(p)>=str_ptr) then print_esc("NONEXISTENT.")
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [18.265][18.266] \jfont
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
primitive("font",def_font,0);@/
@!@:font_}{\.{\\font} primitive@>
@y
primitive("font",def_font,0);@/
@!@:font_}{\.{\\font} primitive@>
primitive("jfont",def_jfont,0);@/
@!@:jfont_}{\.{\\jfont} primitive@>
@z

@x
def_font: print_esc("font");
@y
def_font: print_esc("font");
def_jfont: print_esc("jfont");
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [19.276] save double word registr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure eq_save(@!p:pointer;@!l:quarterword); {saves |eqtb[p]|}
begin check_full_save_stack;
if l=level_zero then save_type(save_ptr):=restore_zero
else  begin save_stack[save_ptr]:=eqtb[p]; incr(save_ptr);
  save_type(save_ptr):=restore_old_value;
  end;
save_level(save_ptr):=l; save_index(save_ptr):=p; incr(save_ptr);
end;
@y
@p procedure eq_save(@!p:pointer;@!l:quarterword); {saves |eqtb[p]|}
begin check_full_save_stack;
if l=level_zero then save_type(save_ptr):=restore_zero
else  begin save_stack[save_ptr]:=eqtb[p]; incr(save_ptr);
  if p>=kinsoku_base then begin
    save_stack[save_ptr]:=eqtb[p+1]; incr(save_ptr) end;
  save_type(save_ptr):=restore_old_value;
  end;
save_level(save_ptr):=l; save_index(save_ptr):=p; incr(save_ptr);
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [19.278] eq_dword_define
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure eq_word_define(@!p:pointer;@!w:integer);
begin if xeq_level[p]<>cur_level then
  begin eq_save(p,xeq_level[p]); xeq_level[p]:=cur_level;
  end;
eqtb[p].int:=w;
end;
@y
@p procedure eq_word_define(@!p:pointer;@!w:integer);
begin if xeq_level[p]<>cur_level then
  begin eq_save(p,xeq_level[p]); xeq_level[p]:=cur_level;
  end;
eqtb[p].int:=w;
end;
@#
procedure eq_dword_define
     (@!p:pointer;@!t:quarterword;@!e:integer;x:integer); {new data for |eqtb|}
var cx:KANJI_code;
begin if (eq_level(p)<>cur_level) and (cur_level>level_one) then
  eq_save(p,eq_level(p))
else if (cur_level=level_one) and (x=0) then t:=0;
cx:=tokanji(e);
eq_level(p):=cur_level; eq_type(p):=t; equiv(p):=cx; eqtb[p+1].int:=x;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [19.279] geq_dword_define
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
procedure geq_word_define(@!p:pointer;@!w:integer); {global |eq_word_define|}
begin eqtb[p].int:=w; xeq_level[p]:=level_one;
end;
@y
procedure geq_word_define(@!p:pointer;@!w:integer); {global |eq_word_define|}
begin eqtb[p].int:=w; xeq_level[p]:=level_one;
end;

procedure geq_dword_define
     (@!p:pointer;@!t:quarterword;@!e:integer;x:integer); {global |eq_define|}
var cx:KANJI_code;
begin 
cx:=tokanji(e); if x=0 then t:=0;
eq_level(p):=level_one; eq_type(p):=t; equiv(p):=cx; eqtb[p+1].int:=x;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [19.278] restore dword register
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else if xeq_level[p]<>level_one then
  begin eqtb[p]:=save_stack[save_ptr]; xeq_level[p]:=l;
  @!stat if tracing_restores>0 then restore_trace(p,"restoring");@+tats@;@/
  end
else  begin
  @!stat if tracing_restores>0 then restore_trace(p,"retaining");@+tats@;@/
  end
@y
else if p<kinsoku_base then
  if xeq_level[p]<>level_one then
    begin eqtb[p]:=save_stack[save_ptr]; xeq_level[p]:=l;
    @!stat if tracing_restores>0 then restore_trace(p,"restoring");@+tats@;@/
    end
    else  begin
    @!stat if tracing_restores>0 then restore_trace(p,"retaining");@+tats@;@/
    end
else if eq_level(p)=level_one then begin decr(save_ptr);
    @!stat if tracing_restores>0 then restore_trace(p,"retaining");@+tats@;@/
  end else begin
    eqtb[p+1]:=save_stack[save_ptr]; decr(save_ptr);
    eqtb[p]:=save_stack[save_ptr];
    @!stat if tracing_restores>0 then restore_trace(p,"restoring");@+tats@;@/
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [20.289] cs_token_flag
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d cs_token_flag==@'10000 {amount added to the |eqtb| location in a
  token that stands for a control sequence; is a multiple of~256}
@d left_brace_token=@'0400 {$2^8\cdot|left_brace|$}
@d left_brace_limit=@'1000 {$2^8\cdot(|left_brace|+1)$}
@d right_brace_token=@'1000 {$2^8\cdot|right_brace|$}
@d right_brace_limit=@'1400 {$2^8\cdot(|right_brace|+1)$}
@d math_shift_token=@'1400 {$2^8\cdot|math_shift|$}
@d tab_token=@'2000 {$2^8\cdot|tab_mark|$}
@d out_param_token=@'2400 {$2^8\cdot|out_param|$}
@d space_token=@'5040 {$2^8\cdot|spacer|+|" "|$}
@d letter_token=@'5400 {$2^8\cdot|letter|$}
@d other_token=@'6000 {$2^8\cdot|other_char|$}
@d match_token=@'6400 {$2^8\cdot|match|$}
@d end_match_token=@'7000 {$2^8\cdot|end_match|$}
@y
@d cs_token_flag==@'13000 {amount added to the |eqtb| location in a
  token that stands for a control sequence; is a multiple of~256}
@d left_brace_token=@'0400 {$2^8\cdot|left_brace|$}
@d left_brace_limit=@'1000 {$2^8\cdot(|left_brace|+1)$}
@d right_brace_token=@'1000 {$2^8\cdot|right_brace|$}
@d right_brace_limit=@'1400 {$2^8\cdot(|right_brace|+1)$}
@d math_shift_token=@'1400 {$2^8\cdot|math_shift|$}
@d tab_token=@'2000 {$2^8\cdot|tab_mark|$}
@d out_param_token=@'2400 {$2^8\cdot|out_param|$}
@d space_token=@'5040 {$2^8\cdot|spacer|+|" "|$}
@d letter_token=@'5400 {$2^8\cdot|letter|$}
@d other_token=@'6000 {$2^8\cdot|other_char|$}
@d match_token=@'6400 {$2^8\cdot|match|$}
@d end_match_token=@'7000 {$2^8\cdot|end_match|$}
@d kanji_token_base=@'10000 {$2^8\cdot|kanji|$}
@d jother_token=@'11000 {$2^8\cdot|other_kchar|$}
@d kanji_token_end=@'11400 {$2^8\cdot|other_kchar+1|$}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [20.293] More useless unsigned comparisons to zero
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%  if (info(p)<0)or(c>127) then print_esc("BAD.")
%@y
%  if (c>127) then print_esc("BAD.")
%@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [20.293][20.294] show_token_list
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if info(p)>=cs_token_flag then print_cs(info(p)-cs_token_flag)
else  begin m:=info(p) div @'400; c:=info(p) mod @'400;
  if (info(p)<0)or(c>127) then print_esc("BAD.")
@.BAD@>
  else @<Display the token $(|m|,|c|)$@>;
  end
@y
if info(p)>=cs_token_flag then print_cs(info(p)-cs_token_flag)
else  begin getcmdx(info(p), m, c);
  if (m<kanji)and(c>127) then print_esc("BAD.")
@.BAD@>
  else @<Display the token $(|m|,|c|)$@>;
  end
@z

@x
left_brace,right_brace,math_shift,tab_mark,sup_mark,sub_mark,spacer,
  letter,other_char: print(c);
mac_param: begin print(c); print(c);
  end;
@y
kanji,kana,other_kchar: begin print(c);p:=link(p);
  getcmdx(info(p), m, c);print(c);end;
left_brace,right_brace,math_shift,tab_mark,sup_mark,sub_mark,spacer,
  letter,other_char: print(c);
mac_param: begin print(c); print(c);
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [21.297] jflag cur_jchr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
To handle these situations, which might all be present simultaneously,
\TeX\ uses various stacks that hold information about the incomplete
activities, and there is a finite state control for each level of the
input mechanism. These stacks record the current state of an implicitly
recursive process, but the |get_next| procedure is not recursive.
Therefore it will not be difficult to translate these algorithms into
low-level languages that do not support recursion.

@<Glob...@>=
@!cur_cmd: eight_bits; {current command set by |get_next|}
@!cur_chr: halfword; {operand of current command}
@!cur_cs: pointer; {control sequence found here, zero if none found}
@!cur_tok: halfword; {packed representative of |cur_cmd| and |cur_chr|}
@y
To handle these situations, which might all be present simultaneously,
\TeX\ uses various stacks that hold information about the incomplete
activities, and there is a finite state control for each level of the
input mechanism. These stacks record the current state of an implicitly
recursive process, but the |get_next| procedure is not recursive.
Therefore it will not be difficult to translate these algorithms into
low-level languages that do not support recursion.

@<Glob...@>=
@!cur_cmd: halfword; {current command set by |get_next|}
@!cur_chr: halfword; {operand of current command}
@!cur_cs: pointer; {control sequence found here, zero if none found}
@!cur_tok: halfword; {packed representative of |cur_cmd| and |cur_chr|}
@!cur_jchr: KANJI_code; {operand of current command}
@!last_chr: halfword;
@!jflag: halfword; {kanji flag}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [21.298] print_cmd_chr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
endv: print("end of alignment template");
spacer: chr_cmd("blank space ");
letter: chr_cmd("the letter ");
other_char: chr_cmd("the character ");
@y
endv: print("end of alignment template");
spacer: chr_cmd("blank space ");
letter: chr_cmd("the letter ");
other_char: chr_cmd("the character ");
kana,kanji,other_kchar: print("kanji character");
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [22.303] state
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
The |state| variable has one of three values, when we are scanning such
files:
$$\baselineskip 15pt\vbox{\halign{#\hfil\cr
1) |state=mid_line| is the normal state.\cr
2) |state=skip_blanks| is like |mid_line|, but blanks are ignored.\cr
3) |state=new_line| is the state at the beginning of a line.\cr}}$$
These state values are assigned numeric codes so that if we add the state
code to the next character's command code, we get distinct values. For
example, `|mid_line+spacer|' stands for the case that a blank
space character occurs in the middle of a line when it is not being
ignored; after this case is processed, the next value of |state| will
be |skip_blanks|.

@d mid_line=1 {|state| code when scanning a line of characters}
@d skip_blanks=2+max_char_code {|state| code when ignoring blanks}
@d new_line=3+max_char_code+max_char_code {|state| code at start of line}
@y
The |state| variable has one of four values, when we are scanning such
files:
$$\baselineskip 15pt\vbox{\halign{#\hfil\cr
1) |state=mid_line| is the normal state.\cr
2) |state=mid_kanji| is like |mid_line|, and internal KANJI string.\cr
3) |state=skip_blanks| is like |mid_line|, but blanks are ignored.\cr
4) |state=new_line| is the state at the beginning of a line.\cr}}$$
These state values are assigned numeric codes so that if we add the state
code to the next character's command code, we get distinct values. For
example, `|mid_line+spacer|' stands for the case that a blank
space character occurs in the middle of a line when it is not being
ignored; after this case is processed, the next value of |state| will
be |skip_blanks|.

@d mid_line=1 {|state| code when scanning a line of characters}
@d mid_kanji=2+max_char_code {|state| code when scanning a line of characters}
@d skip_blanks=3+max_char_code+max_char_code {|state| code when ignoring blanks}
@d new_line=4+max_char_code+max_char_code+max_char_code
					{|state| code at start of line}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [22.311] label
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure show_context; {prints where the scanner is}
label done;
@y
@p procedure show_context; {prints where the scanner is}
label done, done1;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [22.315] local vavariable
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!q: integer; {temporary index}
@y
@!q: integer; {temporary index}
@!s: pointer; {temporary pointer}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [22.316] init kcode_pos
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  begin l:=tally; tally:=0; selector:=pseudo;
@y
  begin l:=tally; tally:=0; selector:=pseudo; kcode_pos:=0;
@z

@x
  begin first_count:=tally;
@y
  begin first_count:=tally;
  if first_count>0 then
    if trick_buf2[(first_count-1) mod error_line]=1 then incr(first_count);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [22.317] adjust kanji code pos
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else  begin print("..."); p:=l+first_count-half_error_line+3;
  n:=half_error_line;
@y
else  begin print("..."); p:=l+first_count-half_error_line+3;
  n:=half_error_line;
  if trick_buf2[p mod error_line]=2 then begin p:=p+1; n:=n-1; end;
@z

@x
if m+n<=error_line then p:=first_count+m else p:=first_count+(error_line-n-3);
@y
if m+n<=error_line then p:=first_count+m else begin
  p:=first_count+(error_line-n-3);
  if trick_buf2[(p-1) mod error_line]=1 then p:=p-1;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [22.319] adjust kanji code token
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if token_type<macro then show_token_list(start,loc,100000)
else show_token_list(link(start),loc,100000) {avoid reference count}
@y
if token_type<macro then begin
  if (token_type=backed_up)and(loc<>null) then begin
    if (link(start)=null)and(info(start)>kanji_token_base)
        and(info(start)<kanji_token_end) then begin
      cur_input:=input_stack[base_ptr-1];
      s:=get_avail; info(s):=mktok(0, buffer[loc]);
      cur_input:=input_stack[base_ptr];
      link(start):=s;
      show_token_list(start,loc,100000);
      free_avail(s);link(start):=null;
      goto done1;
    end;
  end;
  show_token_list(start,loc,100000);
end else show_token_list(link(start),loc,100000); {avoid reference count}
done1:
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [23.331] use a different for loop index, local to tex_body
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
first:=buf_size; repeat buffer[first]:=0; decr(first); until first=0;
@y
bufindx:=buf_size; repeat buffer[bufindx]:=0; decr(bufindx); until bufindx=0;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.341] set last_chr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!cat:0..15; {|cat_code(cur_chr)|, usually}
begin restart: cur_cs:=0;
if state<>token_list then
@<Input from external file, |goto restart| if no input found@>
else @<Input from token list, |goto restart| if end of list or
  if a parameter needs to be expanded@>;
@<If an alignment entry has just ended, take appropriate action@>;
exit:end;
@y
@!cat:escape..max_char_code; {|cat_code(cur_chr)|, usually}
begin restart: cur_cs:=0;
if state<>token_list then
@<Input from external file, |goto restart| if no input found@>
else @<Input from token list, |goto restart| if end of list or
  if a parameter needs to be expanded@>;
@<If an alignment entry has just ended, take appropriate action@>;
exit:last_chr:=cur_chr;
end;

@ @<Initialize table entries...@>=
last_chr:=0;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.343] input external file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Input from external file, |goto restart| if no input found@>=
@^inner loop@>
begin switch: if loc<=limit then {current line not yet finished}
  begin cur_chr:=buffer[loc]; incr(loc);
  reswitch: cur_cmd:=cat_code(cur_chr);
  @<Change state if necessary, and |goto switch| if the
    current character should be ignored,
    or |goto reswitch| if the current character
    changes to another@>;
  end
else  begin state:=new_line;@/
  @<Move to next line of file,
    or |goto restart| if there is no next line,
    or |return| if a \.{\\read} line has finished@>;
  check_interrupt;
  goto switch;
  end;
end
@y
@ @<Input from external file, |goto restart| if no input found@>=
@^inner loop@>
begin switch: if loc<=limit then {current line not yet finished}
  begin cur_chr:=buffer[loc]; incr(loc);
  if jflag = 1  then begin jflag:=0; goto exit; end;
  reswitch: cur_cmd:=cat_code(cur_chr);
  if (cur_cmd >= kanji)and(cur_cmd <= other_kchar) then incr(jflag);
  @<Change state if necessary, and |goto switch| if the
    current character should be ignored,
    or |goto reswitch| if the current character
    changes to another@>;
  end
else  begin state:=new_line;@/
  @<Move to next line of file,
    or |goto restart| if there is no next line,
    or |return| if a \.{\\read} line has finished@>;
  check_interrupt;
  goto switch;
  end;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.344][24.345] ASCII - KANJI space handling
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The following 48-way switch accomplishes the scanning quickly, assuming
that a decent \PASCAL\ compiler has translated the code. Note that the numeric
values for |mid_line|, |skip_blanks|, and |new_line| are spaced
apart from each other by |max_char_code+1|, so we can add a character's
command code to the state to get a single number that characterizes both.

@d any_state_plus(#) == mid_line+#,skip_blanks+#,new_line+#

@<Change state if necessary...@>=
case state+cur_cmd of
@<Cases where character is ignored@>: goto switch;
any_state_plus(escape): @<Scan a control sequence
  and set |state:=skip_blanks| or |mid_line|@>;
any_state_plus(active_char): @<Process an active-character control sequence
  and set |state:=mid_line|@>;
any_state_plus(sup_mark): @<If this |sup_mark| starts a control character
  like~\.{\^\^A}, then |goto reswitch|, otherwise set |state:=mid_line|@>;
any_state_plus(invalid_char): @<Decry the invalid character and
  |goto restart|@>;
@t\4@>@<Handle situations involving spaces, braces, changes of state@>@;
othercases do_nothing
endcases

@ @<Cases where character is ignored@>=
any_state_plus(ignore),skip_blanks+spacer,new_line+spacer
@y
@ The following 88-way switch accomplishes the scanning quickly, assuming
that a decent \PASCAL\ compiler has translated the code. Note that the numeric
values for |mid_line|, |skip_blanks|, and |new_line| are spaced
apart from each other by |max_char_code+1|, so we can add a character's
command code to the state to get a single number that characterizes both.

@d any_state_plus(#) == mid_line+#,mid_kanji+#,skip_blanks+#,new_line+#

@<Change state if necessary...@>=
case state+cur_cmd of
@<Cases where character is ignored@>: goto switch;
any_state_plus(escape): @<Scan a control sequence
  and set |state:=skip_blanks| or |mid_line|@>;
any_state_plus(active_char): @<Process an active-character control sequence
  and set |state:=mid_line|@>;
any_state_plus(sup_mark): @<If this |sup_mark| starts a control character
  like~\.{\^\^A}, then |goto reswitch|, otherwise set |state:=mid_line|@>;
any_state_plus(invalid_char): @<Decry the invalid character and
  |goto restart|@>;
@t\4@>@<Handle situations involving spaces, braces, changes of state@>@;
othercases do_nothing
endcases

@ @<Cases where character is ignored@>=
any_state_plus(ignore),skip_blanks+spacer,new_line+spacer

@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.347] scaner
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @d add_delims_to(#)==#+math_shift,#+tab_mark,#+mac_param,
  #+sub_mark,#+letter,#+other_char

@<Handle situations involving spaces, braces, changes of state@>=
mid_line+spacer:@<Enter |skip_blanks| state, emit a space@>;
mid_line+car_ret:@<Finish line, emit a space@>;
skip_blanks+car_ret,any_state_plus(comment):
  @<Finish line, |goto switch|@>;
new_line+car_ret:@<Finish line, emit a \.{\\par}@>;
mid_line+left_brace: incr(align_state);
skip_blanks+left_brace,new_line+left_brace: begin
  state:=mid_line; incr(align_state);
  end;
mid_line+right_brace: decr(align_state);
skip_blanks+right_brace,new_line+right_brace: begin
  state:=mid_line; decr(align_state);
  end;
add_delims_to(skip_blanks),add_delims_to(new_line): state:=mid_line;
@y
@ @d add_delims_to(#)==#+math_shift,#+tab_mark,#+mac_param,
  #+sub_mark,#+letter,#+other_char
@d all_jcode(#)==#+kana,#+other_kchar,#+kanji

@<Handle situations involving spaces, braces, changes of state@>=
mid_kanji+spacer,mid_line+spacer:@<Enter |skip_blanks| state, emit a space@>;
mid_line+car_ret:@<Finish line, emit a space@>;
mid_kanji+car_ret:@<If skip_mode Finish line, |goto switch|@>;
skip_blanks+car_ret,any_state_plus(comment):
  @<Finish line, |goto switch|@>;
new_line+car_ret:@<Finish line, emit a \.{\\par}@>;
mid_line+left_brace,mid_kanji+left_brace: incr(align_state);
skip_blanks+left_brace,new_line+left_brace: begin
  state:=mid_line; incr(align_state);
  end;
mid_line+right_brace,mid_kanji+right_brace: decr(align_state);
skip_blanks+right_brace,new_line+right_brace: begin
  state:=mid_line; decr(align_state);
  end;
add_delims_to(skip_blanks),add_delims_to(new_line),add_delims_to(mid_kanji):
  state:=mid_line;
all_jcode(skip_blanks),all_jcode(new_line),all_jcode(mid_line):state:=mid_kanji;

@ @<If skip_mode Finish line, |goto switch|@>=
if skip_mode then @<Finish line, |goto switch|@>
else @<Finish line, emit a space@>

@ @<Global...@>=
skip_mode:boolean;

@ @<Set init...@>=
skip_mode:=true;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.354] scan control sequence
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%begin if loc>limit then cur_cs:=null_cs {|state| is irrelevant in this case}
%else  begin start_cs: k:=loc; cur_chr:=buffer[k]; cat:=cat_code(cur_chr);
%  incr(k);
%  if cat=letter then state:=skip_blanks
%  else if cat=spacer then state:=skip_blanks
%  else state:=mid_line;
%  if (cat=letter)and(k<=limit) then
%@y
%begin if loc>limit then cur_cs:=null_cs {|state| is irrelevant in this case}
%else  begin start_cs: k:=loc; cur_chr:=buffer[k]; cat:=cat_code(cur_chr);
%  incr(k);
%  if (cat=letter) or (cat=kanji) or (cat=kana) then state:=skip_blanks
%  else if cat=spacer then state:=skip_blanks
%  else state:=mid_line;
%  if (cat=kanji)or(cat=kana) then incr(k);
%  if ((cat=letter)or(cat=kanji)or(cat=kana))and(k<=limit) then
%@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.354] scan control sequence
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% other_kchar 1 文字のコントロールシーケンスの扱い
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin if loc>limit then cur_cs:=null_cs {|state| is irrelevant in this case}
else  begin start_cs: k:=loc; cur_chr:=buffer[k]; cat:=cat_code(cur_chr);
  incr(k);
  if cat=letter then state:=skip_blanks
  else if cat=spacer then state:=skip_blanks
  else state:=mid_line;
  if (cat=letter)and(k<=limit) then
@y
begin if loc>limit then cur_cs:=null_cs {|state| is irrelevant in this case}
else  begin start_cs: k:=loc; cur_chr:=buffer[k]; cat:=cat_code(cur_chr);
  incr(k);
  if (cat=kanji)or(cat=kana)or(cat=other_kchar) then incr(k);
  if (cat=letter)or(cat=kanji)or(cat=kana) then state:=skip_blanks
  else if cat=spacer then state:=skip_blanks
  else state:=mid_line;
  if (cat=other_kchar)and(k<=limit) then begin
    cur_cs:=id_lookup(loc,k-loc); loc:=k; goto found;
  end else if ((cat=letter)or(cat=kanji)or(cat=kana))and(k<=limit) then
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.356] scan control sequence (cont)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin repeat cur_chr:=buffer[k]; cat:=cat_code(cur_chr); incr(k);
until (cat<>letter)or(k>limit);
@<If an expanded...@>;
if cat<>letter then decr(k);
  {now |k| points to first nonletter}
if k>loc+1 then {multiletter control sequence has been scanned}
  begin cur_cs:=id_lookup(loc,k-loc); loc:=k; goto found;
  end;
end
@y
begin repeat cur_chr:=buffer[k]; cat:=cat_code(cur_chr);
if (cat=kanji)or(cat=kana) then incr2(k) else incr(k);
until not((cat=letter)or(cat=kanji)or(cat=kana))or(k>limit);
@<If an expanded...@>;
if not((cat=letter)or(cat=kanji)or(cat=kana)) then decr(k);
  {now |k| points to first nonletter}
if k>loc+1 then {multiletter control sequence has been scanned}
  begin cur_cs:=id_lookup(loc,k-loc); loc:=k; goto found;
  end;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.357] input from token list
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
      if cur_cmd=dont_expand then
        @<Get the next token, suppressing expansion@>
      else check_outer_validity;
    end
  else  begin cur_cmd:=t div @'400; cur_chr:=t mod @'400;
    case cur_cmd of
    left_brace: incr(align_state);
@y
      if cur_cmd=dont_expand then
        @<Get the next token, suppressing expansion@>
      else check_outer_validity;
    end
  else  begin get_cmd(t); {cur_cmd:=t div @'400; cur_chr:=t mod @'400;}
    case cur_cmd of
    left_brace: incr(align_state);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [24.365] get_token
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if cur_cs=0 then cur_tok:=(cur_cmd*@'400)+cur_chr
else cur_tok:=cs_token_flag+cur_cs;
end;
@y
if cur_cs=0 then set_tok
else cur_tok:=cs_token_flag+cur_cs;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [25.367] inhibit_glue
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%@x
%input:@<Initiate or terminate input from a file@>;
%othercases @<Complain about an undefined macro@>
%@y
%input:@<Initiate or terminate input from a file@>;
%inhibit_glue:inhibit_glue_flag:=true;
%othercases @<Complain about an undefined macro@>
%@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [25.374] get_chr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@:TeX capacity exceeded buffer size}{\quad buffer size@>
    end;
  buffer[j]:=info(p) mod @'400; incr(j); p:=link(p);
@y
@:TeX capacity exceeded buffer size}{\quad buffer size@>
    end;
  buffer[j]:=get_chr(info(p)); {info(p) mod @'400;} incr(j); p:=link(p);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [25.380] get_x_token
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
goto restart;
done: if cur_cs=0 then cur_tok:=(cur_cmd*@'400)+cur_chr
else cur_tok:=cs_token_flag+cur_cs;
end;
@y
goto restart;
done: if cur_cs=0 then set_tok {cur_tok:=(cur_cmd*@'400)+cur_chr}
else cur_tok:=cs_token_flag+cur_cs;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [25.381] x_token
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure x_token; {|get_x_token| without the initial |get_next|}
begin while cur_cmd>max_command do
  begin expand;
  get_next;
  end;
if cur_cs=0 then cur_tok:=(cur_cmd*@'400)+cur_chr
else cur_tok:=cs_token_flag+cur_cs;
end;
@y
@p procedure x_token; {|get_x_token| without the initial |get_next|}
begin while cur_cmd>max_command do
  begin expand;
  get_next;
  end;
if cur_cs=0 then set_tok {cur_tok:=(cur_cmd*@'400)+cur_chr}
else cur_tok:=cs_token_flag+cur_cs;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [25.393] store 2nd KANJI token at macro parameter
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
    if info(r)>=match_token then goto continue;
store_new_token(cur_tok);
end
@y
    if info(r)>=match_token then goto continue;
store_new_token(cur_tok);
if (cur_tok>kanji_token_base)and(cur_tok<kanji_token_end) then
begin get_token; store_new_token(cur_tok); end;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [26.413] scan_somthing_internal
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure scan_something_internal(@!level:small_number;@!negative:boolean);
  {fetch an internal parameter}
var m:halfword; {|chr_code| part of the operand token}
@!p:0..nest_size; {index into |nest|}
begin m:=cur_chr;
case cur_cmd of
def_code: @<Fetch a character code from some table@>;
toks_register,assign_toks,def_family,set_font,def_font: @<Fetch a token list or
  font identifier, provided that |level=tok_val|@>;
@y
@p @t\4@>@<Declare procedures needed in |scan_something|@>@t@>@/
procedure scan_something_internal(@!level:small_number;@!negative:boolean);
  {fetch an internal parameter}
var m:halfword; {|chr_code| part of the operand token}
@!p:0..nest_size; {index into |nest|}
@!q:pointer;
begin m:=cur_chr;
case cur_cmd of
assign_kinsoku: @<Fetch breaking penalty from some table@>;
assign_inhibit_xsp_code: @<Fetch inhibit type from some table@>;
def_code: @<Fetch a character code from some table@>;
toks_register,assign_toks,def_family,set_font,def_font,def_jfont:
  @<Fetch a token list or font identifier, provided that |level=tok_val|@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [26.435] scan_char_num
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
procedure scan_char_num;
begin scan_int;
if (cur_val<0)or(cur_val>255) then
  begin print_err("Bad character code");
@.Bad character code@>
  help2("A character number must be between 0 and 255.")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end;
end;
@y
procedure scan_char_num;
var cx:KANJI_code;
begin scan_int;
if not checkkanji(cur_val) then
  begin print_err("Bad character code");
@.Bad character code@>
  help2("A character number must be between 0 and 255, or KANJI code.")@/
    ("I changed this one to zero."); int_error(cur_val); cur_val:=0;
  end else if (cur_val<0)or(cur_val>256) then
    begin cx:=tokanji(cur_val);cur_val:=KANJI(cx);end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [26.440] scan_int
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure scan_int; {sets |cur_val| to an integer}
label done;
var negative:boolean; {should the answer be negated?}
@!m:integer; {|@t$2^{31}$@> div radix|, the threshold of danger}
@!d:small_number; {the digit just scanned}
@!vacuous:boolean; {have no digits appeared?}
@!OK_so_far:boolean; {has an error message been issued?}
begin radix:=0; OK_so_far:=true;@/
@y
@p procedure scan_int; {sets |cur_val| to an integer}
label done;
var negative:boolean; {should the answer be negated?}
@!m:integer; {|@t$2^{31}$@> div radix|, the threshold of danger}
@!d:small_number; {the digit just scanned}
@!vacuous:boolean; {have no digits appeared?}
@!OK_so_far:boolean; {has an error message been issued?}
@!cx:KANJI_code;
begin cx:=0; radix:=0; OK_so_far:=true;@/
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [26.442] KANJI caracter scanning
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Scan an alphabetic character code into |cur_val|@>=
begin get_token; {suppress macro expansion}
if cur_tok<cs_token_flag then
  begin cur_val:=cur_chr;
  if cur_cmd<=right_brace then
    if cur_cmd=right_brace then incr(align_state)
    else decr(align_state);
  end
else if cur_tok<cs_token_flag+single_base then
  cur_val:=cur_tok-cs_token_flag-active_base
else cur_val:=cur_tok-cs_token_flag-single_base;
if cur_val>127 then
  begin print_err("Improper alphabetic constant");
@.Improper alphabetic constant@>
  help2("A one-character control sequence belongs after a ` mark.")@/
    ("So I'm essentially inserting \0 here.");
  cur_val:="0"; back_error;
  end
else @<Scan an optional space@>;
end
@y
@<Scan an alphabetic character code into |cur_val|@>=
begin get_token; {suppress macro expansion}
if cur_tok<cs_token_flag then
  if (cur_cmd>=kanji) and (cur_cmd <= other_kchar) then
    begin skip_mode:=false;
      PutHi(cx,cur_chr); get_next; PutLo(cx,cur_chr);
      cur_val:=tonum(cx);
    end else begin cur_val:=cur_chr;
      if cur_cmd<=right_brace then
        if cur_cmd=right_brace then incr(align_state)
        else decr(align_state);
    end
else if cur_tok<cs_token_flag+single_base then
  cur_val:=cur_tok-cs_token_flag-active_base
else cur_val:=cur_tok-cs_token_flag-single_base;
if (cur_val>127) and (cx=0) then
  begin print_err("Improper alphabetic or KANJI constant");
@.Improper alphabetic constant@>
  help2("A one-character control sequence belongs after a ` mark.")@/
    ("So I'm essentially inserting \0 here.");
  cur_val:="0"; back_error;
  end
else @<Scan an optional space@>;
skip_mode:=true;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [26.455] zw, zh: zenkaku width & height
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if scan_keyword("em") then v:=(@<The em width for |cur_font|@>)
@.em@>
else if scan_keyword("ex") then v:=(@<The x-height for |cur_font|@>)
@.ex@>
else goto not_found;
@y
if scan_keyword("em") then v:=(@<The em width for |cur_font|@>)
@.em@>
else if scan_keyword("ex") then v:=(@<The x-height for |cur_font|@>)
@.ex@>
else if scan_keyword("zw") then @<The KANJI width for |cur_jfont|@>
@.ex@>
else if scan_keyword("zh") then @<The KANJI height for |cur_jfont|@>
@.ex@>
else goto not_found;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [27.464] str_toks
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  if t=" " then t:=space_token
  else t:=other_token+t;
  fast_store_new_token(t);
  incr(k);
  end;
@y
  if (t<0)or(t>127) then begin
  if t<0 then t:=t+256; t:=jother_token+t;
  fast_store_new_token(t); incr(k);
  t:=str_pool[k]; if t<0 then t:=t+256; t:=jother_token+t;
  end else if t=" " then t:=space_token
  else t:=other_token+t;
  fast_store_new_token(t);
  incr(k);
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [27.468][27.469] convert KANJI code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d number_code=0 {command code for \.{\\number}}
@d roman_numeral_code=1 {command code for \.{\\romannumeral}}
@d string_code=2 {command code for \.{\\string}}
@d meaning_code=3 {command code for \.{\\meaning}}
@d font_name_code=4 {command code for \.{\\fontname}}
@d job_name_code=5 {command code for \.{\\jobname}}

@<Put each...@>=
primitive("number",convert,number_code);@/
@!@:number_}{\.{\\number} primitive@>
primitive("romannumeral",convert,roman_numeral_code);@/
@!@:roman_numeral_}{\.{\\romannumeral} primitive@>
primitive("string",convert,string_code);@/
@!@:string_}{\.{\\string} primitive@>
primitive("meaning",convert,meaning_code);@/
@!@:meaning_}{\.{\\meaning} primitive@>
primitive("fontname",convert,font_name_code);@/
@!@:font_name_}{\.{\\fontname} primitive@>
primitive("jobname",convert,job_name_code);@/
@!@:job_name_}{\.{\\jobname} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
convert: case chr_code of
  number_code: print_esc("number");
  roman_numeral_code: print_esc("romannumeral");
  string_code: print_esc("string");
  meaning_code: print_esc("meaning");
  font_name_code: print_esc("fontname");
  othercases print_esc("jobname")
  endcases;
@y
@d number_code=0 {command code for \.{\\number}}
@d roman_numeral_code=1 {command code for \.{\\romannumeral}}
@d string_code=2 {command code for \.{\\string}}
@d meaning_code=3 {command code for \.{\\meaning}}
@d font_name_code=4 {command code for \.{\\fontname}}
@d sjis_code=5 {command code for \.{\\sjis}}
@d jis_code=6 {command code for \.{\\jis}}
@d kuten_code=7 {command code for \.{\\kuten}}
@d job_name_code=8 {command code for \.{\\jobname}}

@<Put each...@>=
primitive("number",convert,number_code);@/
@!@:number_}{\.{\\number} primitive@>
primitive("romannumeral",convert,roman_numeral_code);@/
@!@:roman_numeral_}{\.{\\romannumeral} primitive@>
primitive("string",convert,string_code);@/
@!@:string_}{\.{\\string} primitive@>
primitive("meaning",convert,meaning_code);@/
@!@:meaning_}{\.{\\meaning} primitive@>
primitive("fontname",convert,font_name_code);@/
@!@:font_name_}{\.{\\fontname} primitive@>
primitive("jobname",convert,job_name_code);@/
@!@:job_name_}{\.{\\jobname} primitive@>
primitive("sjis",convert,sjis_code);
@!@:sjis_}{\.{\\sjis} primitive@>
primitive("jis",convert,jis_code);
@!@:jis_}{\.{\\jis} primitive@>
primitive("kuten",convert,kuten_code);
@!@:kuten_}{\.{\\jis} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
convert: case chr_code of
  number_code: print_esc("number");
  roman_numeral_code: print_esc("romannumeral");
  string_code: print_esc("string");
  meaning_code: print_esc("meaning");
  font_name_code: print_esc("fontname");
  sjis_code:print_esc("sjis");
  jis_code:print_esc("jis");
  kuten_code:print_esc("kuten");
  othercases print_esc("jobname")
  endcases;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [27.470][27.471][27.472] convert KANJI code continue
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure conv_toks;
var old_setting:0..max_selector; {holds |selector| setting}
@!c:number_code..job_name_code; {desired type of conversion}
@!save_scanner_status:small_number; {|scanner_status| upon entry}
@y
@p procedure conv_toks;
var old_setting:0..max_selector; {holds |selector| setting}
@!c:number_code..job_name_code; {desired type of conversion}
@!save_scanner_status:small_number; {|scanner_status| upon entry}
@!cx:KANJI_code;
@z

@x
@ @<Scan the argument for command |c|@>=
case c of
number_code,roman_numeral_code: scan_int;
string_code, meaning_code: begin save_scanner_status:=scanner_status;
  scanner_status:=normal; get_token; scanner_status:=save_scanner_status;
  end;
font_name_code: scan_font_ident;
job_name_code: if job_name=0 then open_log_file;
end {there are no other cases}

@ @<Print the result of command |c|@>=
case c of
number_code: print_int(cur_val);
roman_numeral_code: print_roman_int(cur_val);
string_code:if cur_cs<>0 then sprint_cs(cur_cs)
  else print_char(cur_chr);
meaning_code: print_meaning;
font_name_code: begin print(font_name[cur_val]);
  if font_size[cur_val]<>font_dsize[cur_val] then
    begin print(" at "); print_scaled(font_size[cur_val]);
    print("pt");
    end;
  end;
job_name_code: print(job_name);
end {there are no other cases}
@y
@ @<Scan the argument for command |c|@>=
KANJI(cx):=0;
case c of
sjis_code,jis_code,kuten_code,number_code,roman_numeral_code: scan_int;
string_code, meaning_code: begin save_scanner_status:=scanner_status;
  scanner_status:=normal; get_token;
  if (cur_cmd >= kanji) and (cur_cmd <= other_kchar) then
    begin PutHi(cx, cur_chr); get_token; end;
   scanner_status:=save_scanner_status;
  end;
font_name_code: scan_font_ident;
job_name_code: if job_name=0 then open_log_file;
end {there are no other cases}

@ @<Print the result of command |c|@>=
case c of
jis_code: begin cur_val:=JIStoSJIS(cur_val); print_int(cur_val); end;
kuten_code: begin cur_val:=KUTENtoSJIS(cur_val); print_int(cur_val); end;
sjis_code,number_code: print_int(cur_val);
roman_numeral_code: print_roman_int(cur_val);
string_code:if cur_cs<>0 then sprint_cs(cur_cs)
  else begin if KANJI(cx) <> 0 then print_char(Hi(cx));
     print_char(cur_chr); end;
meaning_code: print_meaning;
font_name_code: begin print(font_name[cur_val]);
  if font_size[cur_val]<>font_dsize[cur_val] then
    begin print(" at "); print_scaled(font_size[cur_val]);
    print("pt");
    end;
  end;
job_name_code: print(job_name);
end {there are no other cases}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [28.498] conditional: temp var for ifx
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure conditional;
label exit,common_ending;
var b:boolean; {is the condition true?}
@!r:"<"..">"; {relation to be evaluated}
@!m,@!n:integer; {to be tested against the second operand}
@!p,@!q:pointer; {for traversing token lists in \.{\\ifx} tests}
@!save_scanner_status:small_number; {|scanner_status| upon entry}
@!save_cond_ptr:pointer; {|cond_ptr| corresponding to this conditional}
@!this_if:small_number; {type of this conditional}
begin @<Push the condition stack@>;@+save_cond_ptr:=cond_ptr;this_if:=cur_chr;@/
@y
@p procedure conditional;
label exit,common_ending;
var b:boolean; {is the condition true?}
  @!r:"<"..">"; {relation to be evaluated}
  @!m,@!n:integer; {to be tested against the second operand}
  @!p,@!q:pointer; {for traversing token lists in \.{\\ifx} tests}
  @!x:integer; {to be tested against the second operand}
  @!y,@!z:pointer; {for traversing token lists in \.{\\ifx} tests}
  @!save_scanner_status:small_number; {|scanner_status| upon entry}
  @!save_cond_ptr:pointer; {|cond_ptr| corresponding to this conditional}
  @!this_if:small_number; {type of this conditional}
begin @<Push the condition stack@>;@+save_cond_ptr:=cond_ptr;this_if:=cur_chr;@/
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [28.507] ifx : Test token : kanji token
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ifx での 漢字トークンの扱い.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Test if two tokens match@>=
begin save_scanner_status:=scanner_status; scanner_status:=normal;
get_next; n:=cur_cs; p:=cur_cmd; q:=cur_chr;
get_next; if cur_cmd<>p then b:=false
else if cur_cmd<call then b:=(cur_chr=q)
else @<Test if two macro texts match@>;
scanner_status:=save_scanner_status;
end
@y
@<Test if two tokens match@>=
  begin
    save_scanner_status:=scanner_status; scanner_status:=normal;
    get_next; n:=cur_cs; p:=cur_cmd; q:=cur_chr;
    if (cur_cmd=kanji)or(cur_cmd=kana)or(cur_cmd=other_kchar) then begin
      getnext; q:=256*q+cur_chr;
    end;
    get_next; z:=cur_chr;
    if (cur_cmd=kanji)or(cur_cmd=kana)or(cur_cmd=other_kchar) then begin
      getnext; cur_chr:=256*z+cur_chr;
    end;
    if cur_cmd<>p then b:=false
    else if cur_cmd<call then b:=(cur_chr=q)
    else @<Test if two macro texts match@>;
    scanner_status:=save_scanner_status;
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.513] area and extension rules
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The file names we shall deal with for illustrative purposes have the
following structure:  If the name contains `\.>' or `\.:', the file area
consists of all characters up to and including the final such character;
otherwise the file area is null.  If the remaining file name contains
`\..', the file extension consists of all such characters from the first
remaining `\..' to the end, otherwise the file extension is null.
@^system dependencies@>

We can scan such file names easily by using two global variables that keep track
of the occurrences of area and extension delimiters:

@<Glob...@>=
@!area_delimiter:pool_pointer; {the most recent `\.>' or `\.:', if any}
@!ext_delimiter:pool_pointer; {the relevant `\..', if any}
@y
@ The file names we shall deal with for illustrative purposes have the
following structure:  If the name contains `\./', the file area
consists of all characters up to and including the final such character;
otherwise the file area is null.  If the remaining file name contains
`\..', the file extension consists of all such characters from the first
remaining `\..' to the end, otherwise the file extension is null.
@^system dependencies@>

We can scan such file names easily by using two global variables that keep
track of the occurrences of area and extension delimiters:

@<Glob...@>=
@!area_delimiter:pool_pointer; {the most recent `\./', if any}
@!ext_delimiter:pool_pointer; {the most recent `\..', if any}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.514] TEX area directories
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d TEX_area=="TeXinputs:"
@.TeXinputs@>
@d TEX_font_area=="TeXfonts:"
@.TeXfonts@>
@y
We'll handle the path stuff in an external C module.
@z


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.516] more_name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else  begin if (c=">")or(c=":") then
@y
else  begin if (c="/") then
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.519] xchr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d append_to_name(#)==begin c:=#; incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr[c];
  end
@y
@d append_to_name(#)==begin c:=#; incr(k);
  if k<=file_name_size then name_of_file[k]:=xchr(c);
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.520] default format
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d format_default_length=20 {length of the |TEX_format_default| string}
@d format_area_length=11 {length of its area part}
@d format_ext_length=4 {length of its `\.{.fmt}' part}
@y
Under Berkeley {\mc UNIX} we don't give the area part, instead depending
on the path searching that will happen during file opening.

@d format_default_length=9 {length of the |TEX_format_default| string}
@d format_area_length=0 {length of its area part}
@d format_ext_length=4 {length of its `\.{.fmt}' part}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.521] plain format location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!TEX_format_default:packed array[1..format_default_length] of char;

@ @<Set init...@>=
TEX_format_default:='TeXformats:plain.fmt';
@y
@!TEX_format_default:c_char_pointer;

@ @<Set init...@>=
TEX_format_default := ' plain.fmt';
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.523] xord
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure pack_buffered_name(@!n:small_number;@!a,@!b:integer);
var k:integer; {number of positions filled in |name_of_file|}
@!c: ASCII_code; {character being packed}
@!j:integer; {index into |buffer| or |TEX_format_default|}
begin if n+b-a+1+format_ext_length>file_name_size then
  b:=a+file_name_size-n-1-format_ext_length;
k:=0;
for j:=1 to n do append_to_name(xord[TEX_format_default[j]]);
for j:=a to b do append_to_name(buffer[j]);
for j:=format_default_length-format_ext_length+1 to format_default_length do
  append_to_name(xord[TEX_format_default[j]]);
if k<=file_name_size then name_length:=k@+else name_length:=file_name_size;
for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
end;
@y
@p procedure pack_buffered_name(@!n:small_number;@!a,@!b:integer);
var k:integer; {number of positions filled in |name_of_file|}
@!c: ASCII_code; {character being packed}
@!j:integer; {index into |buffer| or |TEX_format_default|}
begin if n+b-a+1+format_ext_length>file_name_size then
  b:=a+file_name_size-n-1-format_ext_length;
k:=0;
for j:=1 to n do append_to_name(xord(TEX_format_default[j]));
for j:=a to b do append_to_name(buffer[j]);
for j:=format_default_length-format_ext_length+1 to format_default_length do
  append_to_name(xord(TEX_format_default[j]));
if k<=file_name_size then name_length:=k@+else name_length:=file_name_size;
for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.524] format file opening: only try once, with path search
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  pack_buffered_name(0,loc,j-1); {try first without the system file area}
  if w_open_in(fmt_file) then goto found;
  pack_buffered_name(format_area_length,loc,j-1);
    {now try the system format file area}
  if w_open_in(fmt_file) then goto found;
@y
  pack_buffered_name(0,loc,j-1);
  if w_open_in(fmt_file) then goto found;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.525] make_name_string
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
which simply makes a \TeX\ string from the value of |name_of_file|, should
ideally be changed to deduce the full name of file~|f|, which is the file
most recently opened, if it is possible to do this in a \PASCAL\ program.
@^system dependencies@>

This routine might be called after string memory has overflowed, hence
we dare not use `|str_room|'.

@p function make_name_string:str_number;
var k:1..file_name_size; {index into |name_of_file|}
begin if (pool_ptr+name_length>pool_size)or(str_ptr=max_strings)or
 (cur_length>0) then
  make_name_string:="?"
else  begin for k:=1 to name_length do append_char(xord[name_of_file[k]]);
  make_name_string:=make_string;
  end;
end;
function a_make_name_string(var f:alpha_file):str_number;
begin a_make_name_string:=make_name_string;
end;
function b_make_name_string(var f:byte_file):str_number;
begin b_make_name_string:=make_name_string;
end;
function w_make_name_string(var f:word_file):str_number;
begin w_make_name_string:=make_name_string;
end;
@y
which simply makes a \TeX\ string from the value of |name_of_file|, should
ideally be changed to deduce the full name of file~|f|, if it is
possible to do this in a \PASCAL\ program.

With the C version, we know that |real_name_of_file|
contains |name_of_file| prepended with the directory name that was found
by path searching.

If |real_name_of_file| starts with |'./'|, we don't use that part of the
name, since {\mc UNIX} users understand that.
@^system dependencies@>

@p function make_name_string:str_number;
var k,@!kstart:1..file_name_size; {index into |name_of_file|}
begin
k:=1;
while (k<file_name_size) and (xord(real_name_of_file[k])<>" ") do
    incr(k);
name_length:=k-1; {the real |name_length|}
str_room(name_length);
if (xord(real_name_of_file[1])=".") and (xord(real_name_of_file[2])="/") then
    kstart:=3
else
    kstart:=1;
for k:=kstart to name_length do append_char(xord(real_name_of_file[k]));
make_name_string:=make_string;
end;

{The X_make_name_string functions are changed to macros in C}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.526] scan file name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure scan_file_name;
label done;
begin name_in_progress:=true; begin_name;
@<Get the next non-blank non-call...@>;
loop@+begin if (cur_cmd>other_char)or(cur_chr>127) then {not a character}
    begin back_input; goto done;
    end;
  if not more_name(cur_chr) then goto done;
  get_x_token;
  end;
done: end_name; name_in_progress:=false;
end;
@y
@p procedure scan_file_name;
label done;
begin name_in_progress:=true; begin_name;
@<Get the next non-blank non-call...@>;
skip_mode:=false;
loop@+begin if ((cur_cmd>other_char) and
	((cur_cmd<kanji) or (cur_cmd>other_kchar))) or
	(cur_chr>255) then {not a character}
    begin back_input; goto done;
    end;
  if (cur_cmd>=kanji) and (cur_cmd<=other_kchar) then 
  begin
    str_room(2); append_char(cur_chr);
    get_x_token; append_char(cur_chr);
  end else if not more_name(cur_chr) then goto done;
  get_x_token;
  end;
done: end_name; name_in_progress:=false;
skip_mode:=true;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.534][29.536] Adjust for C string conventions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!months:packed array [1..36] of char; {abbreviations of month names}
@y
@!months:c_char_pointer;
@z

@x
months:='JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC';
@y
months := ' JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC';
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.537] a_open_in of \input file needs path selector
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  if a_open_in(cur_file) then goto done;
  if cur_area="" then
    begin pack_file_name(cur_name,TEX_area,cur_ext);
    if a_open_in(cur_file) then goto done;
    end;
@y
  if a_open_in(cur_file,input_path_spec) then goto done;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [29.537] get rid of return of name to string pool
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if name=str_ptr-1 then {we can conserve string pool space now}
  begin flush_string; name:=cur_name;
  end;
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.544] gk_tag
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d no_tag=0 {vanilla character}
@d lig_tag=1 {character has a ligature/kerning program}
@d list_tag=2 {character has a successor in a charlist}
@d ext_tag=3 {character is extensible}
@y
@d no_tag=0 {vanilla character}
@d lig_tag=1 {character has a ligature/kerning program}
@d gk_tag=1 {character has a glue/kerning program}
@d list_tag=2 {character has a successor in a charlist}
@d ext_tag=3 {character is extensible}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.549] font_jtype
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!font_used:array[internal_font_number] of boolean;
  {has a character from this font actually appeared in the output?}
@!hyphen_char:array[internal_font_number] of integer;
  {current \.{\\hyphenchar} values}
@!skew_char:array[internal_font_number] of integer;
  {current \.{\\skewchar} values}
@y
@!font_jtype:array[internal_font_number] of boolean;
@!font_used:array[internal_font_number] of boolean;
  {has a character from this font actually appeared in the output?}
@!font_num_ext:array[internal_font_number] of halfword;
@!hyphen_char:array[internal_font_number] of integer;
  {current \.{\\hyphenchar} values}
@!skew_char:array[internal_font_number] of integer;
  {current \.{\\skewchar} values}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.550] ctype_base
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!param_base:array[internal_font_number] of integer;
  {base addresses for font parameters}
@y
@!param_base:array[internal_font_number] of integer;
  {base addresses for font parameters}
@!ctype_base:array[internal_font_number] of integer;
  {base addresses for font parameters}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.552] init null_font
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
italic_base[null_font]:=0; lig_kern_base[null_font]:=0;
kern_base[null_font]:=0; exten_base[null_font]:=0;
font_glue[null_font]:=null; font_params[null_font]:=7;
@y
italic_base[null_font]:=0; lig_kern_base[null_font]:=0;
kern_base[null_font]:=0; exten_base[null_font]:=0;
ctype_base[null_font]:=0;
font_glue[null_font]:=null; font_params[null_font]:=7;
for k:=font_base to font_max do font_jtype[k]:=false;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.557] glue_kern_start
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d lig_kern_start(#)==lig_kern_base[#]+rem_byte {beginning of lig/kern program}
@y
@d glue_kern_base==lig_kern_base

@d lig_kern_start(#)==lig_kern_base[#]+rem_byte {beginning of lig/kern program}
@d glue_kern_start(#)==glue_kern_base[#]+rem_byte {beginning of glue/kern program}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.560] jfm_flag
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function read_font_info(@!u:pointer;@!nom,@!aire:str_number;
  @!s:scaled):internal_font_number; {input a \.{TFM} file}
label done,bad_tfm,not_found;
var k:0..font_mem_size; {index into |font_info|}
@!file_opened:boolean; {was |tfm_file| successfully opened?}
@y
@p function read_font_info(@!u:pointer;@!nom,@!aire:str_number;
  @!s:scaled):internal_font_number; {input a \.{TFM} file}
label done,bad_tfm,not_found;
var k:0..font_mem_size; {index into |font_info|}
@!file_opened:boolean; {was |tfm_file| successfully opened?}
@!jfm_flag:boolean;
@!x,jfm_id,nt:halfword;
@!cx:KANJI_code;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.563] opening tfm file: now path searching is done
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Set temp_int to value of first byte
@x
@ @<Open |tfm_file| for input@>=
file_opened:=false;
if aire="" then pack_file_name(nom,TEX_font_area,".tfm")
else pack_file_name(nom,aire,".tfm");
if not b_open_in(tfm_file) then abort;
file_opened:=true
@y
@ @<Open |tfm_file| for input@>=
file_opened:=false;
jfm_flag:=false;
pack_file_name(nom,aire,".tfm");
if not b_open_in(tfm_file) then abort;
file_opened:=true
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.564] reading the tfm file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% As a special case, whenever we open a tfm file, we read its first
% byte into "tfm_temp" right away.  TeX looks at "fbyte" before calling
% "fget", so it ends up seeing every byte.  This is Pascal-like I/O.
@x
@d fget==get(tfm_file)
@d fbyte==tfm_file^
@y
@d fget==tfm_temp:=getc(tfm_file)
@d fbyte==tfm_temp
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.565] read tfm size
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Read the {\.{TFM}} size fields@>=
begin read_sixteen(lf);
fget; read_sixteen(lh);
fget; read_sixteen(bc);
fget; read_sixteen(ec);
if (bc>ec+1)or(ec>255) then abort;
fget; read_sixteen(nw);
fget; read_sixteen(nh);
fget; read_sixteen(nd);
fget; read_sixteen(ni);
fget; read_sixteen(nl);
fget; read_sixteen(nk);
fget; read_sixteen(ne);
fget; read_sixteen(np);
if lf<>6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np then abort;
end
@y
@ @<Read the {\.{TFM}} size fields@>=
begin read_sixteen(jfmid);
fget; read_sixteen(nt);
if jfmid = 11{ JFM_ID=11 } then
  begin jfm_flag:=true; fget; read_sixteen(lf); fget; read_sixteen(lh); end
  else begin jfm_flag:=false; lf:=jfmid; lh:=nt; nt:=0; end;
fget; read_sixteen(bc);
fget; read_sixteen(ec);
if (bc>ec+1)or(ec>255) then abort;
fget; read_sixteen(nw);
fget; read_sixteen(nh);
fget; read_sixteen(nd);
fget; read_sixteen(ni);
fget; read_sixteen(nl);
fget; read_sixteen(nk);
fget; read_sixteen(ne);
fget; read_sixteen(np);
if jfm_flag then begin
  if lf<>7+lh+(ec-bc+1)+nt+nw+nh+nd+ni+nl+nk+ne+np then abort
end else begin
  if lf<>6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np then abort
  end
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.566] set font_jtype & font_num_ext
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Use size fields to allocate font information@>=
lf:=lf-6-lh; {|lf| words should be loaded into |font_info|}
if np<7 then lf:=lf+7-np; {at least seven parameters will appear}
if (font_ptr=font_max)or(fmem_ptr+lf>font_mem_size) then
  @<Apologize for not loading the font, |goto done|@>;
f:=font_ptr+1;
char_base[f]:=fmem_ptr-bc;
width_base[f]:=char_base[f]+ec+1;
height_base[f]:=width_base[f]+nw;
depth_base[f]:=height_base[f]+nh;
italic_base[f]:=depth_base[f]+nd;
lig_kern_base[f]:=italic_base[f]+ni;
kern_base[f]:=lig_kern_base[f]+nl;
exten_base[f]:=kern_base[f]+nk;
param_base[f]:=exten_base[f]+ne
@y
@<Use size fields to allocate font information@>=
lf:=lf-6-lh; {|lf| words should be loaded into |font_info|}
if jfm_flag then decr(lf);
if np<7 then lf:=lf+7-np; {at least seven parameters will appear}
if (font_ptr=font_max)or(fmem_ptr+lf>font_mem_size) then
  @<Apologize for not loading the font, |goto done|@>;
f:=font_ptr+1;
font_jtype[f]:=jfm_flag;
font_num_ext[f]:=nt;
char_base[f]:=fmem_ptr-bc;
width_base[f]:=char_base[f]+ec+1;
height_base[f]:=width_base[f]+nw;
depth_base[f]:=height_base[f]+nh;
italic_base[f]:=depth_base[f]+nd;
lig_kern_base[f]:=italic_base[f]+ni;
kern_base[f]:=lig_kern_base[f]+nl;
exten_base[f]:=kern_base[f]+nk;
ctype_base[f]:=exten_base[f]+ne;
param_base[f]:=ctype_base[f]+nt;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.569] read char_type 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Read character data@>=
for k:=fmem_ptr to width_base[f]-1 do
  begin store_four_quarters(font_info[k].qqqq);
@y
@ @<Read character data@>=
if jfm_flag then
  for k:=ctype_base[f] to param_base[f]-1 do
    begin fget; PutHi(cx,fbyte); fget; PutLo(cx,fbyte);
	font_info[k].hh.rh:=tonum(cx);
	fget; PutHi(cx,fbyte); fget; PutLo(cx,fbyte);
	font_info[k].hh.v.LH:=tonum(cx); end;
for k:=fmem_ptr to width_base[f]-1 do
  begin store_four_quarters(font_info[k].qqqq);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.573] read  glue/kern program
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Read ligature/kern program@>=
begin for k:=lig_kern_base[f] to kern_base[f]-1 do
  begin store_four_quarters(font_info[k].qqqq);
  check_byte_range(b);
  if c<128 then check_byte_range(d) {check ligature}
  else if d>=nk then abort; {check kern}
  end;
@y
@ @<Read ligature/kern program@>=
begin for k:=lig_kern_base[f] to kern_base[f]-1 do
  begin store_four_quarters(font_info[k].qqqq);
  check_byte_range(b);
  if c<128 then
    if jfm_flag then begin if d>=ne then abort; end
    else check_byte_range(d) {check ligature}
  else if d>=nk then abort; {check kern}
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.574] read jfm exten
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Read extensible character recipes@>=
for k:=exten_base[f] to param_base[f]-1 do
  begin store_four_quarters(font_info[k].qqqq);
  if a<>0 then check_byte_range(a);
  if b<>0 then check_byte_range(b);
  if c<>0 then check_byte_range(c);
  check_byte_range(d);
  end
@y
@ @<Read extensible character recipes@>=
if jfm_flag then
for k:=exten_base[f] to ctype_base[f]-1 do
  store_scaled(font_info[k].sc)
else for k:=exten_base[f] to param_base[f]-1 do
  begin store_four_quarters(font_info[k].qqqq);
  if a<>0 then check_byte_range(a);
  if b<>0 then check_byte_range(b);
  if c<>0 then check_byte_range(c);
  check_byte_range(d);
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.576] adjust ctype_base
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
adjust(char_base); adjust(width_base); adjust(lig_kern_base);
adjust(kern_base); adjust(exten_base);
decr(param_base[f]);
@y
adjust(char_base); adjust(width_base); adjust(lig_kern_base);
adjust(kern_base); adjust(exten_base); adjust(ctype_base);
decr(param_base[f]);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.577] jfont
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin @<Get the next non-blank non-call...@>;
if cur_cmd=def_font then f:=cur_font
@y
begin @<Get the next non-blank non-call...@>;
if cur_cmd=def_jfont then f:=cur_jfont
else if cur_cmd=def_font then f:=cur_font
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30.581] jchar_worning
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure char_warning(@!f:internal_font_number;@!c:eight_bits);
begin if tracing_lost_chars>0 then
  begin begin_diagnostic;
  print_nl("Missing character: There is no ");
@.Missing character@>
  print_ASCII(c); print(" in font ");
  print(font_name[f]); print_char("!"); end_diagnostic(false);
  end;
end;
@y
@p procedure char_warning(@!f:internal_font_number;@!c:eight_bits);
begin if tracing_lost_chars>0 then
  begin begin_diagnostic;
  print_nl("Missing character: There is no ");
@.Missing character@>
  print_ASCII(c); print(" in font ");
  print(font_name[f]); print_char("!"); end_diagnostic(false);
  end;
end;

procedure jchar_warning(@!f:internal_font_number;@!jc:KANJI_code);
begin if tracing_lost_chars>0 then
  begin begin_diagnostic;
  print_nl("Missing character: There is no ");
@.Missing character@>
  print_char(Hi(jc)); print_char(Lo(jc));
  print(" in font ");
  print(font_name[f]); print_char("!"); end_diagnostic(false);
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [31.586] define set2 put2
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @d set_char_0=0 {typeset character 0 and move right}
@d set1=128 {typeset a character and move right}
@d set_rule=132 {typeset a rule and move right}
@d put1=133 {typeset a character}
@d put_rule=137 {typeset a rule}
@d nop=138 {no operation}
@d bop=139 {beginning of page}
@d eop=140 {ending of page}
@d push=141 {save the current positions}
@d pop=142 {restore previous positions}
@d right1=143 {move right}
@d w0=147 {move right by |w|}
@d w1=148 {move right and set |w|}
@d x0=152 {move right by |x|}
@d x1=153 {move right and set |x|}
@d down1=157 {move down}
@d y0=161 {move down by |y|}
@d y1=162 {move down and set |y|}
@d z0=166 {move down by |z|}
@d z1=167 {move down and set |z|}
@d fnt_num_0=171 {set current font to 0}
@d fnt1=235 {set current font}
@d xxx1=239 {extension to \.{DVI} primitives}
@d xxx4=242 {potentially long extension to \.{DVI} primitives}
@d fnt_def1=243 {define the meaning of a font number}
@d pre=247 {preamble}
@d post=248 {postamble beginning}
@d post_post=249 {postamble ending}
@y
@ @d set_char_0=0 {typeset character 0 and move right}
@d set1=128 {typeset a character and move right}
@d set2=129 {typeset a character and move right}
@d set_rule=132 {typeset a rule and move right}
@d put1=133 {typeset a character}
@d put2=134 {typeset a character}
@d put_rule=137 {typeset a rule}
@d nop=138 {no operation}
@d bop=139 {beginning of page}
@d eop=140 {ending of page}
@d push=141 {save the current positions}
@d pop=142 {restore previous positions}
@d right1=143 {move right}
@d w0=147 {move right by |w|}
@d w1=148 {move right and set |w|}
@d x0=152 {move right by |x|}
@d x1=153 {move right and set |x|}
@d down1=157 {move down}
@d y0=161 {move down by |y|}
@d y1=162 {move down and set |y|}
@d z0=166 {move down by |z|}
@d z1=167 {move down and set |z|}
@d fnt_num_0=171 {set current font to 0}
@d fnt1=235 {set current font}
@d xxx1=239 {extension to \.{DVI} primitives}
@d xxx4=242 {potentially long extension to \.{DVI} primitives}
@d fnt_def1=243 {define the meaning of a font number}
@d pre=247 {preamble}
@d post=248 {postamble beginning}
@d post_post=249 {postamble ending}

@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [32.597] write_dvi
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The actual output of |dvi_buf[a..b]| to |dvi_file| is performed by calling
|write_dvi(a,b)|. For best results, this procedure should be optimized to
run as fast as possible on each particular system, since it is part of
\TeX's inner loop. It is safe to assume that |a| and |b+1| will both be
multiples of 4 when |write_dvi(a,b)| is called; therefore it is possible on
many machines to use efficient methods to pack four bytes per word and to
output an array of words with one system call.
@^system dependencies@>
@^inner loop@>
@^defecation@>

@p procedure write_dvi(@!a,@!b:dvi_index);
var k:dvi_index;
begin for k:=a to b do write(dvi_file,dvi_buf[k]);
end;
@y
@ In C, we use a macro to call fwrite() or write() directly, writing all
the bytes to be written in one shot.  Much better even than writing four
bytes at a time.
@^system dependencies@>
@^inner loop@>
@^defecation@>
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [32.619] hlist_out
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p @t\4@>@<Declare procedures needed in |hlist_out|, |vlist_out|@>@t@>@/
procedure hlist_out; {output an |hlist_node| box}
label reswitch, move_past, fin_rule, next_p;
var base_line: scaled; {the baseline coordinate for this box}
@!left_edge: scaled; {the left coordinate for this box}
@!save_h,@!save_v: scaled; {what |dvi_h| and |dvi_v| should pop to}
@!this_box: pointer; {pointer to containing box}
@!g_order: glue_ord; {applicable order of infinity for glue}
@!g_sign: normal..shrinking; {selects type of glue}
@!p:pointer; {current position in the hlist}
@!save_loc:integer; {\.{DVI} byte location upon entry}
@!leader_box:pointer; {the leader box being replicated}
@!leader_wd:scaled; {width of leader box being replicated}
@!lx:scaled; {extra space between leader boxes}
@!outer_doing_leaders:boolean; {were we doing leaders?}
@!edge:scaled; {left edge of sub-box, or right edge of leader space}
begin this_box:=temp_ptr; g_order:=glue_order(this_box);
g_sign:=glue_sign(this_box); p:=list_ptr(this_box);
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; base_line:=cur_v; left_edge:=cur_h;
while p<>null do @<Output node |p| for |hlist_out| and move to the next node,
  maintaining the condition |cur_v=base_line|@>;
prune_movements(save_loc);
if cur_s>0 then dvi_pop(save_loc);
decr(cur_s);
end;
@y
@p @t\4@>@<Declare procedures needed in |hlist_out|, |vlist_out|@>@t@>@/
procedure hlist_out; {output an |hlist_node| box}
label reswitch, move_past, fin_rule, next_p;
var base_line: scaled; {the baseline coordinate for this box}
@!left_edge: scaled; {the left coordinate for this box}
@!save_h,@!save_v: scaled; {what |dvi_h| and |dvi_v| should pop to}
@!this_box: pointer; {pointer to containing box}
@!g_order: glue_ord; {applicable order of infinity for glue}
@!g_sign: normal..shrinking; {selects type of glue}
@!p:pointer; {current position in the hlist}
@!k:pointer; {position of |auto_spacing_glue| in the hlist}
@!save_loc:integer; {\.{DVI} byte location upon entry}
@!leader_box:pointer; {the leader box being replicated}
@!leader_wd:scaled; {width of leader box being replicated}
@!lx:scaled; {extra space between leader boxes}
@!outer_doing_leaders:boolean; {were we doing leaders?}
@!edge:scaled; {left edge of sub-box, or right edge of leader space}
@!jc:KANJI_code;
@!t:integer;
begin this_box:=temp_ptr; g_order:=glue_order(this_box);
g_sign:=glue_sign(this_box); p:=list_ptr(this_box);
k:=space_ptr(this_box);
incr(cur_s);
if cur_s>0 then dvi_out(push);
if cur_s>max_push then max_push:=cur_s;
save_loc:=dvi_offset+dvi_ptr; base_line:=cur_v; left_edge:=cur_h;
while p<>null do @<Output node |p| for |hlist_out| and move to the next node,
  maintaining the condition |cur_v=base_line|@>;
prune_movements(save_loc);
if cur_s>0 then dvi_pop(save_loc);
decr(cur_s);
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [32.620,621] dvi output
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Output node |p| for |hlist_out|...@>=
reswitch: if is_char_node(p) then
  begin synch_h; synch_v;
  repeat f:=font(p); c:=character(p);
  if f<>dvi_f then @<Change font |dvi_f| to |f|@>;
  if c>=qi(128) then dvi_out(set1);
  dvi_out(qo(c));@/
  cur_h:=cur_h+char_width(f)(char_info(f)(c));
  p:=link(p);
  until not is_char_node(p);
  dvi_h:=cur_h;
  end
else @<Output the non-|char_node| |p| for |hlist_out|
    and move to the next node@>

@ @<Change font |dvi_f| to |f|@>=
begin if not font_used[f] then
  begin dvi_font_def(f); font_used[f]:=true;
  end;
if f<=64+font_base then dvi_out(f-font_base-1+fnt_num_0)
else  begin dvi_out(fnt1); dvi_out(f-font_base-1);
  end;
dvi_f:=f;
end
@y
@<Output node |p| for |hlist_out|...@>=
reswitch: if is_char_node(p) then
  begin synch_h; synch_v; chain:=false;
  repeat f:=font(p); c:=character(p);
  if f<>dvi_f then @<Change font |dvi_f| to |f|@>;
  if font_jtype[f] then begin
    if chain then begin
      cur_h:=cur_h+width(k);
      if g_sign<>normal then
        begin if g_sign=stretching then
          begin if stretch_order(k)=g_order then
            cur_h:=cur_h+round(float(glue_set(this_box))*stretch(k));
@^real multiplication@>
          end else
          begin if shrink_order(k)=g_order then
            cur_h:=cur_h-round(float(glue_set(this_box))*shrink(k));
          end;
        end;
      synch_h;
    end else chain:=true;
    p:=link(p);
    KANJI(jc):=info(p); t:=tonum(jc);
    dvi_out(set2); KANJI(jc):=SJIStoJIS(t); jc:=tokanji(KANJI(jc));
    dvi_out(Hi(jc)); dvi_out(Lo(jc))
  end else begin
    if c<qi(128) then dvi_out(qo(c))
    else  begin dvi_out(set1); dvi_out(qo(c));
      end;
    chain:=false;
  end;
  cur_h:=cur_h+char_width(f)(char_info(f)(c));
  dvi_h:=cur_h; p:=link(p);
  until not is_char_node(p);
  chain:=false;
  end
else @<Output the non-|char_node| |p| for |hlist_out|
    and move to the next node@>

@ @<Change font |dvi_f| to |f|@>=
begin if not font_used[f] then
  begin dvi_font_def(f); font_used[f]:=true;
  end;
if f<=64+font_base then dvi_out(f-font_base-1+fnt_num_0)
else  begin dvi_out(fnt1); dvi_out(f-font_base-1);
  end;
dvi_f:=f;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [33.649] hpack
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function hpack(@!p:pointer;@!w:scaled;@!m:small_number):pointer;
label reswitch, common_ending, exit;
var r:pointer; {the box node that will be returned}
@!q:pointer; {trails behind |p|}
@!h,@!d,@!x:scaled; {height, depth, and natural width}
@!s:scaled; {shift amount}
@!g:pointer; {points to a glue specification}
@!o:glue_ord; {order of infinity}
@!f:internal_font_number; {the font in a |char_node|}
@!i:four_quarters; {font information about a |char_node|}
@!hd:eight_bits; {height and depth indices for a character}
@!b:integer; {badness of the new box}
begin r:=get_node(box_node_size); type(r):=hlist_node;
subtype(r):=min_quarterword; shift_amount(r):=0;
q:=r+list_offset; link(q):=p;@/
h:=0; @<Clear dimensions to zero@>;
while p<>null do @<Examine node |p| in the hlist, taking account of its effect
  on the dimensions of the new box, or moving it to the adjustment list;
  then advance |p| to the next node@>;
if adjust_tail<>null then link(adjust_tail):=null;
height(r):=h; depth(r):=d;@/
@<Determine the value of |width(r)| and the appropriate glue setting;
  then |return| or |goto common_ending|@>;
common_ending: @<Finish issuing a diagnostic message
      for an overfull or underfull hbox@>;
exit: hpack:=r;
end;
@y
@p function hpack(@!p:pointer;@!w:scaled;@!m:small_number):pointer;
label reswitch, common_ending, exit;
var r:pointer; {the box node that will be returned}
@!q:pointer; {trails behind |p|}
@!h,@!d,@!x:scaled; {height, depth, and natural width}
@!s:scaled; {shift amount}
@!g:pointer; {points to a glue specification}
@!k:pointer; {points to a kanji_space specification}
@!n:halfword; {number of insert kanji_space}
@!o:glue_ord; {order of infinity}
@!f:internal_font_number; {the font in a |char_node|}
@!i:four_quarters; {font information about a |char_node|}
@!hd:eight_bits; {height and depth indices for a character}
@!b:integer; {badness of the new box}
begin n:=0;
r:=get_node(box_node_size); type(r):=hlist_node;
subtype(r):=min_quarterword; shift_amount(r):=0;
space_ptr(r):=cur_kanji_skip; xspace_ptr(r):=cur_xkanji_skip;
incr(glue_ref_count(cur_kanji_skip));
incr(glue_ref_count(cur_xkanji_skip));
k:=cur_kanji_skip;
q:=r+list_offset; link(q):=p;@/
h:=0; @<Clear dimensions to zero@>;
while p<>null do @<Examine node |p| in the hlist, taking account of its effect
  on the dimensions of the new box, or moving it to the adjustment list;
  then advance |p| to the next node@>;
if adjust_tail<>null then link(adjust_tail):=null;
height(r):=h; depth(r):=d;@/
@<Determine the value of |width(r)| and the appropriate glue setting;
  then |return| or |goto common_ending|@>;
common_ending: @<Finish issuing a diagnostic message
      for an overfull or underfull hbox@>;
exit: hpack:=r;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [33.651] reset chain
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin reswitch: while is_char_node(p) do
@y
begin reswitch: chain:=false; while is_char_node(p) do
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [33.654] auto spacing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Incorporate character dimensions into the dimensions of the hbox...@>=
begin f:=font(p); i:=char_info(f)(character(p)); hd:=height_depth(i);
x:=x+char_width(f)(i);@/
s:=char_height(f)(hd);@+if s>h then h:=s;
s:=char_depth(f)(hd);@+if s>d then d:=s;
p:=link(p);
end
@y
@<Incorporate character dimensions into the dimensions of the hbox...@>=
begin f:=font(p); i:=char_info(f)(character(p)); hd:=height_depth(i);
x:=x+char_width(f)(i);@/
s:=char_height(f)(hd);@+if s>h then h:=s;
s:=char_depth(f)(hd);@+if s>d then d:=s;
if font_jtype[f] then begin p:=link(p);
  if chain then begin x:=x+width(k);@/
    o:=stretch_order(k); total_stretch[o]:=total_stretch[o]+stretch(k);
    o:=shrink_order(k); total_shrink[o]:=total_shrink[o]+shrink(k);
    incr(n)
    end else chain:=true;
  end else chain:=false;
p:=link(p);
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [34.668] vpackage
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin r:=get_node(box_node_size); type(r):=vlist_node;
subtype(r):=min_quarterword; shift_amount(r):=0;
list_ptr(r):=p;@/
@y
begin r:=get_node(box_node_size); type(r):=vlist_node;
subtype(r):=min_quarterword; shift_amount(r):=0;
space_ptr(r):=zero_glue; xspace_ptr(r):=zero_glue;
incr(glue_ref_count(zero_glue));
incr(glue_ref_count(zero_glue));
list_ptr(r):=p;@/
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [34.681] math noad
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d noad_size=4 {number of words in a normal noad}
@d nucleus(#)==#+1 {the |nucleus| field of a noad}
@d supscr(#)==#+2 {the |supscr| field of a noad}
@d subscr(#)==#+3 {the |subscr| field of a noad}
@y
@d noad_size=5 {number of words in a normal noad}
@d nucleus(#)==#+1 {the |nucleus| field of a noad}
@d supscr(#)==#+2 {the |supscr| field of a noad}
@d subscr(#)==#+3 {the |subscr| field of a noad}
@d math_kcode(#)==info(#+4) {the |kanji character| field of a noad}
@d math_kcode_nucleus(#)==info(#+3) {the |kanji character| field from nucleus}
@d kcode_noad(#)==#+4
@d kcode_noad_nucleus(#)==#+3
@d math_jchar=5
@d math_text_jchar=6
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [34.686] new_noad
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
mem[supscr(p)].hh:=empty_field;
new_noad:=p;
@y
mem[supscr(p)].hh:=empty_field;
mem[kcode_noad(p)].hh:=empty_field;
new_noad:=p;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [35.691] print_fam_and_char
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
procedure print_fam_and_char(@!p:pointer); {prints family and character}
begin print_esc("fam"); print_int(fam(p)); print_char(" ");
print_ASCII(qo(character(p)));
@y
procedure print_fam_and_char(@!p:pointer;@!t:small_number);
					{prints family and character}
var @!cx:KANJI_code;
begin print_esc("fam"); print_int(fam(p)); print_char(" ");
if t=math_char then print_ASCII(qo(character(p)))
  else begin KANJI(cx):=math_kcode_nucleus(p);
    print_char(Hi(cx)); print_char(Lo(cx));
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [35.692] print_subsidiary_data
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  math_char: begin print_ln; print_current_string; print_fam_and_char(p);
@y
  math_char, math_jchar: begin print_ln; print_current_string;
    print_fam_and_char(p, math_type(p));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [35.696] print_fam_and_char
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
accent_noad: begin print_esc("accent"); print_fam_and_char(accent_chr(p));
@y
accent_noad: begin print_esc("accent");
  print_fam_and_char(accent_chr(p), math_char);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [35.715] rebox
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function rebox(@!b:pointer;@!w:scaled):pointer;
var p:pointer; {temporary register for list manipulation}
@!f:internal_font_number; {font in a one-character box}
@!v:scaled; {width of a character without italic correction}
begin if (width(b)<>w)and(list_ptr(b)<>null) then
  begin if type(b)=vlist_node then b:=hpack(b,natural);
  p:=list_ptr(b);
  if (is_char_node(p))and(link(p)=null) then
    begin f:=font(p); v:=char_width(f)(char_info(f)(character(p)));
    if v<>width(b) then link(p):=new_kern(width(b)-v);
    end;
  free_node(b,box_node_size);
  b:=new_glue(ss_glue); link(b):=p;
  while link(p)<>null do p:=link(p);
  link(p):=new_glue(ss_glue);
  rebox:=hpack(b,w,exactly);
  end
else  begin width(b):=w; rebox:=b;
  end;
end;
@y
@p function rebox(@!b:pointer;@!w:scaled):pointer;
var p:pointer; {temporary register for list manipulation}
@!f:internal_font_number; {font in a one-character box}
@!v:scaled; {width of a character without italic correction}
begin if (width(b)<>w)and(list_ptr(b)<>null) then
  begin if type(b)=vlist_node then b:=hpack(b,natural);
  p:=list_ptr(b);
  if is_char_node(p) then
    if font_jtype[font(p)] then begin
      if link(link(p))=null then
      begin f:=font(p); v:=char_width(f)(char_info(f)(character(p)));
      if v<>width(b) then link(link(p)):=new_kern(width(b)-v);
      end
    end else if link(p)=null then
        begin f:=font(p); v:=char_width(f)(char_info(f)(character(p)));
        if v<>width(b) then link(p):=new_kern(width(b)-v);
        end;
  delete_glue_ref(space_ptr(b)); delete_glue_ref(xspace_ptr(b));
  free_node(b,box_node_size);
  b:=new_glue(ss_glue); link(b):=p;
  while link(p)<>null do p:=link(p);
  link(p):=new_glue(ss_glue);
  rebox:=hpack(b,w,exactly);
  end
else  begin width(b):=w; rebox:=b;
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.720] clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
function clean_box(@!p:pointer;@!s:small_number):pointer;
@y
function clean_box(@!p:pointer;@!s:small_number;@!jc:halfword):pointer;
@z

@x
math_char: begin cur_mlist:=new_noad; mem[nucleus(cur_mlist)]:=mem[p];
@y
math_char,math_jchar: begin cur_mlist:=new_noad;
  mem[nucleus(cur_mlist)]:=mem[p];
  if math_type(p)=math_jchar then math_kcode(cur_mlist):=jc;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.721] skip 2nd kanji node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Simplify a trivial box@>=
q:=list_ptr(x);
if is_char_node(q) then
  begin r:=link(q);
  if r<>null then if link(r)=null then
   if not is_char_node(r) then if type(r)=kern_node then
@y
@<Simplify a trivial box@>=
q:=list_ptr(x);
if is_char_node(q) then
  begin if font_jtype[font(q)] then q:=link(q);
  r:=link(q);
  if r<>null then if link(r)=null then
   if not is_char_node(r) then if type(r)=kern_node then
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.722] fetch
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  @<Complain about an undefined family and set |cur_i| null@>
else  begin if (qo(cur_c)>=font_bc[cur_f])and(qo(cur_c)<=font_ec[cur_f]) then
    cur_i:=char_info(cur_f)(cur_c)
@y
  @<Complain about an undefined family and set |cur_i| null@>
else
  begin if font_jtype[cur_f] then
    cur_c:=qi(get_jfm_pos(math_kcode_nucleus(a), cur_f));
  if (qo(cur_c)>=font_bc[cur_f])and(qo(cur_c)<=font_ec[cur_f]) then
    cur_i:=char_info(cur_f)(cur_c)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.726] mlist_to_hlist
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
var mlist:pointer; {beginning of the given list}
@!penalties:boolean; {should penalty nodes be inserted?}
@!style:small_number; {the given style}
@y
var mlist:pointer; {beginning of the given list}
@!penalties:boolean; {should penalty nodes be inserted?}
@!style:small_number; {the given style}
@!u:pointer; {temporary register}
@z

@x
@<Make a second pass over the mlist, removing all noads and inserting the
  proper spacing and penalties@>;
end;
@y
@<Make a second pass over the mlist, removing all noads and inserting the
  proper spacing and penalties@>;
  p:=new_null_box; link(p):=link(temp_head);
  adjust_hlist(p, false); link(temp_head):=link(p);
  delete_glue_ref(space_ptr(p)); delete_glue_ref(xspace_ptr(p));
  free_node(p,box_node_size);
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.727] free box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
free_node(z,box_node_size);
@y
delete_glue_ref(space_ptr(z)); delete_glue_ref(xspace_ptr(z));
free_node(z,box_node_size);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.734] make_over:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  overbar(clean_box(nucleus(q),cramped_style(cur_style)),@|
@y
  overbar(clean_box(nucleus(q),cramped_style(cur_style),math_kcode(q)),@|
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.735] make_under:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin x:=clean_box(nucleus(q),cur_style);
@y
begin x:=clean_box(nucleus(q),cur_style,math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.737] make_radical:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin x:=clean_box(nucleus(q),cramped_style(cur_style));
@y
begin x:=clean_box(nucleus(q),cramped_style(cur_style),math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.738] make_math_accent:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  x:=clean_box(nucleus(q),cramped_style(cur_style)); w:=width(x); h:=height(x);
@y
  x:=clean_box(nucleus(q),cramped_style(cur_style),math_kcode(q));
  w:=width(x); h:=height(x);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.742] make_math_accent:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
x:=clean_box(nucleus(q),cur_style); delta:=delta+height(x)-h; h:=height(x);
@y
x:=clean_box(nucleus(q),cur_style,math_kcode(q));
delta:=delta+height(x)-h; h:=height(x);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.744] make_fraction:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
x:=clean_box(numerator(q),num_style(cur_style));
z:=clean_box(denominator(q),denom_style(cur_style));
@y
x:=clean_box(numerator(q),num_style(cur_style),math_kcode(q));
z:=clean_box(denominator(q),denom_style(cur_style),math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.749] make_op:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  delta:=char_italic(cur_f)(cur_i); x:=clean_box(nucleus(q),cur_style);
@y
  delta:=char_italic(cur_f)(cur_i);
  x:=clean_box(nucleus(q),cur_style,math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.750] make_op:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin x:=clean_box(supscr(q),sup_style(cur_style));
y:=clean_box(nucleus(q),cur_style);
z:=clean_box(subscr(q),sub_style(cur_style));
@y
begin x:=clean_box(supscr(q),sup_style(cur_style),math_kcode(q));
y:=clean_box(nucleus(q),cur_style,math_kcode(q));
z:=clean_box(subscr(q),sub_style(cur_style),math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.751] free box node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  begin free_node(x,box_node_size); list_ptr(v):=y;
  end
@y
  begin delete_glue_ref(space_ptr(x)); delete_glue_ref(xspace_ptr(x));
  free_node(x,box_node_size); list_ptr(v):=y;
  end
@z

@x
if math_type(subscr(q))=empty then free_node(z,box_node_size)
@y
if math_type(subscr(q))=empty then
  begin delete_glue_ref(space_ptr(z)); delete_glue_ref(xspace_ptr(z));
    free_node(z,box_node_size)
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.752] make_ord
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
var a:integer; {address of lig/kern instruction}
@!p:pointer; {temporary register for list manipulation}
@y
var a:integer; {address of lig/kern instruction}
@!gp,@!gq,@!p:pointer; {temporary register for list manipulation}
@!r:halfword;
@z

@x
if (math_type(subscr(q))=empty)and(math_type(supscr(q))=empty)and@|
  (math_type(nucleus(q))=math_char) then
  begin p:=link(q);
  if p<>null then if (type(p)>=ord_noad)and(type(p)<=punct_noad) then
    if math_type(nucleus(p))=math_char then
    if fam(nucleus(p))=fam(nucleus(q)) then
      begin math_type(nucleus(q)):=math_text_char;
@y
if (math_type(subscr(q))=empty)and(math_type(supscr(q))=empty)and@|
  ((math_type(nucleus(q))=math_char)or(math_type(nucleus(q))=math_jchar)) then
  begin p:=link(q);
  if p<>null then if (type(p)>=ord_noad)and(type(p)<=punct_noad) then
    if (math_type(nucleus(p))=math_char)or
      (math_type(nucleus(p))=math_jchar) then
      if fam(nucleus(p))=fam(nucleus(q)) then
        if math_type(nucleus(q))=math_char then
        begin math_type(nucleus(q)):=math_text_char;
@z

@x
        until stop_bit(cur_i)>=stop_flag;
        end;
      end;
  end;
@y
        until stop_bit(cur_i)>=stop_flag;
        end;
      end else
        begin math_type(nucleus(q)):=math_text_jchar;
        fetch(nucleus(p)); a:=cur_c;
        fetch(nucleus(q));
	if char_tag(cur_i)=gk_tag then
	begin cur_c:=a; a:=glue_kern_start(cur_f)(cur_i);
        {cur_c:=qi(get_jfm_pos(math_kcode(p),fam_fnt(fam(nucleus(p))+cur_size)));}
	repeat cur_i:=font_info[a].qqqq;
	  if next_char(cur_i)=cur_c then
	    if op_bit(cur_i)<kern_flag then
	      begin
		gp:=font_glue[cur_f];
		r:=rem_byte(cur_i);
		if gp<>null then begin
		  while((type(gp)<>r)and(link(gp)<>null)) do
		    begin gp:=link(gp); end;
 		  gq:=glue_ptr(gp);
		end else begin gp:=get_node(small_node_size);
		  font_glue[cur_f]:=gp; gq:=null;
		end;
		if gq=null then begin
		  type(gp):=r; gq:=new_spec(zero_glue);
		  glue_ptr(gp):=gq;
		  a:=exten_base[cur_f]+qi((qo(r))*3);
		  width(gq):=font_info[a].sc;
		  stretch(gq):=font_info[a+1].sc; shrink(gq):=font_info[a+2].sc;
		  incr(glue_ref_count(gq)); link(gp):=get_node(small_node_size);
		  gp:=link(gp); glue_ptr(gp):=null; link(gp):=null;
		end;
		p:=new_glue(gq);
		link(p):=link(q); link(q):=p; return;
	      end else begin
		p:=new_kern(char_kern(cur_f)(cur_i));
		link(p):=link(q); link(q):=p; return;
	      end;
	    incr(a);
	  until stop_bit(cur_i)>=stop_flag;
	end;
      end;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.754] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
math_char, math_text_char:
@y
math_char, math_text_char, math_jchar, math_text_jchar:
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.755] convert math text to KANJI char_node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  begin delta:=char_italic(cur_f)(cur_i); p:=new_character(cur_f,qo(cur_c));
  if (math_type(nucleus(q))=math_text_char)and(space(cur_f)<>0) then
    delta:=0; {no italic correction in mid-word of text font}
  if (math_type(subscr(q))=empty)and(delta<>0) then
    begin link(p):=new_kern(delta); delta:=0;
@y
  begin delta:=char_italic(cur_f)(cur_i); p:=new_character(cur_f,qo(cur_c));
  u:=p;
  if font_jtype[cur_f] then
    begin link(u):=get_avail; u:=link(u); info(u):=math_kcode(q); end;
  if ((math_type(nucleus(q))=math_text_char)or
     (math_type(nucleus(q))=math_text_jchar))and(space(cur_f)<>0) then
    delta:=0; {no italic correction in mid-word of text font}
  if (math_type(subscr(q))=empty)and(delta<>0) then
    begin link(u):=new_kern(delta); delta:=0;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.756] free box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  shift_down:=depth(z)+sub_drop(t);
  free_node(z,box_node_size);
  end;
@y
  shift_down:=depth(z)+sub_drop(t);
  delete_glue_ref(space_ptr(z));
  delete_glue_ref(xspace_ptr(z));
  free_node(z,box_node_size);
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.757] make_scripts:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin x:=clean_box(subscr(q),sub_style(cur_style));
@y
begin x:=clean_box(subscr(q),sub_style(cur_style),math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.758] make_scripts:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin x:=clean_box(supscr(q),sup_style(cur_style));
@y
begin x:=clean_box(supscr(q),sup_style(cur_style),math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36.758] make_scripts:clean_box
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin y:=clean_box(subscr(q),sub_style(cur_style));
@y
begin y:=clean_box(subscr(q),sub_style(cur_style),math_kcode(q));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [37.796][37.799] call adjust_hlist
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  begin adjust_tail:=cur_tail; u:=hpack(link(head),natural); w:=width(u);
@y
  begin adjust_tail:=cur_tail; adjust_hlist(head, false);
  delete_glue_ref(cur_kanji_skip); delete_glue_ref(cur_xkanji_skip);
  cur_kanji_skip:=space_ptr(head); cur_xkanji_skip:=xspace_ptr(head);
  add_glue_ref(cur_kanji_skip);	add_glue_ref(cur_xkanji_skip);
  u:=hpack(link(head),natural); w:=width(u);
@z

@x
  begin p:=hpack(link(head),natural);
@y
  begin adjust_hlist(head, false);
  delete_glue_ref(cur_kanji_skip); delete_glue_ref(cur_xkanji_skip);
  cur_kanji_skip:=space_ptr(head); cur_xkanji_skip:=xspace_ptr(head);
  add_glue_ref(cur_kanji_skip);	add_glue_ref(cur_xkanji_skip);
  p:=hpack(link(head),natural);
@z

@x
var @!p,@!q,@!r,@!s,@!u,@!v: pointer; {registers for the list operations}
@y
var @!p,@!q,@!r,@!s,@!u,@!v,@!z: pointer; {registers for the list operations}
@z

@x
  p:=hpack(preamble,saved(1),saved(0)); overfull_rule:=rule_save;
@y
  z:=new_null_box; link(z):=preamble;
  adjust_hlist(z,false);
  delete_glue_ref(cur_kanji_skip); delete_glue_ref(cur_xkanji_skip);
  cur_kanji_skip:=space_ptr(z); cur_xkanji_skip:=xspace_ptr(z);
  add_glue_ref(cur_kanji_skip);	add_glue_ref(cur_xkanji_skip);
  p:=hpack(preamble,saved(1),saved(0)); overfull_rule:=rule_save;
  delete_glue_ref(space_ptr(z)); delete_glue_ref(xspace_ptr(z));
  free_node(z,box_node_size);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [38.816] init chain
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
This code assumes that a |glue_node| and a |penalty_node| occupy the
same number of words in |mem|.
@^data structure assumptions@>

@<Get ready...@>=
link(temp_head):=link(head);
if is_char_node(tail) then tail_append(new_penalty(inf_penalty))
else if type(tail)<>glue_node then tail_append(new_penalty(inf_penalty))
@y
This code assumes that a |glue_node| and a |penalty_node| occupy the
same number of words in |mem|.
@^data structure assumptions@>

@<Get ready...@>=
first_use:=true; chain:=false;
delete_glue_ref(cur_kanji_skip); delete_glue_ref(cur_xkanji_skip);
cur_kanji_skip:=space_ptr(head); cur_xkanji_skip:=xspace_ptr(head);
add_glue_ref(cur_kanji_skip); add_glue_ref(cur_xkanji_skip);
link(temp_head):=link(head);
if is_char_node(tail) then tail_append(new_penalty(inf_penalty))
else if type(tail)<>glue_node then tail_append(new_penalty(inf_penalty))
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [38.828] chain
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
feasible solution, |threshold| is set to |tolerance|, |second_pass| is set
|true|, and an attempt is made to hyphenate as many words as possible.

@<Glob...@>=
@!cur_p:pointer; {the current breakpoint under consideration}
@!second_pass:boolean; {is this our second attempt to break this paragraph?}
@!threshold:integer; {maximum badness on feasible lines}
@y
feasible solution, |threshold| is set to |tolerance|, |second_pass| is set
|true|, and an attempt is made to hyphenate as many words as possible.

@<Glob...@>=
@!cur_p:pointer; {the current breakpoint under consideration}
@!second_pass:boolean; {is this our second attempt to break this paragraph?}
@!threshold:integer; {maximum badness on feasible lines}
@!chain:boolean;
@!cur_kanji_skip:pointer;
@!cur_xkanji_skip:pointer;

@ @<Set init...@>=
cur_kanji_skip:=zero_glue; cur_xkanji_skip:=zero_glue;
incr(glue_ref_count(cur_kanji_skip));
incr(glue_ref_count(cur_xkanji_skip));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [38.830] local vavariable in tray_break
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!save_link:pointer; {temporarily holds value of |link(cur_p)|}
@y
@!save_link:pointer; {temporarily holds value of |link(cur_p)|}
@!tp:pointer;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [38.837] add kanji_skip width
% chain が真の場合、後ろに cur_kanji_skip が挿入されるので、ここで
% break_width から引いておかなければならない。
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Compute the values of |break...@>=
begin no_break_yet:=false; do_all_six(set_break_width_to_background);
s:=cur_p;
if break_type>unhyphenated then if cur_p<>null then
  @<Compute the discretionary |break_width| values@>;
while s<>null do
  begin if is_char_node(s) then goto done;
  case type(s) of
  glue_node:@<Subtract glue from |break_width|@>;
@y
@<Compute the values of |break...@>=
begin no_break_yet:=false; do_all_six(set_break_width_to_background);
s:=cur_p;
if break_type>unhyphenated then if cur_p<>null then
  @<Compute the discretionary |break_width| values@>;
while s<>null do
  begin if is_char_node(s) then
    begin if chain then
      begin break_width[1]:=break_width[1]-width(cur_kanji_skip);
      break_width[2+stretch_order(cur_kanji_skip)]:=
	break_width[2+stretch_order(cur_kanji_skip)]-
	stretch(cur_kanji_skip);
      break_width[6]:=break_width[6]-shrink(cur_kanji_skip);
    end;
    goto done end;
  case type(s) of
  glue_node:@<Subtract glue from |break_width|@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [38.841] [38.842] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Subtract the width of node |v|...@>=
if is_char_node(v) then
  begin f:=font(v);
  break_width[1]:=break_width[1]-char_width(f)(char_info(f)(character(v)));
  end
@y
@<Subtract the width of node |v|...@>=
if is_char_node(v) then
  begin f:=font(v);
  break_width[1]:=break_width[1]-char_width(f)(char_info(f)(character(v)));
  if font_jtype[f] then v:=link(v);
  end
@z

@x
@ @<Add the width of node |s| to |b...@>=
if is_char_node(s) then
  begin f:=font(s);
  break_width[1]:=@|break_width[1]+char_width(f)(char_info(f)(character(s)));
  end
@y
@ @<Add the width of node |s| to |b...@>=
if is_char_node(s) then
  begin f:=font(s);
  break_width[1]:=@|break_width[1]+char_width(f)(char_info(f)(character(s)));
  if font_jtype[f] then s:=link(s);
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [39.856] print symbolic feasibe node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if cur_p=null then print_esc("par")
else if type(cur_p)<>glue_node then
  begin if type(cur_p)=penalty_node then print_esc("penalty")
  else if type(cur_p)=disc_node then print_esc("discretionary")
@y
if cur_p=null then print_esc("par")
else if (type(cur_p)<>glue_node) and not is_char_node(cur_p) then
  begin if type(cur_p)=penalty_node then print_esc("penalty")
  else if type(cur_p)=disc_node then print_esc("discretionary")
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [38.859] Fix a C casting/expression evaluation problem
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if abs(fit_class-fitness(r))>1 then d:=d+adj_demerits
@y
if abs(toint(fit_class)-toint(fitness(r)))>1 then d:=d+adj_demerits
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [39.862] local k
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!auto_breaking:boolean; {is node |cur_p| outside a formula?}
@!prev_p:pointer; {helps to determine when glue nodes are breakpoints}
@!q,@!r,@!s:pointer; {miscellaneous nodes of temporary interest}
@!f:internal_font_number; {used when calculating character widths}
@y
@!auto_breaking:boolean; {is node |cur_p| outside a formula?}
@!prev_p:pointer; {helps to determine when glue nodes are breakpoints}
@!q,@!r,@!s:pointer; {miscellaneous nodes of temporary interest}
@!f,@!post_f:internal_font_number; {used when calculating character widths}
@!cc:ASCII_code;
@!k,@!post_p:pointer;
@!first_use:boolean;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [39.867] add kanji char width
% ここで、連続する漢字コード間に cur_kanji_skip を挿入する。
% penalty_node や box_node には、adjust_hlist ルーチンによって既に実際の
% glue_node が挿入されているものと仮定している。
% chain は、try_break で cur_kanji_skip のアジャストのために使われる。
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Advance \(c)|cur_p| to the node following the present string...@>=
begin prev_p:=cur_p;
repeat f:=font(cur_p);
act_width:=act_width+char_width(f)(char_info(f)(character(cur_p)));
cur_p:=link(cur_p);
until not is_char_node(cur_p);
end
@y
@<Advance \(c)|cur_p| to the node following the present string...@>=
begin chain:=false;
if is_char_node(cur_p) then
  if font_jtype[font(cur_p)] then begin
    case type(prev_p) of
    hlist_node,vlist_node,rule_node,ligature_node,disc_node,math_node:
      begin cur_p:=prev_p; try_break(0, unhyphenated); cur_p:=link(cur_p); end;
    othercases do_nothing;
    endcases;
    end;
prev_p:=cur_p; post_p:=cur_p; post_f:=font(post_p);
repeat f:=post_f; cc:=character(cur_p);
act_width:=act_width+char_width(f)(char_info(f)(cc));
post_p:=link(cur_p);
if font_jtype[f] then
  begin prev_p:=cur_p; cur_p:=post_p; post_p:=link(post_p);
  if is_char_node(post_p) then begin
    post_f:=font(post_p);
    if font_jtype[post_f] then chain:=true else chain:=false;
    try_break(0, unhyphenated);
    end else begin chain:=false;
    case type(post_p) of
      hlist_node, vlist_node, rule_node, ligature_node,
        disc_node, math_node: try_break(0, unhyphenated);
      othercases do_nothing;
      endcases;
    end;
  if chain then begin
    if first_use then begin
      check_shrinkage(cur_kanji_skip);
      first_use:=false;
      end;
    act_width:=act_width+width(cur_kanji_skip);@|
    active_width[2+stretch_order(cur_kanji_skip)]:=@|
    active_width[2+stretch_order(cur_kanji_skip)]+stretch(cur_kanji_skip);@/
    active_width[6]:=active_width[6]+shrink(cur_kanji_skip);
    end;
  prev_p:=cur_p;
  end else if is_char_node(post_p) then begin
    post_f:=font(post_p); chain:=false;
    if font_jtype[post_f] then try_break(0, unhyphenated);
    end;
cur_p:=post_p;
until not is_char_node(cur_p);
chain:=false;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [39.871] add kanji char width
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Add the width of node |s| to |disc_width|@>=
if is_char_node(s) then
  begin f:=font(s);
  disc_width:=disc_width+char_width(f)(char_info(f)(character(s)));
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
    disc_width:=disc_width+
      char_width(f)(char_info(f)(character(lig_char(s))));
    end;
  hlist_node,vlist_node,rule_node,kern_node:
    disc_width:=disc_width+width(s);
  othercases confusion("disc3")
@:this can't happen disc3}{\quad disc3@>
  endcases
@y
@ @<Add the width of node |s| to |disc_width|@>=
if is_char_node(s) then
  begin f:=font(s);
  disc_width:=disc_width+char_width(f)(char_info(f)(character(s)));
  if font_jtype[f] then s:=link(s)
  end
else  case type(s) of
  ligature_node: begin f:=font(lig_char(s));
    disc_width:=disc_width+
      char_width(f)(char_info(f)(character(lig_char(s))));
    end;
  hlist_node,vlist_node,rule_node,kern_node:
    disc_width:=disc_width+width(s);
  othercases confusion("disc3")
@:this can't happen disc3}{\quad disc3@>
  endcases
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%j[39.881] |q| may be a |char_node|
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Modify the end of the line...@>=
q:=cur_break(cur_p); disc_break:=false;
if q<>null then {|q| cannot be a |char_node|}
  if type(q)=glue_node then
    begin delete_glue_ref(glue_ptr(q));
    glue_ptr(q):=right_skip;
    subtype(q):=right_skip_code+1; add_glue_ref(right_skip);
    goto done;
    end
  else  begin if type(q)=disc_node then
      @<Change discretionary to compulsory and set
        |disc_break:=true|@>
    else if (type(q)=math_node)or(type(q)=kern_node) then width(q):=0;
    end
else  begin q:=temp_head;
  while link(q)<>null do q:=link(q);
  end;
@<Put the \(r)\.{\\rightskip} glue after node |q|@>;
done:
@y
@<Modify the end of the line...@>=
  q:=cur_break(cur_p); disc_break:=false;
  if q<>null then begin  {|q| may be a |char_node|}
    if not is_char_node(q) then 
      if type(q)=glue_node then begin
        delete_glue_ref(glue_ptr(q));
        glue_ptr(q):=right_skip;
        subtype(q):=right_skip_code+1; add_glue_ref(right_skip);
        goto done;
      end else begin
        if type(q)=disc_node then
          @<Change discretionary to compulsory and set
            |disc_break:=true|@>
        else if (type(q)=math_node)or(type(q)=kern_node) then
          width(q):=0;
      end
  end else begin
    q:=temp_head;
    while link(q)<>null do q:=link(q);
  end;
  @<Put the \(r)\.{\\rightskip} glue after node |q|@>;
done:
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [40.896] hyphenation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Skip to node |ha|, or |goto done1|...@>=
loop@+  begin if is_char_node(s) then
    begin c:=qo(character(s)); hf:=font(s);
    end
  else if type(s)=ligature_node then
    begin q:=lig_ptr(s); c:=qo(character(q)); hf:=font(q);
    end
  else if (type(s)=kern_node)and(subtype(s)=normal) then c:=128
  else if type(s)=whatsit_node then c:=128
  else goto done1;
  if c<128 then if lc_code(c)<>0 then
    if (lc_code(c)=c)or(uc_hyph>0) then goto done2
    else goto done1;
  s:=link(s);
  end;
done2: hyf_char:=hyphen_char[hf];
if hyf_char<0 then goto done1;
if hyf_char>255 then goto done1;
ha:=s
@y
@<Skip to node |ha|, or |goto done1|...@>=
loop@+  begin if is_char_node(s) then
    begin hf:=font(s);
    if font_jtype[hf] then goto done1
    else c:=qo(character(s))
    end
  else if type(s)=ligature_node then
    begin q:=lig_ptr(s); c:=qo(character(q)); hf:=font(q);
    end
  else if (type(s)=kern_node)and(subtype(s)=normal) then c:=128
  else if type(s)=whatsit_node then c:=128
  else goto done1;
  if c<128 then if lc_code(c)<>0 then
    if (lc_code(c)=c)or(uc_hyph>0) then goto done2
    else goto done1;
  s:=link(s);
  end;
done2: hyf_char:=hyphen_char[hf];
if hyf_char<0 then goto done1;
if hyf_char>255 then goto done1;
ha:=s
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [43.944][43.947] Fix a C casting/expression evaluation problem
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin h:=abs(n+313*d+361*v) mod trie_op_hash_size;
@y
begin h:=abs(toint(n)+313*toint(d)+361*toint(v)) mod trie_op_hash_size;
@z

@x
begin h:=abs(trie_c[p]+1009*trie_o[p]+@|
    2718*trie_l[p]+3142*trie_r[p]) mod trie_size;
@y
begin h:=abs(toint(trie_c[p])+1009*toint(trie_o[p])+@|
    2718*toint(trie_l[p])+3142*toint(trie_r[p])) mod trie_size;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [44.977] free box node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  link to |null| at the break@>;
q:=prune_page_top(q); p:=list_ptr(v); free_node(v,box_node_size);
@y
  link to |null| at the break@>;
q:=prune_page_top(q); p:=list_ptr(v);
delete_glue_ref(space_ptr(v));
delete_glue_ref(xspace_ptr(v));
free_node(v,box_node_size);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [45.1021] free box node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
      height(p):=height(temp_ptr)+depth(temp_ptr);
      free_node(temp_ptr,box_node_size); wait:=true;
@y
      height(p):=height(temp_ptr)+depth(temp_ptr);
      delete_glue_ref(space_ptr(temp_ptr));
      delete_glue_ref(xspace_ptr(temp_ptr));
      free_node(temp_ptr,box_node_size); wait:=true;
@z

@x
temp_ptr:=list_ptr(box(n));
free_node(box(n),box_node_size);
@y
temp_ptr:=list_ptr(box(n));
delete_glue_ref(space_ptr(box(n)));
delete_glue_ref(xspace_ptr(box(n)));
free_node(box(n),box_node_size);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [46.1030] main_control
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ We shall concentrate first on the inner loop of |main_control|, deferring
consideration of the other cases until later.

@d big_switch=60 {go here to branch on the next token of input}
@d main_loop=70 {go here to typeset |cur_chr| in the current font}
@d main_loop_1=71 {like |main_loop|, but |(f,c)| = current font and char}
@d main_loop_2=72 {like |main_loop_1|, but |c| is known to be in range}
@d main_loop_3=73 {like |main_loop_2|, but several variables are set up}
@d append_normal_space=74 {go here to append a normal space between words}

@p @t\4@>@<Declare action procedures for use by |main_control|@>@;
@t\4@>@<Declare the procedure called |handle_right_brace|@>@;
procedure main_control; {governs \TeX's activities}
label big_switch,reswitch,main_loop,main_loop_1,main_loop_2,main_loop_3,
  append_normal_space,exit;
var t:integer; {general-purpose temporary variable}
@<Local variables for the inner loop of |main_control|@>@;
begin if every_job<>null then begin_token_list(every_job,every_job_text);
big_switch: get_x_token;@/
reswitch: @<Give diagnostic information, if requested@>;
case abs(mode)+cur_cmd of
hmode+letter,hmode+other_char,hmode+char_given: goto main_loop;
hmode+char_num: begin scan_char_num; cur_chr:=cur_val; goto main_loop;
  end;
hmode+spacer: if space_factor=1000 then goto append_normal_space
  else app_space;
hmode+ex_space,mmode+ex_space: goto append_normal_space;
@t\4@>@<Cases of |main_control| that are not part of the inner loop@>@;
end; {of the big |case| statement}
goto big_switch;
main_loop:@<Append character |cur_chr| and the following characters (if~any)
  to the current hlist in the current font; |goto reswitch| when
  a non-character has been fetched@>;
append_normal_space:@<Append a normal inter-word space to the current list,
  then |goto big_switch|@>;
exit:end;
@y
@ We shall concentrate first on the inner loop of |main_control|, deferring
consideration of the other cases until later.

@d big_switch=60 {go here to branch on the next token of input}
@d main_loop=70 {go here to typeset |cur_chr| in the current font}
@d main_loop_1=71 {like |main_loop|, but |(f,c)| = current font and char}
@d main_loop_2=72 {like |main_loop_1|, but |c| is known to be in range}
@d main_loop_3=73 {like |main_loop_2|, but several variables are set up}
@d append_normal_space=74 {go here to append a normal space between words}
@d main_loop_j=75 {like |main_loop|, but |cur_chr| in the KANJI code}
@d main_loop_j1=76 {like |main_loop_j|}
@d main_loop_j2=77 {like |main_loop_j|}
@d main_loop_j3=78
@d incert_glue=79
@d skip_loop=80
@d skip_loop_1=81
@d again_1=82
@d again_2=83

@p @t\4@>@<Declare action procedures for use by |main_control|@>@;
@t\4@>@<Declare the procedure called |handle_right_brace|@>@;
procedure main_control; {governs \TeX's activities}
label big_switch,reswitch,main_loop,main_loop_1,main_loop_2,main_loop_3,
  append_normal_space,main_loop_j,main_loop_j1,main_loop_j2,main_loop_j3,
  incert_glue,skip_loop,skip_loop_1,again_1,again_2,exit;
var t:integer; {general-purpose temporary variable}
@<Local variables for the inner loop of |main_control|@>@;
begin if every_job<>null then begin_token_list(every_job,every_job_text);
big_switch: get_x_token; lp:=null;@/
reswitch: @<Give diagnostic information, if requested@>;
case abs(mode)+cur_cmd of
hmode+letter,hmode+other_char: goto main_loop;
hmode+char_given: begin if (cur_chr>=0)and(cur_chr<256) then goto main_loop
  else @<Insert post_break_penalty, check char_node@>; KANJI(cx):=cur_chr;
     f:=cur_jfont; goto main_loop_j2;
goto main_loop_j2; end;
hmode+kana,hmode+other_kchar,hmode+kanji: goto main_loop_j;
hmode+char_num: begin scan_char_num;
  if (cur_val>=0)and(cur_val<256) then
    begin cur_chr:=cur_val; goto main_loop; end 
    else begin @<Insert post_break_penalty, check char_node@>;
     KANJI(cx):=cur_val; f:=cur_jfont; goto main_loop_j2;
    end;
  end;
hmode+spacer: if space_factor=1000 then goto append_normal_space
  else app_space;
hmode+ex_space,mmode+ex_space: goto append_normal_space;
@t\4@>@<Cases of |main_control| that are not part of the inner loop@>@;
end; {of the big |case| statement}
goto big_switch;
main_loop_j:@<Append KANJI-character |cur_chr| 
  to the current hlist in the current font; |goto reswitch| when
  a non-character has been fetched@>;
main_loop:@<Append character |cur_chr| and the following characters (if~any)
  to the current hlist in the current font; |goto reswitch| when
  a non-character has been fetched@>;
append_normal_space:@<Append a normal inter-word space to the current list,
  then |goto big_switch|@>;
exit:end;

@ @<Append KANJI-character |cur_chr| ...@>=
@<Insert post_break_penalty, check char_node@>;
f:=cur_jfont; l:=qi(0);
goto skip_loop_1;
main_loop_j1: PutHi(cx, cur_chr);
get_next; PutLo(cx,cur_chr);{get 2nd byte}
main_loop_j2:
if (not font_jtype[f] or (hbyte = 255)) then
  begin jchar_warning(f, cx); goto big_switch;
  end;
ligature_present:=false; 
l:=qi(get_jfm_pos(KANJI(cx), f));
q:=tail;
main_loop_j3:
lp:=null; space_factor:=1000; cur_jchr:=cx;
fast_get_avail(p); font(p):=f;
character(p):=l; link(tail):=p; tail:=p; last_jchr:=tail;
fast_get_avail(p); info(p):=KANJI(cur_jchr); link(tail):=p; tail:=p;
@<Insert kinsoku penalty@>;
again_2:
get_next;
skip_loop_1:
i:=char_info(f)(l); KANJI(cx):=min_halfword;
if(cur_cmd >= kanji)and(cur_cmd <= other_kchar) then
  begin PutHi(cx,cur_chr); get_next; PutLo(cx,cur_chr); 
    l:=qi(get_jfm_pos(KANJI(cx), f)); end
  else if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
    l:=qi(0)
    else begin x_token;
      if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
	l:=qi(0)
        else if(cur_cmd >= kanji)and(cur_cmd <= other_kchar) then
          begin PutHi(cx,cur_chr); get_next; PutLo(cx,cur_chr); 
            l:=qi(get_jfm_pos(KANJI(cx), f)); end
	  else if cur_cmd=char_num then
	    begin scan_char_num; 
              if (cur_val>=0)and(cur_val<256) then
		begin l:=qi(0); cur_chr:=cur_val; end
	        else begin KANJI(cx):=cur_val;
		  l:=qi(get_jfm_pos(KANJI(cx), f)); end;
	    end
	    else if cur_cmd=inhibit_glue then
	      begin inhibit_glue_flag:=true; goto again_2; end
	      else begin l:=qi(0); KANJI(cx):=max_halfword; end;{goto reswitch;}
	  end;
if KANJI(cx)=min_halfword then begin
  @<Insert pre_break_penalty@>; KANJI(cx):=min_halfword; end;
incert_glue: q:=tail;
if not inhibit_glue_flag then begin
  if char_tag(i)=gk_tag then
  begin k:=glue_kern_start(f)(i);
  repeat j:=font_info[k].qqqq;
    if next_char(j)=l then
      if op_bit(j)<kern_flag then
        begin
  	gp:=font_glue[f];
  	r:=rem_byte(j);
  	if gp<>null then begin
  	  while((type(gp)<>r)and(link(gp)<>null)) do
  	    begin gp:=link(gp); end;
   	  gq:=glue_ptr(gp);
  	end else begin gp:=get_node(small_node_size); font_glue[f]:=gp;
  	  gq:=null;
  	end;
  	if gq=null then begin
  	  type(gp):=r; gq:=new_spec(zero_glue);
  	  glue_ptr(gp):=gq;
  	  k:=exten_base[f]+qi((qo(r))*3); width(gq):=font_info[k].sc;
  	  stretch(gq):=font_info[k+1].sc; shrink(gq):=font_info[k+2].sc;
  	  incr(glue_ref_count(gq)); link(gp):=get_node(small_node_size);
  	  gp:=link(gp); glue_ptr(gp):=null; link(gp):=null;
  	end;
  	tail_append(new_glue(gq)); subtype(tail):=jfm_skip+1;
  	goto skip_loop;
        end else begin
  	tail_append(new_kern(char_kern(f)(j)));
  	goto skip_loop;
        end;
      incr(k);
    until stop_bit(j)>=stop_flag;
  end;
end;
skip_loop:
inhibit_glue_flag:=false;
if KANJI(cx)=max_halfword then goto reswitch;
if KANJI(cx)<>min_halfword then goto main_loop_j3;
goto main_loop;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [46.1032] local cx
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!s:integer; {space factor code}
@!ligature_present:boolean; {should a ligature node be made?}
@y
@!s:integer; {space factor code}
@!ligature_present:boolean; {should a ligature node be made?}
@!cx:KANJI_code;
@!kp:pointer; {kinsoku penalty}
@!gp, gq:pointer;
@!lp:pointer;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [46.1033] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Append character |cur_chr| and the following characters...@>=
f:=cur_font; c:=cur_chr;
@y
@ @<Append character |cur_chr| and the following characters...@>=
inhibit_glue_flag:=false;
f:=cur_font; c:=cur_chr;
@z

@x
main_loop_2: q:=tail; ligature_present:=false; l:=qi(c);
@y
main_loop_2: if lp<>null then q:=lp else q:=tail;
  lp:=null; ligature_present:=false; l:=qi(c);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [46.1036] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Look ahead for ligature...@>=
get_next; {set only |cur_cmd| and |cur_chr|}
if cur_cmd=letter then r:=qi(cur_chr)
@y
@<Look ahead for ligature...@>=
again_1:
get_next; {set only |cur_cmd| and |cur_chr|}
KANJI(cur_jchr):=0;
if cur_cmd=letter then r:=qi(cur_chr)
@z

@x
else if cur_cmd=char_given then r:=qi(cur_chr)
else  begin x_token; {set |cur_cmd|, |cur_chr|, |cur_tok|}
  if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
    r:=qi(cur_chr)
  else if cur_cmd=char_num then
    begin scan_char_num; r:=qi(cur_val);
    end
  else r:=qi(256); {this flag means that no character follows}
  end;
if char_tag(i)=lig_tag then if r<>qi(256) then
  @<Follow the lig/kern program; |goto main_loop_3| if scoring a hit@>;
@<Make a ligature node, if |ligature_present|; insert a discretionary
  node for an explicit hyphen, if |c| is the current |hyphen_char|@>;
if r=qi(256) then goto reswitch; {|cur_cmd|, |cur_chr|, |cur_tok| are untouched}
c:=qo(r); goto main_loop_1 {|f| is still valid}
@y
else if cur_cmd=char_given then
  if (cur_chr>=0)and(cur_chr<256)then r:=qi(cur_chr) else
    begin r:=qi(256); KANJI(cur_jchr):=cur_val; end
else if(cur_cmd >= kanji)and(cur_cmd <= other_kchar) then
  begin r:=qi(256); PutHi(cur_jchr,cur_chr);
    get_next; PutLo(cur_jchr,cur_chr); end
else  begin x_token; {set |cur_cmd|, |cur_chr|, |cur_tok|}
  if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
    r:=qi(cur_chr)
  else if(cur_cmd >= kanji)and(cur_cmd <= other_kchar) then
    begin r:=qi(256); PutHi(cur_jchr,cur_chr);
    get_next; PutLo(cur_jchr,cur_chr); end
  else if cur_cmd=char_num then
    begin scan_char_num;
      if (cur_val>=0)and(cur_val<256) then
        r:=qi(cur_val)
      else begin r:=qi(256); KANJI(cur_jchr):=cur_val;
      end;
    end
  else if cur_cmd=inhibit_glue then
    begin inhibit_glue_flag:=true; goto again_1; end
  else r:=qi(256); {this flag means that no character follows}
  end;
if char_tag(i)=lig_tag then if r<>qi(256) then
  @<Follow the lig/kern program; |goto main_loop_3| if scoring a hit@>;
@<Make a ligature node, if |ligature_present|; insert a discretionary
  node for an explicit hyphen, if |c| is the current |hyphen_char|@>;
if r=qi(256) then
  if KANJI(cur_jchr)<>0 then begin @<Insert post_break_penalty, no check@>;
      cx:=cur_jchr; f:=cur_jfont; l:=qi(get_jfm_pos(KANJI(cx), f));
      i:=char_info(f)(qi(0)); goto incert_glue end
      else goto reswitch; {|cur_cmd|, |cur_chr|, |cur_tok| are untouched}
c:=qo(r); goto main_loop_1 {|f| is still valid}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1039] 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Make a ligature node,...@>=
if ligature_present then
  begin p:=new_ligature(f,l,link(q)); link(q):=p; tail:=p;
  end;
if c=hyphen_char[f] then if mode=hmode then tail_append(new_disc)
@y
@<Make a ligature node,...@>=
if ligature_present then
  @<Make ligature node and Insert post_break_penalty@>;
if c=hyphen_char[f] then if mode=hmode then tail_append(new_disc)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1080] check head = tail
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else  begin if not is_char_node(tail) then
    if (type(tail)=hlist_node)or(type(tail)=vlist_node) then
      @<Remove the last box, unless it's part of a discretionary@>;
@y
else  begin if (not is_char_node(tail)) and (head <> tail) then
    if (type(tail)=hlist_node)or(type(tail)=vlist_node) then
      @<Remove the last box, unless it's part of a discretionary@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1085] end of box, call adjust_hlist
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
hbox_group: package(0);
adjusted_hbox_group: begin adjust_tail:=adjust_head; package(0);
  end;
vbox_group: begin end_graf; package(0);
  end;
vtop_group: begin end_graf; package(vtop_code);
  end;
@y
hbox_group: begin adjust_hlist(head, false); package(0); end;
adjusted_hbox_group: begin adjust_hlist(head, false);
  adjust_tail:=adjust_head; package(0);
  end;
vbox_group: begin end_graf; package(0);
  end;
vtop_group: begin end_graf; package(vtop_code);
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1086] set cur_kanji_skip, cur_xkanji_skip
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin d:=box_max_depth; unsave; save_ptr:=save_ptr-3;
if mode=-hmode then cur_box:=hpack(link(head),saved(2),saved(1))
@y
begin d:=box_max_depth;
delete_glue_ref(cur_kanji_skip);
if auto_spacing>0 then cur_kanji_skip:=kanji_skip
  else cur_kanji_skip:=zero_glue;
add_glue_ref(cur_kanji_skip);
delete_glue_ref(cur_xkanji_skip);
if auto_xspacing>0 then cur_xkanji_skip:=xkanji_skip
  else cur_xkanji_skip:=zero_glue;
add_glue_ref(cur_xkanji_skip);
unsave; save_ptr:=save_ptr-3;
if mode=-hmode then cur_box:=hpack(link(head),saved(2),saved(1))
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1090] apend vmode case
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Cases of |main_control| that build...@>=
vmode+start_par: new_graf(cur_chr>0);
vmode+letter,vmode+other_char,vmode+char_num,vmode+char_given,
   vmode+math_shift,vmode+un_hbox,vmode+vrule,
   vmode+accent,vmode+discretionary,vmode+hskip,vmode+valign,
   vmode+ex_space:@t@>@;@/
  begin back_input; new_graf(true);
  end;
@y
@ @<Cases of |main_control| that build...@>=
vmode+start_par: new_graf(cur_chr>0);
vmode+letter,vmode+other_char,vmode+char_num,vmode+char_given,
   vmode+math_shift,vmode+un_hbox,vmode+vrule,
   vmode+accent,vmode+discretionary,vmode+hskip,vmode+valign,
   vmode+kana,vmode+kanji,vmode+other_kchar,vmode+ex_space:@t@>@;@/
  begin back_input; new_graf(true);
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1096] end_graf, call adjust_hlist
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
procedure end_graf;
begin if mode=hmode then
  begin if head=tail then pop_nest {null paragraphs are ignored}
  else line_break(widow_penalty);
  normal_paragraph;
  error_count:=0;
  end;
end;
@y
procedure end_graf;
begin if mode=hmode then
  begin if head=tail then pop_nest {null paragraphs are ignored}
  else begin adjust_hlist(head, true); line_break(widow_penalty) end;
  normal_paragraph;
  error_count:=0;
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1100] free box node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
    adjust_ptr(tail):=list_ptr(p); delete_glue_ref(q);
    end;
  free_node(p,box_node_size);
@y
    adjust_ptr(tail):=list_ptr(p); delete_glue_ref(q);
    end;
  delete_glue_ref(space_ptr(p));
  delete_glue_ref(xspace_ptr(p));
  free_node(p,box_node_size);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1110] free box node, delete kanji_skip
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else  begin link(tail):=list_ptr(p); box(cur_val):=null;
  free_node(p,box_node_size);
  end;
while link(tail)<>null do tail:=link(tail);
exit:end;
@y
  else begin
    link(tail):=list_ptr(p); box(cur_val):=null;
    delete_glue_ref(space_ptr(p));
    delete_glue_ref(xspace_ptr(p));
    free_node(p,box_node_size);
  end;
  while link(tail)<>null do begin
    p:=tail; tail:=link(tail); 
    if not is_char_node(tail) then
      case type(tail) of
      glue_node :
        if (subtype(tail)=kanji_skip_code+1)
            or(subtype(tail)=xkanji_skip_code+1) then begin
          link(p):=link(tail);
          delete_glue_ref(glue_ptr(tail));
          free_node(tail, small_node_size); tail:=p;
        end;
      penalty_node :
        if subtype(tail)=1 then begin
          link(p):=link(tail); free_node(tail, small_node_size);
          tail:=p;
        end;
      endcases;
  end;
exit:
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1113] italic correction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  begin if is_char_node(tail) then p:=tail
  else if type(tail)=ligature_node then p:=lig_char(tail)
  else return;
  f:=font(p);
  tail_append(new_kern(char_italic(f)(char_info(f)(character(p)))));
  subtype(tail):=explicit;
  end;
@y
  begin if is_char_node(tail) then begin
    p:=tail;
    if last_jchr<>null then
      if link(last_jchr)=tail then p:=last_jchr; end
  else if type(tail)=ligature_node then p:=lig_char(tail)
  else return;
  f:=font(p);
  tail_append(new_kern(char_italic(f)(char_info(f)(character(p)))));
  subtype(tail):=explicit;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47.1123][47.1124][47.1125] make_accent
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
procedure make_accent;
var s,@!t: real; {amount of slant}
@!p,@!q,@!r:pointer; {character, box, and kern nodes}
@!f:internal_font_number; {relevant font}
@!a,@!h,@!x,@!w,@!delta:scaled; {heights and widths, as explained above}
@!i:four_quarters; {character information}
begin scan_char_num; f:=cur_font; p:=new_character(f,cur_val);
if p<>null then
  begin x:=x_height(f); s:=slant(f)/float_constant(65536);
@^real division@>
  a:=char_width(f)(char_info(f)(character(p)));@/
  do_assignments;@/
  @<Create a character node |q| for the next character,
    but set |q:=null| if problems arise@>;
  if q<>null then @<Append the accent with appropriate kerns,
      then set |p:=q|@>;
  link(tail):=p; tail:=p; space_factor:=1000;
  end;
end;

@ @<Create a character node |q| for the next...@>=
q:=null; f:=cur_font;
if (cur_cmd=letter)or(cur_cmd=other_char)or(cur_cmd=char_given) then
  q:=new_character(f,cur_chr)
else if cur_cmd=char_num then
  begin scan_char_num; q:=new_character(f,cur_val);
  end
else back_input
@y
procedure make_accent;
var s,@!t: real; {amount of slant}
@!p,@!q,@!r:pointer; {character, box, and kern nodes}
@!f:internal_font_number; {relevant font}
@!a,@!h,@!x,@!w,@!delta:scaled; {heights and widths, as explained above}
@!i:four_quarters; {character information}
@!cx:KANJI_code;
begin scan_char_num;
if (cur_val<0)or(cur_val>255) then begin
  KANJI(cx):=cur_val;
  f:=cur_jfont; p:=new_character(f,get_jfm_pos(KANJI(cx),f));
  link(p):=get_avail; info(link(p)):=KANJI(cx);
end else begin
  f:=cur_font; p:=new_character(f,cur_val);
end;
if p<>null then
  begin x:=x_height(f); s:=slant(f)/float_constant(65536);
@^real division@>
  a:=char_width(f)(char_info(f)(character(p)));@/
  do_assignments;@/
  @<Create a character node |q| for the next character,
    but set |q:=null| if problems arise@>;
  if q<>null then @<Append the accent with appropriate kerns,
      then set |p:=q|@>;
  link(tail):=p;
  if link(p)<>null then tail:=link(p) else tail:=p;
  space_factor:=1000;
  end;
end;

@ @<Create a character node |q| for the next...@>=
q:=null; f:=cur_font; KANJI(cx):=empty;
if (cur_cmd=letter)or(cur_cmd=other_char) then q:=new_character(f,cur_chr)
else
  if cur_cmd=char_given then
    if (cur_chr>=0)and(cur_chr<256)then q:=new_character(f,cur_chr)
    else KANJI(cx):=cur_chr
  else if cur_cmd=char_num then begin scan_char_num;
    if (cur_chr>=0)and(cur_chr<256)then q:=new_character(f,cur_val)
      else KANJI(cx):=cur_chr
    end else
      if (cur_cmd=kanji)or(cur_cmd=kana)or(cur_cmd=other_kchar)then
      begin f:=cur_jfont; PutHi(cx,cur_chr); get_next; PutLo(cx,cur_chr);
      end
else back_input;
if KANJI(cx)<>empty then begin
  q:=new_character(f,get_jfm_pos(KANJI(cx), f));
  link(q):=get_avail; info(link(q)):=KANJI(cx); end;
@z

@x
if h<>x then {the accent must be shifted up or down}
  begin p:=hpack(p,natural); shift_amount(p):=x-h;
  end;
@y
if h<>x then {the accent must be shifted up or down}
  begin
    delete_glue_ref(cur_kanji_skip); delete_glue_ref(cur_xkanji_skip);
    cur_kanji_skip:=zero_glue; cur_xkanji_skip:=zero_glue;
    add_glue_ref(cur_kanji_skip);	add_glue_ref(cur_xkanji_skip);
    p:=hpack(p,natural); shift_amount(p):=x-h;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1139] init math
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Go into ordinary math mode@>=
begin push_math(math_shift_group); eq_word_define(int_base+cur_fam_code,-1);
if every_math<>null then begin_token_list(every_math,every_math_text);
end
@y
@ @<Go into ordinary math mode@>=
begin push_math(math_shift_group); eq_word_define(int_base+cur_fam_code,-1);
{eq_word_define(int_base+cur_jfam_code,-1);}
if every_math<>null then begin_token_list(every_math,every_math_text);
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1145] Call adjust_hlist at begin of display
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else  begin line_break(display_widow_penalty);@/
@y
else  begin adjust_hlist(head, true); line_break(display_widow_penalty);@/
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1147] Skip kanji 2nd node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Let |d| be the natural width of node |p|...@>=
reswitch: if is_char_node(p) then
  begin f:=font(p); d:=char_width(f)(char_info(f)(character(p)));
  goto found;
@y
@ @<Let |d| be the natural width of node |p|...@>=
reswitch: if is_char_node(p) then
  begin f:=font(p); d:=char_width(f)(char_info(f)(character(p)));
  if font_jtype[f] then p:=link(p);
  goto found;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1150] scan_math
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  back_input; scan_math(nucleus(tail));
@y
  back_input; scan_math(nucleus(tail),kcode_noad(tail));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1151] scan_math
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
procedure scan_math(@!p:pointer);
label restart,reswitch,exit;
var c:integer; {math character code}
begin restart:@<Get the next non-blank non-relax...@>;
@y
procedure scan_math(@!p,@!q:pointer);
label restart,reswitch,exit;
var c:integer; {math character code}
cx:KANJI_code;
begin KANJI(cx):=0;
restart:@<Get the next non-blank non-relax...@>;
@z

@x
letter,other_char,char_given: if cur_chr>=128 then c:=cur_chr
  else  begin c:=ho(math_code(cur_chr));
    if c=@'100000 then
      begin @<Treat |cur_chr| as an active character@>;
      goto restart;
      end;
    end;
@y
letter,other_char,char_given:
  if (cur_chr>=0)and(cur_chr<=256) then
    begin if cur_chr>=128 then c:=cur_chr
      else  begin c:=ho(math_code(cur_chr));
        if c=@'100000 then
          begin @<Treat |cur_chr| as an active character@>;
          goto restart;
          end;
        end;
    end else KANJI(cx):=cur_chr;
kanji,kana,other_kchar: begin PutHi(cx,cur_chr); get_next; PutLo(cx,cur_chr);
  end;
@z

@x
math_type(p):=math_char; character(p):=qi(c mod 256);
if (c>=var_code)and fam_in_range then fam(p):=cur_fam
else fam(p):=(c div 256) mod 16;
exit:end;
@y
if KANJI(cx)=0 then begin math_type(p):=math_char; character(p):=qi(c mod 256);
  if (c>=var_code)and fam_in_range then fam(p):=cur_fam
  else fam(p):=(c div 256) mod 16;
  end else begin 
    if q=null then begin math_type(p):=sub_mlist; info(p):=new_noad;
      p:=nucleus(info(p)); q:=kcode_noad_nucleus(p); end;
    math_type(p):=math_jchar; character(p):=qi(0);
    info(q):=KANJI(cx); fam(p):=cur_jfam;
  end;
exit:end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1154] math mode
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
mmode+letter,mmode+other_char,mmode+char_given: if cur_chr<128 then
    set_math_char(ho(math_code(cur_chr)))
  else set_math_char(cur_chr);
mmode+char_num: begin scan_char_num; cur_chr:=cur_val;
  if cur_chr<128 then set_math_char(ho(math_code(cur_chr)))
  else set_math_char(cur_chr);
@y
mmode+letter,mmode+other_char,mmode+char_given:
  if (cur_chr>=0)and(cur_chr<256) then
    if cur_chr<128 then set_math_char(ho(math_code(cur_chr)))
    else set_math_char(cur_chr)
  else set_math_kchar(cur_chr);
mmode+kanji,mmode+kana,mmode+other_kchar: begin PutHi(cx,cur_chr);
  get_next; PutLo(cx,cur_chr); set_math_kchar(KANJI(cx));
  end;
mmode+char_num: begin scan_char_num;
  if (cur_chr>=0)and(cur_chr<256) then 
  begin cur_chr:=cur_val;
    if cur_chr<128 then set_math_char(ho(math_code(cur_chr)))
    else set_math_char(cur_chr);
  end else set_math_kchar(cur_chr);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1158] scan_math
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  type(tail):=cur_chr; scan_math(nucleus(tail));
@y
  type(tail):=cur_chr; scan_math(nucleus(tail),kcode_noad(tail));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1163] scan_math
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
scan_delimiter(left_delimiter(tail),true); scan_math(nucleus(tail));
@y
scan_delimiter(left_delimiter(tail),true);
scan_math(nucleus(tail),kcode_noad(tail));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1164] scan_math
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
scan_math(nucleus(tail));
@y
scan_math(nucleus(tail),kcode_noad(tail));
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1176] scan_math
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
scan_math(p);
@y
scan_math(p,null);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1186] copy kanji code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
     if math_type(supscr(p))=empty then
      begin mem[saved(0)].hh:=mem[nucleus(p)].hh;
@y
     if ((math_type(supscr(p))=empty)and(math_kcode(p)=null)) then
      begin mem[saved(0)].hh:=mem[nucleus(p)].hh;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1194] set cur_kanji_skip, cur_xkanji_skip
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Check that the necessary fonts for math symbols are present;
  if not, flush the current math lists and set |danger:=true|@>;
m:=mode; l:=false; p:=fin_mlist(null); {this pops the nest}
if mode=-m then {end of equation number}
@y
@<Check that the necessary fonts for math symbols are present;
  if not, flush the current math lists and set |danger:=true|@>;
delete_glue_ref(cur_kanji_skip); delete_glue_ref(cur_xkanji_skip);
if auto_spacing>0 then cur_kanji_skip:=kanji_skip
  else cur_kanji_skip:=zero_glue;
if auto_xspacing>0 then cur_xkanji_skip:=xkanji_skip
  else cur_xkanji_skip:=zero_glue;
add_glue_ref(cur_kanji_skip);	add_glue_ref(cur_xkanji_skip);
m:=mode; l:=false; p:=fin_mlist(null); {this pops the nest}
if mode=-m then {end of equation number}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48.1201] free box node
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
   (total_shrink[filll]<>0)) then
  begin free_node(b,box_node_size);
  b:=hpack(p,z-q,exactly);
@y
   (total_shrink[filll]<>0)) then
  begin delete_glue_ref(space_ptr(b));
  delete_glue_ref(xspace_ptr(b));
  free_node(b,box_node_size);
  b:=hpack(p,z-q,exactly);
@z
@x
  if w>z then
    begin free_node(b,box_node_size);
    b:=hpack(p,z,exactly);
@y
  if w>z then
    begin delete_glue_ref(space_ptr(b));
    delete_glue_ref(xspace_ptr(b));
    free_node(b,box_node_size);
    b:=hpack(p,z,exactly);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1210] set_auto_spacing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ Every prefix, and every command code that might or might not be prefixed,
calls the action procedure |prefixed_command|. This routine accumulates
a sequence of prefixes until coming to a non-prefix, then it carries out
the command.

@<Cases of |main_control| that don't...@>=
any_mode(toks_register),
any_mode(assign_toks),
any_mode(assign_int),
@y
@ Every prefix, and every command code that might or might not be prefixed,
calls the action procedure |prefixed_command|. This routine accumulates
a sequence of prefixes until coming to a non-prefix, then it carries out
the command.

@<Cases of |main_control| that don't...@>=
any_mode(assign_kinsoku),
any_mode(assign_inhibit_xsp_code),
any_mode(set_auto_spacing),
any_mode(toks_register),
any_mode(assign_toks),
any_mode(assign_int),
any_mode(def_jfont),
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1214] dword_define
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d global==(a>=4)
@d define(#)==if global then geq_define(#)@+else eq_define(#)
@d word_define(#)==if global then geq_word_define(#)@+else eq_word_define(#)
@y
@d global==(a>=4)
@d define(#)==if global then geq_define(#)@+else eq_define(#)
@d word_define(#)==if global then geq_word_define(#)@+else eq_word_define(#)
@d dword_define(#)==if global then geq_dword_define(#)@+else eq_dword_define(#)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1217] select cur font
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Assignments@>=
set_font: define(cur_font_loc,data,cur_chr);

@y
@<Assignments@>=
set_font: begin
if font_jtype[cur_chr] then define(cur_jfont_loc,data,cur_chr)
else define(cur_font_loc,data,cur_chr) end;

@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1228] assign jfam
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
assign_int: begin p:=cur_chr; scan_optional_equals; scan_int;
  word_define(p,cur_val);
  end;
assign_dimen: begin p:=cur_chr; scan_optional_equals;
@y
assign_int: begin p:=cur_chr; scan_optional_equals; scan_int;
  if p=int_base+cur_fam_code then begin
    if font_jtype[fam_fnt(cur_val)] then word_define(p+1, cur_val)
      else word_define(p,cur_val);
  end else word_define(p,cur_val);
  end;
assign_dimen: begin p:=cur_chr; scan_optional_equals;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1230][49.1231] xspcode
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
primitive("catcode",def_code,cat_code_base);
@!@:cat_code_}{\.{\\catcode} primitive@>
@y
primitive("catcode",def_code,cat_code_base);
@!@:cat_code_}{\.{\\catcode} primitive@>
primitive("xspcode",def_code,auto_xsp_code_base);
@!@:auto_xsp_code_}{\.{\\xspcode} primitive@>
@z

@x
def_code: if chr_code=cat_code_base then print_esc("catcode")
  else if chr_code=math_code_base then print_esc("mathcode")
@y
def_code: if chr_code=cat_code_base then print_esc("catcode")
  else if chr_code=auto_xsp_code_base then print_esc("xspcode")
  else if chr_code=math_code_base then print_esc("mathcode")
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1256] def_jfont
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
def_font: new_font(a);
@y
def_jfont, def_font: new_font(a);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1275] a_open_in of \read file needs path specifier
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  if a_open_in(read_file[n]) then read_open[n]:=just_open;
@y
  if a_open_in(read_file[n],read_path_spec) then read_open[n]:=just_open;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1289] shift_case
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
t:=info(p);
if t<cs_token_flag+single_base then
  begin if t>=cs_token_flag then t:=t-active_base;
  c:=t mod 256;
  if c<128 then if equiv(b+c)<>0 then t:=256*(t div 256)+equiv(b+c);
  if t>=cs_token_flag then info(p):=t+active_base
  else info(p):=t;
  end
@y
t:=info(p);
if t<cs_token_flag+single_base then
  if (t>kanji_token_base)and(t<kanji_token_end) then p:=link(p)
  else begin if t>=cs_token_flag then t:=t-active_base;
    c:=t mod 256;
    if c<128 then if equiv(b+c)<>0 then t:=256*(t div 256)+equiv(b+c);
    if t>=cs_token_flag then info(p):=t+active_base
    else info(p):=t;
    end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49.1291] show_mode
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d show_lists=3 { \.{\\showlists} }
@y
@d show_lists=3 { \.{\\showlists} }
@d show_mode=4 { \.{\\showmode} }
@z

@x
primitive("showlists",xray,show_lists);
@!@:show_lists_}{\.{\\showlists} primitive@>
@y
primitive("showlists",xray,show_lists);
@!@:show_lists_}{\.{\\showlists} primitive@>
primitive("showmode",xray,show_mode);
@!@:show_mode_}{\.{\\showmode} primitive@>
@z

@x
  show_lists:print_esc("showlists");
  othercases print_esc("show")
@y
  show_lists:print_esc("showlists");
  show_mode:print_esc("showmode");
  othercases print_esc("show")
@z

@x
show_box_code: @<Show the current contents of a box@>;
show_code: @<Show the current meaning of a token, then |goto common_ending|@>;
@y
show_box_code: @<Show the current contents of a box@>;
show_code: @<Show the current meaning of a token, then |goto common_ending|@>;
show_mode: @<Show the courent japanese prcessing mode@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1302] Eliminate unused variable w in |store_fmt_file|
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!init procedure store_fmt_file;
label found1,found2,done1,done2;
var j,@!k,@!l:integer; {all-purpose indices}
@!p,@!q: pointer; {all-purpose pointers}
@!x: integer; {something to dump}
@!w: four_quarters; {four ASCII codes}
@y
@!init procedure store_fmt_file;
label found1,found2,done1,done2;
var j,@!k,@!l:integer; {all-purpose indices}
@!p,@!q: pointer; {all-purpose pointers}
@!x: integer; {something to dump}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1303] Eliminate unused variable w in |load_fmt_file|
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p @t\4@>@<Declare the function called |open_fmt_file|@>@;
function load_fmt_file:boolean;
label bad_fmt,exit;
var j,@!k:integer; {all-purpose indices}
@!p,@!q: pointer; {all-purpose pointers}
@!x: integer; {something undumped}
@!w: four_quarters; {four ASCII codes}
@y
@p @t\4@>@<Declare the function called |open_fmt_file|@>@;
function load_fmt_file:boolean;
label bad_fmt,exit;
var j,@!k:integer; {all-purpose indices}
@!p,@!q: pointer; {all-purpose pointers}
@!x: integer; {something undumped}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1305] Fix reading and writing fmt_file for C
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d dump_wd(#)==begin fmt_file^:=#; put(fmt_file);@+end
@d dump_int(#)==begin fmt_file^.int:=#; put(fmt_file);@+end
@d dump_hh(#)==begin fmt_file^.hh:=#; put(fmt_file);@+end
@d dump_qqqq(#)==begin fmt_file^.qqqq:=#; put(fmt_file);@+end
@y
@d dump_wd(#)==put_fmt_word(#)
@d dump_int(#)==put_fmt_int(#)
@d dump_hh(#)==put_fmt_hh(#)
@d dump_qqqq(#)==put_fmt_qqqq(#)
@z

@x
@d undump_wd(#)==begin get(fmt_file); #:=fmt_file^;@+end
@d undump_int(#)==begin get(fmt_file); #:=fmt_file^.int;@+end
@d undump_hh(#)==begin get(fmt_file); #:=fmt_file^.hh;@+end
@d undump_qqqq(#)==begin get(fmt_file); #:=fmt_file^.qqqq;@+end
@y
@d undump_wd(#)==get_fmt_word(#)
@d undump_int(#)==get_fmt_int(#)
@d undump_hh(#)==get_fmt_hh(#)
@d undump_qqqq(#)==get_fmt_qqqq(#)
@z

@x
x:=fmt_file^.int;
@y
get_fmt_int(x);		{This is reading the first word of the fmt file}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1309][50.1310] dump & undump string_pool
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
for k:=0 to str_ptr do dump_int(str_start[k]);
k:=0;
while k+4<pool_ptr do
  begin dump_four_ASCII; k:=k+4;
  end;
k:=pool_ptr-4; dump_four_ASCII;
@y
dump_things(str_start[0], str_ptr+1);
dump_things(str_pool[0], pool_ptr);
@z

@x
for k:=0 to str_ptr do undump(0)(pool_ptr)(str_start[k]);
k:=0;
while k+4<pool_ptr do
  begin undump_four_ASCII; k:=k+4;
  end;
k:=pool_ptr-4; undump_four_ASCII
@y
undump_things(str_start[0], str_ptr+1);
undump_things(str_pool[0], pool_ptr);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1311][50.1312] dump & undump dynamic_memory
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
repeat for k:=p to q+1 do dump_wd(mem[k]);
x:=x+q+2-p; var_used:=var_used+q-p;
p:=q+node_size(q); q:=rlink(q);
until q=rover;
var_used:=var_used+lo_mem_max-p; dyn_used:=mem_end+1-hi_mem_min;@/
for k:=p to lo_mem_max do dump_wd(mem[k]);
x:=x+lo_mem_max+1-p;
dump_int(hi_mem_min); dump_int(avail);
for k:=hi_mem_min to mem_end do dump_wd(mem[k]);
@y
repeat
 dump_things(mem[p], q+2-p);
x:=x+q+2-p; var_used:=var_used+q-p;
p:=q+node_size(q); q:=rlink(q);
until q=rover;
var_used:=var_used+lo_mem_max-p; dyn_used:=mem_end+1-hi_mem_min;@/
dump_things(mem[p], lo_mem_max+1-p);
x:=x+lo_mem_max+1-p;
dump_int(hi_mem_min); dump_int(avail);
dump_things(mem[hi_mem_min], mem_end+1-hi_mem_min);
@z

@x
repeat for k:=p to q+1 do undump_wd(mem[k]);
p:=q+node_size(q);
if (p>lo_mem_max)or((q>=rlink(q))and(rlink(q)<>rover)) then goto bad_fmt;
q:=rlink(q);
until q=rover;
for k:=p to lo_mem_max do undump_wd(mem[k]);
@y
repeat
  undump_things(mem[p], q+2-p);
p:=q+node_size(q);
if (p>lo_mem_max)or((q>=rlink(q))and(rlink(q)<>rover)) then goto bad_fmt;
q:=rlink(q);
until q=rover;
undump_things(mem[p], lo_mem_max+1-p);
@z

@x
for k:=hi_mem_min to mem_end do undump_wd(mem[k]);
@y
undump_things(mem[hi_mem_min], mem_end+1-hi_mem_min);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1315]-[50.1317] dump,umdump 1 to 6 of eqtb
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
while k<l do
  begin dump_wd(eqtb[k]); incr(k);
  end;
@y
dump_things(eqtb[k], l-k);
@z

@x
while k<l do
  begin dump_wd(eqtb[k]); incr(k);
  end;
@y
dump_things(eqtb[k], l-k);
@z

@x
for j:=k to k+x-1 do undump_wd(eqtb[j]);
@y
undump_things(eqtb[k], x);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1318][50.1319] dump,umdump hash table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
for p:=hash_used+1 to undefined_control_sequence-1 do dump_hh(hash[p]);
@y
dump_things(hash[hash_used+1], undefined_control_sequence-1-hash_used);
@z

@x
for p:=hash_used+1 to undefined_control_sequence-1 do undump_hh(hash[p]);
@y
undump_things(hash[hash_used+1], undefined_control_sequence-1-hash_used);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1322][50.1323] dump,umdump font_jtype[]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
for k:=0 to fmem_ptr-1 do dump_wd(font_info[k]);
dump_int(font_ptr);
for k:=null_font to font_ptr do
  @<Dump the array info for internal font number |k|@>;
@y
@<Dump the array info for internal font number |k|@>;
@z

%@x
%for k:=0 to fmem_ptr-1 do dump_wd(font_info[k]);
%dump_int(font_ptr);
%for k:=null_font to font_ptr do
%  @<Dump the array info for internal font number |k|@>;
%@y
%dump_things(font_info[0], fmem_ptr);
%dump_int(font_ptr);
%dump_things(font_jtype[null_font], font_ptr+1-null_font);
%dump_things(font_size[null_font], font_ptr+1-null_font);
%dump_things(font_dsize[null_font], font_ptr+1-null_font);
%dump_things(font_params[null_font], font_ptr+1-null_font);
%dump_things(font_num_ext[null_font], font_ptr+1-null_font);
%dump_things(ctype_base[null_font], font_ptr+1-null_font);
%dump_things(hyphen_char[null_font], font_ptr+1-null_font);
%dump_things(skew_char[null_font], font_ptr+1-null_font);
%dump_things(font_name[null_font], font_ptr+1-null_font);
%dump_things(font_area[null_font], font_ptr+1-null_font);
%dump_things(font_bc[null_font], font_ptr+1-null_font);
%dump_things(font_ec[null_font], font_ptr+1-null_font);
%dump_things(char_base[null_font], font_ptr+1-null_font);
%dump_things(width_base[null_font], font_ptr+1-null_font);
%dump_things(height_base[null_font], font_ptr+1-null_font);
%dump_things(depth_base[null_font], font_ptr+1-null_font);
%dump_things(italic_base[null_font], font_ptr+1-null_font);
%dump_things(lig_kern_base[null_font], font_ptr+1-null_font);
%dump_things(kern_base[null_font], font_ptr+1-null_font);
%dump_things(exten_base[null_font], font_ptr+1-null_font);
%dump_things(param_base[null_font], font_ptr+1-null_font);
%for k:=null_font to font_ptr do begin
%  print_nl("\font"); print_esc(font_id_text(k)); print_char("=");
%  print_file_name(font_name[k],font_area[k],"");
%  if font_size[k]<>font_dsize[k] then begin
%    print(" at "); print_scaled(font_size[k]); print("pt");
%  end;
%end;
%@z

@x
for k:=0 to fmem_ptr-1 do undump_wd(font_info[k]);
undump_size(font_base)(font_max)('font max')(font_ptr);
for k:=null_font to font_ptr do
  @<Undump the array info for internal font number |k|@>
@y
@<Undump the array info for internal font number |k|@>;
@z

%@x
%for k:=0 to fmem_ptr-1 do undump_wd(font_info[k]);
%undump_size(font_base)(font_max)('font max')(font_ptr);
%for k:=null_font to font_ptr do
%  @<Undump the array info for internal font number |k|@>
%@y
%undump_things(font_info[0], fmem_ptr);
%undump_size(font_base)(font_max)('font max')(font_ptr);
%undump_things(font_jtype[null_font], font_ptr+1-null_font);
%undump_things(font_size[null_font], font_ptr+1-null_font);
%undump_things(font_dsize[null_font], font_ptr+1-null_font);
%undump_things(font_params[null_font], font_ptr+1-null_font);
%undump_things(font_num_ext[null_font], font_ptr+1-null_font);
%undump_things(ctype_base[null_font], font_ptr+1-null_font);
%undump_things(hyphen_char[null_font], font_ptr+1-null_font);
%undump_things(skew_char[null_font], font_ptr+1-null_font);
%undump_things(font_name[null_font], font_ptr+1-null_font);
%undump_things(font_area[null_font], font_ptr+1-null_font);
%undump_things(font_bc[null_font], font_ptr+1-null_font);
%undump_things(font_ec[null_font], font_ptr+1-null_font);
%undump_things(char_base[null_font], font_ptr+1-null_font);
%undump_things(width_base[null_font], font_ptr+1-null_font);
%undump_things(height_base[null_font], font_ptr+1-null_font);
%undump_things(depth_base[null_font], font_ptr+1-null_font);
%undump_things(italic_base[null_font], font_ptr+1-null_font);
%undump_things(lig_kern_base[null_font], font_ptr+1-null_font);
%undump_things(kern_base[null_font], font_ptr+1-null_font);
%undump_things(exten_base[null_font], font_ptr+1-null_font);
%undump_things(param_base[null_font], font_ptr+1-null_font);
%@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1322] Writing font info (almost at end of dump stuff)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Knuth's code writes all the information relevant to a single font
% in the same section of the fmt file.  But it's a lot faster to
% write the arrays of information out, one whole array at a time.
% So that's the way we handle dumping and undumping font info.
@x
@ @<Dump the array info for internal font number |k|@>=
begin dump_qqqq(font_check[k]);
dump_int(font_size[k]);
dump_int(font_dsize[k]);
dump_int(font_params[k]);@/
dump_int(hyphen_char[k]);
dump_int(skew_char[k]);@/
dump_int(font_name[k]);
dump_int(font_area[k]);@/
dump_int(font_bc[k]);
dump_int(font_ec[k]);@/
dump_int(char_base[k]);
dump_int(width_base[k]);
dump_int(height_base[k]);@/
dump_int(depth_base[k]);
dump_int(italic_base[k]);
dump_int(lig_kern_base[k]);@/
dump_int(kern_base[k]);
dump_int(exten_base[k]);
dump_int(param_base[k]);@/
dump_int(font_glue[k]);@/
print_nl("\font"); print_esc(font_id_text(k)); print_char("=");
print_file_name(font_name[k],font_area[k],"");
if font_size[k]<>font_dsize[k] then
  begin print(" at "); print_scaled(font_size[k]); print("pt");
  end;
end
@y
@ @<Dump the array info for internal font number |k|@>=
begin dump_things(font_info[0], fmem_ptr);
dump_int(font_ptr);
dump_things(font_jtype[null_font], font_ptr+1-null_font);
dump_things(ctype_base[null_font], font_ptr+1-null_font);
dump_things(font_check[null_font], font_ptr+1-null_font);
dump_things(font_size[null_font], font_ptr+1-null_font);
dump_things(font_dsize[null_font], font_ptr+1-null_font);
dump_things(font_params[null_font], font_ptr+1-null_font);
dump_things(font_num_ext[null_font], font_ptr+1-null_font);
dump_things(hyphen_char[null_font], font_ptr+1-null_font);
dump_things(skew_char[null_font], font_ptr+1-null_font);
dump_things(font_name[null_font], font_ptr+1-null_font);
dump_things(font_area[null_font], font_ptr+1-null_font);
dump_things(font_bc[null_font], font_ptr+1-null_font);
dump_things(font_ec[null_font], font_ptr+1-null_font);
dump_things(char_base[null_font], font_ptr+1-null_font);
dump_things(width_base[null_font], font_ptr+1-null_font);
dump_things(height_base[null_font], font_ptr+1-null_font);
dump_things(depth_base[null_font], font_ptr+1-null_font);
dump_things(italic_base[null_font], font_ptr+1-null_font);
dump_things(lig_kern_base[null_font], font_ptr+1-null_font);
dump_things(kern_base[null_font], font_ptr+1-null_font);
dump_things(exten_base[null_font], font_ptr+1-null_font);
dump_things(param_base[null_font], font_ptr+1-null_font);
dump_things(font_glue[null_font], font_ptr+1-null_font);
for k:=null_font to font_ptr do begin
  print_nl("\font"); print_esc(font_id_text(k)); print_char("=");
  print_file_name(font_name[k],font_area[k],"");
  if font_size[k]<>font_dsize[k] then begin
    print(" at "); print_scaled(font_size[k]); print("pt");
  end;
end;
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50.1322] Reading font info for C (Nearly done with undump stuff)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Undump the array info for internal font number |k|@>=
begin undump_qqqq(font_check[k]);@/
undump_int(font_size[k]);
undump_int(font_dsize[k]);
undump(min_halfword)(max_halfword)(font_params[k]);@/
undump_int(hyphen_char[k]);
undump_int(skew_char[k]);@/
undump(0)(str_ptr)(font_name[k]);
undump(0)(str_ptr)(font_area[k]);@/
undump(0)(255)(font_bc[k]);
undump(0)(255)(font_ec[k]);@/
undump_int(char_base[k]);
undump_int(width_base[k]);
undump_int(height_base[k]);@/
undump_int(depth_base[k]);
undump_int(italic_base[k]);
undump_int(lig_kern_base[k]);@/
undump_int(kern_base[k]);
undump_int(exten_base[k]);
undump_int(param_base[k]);@/
undump(min_halfword)(lo_mem_max)(font_glue[k]);
end
@y
@  The way this is done in C makes the reference to
the internal font number meaningless, but putting the code
here preserves the association with the WEB modules.

@<Undump the array info for internal font number |k|@>=
begin undump_things(font_info[0], fmem_ptr);
undump_size(font_base)(font_max)('font max')(font_ptr);
undump_things(font_jtype[null_font], font_ptr+1-null_font);
undump_things(ctype_base[null_font], font_ptr+1-null_font);
undump_things(font_check[null_font], font_ptr+1-null_font);
undump_things(font_size[null_font], font_ptr+1-null_font);
undump_things(font_dsize[null_font], font_ptr+1-null_font);
undump_things(font_params[null_font], font_ptr+1-null_font);
undump_things(font_num_ext[null_font], font_ptr+1-null_font);
undump_things(hyphen_char[null_font], font_ptr+1-null_font);
undump_things(skew_char[null_font], font_ptr+1-null_font);
undump_things(font_name[null_font], font_ptr+1-null_font);
undump_things(font_area[null_font], font_ptr+1-null_font);
undump_things(font_bc[null_font], font_ptr+1-null_font);
undump_things(font_ec[null_font], font_ptr+1-null_font);
undump_things(char_base[null_font], font_ptr+1-null_font);
undump_things(width_base[null_font], font_ptr+1-null_font);
undump_things(height_base[null_font], font_ptr+1-null_font);
undump_things(depth_base[null_font], font_ptr+1-null_font);
undump_things(italic_base[null_font], font_ptr+1-null_font);
undump_things(lig_kern_base[null_font], font_ptr+1-null_font);
undump_things(kern_base[null_font], font_ptr+1-null_font);
undump_things(exten_base[null_font], font_ptr+1-null_font);
undump_things(param_base[null_font], font_ptr+1-null_font);
undump_things(font_glue[null_font], font_ptr+1-null_font);
end
@z

@x
for k:=0 to trie_max do dump_hh(trie[k]);
dump_int(trie_op_ptr);
for k:=min_quarterword+1 to trie_op_ptr do
  begin dump_int(hyf_distance[k]);
  dump_int(hyf_num[k]);
  dump_int(hyf_next[k]);
  end;
@y
dump_things(trie[0], trie_max+1);
dump_int(trie_op_ptr);
dump_things(hyf_distance[min_quarterword+1], trie_op_ptr-min_quarterword);
dump_things(hyf_num[min_quarterword+1], trie_op_ptr-min_quarterword);
dump_things(hyf_next[min_quarterword+1], trie_op_ptr-min_quarterword);
@z

@x
for k:=0 to trie_max do undump_hh(trie[k]);
undump(min_quarterword)(max_quarterword)(trie_op_ptr);
for k:=min_quarterword+1 to trie_op_ptr do
  begin undump(0)(63)(hyf_distance[k]); {a |small_number|}
  undump(0)(63)(hyf_num[k]);
  undump(min_quarterword)(max_quarterword)(hyf_next[k]);
  end
@y
undump_things(trie[0], trie_max+1);
undump(min_quarterword)(max_quarterword)(trie_op_ptr);
undump_things(hyf_distance[min_quarterword+1], trie_op_ptr-min_quarterword);
undump_things(hyf_num[min_quarterword+1], trie_op_ptr-min_quarterword);
undump_things(hyf_next[min_quarterword+1], trie_op_ptr-min_quarterword);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [51.1332] uexit() call depends on `history'; procedure-ize main prog;
%	Also, add call to set_paths
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ Now this is really it: \TeX\ starts and ends here.

The initial test involving |ready_already| should be deleted if the
\PASCAL\ runtime system is smart enough to detect such a ``mistake.''
@y
@ Now this is really it: \TeX\ starts and ends here.

Use the value of |history| to determine what exit-code to use.  We use
1 if |history <> spotless| and 0 otherwise.

@z

@x
@p begin @!{|start_here|}
history:=fatal_error_stop; {in case we quit during initialization}
t_open_out; {open the terminal for output}
@y
@p procedure tex_body;
label @<Labels in the outer block@>@/
var bufindx:0..buf_size; {an index used in a for loop below}
begin @!{|start_here|}
history:=fatal_error_stop; {in case we quit during initialization}
t_open_out; {open the terminal for output}
set_paths; {get default file paths from the Unix environment}
@z

@x
final_end: ready_already:=0;
end.
@y
final_end: do_final_end;
end {tex_body};
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [51.1333] omit call to wake_up_terminal on normal termination
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
wake_up_terminal; @<Finish the \.{DVI} file@>;
@y
@<Finish the \.{DVI} file@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [51.1333] print new line before termination; switch to editor if nec.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
    print(log_name); print_char(".");
    end;
  end;
@y
    print(log_name); print_char(".");
    end;
  end;
print_ln;
if (edit_name_start<>0) and (interaction>batch_mode) then
    calledit(str_pool,edit_name_start,edit_name_length,edit_line);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Core-dump in debugging mode on 0 input
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% In Unix, it's not possible to switch into the debugger while a program
% is running.  The best approximation is to do a core dump, then run
% the debugger on it later.
@x
    begin goto breakpoint;@\ {go to every label at least once}
    breakpoint: m:=0; @{'BREAKPOINT'@}@\
    end
@y
    dump_core {Do something to cause a core dump}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [54.1376] add editor-switch variables to globals
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@* \[54] System-dependent changes.
This section should be replaced, if necessary, by changes to the program
that are necessary to make \TeX\ work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the published program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@^system dependencies@>
@y
@* \[54] System-dependent changes.
Here is a temporary integer, used as a holder during reading and writing of
TFM files, and a temporary memory_word, used in reading/writing format
files.
Also, the variables used to hold ``switch-to-editor'' information.
@^<system dependencies@>

@<Glob...@>=
@!edit_name_start: pool_pointer;
@!edit_name_length,@!edit_line,@!tfm_temp: integer;

@ The |edit_name_start| will be set to point into |str_pool| somewhere after
its beginning if \TeX\ is supposed to switch to an editor on exit.

@<Set init...@>=
edit_name_start:=0;

@ @d reset_auto_spacing_code=0
@d set_auto_spacing_code=1
@d reset_auto_xspacing_code=2
@d set_auto_xspacing_code=3

@<Put each...@>=
primitive("autospacing",set_auto_spacing,set_auto_spacing_code);
@!@:auto_spacing_}{\.{\\autospacing} primitive@>
primitive("noautospacing",set_auto_spacing,reset_auto_spacing_code);
@!@:no_auto_spacing_}{\.{\\noautospacing} primitive@>
primitive("autoxspacing",set_auto_spacing,set_auto_xspacing_code);
@!@:auto_xspacing_}{\.{\\autoxspacing} primitive@>
primitive("noautoxspacing",set_auto_spacing,reset_auto_xspacing_code);
@!@:no_auto_xspacing_}{\.{\\noautoxspacing} primitive@>

@ @<Assignments@>=
set_auto_spacing:begin
  if cur_chr<2 then p:=auto_spacing_code
  else begin p:=auto_xspacing_code; cur_chr:=(cur_chr mod 2); end;
  define(p, data, cur_chr);
end;

@ @<Cases of |print_cmd_chr|...@>=
set_auto_spacing:begin
  if (chr_code mod 2)=0 then print_esc("noauto") else print_esc("auto");
  if chr_code<2 then print("spacing") else print("xspacing");
end;

@ @<Show the courent japanese prcessing mode@>=
begin print_nl("> ");
if auto_spacing>0 then print("auto spacing mode; ")
  else print("no auto spacing mode; ");
print_nl("> ");
if auto_xspacing>0 then print("auto xspacing mode")
  else print("no auto xspacing mode");
goto common_ending;
end

@ @<Glob...@>=
@!first_char,@!last_char:pointer;
@!find_first:boolean;

@ @<Declare procedures needed in |hlist_out|, |vlist_out|@>=
function check_box(box_p:pointer):boolean;
label done;
var @!p:pointer;
@!flag:boolean;
begin flag:=false; p:=box_p;
while p <> null do begin
  if is_char_node(p) then
    repeat
      if find_first then begin
        first_char:=p; find_first:=false end;
      last_char:=p;
      flag:=true;
      if font_jtype[font(p)] then p:=link(p);
      p:=link(p);
      if p=null then goto done;
    until not is_char_node(p);
  case type(p) of
  hlist_node: begin if shift_amount(p)=0 then begin
    if check_box(list_ptr(p)) then flag:=true;
    end else if find_first then find_first:=false else last_char:=null;
    flag:=true; end;
  ligature_node: if check_box(lig_ptr(p)) then flag:=true;
  penalty_node,mark_node,adjust_node,ins_node,whatsit_node:do_nothing;
  othercases begin
    if find_first then find_first:=false else last_char:=null;
    flag:=true;
    end;
  endcases;
  p:=link(p);
  end;
done: check_box:=flag;
end;

@ @<Declare procedures needed in |hlist_out|, |vlist_out|@>=
procedure adjust_hlist(p:pointer;pf:boolean);
label done1;
var q,r,s,t,u,v,x,z:pointer;
i,k:halfword;
last:-1..1;
xsp:0..3;
j:KANJI_code;
f:boolean;
begin
if link(p)=null then goto done1;
if auto_spacing>0 then begin decr(glue_ref_count(space_ptr(p)));
space_ptr(p):=kanji_skip; incr(glue_ref_count(kanji_skip)); end;
if auto_xspacing>0 then begin decr(glue_ref_count(xspace_ptr(p)));
xspace_ptr(p):=xkanji_skip; incr(glue_ref_count(xkanji_skip)); end;
s:=xspace_ptr(p);
u:=space_ptr(p);
i:=0; last:=0;
if (not is_char_node(link(p)))
    and(type(link(p))=glue_node)and(subtype(link(p))=jfm_skip+1) then begin
  v:=link(p); link(p):=link(v);
  fast_delete_glue_ref(glue_ptr(v)); free_node(v,small_node_size);
end;
p:=link(p); v:=p; q:=p;
while p<> null do begin
  if is_char_node(p) then begin
    repeat
      if font_jtype[font(p)] then begin
        KANJI(j):=info(link(p));
        if last<0 then @<Insert ASCII-KANJI spacing@>;
        last:=1; p:=link(p);
      end else begin
        xsp:=auto_xsp_code(qo(character(p)));
        if (xsp mod 2)=1 then
          begin if last>0 then @<Insert KANJI-ASCII spacing@>;
        end;
        if xsp>=2 then last:=-1 else last:=0;
      end;
      q:=p; p:=link(p); incr(i);
      if (i>5)and(pf) then begin
	 if is_char_node(v) then
	   if font_jtype[font(v)] then v:=link(v);
	 v:=link(v);
	 end;
    until not is_char_node(p)
  end else begin case type(p) of
    hlist_node: begin find_first:=true;
      first_char:=null; last_char:=null;
      if shift_amount(p)=0 then begin
        if check_box(list_ptr(p)) then begin
          if first_char <> null then 
            if font_jtype[font(first_char)] then begin
              KANJI(j):=info(link(first_char));
              if last<0 then @<Insert ASCII-KANJI spacing@>;
              if last>0 then @<Insert KANJI-KANJI spacing@>;
            end else begin
	      if last>0 then
                if (auto_xsp_code(qo(character(first_char))) mod 2)=1 then
                  @<Insert KANJI-ASCII spacing@>;
	    end;
          if last_char <> null then begin
            if font_jtype[font(last_char)] then begin
	      last:=1;
	      if is_char_node(link(p)) then
	        if font_jtype[font(link(p))] then begin
		  @<Append KANJI-KANJI spacing @>; p:=link(p);
	        end;
            end else begin
              if auto_xsp_code(qo(character(last_char)))>=2 then
                last:=-1 else last:=0;
            end;
          end else last:=0;
        end else last:=0;
      end else last:=0;
    end;
    ligature_node: begin t:=lig_ptr(p);
      if last>0 then begin
	if is_char_node(t) then
          if (auto_xsp_code(qo(character(t))) mod 2)=1 then
            @<Insert KANJI-ASCII spacing@>;
      end;
      while link(t)<>null do t:=link(t);
      if is_char_node(t) then
        if auto_xsp_code(qo(character(t)))>=2 then
        last:=-1 else last:=0;
    end;
    penalty_node: if is_char_node(link(p)) then
      begin q:=p; p:=link(p);
        if font_jtype[font(p)] then begin
          KANJI(j):=info(link(p));
          if last<0 then @<Insert ASCII-KANJI spacing@> else
            if last>0 then @<Insert KANJI-KANJI spacing@>;
          p:=link(p); last:=1;
        end else begin xsp:=auto_xsp_code(qo(character(p)));
          if (xsp mod 2)=1 then
            if last>0 then @<Insert KANJI-ASCII spacing@>;
	  if xsp>=2 then last:=-1 else last:=0;
        end;
      end;
    math_node,mark_node,adjust_node,ins_node,whatsit_node:do_nothing;
    othercases last:=0;
    endcases;
    q:=p; p:=link(p);
  end;
end;
if (not is_char_node(q))and(type(q)=glue_node)and(subtype(q)=jfm_skip+1) then
  begin
    fast_delete_glue_ref(glue_ptr(q));
    glue_ptr(q):=zero_glue; add_glue_ref(zero_glue);
  end;
if (v<>null)and(pf) then begin
  q:=v; p:=link(v); s:=null;
  if is_char_node(v) then
    if font_jtype[font(v)] then begin q:=p; p:=link(p); end;
  t:=q;
  while(p<>null) do begin
    if is_char_node(p) then begin
      if font_jtype[font(p)] then begin
        KANJI(j):=info(link(p));
        i:=cat_code(Hi(j));
	k:=0;
        if (i=kana)or(i=kanji) then
	  begin t:=q; s:=p; end;
        p:=link(p); q:=p;
      end else begin
	k:=k+1;
	if k>1 then begin q:=p; s:=null; end;
      end;
    end else begin
      case type(p) of
      penalty_node,mark_node,adjust_node,whatsit_node,
        glue_node,kern_node,math_node :do_nothing;
      othercases begin q:=p; s:=null; end;
      endcases;
      end;
    p:=link(p);
    end;
  if s<>null then begin
    s:=new_penalty(jchr_widow_penalty); link(s):=link(t); link(t):=s;
    subtype(s):=1; t:=link(s);
    while(not is_char_node(t)) do begin
      if (type(t)=glue_node)or(type(t)=kern_node) then goto done1;
      t:=link(t);
      end;
    z:=new_glue(u); subtype(z):=kanji_skip_code+1;
    link(z):=link(s); link(s):=z;
    end;
  end;
done1:end;

@ @d new_pos=0
@d cur_pos=1
@d no_entry=1000
@d inhibit_both=0
@d inhibit_previous=1
@d inhibit_after=2

@ @<Insert ASCII-KANJI spacing@>=
begin x:=get_inhibit_pos(j, cur_pos);
  if x<>no_entry then
    if (inhibit_xsp_type(x)=inhibit_both)or
	(inhibit_xsp_type(x)=inhibit_previous) then f:=false else f:=true
  else f:=true;
  if f then begin
    z:=new_glue(s); subtype(z):=xkanji_skip_code+1;
    link(z):=link(q); link(q):=z; q:=z;
  end;
end

@ @<Insert KANJI-ASCII spacing@>=
begin x:=get_inhibit_pos(j, cur_pos);
  if x<>no_entry then
    if (inhibit_xsp_type(x)=inhibit_both)or
	(inhibit_xsp_type(x)=inhibit_after) then f:=false else f:=true
  else f:=true;
  if f then begin
      z:=new_glue(s); subtype(z):=xkanji_skip_code+1;
      link(z):=link(q); link(q):=z; q:=z;
    end;
end

@ @<Insert KANJI-KANJI spacing@>=
begin z:=new_glue(u); subtype(z):=kanji_skip_code+1;
  link(z):=link(q); link(q):=z; q:=z; end

@ @<Append KANJI-KANJI spacing@>=
begin z:=new_glue(u); subtype(z):=kanji_skip_code+1;
  link(z):=link(p); link(p):=z; p:=link(z); q:=z; end

@ @d pre_break_penalty_code=1
@d post_break_penalty_code=2

@<Put each...@>=
primitive("prebreakpenalty",assign_kinsoku,pre_break_penalty_code);
@!@:pre_break_penalty_}{\.{\\prebreakpenalty} primitive@>
primitive("postbreakpenalty",assign_kinsoku,post_break_penalty_code);
@!@:psot_break_penalty_}{\.{\\postbreakpenalty} primitive@>
primitive("inhibitxspcode",assign_inhibit_xsp_code,inhibit_xsp_code_base);
@!@:inhibit_xsp_code_}{\.{\\inhibitxspcode} primitive@>

@ @<Cases of |print_cmd_chr|...@>=
assign_kinsoku: case chr_code of
  pre_break_penalty_code: print_esc("prebreakpenalty");
  post_break_penalty_code: print_esc("postbreakpenalty");
  endcases;
assign_inhibit_xsp_code: print_esc("inhibitxspcode");

@ @<Declare procedures needed in |scan_something|@>=
function get_kinsoku_pos(c:KANJI_code; n:small_number):pointer;
label done, done1;
var p,s:pointer;
cx:KANJI_code;
begin s:=calc_pos(c); p:=s;
if n=new_pos then begin
  repeat if (kinsoku_type(p)=0)or(kinsoku_code(p)=c) then goto done;
    incr2(p); if p>511 then p:=0;
  until s=p; p:=no_entry;
  end else begin
  repeat if kinsoku_type(p)=0 then goto done1;
    if kinsoku_code(p)=c then goto done;
    incr2(p); if p>511 then p:=0;
  until s=p;
done1: p:=no_entry;
  end;
done: get_kinsoku_pos:=p;
end;

@ @<Declare procedures needed in |scan_something|@>=
function get_inhibit_pos(c:KANJI_code; n:small_number):pointer;
label done, done1;
var p,s:pointer;
cx:KANJI_code;
begin s:=calc_pos(c)/2; p:=s;
if n=new_pos then begin
  repeat if (inhibit_xsp_code(p)=0)or(inhibit_xsp_code(p)=c) then goto done;
    incr(p); if p>255 then p:=0;
  until s=p; p:=no_entry;
end else begin
  repeat if inhibit_xsp_code(p)=0 then goto done1;
    if inhibit_xsp_code(p)=c then goto done;
    incr(p); if p>255 then p:=0;
  until s=p;
done1: p:=no_entry;
  end;
done: get_inhibit_pos:=p;
end;

@ @<Assignments@>=
assign_kinsoku:begin
  p:=cur_chr; scan_int; n:=cur_val; scan_optional_equals; scan_int;
  if check_kanji(n) then begin j:=get_kinsoku_pos(tokanji(n), new_pos);
    if j=no_entry then begin
      print_err("KINSOKU table is full!!");
      help1("I'm skip this control sequences.");@/
      error; return;
      end;
    if p=pre_break_penalty_code then
      dword_define(kinsoku_base+j,pre_break_penalty_code,n,cur_val)
    else if p=post_break_penalty_code then
      dword_define(kinsoku_base+j,post_break_penalty_code,n,cur_val)
    else confusion("kinsoku");
  end else begin print_err("Invalid KANJI code ("); print_int(n);
@.Invalid KANJI code@>
    help1("I'm skip this control sequences.");@/
    error; return;
    end;
end;
assign_inhibit_xsp_code:begin
  p:=cur_chr; scan_int; n:=cur_val; scan_optional_equals; scan_int;
  if check_kanji(n) then begin j:=get_inhibit_pos(tokanji(n), new_pos);
    if j=no_entry then begin
      print_err("Inhibit table is full!!");
      help1("I'm skip this control sequences.");@/
      error; return;
      end;
    define(inhibit_xsp_code_base+j,cur_val,n);
  end else begin print_err("Invalid KANJI code ("); print_int(n);
@.Invalid KANJI code@>
    help1("I'm skip this control sequences.");@/
    error; return;
    end;
end;

@ @<Fetch breaking penalty from some table@>=
begin scan_int; q:=get_kinsoku_pos(tokanji(cur_val), cur_pos);
  cur_val_level:=int_val; cur_val:=0;
  if q<>no_entry then
    if m=kinsoku_type(q) then scanned_result(kinsoku(q))(int_val)
end

@ @<Fetch inhibit type from some table@>=
begin scan_int; q:=get_inhibit_pos(tokanji(cur_val), cur_pos);
  cur_val_level:=int_val; cur_val:=3;
  if q<>no_entry then
    cur_val:=inhibit_xsp_type(q);
end

@ @<Insert post_break_penalty, check char_node@>=
if is_char_node(tail) then
  begin
  cx:=tokanji(qo(character(tail))); kp:=get_kinsoku_pos(cx, cur_pos);
  if kp<>no_entry then if kinsoku_type(kp)=post_break_penalty_code then
    tail_append(new_penalty(kinsoku(kp)));
  end;

@ @<Insert post_break_penalty, no check@>=
  begin
  cx:=tokanji(c); kp:=get_kinsoku_pos(cx, cur_pos);
  if kp<>no_entry then if kinsoku_type(kp)=post_break_penalty_code then
    tail_append(new_penalty(kinsoku(kp)));
  end;

@ @<Insert kinsoku penalty@>=
kp:=get_kinsoku_pos(cur_jchr, cur_pos);
if kp<>no_entry then begin
  if kinsoku_type(kp)=pre_break_penalty_code then begin
    if (not is_char_node(q))and(type(q)=penalty_node) then
      penalty(q):=penalty(q)+kinsoku(kp)
      else begin
      p:=link(q); link(q):=new_penalty(kinsoku(kp)); link(link(q)):=p;
      lp:=q;
      end;
    end else tail_append(new_penalty(kinsoku(kp)));
  end;

@ @<Insert pre_break_penalty@>=
begin cx:=tokanji(cur_chr); kp:=get_kinsoku_pos(cx, cur_pos); lp:=null;
  if kp<>no_entry then if kinsoku_type(kp)=pre_break_penalty_code then
    begin if (not is_char_node(tail)) and (type(tail)=penalty_node) then
      penalty(tail):=penalty(tail)+kinsoku(kp)
    else begin lp:=tail; tail_append(new_penalty(kinsoku(kp))); end; end;
end;

@ @<Make ligature node and Insert post_break_penalty@>=
  begin kp:=get_kinsoku_pos(l, cur_pos);
    p:=new_ligature(f,l,link(q)); link(q):=p;
    if (kp<>no_entry)and(kinsoku_type(kp)=pre_break_penalty_code)then
      begin link(q):=new_penalty(kinsoku(kp)); link(link(q)):=p; end;
    tail:=p;
  end;

@ @<Declare act...@>=
procedure set_math_kchar(@!c:integer);
var p:pointer; {the new noad}
begin
p:=new_noad; math_type(nucleus(p)):=math_jchar;
character(nucleus(p)):=qi(0);
math_kcode(p):=c; fam(nucleus(p)):=cur_jfam;
type(p):=ord_noad;
link(tail):=p; tail:=p;
end;

@ @<Local variables for dimension calculations@>=
@!t: eight_bits;

@ @<The KANJI width for |cur_jfont|@>=
v:=char_width(cur_jfont)(char_info(cur_jfont)(qi(0)))

@ @<The KANJI height for |cur_jfont|@>=
begin
t:= height_depth(char_info(cur_jfont)(qi(0)));
v:=char_height(cur_jfont)(t)+char_depth(cur_jfont)(t);
end

@ @<set zero glue@>=
space_ptr(p):=zero_glue;xspace_ptr(p):=zero_glue;
add_glue_ref(zero_glue);add_glue_ref(zero_glue);

@ @<Global...@>=
inhibit_glue_flag:boolean;

@ @<Set init...@>=
inhibit_glue_flag:=false;

@ @<Put each of \TeX's primitives into the hash table@>=
primitive("inhibitglue",inhibit_glue,0);@/
@!@:inhibitglue_}{\.{\\inhibitglue} primitive@>

@ @<Cases of |print_cmd_chr| for symbolic printing of primitives@>=
inhibit_glue: print_esc("inhibitglue");

@ @<Cases of |main_control| that don't...@>=
any_mode(inhibit_glue): inhibit_glue_flag:=true;

@z
