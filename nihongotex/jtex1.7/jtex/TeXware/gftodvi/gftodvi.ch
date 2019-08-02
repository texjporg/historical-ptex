% gftodvi.ch for C compilation using web2c
%
% Hisato Hamano, 4/18/89 for version 1.7
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [0] WEAVE: print changes only
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
\pageno=\contentspagenumber \advance\pageno by 1
@y
\pageno=\contentspagenumber \advance\pageno by 1
\let\maybe=\iffalse
\def\title{GF$\,$\lowercase{to}$\,$PK changes C}
@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [1] Change banner string
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d banner=='This is GFtoDVI, Version 1.7' {printed when the program starts}
@y
@d banner=='This is GFtoDVI, C Version 1.7' {printed when the program starts}
@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [4] Redefine program header, eliminate output
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d print(#)==write(#)
@d print_ln(#)==write_ln(#)
@d print_nl(#)==begin write_ln; write(#);
  end

@p program GF_to_DVI(@!output);
label @<Labels in the outer block@>@/
const @<Constants in the outer block@>@/
type @<Types in the outer block@>@/
var @<Globals in the outer block@>@/
procedure initialize; {this procedure gets things started properly}
  var @!i,@!j,@!m,@!n:integer; {loop indices for initializations}
  begin print_ln(banner);@/
  @<Set initial values@>@/
  end;
@y
@d term_out==stdout {standard output}
@d print(#)==write(term_out, #)
@d print_ln(#)==write_ln(term_out, #)
@d print_nl(#)==begin write_ln(term_out); write(term_out, #);
  end

@p program GF_to_DVI;
const @<Constants in the outer block@>@/
type @<Types in the outer block@>@/
var @<Globals in the outer block@>@/
procedure initialize; {this procedure gets things started properly}
  var @!i,@!j,@!m,@!n:integer; {loop indices for initializations}
  begin
  setpaths;@/
  @<Set initial values@>@/
  end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [4] Eliminate the |final_end| label
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ If the program has to stop prematurely, it goes to the
`|final_end|'.

@d final_end=9999 {label for the end of it all}

@<Labels...@>=final_end;
@y
@ this module is deleted, because it is only useful for
a non-local goto, which we don't use in C.
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [6] add file_name_size to constants in the outer block
%     and remove terminal_line_length since there is no dialog
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!file_name_size=50; {a file name shouldn't be longer than this}
@y
@!file_name_size=1024; {a file name shouldn't be longer than this}
{If it is less than FILENAMESIZE in site.h, C may zero neighboring arrays}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [8] have abort() add <nl> to end of msg and eliminate non-local goto
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d abort(#)==begin print(' ',#); jump_out;
    end
@d bad_gf(#)==abort('Bad GF file: ',#,'! (at byte ',cur_loc-1:1,')')
@.Bad GF file@>

@p procedure jump_out;
begin goto final_end;
end;
@y
@d abort(#)==
     begin print_ln(' ',#); uexit(1); end
@d bad_gf(#)==abort('Bad GF file: ',#,'! (at byte ',cur_loc-1:1,')')
@.Bad GF file@>
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [15]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d update_terminal == break(output) {empty the terminal output buffer}

@<Glob...@>=
@!buffer:array[0..terminal_line_length] of 0..255;
@!term_in:text_file; {the terminal, considered as an input file}
@y
@d update_terminal == flush(term_out) {empty the terminal output buffer}
@d term_in == stdin {the terminal, considered as an input file}

@<Glob...@>=
@!buffer:array[0..terminal_line_length] of 0..255;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [16] input_ln
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure input_ln; {inputs a line from the terminal}
begin update_terminal; reset(term_in);
if eoln(term_in) then read_ln(term_in);
line_length:=0;
while (line_length<terminal_line_length)and not eoln(term_in) do
  begin buffer[line_length]:=xord[term_in^]; incr(line_length); get(term_in);
  end;
end;
@y
@p procedure input_ln; {inputs a line from the terminal}
begin
  update_terminal;
  if eoln(term_in) then read_ln(term_in);
  line_length:=0;
  while (line_length<terminal_line_length)and not eoln(term_in) do begin
    buffer[line_length]:=xord[getc(term_in)];
    incr(line_length);
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [45] Add UNIX_file_name type.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!eight_bits=0..255; {unsigned one-byte quantity}
@!byte_file=packed file of eight_bits; {files that contain binary data}
@y
@!eight_bits=0..255; {unsigned one-byte quantity}
@!byte_file=packed file of eight_bits; {files that contain binary data}
@!UNIX_file_name=packed array [1..file_name_size] of text_char;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [46] add gf_name, tfm_name and real_name_of_file
% 	global vars; also a boolean, gf_file_exists, tfm_file_exists
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!gf_file:byte_file; {the character data we are reading}
@!dvi_file:byte_file; {the typesetting instructions we are writing}
@!tfm_file:byte_file; {a font metric file}
@y
@!gf_file:byte_file; {the character data we are reading}
@!dvi_file:byte_file; {the typesetting instructions we are writing}
@!tfm_file:byte_file; {a font metric file}
@!gf_name,@!tfm_name,uarg: UNIX_file_name;
	{names of input and output files; pascal-style origin from one}
@!real_name_of_file:packed array[0..file_name_size] of text_char;
	{C style origin from zero}
gf_file_exists,tfm_file_exists : boolean;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [47] redo open_gf_file
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ To prepare these files for input or output, we |reset| or |rewrite|
them. An extension of \PASCAL\ is needed, since we want to associate
it with external files whose names are specified dynamically (i.e., not
known at compile time). The following code assumes that `|reset(f,s)|' and
`|rewrite(f,s)|' do this, when |f| is a file variable and |s| is a string
variable that specifies the file name.
@^system dependencies@>

@p procedure open_gf_file; {prepares to read packed bytes in |gf_file|}
begin reset(gf_file,name_of_file);
cur_loc:=0;
end;
@y
@ In C, we use the external |test_access| procedure, which also does path
searching based on the user's environment or the default path.

@d read_access_mode=4  {``read'' mode for |test_access|}
@d write_access_mode=2 {``write'' mode for |test_access|}

@d no_file_path=0    {no path searching should be done}
@d tex_font_file_path=3  {path specifier for \.{TFM} files}
@d generic_font_file_path=4  {path specifier for \.{GF} files}
@d packed_font_file_path=5  {path specifier for \.{PK} files}

@p procedure open_gf_file; {prepares to read packed bytes in |gf_file|}
var j:integer;
begin
  gf_file_exists:=test_access(read_access_mode,generic_font_file_path);
  if gf_file_exists then begin
    for j:=1 to file_name_size do
      gf_name[j]:=real_name_of_file[j-1];
    reset(gf_file,gf_name)
  end else
    abort('GF file not found');
  cur_loc:=0;
end;
@z

@x
procedure open_tfm_file; {prepares to read packed bytes in |tfm_file|}
begin reset(tfm_file,name_of_file);
end;
@y
procedure open_tfm_file; {prepares to read packed bytes in |tfm_file|}
var j:integer;
begin
  tfm_file_exists := test_access(read_access_mode,tex_font_file_path);
  if tfm_file_exists then begin
    for j:=1 to file_name_size do
      tfm_name[j]:=real_name_of_file[j-1];
    reset(tfm_file, tfm_name)
  end else
    abort('TFM file not found');
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [49] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@!b0,@!b1,@!b2,@!b3: eight_bits; {four bytes input at once}
@y
@!q0,@!q1,@!q2,@!q3: eight_bits; {four bytes input at once}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [50] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure read_tfm_word;
begin read(tfm_file,b0); read(tfm_file,b1);
read(tfm_file,b2); read(tfm_file,b3);
end;
@y
@p procedure read_tfm_word;
begin
  read(tfm_file,q0); read(tfm_file,q1);
  read(tfm_file,q2); read(tfm_file,q3);
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [52]
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d logo_font=5

@<Types ...@>=
@!quarterword = min_quarterword..max_quarterword; {1/4 of a word}
@!four_quarters = packed record@;@/
  @!b0:quarterword;
  @!b1:quarterword;
  @!b2:quarterword;
  @!b3:quarterword;
  end;
@!memory_word = record@;@/
  case boolean of
  true: (@!sc:scaled);
  false: (@!qqqq:four_quarters);
  end;
@y
@d logo_font=5
@d sc == cint

@<Types ...@>=
@!quarterword=min_quarterword..max_quarterword;
@=#include "memory.h";@>
@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [60] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @d read_two_halves_end(#)==#:=b2*256+b3
@d read_two_halves(#)==read_tfm_word; #:=b0*256+b1; read_two_halves_end
@y
@ @d read_two_halves_end(#)==#:=q2*256+q3
@d read_two_halves(#)==read_tfm_word; #:=q0*256+q1; read_two_halves_end
@z
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [62] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d store_four_quarters(#)==
  begin read_tfm_word;
  qw.b0:=qi(b0); qw.b1:=qi(b1); qw.b2:=qi(b2); qw.b3:=qi(b3);
  #:=qw;
  end
@y
@d store_four_quarters(#)==
  begin
    read_tfm_word;
    qw.b0:=qi(q0); qw.b1:=qi(q1); qw.b2:=qi(q2); qw.b3:=qi(q3);
    #:=qw;
  end
@z

@x
if b0>127 then abend; {design size must be positive}
z:=((b0*256+b1)*256+b2)*16+(b3 div 16);
@y
if q0>127 then abend; {design size must be positive}
z:=((q0*256+q1)*256+q2)*16+(q3 div 16);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [63] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  if (b0>=nw)or(b1 div @'20>=nh)or(b1 mod @'20>=nd)or
    (b2 div 4>=ni) then abend;
  case b2 mod 4 of
  lig_tag: if b3>=nl then abend;
  ext_tag: if b3>=ne then abend;
@y
  if (q0>=nw)or(q1 div @'20>=nh)or(q1 mod @'20>=nd)or
    (q2 div 4>=ni) then abend;
  case q2 mod 4 of
  lig_tag: if q3>=nl then abend;
  ext_tag: if q3>=ne then abend;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [64] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@d store_scaled(#)==begin read_tfm_word;
  sw:=(((((b3*z)div@'400)+(b2*z))div@'400)+(b1*z))div beta;
  if b0=0 then #:=sw@+else if b0=255 then #:=sw-alpha@+else abend;
  end
@y
@d store_scaled(#)==
  begin
    read_tfm_word;
    sw:=(((((q3*z)div@'400)+(q2*z))div@'400)+(q1*z))div beta;
    if q0=0 then #:=sw
    else if q0=255 then #:=sw-alpha
    else abend;
  end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [66] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  check_byte_range(b1);
  if b2<128 then check_byte_range(b3) {check ligature}
  else if b3>=nk then abend; {check kern}
  end;
if (nl>0)and(b0<128) then abend; {check for stop bit on last command}
@y
  check_byte_range(q1);
  if q2<128 then check_byte_range(q3) {check ligature}
  else if q3>=nk then abend; {check kern}
end;
if (nl>0)and(q0<128) then abend; {check for stop bit on last command}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [67] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
  if b0<>0 then check_byte_range(b0);
  if b1<>0 then check_byte_range(b1);
  if b2<>0 then check_byte_range(b2);
  check_byte_range(b3);
@y
  if q0<>0 then check_byte_range(q0);
  if q1<>0 then check_byte_range(q1);
  if q2<>0 then check_byte_range(q2);
  check_byte_range(q3);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [68] b0..b3 to q0..q3
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
    if b0>127 then sw:=b0-256@+else sw:=b0;
    sw:=sw*@'400+b1; sw:=sw*@'400+b2;
    font_info[param_base[f]].sc:=(sw*@'20)+(b3 div@'20);
    end
@y
    if q0>127 then sw:=q0-256@+else sw:=q0;
    sw:=sw*@'400+q1; sw:=sw*@'400+q2;
    font_info[param_base[f]].sc:=(sw*@'20)+(q3 div@'20);
    end
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [89] begin_name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure begin_name;
begin area_delimiter:=0; ext_delimiter:=0;
end;
@y
@p
procedure begin_name;
begin
  area_delimiter:=0; ext_delimiter:=0;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [90] more_name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function more_name(@!c:ASCII_code):boolean;
begin if c=" " then more_name:=false
else  begin if (c=">")or(c=":") then
    begin area_delimiter:=pool_ptr; ext_delimiter:=0;
    end
  else if (c=".")and(ext_delimiter=0) then ext_delimiter:=pool_ptr;
  str_room(1); append_char(c); {contribute |c| to the current string}
  more_name:=true;
  end;
end;
@y
@p function more_name(@!c:ASCII_code):boolean;
begin
  if c=" " then more_name:=false
  else begin
    if c="/" then begin
      area_delimiter:=pool_ptr; ext_delimiter:=0;
    end else if c="." then
      ext_delimiter:=pool_ptr;
    str_room(1); append_char(c); {contribute |c| to the current string}
    more_name:=true;
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [91] end_name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure end_name;
begin if str_ptr+3>max_strings then
  abort('Too many strings!');
@.Too many strings@>
if area_delimiter=0 then cur_area:=null_string
else  begin cur_area:=str_ptr; incr(str_ptr);
  str_start[str_ptr]:=area_delimiter+1;
  end;
if ext_delimiter=0 then
  begin cur_ext:=null_string; cur_name:=make_string;
  end
else  begin cur_name:=str_ptr; incr(str_ptr);
  str_start[str_ptr]:=ext_delimiter; cur_ext:=make_string;
  end;
end;
@y
@p
procedure end_name;
begin
  if str_ptr+3>max_strings then
    abort('Too many strings!');
@.Too many strings@>
  if area_delimiter=0 then
    cur_area:=null_string
  else begin
    cur_area:=str_ptr; incr(str_ptr);
    str_start[str_ptr]:=area_delimiter+1;
  end;
  if ext_delimiter=0 then begin
    cur_ext:=null_string; cur_name:=make_string;
  end else begin
    cur_name:=str_ptr; incr(str_ptr);
    str_start[str_ptr]:=ext_delimiter;
    cur_ext:=make_string;
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [92] pack_file_name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure pack_file_name(@!n,@!a,@!e:str_number);
var k:integer; {number of positions filled in |name_of_file|}
@!c: ASCII_code; {character being packed}
@!j:integer; {index into |str_pool|}
@!name_length:0..file_name_size; {number of characters packed}
begin k:=0;
for j:=str_start[a] to str_start[a+1]-1 do append_to_name(str_pool[j]);
for j:=str_start[n] to str_start[n+1]-1 do append_to_name(str_pool[j]);
for j:=str_start[e] to str_start[e+1]-1 do append_to_name(str_pool[j]);
if k<=file_name_size then name_length:=k@+else name_length:=file_name_size;
for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
end;
@y
@p procedure pack_file_name(@!n,@!a,@!e:str_number);
var k:integer; {number of positions filled in |name_of_file|}
  @!c: ASCII_code; {character being packed}
  @!j:integer; {index into |str_pool|}
  @!name_length:0..file_name_size; {number of characters packed}
begin
  k:=0;
  for j:=str_start[a] to str_start[a+1]-1 do append_to_name(str_pool[j]);
  for j:=str_start[n] to str_start[n+1]-1 do append_to_name(str_pool[j]);
  for j:=str_start[e] to str_start[e+1]-1 do append_to_name(str_pool[j]);
  if k<=file_name_size then name_length:=k@+else name_length:=file_name_size;
  for k:=name_length+1 to file_name_size do name_of_file[k]:=' ';
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [94] setup file_name
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure start_gf;
label found,done;
begin loop@+begin print_nl('GF file name: '); input_ln;
@.GF file name@>
  buf_ptr:=0; buffer[line_length]:="?";
  while buffer[buf_ptr]=" " do incr(buf_ptr);
  if buf_ptr<line_length then
    begin @<Scan the file name in the buffer@>;
    if cur_ext=null_string then cur_ext:=gf_ext;
    pack_file_name(cur_name,cur_area,cur_ext); open_gf_file;
    if not eof(gf_file) then goto found;
    print_nl('Oops... I can''t find file '); print(name_of_file);
@.Oops...@>
@.I can't find...@>
    end;
  end;
found:job_name:=cur_name; pack_file_name(job_name,null_string,dvi_ext);
open_dvi_file;
end;
@y
@p
procedure start_gf;
label found,done;
var i:integer;
begin
  uarg[1]:=xchr[" "];
  if argc>2 then abort('Usage: gftodvi [gf_file]');
  if argc=2 then argv(1, uarg);
  line_length:=0;
  while (line_length<terminal_line_length)
        and(uarg[line_length]<>xchr[" "]) do begin
    buffer[line_length]:=xord[uarg[line_length+1]];
    incr(line_length);
  end;
  print_ln(banner);@/
  loop@+begin
    buf_ptr:=0; buffer[line_length]:="?";
    while buffer[buf_ptr]=" " do incr(buf_ptr);
    if buf_ptr<line_length then begin
      @<Scan the file name in the buffer@>;
      if cur_ext=null_string then cur_ext:=gf_ext;
      pack_file_name(cur_name,cur_area,cur_ext); open_gf_file;
      if not eof(gf_file) then goto found;
      print_nl('Oops... I can''t find file '); print(name_of_file);
@.Oops...@>
@.I can't find...@>
    end;
    print_nl('GF file name: '); input_ln;
@.GF file name@>
  end;
found:
  job_name:=cur_name; pack_file_name(job_name,null_string,dvi_ext);
  open_dvi_file;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [94] scan filename
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@ @<Scan the file name in the buffer@>=
if buffer[line_length-1]="/" then
  begin interaction:=true; decr(line_length);
  end;
begin_name;
loop@+  begin if buf_ptr=line_length then goto done;
  if not more_name(buffer[buf_ptr]) then goto done;
  incr(buf_ptr);
  end;
done:end_name
@y
@ @<Scan the file name in the buffer@>=
  if buffer[line_length-1]="/" then begin
    interaction:=true; decr(line_length);
  end;
  begin_name;
  loop@+ begin
    if buf_ptr=line_length then goto done;
    if not more_name(buffer[buf_ptr]) then goto done;
    incr(buf_ptr);
  end;
done:
  end_name
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [98] font area
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
 if (f<>slant_font)or(length(font_name[f])>0) then
  begin if length(font_area[f])=0 then font_area[f]:=home_font_area;
  pack_file_name(font_name[f],font_area[f],tfm_ext);
@y
  if (f<>slant_font)or(length(font_name[f])>0) then begin
    pack_file_name(font_name[f],font_area[f],tfm_ext);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [107] write_dvi
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure write_dvi(@!a,@!b:dvi_index);
var k:dvi_index;
begin for k:=a to b do write(dvi_file,dvi_buf[k]);
end;
@y
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [115] final_end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@<Empty the last bytes out of |dvi_buf|@>;
goto final_end;
@y
@<Empty the last bytes out of |dvi_buf|@>;
  print_ln(' ');
  uexit(2);
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [135] print real
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure slant_complaint(@!r:real);
begin if abs(r-slant_reported)>0.001 then
  begin print_nl('Sorry, I can''t make diagonal rules of slant ',r:10:5,'!');
@.Sorry, I can't...@>
  slant_reported:=r;
  end;
end;
@y
@p
procedure slant_complaint(@!r:real);
begin
  if abs(r-slant_reported)>0.001 then begin
    print_nl('Sorry, I can''t make diagonal rules of slant ');
    printreal(r,10,5);
    print('!');
@.Sorry, I can't...@>
    slant_reported:=r;
  end;
end;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [141] WEBtoC : forward bug
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function@?even_overlap(@!p:tree_pointer):boolean; forward;@t\2@>@/
function@?odd_overlap(@!p:tree_pointer):boolean; forward;@t\2@>@/
@y
@p
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [143] WEBtoC : forward bug
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function even_overlap; {this was declared |forward| above}
@y
@p function even_overlap_body(@!p:tree_pointer):boolean;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [144] WEBtoC : forward bug
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p function odd_overlap; {this was declared |forward| above}
@y
@p function odd_overlap_body(@!p:tree_pointer):boolean;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [147] max_int
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure nearest_dot;
@y
@d max_int==536870911

@p procedure nearest_dot;
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [150] WEBtoC : forward bug
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure@?even_n_l_dot(@!p:tree_pointer); forward;@t\2@>@/
procedure@?odd_n_l_dot(@!p:tree_pointer); forward;@t\2@>@/
@y
@p
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [152] WEBtoC : forward bug
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
@p procedure even_n_l_dot; {declared |forward| above}
@y
@p
procedure even_n_l_dot_body(@!p:tree_pointer); {declared |forward| above}
@z

@x
procedure odd_n_l_dot; {declared |forward| above}
@y
procedure odd_n_l_dot_body(@!p:tree_pointer); {declared |forward| above}
@z

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [217] final_end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
@x
final_end:end.
@y
end.
@z
