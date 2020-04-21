/*
**
**
**
**
**
*/

#include		"mangel.h"

void			fire_bullet(t_mangel		*mangel,
				    t_bunny_accurate_position	original_pos,
				    double		width)
{
  mangel->bullet[mangel->last_bullet].dist = 0;
  mangel->bullet[mangel->last_bullet].delay = bunny_get_frequency() * 1;
  mangel->bullet[mangel->last_bullet].original_position = original_pos;
  mangel->bullet[mangel->last_bullet].width = width;
  mangel->last_bullet += 1;
}

