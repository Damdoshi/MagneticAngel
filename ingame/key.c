/*
**
**
**
**
*/

#include		"mangel.h"

t_bunny_response	ingame_key(t_bunny_event_state	state,
				   t_bunny_keysym	sym,
				   t_mangel		*mangel)
{
  if (sym == BKS_ESCAPE)
    return (EXIT_ON_SUCCESS);
  if (sym == BKS_SPACE && state == GO_DOWN && mangel->shield_energy > 200)
    {
      mangel->slow += bunny_get_frequency() * 2;
      mangel->shield_energy -= 200;
    }
  return (GO_ON);
}

