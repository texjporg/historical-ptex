% BibTeX standard bibliography style 'abbrv'
	% version 0.98c for BibTeX versions 0.98i or later, LaTeX version 2.08
	% Copyright (C) 1985, all rights reserved
	% Copying of this file is authorized only if either
	% (1) you make absolutely no changes to your copy, including name, or
	% (2) if you do make changes, you name it something other than
	% btxbst.doc, plain.bst, unsrt.bst, alpha.bst, and abbrv.bst
	% This restriction helps ensure that all standard styles are identical

ENTRY
  { address
    author
    booktitle
    chapter
    edition
    editor
    howpublished
    institution
    journal
    key
    month
    note
    number
    organization
    pages
    publisher
    school
    series
    title
    type
    volume
    year
  }
  {}

  {label}

INTEGERS { output.state before.all mid.sentence after.sentence after.block }

FUNCTION {init.state.consts}
{ 'before.all #0 :=
  'mid.sentence #1 :=
  'after.sentence #2 :=
  'after.block #3 :=
}

STRINGS { s t }

FUNCTION {output.nonnull}
{ 's swap$ :=
  output.state mid.sentence =
    { ", " * write$ }
    { output.state after.block =
	{ add.period$ write$
	  newline$
	  "\newblock " write$
	}
	{ output.state before.all =
	    'write$
	    { add.period$ " " * write$ }
	  if$
	}
      if$
    }
  if$
  'output.state mid.sentence :=
  s
}

FUNCTION {output}
{ 's swap$ :=
  s "" =
    'skip$
    { s output.nonnull }

  if$
}

FUNCTION {output.check}
{ 's swap$ :=
  't swap$ :=
  s "" =
    { "Warning: the " t * " shouldn't be empty in " * cite$ * top$ }
    { s output.nonnull }

  if$
}

FUNCTION {output.bibitem}
{ newline$
  "\bibitem{" write$

  cite$ write$
  "}" write$
  newline$
  ""
  'output.state before.all :=
}

FUNCTION {fin.entry}
{ add.period$
  write$
  newline$
}

FUNCTION {new.block}
{ output.state before.all =
    'skip$
    { 'output.state after.block := }
  if$
}

FUNCTION {new.sentence}
{ output.state after.block =
    'skip$
    { output.state before.all =
	'skip$
	{ 'output.state after.sentence := }
      if$
    }
  if$
}

FUNCTION {not}
{   { #0 }
    { #1 }
  if$
}

FUNCTION {and}
{   'skip$
    { pop$ #0 }
  if$
}

FUNCTION {or}
{   { pop$ #1 }
    'skip$
  if$
}

FUNCTION {field.or.null}
{ duplicate$
  missing$
    { pop$ "" }
    'skip$
  if$
}

FUNCTION {italicize}
{ 's swap$ :=
  s "" =
    { "" }
    { "{\it " s * "}" * }
  if$
}

INTEGERS { nameptr namesleft numnames }

STRINGS {nameresult}

FUNCTION {format.names}
{ 's swap$ :=
  'nameptr #1 :=
  'nameresult "" :=
  'numnames s num.names$ :=
  'namesleft numnames :=
    { namesleft #0 > }
    {
      't s nameptr "{f.~}{vv~}{ll}{, jj}" format.name$ :=

      nameptr #1 >
	{ namesleft #1 >
	    { 'nameresult nameresult ", " * t * := }
	    { numnames #2 >
		{ 'nameresult nameresult "," * := }
		'skip$
	      if$
	      t "others" =
		{ 'nameresult nameresult " et al." * := }
		{ 'nameresult nameresult " and " * t * := }
	      if$
	    }
	  if$
	}
	{ 'nameresult nameresult t * := }
      if$
      'nameptr nameptr #1 + :=
      'namesleft namesleft #1 - :=
    }
  while$
  nameresult
}

FUNCTION {format.authors}
{ author missing$
    { "" }
    { author format.names }

  if$
}

FUNCTION {format.editors}
{ editor missing$
    { "" }
    { editor num.names$ #1 >
	{ editor format.names ", editors" * }
	{ editor format.names ", editor" * }
      if$
    }

  if$
}

FUNCTION {format.title}

{ title missing$
    { "" }
    { title "ul" change.case$ }
  if$
}

FUNCTION {entry.string.max} { #100 }

FUNCTION {global.string.max} { #300 }

STRINGS {pageresult}

FUNCTION {n.dashify}
{ 't swap$ :=
  'pageresult "" :=
    { t "" = not }
    { t #1 #1 substring$ "-" =
	{ t #1 #2 substring$ "--" = not
	    { 'pageresult pageresult "--" * :=
	      't t #2 global.string.max substring$ :=
	    }
	    {   { t #1 #1 substring$ "-" = }
		{ 'pageresult pageresult "-" * :=
		  't t #2 global.string.max substring$ :=
		}
	      while$
	    }
	  if$
	}
	{ 'pageresult pageresult t #1 #1 substring$ * :=
	  't t #2 global.string.max substring$ :=
	}
      if$
    }
  while$
  pageresult
}

FUNCTION {format.date}
{ year missing$
    { month missing$
	{ "" }
	{ "Warning: there's a month but no year in " cite$ * top$
	  month
	}
      if$
    }
    { month missing$
	{ year }
	{ month " " * year * }

      if$
    }
  if$
}

FUNCTION {format.btitle}
{ title field.or.null
  italicize
}

FUNCTION {format.bvolume}
{ volume missing$
    { series missing$
	{ "" }
	{ series italicize }
      if$
    }
    { "Volume~" volume *
      series missing$
	'skip$
	{ " of " * series italicize * }
      if$
    }

  if$
}

FUNCTION {format.edition}
{ edition missing$
    { "" }
    { edition "ll" change.case$ " edition" * }

  if$
}

INTEGERS {multiresult}

FUNCTION {multi.page.check}
{ 't swap$ :=
  'multiresult #0 :=
    { multiresult not
      t "" = not
      and
    }
    { t #1 #1 substring$ "-" =
      t #1 #1 substring$ "," =
      or
	{ 'multiresult #1 := }
	{ 't t #2 global.string.max substring$ := }
      if$
    }
  while$
  multiresult
}

FUNCTION {format.pages}
{ pages missing$
    { "" }
    { pages multi.page.check
	{ "pages~" pages n.dashify * }
	{ "page~" pages n.dashify * }
      if$
    }

  if$
}

FUNCTION {format.vol.num.pages}
{ volume field.or.null
  number missing$
    'skip$
    { "(" number * ")" * *
      volume missing$
	{ "Warning: there's a number but no volume in " cite$ * top$ }
	'skip$
      if$
    }
  if$
  pages missing$
    'skip$
    { duplicate$ "" =
	'skip$
	{ ":" * }
      if$
      pages n.dashify *
    }
  if$
}

FUNCTION {format.chapter.pages}
{ chapter missing$
    'format.pages
    { "chapter~" chapter *
      pages missing$
	'skip$
	{ ", " * format.pages * }
      if$
    }

  if$
}

FUNCTION {format.in.ed.booktitle}
{ booktitle missing$
    { "" }
    { 's format.editors :=
      s "" =
	{ "In " booktitle italicize * }
	{ "In " s * ", " * booktitle italicize * }
      if$
    }

  if$
}

FUNCTION {format.tr.number}
{ type missing$
    { kanji.style
        { "�Z��" }
        { "Technical Report" }
      if$
    }
    { type }
  if$
  number missing$
    'skip$
    { "~" * number * }
  if$
}

FUNCTION {article}
{ output.bibitem
  "author" format.authors output.check
  new.block
  "title" format.title output.check
  new.block
  "journal" journal field.or.null italicize output.check
  format.vol.num.pages output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {book}
{ output.bibitem
  author missing$
    { "author and editor" format.editors output.check }
    { "author" format.authors output.check }
  if$
  new.block
  "title" format.btitle output.check
  new.block
  format.bvolume output
  "publisher" publisher field.or.null output.check
  address field.or.null output
  format.edition output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {booklet}
{ output.bibitem
  format.authors output
  new.block
  "title" format.btitle output.check
  new.block
  howpublished field.or.null output
  address field.or.null output
  format.date output
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {inbook}
{ output.bibitem
  author missing$
    { "author and editor" format.editors output.check }
    { "author" format.authors output.check }
  if$
  new.block
  "title" format.btitle output.check
  "chapter and pages" format.chapter.pages output.check
  new.block
  format.bvolume output
  "publisher" publisher field.or.null output.check
  address field.or.null output
  format.edition output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {incollection}
{ output.bibitem
  "authors" format.authors output.check
  new.block
  "title" format.title output.check
  new.block
  "booktitle" format.in.ed.booktitle output.check
  format.chapter.pages output
  "publisher" publisher field.or.null output.check
  address field.or.null output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {inproceedings}
{ output.bibitem
  "author" format.authors output.check
  new.block
  "title" format.title output.check
  new.block
  "booktitle" format.in.ed.booktitle output.check
  format.pages output
  organization field.or.null output
  publisher field.or.null output
  address field.or.null output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {conference} { inproceedings }

FUNCTION {manual}
{ output.bibitem
  format.authors output
  new.block
  "title" format.btitle output.check
  new.block
  organization field.or.null output
  address field.or.null output
  format.edition output
  format.date output
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {mastersthesis}
{ output.bibitem
  "author" format.authors output.check
  new.block
  "title" format.btitle output.check
  new.block
  kanji.style
    { "�C�m�_��" output }
    { "Master's thesis" output }
  if$
  "school" school field.or.null output.check
  address field.or.null output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {misc}
{ output.bibitem
  format.authors output
  new.block
  format.title output
  new.block
  howpublished field.or.null output
  format.date output
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {phdthesis}
{ output.bibitem
  "author" format.authors output.check
  new.block
  "title" format.btitle output.check
  new.block
  kanji.style
    { "���m�_��" output }
    { "PhD thesis" output }
  if$
  "school" school field.or.null output.check
  address field.or.null output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {proceedings}
{ output.bibitem
  editor missing$
    { organization missing$
	'skip$
	{ organization field.or.null output }
      if$
    }
    { format.editors output }
  if$
  new.block
  "title" format.btitle output.check
  editor missing$
    'skip$
    { organization field.or.null output }
  if$
  publisher field.or.null output
  address field.or.null output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {techreport}
{ output.bibitem
  "author" format.authors output.check
  new.block
  "title" format.btitle output.check
  new.block
  format.tr.number output
  "institution" institution field.or.null output.check
  address field.or.null output
  "year" format.date output.check
  new.block
  note field.or.null output
  fin.entry
}

FUNCTION {unpublished}
{ output.bibitem
  "author" format.authors output.check
  new.block
  "title" format.title output.check
  new.block
  format.date output
  new.block
  "note" note field.or.null output.check
  fin.entry
}

FUNCTION {default.type} { book }

MACRO {jan} {"Jan."}

MACRO {feb} {"Feb."}

MACRO {mar} {"March"}

MACRO {apr} {"Apr."}

MACRO {may} {"May"}

MACRO {jun} {"June"}

MACRO {jul} {"July"}

MACRO {aug} {"Aug."}

MACRO {sep} {"Sep."}

MACRO {oct} {"Oct."}

MACRO {nov} {"Nov."}

MACRO {dec} {"Dec."}

MACRO {acmcs} {"ACM Comput. Surv."}

MACRO {acta} {"Acta Inf."}

MACRO {cacm} {"Commun. ACM"}

MACRO {ibmjrd} {"IBM J. Res. Dev."}

MACRO {ibmsj} {"IBM Syst. J."}

MACRO {ieeese} {"IEEE Trans. Softw. Eng."}

MACRO {ieeetc} {"IEEE Trans. Comput."}

MACRO {ieeetcad}
 {"IEEE Trans. Comput.-Aided Design Integrated Circuits"}

MACRO {ipl} {"Inf. Process. Lett."}

MACRO {jacm} {"J. ACM"}

MACRO {jcss} {"J. Comput. Syst. Sci."}

MACRO {scp} {"Sci. Comput. Programming"}

MACRO {sicomp} {"SIAM J. Comput."}

MACRO {tocs} {"ACM Trans. Comput. Syst."}

MACRO {tods} {"ACM Trans. Database Syst."}

MACRO {tog} {"ACM Trans. Gr."}

MACRO {toms} {"ACM Trans. Math. Softw."}

MACRO {toois} {"ACM Trans. Office Inf. Syst."}

MACRO {toplas} {"ACM Trans. Prog. Lang. Syst."}

MACRO {tcs} {"Theoretical Comput. Sci."}

READ

FUNCTION {sortify}
{ purify$
  "ll" change.case$
}

FUNCTION {sort.format.names}
{ 's swap$ :=
  'nameptr #1 :=
  'nameresult "" :=
  'numnames s num.names$ :=
  'namesleft numnames :=
    { namesleft #0 > }
    { nameptr #1 >
	{ 'nameresult nameresult "   " * := }
	'skip$
      if$
      't s nameptr "{vv{ } }{ll{ }}{  f{ }}{  jj{ }}" format.name$ :=

      nameptr numnames = t "others" = and
	{ 'nameresult nameresult "et al" * := }
	{ 'nameresult nameresult t sortify * := }
      if$
      'nameptr nameptr #1 + :=
      'namesleft namesleft #1 - :=
    }
  while$
  nameresult
}

INTEGERS {len}

FUNCTION {chop.word}
{ 's swap$ :=
  'len swap$ :=
  s #1 len substring$ =
    { s len #1 + global.string.max substring$ }
    { s }
  if$
}

FUNCTION {sort.format.title}
{ 't swap$ :=
  "A " #2
    "An " #3
      "The " #4 t chop.word
    chop.word
  chop.word
  #1 global.string.max substring$
  sortify
}

FUNCTION {presort}
{
  author missing$
    { editor missing$
	{ key field.or.null sortify }
	{ editor sort.format.names }
      if$
    }
    { author sort.format.names }
  if$

  "    "
  *
  title field.or.null
  sort.format.title
  *
  #1 entry.string.max substring$
  'sort.key$ swap$ :=
}

ITERATE {presort}

SORT

STRINGS {longest.label}

INTEGERS { number.label longest.label.width }

FUNCTION {initialize.longest.label}
{ 'longest.label "" :=
  'number.label #1  :=
  'longest.label.width #0 :=
}

FUNCTION {longest.label.pass}
{ 'label number.label int.to.str$ :=
  'number.label number.label #1 + :=
  label width$ longest.label.width >
    { 'longest.label label :=
      'longest.label.width label width$ :=
    }
    'skip$
  if$
}

EXECUTE {initialize.longest.label}

ITERATE {longest.label.pass}

FUNCTION {preamble}
{ "\begin{thebibliography}{"  longest.label  * "}" * write$
  newline$
}

EXECUTE {preamble}

EXECUTE {init.state.consts}

ITERATE {call.type$}

FUNCTION {finish.up} { newline$ "\end{thebibliography}" write$ newline$ }

EXECUTE {finish.up}
