/*
** Jason Brillante "Damdoshi"
** Hanged Bunny Studio 2014-2015
**
** TekFunction
*/

#include		"tekf.h"

typedef struct		s_bunny_square
{
  int			left;
  int			top;
  int			right;
  int			bottom;
}			t_bunny_square;

static inline void	_tekpixel(t_bunny_pixelarray	*pix,
				  t_bunny_position	pos,
				  unsigned int		color)
{
  t_color		res;
  t_color		ori;
  t_color		lay;
  double		under_ratio;
  double		upper_ratio;
  int			fpos;

  lay.full = color;
  if (lay.argb[ALPHA_CMP] == 0)
    return ;
  if (lay.argb[ALPHA_CMP] == 255)
    ((unsigned int*)pix->pixels)[pos.x + pos.y * pix->clipable.buffer.width] = color;
  else
    {
      res.full = 0;
      fpos = pos.x + pos.y * pix->clipable.buffer.width;
      ori.full = ((unsigned int*)pix->pixels)[fpos];

      upper_ratio = (double)lay.argb[ALPHA_CMP] / 255.0;
      under_ratio = (double)(255 - lay.argb[ALPHA_CMP]) / 255.0;

      res.argb[ALPHA_CMP] = 255;

      res.argb[RED_CMP] = ori.argb[RED_CMP] * under_ratio + lay.argb[RED_CMP] * upper_ratio;
      res.argb[GREEN_CMP] = ori.argb[GREEN_CMP] * under_ratio + lay.argb[GREEN_CMP] * upper_ratio;
      res.argb[BLUE_CMP] = ori.argb[BLUE_CMP] * under_ratio + lay.argb[BLUE_CMP] * upper_ratio;

      ((unsigned int*)pix->pixels)[fpos] = res.full;
    }
}

void			tekblit(t_bunny_pixelarray			*out,
				const t_bunny_pixelarray		*in,
				const t_bunny_position			*pos)
{
  static const t_bunny_position defaul = {0, 0};
  t_bunny_square	destination;
  t_bunny_position	i;
  t_bunny_position	e;

  if (pos == NULL)
    pos = &defaul;
  destination.left = pos->x;
  destination.top = pos->y;
  destination.right = destination.left + in->clipable.clip_width * in->clipable.scale.x;
  destination.bottom = destination.top + in->clipable.clip_height * in->clipable.scale.y;

  for (i.y = destination.top; i.y < destination.bottom; ++i.y)
    for (i.x = destination.left; i.x < destination.right; ++i.x)
      {
	e.x = in->clipable.clip_width *
	  (double)(i.x - destination.left) / (destination.right - destination.left)
	  + in->clipable.clip_x_position;
	e.y = in->clipable.clip_height *
	  (double)(i.y - destination.top) / (destination.bottom - destination.top) +
	  + in->clipable.clip_y_position;
	_tekpixel(out, i, ((int*)in->pixels)[e.x + e.y * in->clipable.buffer.width]);
      }
}
