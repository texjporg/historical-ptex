/* localization maintainers and bug report address */
#ifndef MAINTAIN_H
#define MAINTAIN_H

#if defined(TeX)
#if defined(TEXPOOLNAME)
#undef TEXPOOLNAME
#endif
#define TEXPOOLNAME "ptex.pool"
/* others are defined at ptexextra.c */
#endif

#if defined(TFTOPL)
const_string based_prog_name="TFtoPL";
const_string based_prog_version="3.1";
#endif

#if defined(PLTOTF)
const_string based_prog_name="PLtoTF";
const_string based_prog_version="3.5";
#endif

#if defined(PDVITYPE)
const_string based_prog_name="DVItype";
const_string based_prog_version="3.5";
#endif

#if defined(JBIBTEX)
/* defined at jbibextra.c */
#endif

#endif /* !MAINTAIN_H */
