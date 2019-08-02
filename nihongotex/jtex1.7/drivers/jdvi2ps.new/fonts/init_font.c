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
/*  DEFFONT(jfmblankops); */
  DEFFONT(jfmboxops);
  DEFFONT(pxlops);
  DEFFONT(gfops);
  DEFFONT(pkops);
  DEFFONT(kfontops); 
  DEFFONT(jxl4ops);
}
