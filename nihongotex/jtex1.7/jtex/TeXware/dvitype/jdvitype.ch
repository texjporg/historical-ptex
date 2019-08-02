%   Change file for the DVItype processor, for ASCII Kanji UNIX.
%                                                   (Shift JIS code only :-< )
%	by Hamano Hisato(hisato-h@ascii.co.jp), ASCII corporation.

%   Change file for the DVItype processor, for use with WEB to C
%   This file was created by Howard Trickey, CSD.Trickey@SCORE, and modified
%   by Pavel Curtis, Pavel@Cornell.

% History:
%   ??     (HT) Original version.
%   4/4/83 (PC) Merged with Pavel's change file and made to work with the
%               version 1.0 of DVItype released with version 0.95 of TeX in
%               February, 1983.
%   4/18   (PC) Added changes to module 47 so that it would work the same
%               when input was a file (or pipe) as with a terminal.
%   6/29  (HWT) Brought up to version 1.1 as released with version 0.99 of
%		TeX, with new change file format
%   7/28  (HWT) Brought up to version 2 as released with version 0.999.
%		Only the banner changes.
%  11/21  (HWT) Brought up to version 2.2 as released with version 1.0.
%  2/19/84 (HWT) Made it use TEXFONTS environment.
%  3/23/84 (HWT) Brought up to version 2.3.
%  7/11/84 (HWT) Brought up to version 2.6 as released with version 1.1.
%  11/7/84 (ETM) Brought up to version 2.7 as released with version 1.2.
%  3/9/88  (ETM) Brought up to version 2.9
%  3/16/88 (ETM) Converted for use with WEB to C.
%  ?/??/89 (ETM) Converted for Japanese TeX

% The module numbers used in this file refer to those in the red-covered
% listing (Version 2, July 1983) (same numbers as in blue-covered).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [0] WEAVE: print changes only
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
\let\maybe=\iffalse
\def\title{pDVI$\,$\lowercase{type} changes for C}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1] Change banner string
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d banner=='This is DVItype, Version 2.9' {printed when the program starts}
@y
@d banner=='This is DVItype, C Version p2.9' {printed when the program starts}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3] Change filenames in program statement
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d print(#)==write(#)
@d print_ln(#)==write_ln(#)

@p program DVI_type(@!dvi_file,@!output);
@y
@d print(#)==write(dvityout,#)
@d print_ln(#)==write_ln(dvityout,#)

@p program pDVI_type;
@z

@x
  begin print_ln(banner);@/
@y
  begin
  setpaths; {read environment, to find TEXFONTS, if there}
  rewrite(dvityout,'dvitype.out'); {prepare typescript for output}
  print_ln(banner);@/
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5] Increase name_length
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!max_fonts=100; {maximum number of distinct fonts per \.{DVI} file}
@!max_widths=10000; {maximum number of different characters among all fonts}
@y
@!max_fonts=100; {maximum number of distinct fonts per \.{DVI} file}
@!max_widths=10000; {maximum number of different characters among all fonts}
@!max_ctype=5000; {maximum number of different ctypes among all jfm fonts}
@z

@x
@!name_length=50; {a file name shouldn't be longer than this}
@y
@!name_length=100; {a file name shouldn't be longer than this}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [7] Remove non-local goto
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ If the \.{DVI} file is badly malformed, the whole process must be aborted;
\.{DVItype} will give up, after issuing an error message about the symptoms
that were noticed.

Such errors might be discovered inside of subroutines inside of subroutines,
so a procedure called |jump_out| has been introduced. This procedure, which
simply transfers control to the label |final_end| at the end of the program,
contains the only non-local |goto| statement in \.{DVItype}.
@^system dependencies@>

@d abort(#)==begin print(' ',#); jump_out;
    end
@d bad_dvi(#)==abort('Bad DVI file: ',#,'!')
@.Bad DVI file@>

@p procedure jump_out;
begin goto final_end;
end;
@y
@ If the \.{DVI} file is badly malformed, the whole process must be aborted;
\.{DVItype} will give up, after issuing an error message about the symptoms
that were noticed.

Such errors might be discovered inside of subroutines inside of subroutines,
so a procedure called |jump_out| has been introduced. This procedure is
actually a macro which calls exit() with a non-zero exit status.

@d jump_out==uexit(1)
@d abort(#)==begin print(' ',#); jump_out;
    end
@d bad_dvi(#)==abort('Bad DVI file: ',#,'!')
@.Bad DVI file@>
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [11] xchr for kanji
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
for i:=@'177 to 255 do xchr[i]:='?';
@y
for i:=@'177 to 255 do xchr[i]:=i;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [16] add 'dirchg' command
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d undefined_commands==250,251,252,253,254,255
@y
@d undefined_commands==250,251,252,253,254
@d dirchg==255
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [17] extended DVI's id
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d id_byte=2 {identifies the kind of \.{DVI} files described here}
@y
@d id_byte=2 {identifies the kind of \.{DVI} files described here}
@d ex_id_byte=3 {identifies the kind of \.{DVI} extention described here}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [23] Fix up opening the binary files
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure open_dvi_file; {prepares to read packed bytes in |dvi_file|}
begin reset(dvi_file);
cur_loc:=0;
end;
@#
procedure open_tfm_file; {prepares to read packed bytes in |tfm_file|}
begin reset(tfm_file,cur_name);
end;
@y
In C, we use the external |test_access| procedure, which also does path
searching based on the user's environment or the default path.

@d read_access_mode=4  {``read'' mode for |test_access|}
@d write_access_mode=2 {``write'' mode for |test_access|}

@d no_file_path=0    {no path searching should be done}
@d font_file_path=3  {path specifier for \.{TFM} files}

@p procedure open_dvi_file; {prepares to read packed bytes in |dvi_file|}
begin
    if argc <> 2 then begin
        write_ln('Usage: dvitype <dvi-file>');
	jump_out;
    end;
    argv(1, cur_name);
    if test_access(read_access_mode,no_file_path) then
	reset(dvi_file, real_name_of_file)
    else begin
	write_ln('DVI file not found');
	jump_out;
    end;
    cur_loc:=0;
end;
@#
procedure open_tfm_file; {prepares to read packed bytes in |tfm_file|}
begin
if test_access(read_access_mode,font_file_path) then
    reset(tfm_file,real_name_of_file)
else begin
    write_ln('TFM file not found');
    jump_out
end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [25] Declare real_name_of_file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
|dvi_file|, and |cur_name| is a string variable that will be set to the
current font metric file name before |open_tfm_file| is called.

@<Glob...@>=
@!cur_loc:integer; {where we are about to look, in |dvi_file|}
@!cur_name:packed array[1..name_length] of char; {external name,
  with no lower case letters}
@y
|dvi_file|, and |cur_name| is a string variable that will be set to the
current font metric file name before |open_tfm_file| is called.
In C, we also have a |real_name_of_file| string, that gets
set by the external |test_access| procedure after path searching.

@<Glob...@>=
@!cur_loc:integer; {where we are about to look, in |dvi_file|}
@!cur_name,@!real_name_of_file:packed array[1..name_length] of char;
	 {external name}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [28] Make dvi_length() and move_to_byte() work.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function dvi_length:integer;
begin set_pos(dvi_file,-1); dvi_length:=cur_pos(dvi_file);
end;
@#
procedure move_to_byte(n:integer);
begin set_pos(dvi_file,n); cur_loc:=n;
end;
@y
@p function dvi_length:integer;
begin zfseek(dvi_file, 0, 2); dvi_length:=ftell(dvi_file);
end;
@#
procedure move_to_byte(n:integer);
begin zfseek(dvi_file, n, 0); cur_loc:=n;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [30] Globals for JFM
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d char_width_end(#)==#]
@d char_width(#)==width[width_base[#]+char_width_end
@d invalid_width==@'17777777777
@y
@d char_width_end(#)==#]
@d char_width(#)==width[width_base[#]+char_width_end
@d invalid_width==@'17777777777
@z

@x
@!width_base:array [0..max_fonts] of integer; {index into |width| table}
@!width:array [0..max_widths] of integer; {character widths, in \.{DVI} units}
@!nf:0..max_fonts; {the number of known fonts}
@!width_ptr:0..max_widths; {the number of known character widths}
@y
@!nf:0..max_fonts; {the number of known fonts}
@!font_nt:array [0..max_fonts] of integer; {numberof words in the ctype table}
@!ctype_base:array [0..max_fonts] of integer; {index into |ctype| table}
@!ctype_code:array [0..max_ctype] of integer; {code of ctype}
@!ctype:array [0..max_ctype] of integer; {character types}
@!ctype_ptr:0..max_ctype; {the number of known character types}
@!width_base:array [0..max_fonts] of integer; {index into |width| table}
@!width:array [0..max_widths] of integer; {character widths, in \.{DVI} units}
@!width_ptr:0..max_widths; {the number of known character widths}

@ serch ctype table

@p function chartype(f,code:integer):eight_bits;
label done;
var h,m,l:eight_bits;
begin
  l:=ctype_base[f]; h:=l+font_nt[f]-1;
  while l<=h do begin
    m:=(l+h)/2;
    if ctype_code[m]=code then begin
      chartype:=ctype[m]; goto done;
    end else
      if ctype_code[m]<code then l:=m+1 else h:=m-1;
  end;
  chartype:=0;
done:
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [31] initialize ctype_ptr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
nf:=0; width_ptr:=0; font_name[0]:=0; font_space[0]:=0;
@y
nf:=0; width_ptr:=0; ctype_ptr:=0; font_name[0]:=0; font_space[0]:=0;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [34]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function in_TFM(@!z:integer):boolean; {input \.{TFM} data or return |false|}
label 9997, {go here when the format is bad}
  9998,  {go here when the information cannot be loaded}
  9999;  {go here to exit}
var k:integer; {index for loops}
@!lh:integer; {length of the header data, in four-byte words}
@!nw:integer; {number of words in the width table}
@!wp:0..max_widths; {new value of |width_ptr| after successful input}
@!alpha,@!beta:integer; {quantities used in the scaling computation}
@y
@p function in_TFM(@!z:integer):boolean; {input \.{TFM} data or return |false|}
label 9997, {go here when the format is bad}
  9998,  {go here when the information cannot be loaded}
  9999;  {go here to exit}
var k:integer; {index for loops}
@!lh:integer; {length of the header data, in four-byte words}
@!nw:integer; {number of words in the width table}
@!wp:0..max_widths; {new value of |width_ptr| after successful input}
@!tp:0..max_ctype;
@!alpha,@!beta:integer; {quantities used in the scaling computation}
@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [35] Read header and ctype table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Read past the header...@>=
read_tfm_word; lh:=b2*256+b3;
read_tfm_word; font_bc[nf]:=b0*256+b1; font_ec[nf]:=b2*256+b3;
if font_ec[nf]<font_bc[nf] then font_bc[nf]:=font_ec[nf]+1;
if width_ptr+font_ec[nf]-font_bc[nf]+1>max_widths then
  begin print_ln('---not loaded, DVItype needs larger width table');
@.DVItype needs larger...@>
    goto 9998;
  end;
wp:=width_ptr+font_ec[nf]-font_bc[nf]+1;
read_tfm_word; nw:=b0*256+b1;
if (nw=0)or(nw>256) then goto 9997;
for k:=1 to 3+lh do
  begin if eof(tfm_file) then goto 9997;
  read_tfm_word;
  if k=4 then
    if b0<128 then tfm_check_sum:=((b0*256+b1)*256+b2)*256+b3
    else tfm_check_sum:=(((b0-256)*256+b1)*256+b2)*256+b3;
  end;
@y
@ @<Read past the header...@>=
  read_tfm_word;
  if (b0*256+b1=11)or(b0*256+b1=9) then begin
    font_nt[nf]:=b2*256+b3;
    if ctype_ptr+font_nt[nf]>max_ctype then begin
      print_ln('---not loaded, DVItype needs larger ctype table');
@.DVItype needs larger...@>
      goto 9998;
    end;
    tp:=ctype_ptr+font_nt[nf];
    read_tfm_word;
  end else begin
    font_nt[nf]:=0; tp:=0
  end;
  lh:=b2*256+b3;
  read_tfm_word; font_bc[nf]:=b0*256+b1; font_ec[nf]:=b2*256+b3;
  if font_ec[nf]<font_bc[nf] then font_bc[nf]:=font_ec[nf]+1;
  if width_ptr+font_ec[nf]-font_bc[nf]+1>max_widths then begin
    print_ln('---not loaded, DVItype needs larger width table');
@.DVItype needs larger...@>
    goto 9998;
  end;
  wp:=width_ptr+font_ec[nf]-font_bc[nf]+1;
  read_tfm_word; nw:=b0*256+b1;
  if (nw=0)or(nw>256) then goto 9997;
  for k:=1 to 3+lh do begin
    if eof(tfm_file) then goto 9997;
    read_tfm_word;
    if k=4 then
      if b0<128 then tfm_check_sum:=((b0*256+b1)*256+b2)*256+b3
      else tfm_check_sum:=(((b0-256)*256+b1)*256+b2)*256+b3;
  end;
  if font_nt[nf]>0 then begin
    read_tfm_word;
    ctype_code[ctype_ptr]:=b0*256+b1; ctype[ctype_ptr]:=b2*256+b3;
    if (ctype_code[ctype_ptr]<>0)or(ctype[ctype_ptr]<>0) then goto 9997;
    for k:=ctype_ptr+1 to tp-1 do begin
      read_tfm_word; ctype_code[k]:=b0*256+b1; ctype[k]:=b2*256+b3;
      {if ctype_code[k]<ctype_code[k-1] then goto 9997;
      if (ctype[k]<font_bc[nf])or(ctype[k]>font_ec[nf]) then goto 9997;}
    end;
    ctype_ptr:=tp;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [39] extended flag
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d char_pixel_width(#)==pixel_width[width_base[#]+char_width_end
@y
@d char_pixel_width(#)==pixel_width[width_base[#]+char_width_end
@d null=0
@d normal=1
@d extended=2
@z

@x
@!numerator,@!denominator:integer; {stated conversion ratio}
@!mag:integer; {magnification factor times 1000}
@y
@!numerator,@!denominator:integer; {stated conversion ratio}
@!mag:integer; {magnification factor times 1000}
@!format:null..extended; {DVI format}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [40] format init
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The following code computes pixel widths by simply rounding the \.{TFM}
@y
@ @<Set init...@>=
format:=null;

@ The following code computes pixel widths by simply rounding the \.{TFM}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [45] Define term_in and term_out and declare dvityout
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
and |term_out| for terminal output.
@^system dependencies@>

@<Glob...@>=
@!buffer:array[0..terminal_line_length] of ASCII_code;
@!term_in:text_file; {the terminal, considered as an input file}
@!term_out:text_file; {the terminal, considered as an output file}
@y
and |term_out| for terminal output.
@^system dependencies@>

@d term_in==stdin
@d term_out==stdout

@<Glob...@>=
@!buffer:array[0..terminal_line_length] of ASCII_code;
@!dvityout:text;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [46] Define update_terminal
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d update_terminal == break(term_out) {empty the terminal output buffer}
@y
@d update_terminal == flush(term_out) {empty the terminal output buffer}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47] Fix input_ln.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure input_ln; {inputs a line from the terminal}
var k:0..terminal_line_length;
begin update_terminal; reset(term_in);
if eoln(term_in) then read_ln(term_in);
k:=0;
while (k<terminal_line_length)and not eoln(term_in) do
  begin buffer[k]:=xord[term_in^]; incr(k); get(term_in);
  end;
buffer[k]:=" ";
end;
@y
@p procedure input_ln; {inputs a line from the terminal}
var k:0..terminal_line_length;
begin update_terminal;
if eoln(term_in) then read_ln(term_in);
k:=0;
while (k<terminal_line_length)and not eoln(term_in) do
  begin buffer[k]:=xord[getc(term_in)]; incr(k);
  end;
buffer[k]:=" ";
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50] Remove call to rewrite(term_out)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin rewrite(term_out); {prepare the terminal for output}
@y
begin
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [56] Fix printing of floating point number
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
print_ln('  Resolution = ',resolution:12:8,' pixels per inch');
if new_mag>0 then print_ln('  New magnification factor = ',new_mag/1000:8:3)
@y
print('  Resolution = ');
print_real(resolution,12,8);
print_ln(' pixels per inch');
if new_mag>0 then begin
  print('  New magnification factor = ');
  print_real(new_mag/1000.0,8,3);
  print_ln('')
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [64] Set default_directory_name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d default_directory_name=='TeXfonts:' {change this to the correct name}
@d default_directory_name_length=9 {change this to the correct length}

@<Glob...@>=
@!default_directory:packed array[1..default_directory_name_length] of char;
@y
Actually, under UNIX the standard area is defined in an external
``site.h'' file.  And the user have a path serached for fonts,
by setting the TEXFONTS environment variable.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [65] Remove initialization of now-defunct array
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Set init...@>=
default_directory:=default_directory_name;
@y
@ (No initialization to be done.  Keep this module to preserve numbering.)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [66] Fix addition of ".tfm" suffix for portability and keep lowercase
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ The string |cur_name| is supposed to be set to the external name of the
\.{TFM} file for the current font. This usually means that we need to
prepend the name of the default directory, and
to append the suffix `\.{.TFM}'. Furthermore, we change lower case letters
to upper case, since |cur_name| is a \PASCAL\ string.
@y
@ The string |cur_name| is supposed to be set to the external name of the
\.{TFM} file for the current font. This usually means that we need to,
at most sites, append the
suffix ``.tfm''. 
@z

@x
if p=0 then
  begin for k:=1 to default_directory_name_length do
    cur_name[k]:=default_directory[k];
  r:=default_directory_name_length;
  end
else r:=0;
@y
r:=0;
@z

@x
  if (names[k]>="a")and(names[k]<="z") then
      cur_name[r]:=xchr[names[k]-@'40]
  else cur_name[r]:=xchr[names[k]];
  end;
cur_name[r+1]:='.'; cur_name[r+2]:='T'; cur_name[r+3]:='F'; cur_name[r+4]:='M'
@y
  cur_name[r]:=xchr[names[k]];
  end;
cur_name[r+1]:='.'; cur_name[r+2]:='t'; cur_name[r+3]:='f'; cur_name[r+4]:='m'
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [67] Kanji display
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!text_buf:array[1..line_length] of ASCII_code; {saved characters}
@y
@!text_buf:array[1..line_length] of eight_bits; {saved characters}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [69] Kanji display
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure flush_text;
var k:0..line_length; {index into |text_buf|}
begin if text_ptr>0 then
  begin if out_mode>errors_only then
    begin print('[');
    for k:=1 to text_ptr do print(xchr[text_buf[k]]);
    print_ln(']');
    end;
  text_ptr:=0;
  end;
end;
@y
@p procedure flush_text;
  var k:0..line_length; {index into |text_buf|}
    c1, c2:eight_bits; {1st & 2nd byte of Kanji code}
    s1, s2:eight_bits; {1st & 2nd byte of sjis code}
  begin
    if text_ptr>0 then begin
      if out_mode>errors_only then begin
        print('[');
        k:=1;
        while k<=text_ptr do begin
          if text_buf[k]<128 then begin
            print(xchr[text_buf[k]]); incr(k)
          end else begin
            c1:=text_buf[k]-128; incr(k);
            c2:=text_buf[k]-128; incr(k);
            s1:=((c1-@'41)div 2)+@'201;
            if s1>@'237 then s1:=s1+@'100;
            if (c1 mod 2)=1 then begin
              s2:=c2+@'37;
              if c2>@'137 then incr(s2);
            end else
              s2:=c2+@'176;
            print(xchr[s1]);
            print(xchr[s2]);
          end;
        end;
        print_ln(']');
      end;
    text_ptr:=0;
    end;
  end;

@ @<Glob...@>=
first_byte:boolean;

@ @<Set init...@>=
first_byte:=true;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [70] Kanji display
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure out_text(c:ASCII_code);
begin if text_ptr=line_length-2 then flush_text;
incr(text_ptr); text_buf[text_ptr]:=c;
end;

@y
@p procedure out_text(c:eight_bits);
  begin
    if c<128 then begin
      if text_ptr>=line_length-2 then flush_text;
      first_byte:=true;
    end else begin
      if first_byte then begin
        if text_ptr>=line_length-3 then flush_text;
        first_byte:=false;
      end else
        first_byte:=true;
    end;
    incr(text_ptr);
    text_buf[text_ptr]:=c;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [71] Direction state and stack
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!h,@!v,@!w,@!x,@!y,@!z,@!hh,@!vv:integer; {current state values}
@!hstack,@!vstack,@!wstack,@!xstack,@!ystack,@!zstack:
  array [0..stack_size] of integer; {pushed down values in \.{DVI} units}
@!hhstack,@!vvstack:
  array [0..stack_size] of integer; {pushed down values in pixels}
@y
@!h,@!v,@!dir,@!w,@!x,@!y,@!z,@!hh,@!vv:integer; {current state values}
@!hstack,@!vstack,@!wstack,@!xstack,@!ystack,@!zstack:
  array [0..stack_size] of integer; {pushed down values in \.{DVI} units}
@!dstack: array [0..stack_size] of integer; {pushed down dir_mode}
@!hhstack,@!vvstack:
  array [0..stack_size] of integer; {pushed down values in pixels}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [75] dirchg par
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
set1,put1,fnt1,xxx1,fnt_def1: first_par:=get_byte;
@y
set1,put1,fnt1,xxx1,fnt_def1,dirchg: first_par:=get_byte;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [79] Kanji display
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
var o:eight_bits; {operation code of the current command}
@!p,@!q:integer; {parameters of the current command}
@!a:integer; {byte number of the current command}
@!hhh:integer; {|h|, rounded to the nearest pixel}
@y
var o:eight_bits; {operation code of the current command}
@!p,@!q:integer; {parameters of the current command}
@!a:integer; {byte number of the current command}
@!hhh,@!vvv:integer; {|h|,|v| rounded to the nearest pixel}
@!c1, c2:eight_bits; {1st & 2nd byte of Kanji code}
@z

@x
s:=0; h:=0; v:=0; w:=0; x:=0; y:=0; z:=0; hh:=0; vv:=0;
  {initialize the state variables}
@y
s:=0; h:=0; v:=0; dir:=0; w:=0; x:=0; y:=0; z:=0; hh:=0; vv:=0;
  {initialize the state variables}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [80] move to row direction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
move_right: @<Finish a command that sets |h:=h+q|, then |goto done|@>;
@y
move_right:
  if dir=0 then begin
    @<Finish a command that sets |h:=h+q|, then |goto done|@>;
  end else begin
    if dir=1 then p:=q else p:=-q;
    @<Finish a command that sets |v:=v+p|, then |goto done|@>;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [81] Kanji display
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Start translation...@>=
if o<set_char_0+128 then @<Translate a |set_char| command@>
else case o of
  four_cases(set1): begin major('set',o-set1+1:1,' ',p:1); goto fin_set;
    end;
  four_cases(put1): begin major('put',o-put1+1:1,' ',p:1); goto fin_set;
    end;
  set_rule: begin major('setrule'); goto fin_rule;
    end;
  put_rule: begin major('putrule'); goto fin_rule;
    end;
  @t\4@>@<Cases for commands |nop|, |bop|, \dots, |pop|@>@;
  @t\4@>@<Cases for horizontal motion@>@;
  othercases if special_cases(o,p,a) then goto done@+else goto 9998
  endcases
@y
@<Start translation...@>=
  if o<set_char_0+128 then @<Translate a |set_char| command@>
  else case o of
    four_cases(set1),four_cases(put1):
      @<Kanji display@>;
    set_rule:
      begin
        major('setrule'); goto fin_rule;
      end;
    put_rule:
      begin
        major('putrule'); goto fin_rule;
      end;
    @t\4@>@<Cases for commands |nop|, |bop|, \dots, |pop|@>@;
    @t\4@>@<Cases for horizontal motion@>@;
    othercases if special_cases(o,p,a) then goto done@+else goto 9998
  endcases

@ Kanji display

@<Kanji display@>=
  begin
    c1:=p div 256; c2:=p mod 256;
    if (c1<@'41)or(c1>@'176)or(c2<@'41)or(c2>@'176) then begin
      if o<put1 then major('set',o-set1+1:1,' ',p:1)
      else major('put',o-put1+1:1,' ',p:1);
    end else begin
      out_text(c1+128); out_text(c2+128);
      if o<put1 then minor('set',o-set1+1:1,' ',p:1)
      else minor('put',o-put1+1:1,' ',p:1);
    end;
    goto fin_set;
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [82] move to column direction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!vvv:integer; {|v|, rounded to the nearest pixel}
@y
@!vvv,hhh:integer; {|v|,|h| rounded to the nearest pixel}
@z

@x
move_down: @<Finish a command that sets |v:=v+p|, then |goto done|@>;
@y
move_down:
  if dir=0 then begin
    @<Finish a command that sets |v:=v+p|, then |goto done|@>;
  end else begin
    if dir=1 then q:=-p else q:=p;
    @<Finish a command that sets |h:=h+q|, then |goto done|@>;
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [83] push pop
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
nop: begin minor('nop'); goto done;
  end;
@y
dirchg:
  begin
    if format=normal then begin
      error('dir command widthin normal dvi file!'); goto 9998;
    end else if format=null then
      format:=extended;
    dir:=p; major('dir ',p:1); goto done;
  end;
nop: begin minor('nop'); goto done;
  end;
@z

@x
  hstack[s]:=h; vstack[s]:=v; wstack[s]:=w;
  xstack[s]:=x; ystack[s]:=y; zstack[s]:=z;
  hhstack[s]:=hh; vvstack[s]:=vv; incr(s); ss:=s-1; goto show_state;
@y
  hstack[s]:=h; vstack[s]:=v; dstack[s]:=dir; wstack[s]:=w;
  xstack[s]:=x; ystack[s]:=y; zstack[s]:=z;
  hhstack[s]:=hh; vvstack[s]:=vv; incr(s); ss:=s-1; goto show_state;
@z

@x
    h:=hstack[s]; v:=vstack[s]; w:=wstack[s];
    x:=xstack[s]; y:=ystack[s]; z:=zstack[s];
@y
    h:=hstack[s]; v:=vstack[s]; dir:=dstack[s]; w:=wstack[s];
    x:=xstack[s]; y:=ystack[s]; z:=zstack[s];
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [84] out_space
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d out_space(#)==if (p>=font_space[cur_font])or(p<=-4*font_space[cur_font]) then
    begin out_text(" "); hh:=pixel_round(h+p);
    end
  else hh:=hh+pixel_round(p);
  minor(#,' ',p:1); q:=p; goto move_right
@y
@d out_space(#)==
  if (p>=font_space[cur_font])or(p<=-4*font_space[cur_font]) then begin
    out_text(" ");
    if dir=0 then hh:=pixel_round(h+p)
    else if dir=1 then vv:=pixel_round(v+p)
    else vv:=pixel_round(v-p);
  end else begin
    if dir=0 then hh:=hh+pixel_round(p)
    else if dir=1 then vv:=vv+pixel_round(p)
    else vv:=vv-pixel_round(p);
  end;
  minor(#,' ',p:1,' dir=',dir:1); q:=p; goto move_right
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [85] out_vmove
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d out_vmove(#)==if abs(p)>=5*font_space[cur_font] then vv:=pixel_round(v+p)
  else vv:=vv+pixel_round(p);
  major(#,' ',p:1); goto move_down
@y
@d out_vmove(#)==
  if abs(p)>=5*font_space[cur_font] then begin
    if dir=0 then vv:=pixel_round(v+p)
    else if dir=1 then hh:=pixel_round(h-p)
    else hh:=pixel_round(h+p);
  end else begin
    if dir=0 then vv:=vv+pixel_round(p)
    else if dir=1 then hh:=hh-pixel_round(p)
    else hh:=hh+pixel_round(p);
  end;
  major(#,' ',p:1,' dir=',dir:1); goto move_down
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [89] char width JFM, direction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Finish a command that either sets or puts a character...@>=
if p<0 then p:=255-((-1-p) mod 256)
@y
@ @<Finish a command that either sets or puts a character...@>=
if font_nt[cur_font]>0 then p:=chartype(cur_font,p);
if p<0 then p:=255-((-1-p) mod 256)
@z

@x
if q=invalid_width then q:=0
else hh:=hh+char_pixel_width(cur_font)(p);
@y
if q=invalid_width then q:=0
else
  if dir=0 then hh:=hh+char_pixel_width(cur_font)(p)
  else if dir=1 then vv:=vv+char_pixel_width(cur_font)(p)
  else vv:=vv-char_pixel_width(cur_font)(p);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [90] direction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
hh:=hh+rule_pixels(q); goto move_right
@y
if dir=0 then hh:=hh+rule_pixels(q)
else if dir=1 then vv:=vv+rule_pixels(q)
else vv:=vv-rule_pixels(q);
goto move_right
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [93] direction
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Show the values of |ss|, |h|, |v|, |w|, |x|, |y|, |z|...@>=
if showing then
  begin print_ln(' ');
  print('level ',ss:1,':(h=',h:1,',v=',v:1,
    ',w=',w:1,',x=',x:1,',y=',y:1,',z=',z:1,
    ',hh=',hh:1,',vv=',vv:1,')');
  end;
goto done
@y
@ @<Show the values of |ss|, |h|, |v|, |w|, |x|, |y|, |z|...@>=
if showing then
  begin print_ln(' ');
  print('level ',ss:1,':(h=',h:1,',v=',v:1,
    ',dir=',dir:1,
    ',w=',w:1,',x=',x:1,',y=',y:1,',z=',z:1,
    ',hh=',hh:1,',vv=',vv:1,')');
  end;
goto done
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [99] Check id_byte of postamble
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
if k<>id_byte then bad_dvi('ID byte is ',k:1);
@.ID byte is wrong@>
@y
if k=id_byte then format:=null
else if k=ex_id_byte then format:=extended
else bad_dvi('ID byte is ',k:1);
@.ID byte is wrong@>
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [104] Check id_byte
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
m:=get_byte;
if m<>id_byte then print_ln('identification in byte ',cur_loc-1:1,
@.identification...should be n@>
    ' should be ',id_byte:1,'!');
@y
m:=get_byte;
if format=extended then begin
  print_ln('This is EXTENDED DVI. ID byte is ',m:1,'.');
  if m<>ex_id_byte then print_ln('identification in byte ',cur_loc-1:1,
@.identification...should be n@>
      ' should be ',ex_id_byte:1,'!');
end else begin
  if m<>id_byte then print_ln('identification in byte ',cur_loc-1:1,
@.identification...should be n@>
      ' should be ',id_byte:1,'!',m:1);
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [106] Print newline at end of program; remove unused label
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
final_end:end.
@y
print_ln(' '); end.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Fix another floating point print
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
print_ln('magnification=',mag:1,'; ',conv:16:8,' pixels per DVI unit')
@y
print('magnification=',mag:1,'; ');
print_real(conv,16,8);
print_ln(' pixels per DVI unit')
@z
