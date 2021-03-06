/*
**
**
**
**
*/

#include		<time.h>
#include		"mangel.h"

bunny_declare_context(ingame);

const t_bunny_context
gl_context[2] =
  {
   {},
   {
    .entering_context = ingame_entering,
    .move = ingame_move,
    .key = ingame_key,
    .loop = ingame_loop,
    .wheel = ingame_wheel,
    .display = ingame_display,
    .click = ingame_click
   }
  };

int				main(int	argc,
				     char	**argv)
{
  t_mangel			*mangel = bunny_calloc(1, sizeof(*mangel));
  const t_bunny_area		*area;
  int				seed;

  mangel->difficulty = 0; // max is 3 for generating performence reasons
  mangel->tile_size.x = 30;
  mangel->tile_size.y = 30;
  if (argc == 1)
    {
      seed = time(NULL);
      printf("Seed is %d\n", seed);
      srand(seed);
    }
  else
    srand(atoi(argv[1]));
  //srand(0);
  tekinit();
  area = bunny_list_autonomous_monitors();
  mangel->window = bunny_start(area[0].w, area[0].h, true, "Magnetic Angel");
  //mangel->window = bunny_start(1024, 768, false, "Magnetic Angel");
  mangel->pencil = bunny_new_picture(mangel->window->buffer.width, mangel->window->buffer.height);
  bunny_clear(&mangel->pencil->buffer, WHITE);
  mangel->screen = bunny_new_pixelarray(300, 300);
  mangel->screen->clipable.scale.y = floor((double)mangel->window->buffer.height / mangel->screen->clipable.buffer.height);
  mangel->screen->clipable.scale.x = mangel->screen->clipable.scale.y;
  mangel->screen->clipable.origin.x = mangel->screen->clipable.buffer.width / 2;
  mangel->screen->clipable.origin.y = mangel->screen->clipable.buffer.height / 2;
  mangel->screen->clipable.position.x = mangel->window->buffer.width / 2;
  mangel->screen->clipable.position.y = mangel->window->buffer.height / 2;
  mangel->context = 1; // 0;
  
  t_bunny_response	ret;

  do
    {
      bunny_set_context(&gl_context[mangel->context]);	
      if ((ret = bunny_loop(mangel->window, mangel->tile_size.x, mangel)) == SWITCH_CONTEXT)
	ret = GO_ON;
    }
  while (ret == GO_ON || ret == SWITCH_CONTEXT);
  bunny_free(mangel);
  if (ret == EXIT_ON_ERROR)
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}

