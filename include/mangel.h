/*
**
**
**
**
*/

#ifndef				__MANGEL_H__
# define			__MANGEL_H__
# include			"tekf.h"

typedef enum			e_tile
  {
   HOLE,
   FLOOR,
   WALKABLE_FLOOR,
   WALL,
   END_TILE,
   MINE,
   CANON,
   ENERGY
  }				t_tile;

typedef enum			e_dir
  {
   GORIGHT,
   GOLEFT,
   GOUP,
   GODOWN,
   WAIT
  }				t_dir;

typedef struct			s_particle
{
  unsigned int			color;
  t_bunny_accurate_position	pos;
  t_bunny_accurate_position	speed;
  int				delay;
}				t_particle;

typedef struct			s_move
{
  int				direction;
  double			repeat;
}				t_move;

typedef struct			s_canon
{
  t_bunny_accurate_position	position;
  int				delay;
  int				life;
}				t_canon;

typedef struct			s_move_list
{
  t_move			moves[1024 * 1024];
  size_t			current_move;
  size_t			move_length;
}				t_move_list;

typedef struct			s_bullet
{
  double			dist;
  double			delay;
  t_bunny_accurate_position	original_position;
  double			width;
}				t_bullet;

typedef struct			s_mangel
{
  t_bunny_window		*window;
  t_bunny_pixelarray		*screen;
  t_bunny_picture		*pencil;
  int				context;
  t_bunny_position		tile_size;
  double			difficulty;
  char				start[0];
  t_bunny_position		map_size;
  t_move_list			move_list;
  t_canon			canons[512];
  size_t			last_canon;
  t_bullet			bullet[512];
  size_t			last_bullet;
  int				*map;
  t_bunny_accurate_position	end_position;
  t_bunny_accurate_position	guy;
  bool				shield_detached;
  t_bunny_accurate_position	shield_position;
  double			shield_radius;
  double			shield_angle;
  double			shield_width;
  double			shield_thickness;
  double			shield_energy;
  double			shield_consumption;
  int				delay;
  int				guy_life;
  t_particle			particles[1024 * 1024];
  int				last_particle;
  double			shake;
  int				slow;
}				t_mangel;

double				distance(t_bunny_accurate_position	a,
					 t_bunny_accurate_position	b);
double				distancen(t_bunny_accurate_position	a,
					 t_bunny_accurate_position	b);
void				generate_map(t_mangel			*mangel,
					     double			difficulty);
void				fire_particles(t_mangel			*mangel,
					       unsigned int		color,
					       unsigned int		nbr,
					       t_bunny_position		where);
void				fire_bullet(t_mangel			*mangel,
					    t_bunny_accurate_position	opos,
					    double			width);

void				move_particles(t_mangel			*mangel);
void				move_shot(t_mangel			*mangel);

#endif	/*			__MANGEL_H__		*/

