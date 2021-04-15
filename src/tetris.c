/* tetris.c
 *
 * Copyright 2021 Niklas
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include "tetris.h"


void piece_rotate(Piece *piece) {
  int x,y,i;
  int w = piece->width, h = piece->height;

  unsigned short copy[piece->width * piece->height];

  for (i = 0; i<piece->width * piece->height; i++) {
    copy[i] = *(piece->field + i);
  }

  for (x = 0; x < piece->width; x++)
    for (y = 0; y < piece->height; y++)
      (piece->field[x + y * w]) = copy[y + (w - x - 1) * w];
}

void rotate_piece(unsigned short *(piece)) {
  int i;
  unsigned short p[9];

  for (i = 0; i<9; i++) {
    p[i] = *(piece + i);
  }

  for (i = 0; i < 3; i++) {
    piece[6 + i] = p[3 * i];
    piece[3 + i] = p[3 * i + 1];
    piece[i] = p[3 * i + 2];
  }
}

void piece_copy_to(Piece *copy, Piece *duplicate) {
  duplicate->height = copy->height;
  duplicate->width = copy->width;
  int i;
  for (i = 0; i < copy->height * copy->width; i++) {
    duplicate->field[i] = copy->field[i];
  }
}

void print_piece(unsigned short *(piece)) {
  int i;
  for (i = 0; i<3; i++) {
    printf("%d %d %d\n", *(piece + i * 3), *(piece + i * 3 + 1), *(piece +i * 3 + 2));
  }
}

void array_print(unsigned short *map, int width, int height) {
  int y,x;
  for (y = 0; y<height; y++) {
    for (x = 0; x<width; x++) {
      printf ("%d_",*(map + y * width + x));
    }
    printf("\n");
  }
}

void piece_print(Piece *piece) {
  int y,x;
  int w = piece->width;
  int h = piece->height;
  unsigned short *f = piece->field;

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      printf ("%d_",*(f + y * w + x));
    }
    printf("\n");
  }
}

void tetris_add_piece(Tetris *tetris, Piece *piece) {
  int pos_x = tetris->piece_pos_x;
  int pos_y = tetris->piece_pos_y;
  int width = piece->width;
  int height = piece->height;

  unsigned short *p = piece->field;

  int w, h;
  for (h = 0; h < height; h++) {
    for (w = 0; w < width; w++) {
      if (*(p + h * width + w))
        tetris->field[h + pos_y][w + pos_x] = *(p + h * width + w);
    }
  }
}

void tetris_clear_field(Tetris *tetris) {
  int x,y;

  for (y = 0; y < tetris->height; y++) {
    for (x = 0; x < tetris->width; x++) {
      tetris->field[y][x] = 0;
    }

  }
}

//TODO: use tetris width, height
// returns 0: game over, 1: nothing to clear, >1: amount of lines cleared + 1
int tetris_clear_line (Tetris *tetris) {
  int lines_cleared = 1;

  unsigned short *f = &(tetris->field[0][0]);

  int x = 0;
  int y = 0;


  for (; y < 2; y++ ) {
    for (x = 0; x < COLS; x++) {
      //printf("(w,h)[%d][%d] =>%d\n", y, x, *(f + x + y * COLS));
      if( *(f + x + y * COLS) != 0)
        return 0;
    }
  }

  int xx, yy;

  for (y = 2; y < ROWS; y ++) {
    for (x = 0; x < COLS; x++) {
      if (0 == *(f + x + y * COLS))
        break;
      else if (x == COLS - 1) {
        // clear line
        for (yy = y; yy > 0; yy--) {
          for (xx = 0; xx < COLS; xx++) {
            *(f + xx + yy * COLS)  = *(f + xx + (yy - 1) * COLS);
          }
        }
        lines_cleared ++;
      }
    }
  }

  return lines_cleared;
}

//TODO: use tetris width, height
// returns 0:valid, 1:invalid, 2:collision
int tetris_possible_piece_position(Tetris *tetris, Piece *piece) {
  unsigned int piece_x = tetris->piece_pos_x;
  unsigned int piece_y = tetris->piece_pos_y;
  int piece_width = piece->width;
  int piece_height = piece->height;
  unsigned short *f = piece->field;

  int ret = 0;

  int x,y;
  int in_grid_x, in_grid_y, is_null;

 // printf("pos_x:%d ,pos_y:%d ,width:%d ,height:%d\n", piece_x, piece_y, piece_width, piece_height);

  for (y = 0; y < piece_height; y ++) {
    for (x = 0; x < piece_width; x ++) {
      in_grid_x = ((piece_x + x) >= 0 && (piece_x + x) < COLS) ? 1 : 0;
      in_grid_y = ((piece_y + y) >=0 && (piece_y + y) < ROWS) ? 1 : 0;
      is_null = ( *(f + y * piece_width + x) == 0) ? 1 : 0;
     // printf("(h,w)[%d][%d]=>x:%d ,y:%d , n:%d \n", y, x, in_grid_x, in_grid_y, is_null);
      if (is_null || (in_grid_x && in_grid_y) ) {
        if (!is_null && (tetris->field[piece_y + y][piece_x + x] != 0))
          ret = 2;
        continue;
      }
      else
        return (piece_y + y == ROWS) ? 2 : 1;
    }
  }

  return ret;
}
/*
int main (int argc, char *argv[])
{

  unsigned short p2[9] = {0,1,2,3,4,5,6,7,8};
  unsigned short p[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
  //unsigned short f[20][10] =
  int a = 5;
  int *pa = &a;
  //int map[5][3] = {{1,2,3},{2,4,6},{9,8,7},{3,6,9},{0,1,0}};

  Piece *piece_1 = (Piece*) malloc (sizeof (Piece));
  piece_1->field = &p[0][0];
  piece_1->width = 3;
  piece_1->height = 3;

  piece_print (piece_1);

  Tetris *tetris = (Tetris*) malloc (sizeof (Tetris));
  tetris->field[2][3] = 9;
  tetris->piece_pos_x = 1;
  tetris->piece_pos_y = 1;

  array_print ((unsigned short*)&(tetris->field[0][0]), COLS, ROWS);
  printf("\n");
  tetris->piece_pos_y = 2;
  tetris->piece_pos_x = 0;
  add_piece(tetris, piece_1);
  tetris->piece_pos_x = 3;
  add_piece(tetris, piece_1);
  tetris->piece_pos_x = 6;
  add_piece(tetris, piece_1);
  tetris->piece_pos_x = 7;
  add_piece (tetris, piece_1);
  //tetris->field[2][5] = 0;

  array_print ((unsigned short*)&(tetris->field[0][0]), COLS, ROWS);


  print_piece (p2);
  rotate_piece (p2);
  print_piece (p2);
  printf("\n");
  //piece_1->height = 6;
  tetris->piece_pos_y = 1;

  //printf("%d\n", tetris_possible_piece_position (tetris, piece_1));
  //unsigned short *ttt = tetris_get_snapshot (tetris, piece_1);
  //printf("%d  %d<>%d\n", *(ttt + 13), ttt, &tetris->field[0][0]);

  printf(">>%d\n", tetris_clear_line (tetris));


  array_print(&(tetris->field[0][0]), 10, 20);



 // free(ttt);
  free (tetris);
  free (piece_1);

  return EXIT_SUCCESS;
}
*/
