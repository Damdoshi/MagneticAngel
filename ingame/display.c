/*
**
**
**
**
*/

#include		"mangel.h"

static int		angle_diff(int			a,
				   int			b)
{
  int			phi = abs(b - a) % 360;
  int			dist = phi > 180 ? 360 - phi : phi;
  return (dist);
}

static void		draw_arc(t_mangel		*mangel)
{
  t_bunny_position	pos;
  t_bunny_position	size =
    {
     .x = mangel->tile_size.x / 3,
     .y = mangel->tile_size.y / 3
    };

  pos.x = mangel->shield_position.x - mangel->guy.x + mangel->screen->clipable.buffer.width / 2;
  pos.y = mangel->shield_position.y - mangel->guy.y + mangel->screen->clipable.buffer.height / 2;
  if (mangel->shield_detached)
    {
      bunny_set_disk(&mangel->screen->clipable.buffer, pos, size, GREEN, GREEN, 0);
      return ;
    }
  
  double angle = 2 * M_PI * mangel->shield_angle / 360.0;
  double width = 2 * M_PI * mangel->shield_width / 360.0;
  for (int i = 0; i <= mangel->shield_thickness; ++i)
    for (double w = angle - width / 2; w < angle + width / 2; w += M_PI / 200)
      {
	t_bunny_position	p =
	  {
	   .x = cos(w) * (mangel->shield_radius + i) + pos.x,
	   .y = sin(w) * (mangel->shield_radius + i) + pos.y
	  };
	
	bunny_set_pixel(&mangel->screen->clipable.buffer, p, GREEN);
      }
}

static void		project(t_mangel		*mangel,
				t_bunny_position	*c,
				int			z)
{
  double		focal = 1000;

  if (focal == -z)
    {
      project(mangel, c, focal + 0.01);
      return ;
    }
  c->x -= mangel->screen->clipable.buffer.width / 2;
  c->y -= mangel->screen->clipable.buffer.height / 2;
  c->x = (c->x) * (focal / (focal + z));
  c->y = (c->y) * (focal / (focal + z));
  c->x += mangel->screen->clipable.buffer.width / 2;
  c->y += mangel->screen->clipable.buffer.height / 2;
}

static void		set_line(t_mangel		*mangel,
				 t_bunny_position	*pos,
				 unsigned int		*col,
				 int			_3d)
{
  t_bunny_pixelarray	*screen = mangel->screen;

  bunny_set_line(&screen->clipable.buffer, pos, col);
  if (_3d)
    {
      t_bunny_position	p[2] = {pos[0], pos[1]};

      project(mangel, &p[0], _3d);
      project(mangel, &p[1], _3d);
      bunny_set_line(&screen->clipable.buffer, &p[0], col);

      t_bunny_position	q[2];

      q[0].x = p[0].x;
      q[0].y = p[0].y;
      q[1].x = pos[0].x;
      q[1].y = pos[0].y;
      bunny_set_line(&screen->clipable.buffer, &q[0], col);

      q[0].x = p[1].x;
      q[0].y = p[1].y;
      q[1].x = pos[1].x;
      q[1].y = pos[1].y;
      bunny_set_line(&screen->clipable.buffer, &q[0], col);
    }
}			       

static void		draw_square(t_mangel		*mangel,
				    int			x,
				    int			y,
				    unsigned int	*col,
				    unsigned int	back_color,
				    int			_3d,
				    unsigned int	size)
{
  t_bunny_position	pos[2];

  if (back_color != 0 && false)
    {
      mangel->screen->clipable.clip_x_position = x;
      mangel->screen->clipable.clip_y_position = y;
      mangel->screen->clipable.clip_width = mangel->tile_size.x;
      mangel->screen->clipable.clip_height = mangel->tile_size.y;
      bunny_fill(&mangel->screen->clipable.buffer, back_color);
      mangel->screen->clipable.clip_x_position = 0;
      mangel->screen->clipable.clip_y_position = 0;
      mangel->screen->clipable.clip_width = mangel->screen->clipable.buffer.width;
      mangel->screen->clipable.clip_height = mangel->screen->clipable.buffer.height;
    }
  
  pos[0].x = x;
  pos[0].y = y;
  pos[1].x = x;
  pos[1].y = y + size;
  set_line(mangel, pos, col, _3d);
  
  pos[1].x = x + size;
  pos[1].y = y;
  set_line(mangel, pos, col, _3d);

  pos[0].x = x + size;
  pos[0].y = y + size;
  pos[1].x = x + size;
  pos[1].y = y;
  set_line(mangel, pos, col, _3d);

  pos[1].x = x;
  pos[1].y = y + size;
  set_line(mangel, pos, col, _3d);
}

static void		floor_tile(t_mangel		*mangel,
				   int			x,
				   int			y)
{
  unsigned int		col[2] = {GRAY(128), GRAY(128)};

  draw_square(mangel, x + 2, y + 2, col, ALPHA(128, GRAY(32)), 0, mangel->tile_size.x - 4);
}

static void		walkable_floor_tile(t_mangel		*mangel,
					    int			x,
					    int			y)
{
  unsigned int		col[2] = {WHITE, WHITE};

  draw_square(mangel, x + 2, y + 2, col, ALPHA(128, GRAY(32)), 0, mangel->tile_size.x - 4);
}

static void		wall_tile(t_mangel		*mangel,
				  int			x,
				  int			y)
{
  unsigned int		col[2] = {WHITE, WHITE};
  t_bunny_position	pos[2];

  //draw_square(mangel, x, y, col, ALPHA(128, GRAY(32)), 1);
  pos[0].x = x;
  pos[0].y = y;
  pos[1].x = x + mangel->tile_size.x;
  pos[1].y = y + mangel->tile_size.y;
  set_line(mangel, pos, col, -500);

  pos[0].x = x + mangel->tile_size.x;
  pos[0].y = y;
  pos[1].x = x;
  pos[1].y = y + mangel->tile_size.y;
  set_line(mangel, pos, col, -500);
}

static void		mine_tile(t_mangel		*mangel,
				  int			x,
				  int			y)
{
  unsigned int		col[2] = {RED, RED};
  t_bunny_position	pos[2];

  //draw_square(mangel, x, y, col, ALPHA(128, GRAY(32)), 1);
  pos[0].x = x;
  pos[0].y = y;
  pos[1].x = x + mangel->tile_size.x;
  pos[1].y = y + mangel->tile_size.y;
  set_line(mangel, pos, col, cos(bunny_get_current_time() * 10) * 75);

  pos[0].x = x + mangel->tile_size.x;
  pos[0].y = y;
  pos[1].x = x;
  pos[1].y = y + mangel->tile_size.y;
  set_line(mangel, pos, col, cos(bunny_get_current_time() * 10.0) * 75);
}

static void		canon_tile(t_mangel		*mangel,
				   int			x,
				   int			y)
{
  unsigned int		col[2] = {RED, RED};
  t_bunny_position	pos[2];
  double		rot = bunny_get_current_time() * 5;

  pos[0].x = x / mangel->tile_size.x;
  pos[0].y = x / mangel->tile_size.y;
  for (size_t i = 0; i < mangel->last_canon; ++i)
    if (mangel->canons[i].position.x - pos[0].x < 0.9 && mangel->canons[i].position.y - pos[0].x < 0.9)
      {
	rot *= 4 - mangel->canons[i].life;
	break ;
      }
  
  pos[0].x = x + mangel->tile_size.x / 2 + cos(rot) * mangel->tile_size.x / 2;
  pos[0].y = y + mangel->tile_size.x / 2 + sin(rot) * mangel->tile_size.x / 2;
  pos[1].x = x + mangel->tile_size.x / 2 + cos(M_PI + rot) * mangel->tile_size.x / 2;
  pos[1].y = y + mangel->tile_size.y / 2 + sin(M_PI + rot) * mangel->tile_size.x / 2;
  set_line(mangel, pos, col, -100);
}

static void		energy_tile(t_mangel		*mangel,
				    int			x,
				    int			y)
{
  unsigned int		col[2] = {GREEN, GREEN};
  t_bunny_position	pos[2];

  for (double r = 0; r < 2 * M_PI; r += 2 * M_PI / 30)
    {
      pos[0].x = cos(r) * mangel->tile_size.x / 2 + x + mangel->tile_size.x / 2;
      pos[0].y = sin(r) * mangel->tile_size.y / 2 + y + mangel->tile_size.y / 2;
      pos[1].x = cos(r + 2 * M_PI / 30) * mangel->tile_size.x / 2 + x + mangel->tile_size.x / 2;
      pos[1].y = sin(r + 2 * M_PI / 30) * mangel->tile_size.y / 2 + y + mangel->tile_size.y / 2;
      set_line(mangel, pos, col, -25);
      set_line(mangel, pos, col, cos(bunny_get_current_time() * 5) * 50 + 50);
      set_line(mangel, pos, col, -75);
      set_line(mangel, pos, col, -100);
    }
}


static void		hole_tile(t_mangel		*mangel,
				  int			x,
				  int			y)
{
  (void)mangel;
  (void)x;
  (void)y;
  //draw_square(mangel, x, y, col, ALPHA(128, BLACK), -1);
}

static void		end_tile(t_mangel		*mangel,
				 int			x,
				 int			y)
{
  unsigned int		col[2] = {rand() | BLACK, rand() | BLACK};

  draw_square(mangel, x + 1, y + 1, col, ALPHA(128, BLACK), 0, mangel->tile_size.x - 2);
  draw_square(mangel, x + 2, y + 2, col, ALPHA(128, BLACK), 0, mangel->tile_size.x - 4);
  draw_square(mangel, x + 3, y + 3, col, ALPHA(128, BLACK), 0, mangel->tile_size.x - 6);
}

typedef void		(*t_display_tile)(t_mangel	*mangel,
					  int		x,
					  int		y);

const t_display_tile	gl_display_tile[] =
  {
   hole_tile,
   floor_tile,
   walkable_floor_tile,
   wall_tile,
   end_tile,
   mine_tile,
   canon_tile,
   energy_tile
  };

t_bunny_response	ingame_display(t_mangel		*mangel)
{
  int			left;
  int			right;
  int			top;
  int			bottom;

  bunny_fill(&mangel->screen->clipable.buffer, ALPHA(128, BLACK));
  
  left = (mangel->guy.x - mangel->screen->clipable.buffer.width / 2) / mangel->tile_size.x;
  right = (mangel->guy.x + mangel->screen->clipable.buffer.width / 2) / mangel->tile_size.x;
  top = (mangel->guy.y - mangel->screen->clipable.buffer.height / 2) / mangel->tile_size.y;
  bottom = (mangel->guy.y + mangel->screen->clipable.buffer.height / 2) / mangel->tile_size.y;


  
  for (int x = left; x <= right; ++x)
    for (int y = top; y <= bottom; ++y)
      if (x < 0 || x >= mangel->map_size.x || y < 0 || y >= mangel->map_size.y)
	continue ;
      else
	{
	  int		tile = mangel->map[y * mangel->map_size.x + x];
	  int		px = x * mangel->tile_size.x - mangel->guy.x + mangel->screen->clipable.buffer.width / 2;
	  int		py = y * mangel->tile_size.y - mangel->guy.y + mangel->screen->clipable.buffer.height / 2;

	  if (tile < 0 || NBRCELL(gl_display_tile) <= tile)
	    continue ;
	  gl_display_tile[tile](mangel, px, py);
	}

  t_bunny_position	guy_on_screen =
    {
     .x = mangel->screen->clipable.buffer.width / 2,
     .y = mangel->screen->clipable.buffer.height / 2
    };
  t_bunny_position	guy_size =
    {
     .x = mangel->tile_size.x / 4,
     .y = mangel->tile_size.y / 4
    };
  if (mangel->guy_life > 0)
    {
      bunny_set_disk(&mangel->screen->clipable.buffer, guy_on_screen, guy_size, WHITE, WHITE, 0);
      guy_size.x -= 2;
      guy_size.y -= 2;
      bunny_set_disk(&mangel->screen->clipable.buffer, guy_on_screen, guy_size, BLACK, BLACK, 0);
      draw_arc(mangel);
    }

  //
  for (size_t b = 0; b < mangel->last_bullet; ++b)
    {
      /// TRAIT COMPLET SANS BOUCLIER
      t_bunny_accurate_position bpos =
	{
	 (mangel->bullet[b].original_position.x + 0.5) * mangel->tile_size.x,
	 (mangel->bullet[b].original_position.y + 0.5) * mangel->tile_size.y
	};
      volatile t_bunny_position	pos[2] =
	{
	 {
	  (mangel->bullet[b].original_position.x + 0.5) * mangel->tile_size.x,
	  (mangel->bullet[b].original_position.y + 0.5) * mangel->tile_size.y
	 },
	 {mangel->guy.x, mangel->guy.y}
	};
      volatile t_bunny_position	diff =
	{
	 pos[1].x - pos[0].x,
	 pos[1].y - pos[0].y
	};
      double		angle;
      t_bunny_position	lin[2];
      unsigned int	color[2] = {RED, RED};
      
      pos[1].x = pos[0].x + diff.x * mangel->bullet[b].dist;
      pos[1].y = pos[0].y + diff.y * mangel->bullet[b].dist;

      // on regarde si le tir est legal... meme si bon, une fois passé la verif...
      lin[0].x = pos[1].x / mangel->tile_size.x;
      lin[0].y = pos[1].y / mangel->tile_size.y;
      int xxx = mangel->map[lin[0].x + lin[0].y * mangel->map_size.x];

      if (xxx == WALL) // On tue le tir is ilse prend un mur
	mangel->bullet[b].delay = 0;
      angle = atan2(pos[1].y - pos[0].y, pos[1].x - pos[0].x);
      angle = (int)(180.0 * angle / M_PI + 180.0) % 360;
      t_bunny_accurate_position impact = {pos[1].x, pos[1].y};
      double rdist = mangel->bullet[b].dist;
      
      if (distance(mangel->guy, impact) < mangel->shield_radius)
	{
	  // printf("Angle is : %d - Shield angle is : %d\n", (int)angle, (int)mangel->shield_angle);
	  if (angle_diff(angle, mangel->shield_angle) < mangel->shield_width / 2
	      && mangel->shield_detached == false)
	    {
	      rdist = mangel->shield_radius / distance(mangel->guy, bpos);
	      pos[1].x = pos[0].x + diff.x * (1.0 - rdist);
	      pos[1].y = pos[0].y + diff.y * (1.0 - rdist);
	      fire_particles(mangel, GREEN, 3, pos[1]);
	      if ((mangel->shield_energy -= 10) < 0)
		mangel->shield_energy = 0;
	    }
	  else
	    {
	      t_bunny_position g = {mangel->guy.x, mangel->guy.y};
	      fire_particles(mangel, RED, 30, g);
	      if ((mangel->guy_life -= 10) < 0)
		mangel->delay = bunny_get_frequency() * 2;
	      
	      if (rand() % 2)
		mangel->shake += rand() % 5 + 5;
	      else
		mangel->shake += -rand() % 5 - 5;
	    }
	}

      lin[0].x = pos[0].x - mangel->guy.x + mangel->screen->clipable.buffer.width / 2;
      lin[0].y = pos[0].y - mangel->guy.y + mangel->screen->clipable.buffer.height / 2;
      lin[1].x = pos[1].x - mangel->guy.x + mangel->screen->clipable.buffer.width / 2;
      lin[1].y = pos[1].y - mangel->guy.y + mangel->screen->clipable.buffer.height / 2;
      bunny_set_line(&mangel->screen->clipable.buffer, lin, color);
    }
  
  //
  for (int p = 0; p < mangel->last_particle; ++p)
    {
      t_bunny_position	pos = {mangel->particles[p].pos.x, mangel->particles[p].pos.y};

      pos.x = pos.x - mangel->guy.x + mangel->screen->clipable.buffer.width / 2;
      pos.y = pos.y - mangel->guy.y + mangel->screen->clipable.buffer.height / 2;
      bunny_set_pixel(&mangel->screen->clipable.buffer, pos, mangel->particles[p].color);
    }

  bunny_clear(&mangel->window->buffer, BLACK);
  double		scale = mangel->screen->clipable.scale.x;

  mangel->screen->clipable.scale.x = mangel->screen->clipable.scale.y = scale * 2;
  mangel->screen->clipable.color_mask.full = ALPHA(64, WHITE);
  bunny_blit(&mangel->window->buffer, &mangel->screen->clipable, NULL);

  t_bunny_position	spos =
    {
     mangel->screen->clipable.position.x,
     mangel->screen->clipable.position.y
    };
  if (mangel->shake > 0.1)
    {
      spos.x += cos(bunny_get_current_time() * 50) * mangel->shake;
      mangel->screen->clipable.rotation = mangel->shake * cos(bunny_get_current_time() * 25);
    }
  else
    mangel->screen->clipable.rotation = 0;
  mangel->screen->clipable.scale.x = mangel->screen->clipable.scale.y = scale;
  mangel->screen->clipable.color_mask.full = WHITE;
  bunny_blit(&mangel->window->buffer, &mangel->screen->clipable, &spos);

  mangel->pencil->position.x = mangel->window->buffer.width * 0.01;
  mangel->pencil->position.y = mangel->window->buffer.height * 0.01;
  mangel->pencil->clip_height = mangel->window->buffer.height * 0.05;
  mangel->pencil->clip_width = mangel->window->buffer.width * 0.33 * mangel->guy_life / 1000.0;
  mangel->pencil->color_mask.full = RED;
  if (mangel->guy_life > 0)
    bunny_blit(&mangel->window->buffer, mangel->pencil, NULL);

  mangel->pencil->position.y = mangel->window->buffer.height * 0.02 + mangel->pencil->clip_height;
  mangel->pencil->clip_width = mangel->window->buffer.width * 0.33 * mangel->shield_energy / 1000.0;
  mangel->pencil->color_mask.full = GREEN;
  if (mangel->shield_energy > 0)
    bunny_blit(&mangel->window->buffer, mangel->pencil, NULL);

  mangel->pencil->position.y = mangel->window->buffer.height * 0.03 + mangel->pencil->clip_height * 2;
  mangel->pencil->clip_width = mangel->window->buffer.width * 0.33 * mangel->shield_consumption / 20;
  mangel->pencil->color_mask.argb[GREEN_CMP] = 128;
  bunny_blit(&mangel->window->buffer, mangel->pencil, NULL);
  
  bunny_display(mangel->window);
  return (GO_ON);
}


