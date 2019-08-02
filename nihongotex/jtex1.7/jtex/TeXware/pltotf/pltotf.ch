%  June/89 Hamano Hisato, ASCII:
%		Marge Kanji and C Version
%		Handle TATE-kumi tfm files
%  May/87 Greg McFarlane, ASCII:
%		handles kanji format tfm files
%               (changes indicated by KANJI:)

%   Change file for the PLtoTF processor, for use with WEB to C.
%   This file was created by Pavel Curtis, Pavel@Cornell.

% History:
%   4/4/83 (PC) Original version, made to work with version 1.2 of PLtoTF.
%   4/16   (PC) Brought up to version 1.3 of PLtoTF.
%   6/30  (HWT) Revised changefile format for version 1.7 Tangle
%   7/28  (HWT) Brought up to version 2
%  12/19  (ETM) Brought up to version 2.1
%  7/5/87 (ETM) Brought up to version 2.3
% 3/22/88 (ETM) Converted for use with WEB to C

% The module numbers used in this file refer to those in the red-covered
% listing (Version 2) (same numbers as in blue-covered).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [0] WEAVE: print changes only
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
\let\maybe=\iffalse
\def\title{PLTOTF changes for C}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1] Change banner string
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
@d banner=='This is PLtoTF, Version 2.3' {printed when the program starts}
@y
@d banner=='This is PLtoTF, C Version j2.3'
					{printed when the program starts}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [2] Fix filenames in program statement; add `final_end' label
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p program PLtoTF(@!pl_file,@!tfm_file,@!output);
@y
@p program PLtoTF;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [6] Open PL file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
reset(pl_file);
@y
if argc < 3 then begin
    print_ln('Usage: pltotf <pl-file> <tfm-file>');
    uexit(1);
end;
argv(1, pl_name);
reset(pl_file, pl_name);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [15] Change type of tfm_file and declare extra TFM-file variables
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!tfm_file:packed file of 0..255;
@y
@!tfm_file            : packed file of 0..255;
@!tfm_name, 
@!pl_name             : packed array[1..100] of char;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [16] Open TFM file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ On some systems you may have to do something special to write a
packed file of bytes. For example, the following code didn't work
when it was first tried at Stanford, because packed files have to be
opened with a special switch setting on the \PASCAL\ that was used.
@^system dependencies@>

@<Set init...@>=
rewrite(tfm_file);
@y
@ On some systems you may have to do something special to write a
packed file of bytes with Pascal.  It's no problem in C.
@^system dependencies@>

@<Set init...@>=
argv(2, tfm_name);
rewrite(tfm_file, tfm_name);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [28] This fixes a bug in the original. If get_byte is reading a
%      number at the end of a line and the next line has a number
%      at the beginning (possibly preceded by some spaces!!) these
%      two numbers are run together.
%      This bug may be found in other routines so...
%      Fix: add some (more?) space at the end of each line, in fill_buffer.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
  buffer[limit+1]:=' '; right_ln:=eoln(pl_file);
@y
  buffer[limit+1]:=' '; right_ln:=eoln(pl_file);
  if right_ln then begin
    incr(limit);
    buffer[limit+1]:=' ';
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [36] May have to increase some numbers to fit new commands
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
@d max_name_index=66 {upper bound on the number of keywords}
@d max_letters=500 {upper bound on the total length of all keywords}
@y
@d max_name_index=75 {upper bound on the number of keywords}
@d max_letters=600 {upper bound on the total length of all keywords}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [44] Add kanji related codes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
@d character_code=11
@y
@d type_code = 11
@d glue_kern_code = 12
@d chars_in_type_code = 13
@d dir_code=14
@d character_code=15
@z

@x
@d stop_code=73
@y
@d glue_code = 73
@d stop_code=74

@d undefined = 0
@d tfm_format = 1
@d jfm_format = 2
@d vfm_format = 3
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47] Add kanji related names into hash table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
load4("S")("T")("O")("P")(stop_code);
@y
load4("S")("T")("O")("P")(stop_code);
load8("G")("L")("U")("E")("K")("E")("R")("N")(glue_kern_code);
load4("G")("L")("U")("E")(glue_code);
load11("C")("H")("A")("R")("S")("I")("N")("T")("Y")("P")("E")
  (chars_in_type_code);
load4("T")("Y")("P")("E")(type_code);
load9("D")("I")("R")("E")("C")("T")("I")("O")("N")(dir_code);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [48] parameter names for kanji-font
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
load10("E")("X")("T")("R")("A")("S")("P")("A")("C")("E")(parameter_code+7);@/
load4("N")("U")("M")("1")(parameter_code+8);@/
@y
load10("E")("X")("T")("R")("A")("S")("P")("A")("C")("E")(parameter_code+7);@/
load12("E")("X")("T")("R")("A")("S")("T")("R")("E")("T")("C")("H")
  (parameter_code+8);@/
load11("E")("X")("T")("R")("A")("S")("H")("R")("I")("N")("K")
  (parameter_code+9);@/
load4("N")("U")("M")("1")(parameter_code+8);@/
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49] Are we dealing with a kanji format file or a normal format file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
cur_code:=equiv[name_ptr];
@y
cur_code:=equiv[name_ptr];
if file_format=undefined then
  if (cur_code=glue_kern_code)or(cur_code = type_code)
      or(cur_code=chars_in_type_code) then
    file_format:=jfm_format
  else if (cur_code=lig_table_code)or(cur_code=character_code) then
    file_format:=tfm_format;
case file_format of
jfm_format, vfm_format:
  if (cur_code = lig_table_code)or(cur_code = character_code) then begin
    err_print('This is an illegal command for kanji format files');
    cur_code := comment_code;
  end;
tfm_format:
  if (cur_code=glue_kern_code)or(cur_code=type_code)
         or(cur_code = chars_in_type_code) then begin
    err_print('This is an illegal command for normal format files');
    cur_code := comment_code;
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [85]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
character_code: read_char_info;
@y
character_code: read_char_info;
type_code: read_char_info;
glue_kern_code: read_glue_kern;
chars_in_type_code: read_chars_in_type;
dir_code: @<Read direction@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [103] next_larger and var_char codes are illegal in kanji format files
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
  next_larger_code:begin check_tag(c); char_tag[c]:=list_tag;
    char_remainder[c]:=get_byte;
    end;
  var_char_code:@<Read an extensible recipe for |c|@>;
@y
  next_larger_code:
    if file_format=tfm_format then begin
      check_tag(c);
      char_tag[c]:=list_tag;
      char_remainder[c]:=get_byte;
    end else
      err_print('NEXTLARGER is illegal in kanji format files');
  var_char_code:
    if file_format=tfm_format then
      @<Read an extensible recipe for |c|@>
    else
      err_print('VARVHAR is illegal in kanji format files');
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [109] there are no charlists in kanji format files
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
@<Doublecheck the lig/kern commands and the extensible recipes@>;
for c:=0 to 255 do
  @<Make sure that |c| is not the largest element of a charlist cycle@>;
@y
@<Doublecheck the lig/kern commands and the extensible recipes@>;
if file_format=tfm_format then
  for c:=0 to 255 do
    @<Make sure that |c| is not the largest element of a charlist cycle@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [110] change error message if gluekern table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
  print_ln('Last LIGTABLE LABEL was not used.');
@y
  if file_format=tfm_format then
    print_ln('Last LIGTABLE LABEL was not used.')
  else
    print_ln('Last GLUEKERN LABEL was not used.');
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [113] when checking glue_kern prog check glues as well
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
  begin check_existence(lig_kern[lig_ptr].b1)('LIG character generated by');
  check_existence(lig_kern[lig_ptr].b3)('LIG character generated by');
@y
  begin
  if file_format=tfm_format then begin
    check_existence(lig_kern[lig_ptr].b1)('LIG character generated by');
    check_existence(lig_kern[lig_ptr].b3)('LIG character generated by');
  end else
    check_existence(lig_kern[lig_ptr].b1)('GLUE character generated by')
@z

@x
if nl>0 then for lig_ptr:=0 to nl-1 do
  if lig_kern[lig_ptr].b2<kern_flag then
    double_check_lig(b3)('LIG step')
  else double_check_lig(b1)('KRN step');
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [118] More floating point stuff
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @d round_message(#)==if delta>0 then print_ln('I had to round some ',
@.I had to round...@>
  #,'s by',(((delta+1) div 2)/@'4000000):1:7,' units.')
@y
@ @d round_message(#)==if delta>0 then begin print('I had to round some ',
@.I had to round...@>
  #,'s by'); print_real((((delta+1) div 2)/@'4000000),1,7);
  print_ln(' units.'); end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [119] Fix up output of bytes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d out(#)==write(tfm_file,#)
@y
@d out(#)==putbyte(#,tfm_file)
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [120] We also have to output the character type table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
@<Output the character info@>;
@y
if (file_format=jfm_format)or(file_format=vfm_format) then
  @<Output the character_type info@>;
@<Output the character info@>;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [122] correct lf computation and compute nt
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
lf:=6+lh+(ec-bc+1)+memory[width]+memory[height]+memory[depth]+
memory[italic]+nl+nk+ne+np;
@y
if file_format=tfm_format then
  lf:=6+lh+(ec-bc+1)+memory[width]+memory[height]+memory[depth]+
    memory[italic]+nl+nk+ne+np
else begin
  @<Compute nt@>;
  lf:=7+nt+lh+(ec-bc+1)+memory[width]+memory[height]+memory[depth]+
    memory[italic]+nl+nk+3*ng+np
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [123] correct output of 12(14) subfile sizes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
out_size(lf); out_size(lh); out_size(bc); out_size(ec);
out_size(memory[width]); out_size(memory[height]);
out_size(memory[depth]); out_size(memory[italic]);
out_size(nl); out_size(nk); out_size(ne); out_size(np);
@y
if file_format=tfm_format then begin
  out_size(lf); out_size(lh); out_size(bc); out_size(ec);
  out_size(memory[width]); out_size(memory[height]);
  out_size(memory[depth]); out_size(memory[italic]);
  out_size(nl); out_size(nk); out_size(ne); out_size(np);
end else begin
  if file_format=jfm_format then begin
    out_size(kanji_id_number);
  end else begin
    out_size(tate_id_number);
  end;
  out_size(nt);
  out_size(lf); out_size(lh); out_size(bc); out_size(ec);
  out_size(memory[width]); out_size(memory[height]);
  out_size(memory[depth]); out_size(memory[italic]);
  out_size(nl); out_size(nk);
  out_size(ng*3);
  out_size(np);
end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [127] Fix floating point stuff
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure out_scaled(x:fix_word); {outputs a scaled |fix_word|}
var z:real; {a number to output after conversion to fixed point}
@!n:byte; {the first byte after the sign}
@!m:0..65535; {the two least significant bytes}
begin if abs(x/design_units)>=16.0 then
  begin print_ln('The relative dimension',x/@'4000000:1:3,
    ' is too large.');
@.The relative dimension...@>
  print('  (Must be less than 16*designsize');
  if design_units<>unity then print(' =',design_units/@'200000:1:3,
      ' designunits');
@y
@p procedure out_scaled(x:fix_word); {outputs a scaled |fix_word|}
var z:real; {a number to output after conversion to fixed point}
  @!n:byte; {the first byte after the sign}
  @!m:0..65535; {the two least significant bytes}
begin
  if fabs(x/design_units)>=16.0 then begin
    print('The relative dimension');
    print_real(x/@'4000000,1,3);
    print_ln(' is too large.');
@.The relative dimension...@>
    print('  (Must be less than 16*designsize');
    if design_units<>unity then begin
      print(' =');
      print_real(design_units/@'200000,1,3);
      print(' designunits');
    end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [129] Also output glue table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
if nk>0 then for krn_ptr:=0 to nk-1 do out_scaled(kern[krn_ptr])
@y
if nk>0 then for krn_ptr:=0 to nk-1 do out_scaled(kern[krn_ptr]);
if (file_format=jfm_format)or(file_format=vfm_format) then
  if ng>0 then
    for krn_ptr:=0 to ng-1 do begin
      out_scaled(glue[3 * krn_ptr + 0]);
      out_scaled(glue[3 * krn_ptr + 1]);
      out_scaled(glue[3 * krn_ptr + 2]);
    end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [133] declare kanji related routines and package gluekern stuff
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
procedure read_lig_kern;
var krn_ptr:0..256; {an index into |kern|}
@!c:byte; {runs through all character codes}
begin @<Read ligature/kern list@>;
end;
@y
@<Declare kanji related routines@>

procedure read_lig_kern;
var krn_ptr:0..256; {an index into |kern|}
  @!c:byte; {runs through all character codes}
begin
  @<Read ligature/kern list@>;
end;

procedure read_glue_kern;
var krn_ptr:0..256; {an index into |kern|}
  @!c:byte; {runs through all character codes}
begin
  @<Read glue/kern list@>;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [134] Print newline at end of program
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
end.
@y
print_ln(' '); end.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [135] From here to end of file is kanji related stuff
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% KANJI:
@x
@* System-dependent changes.
This section should be replaced, if necessary, by changes to the program
that are necessary to make \.{PLtoTF} work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@^system dependencies@>
@y
@* System-dependent changes.
This section should be replaced, if necessary, by changes to the program
that are necessary to make \.{PLtoTF} work at a particular installation.
It is usually best to design your change file so that all changes to
previous sections preserve the section numbering; then everybody's version
will be consistent with the printed program. More extensive changes,
which introduce new sections, can be inserted here; then only the index
itself will get a new section number.
@^system dependencies@>

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [136]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Constants...@>=
max_kanji = 7237;
kanji_id_number = 11;
tate_id_number = 9;

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [137] Globals needed for kanji routines
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Glob...@>=
file_format : undefined..vfm_format; {the format of the input file}
kanji_type : array[0..max_kanji] of -1..256; {the type of every kanji char }
kanji_type_index : 0..max_kanji; { index into above }
nt : integer; {number of entries in character type table}
glue : array[0..768] of fix_word; {the distinct glue amounts}
ng : integer; {number of 3-word entries in glue table}

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [138] Initialisation needed for kanji routines
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Set init...@>=
file_format := undefined;
for kanji_type_index := 0 to max_kanji do
  kanji_type[kanji_type_index] := -1;
ng := 0;

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [139] define get_jis_code function to read codes from CHARS_IN_TYPE command
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@d next_non_blank(#) ==
begin
repeat
  # := get_next_raw;
until # <> ' ';
end

@<Declare kanji related routines@>=
function get_next_raw : char;
begin
while loc=limit do
  fill_buffer;
incr(loc);
get_next_raw := buffer[loc];
if isfbkanji(buffer[loc]) then
  cur_char := " "
else
  cur_char := xord[buffer[loc]];
end;

function get_jis_from_kanji(byte1 : char) : integer;
var
  byte2 : char;
  ms_code : integer;
begin
  byte2 := get_next_raw;
  if kanjiord(byte1) < 0 then
    ms_code := 256 + kanjiord(byte1)
  else
    ms_code := kanjiord(byte1);
  ms_code := ms_code * 256;
  if kanjiord(byte2) < 0 then
    ms_code := ms_code + 256 + kanjiord(byte2)
  else
    ms_code := ms_code + kanjiord(byte2);
  get_jis_from_kanji := mstojis(ms_code);
end;

function get_hex_code(ch : char) : integer;
var
  num : integer;
begin
num := 0;
while ((ch >= '0') and (ch <= '9')) or
      ((ch >= 'a') and (ch <= 'f')) or
      ((ch >= 'A') and (ch <= 'F')) do
  begin
  if ch >= 'a' then
    num := num * 16 + ord(ch) - ord('a') + 10
  else if ch >= 'A' then
    num := num * 16 + ord(ch) - ord('A') + 10
  else
    num := num * 16 + ord(ch) - ord('0');
  ch := get_next_raw;
  end;
decr(loc); {we've gone one char too far}
get_hex_code := num;
end;

function get_jis_code : integer;
var
  ch : char;
  tmp_jis_code : integer;
begin
next_non_blank(ch);
{
if isfbkanji(ch) then
  write_ln('get_jis_code: kanjiord(ch) = ', kanjiord(ch))
else
  write_ln('get_jis_code: ch = ', ch);
}
if ch = ')' then
  tmp_jis_code := 0
else if isfbkanji(ch) then
  tmp_jis_code := get_jis_from_kanji(ch)
else if (ch >= '0') and (ch <= '9') then
  tmp_jis_code := get_hex_code(ch)
else if (ch = 'c') or (ch = 'C') or (ch = 'k') or (ch = 'K') then
  begin
  next_non_blank(ch);
  if isfbkanji(ch) then
    tmp_jis_code := get_jis_from_kanji(ch)
  else if ch = ')' then
    tmp_jis_code := 0
  else
    tmp_jis_code := -1
  end
else if (ch = 'h') or (ch = 'H') then
  begin
  next_non_blank(ch);
  if (ch >= '0') and (ch <= '9') then
    tmp_jis_code := get_hex_code(ch)
  else if ch = ')' then
    tmp_jis_code := 0
  else
    tmp_jis_code := -1
  end
else
  tmp_jis_code := -1;
get_jis_code := tmp_jis_code;
end;

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [140] (in 133) similar to 94-101 but reads a glue kern program
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Read glue/kern list@>=
begin while level=1 do
  begin while cur_char=" " do get_next;
  if cur_char="(" then @<Read a glue/kern command@>
  else if cur_char=")" then skip_to_end_of_item
  else junk_error;
  end;
finish_inner_property_list;
end

@ @<Read a glue/kern command@>=
begin get_name;
if cur_code=comment_code then skip_to_end_of_item
else if (cur_code<label_code)or(cur_code>stop_code) then
  flush_error('This property name doesn''t belong in a GLUEKERN list')
@.This property name doesn't belong...@>
else  begin case cur_code of
  label_code:@<Read a glue label step@>;
  glue_code:@<Read a glue step@>;
  krn_code:@<Read a (glue) kerning step@>;
  stop_code:@<Read a stop step@>;
  end;@/
  finish_the_property;
  end;
end

@ When a character is about to be tagged, we use the following
so that an error message is given in case of multiple tags.

@d check_glue_tag(#) == {print error if |c| already tagged}
begin
case char_tag[#] of
  no_tag: do_nothing;
  lig_tag: err_print('This character already appeared in a GLUEKERN LABEL');
  @.This character already...@>
  list_tag: err_print('Impossible: a list tag in a kanji format file?');
  ext_tag: err_print('Impossible: an extensible tag in a kanji format file?');
  end;
end

@ @<Read a glue label step@>=
begin c:=get_byte;
check_glue_tag(c);
if nl>255 then
  err_print('GLUEKERN with more than 255 commands cannot have further labels')
@.GLUEKERN with more than 255...@>
else  begin char_tag[c]:=lig_tag; char_remainder[c]:=nl;
  unused_label:=true;
  end;
end

@ @<Read a glue step@>=
begin
lig_kern[nl].b0:=0;
lig_kern[nl].b1:=get_byte;
lig_kern[nl].b2:=0;
glue[3 * ng + 0]:=get_fix;
glue[3 * ng + 1]:=get_fix;
glue[3 * ng + 2]:=get_fix;
krn_ptr:=0;
while (glue[3 * krn_ptr + 0]<>glue[3 * ng + 0]) or
      (glue[3 * krn_ptr + 1]<>glue[3 * ng + 1]) or
      (glue[3 * krn_ptr + 2]<>glue[3 * ng + 2]) do
  incr(krn_ptr);
if krn_ptr=ng then
  begin
  if ng<256 then
    incr(ng)
  else
    begin
    err_print('At most 256 different glues are allowed');
    krn_ptr:=255;
    end;
  end;
lig_kern[nl].b3:=krn_ptr;
if nl=511 then
  err_print('GLUEKERN table should never exceed 511 GLUE/KRN commands')
@.GLUEKERN table should never...@>
else incr(nl);
unused_label:=false;
end

@ @<Read a (glue) kerning step@>=
begin
lig_kern[nl].b0:=0;
lig_kern[nl].b1:=get_byte;
lig_kern[nl].b2:=kern_flag;
kern[nk]:=get_fix; krn_ptr:=0;
while kern[krn_ptr]<>kern[nk] do incr(krn_ptr);
if krn_ptr=nk then
  begin if nk<256 then incr(nk)
  else  begin err_print('At most 256 different kerns are allowed');
@.At most 256 different kerns...@>
    krn_ptr:=255;
    end;
  end;
lig_kern[nl].b3:=krn_ptr;
if nl=511 then
  err_print('GLUEKERN table should never exceed 511 LIG/KRN commands')
@.GLUEKERN table should never...@>
else incr(nl);
unused_label:=false;
end

@ 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [141] Declare the kanji code conversion functions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
The kanji jis code is taken from the char_ext and char_code values
set by the user.
The index into the kanji_type array is based on the kuten codes,
with all unused codes removed and beginning at 0, not 0101.

@<Declare kanji related routines@>=
function valid_jis_code(jis : integer) : boolean;
var
  first_byte, second_byte : integer; { jis code bytes }
begin
  valid_jis_code := true;

  first_byte := jis div 256;
  if (first_byte < 33) or
     (first_byte > 40) and (first_byte < 48) or
     (first_byte > 116) then
    valid_jis_code := false;

  second_byte := jis mod 256;
  if (second_byte < 33) or
     (second_byte > 126) then
    valid_jis_code := false;
end;

function index_to_jis(kanji_index : integer) : integer;
begin
  if kanji_index <= 8 * 94 + 94 - 1 then
    index_to_jis := (kanji_index div 94 + 33) * 256 + (kanji_index mod 94 + 33)
  else
    index_to_jis := ((kanji_index + 7 * 94) div 94 + 33) * 256 +
      ((kanji_index + 7 * 94) mod 94 + 33)
end;

function jis_to_index(jis : integer) : integer;
var
  first_byte, second_byte : integer; { jis code bytes }
begin
  first_byte := jis div 256 - 33;
  second_byte := jis mod 256 - 33;
  if first_byte <= 8 then
    jis_to_index := first_byte * 94 + second_byte
  else
    jis_to_index := (first_byte - 7) * 94 + second_byte;
end;

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [142] read a list of kanji characters in a type
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Declare kanji related routines@>=
procedure read_chars_in_type;
var
  type_num : byte;
  jis_code : integer;
begin
type_num := get_byte;
if type_num = 0 then
  skip_error('You cannot list the chars in type 0. It is the default type')
else
  begin
  repeat
    jis_code := get_jis_code;
    if jis_code < 0 then
      err_print('Illegal characters. I was expecting a jis code or character')
    else if jis_code = 0 then { 0 signals end_of_list }
      do_nothing
    else if not valid_jis_code(jis_code) then
      err_print('jis code ', jis_code:1, ' is not valid')
    else if kanji_type[jis_to_index(jis_code)] >= 0 then
      err_print('jis code ', jis_code:1, ' is already in type ',
        kanji_type[jis_to_index(jis_code)])
    else
      kanji_type[jis_to_index(jis_code)] := type_num;
  until jis_code = 0;
  decr(loc); {get the ')' back for finish_the_property}
  end
end;

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [143] calculate the number of entries in character type table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Compute nt@>=
nt := 1; {we always have the default type 0}
for kanji_type_index := 0 to max_kanji do
  if kanji_type[kanji_type_index] > 0 then
    incr(nt)

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [144] Output char type table and advise user that it's a kanji file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Output the character_type info@>=
if file_format=jfm_format then
  print_ln('Input file is in kanji format.')
else
  print_ln('Input file is in TATE-kumi format.');
out_size(0); { the default }
out_size(0);
for kanji_type_index := 0 to max_kanji do
  if kanji_type[kanji_type_index] > 0 then
    begin
    {
    write_ln('char index = ', kanji_type_index,
      ' (jis ', index_to_jis(kanji_type_index),
      ') is type ', kanji_type[kanji_type_index]);
    }
    out_size(index_to_jis(kanji_type_index));
    out_size(kanji_type[kanji_type_index]);
    end

@
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [145] Read drection
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@<Read direction@>=
  begin
    while cur_char=" " do get_next;
    if cur_char="T" then begin
      if file_format=tfm_format then
        err_print('This is an illegal command for normal format files')
      else
        file_format:=vfm_format;
    end else if cur_char="Y" then begin
      if file_format=vfm_format then
        err_print('This is an illegal command for tetekumi format files');
    end else
      err_print('The dir value should be "TATE" or "YOKO"');
    skip_to_paren;
  end

@z
