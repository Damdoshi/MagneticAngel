/*
**
**
**
**
**
*/

#include		"mangel.h"

t_bunny_response	ingame_loop(t_mangel		*mangel)
{
  double		tile_per_second = 2 + mangel->difficulty;
  static int		alt;

  if (mangel->slow > 0)
    {
      if (alt == 0)
	alt = 1;
      else
	{
	  alt = 0;
	  return (GO_ON);
	}
      mangel->slow -= 1;
    }
  for (size_t i = 0; i < mangel->last_canon && mangel->guy_life > 0; ++i)
    {
      t_bunny_accurate_position guy =
	{
	 mangel->guy.x / mangel->tile_size.x,
	 mangel->guy.y / mangel->tile_size.y
	};
      if (distance(mangel->canons[i].position, guy) < 5
	  && mangel->canons[i].delay <= 0)
	{
	  mangel->canons[i].delay = bunny_get_frequency() * 4;
	  int w[] = {1, 1, 1, 1, 2, 2, 3};
	  fire_bullet(mangel, mangel->canons[i].position, w[rand() % NBRCELL(w)]);
	}
      mangel->canons[i].delay -= 1;
    }

  if (mangel->delay > 0)
    {
      if ((mangel->delay -= 1) <= 0 && (mangel->move_list.current_move == mangel->move_list.move_length || mangel->guy_life < 0))
	{
	  if (mangel->guy_life <= 0)
	    return (SWITCH_CONTEXT);
	  if ((mangel->difficulty += 0.2) > 3)
	    mangel->difficulty = 3;
	  return (SWITCH_CONTEXT);
	}
    }
  else if (mangel->move_list.current_move < mangel->move_list.move_length)
    {
      for (int i = 0; i < tile_per_second; ++i)
	{
	  if (mangel->move_list.moves[mangel->move_list.current_move].direction == GOLEFT)
	    mangel->guy.x -= 1;
	  if (mangel->move_list.moves[mangel->move_list.current_move].direction == GORIGHT)
	    mangel->guy.x += 1;
	  if (mangel->move_list.moves[mangel->move_list.current_move].direction == GOUP)
	    mangel->guy.y -= 1;
	  if (mangel->move_list.moves[mangel->move_list.current_move].direction == GODOWN)
	    mangel->guy.y += 1;

	  t_bunny_position here =
	    {
	     (int)(mangel->guy.x / mangel->tile_size.x),
	     (int)(mangel->guy.y / mangel->tile_size.y)
	    };
	  int til = mangel->map[here.x + here.y * mangel->map_size.x];

	  if (til == MINE)
	    {
	      t_bunny_position g = {(int)mangel->guy.x, (int)mangel->guy.y};
	      fire_particles(mangel, RED, 30, g);
	      if ((mangel->guy_life -= 334) < 0)
		mangel->delay = bunny_get_frequency() * 2;
	      mangel->map[here.x + here.y * mangel->map_size.x] = WALKABLE_FLOOR;
	      if (rand() % 2)
		mangel->shake += rand() % 5 + 5;
	      else
		mangel->shake += -rand() % 5 - 5;
	    }
	  
	  if ((mangel->move_list.moves[mangel->move_list.current_move].repeat -= 1) <= 0)
	    if ((mangel->move_list.current_move += 1) >= mangel->move_list.move_length)
	      {
		mangel->delay = bunny_get_frequency() * 2;
	      }
	}
    }

  t_bunny_accurate_position nsp =
    {
     mangel->shield_position.x,
     mangel->shield_position.y
    };
  
  mangel->shield_detached = false;
  tile_per_second *= 2;
  if (bunny_get_keyboard()[BKS_Q] || bunny_get_keyboard()[BKS_A])
    {
      mangel->shield_position.x -= (tile_per_second * mangel->tile_size.x) * bunny_get_delay();
      mangel->shield_detached = true;
    }
  if (bunny_get_keyboard()[BKS_D])
    {
      mangel->shield_position.x += (tile_per_second * mangel->tile_size.x) * bunny_get_delay();
      mangel->shield_detached = true;
    }
  if (bunny_get_keyboard()[BKS_Z] || bunny_get_keyboard()[BKS_W])
    {
      mangel->shield_position.y -= (tile_per_second * mangel->tile_size.y) * bunny_get_delay();
      mangel->shield_detached = true;
    }
  if (bunny_get_keyboard()[BKS_S])
    {
      mangel->shield_position.y += (tile_per_second * mangel->tile_size.y) * bunny_get_delay();
      mangel->shield_detached = true;
    }

  if (mangel->shield_detached == false)
    {
      if (distance(mangel->guy, mangel->shield_position) > 10)
	{
	  tile_per_second *= 4;
	  double		angle = atan2
	    (mangel->guy.y - mangel->shield_position.y, mangel->guy.x - mangel->shield_position.x);

	  mangel->shield_position.x +=
	    cos(angle) * tile_per_second * mangel->tile_size.x * bunny_get_delay();
	  mangel->shield_position.y +=
	    sin(angle) * tile_per_second * mangel->tile_size.y * bunny_get_delay();
	}
      else
	{
	  mangel->shield_position = mangel->guy;
	}
      
      double	consumption;

      consumption = 8 * mangel->shield_width / 360.0;
      consumption += mangel->shield_radius / mangel->tile_size.x;
      consumption *= mangel->shield_thickness;
      // ~40 with all set to maximum
      if (consumption < (45.0 / 360.0) * mangel->tile_size.x)
	{
	  consumption = 0;
	  if ((mangel->shield_energy += 0.3) > 1000)
	    mangel->shield_energy = 1000;
	}
      else
	consumption /= 2;
      if ((mangel->shield_energy -= consumption) < 0)
	{
	  mangel->shield_width = 45;
	  mangel->shield_thickness = 1.0;
	  mangel->shield_radius = mangel->tile_size.x / 2;
	}
      mangel->shield_consumption = consumption;
    }
  else
    {
      t_bunny_position ts =
	{
	 mangel->shield_position.x / mangel->tile_size.x,
	 mangel->shield_position.y / mangel->tile_size.y,
	};
      if (ts.x >= 0 && ts.y >= 0 && ts.x < mangel->map_size.x && ts.y < mangel->map_size.y)
	{
	  int stile = mangel->map[ts.x + ts.y * mangel->map_size.x];

	  if (stile != ENERGY && stile != FLOOR && stile != WALKABLE_FLOOR)
	    {
	      mangel->shield_position.x = nsp.x;
	      mangel->shield_position.y = nsp.y;
	    }
	  else if (stile == ENERGY)
	    {
	      mangel->map[ts.x + ts.y * mangel->map_size.x] = FLOOR;
	      if ((mangel->guy_life += 100) > 1000)
		mangel->guy_life = 1000;
	      if ((mangel->shield_energy += 300) > 1000)
		mangel->shield_energy = 1000;
	    }
	}
      else
	{
	  mangel->shield_position.x = nsp.x;
	  mangel->shield_position.y = nsp.y;
	}
    }

  move_particles(mangel);
  move_shot(mangel);
  if (fabs((mangel->shake *= 0.9)) < 1.0)
    mangel->shake = 0;
  return (GO_ON);
}

