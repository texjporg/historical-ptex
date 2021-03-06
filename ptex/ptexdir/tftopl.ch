% This is a change file for TFtoPL
%
% (07/18/2006) ST TFtoPL p1.7 (3.1, Web2c 7.2)
% (03/27/1998) KN TFtoPL p1.4 (3.1, Web2c 7.2)
%
@x [0] l.45 - pTeX:
\def\title{TF\lowercase{to}PL changes for C}
@y
\def\title{TF\lowercase{to}PL changes for C, and for KANJI}
@z

@x [2] l.64 - pTeX:
@d banner=='This is TFtoPL, Version 3.1' {printed when the program starts}
@y
@d banner=='This is Nihongo TFtoPL, Version 3.2-p1.7'
  {printed when the program starts}
@d jis_enc==0
@d euc_enc==1
@d sjis_enc==2
@z

@x [2] l.91 - pTeX:
procedure initialize; {this procedure gets things started properly}
@y
procedure initialize; {this procedure gets things started properly}
var @!k:integer; {all-purpose initiallization index}
@z

@x [2] l.97 - pTeX:
    tfm_file_array := cast_to_byte_pointer (xmalloc (1003));
@y
    tfm_file_array := cast_to_byte_pointer (xmalloc (1003+4));
@z

@x [7] l.149 - pTeX:
  print_ln (version_string);
@y
  print_ln (version_string);
  print ('process kanji code is ');
  case proc_kanji_code of
    jis_enc: print('jis');
    euc_enc: print('euc');
    sjis_enc: print('sjis');
  end;
  print_ln('.');
@z

@x [18.20] l.438 - pTeX:
@d index == index_type
@y
@d index == index_type
@d yoko_id_byte = 11 {id byte for YOKO-kumi kanji tfm files}
@d tate_id_byte = 9   {id byte for TATE-kumi kanji tfm files}
@d tfm_format = 1 {normal tfm formated metric files}
@d jfm_format = 2 {jfm formated metric files for YOKO-kumi kanji}
@d vfm_format = 3 {vfm formated metric files for TATE-kumi kanji}
@z

@x [18.20] l.448 - pTeX:
@!tfm_file_array: pointer_to_byte; {the input data all goes here}
@y
@!tfm_file_array: pointer_to_byte; {the input data all goes here}
@!file_format:tfm_format..vfm_format; {format of tfm file}
@!nt:0..@'77777; {number of words in the character type table}
@!ng:0..@'77777; {number of words in the glue table}
@z

@x [20] l.434 - pTeX: Read JFM header
read(tfm_file,tfm[1]); lf:=tfm[0]*@'400+tfm[1];
@y
read(tfm_file,tfm[1]); lf:=tfm[0]*@'400+tfm[1];
if lf=yoko_id_byte then
  begin file_format:=jfm_format;
  print_ln('Input file is in YOKO-kumi kanji tfm format.');
  write_ln(pl_file,'(COMMENT THIS IS A KANJI FORMAT FILE)');
  end
else if lf=tate_id_byte then
  begin file_format:=vfm_format;
  print_ln('Input file is in TATE-kumi kanji tfm format.');
  write_ln(pl_file,'(COMMENT THIS IS A KANJI FORMAT FILE)');
  write_ln(pl_file,'(DIRECTION TATE)');
  end
else  begin file_format:=tfm_format; nt:=0; tmp_ptr:=2;
  end;
if file_format<>tfm_format then
  begin if eof(tfm_file) then abort('The input file is only two bytes long!');
  read(tfm_file,tfm[2]);
  if eof(tfm_file) then abort('The input file is only three bytes long!');
  read(tfm_file,tfm[3]); nt:=tfm[2]*@'400+tfm[3];
  if eof(tfm_file) then abort('The input file is only four bytes long!');
  read(tfm_file,tfm[4]);
  if eof(tfm_file) then abort('The input file is only five bytes long!');
  read(tfm_file,tfm[5]); lf:=tfm[4]*@'400+tfm[5];
  tmp_ptr:=6;
  end;
@z

@x [20] l.445 - pTeX:  put the rest of the file into tfm[6] and after
for tfm_ptr:=2 to 4*lf-1 do
@y
for tfm_ptr:=tmp_ptr to 4*lf-1 do
@z

@x [21] l.468 - pTeX: the location of subfile sizes is different
begin tfm_ptr:=2;@/
@y
begin
if file_format<>tfm_format then
  begin tfm_ptr:=6; check_sum_value:=28;
  end
else
  begin tfm_ptr:=2; check_sum_value:=24;
  end;
@z

@x [21] l.485 - pTeX: ng has to be treated specially
if (bc>ec+1)or(ec>255) then abort('The character code range ',
@.The character code range...@>
  bc:1,'..',ec:1,'is illegal!');
if (nw=0)or(nh=0)or(nd=0)or(ni=0) then
  abort('Incomplete subfiles for character dimensions!');
@.Incomplete subfiles...@>
if ne>256 then abort('There are ',ne:1,' extensible recipes!');
@.There are ... recipes@>
if lf<>6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np then
  abort('Subfile sizes don''t add up to the stated total!');
@.Subfile sizes don't add up...@>
@y
case file_format of
tfm_format: begin
  if (bc>ec+1)or(ec>255) then abort('The character code range ',
@.The character code range...@>
    bc:1,'..',ec:1,'is illegal!');
  if (nw=0)or(nh=0)or(nd=0)or(ni=0) then
    abort('Incomplete subfiles for character dimensions!');
@.Incomplete subfiles...@>
  if ne>256 then abort('There are ',ne:1,' extensible recipes!');
@.There are ... recipes@>
  if lf<>6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np then
    abort('Subfile sizes don''t add up to the stated total!');
@.Subfile sizes don't add up...@>
  end;
jfm_format,vfm_format: begin ng:=ne;
  if (bc>ec+1)or(ec>255)or(bc<>0) then abort('The character code range ',
@.The character code range...@>
      bc:1,'..',ec:1,'is illegal!');
  if (nw=0)or(nh=0)or(nd=0)or(ni=0) then
    abort('Incomplete subfiles for character dimensions!');
@.Incomplete subfiles...@>
  if lf<>7+nt+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ng+np then
    abort('Sum of subfile sizes (',
      7+nt+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ng+np:1,
      ') is not equal to the stated total ', lf:1);
@.Subfile sizes don't add up...@>
  end;
end;
@z

@x [23] l.506 - pTeX: declare type_base.
@!char_base,@!width_base,@!height_base,@!depth_base,@!italic_base,
@!lig_kern_base,@!kern_base,@!exten_base,@!param_base:integer;
  {base addresses for the subfiles}
@y
@!char_base,@!width_base,@!height_base,@!depth_base,@!italic_base,
@!lig_kern_base,@!kern_base,@!exten_base,@!param_base:integer;
@!type_base,@!glue_base:integer; {base addresses for the subfiles}
@!check_sum_value:integer; {either 24 (normal case) or 28 (kanji case)}
@z

@x [24] l.510 - pTeX: define type_base
@ @<Compute the base addresses@>=
begin char_base:=6+lh-bc;
@y
@ @<Compute the base addresses@>=
begin
if file_format<>tfm_format then
  begin type_base:=7+lh;
  char_base:=type_base+nt-bc; { bc should be zero ...}
  end
else char_base:=6+lh-bc;
@z

@x [24] l.517 - pTeX: define glue_base
kern_base:=lig_kern_base+nl;
exten_base:=kern_base+nk;
param_base:=exten_base+ne-1;
@y
kern_base:=lig_kern_base+nl;
exten_base:=kern_base+nk;
glue_base:=exten_base;
param_base:=exten_base+ne-1;
@z

@x [25] l.529 - pTeX: must change check_sum from macro (=24) to variable
@d check_sum=24
@d design_size=check_sum+4
@d scheme=design_size+4
@d family=scheme+40
@d random_word=family+20
@y
@d check_sum==check_sum_value
@d design_size==check_sum+4
@d scheme==design_size+4
@d family==scheme+40
@d random_word==family+20
@z

@x [25] l.549 - pTeX: add definition of glue macro
@d kern(#)==4*(kern_base+#) {here \#\ is an index, not a character}
@y
@d kern(#)==4*(kern_base+#) {here \#\ is an index, not a character}
@d glue(#)==4*(glue_base+#) {likewise}
@z

@x [63] l.1003 - pTeX: Name of parameter for kanji-font
else if (i<=13)and(font_type=mathex) then
  if i=8 then out('DEFAULTRULETHICKNESS')
  else out('BIGOPSPACING',i-8:1)
else out('PARAMETER D ',i:1)
@y
else if (i<=13)and(font_type=mathex) then
  if i=8 then out('DEFAULTRULETHICKNESS')
  else out('BIGOPSPACING',i-8:1)
else if (i<=9)and(file_format<>tfm_format) then
  if i=8 then out('EXTRASTRETCH')
  else out('EXTRASHRINK')
else out('PARAMETER D ',i:1)
@z

@x [64] l.1027 - pTeX: Add printing of newline at end of program
if nk>0 then for i:=0 to nk-1 do check_fix(kern(i))('Kern');
@.Kern n is too big@>
@y
if nk>0 then for i:=0 to nk-1 do check_fix(kern(i))('Kern');
@.Kern n is too big@>
if file_format<>tfm_format then
  begin if ng>0 then for i:=0 to ng-1 do check_fix(glue(i))('Glue');
@.Glue n is too big@>
  end;
@z

@x [68] l.1068 - pTeX: we print GLUEKERN instead of LIGTABLE
  begin left; out('LIGTABLE'); out_ln;@/
@y
  begin left;
  if file_format<>tfm_format then out('GLUEKERN') else out('LIGTABLE');
  out_ln;@/
@z

@x [69] l.1090 - pTeX: we print Glue/kern instead of Ligature/kern
    print('Ligature/kern starting index for character '); print_octal(c);
    print_ln(' is too large;'); print_ln('so I removed it.'); reset_tag(c);
@.Ligature/kern starting index...@>
@y
    case file_format of
    tfm_format: print('Ligature/kern ');
@.Ligature/kern starting index...@>
    jfm_format,vfm_format: print('Glue/kern ');
@.Glue/kern index starging index...@>
    end;
    print('starting index for character '); print_octal(c);
    print_ln(' is too large;'); print_ln('so I removed it.'); reset_tag(c);
@z

@x [76] l.1179 - pTeX: if kanji format, output a glue step not a ligature step
else @<Output a ligature step@>;
@y
else if file_format=tfm_format then @<Output a ligature step@>
else if (file_format=jfm_format)or(file_format=vfm_format) then
  @<Output a glue step@>;
@z

@x [78] l.1238 - pTeX:
for c:=bc to ec do if width_index(c)>0 then
@y
for c:=bc to ec do
if width_index(c)=0 then
  begin if file_format<>tfm_format then
  bad('width index of type ',c:1,' is zero!!')
  end
else if width_index(c)>0 then
@z

@x [78] l.1246 - pTeX: the 'character' table is really the 'type' table
  left; out('CHARACTER'); out_char(c); out_ln;
@y
  left;
  if file_format<>tfm_format then
    begin out('TYPE'); tfm[0]:=c; out_octal(0,1);
    end
  else 
    begin out('CHARACTER'); out_char(c);
    end;
  out_ln;
@z

@x [78] l.1251 - pTeX: types can only have tags equal to 0 or 1
  case tag(c) of
  no_tag: do_nothing;
  lig_tag: @<Output the applicable part of the ligature/kern
    program as a comment@>;
  list_tag: @<Output the character link unless there is a problem@>;
  ext_tag: @<Output an extensible character recipe@>;
  end; {there are no other cases}
  right;
  end
@y
  case tag(c) of
  no_tag: do_nothing;
  lig_tag: @<Output the applicable part of the ligature/kern
    program as a comment@>;
  list_tag: if file_format<>tfm_format then
       bad('the tag of type ',c:1,' must be 0 or 1')
     else @<Output the character link unless there is a problem@>;
  ext_tag: if file_format<>tfm_format then
       bad('the tag of type ',c:1,' must be 0 or 1')
     else @<Output an extensible character recipe@>;
  end; {there are no other cases}
  right;
  end
@z

@x [96] l.1539 - pTeX:
var tfm_ptr:index; {an index into |tfm|}
@y
var tfm_ptr,tmp_ptr:index; {an index into |tfm|}
@z

@x [98] l.1566 - pTeX: declare kanji conversion subroutines and externals
@p begin initialize;@/
@y
@p
@<declare kanji conversion functions@>;
begin initialize;@/
@z

@x [99] l.1570 - pTeX: don't check extensible recipes and list the char_type table
@<Check the extensible recipes@>;
@y
if file_format<>tfm_format then
  begin @<list |char_type| table@>;
  end
else
  begin @<Check the extensible recipes@>;
  end;
@z

@x
const n_options = 4; {Pascal won't count array lengths for us.}
@y
const n_options = 5; {Pascal won't count array lengths for us.}
@z
@x
var @!long_options: array[0..n_options] of getopt_struct;
    @!getopt_return_val: integer;
    @!option_index: c_int_type;
    @!current_option: 0..n_options;
begin
  @<Initialize the option variables@>;
@y
var @!long_options: array[0..n_options] of getopt_struct;
    @!getopt_return_val: integer;
    @!option_index: c_int_type;
    @!current_option: 0..n_options;
    @!version_switch: boolean;
begin
  @<Initialize the option variables@>;
  version_switch := false;
@z
@x
      usage_help (TFTOPL_HELP, nil);
@y
      usage_help (PTEX_TFTOPL_HELP, nil);
@z
@x
    end else if argument_is ('version') then begin
      print_version_and_exit (banner, nil, 'D.E. Knuth');
@y
    end else if argument_is ('version') then begin
      version_switch := true;

    end else if argument_is ('kanji') then begin
      @<Set process kanji code@>;
@z
@x
    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;
@y
    end; {Else it was a flag; |getopt| has already done the assignment.}
  until getopt_return_val = -1;
  if (version_switch) then
    print_version_and_exit (banner, nil, 'D.E. Knuth');
@z

@x
@ An element with all zeros always ends the list.
@y
@ kanji option

@<Define the option...@> =
long_options[current_option].name := 'kanji';
long_options[current_option].has_arg := 1;
long_options[current_option].flag := 0;
long_options[current_option].val := 0;
incr(current_option);

@ An element with all zeros always ends the list.
@z

@x [99] l.1751 - pTeX:
@* Index.
@y
@* For Japanese Font Metric routines. % pTeX:
We need to include some routines for handling kanji character.

@d max_kanji=7237 {number of the kanji characters - 1}

@<Glob...@>=
@!this_code:integer; {to hold jis code of the current kanji character}
@!this_type:integer; {to hold |char_type| of the current kanji character}
@!type_index:integer; {index into |char_type| table}
@!type_num:integer; {index into |char_info| table}
@!type_count:integer; {number of chars with the same type}
@!kanji_index:integer; {index into |kanji_type| array}
@!kanji_type:array[0..max_kanji] of -1..255; {kanji type index}

@ @<Output a glue step@>=
begin  if nonexistent(tfm[k+1]) then
  correct_bad_char('Glue step for')(k+1)
@.Glue step for nonexistent...@>
else
  begin left; out('GLUE'); out_char(tfm[k+1]);
    if tfm[k+3]>=ng then
      begin bad('Glue index too large.');
@.Glue index too large@>
      out(' R 0.0 R 0.0 R 0.0');
      end
    else begin out_fix(glue(3*tfm[k+3]));
      out_fix(glue(3*tfm[k+3]+1));
      out_fix(glue(3*tfm[k+3]+2));
      end;
    right;
  end;
end;

@ list the |char_type| table in a similar way to the type table

@<list |char_type| table@>=
this_code:=tfm[4*type_base+0]*@'400+tfm[4*type_base+1];
this_type:=tfm[4*type_base+2]*@'400+tfm[4*type_base+3];
if (this_code<>0)or(this_type<>0) then
  begin bad('the first entry in char_type is not zero. I''ll zero it.');
  print_ln('JIS code is ', this_code:1, '. Type is ', this_type:1, '.');
  end;
for kanji_index:=0 to max_kanji do kanji_type[kanji_index]:=-1;
for type_index:=1 to nt-1 do
  begin this_code:=tfm[4*type_base + type_index * 4 + 0] * 256 +
	             tfm[4*type_base + type_index * 4 + 1];
  this_type:=tfm[4*type_base + type_index * 4 + 2] * 256 +
                 tfm[4*type_base + type_index * 4 + 3];
  if not valid_jis_code(this_code) then
    bad('jis code ', this_code:1,
	    ' in char_type table entry ', type_index:1,
    	' is not valid. Ignoring it.')
  else if (this_type <= 0) or (this_type > ec) then
    bad('type ', this_type:1, ' of jis code ', this_code:1,
        ' in char_type table is not valid. Ignoring character.')
  else
    kanji_type[jis_to_index(this_code)] := this_type;
  end;
@#
for type_num:=1 to ec do
  begin left; out('CHARSINTYPE');
  tfm[0]:=type_num; out_octal(0,1); out_ln;
  type_count:=0;
  for kanji_index:=0 to max_kanji do
    if kanji_type[kanji_index]=type_num then
      begin incr(type_count);
      if (type_count mod 14)=0 then out_ln;
      out_kanji(index_to_jis(kanji_index)); out(' ');
      end;
  if type_count=0 then bad('type ', type_num:1, 'has no characters in it!');
  out_ln; right;
  end;

@ Some subroutines to handle kanji codes and i/o

@<Globals...@>=
@!xchr:array[char] of byte; {specifiles conversion of output charcter}

@ @<Set init...@>=
for k:=@'0 to @'37 do xchr[k]:='?';
for k:=@'40 to 255 do xchr[k]:=k;

@ @<declare kanji conversion functions@>=
procedure out_kanji(jis_code:integer); { prints a kanji character }
var @!cx:integer; {KANJI code}
i:0..4; {index of array}
begin@/
if charcode_format=charcode_octal then
  begin cx:=jis_code; out(' J '); {specify jiscode format}
  dig[0]:=Hi(cx) div 16; dig[1]:=Hi(cx) mod 16;
  dig[2]:=Lo(cx) div 16; dig[3]:=Lo(cx) mod 16;
  for i:=0 to 3 do
    if dig[i]<10 then out(dig[i]) else
    case dig[i] of
       10: out('A'); 11: out('B'); 12: out('C');
       13: out('D'); 14: out('E'); 15: out('F');
    end;
  end
else begin
  if (proc_kanji_code=sjis_enc) then cx:=JIStoSJIS(jis_code)
  else cx:=JIStoEUC(jis_code);
  out(xchr[Hi(cx)]); out(xchr[Lo(cx)]);
  end;
end;

@ @<declare kanji conversion functions@>=
function valid_jis_code(cx:integer):boolean;
var first_byte, second_byte:integer; { jis code bytes }
begin valid_jis_code:=true;
first_byte:=cx div @'400; second_byte:=cx mod @'400;
if (first_byte<@"21)
     or((first_byte>@"28)and(first_byte<@"30))
     or(first_byte>@"74) then valid_jis_code:=false;
if (second_byte<@"21)or(second_byte>@"7E) then valid_jis_code:=false;
end;

@ @<declare kanji conversion functions@>=
function index_to_jis(ix:integer):integer;
begin
if ix<=8*94-1 then
  index_to_jis:=(ix div 94 + @"21) * @'400 + (ix mod 94 + @"21)
else
  index_to_jis:=((ix+7 * 94) div 94 + @"21) * @'400 + ((ix+7*94) mod 94 + @"21)
end;

@ @<declare kanji conversion functions@>=
function jis_to_index(cx:integer):integer;
var first_byte,second_byte:integer; { jis code bytes }
begin
first_byte:=cx div @'400 - @"21;
second_byte:=cx mod @'400 - @"21;
if first_byte<8 then
  jis_to_index:=first_byte*94+second_byte
else
  jis_to_index:=(first_byte-7)*94+second_byte;
end

@ output kanji code.

@<Global...@> =
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

@* Index.
@z
