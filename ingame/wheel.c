/*
**
**
**
**
*/

#include		"mangel.h"

t_bunny_response	ingame_wheel(int		wheelid,
				     int		delta,
				     t_mangel		*mangel)
{
  (void)wheelid;
  if (bunny_get_mouse_button()[BMB_LEFT] || bunny_get_keyboard()[BKS_X])
    {
      if ((mangel->shield_width += delta * 30) < 45)
	mangel->shield_width = 45;
      else if (mangel->shield_width > 360)
	mangel->shield_width = 360;
    }
  else if (bunny_get_mouse_button()[BMB_RIGHT] || bunny_get_keyboard()[BKS_V])
    {
      if ((mangel->shield_thickness += delta) < 1.0)
	mangel->shield_thickness = 1.0;
      else if (mangel->shield_thickness > 10)
	mangel->shield_thickness = 10;
    }
  else if (bunny_get_mouse_button()[BMB_MIDDLE] || bunny_get_keyboard()[BKS_C])
    {
      if ((mangel->shield_radius += delta * 5) < mangel->tile_size.x / 2)
	mangel->shield_radius = mangel->tile_size.x / 2;
      else if (mangel->shield_radius > mangel->tile_size.x * 3)
	mangel->shield_radius = mangel->tile_size.x * 3;
    }
  else
    {
      mangel->shield_angle += delta * 360 / 16;
    }
  return (GO_ON);
}
