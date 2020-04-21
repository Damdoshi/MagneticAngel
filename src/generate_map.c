/*
**
**
**
**
*/

#include		<stddef.h>
#include		"mangel.h"

void			debug2(int			*map,
			       t_bunny_size		msize,
			       int			xx,
			       int			yy)
{
  write(1, "\033[H\033[J", 3);
  for (int y = 0; y < msize.y; ++y)
    {
      for (int x = 0; x < msize.x; ++x)
	{
	  if (x == xx && y == yy)
	    printf("OOO ");
	  else
	    {
	      if (map[x + y * msize.x] == WALL)
		printf("XXX ");
	      if (map[x + y * msize.x] == HOLE)
		printf("    ");
	      else
		printf("%03d ", abs(map[x + y * msize.x]));
	    }
	}
      putchar('\n');
    }
  bunny_usleep(1e5);
}

void			debug_map(t_bunny_size		map_size,
				  int			*map,
				  int			xs,
				  int			ys)
{
  char			*c = "@ --X!MCE";

  write(1, "\033[H", 3);
  for (int y = 0; y < map_size.y; ++y)
    {
      for (int x = 0; x < map_size.x; ++x)
	if (x == xs && y == ys)
	  {
	    putchar('O');
	    putchar('O');
	  }
	else if (map[x + y * map_size.x] > ENERGY)
	  {
	    putchar('|');
	    putchar('|');
	  }
	else
	  {
	    putchar(c[map[x + y * map_size.x] + 1]);
	    putchar(c[map[x + y * map_size.x] + 1]);
	  }
      putchar('\n');
    }
}


void			debug_path(t_mangel		*mangel,
				   t_move_list		*ml,
				   int			x,
				   int			y)
{
  int			*copy = bunny_memdup(mangel->map, sizeof(*copy) * mangel->map_size.x * mangel->map_size.y);
  // int			xx = x;
  // int			yy = y;

  for (size_t i = 0; i < ml->move_length; ++i)
    {
      copy[x + y * mangel->map_size.x] = 10;
      if (ml->moves[i].direction == GOLEFT)
	x -= 1;
      if (ml->moves[i].direction == GORIGHT)
	x += 1;
      if (ml->moves[i].direction == GOUP)
	y -= 1;
      if (ml->moves[i].direction == GODOWN)
	y += 1;
    }
  //debug_map(mangel->map_size, copy, xx, yy);
}


int			get_tile(t_mangel		*mangel,
				 int			x,
				 int			y)
{
  if (x < 0 || y < 0 || x >= mangel->map_size.x || y >= mangel->map_size.y)
    return (HOLE);
  return (mangel->map[x + y * mangel->map_size.x]);
}

int			get_tilem(int			*map,
				  t_bunny_size		map_size,
				  int			x,
				  int			y)
{
  if (x < 0 || y < 0 || x >= map_size.x || y >= map_size.y)
    return (HOLE);
  return (map[x + y * map_size.x]);
}

int			set_tilem(int			*map,
				  t_bunny_size		msize,
				  int			x,
				  int			y,
				  int			tile,
				  bool			wall)
{
  if (x < 0 || y < 0 || x >= msize.x || y >= msize.y)
    return (0);
  if (map[x + y * msize.x] == tile)
    return (0);
  if (map[x + y * msize.x] == WALKABLE_FLOOR && tile >= 0)
    return (0);
  if (wall && tile == WALKABLE_FLOOR)
    {
      set_tilem(map, msize, x - 1, y - 1, WALL, false);
      set_tilem(map, msize, x - 1, y + 1, WALL, false);
      set_tilem(map, msize, x + 1, y + 1, WALL, false);
      set_tilem(map, msize, x + 1, y - 1, WALL, false);
      set_tilem(map, msize, x - 1, y, WALL, false);
      set_tilem(map, msize, x + 1, y, WALL, false);
      set_tilem(map, msize, x, y - 1, WALL, false);
      set_tilem(map, msize, x, y + 1, WALL, false);
    }
  map[x + y * msize.x] = tile;
  return (1);
}

int			set_tile(t_mangel		*mangel,
				 int			x,
				 int			y,
				 int			tile,
				 bool			wall)
{
  return (set_tilem(mangel->map, mangel->map_size, x, y, tile, wall));
}

static int		draw_floor_square(t_mangel	*mangel,
					  int		x,
					  int		y,
					  int		siz,
					  bool		wall)
{
  int			count;

  count = 0;
  x -= siz / 2;
  y -= siz / 2;
  for (int j = 0; j < siz; ++j)
    for (int i = 0; i < siz; ++i)
      {
	if (floor(siz / 2) == i && floor(siz / 2) == j)
	  count += set_tile(mangel, i + x, j + y, WALKABLE_FLOOR, wall);
	else
	  count += set_tile(mangel, i + x, j + y, FLOOR, wall);
      }
  return (count);
}

static int		join_dots(t_mangel		*mangel,
				  t_bunny_position	from,
				  t_bunny_position	to,
				  int			angle_size,
				  int			road_size,
				  bool			mid_angle,
				  int			orientation,
				  bool			wall)
{
  int			count = 0;
  
  count += draw_floor_square(mangel, from.x, from.y, angle_size, wall);
  count += draw_floor_square(mangel, to.x, to.y, angle_size, wall);

  if (orientation == 0)
    orientation = rand() % 2 ? 1 : -1;
  if (orientation == 1)
    {
      int			i;

      // Horizontal puis vertical
      for (i = from.x; i != to.x; i < to.x ? ++i : --i)
	for (int k = -road_size / 2; k < round(road_size / 2.0 + 0.5); ++k)
	  {
	    if (k == 0)
	      count += set_tile(mangel, i, from.y + k, WALKABLE_FLOOR, wall);
	    else
	      count += set_tile(mangel, i, from.y + k, FLOOR, wall);
	  }

      if (mid_angle)
	count += draw_floor_square(mangel, i, from.y, angle_size, wall);

      for (int j = from.y; j != to.y; j < to.y ? ++j : --j)
	for (int k = -road_size / 2; k <= round(road_size / 2.0 + 0.5); ++k)
	  {
	    if (k == 0)
	      count += set_tile(mangel, i + k, j, WALKABLE_FLOOR, wall);
	    else
	      count += set_tile(mangel, i + k, j, FLOOR, wall);
	  }
    }
  else
    {
      int			j;

      // Vertical puis horizontal
      for (j = from.y; j != to.y; j < to.y ? ++j : --j)
	for (int k = -road_size / 2; k < round(road_size / 2.0 + 0.5); ++k)
	  {
	    if (k == 0)
	      count += set_tile(mangel, from.x + k, j, WALKABLE_FLOOR, wall);
	    else
	      count += set_tile(mangel, from.x + k, j, FLOOR, wall);
	  }

      if (mid_angle)
	count += draw_floor_square(mangel, from.x, j, angle_size, wall);

      for (int i = from.x; i != to.x; i < to.x ? ++i : --i)
	for (int k = -road_size / 2; k < round(road_size / 2.0 + 0.5); ++k)
	  {
	    if (k == 0)
	      count += set_tile(mangel, i, j + k, WALKABLE_FLOOR, wall);
	    else
	      count += set_tile(mangel, i, j + k, FLOOR, wall);
	  }
    }
  return (count);
}

static int		generate_bridge(t_mangel	*mangel,
					int		bridge,
					int		orientation,
					bool		wall)
{
  t_bunny_position	from;
  t_bunny_position	to;
  int			i;
  int			count = 0;
  
  for (i = 0; i < bridge; ++i)
    {
      do
	{
	  from.x = rand() % mangel->map_size.x;
	  from.y = rand() % mangel->map_size.y;
	}
      while (get_tile(mangel, from.x, from.y) != WALKABLE_FLOOR);
      do
	{
	  to.x = rand() % mangel->map_size.x;
	  to.y = rand() % mangel->map_size.y;
	}
      while (get_tile(mangel, to.x, to.y) != WALKABLE_FLOOR);
      count += join_dots
	(mangel, from, to, rand() % 3 + 1, rand() % 2 + 1, rand() % 2, orientation, wall);
    }
  return (count);
}

bool			is_reachable_rec(t_bunny_position msize,
					 int		*map,
					 t_bunny_position from,
					 t_bunny_position to)
{
  if (get_tilem(map, msize, from.x, from.y) != WALKABLE_FLOOR)
    return (false);
  if (from.x == to.x && from.y == to.y)
    return (true);
  //debug_map(msize, map, from.x, from.y);
  map[from.x + from.y * msize.x] = -1;
  from.x += 1;
  if (is_reachable_rec(msize, map, from, to))
    return (true);
  from.x -= 2;
  if (is_reachable_rec(msize, map, from, to))
    return (true);
  from.x += 1;
  from.y += 1;
  if (is_reachable_rec(msize, map, from, to))
    return (true);
  from.y -= 2;
  if (is_reachable_rec(msize, map, from, to))
    return (true);
  from.y += 1;
  return (false);
}

bool			is_reachable(t_mangel		*mangel,
				     t_bunny_position	from,
				     t_bunny_position	to)
{
  int			*copy = bunny_memdup(mangel->map, sizeof(*copy) * mangel->map_size.x * mangel->map_size.y);
  bool			res;

  res = is_reachable_rec(mangel->map_size, copy, from, to);
  bunny_free(copy);
  return (res);
}

int			find_smallest(int		*tab)
{
  int			index;
  int			val;

  val = tab[0];
  index = 0;
  for (int i = 0; i < 4; ++i)
    if (tab[i] < val)
      {
	index = i;
	val = tab[i];
      }
  return (index);
}


int			find_greatest_not_positive(int		*tab)
{
  int			index;
  int			val;

  val = 1;
  index = -1;
  for (int i = 0; i < 4; ++i)
    if (tab[i] < 0)
      {
	val = tab[i];
	index = i;
	break ;
      }
  if (index == -1)
    {
      puts("fuck this map");
      exit(0);
    }
  for (int i = 0; i < 4; ++i)
    if (tab[i] < 0 && abs(val) > abs(tab[i]))
      {
	index = i;
	val = tab[i];
      }
  return (index);
}

int			set_if_possible(t_mangel	*mangel,
					int		*map,
					int		x,
					int		y,
					int		tile)
{
  int			ctile;
  
  ctile = get_tilem(map, mangel->map_size, x, y);
  if (ctile >= 0 && ctile != WALKABLE_FLOOR) // si c'est une vraie tuile qui est pas du sol, on arrete
    return (0);
  if (ctile < 0 && abs(ctile) < abs(tile)) // Si c'est une tuile sol et qui est plus proche...
    return (ctile);
  return (map[mangel->map_size.x * y + x] = tile);
}

int			astar(t_mangel			*mangel,
			      int			*map,
			      t_bunny_position		from,
			      t_bunny_position		to,
			      int			dist)
{
  int			ctile;

  ctile = get_tilem(map, mangel->map_size, from.x, from.y);
  if (ctile >= 0 && ctile != WALKABLE_FLOOR && ctile != END_TILE)
    return (-1);
  if (ctile == END_TILE)
    {
      map[mangel->map_size.x * from.y + from.x] = dist;
      return (0);
    }
  // Si c'est pas une tuile normale ET
  // 0Si la tuile qu'on a est plus grande DONC, EN VALEUR ABSOLUE, PLUS GRANDE
  if (ctile < 0 && ctile > dist)
    return (-1);
  map[mangel->map_size.x * from.y + from.x] = dist;
  // Si on a fini...
  //debug2(map, mangel->map_size, from.x, from.y);
  if (from.x == to.x && from.y == to.y)
    return (0);
  int			score[4];
  int			index;
  
  score[0] = set_if_possible(mangel, map, from.x + 1, from.y, dist - 1);
  score[1] = set_if_possible(mangel, map, from.x - 1, from.y, dist - 1);
  score[2] = set_if_possible(mangel, map, from.x, from.y - 1, dist - 1);
  score[3] = set_if_possible(mangel, map, from.x, from.y + 1, dist - 1);
  index = find_smallest(&score[0]);

  if (score[GORIGHT] == score[index])
    {
      from.x += 1;
      if (astar(mangel, map, from, to, dist - 1) == 0)
	return (0);
      from.x -= 1;
    }

  if (score[GOLEFT] == score[index])
    {
      from.x -= 1;
      if (astar(mangel, map, from, to, dist - 1) == 0)
	return (0);
      from.x += 1;
    }

  if (score[GOUP] == score[index])
    {
      from.y -= 1;
      if (astar(mangel, map, from, to, dist - 1) == 0)
	return (0);
      from.y += 1;
    }

  if (score[GODOWN] == score[index])
    {
      from.y += 1;
      if (astar(mangel, map, from, to, dist - 1) == 0)
	return (0);
      from.y -= 1;
    }
  return (-1);
}

void			extract_path(t_mangel		*mangel,
				     t_move_list	*ml,
				     int		*map,
				     t_bunny_position	from,
				     t_bunny_position	to)
{
  if (from.x == to.x && from.y == to.y)
    return ;
  int			tile;

  tile = get_tilem(map, mangel->map_size, to.x, to.y);
  if (abs(tile) > ml->move_length)
    ml->move_length = abs(tile) + 1;
  if (tile > 0) // FIX DE MERDE
    tile = NBRCELL(ml->moves) - 1;
  ml->moves[abs(tile)].repeat = bunny_get_frequency(); // frequency
  int			score[4];
  int			index;

  score[0] = get_tilem(map, mangel->map_size, to.x + 1, to.y);
  score[1] = get_tilem(map, mangel->map_size, to.x - 1, to.y);
  score[2] = get_tilem(map, mangel->map_size, to.x, to.y - 1);
  score[3] = get_tilem(map, mangel->map_size, to.x, to.y + 1);
  index = find_greatest_not_positive(&score[0]);
  int			revert_score[4] = {GOLEFT, GORIGHT, GODOWN, GOUP};

  ml->moves[abs(tile)].direction = revert_score[index];
  if (index == GORIGHT)
    to.x += 1;
  else if (index == GOLEFT)
    to.x -= 1;
  else if (index == GOUP)
    to.y -= 1;
  else if (index == GODOWN)
    to.y += 1;
  extract_path(mangel, ml, map, from, to);
}				     

// s'assure que l'endroit a vue sur le bout de route original
t_bunny_position	random_position_around(t_mangel	*mangel,
					       int	x,
					       int	y)
{
  t_bunny_position	pos;
  bool			bad;

  do
    {
      double		rad = (rand() % (2 * 3140)) / 1000.0;
      int			dist = rand() % 5;
      
      bad = false;
      for (int i = 0; i < dist; ++i)
	{
	  pos.x = cos(rad) * i + x;
	  pos.y = sin(rad) * i + y;
	  if (pos.x < 0 || pos.y < 0 || pos.x >= mangel->map_size.x || pos.y >= mangel->map_size.y)
	    {
	      bad = true;
	      break ;
	    }
	  if (mangel->map[pos.x + pos.y * mangel->map_size.x] == WALL)
	    {
	      bad = true;
	      break ;
	    }
	}
    }
  while (bad);
  return (pos);
}

t_bunny_position	random_reachable_around(t_mangel	*mangel,
						int		x,
						int		y,
						int		mindist)
{
  t_bunny_position	here = {x, y};
  t_bunny_position	pos;
  bool			bad;

  do
    {
      double		rad = (rand() % (2 * 3140)) / 1000.0;
      int		dist = rand() % (5 - mindist) + mindist;
      
      bad = false;
      pos.x = cos(rad) * dist + x;
      pos.y = sin(rad) * dist + y;
      if (pos.x < 0 || pos.y < 0 || pos.x >= mangel->map_size.x || pos.y >= mangel->map_size.y)
	{
	  bad = true;
	  break ;
	}
      if (mangel->map[pos.x + pos.y * mangel->map_size.x] != FLOOR)
	{
	  bad = true;
	  break ;
	}
      if (!is_reachable(mangel, here, pos))
	{
	  bad = true;
	  break ;
	}
    }
  while (bad);
  return (pos);
}


void			generate_mine(t_mangel		*mangel,
				      int		x,
				      int		y)
{
  int			*copy = bunny_memdup(mangel->map, sizeof(*copy) * mangel->map_size.x * mangel->map_size.y);
  // int			xx = x;
  // int			yy = y;
  int			peace_at_start = 10;

  for (size_t i = 0; i < mangel->move_list.move_length; ++i)
    {
      copy[x + y * mangel->map_size.x] = 10;
      if (mangel->move_list.moves[i].direction == GOLEFT)
	x -= 1;
      if (mangel->move_list.moves[i].direction == GORIGHT)
	x += 1;
      if (mangel->move_list.moves[i].direction == GOUP)
	y -= 1;
      if (mangel->move_list.moves[i].direction == GODOWN)
	y += 1;
      if (mangel->map[x + y * mangel->map_size.x] == WALKABLE_FLOOR)
	{
	  if (peace_at_start < 0 && rand() % 10 == 0)
	    {
	      mangel->map[x + y * mangel->map_size.x] = MINE;
	    }
	  if (peace_at_start < 0 && rand() % 10 == 0)
	    {
	      t_bunny_position	pos = random_position_around(mangel, x, y);

	      mangel->map[pos.x + pos.y * mangel->map_size.x] = CANON;
	      mangel->canons[mangel->last_canon].position.x = pos.x;
	      mangel->canons[mangel->last_canon].position.y = pos.y;
	      mangel->canons[mangel->last_canon].delay = 0;
	      mangel->canons[mangel->last_canon].life = 3;
	      mangel->last_canon += 1;
	    }
	  if (rand() % 20 == 0)
	    {
	      t_bunny_position	pos = random_reachable_around(mangel, x, y, 2);

	      mangel->map[pos.x + pos.y * mangel->map_size.x] = ENERGY;
	    }

	}
      peace_at_start -= 1;
    }
}

int			write_shortest_path(t_mangel	*mangel,
					    t_bunny_position from,
					    t_bunny_position to)
{
  t_move_list		*ml = bunny_calloc(sizeof(*ml), 1);
  int			*copy = bunny_memdup(mangel->map, sizeof(*copy) * mangel->map_size.x * mangel->map_size.y);

  astar(mangel, copy, from, to, -1);
  //debug2(copy, mangel->map_size, from.x, from.y);
  //debug_map(mangel->map_size, mangel->map, from.x, from.y);
  int i, j;
  for (i = 0; i < NBRCELL(ml->moves); ++i)
    ml->moves[i].direction = -1;
  extract_path(mangel, ml, copy, from, to);

  for (i = 0, j = mangel->move_list.move_length; i < NBRCELL(ml->moves); ++i)
    {
      if (ml->moves[i].direction != -1)
	{
	  mangel->move_list.moves[j].direction = ml->moves[i].direction;
	  mangel->move_list.moves[j].repeat = ml->moves[i].repeat;
	  j += 1;
	}
    }
  mangel->move_list.move_length = j;
  bunny_free(copy);
  bunny_free(ml);
  return (0);
}

void			generate_map(t_mangel		*mangel,
				     double		difficulty)
{
  int			xs, ys, xe, ye;

  if (mangel->map)
    bunny_free(mangel->map);
  memset(&mangel->start, 0, sizeof(*mangel) - offsetof(t_mangel, start));
  
  mangel->map_size.x = rand() % (int)(10 + difficulty * 10) + 20 + difficulty * 20;
  mangel->map_size.y = rand() % (int)(10 + difficulty * 10) + 20 + difficulty * 20;

  int			surface = mangel->map_size.x * mangel->map_size.y;
  int			good_distance;

  if (mangel->map_size.x > mangel->map_size.y)
    good_distance = mangel->map_size.x * 0.75;
  else
    good_distance = mangel->map_size.y * 0.75;
  mangel->map = bunny_calloc((mangel->map_size.x * mangel->map_size.y), sizeof(*mangel->map));
  do
    {
      mangel->guy.x = xs = rand() % (mangel->map_size.x - 2) + 1;
      mangel->guy.y = ys = rand() % (mangel->map_size.y - 2) + 1;
      mangel->end_position.x = xe = rand() % (mangel->map_size.x - 2) + 1;
      mangel->end_position.y = ye = rand() % (mangel->map_size.y - 2) + 1;
    }
  while (distance(mangel->guy, mangel->end_position) < good_distance);

  mangel->guy.x *= mangel->tile_size.x;
  mangel->guy.y *= mangel->tile_size.y;
  mangel->end_position.x *= mangel->tile_size.x;
  mangel->end_position.y *= mangel->tile_size.y;
  mangel->guy.x += mangel->tile_size.x / 2.0;
  mangel->guy.y += mangel->tile_size.y / 2.0;

  t_bunny_position	from = {xs, ys};
  t_bunny_position	to = {xe, ye};  
  int			count = 0;
  double		ratio = ((rand() % 1000) / 1000.0) * 0.1 + 0.05;
  
  // La carte est maintenant remplie de trous. On va commencer par faire deux chemins surs.
  // Avec des carrés pour marquer l'angle, point de départ et d'arrivé
  int			initial_orientation = rand() % 2 ? 1 : -1;

  count += join_dots(mangel, from, to, rand() % 3 + 1, rand() % 2 + 1, rand() % 2, initial_orientation, rand() % 2);

  // On ajoute des ponts dans l'autre sens
  initial_orientation *= -1;
  for (int v = 0; v < 3; ++v)
    generate_bridge(mangel, 1, initial_orientation, rand() % 2);

  // On crée un nombre d'ilot aléatoire, dépendant de l'aire de la carte et d'un ratio random
  while (((double)count / surface) < ratio)
    {
      count += draw_floor_square(mangel, rand() % mangel->map_size.x, rand() % mangel->map_size.y, rand() % 3 + 2, rand() % 2);
      count += generate_bridge(mangel, 1, 0, rand() % 2);
    }

  // On crée entre 3 et 6 passerelle supplémentaires
  generate_bridge(mangel, rand() % 10 + 10, 0, rand() % 2);
  
  // On force la fin
  mangel->map[(int)(xe + ye * mangel->map_size.x)] = END_TILE;

  //debug_map(mangel->map_size, mangel->map, xs, ys);
  
  //debug_map(mangel->map_size, mangel->map, xs, ys);
  // On va maintenant créer le chemin du gars
  t_bunny_position	intermed;
  t_bunny_position	prev;

  intermed.x = prev.x = xs;
  intermed.y = prev.y = ys;
  for (int w = 0; w < difficulty / 2 + 1; ++w)
    {
      do
	{
	  intermed.x = rand() % mangel->map_size.x;
	  intermed.y = rand() % mangel->map_size.y;
	}
      while (get_tile(mangel, intermed.x, intermed.y) != WALKABLE_FLOOR && !is_reachable(mangel, prev, intermed));
      write_shortest_path(mangel, prev, intermed);
      prev = intermed;
    }
  // On ecrit le dernier bout de chemin
  prev.x = xe;
  prev.y = ye;
  write_shortest_path(mangel, intermed, prev);

  generate_mine(mangel, from.x, from.y);

  mangel->delay = 2 * bunny_get_frequency();
  mangel->shield_position = mangel->guy;
  mangel->shield_radius = mangel->tile_size.x;
  mangel->shield_angle = 0;
  mangel->shield_width = 90;
  mangel->shield_thickness = 1;
  mangel->guy_life = 1000;
  mangel->shield_energy = 1000;

  //debug_map(mangel->map_size, mangel->map, xs, ys);
}

