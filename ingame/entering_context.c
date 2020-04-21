/*
**
**
**
**
*/

#include		"mangel.h"

t_bunny_response	ingame_entering(t_mangel	*mangel)
{
  generate_map(mangel, mangel->difficulty);
  return (GO_ON);
}
