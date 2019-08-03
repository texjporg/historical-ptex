% dvitype.ch for C compilation with web2c.
% 
% The original version of this file was created by Howard Trickey, and
% modified by Pavel Curtis.
%
% 04/04/83 (PC)  Merged with Pavel's change file and made to work with the
%                version 1.0 of DVItype released with version 0.95 of TeX in
%                February, 1983.
% 04/18/83 (PC)  Added changes to module 47 so that it would work the same
%                when input was a file (or pipe) as with a terminal.
% 06/29/83 (HWT) Brought up to version 1.1 as released with version 0.99 of
%                TeX, with new change file format
% 07/28/83 (HWT) Brought up to version 2 as released with version 0.999.
%	         Only the banner changes.
% 11/21/83 (HWT) Brought up to version 2.2 as released with version 1.0.
% 02/19/84 (HWT) Made it use the TEXFONTS environment variable.
% 03/23/84 (HWT) Brought up to version 2.3.
% 07/11/84 (HWT) Brought up to version 2.6 as released with version 1.1.
% 11/07/84 (ETM) Brought up to version 2.7 as released with version 1.2.
% 03/09/88 (ETM) Brought up to version 2.9
% 03/16/88 (ETM) Converted for use with WEB to C.
% 11/30/89 (KB)  To version 3.
% 01/16/90 (SR)  To version 3.2.
% 09/27/95 (KA)  Supporting ASCII pTeX
% (more recent changes in ../ChangeLog.W2C)


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [0] WEAVE: print changes only.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
\let\maybe=\iffalse
\def\title{DVI$\,$\lowercase{type} changes for C}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1] Change banner string.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d banner=='This is DVItype, Version 3.4' {printed when the program starts}
@y
@d banner=='This is pDVItype, Version 0.2 based on DVItype 3.4' {more is printed later}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [3] Change filenames in program statement, etc.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d print(#)==write(#)
@d print_ln(#)==write_ln(#)

@p program DVI_type(@!dvi_file,@!output);
@y
@d print(#)==write(stdout, #)
@d print_ln(#)==write_ln(stdout, #)

@p program DVI_type;
@z

@x
  begin print_ln(banner);@/
@y
  begin
  {read environment to find \.{TEXFONTS}}
  set_paths (TFM_FILE_PATH_BIT);
  {The banner will be printed before the dialogue.}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5] Make name_length match the system constant, and allow more widths.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Constants...@>=
@y
@d name_length==PATH_MAX
@<Constants...@>=
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5] pTeX / NTT JTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!max_fonts=100; {maximum number of distinct fonts per \.{DVI} file}
@y
@!max_fonts=256; {maximum number of distinct fonts per \.{DVI} file}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [5] Make name_length match the system constant, and allow more widths.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!max_widths=10000; {maximum number of different characters among all fonts}
@y
@!max_widths=25000; {maximum number of different characters among all fonts}
@z
@x
@!name_length=50; {a file name shouldn't be longer than this}
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [7] Remove non-local goto.
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
@d abort(#)==begin write_ln(term_out,#); jump_out;
    end
@d bad_dvi(#)==abort('Bad DVI file: ',#,'!')
@.Bad DVI file@>
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [8] Permissive input.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@!ASCII_code=" ".."~"; {a subrange of the integers}
@y
@!ASCII_code=0..255; {a subrange of the integers}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [9] The text_char type is used as an array index into `xord'.  The
% default type `char' produces signed integers, which are bad array
% indices in C.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
@x
@d text_char == char {the data type of characters in text files}
@d first_text_char=0 {ordinal number of the smallest element of |text_char|}
@d last_text_char=127 {ordinal number of the largest element of |text_char|}
@y
@d text_char == ASCII_code {the data type of characters in text files}
@d first_text_char=0 {ordinal number of the smallest element of |text_char|}
@d last_text_char=255 {ordinal number of the largest element of |text_char|}
@z

@x
for i:=@'177 to 255 do xchr[i]:='?';
@y
for i:=@'177 to 255 do xchr[i]:=i;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d undefined_commands==250,251,252,253,254,255
@y
@d dir=255 {pTeX direction}
@d undefined_commands==250,251,252,253,254
@z

@x
@d id_byte=2 {identifies the kind of \.{DVI} files described here}
@y
@d id_byte=2 {identifies the kind of \.{DVI} files described here}
@d ptex_id_byte=3 {identifies the kind of pTeX \.{DVI} files described here}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [23] Fix up opening the binary files.
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
In C, we use the external |test_read_access| procedure, which also does path
searching based on the user's environment or the default path.  Our
version of |reset| will abort the program if its argument can't be
opened.

@p procedure open_dvi_file; {prepares to read packed bytes in |dvi_file|}
begin
    argv (1, cur_name);
    reset (dvi_file, cur_name);
    cur_loc := 0;
end;
@#
procedure open_tfm_file; {prepares to read packed bytes in |tfm_file|}
begin
  if test_read_access (cur_name, TFM_FILE_PATH)
  then begin
    reset (tfm_file, cur_name);
  end else begin
    errprint_pascal_string (cur_name);
    abort (': TFM file not found.');
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [27] Make get_n_bytes routines work with 16-bit math.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
get_two_bytes:=a*256+b;
@y
get_two_bytes:=a*toint(256)+b;
@z
@x
get_three_bytes:=(a*256+b)*256+c;
@y
get_three_bytes:=(a*toint(256)+b)*256+c;
@z
@x
if a<128 then signed_trio:=(a*256+b)*256+c
else signed_trio:=((a-256)*256+b)*256+c;
@y
if a<128 then signed_trio:=(a*toint(256)+b)*256+c
else signed_trio:=((a-toint(256))*256+b)*256+c;
@z
@x
if a<128 then signed_quad:=((a*256+b)*256+c)*256+d
else signed_quad:=(((a-256)*256+b)*256+c)*256+d;
@y
if a<128 then signed_quad:=((a*toint(256)+b)*256+c)*256+d
else signed_quad:=(((a-256)*toint(256)+b)*256+c)*256+d;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [28] dvi_length and move_to_byte.
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
begin
  checked_fseek (dvi_file, 0, 2);
  cur_loc := ftell(dvi_file);
  dvi_length := cur_loc;
end;
@#
procedure move_to_byte(n:integer);
begin
  checked_fseek (dvi_file, n, 0);
  cur_loc:=n;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% JFM and pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!width_ptr:0..max_widths; {the number of known character widths}
@y
@!width_ptr:0..max_widths; {the number of known character widths}
@!fnt_jfm_p:array [0..max_fonts] of boolean;
@!jfm_char_code:array [0..max_widths] of integer;
@!jfm_char_type:array [0..max_widths] of integer;
@!jfm_char_font:array [0..max_widths] of integer;
@!jfm_char_type_count:integer;
@!cur_jfm_char_type:integer;

@ @d jfm_char_type_hash_size=347

@ @<Types...@>=
@!jfm_char_type_hash_value=0..jfm_char_type_hash_size-1;

@ @<Glob...@>=
@!jfm_char_type_hash_table:array[jfm_char_type_hash_value] of integer;
  { first pointer to character information. 0 means null pointer. }
@!jfm_char_type_hash_link:array[0..max_widths] of integer;
  { next pointer to character information. 0 means null pointer. }

@ @<Set init...@>=
for i:=0 to jfm_char_type_hash_size-1 do
  jfm_char_type_hash_table[i] := 0;
jfm_char_type[0]:=0;
jfm_char_type_count:=1;

@ Refer char_type table.

@p function get_jfm_char_type(@!fntn:integer;@!jfmc:integer):integer;
  var p:integer; ct:integer;
begin
  p:=jfm_char_type_hash_table[(jfmc+fntn) mod jfm_char_type_hash_size];
  ct:=0; { default char_type is 0 }
  while p <> 0 do
    if (jfm_char_code[p] = jfmc) and (jfm_char_font[p] = fntn) then
      begin ct:=jfm_char_type[p]; p:=0; end
    else
      p:=jfm_char_type_hash_link[p];
  get_jfm_char_type:=ct;
end;

@ @<Glob...@>=
@!ptex_p:boolean;
@!dd:eight_bits;
@!ddstack:array [0..stack_size] of eight_bits;
@z

@x
@!lh:integer; {length of the header data, in four-byte words}
@y
@!lh:integer; {length of the header data, in four-byte words}
@!nt:integer;
@!jfm_h:integer;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [34] Make 16-bit TFM calculations work.
% and JFM by K.A.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
read_tfm_word; lh:=b2*256+b3;
read_tfm_word; font_bc[nf]:=b0*256+b1; font_ec[nf]:=b2*256+b3;
@y
read_tfm_word; lh:=b0*toint(256)+b1;
if (lh = 11) or (lh = 9) then
  begin
    print(' (JFM');
    fnt_jfm_p[nf] := true;
    if lh = 9 then print(' tate');
    print(')');
    nt:=b2*toint(256)+b3;
    read_tfm_word;
  end
else
  begin
    nt:=0;
    fnt_jfm_p[nf] := false;
  end;
lh:=b2*toint(256)+b3;
read_tfm_word; font_bc[nf]:=b0*toint(256)+b1; font_ec[nf]:=b2*toint(256)+b3;
@z
@x
    if b0<128 then tfm_check_sum:=((b0*256+b1)*256+b2)*256+b3
    else tfm_check_sum:=(((b0-256)*256+b1)*256+b2)*256+b3
@y
    if b0<128 then tfm_check_sum:=((b0*toint(256)+b1)*256+b2)*256+b3
    else tfm_check_sum:=(((b0-256)*toint(256)+b1)*256+b2)*256+b3
@z
@x JFM
      tfm_design_size:=round(tfm_conv*(((b0*256+b1)*256+b2)*256+b3))
    else goto 9997;
  end;
@y
      tfm_design_size:=round(tfm_conv*(((b0*256+b1)*256+b2)*256+b3))
    else goto 9997;
  end;
for k:=1 to nt do
  begin
    read_tfm_word;
    jfm_char_code[jfm_char_type_count]:=b0*toint(256)+b1;
    jfm_char_type[jfm_char_type_count]:=b2*toint(256)+b3;
    jfm_char_font[jfm_char_type_count]:=nf;
    jfm_h:= { hash value }
      (jfm_char_code[jfm_char_type_count]+nf) mod jfm_char_type_hash_size;
    jfm_char_type_hash_link[jfm_char_type_count]:=
      jfm_char_type_hash_table[jfm_h];
    jfm_char_type_hash_table[jfm_h]:=jfm_char_type_count;
    jfm_char_type_count := jfm_char_type_count + 1
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [45] Define term_in and term_out.
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
@d term_out==stderr

@<Glob...@>=
@!buffer:array[0..terminal_line_length] of ASCII_code;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [46] Define update_terminal.
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
% [50] Remove call to rewrite(term_out), print web2c version.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
begin rewrite(term_out); {prepare the terminal for output}
write_ln(term_out,banner);
@y
begin
write (term_out, banner);
write_ln (term_out, version_string);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [56] Fix printing of floating point number.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
print_ln('  Resolution = ',resolution:12:8,' pixels per inch');
if new_mag>0 then print_ln('  New magnification factor = ',new_mag/1000:8:3)
@y
print ('  Resolution = ');
print_real (resolution, 12, 8);
print_ln (' pixels per inch');
if new_mag > 0
then begin
  print ('  New magnification factor = ');
  print_real (new_mag / 1000.0, 8, 3);
  print_ln('')
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [64] Don't set default_directory_name.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d default_directory_name=='TeXfonts:' {change this to the correct name}
@d default_directory_name_length=9 {change this to the correct length}

@<Glob...@>=
@!default_directory:packed array[1..default_directory_name_length] of char;
@y
Under UNIX the standard area is defined in an external
file \.{site.h}.  And the users have a path searched for fonts,
by setting the \.{TEXFONTS} environment variable.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [65] Remove initialization of default_directory.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Set init...@>=
default_directory:=default_directory_name;
@y
@ (No initialization needs to be done.  Keep this module to preserve
numbering.)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [66] Append `.tfm', keep lowercase.
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
at most sites, append the suffix \.{.tfm}. 
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
% JFM
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x Not use fprintf() for JIS
    for k:=1 to text_ptr do print(xchr[text_buf[k]]);
    print_ln(']');
@y
    for k:=1 to text_ptr do print(xchr[text_buf[k]]);
    print(']'); print_ln('');
@z

@x
@p procedure out_text(c:ASCII_code);
begin if text_ptr=line_length-2 then flush_text;
incr(text_ptr); text_buf[text_ptr]:=c;
end;
@y
@p procedure out_text(c:ASCII_code);
begin if text_ptr=line_length-2 then flush_text;
incr(text_ptr);
if c>=177 then text_buf[text_ptr]:=@'77 else text_buf[text_ptr]:=c;
end;

@ declare kanji conversion function

@p procedure out_kanji(c:integer);
begin if text_ptr>=line_length-3 then flush_text;
ifdef('EUC')
  incr(text_ptr); text_buf[text_ptr]:= c div 256 + 128;
  incr(text_ptr); text_buf[text_ptr]:= c mod 256 + 128;
endif('EUC')
ifdef('SJIS')
  c := JIStoSJIS(c);
  incr(text_ptr); text_buf[text_ptr]:= c div 256;
  incr(text_ptr); text_buf[text_ptr]:= c mod 256;
endif('SJIS')
end;

@ output hexdecimal / octal character code.

@d print_hex_digit(#)==if # <= 9 then print((#):1)
                       else case # of
                         10: print(xchr['A']);
                         11: print(xchr['B']);
                         12: print(xchr['C']);
                         13: print(xchr['D']);
                         14: print(xchr['E']);
                         15: print(xchr['F']); { no more cases }
                       end

@p
ifdef('HEX_CHAR_CODE')
procedure print_hex_number(c:integer);
var n:integer;
    b:array[1..8] of integer;
begin
  n:=1;
  while (n<8) and (c<>0) do
    begin b[n]:=c mod 16; c:=c div 16; n:=n+1 end;
  print('"');
  if n=1 then print(xchr['0'])
  else
    begin
      n:=n-1;
      while n>0 do
        begin print_hex_digit(b[n]); n:=n-1 end
    end
end;
endif('HEX_CHAR_CODE')
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
z0: first_par:=z;
@y
z0: first_par:=z;
dir: first_par:=get_byte;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
s:=0; h:=0; v:=0; w:=0; x:=0; y:=0; z:=0; hh:=0; vv:=0;
@y
s:=0; h:=0; v:=0; w:=0; x:=0; y:=0; z:=0; hh:=0; vv:=0; dd:=0;
@z

@x
  four_cases(set1): begin major('set',o-set1+1:1,' ',p:1); goto fin_set;
@y
  four_cases(set1):
    begin
      if fnt_jfm_p[cur_font]=true then
        begin
          cur_jfm_char_type:=get_jfm_char_type(cur_font,p);
          out_kanji(p);
          minor('set',o-set1+1:1,' ',p:1);
ifdef('HEX_CHAR_CODE')
          print('(');
          print_hex_number(p);
          print(')');
endif('HEX_CHAR_CODE')
          print(' type=',cur_jfm_char_type);
          p:=cur_jfm_char_type
        end
      else begin
        major('set',o-set1+1:1,' ',p:1);
ifdef('HEX_CHAR_CODE')
        print('(');
        print_hex_number(p);
        print(')');
endif('HEX_CHAR_CODE')
      end;
      goto fin_set;
@z

@x
  put_rule: begin major('putrule'); goto fin_rule;
    end;
@y
  put_rule: begin major('putrule'); goto fin_rule;
    end;
  dir: begin major('dir ',p:1); dd:=p; goto done;
    end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  hstack[s]:=h; vstack[s]:=v; wstack[s]:=w;
@y
  ddstack[s]:=dd;
  hstack[s]:=h; vstack[s]:=v; wstack[s]:=w;
@z

@x
    h:=hstack[s]; v:=vstack[s]; w:=wstack[s];
@y
    dd:=ddstack[s];
    h:=hstack[s]; v:=vstack[s]; w:=wstack[s];
@z

@x
@d out_space(#)==if (p>=font_space[cur_font])or(p<=-4*font_space[cur_font]) then
    begin out_text(" "); hh:=pixel_round(h+p);
    end
  else hh:=hh+pixel_round(p);
@y
@d out_space(#)==if (p>=font_space[cur_font])or(p<=-4*font_space[cur_font]) then
    begin out_text(" ");
      if dd=0 then hh:=pixel_round(h+p) else vv:=pixel_round(v+p);
    end
  else if dd=0 then hh:=hh+pixel_round(p) else vv:=vv+pixel_round(p);
@z

@x
@d out_vmove(#)==if abs(p)>=5*font_space[cur_font] then vv:=pixel_round(v+p)
  else vv:=vv+pixel_round(p);
@y
@d out_vmove(#)==if abs(p)>=5*font_space[cur_font] then
     begin if dd=0 then vv:=pixel_round(v+p) else  hh:=pixel_round(h-p) end
  else if dd=0 then vv:=vv+pixel_round(p) else hh:=hh-pixel_round(p);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Hexadecimal code
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Translate a |set_char|...@>=
begin if (o>" ")and(o<="~") then
  begin out_text(p); minor('setchar',p:1);
  end
else major('setchar',p:1);
@y
@ @<Translate a |set_char|...@>=
begin if (o>" ")and(o<="~") then
  begin out_text(p); minor('setchar',p:1);
  end
else major('setchar',p:1);
ifdef('HEX_CHAR_CODE')
  print(' (');
  print_hex_number(p);
  print(')');
endif('HEX_CHAR_CODE')
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
else hh:=hh+char_pixel_width(cur_font)(p);
@y
else if dd=0 then hh:=hh+char_pixel_width(cur_font)(p)
     else vv:=vv+char_pixel_width(cur_font)(p);
@z

@x
hh:=hh+rule_pixels(q); goto move_right
@y
if dd=0 then hh:=hh+rule_pixels(q) else vv:=vv+rule_pixels(q);
goto move_right
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Finish a command that sets |h:=h+q|, then |goto done|@>=
@y
@<Finish a command that sets |h:=h+q|, then |goto done|@>=
if dd=0 then begin
@z

@x
goto done
@y
goto done end
else begin
if (v>0)and(q>0) then if v>infinity-q then
  begin error('arithmetic overflow! parameter changed from ',
@.arithmetic overflow...@>
    q:1,' to ',infinity-q:1);
  q:=infinity-v;
  end;
if (v<0)and(q<0) then if -v>q+infinity then
  begin error('arithmetic overflow! parameter changed from ',
    q:1, ' to ',(-v)-infinity:1);
  q:=(-v)-infinity;
  end;
hhh:=pixel_round(v+q);
if abs(hhh-vv)>max_drift then
  if hhh>vv then vv:=hhh-max_drift
  else vv:=hhh+max_drift;
if showing then if out_mode>mnemonics_only then
  begin print(' v:=',v:1);
  if q>=0 then print('+');
  print(q:1,'=',v+q:1,', vv:=',vv:1);
  end;
v:=v+q;
if abs(v)>max_v_so_far then
  begin if abs(v)>max_v+99 then
    begin error('warning: |v|>',max_v:1,'!');
@.warning: |v|...@>
    max_v:=abs(v);
    end;
  max_v_so_far:=abs(v);
  end;
goto done
end
@z

@x
@ @<Finish a command that sets |v:=v+p|, then |goto done|@>=
@y
@ @<Finish a command that sets |v:=v+p|, then |goto done|@>=
if dd=0 then begin
@z

@x
goto done
@y
goto done end
else begin
p:=-p;
if (h>0)and(p>0) then if h>infinity-p then
  begin error('arithmetic overflow! parameter changed from ',
@.arithmetic overflow...@>
    p:1,' to ',infinity-h:1);
  p:=infinity-h;
  end;
if (h<0)and(p<0) then if -h>p+infinity then
  begin error('arithmetic overflow! parameter changed from ',
    p:1, ' to ',(-h)-infinity:1);
  p:=(-h)-infinity;
  end;
vvv:=pixel_round(h+p);
if abs(vvv-hh)>max_drift then
  if vvv>hh then hh:=vvv-max_drift
  else hh:=vvv+max_drift;
if showing then if out_mode>mnemonics_only then
  begin print(' h:=',h:1);
  if p>=0 then print('+');
  print(p:1,'=',h+p:1,', hh:=',hh:1);
  end;
h:=h+p;
if abs(h)>max_h_so_far then
  begin if abs(h)>max_h+99 then
    begin error('warning: |h|>',max_h:1,'!');
@.warning: |h|...@>
    max_h:=abs(h);
    end;
  max_h_so_far:=abs(h);
  end;
goto done
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% pTeX
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  print('level ',ss:1,':(h=',h:1,',v=',v:1,
    ',w=',w:1,',x=',x:1,',y=',y:1,',z=',z:1,
    ',hh=',hh:1,',vv=',vv:1,')');
@y
  begin
    print('level ',ss:1,':(h=',h:1,',v=',v:1,
      ',w=',w:1,',x=',x:1,',y=',y:1,',z=',z:1);
    if ptex_p then print(',d=',dd:1);
    print(',hh=',hh:1,',vv=',vv:1,')');
  end;
@z

@x
if k<>id_byte then bad_dvi('ID byte is ',k:1);
@y
ptex_p:=(k=ptex_id_byte);
if (k<>id_byte) and (not ptex_p) then bad_dvi('ID byte is ',k:1);
@z

@x
print_ln(', maxstackdepth=',max_s:1,', totalpages=',total_pages:1);
@y
print_ln(', maxstackdepth=',max_s:1,', totalpages=',total_pages:1);
if ptex_p then print_ln('pTeX DVI (id=',ptex_id_byte:1,')');
@z

@x
if m<>id_byte then print_ln('identification in byte ',cur_loc-1:1,
@.identification...should be n@>
    ' should be ',id_byte:1,'!');
@y
if (m<>id_byte) and (m<>ptex_id_byte) then
  print_ln('identification in byte ',cur_loc-1:1,
@.identification...should be n@>
    ' should be ',id_byte:1,' or ',ptex_id_byte:1,'!');
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [106] Check usage; print newline at end of program; remove unused
% label.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
dialog; {set up all the options}
@y
if argc <> 2
then begin
  write_ln ('Usage: pdvitype <dvi file>.');
  jump_out;
end;
dialog; {set up all the options}
@z

@x
final_end:end.
@y
print_ln(' '); end.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [109] Fix another floating point print.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
print_ln('magnification=',mag:1,'; ',conv:16:8,' pixels per DVI unit')
@y
print ('magnification=', mag:1, '; ');
print_real (conv, 16, 8);
print_ln (' pixels per DVI unit')
@z
