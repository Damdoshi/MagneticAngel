/*
**
**
**
**
**
*/

#include		"mangel.h"

t_bunny_response	ingame_move(const t_bunny_position	*ret,
				    void			*d)
{
  (void)ret;
  (void)d;
  return (GO_ON);
}

t_bunny_response	ingame_click(t_bunny_event_state	state,
				     t_bunny_mouse_button	button,
				     t_mangel			*mangel)
{
  volatile t_bunny_position	click;
  const t_bunny_position *m = bunny_get_mouse_position();
  volatile t_bunny_position	tl;
  volatile t_bunny_size		rsiz;

  rsiz.x = mangel->screen->clipable.clip_width * mangel->screen->clipable.scale.x;
  rsiz.y = mangel->screen->clipable.clip_height * mangel->screen->clipable.scale.y;

  tl.x = (mangel->window->buffer.width - rsiz.x) / 2;
  if (m->x < tl.x || m->x > tl.x + rsiz.x)
    return (GO_ON);

  tl.y = (mangel->window->buffer.height - rsiz.y) / 2;
  if (m->y < tl.y || m->y > tl.y + rsiz.y)
    return (GO_ON);

  double		x;
  double		y;

  x = (double)(m->x - tl.x) / rsiz.x;
  y = (double)(m->y - tl.y) / rsiz.y;
  click.x = x * mangel->screen->clipable.buffer.width;
  click.y = y * mangel->screen->clipable.buffer.height;
  click.x = click.x + mangel->guy.x - mangel->screen->clipable.buffer.width / 2;
  click.y = click.y + mangel->guy.y - mangel->screen->clipable.buffer.height / 2;
  t_bunny_position pix;
  pix = click;
  click.x /= mangel->tile_size.x;
  click.y /= mangel->tile_size.y;

  if (click.x < 0 || click.y < 0 || click.x >= mangel->map_size.x || click.y >= mangel->map_size.y)
    return (GO_ON);
  
  int tile = mangel->map[click.x + click.y * mangel->map_size.x];

  if (tile == MINE)
    {
      mangel->map[click.x + click.y * mangel->map_size.x] = FLOOR;
      fire_particles(mangel, RED, 30, pix);
    }

  if (tile == CANON)
    {
      for (size_t i = 0; i < mangel->last_canon; ++i)
	{
	  if (mangel->canons[i].position.x - click.x < 0.9 &&
	      mangel->canons[i].position.y - click.y < 0.9)
	    {
	      if ((mangel->canons[i].life -= 1) <= 0)
		{
		  mangel->map[click.x + click.y * mangel->map_size.x] = FLOOR;
		  fire_particles(mangel, RED, 30, pix);
		  bunny_memswap
		    (&mangel->canons[i],
		     &mangel->canons[mangel->last_canon - 1],
		     sizeof(mangel->canons[i]));
		  mangel->last_canon -= 1;
		}
	      break ;
	    }
	}
      fire_particles(mangel, RED, 10, pix);
    }

  return (GO_ON);
}
