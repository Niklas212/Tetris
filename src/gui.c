#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tetris.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define TOP_MARGIN      50
#define GRID_PADDING    8
#define GAME_SPEED      2

const ALLEGRO_COLOR grid_background = {0.6, 0.6, 0.6, 1.0};
const ALLEGRO_COLOR grid_margin_color = {0.4, 0.4, 0.4, 1.0};

const char *font_path = "res/fonts/Roboto-Regular.ttf";

const ALLEGRO_COLOR grid_colors[8] = {
  {1.0, 1.0, 1.0, 1.0},  //white
  {0.0, 0.8, 0.8, 1.0},  //cyan
  {0.0, 0.0, 0.8, 1.0},  //blue
  {0.8, 0.4, 0.0, 1.0},  //orange
  {0.8, 0.8, 0.0, 1.0},  //yellow
  {0.0, 0.8, 0.0, 1.0},  //green
  {0.8, 0.0, 0.8, 1.0},  //purple
  {0.8, 0.0, 0.0, 1.0},  //red
};

const Piece piece_list[7] = {
  {4, 4, (unsigned short[16]){0, 0, 0, 0, cyan, cyan, cyan, cyan }},
  {3, 3, (unsigned short[9]){blue, 0, 0, blue, blue, blue}},
  {3, 3, (unsigned short[9]){0, 0, orange, orange, orange, orange}},
  {2, 2, (unsigned short[]){yellow, yellow, yellow, yellow}},
  {3, 3, (unsigned short[9]){0, green, green, green, green}},
  {3, 3, (unsigned short[9]){0, purple, 0, purple, purple, purple}},
  {3, 3, (unsigned short[9]){red, red, 0, 0, red, red}},
};

struct Layout{
  ALLEGRO_FONT *game_over_font, *score_font;
  int grid_size, margin_left, margin_top, width, height;
} layout;



void update_layout_sizes(ALLEGRO_DISPLAY *display) {

  layout.width = al_get_display_width(display);
  layout.height = al_get_display_height (display);

  float px = (layout.width - GRID_PADDING) / COLS;
  float py = (layout.height - TOP_MARGIN - GRID_PADDING) / ROWS;
  layout.grid_size = (px >= py) * py + (py > px) * px;

  layout.margin_left = (layout.width - layout.grid_size * COLS) / 2;
  layout.margin_top = (layout.height - layout.grid_size * ROWS - TOP_MARGIN) / 2 + TOP_MARGIN;

  al_destroy_font (layout.game_over_font);
  layout.game_over_font = al_load_font (font_path, layout.grid_size * 0.8, 0);

  al_destroy_font (layout.score_font);
  layout.score_font = al_load_font (font_path, layout.grid_size * 0.7, 0);
}

int get_score(unsigned int amount) {
  int result = 0;
  int plus = 0;
  int i;

  for (i = 0; i < amount; i++)
    result += plus += 10;

  return result;
}

int main (int argc, char *argv[])
{
  //intialise random
  srand(time(NULL));

  al_init();
  al_init_primitives_addon ();
  al_init_font_addon ();
  al_init_ttf_addon ();

  Tetris *tetris = calloc (sizeof (Tetris), 1);
  tetris->piece_pos_x = 4;
  tetris->piece_pos_y = 0;
  tetris->height = ROWS;
  tetris->width = COLS;

  Piece *piece = malloc (sizeof (Piece));
  unsigned short *ptr = calloc (sizeof (unsigned short), 16);
  piece->field = ptr;

  piece_copy_to (&piece_list[rand() % 7], piece);

  ALLEGRO_FONT *font = al_load_font (font_path, 18, 0);

  al_set_new_display_flags (ALLEGRO_WINDOWED|ALLEGRO_RESIZABLE);

  ALLEGRO_DISPLAY *display = al_create_display (308, 658);
  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue ();
  ALLEGRO_TIMER *game_timer = al_create_timer (1.0 / GAME_SPEED);

  al_install_keyboard ();
  al_install_mouse ();

  al_register_event_source (queue, al_get_keyboard_event_source());
  al_register_event_source (queue, al_get_display_event_source (display));
  al_register_event_source (queue, al_get_timer_event_source (game_timer));
  al_register_event_source (queue, al_get_mouse_event_source ());


  al_set_window_title (display, "TETRIS");
  al_set_app_name ("TETRIS");

  update_layout_sizes (display);

  int need_draw = 1;
  int running = 1;
  int score = 0;
  int game_over = 0;

  ALLEGRO_EVENT event;

  al_start_timer (game_timer);


  while (running) {

    if (need_draw && al_is_event_queue_empty (queue)) {
      al_clear_to_color (al_map_rgb (180, 180, 180));

      int grid_size = layout.grid_size;
      int x = layout.margin_left;
      int y = layout.margin_top;

      //grid background
      al_draw_filled_rectangle (x, y, grid_size * COLS + x, grid_size * ROWS + y, grid_background);


      int i,j;

      //draw grid
      for (i = 0; i < ROWS; i ++)
        for (j = 0; j < COLS; j++) {
          int x = layout.margin_left + j * grid_size + (float) grid_size * 0.05;
          int y = layout.margin_top + i * grid_size + (float) grid_size * 0.05;
          al_draw_filled_rectangle(x, y, x + (float) grid_size * 0.9, y + (float) grid_size * 0.9, grid_colors[tetris->field[i][j]]);
        }


      if (!game_over) {
        //draw text
        al_draw_textf (font, al_map_rgb (0, 0, 0), layout.width / 2, 17, ALLEGRO_ALIGN_CENTER, "SCORE: %d", score);

        //draw piece
        for (i = 0; i < piece->width * piece->height; i++)
          if (piece->field[i]) {
            int pos_x = (i % piece->width);
            int pos_y = (i - pos_x) / piece->width + tetris->piece_pos_y;
            //must be here (after pos_y)
            pos_x += tetris->piece_pos_x;

            int x = layout.margin_left + pos_x * grid_size + (float) grid_size * 0.05;
            int y = layout.margin_top + pos_y *grid_size + (float) grid_size * 0.05;
            al_draw_filled_rectangle(x, y, x + (float) grid_size * 0.9, y + (float) grid_size * 0.9, grid_colors[piece->field[i]]);
          }
      }

      //grid padding
      al_draw_rectangle (x, y, grid_size * COLS + x,  grid_size * ROWS  + y , grid_margin_color, GRID_PADDING);

      if (game_over){
        // "blur"
        al_draw_filled_rectangle (0, 0, layout.width, layout.height, al_map_rgba_f (0.0, 0.0, 0.0, 0.6));
        //game over screen
        al_draw_filled_rounded_rectangle (x + grid_size - GRID_PADDING, y + 6 * grid_size - GRID_PADDING, x + 9 * grid_size + GRID_PADDING, y + 14 * grid_size + GRID_PADDING, grid_size / 2, grid_size / 2, al_map_rgba_f (0, 0, 0, 0.9));
        //game over text
        al_draw_text (layout.game_over_font, al_map_rgb(200, 0, 0), layout.width / 2, y + 6 * grid_size, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        //score
        al_draw_textf (layout.score_font, al_map_rgb_f (1, 1, 1), x + grid_size + GRID_PADDING, y + 8 * grid_size, 0, "score: %d", score);
        //button
        al_draw_rounded_rectangle (x + 2 * grid_size, y + 12.5 * grid_size, x + 8 * grid_size, y + 13.5 * grid_size, grid_size / 5, grid_size / 5, al_map_rgb_f (1, 1, 1), grid_size / 10);
        //button_text
        al_draw_text (layout.score_font, al_map_rgb_f (1, 1, 1), layout.width / 2, y + 12.5 * grid_size, ALLEGRO_ALIGN_CENTER, "restart");
      }

      al_flip_display ();

    }

    al_wait_for_event(queue, &event);

    switch(event.type) {
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        {running = 0; break;}
      case ALLEGRO_EVENT_DISPLAY_RESIZE: {
        al_acknowledge_resize (display);
        update_layout_sizes (display);
        break;
      }
      case ALLEGRO_EVENT_TIMER: {
        tetris->piece_pos_y ++;
        int status = tetris_possible_piece_position (tetris, piece);
        if (status == 1)
          tetris->piece_pos_y --;
        else if (status == 2) {
          tetris->piece_pos_y --;
          tetris_add_piece (tetris, piece);
          tetris->piece_pos_x = 4;
          tetris->piece_pos_y = 0;
          piece_copy_to (&piece_list[rand() % 7], piece);
          int s = tetris_clear_line (tetris);
          if (!s) {
            game_over = 1;
            al_stop_timer (game_timer);
          }
          else
            score +=  get_score (s - 1);
        }
        break;
      }
      case ALLEGRO_EVENT_KEY_DOWN: {
        switch(event.keyboard.keycode) {
          case ALLEGRO_KEY_LEFT: {
            tetris->piece_pos_x --;
            if (tetris_possible_piece_position (tetris, piece))
              tetris->piece_pos_x ++;
            break;
          }
          case ALLEGRO_KEY_RIGHT: {
            tetris->piece_pos_x ++;
            if (tetris_possible_piece_position (tetris, piece))
              tetris->piece_pos_x --;
            break;
          }
          case ALLEGRO_KEY_DOWN: {
            tetris->piece_pos_y ++;
            if (tetris_possible_piece_position (tetris, piece))
              tetris->piece_pos_y --;
            break;
          }
          case ALLEGRO_KEY_R: {
            piece_rotate (piece);

            if (tetris_possible_piece_position (tetris, piece))
              {piece_rotate(piece); piece_rotate(piece); piece_rotate(piece);}

            break;
          }
          case ALLEGRO_KEY_N:
            piece_copy_to (&piece_list[(rand() % 7)], piece); break;
          case ALLEGRO_KEY_SPACE: {
            score = game_over = 0;
            tetris_clear_field (tetris);
            tetris->piece_pos_x = 4;
            tetris->piece_pos_y = 0;
            piece_copy_to (&piece_list[rand() % 7], piece);
            al_start_timer (game_timer);
            break;
          }
        }
        break;
      }
      case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
        int x = event.mouse.x;
        int y = event.mouse.y;

        int in_x = x > layout.margin_left + 2 * layout.grid_size && x < layout.margin_left + 8 * layout.grid_size;
        int in_y = y > layout.margin_top + 12.5 * layout.grid_size && y < layout.margin_top + 13.5 * layout.grid_size;

        if (in_x && in_y) {
            score = game_over = 0;
            tetris_clear_field (tetris);
            tetris->piece_pos_x = 4;
            tetris->piece_pos_y = 0;
            piece_copy_to (&piece_list[rand() % 7], piece);
            al_start_timer (game_timer);
        }
        break;
      }
    }
  }

  al_destroy_display (display);
  al_destroy_timer (game_timer);
  al_destroy_font(font);
  al_destroy_font (layout.score_font);
  al_destroy_font (layout.game_over_font);
  al_destroy_event_queue (queue);
  al_uninstall_keyboard ();
  al_uninstall_mouse ();
  al_shutdown_font_addon ();
  al_shutdown_ttf_addon ();
  al_shutdown_primitives_addon ();
  free(tetris);
  free(piece->field);
  free(piece);

  return EXIT_SUCCESS;
}
  
