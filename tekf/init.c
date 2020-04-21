/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2016
**
** TekFunction
*/

#include		"tekf.h"

void			circle(t_bunny_pixelarray	*pix,
			       t_bunny_position		pos,
			       t_bunny_position		rad,
			       unsigned int		icol,
			       unsigned int		ocol,
			       int			thik)
{
  // Marche uniquement pour un cercle parfait et non une ellipse
  t_bunny_position	p;

  (void)ocol;
  for (p.x = pos.x - rad.x - thik - 1; p.x <= pos.x + rad.x + thik; ++p.x)
    for (p.y = pos.y - rad.y - thik - 1; p.y <= pos.y + rad.y + thik; ++p.y)
      {
	int		dist = pow(p.x - pos.x, 2) + pow(p.y - pos.y, 2);

	if (dist < pow(rad.x, 2))
	  tekpixel(pix, p, icol);
      }
}

void			tekinit(void)
{
  bunny_enable_full_blit(true);
  gl_bunny_my_set_pixel = tekpixel;
  gl_bunny_my_set_line = tekline;
  gl_bunny_my_fill = tekfill;
  gl_bunny_my_blit = tekblit;
  gl_bunny_my_clear = tekclear;
  gl_bunny_my_set_disk = circle;
}

