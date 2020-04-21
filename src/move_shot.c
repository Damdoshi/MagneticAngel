/*
**
**
**
**
**
*/

#include		"mangel.h"

void			move_shot(t_mangel		*mangel)
{
  size_t		i;

  for (i = 0; i < mangel->last_bullet; ++i)
    {
      if ((mangel->bullet[i].delay -= 1) <= 0)
	{
	  bunny_memswap
	    (&mangel->bullet[i],
	     &mangel->bullet[mangel->last_bullet - 1],
	     sizeof(mangel->particles[i]));
	  mangel->last_bullet -= 1;
	  --i;
	  continue ;
	}
      if ((mangel->bullet[i].dist += 0.05) > 1.0)
	mangel->bullet[i].dist = 1.0;
    }
}
