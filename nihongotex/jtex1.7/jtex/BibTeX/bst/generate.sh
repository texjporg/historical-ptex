#!/bin/sh
#	use cpp
/lib/cpp -P -DJPLAIN jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >jplain.bst
/lib/cpp -P -DJALPHA jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >jalpha.bst
/lib/cpp -P -DJUNSRT jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >junsrt.bst
/lib/cpp -P -DJABBRV jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >jabbrv.bst
/lib/cpp -P -DJIPSJ  jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >jipsj.bst
/lib/cpp -P -DTIPSJ  jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >tipsj.bst
/lib/cpp -P -DTIEICE jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >tieice.bst
/lib/cpp -P -DJNAME  jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >jname.bst
/lib/cpp -P -DJORSJ  jbtxbst.doc|sed -e '/^%/d' -e '/^$/d' >jorsj.bst
#	use gawk 
#gawk -f cpp.awk JPLAIN >jplain.bst
#gawk -f cpp.awk JALPHA >jalpha.bst
#gawk -f cpp.awk JUNSRT >junsrt.bst
#gawk -f cpp.awk JABBRV >jabbrv.bst
#gawk -f cpp.awk JIPSJ  > jipsj.bst
#gawk -f cpp.awk TIPSJ  > tipsj.bst
#gawk -f cpp.awk TIEICE >tieice.bst
#gawk -f cpp.awk JNAME  > jname.bst
#gawk -f cpp.awk JORSJ  > jorsj.bst
