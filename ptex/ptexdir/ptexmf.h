/* ptexmf.h:
  This file must be included after `texmfmem.h'. Because, some of these
  functions retrun `halfword' or `quarterword' data-types, which are
  defined in `texmfmem.h'.
*/
#ifndef PTEXMF_H
#define PTEXMF_H
#ifdef TeX
extern void settok();
extern halfword mktok();
extern quarterword getjfmpos();
#endif /* TeX */
#endif /* not PTEXMF_H */
