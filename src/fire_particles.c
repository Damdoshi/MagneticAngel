/*
**
**
**
**
**
*/

#include		"mangel.h"

void			fire_particles(t_mangel		*mangel,
				       unsigned int	color,
				       unsigned int	nbr,
				       t_bunny_position	where)
{
  unsigned int		i;

  for (i = 0; i < nbr; ++i)
    {
      mangel->particles[mangel->last_particle].color = color;
      mangel->particles[mangel->last_particle].delay = bunny_get_frequency();
      mangel->particles[mangel->last_particle].pos.x = where.x;
      mangel->particles[mangel->last_particle].pos.y = where.y;
      mangel->particles[mangel->last_particle].speed.x = ((rand() % 1000) / 1000.0 - 0.5) * 3;
      mangel->particles[mangel->last_particle].speed.y = ((rand() % 1000) / 1000.0 - 0.5) * 3;
      mangel->last_particle += 1;
    }
}

