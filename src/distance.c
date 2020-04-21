
#include		<lapin.h>

double			distance(t_bunny_accurate_position	a,
				 t_bunny_accurate_position	b)
{
  return (sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}
