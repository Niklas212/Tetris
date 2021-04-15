#define ROWS  20
#define COLS  10

typedef struct {
  int width;
  int height;
  unsigned short *field;
} Piece;

typedef struct{
  int width;
  int height;
  unsigned int piece_pos_x;
  unsigned int piece_pos_y;
  unsigned short field[ROWS][COLS];
} Tetris;

typedef struct {
  unsigned short **field;
  int width, height;
} Shape;

enum {none, cyan, blue, orange, yellow, green, purple, red};


void piece_copy_to(Piece *copy, Piece *duplicate);
void piece_rotate (Piece *piece);
void tetris_add_piece(Tetris *tetris, Piece *piece);
void tetris_clear_field(Tetris *tetris);
int tetris_clear_line(Tetris *tetris);
int tetris_possible_piece_position(Tetris *tetris, Piece *piece);
