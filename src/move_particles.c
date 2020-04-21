/*
**
**
**
**
**
*/

#include		"mangel.h"

void			move_particles(t_mangel		*mangel)
{
  int			i;

  for (i = 0; i < mangel->last_particle; ++i)
    {
      if ((mangel->particles[i].delay -= 1) <= 0)
	{
	  bunny_memswap
	    (&mangel->particles[i],
	     &mangel->particles[mangel->last_particle - 1],
	     sizeof(mangel->particles[i]));
	  mangel->last_particle -= 1;
	  --i;
	  continue ;
	}
      mangel->particles[i].pos.x += mangel->particles[i].speed.x;
      mangel->particles[i].pos.y += mangel->particles[i].speed.y;
    }
}
