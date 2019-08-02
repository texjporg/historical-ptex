/*
 * init_font():
 *
 * Initialize font utilities.
 * You must define available fonts using DEFFONT() macro.
 */

static void
init_font()
{
/*  DEFFONT(tfmblankops); */
  DEFFONT(tfmboxops);
  DEFFONT(jfmblankops); /* dummy Japanese font */
/*  DEFFONT(jfmboxops); */
  DEFFONT(pxlops);
  DEFFONT(gfops);
  DEFFONT(pkops);
}
