// Functions to draw on the screen (outside of windows)

#include "libc.h"
#include "heap.h"
#include "display.h"
#include "display_vga.h"
#include "gui_window.h"

void draw_string(const unsigned char *str, int x, int y) {
	int len = strlen(str);

	for (int i=0; i<len; i++) {
		draw_font(str[i], x, y);
		x += 8;
	}
}

void draw_string_n(const unsigned char *str, int x, int y, int len) {
	for (int i=0; i<len; i++) {
		draw_font(str[i], x, y);
		x += 8;
	}
}

void draw_string_inside_frame(const unsigned char *str, int x, int y, uint left_x, uint right_x, uint top_y, uint bottom_y) {
	int len = strlen(str);

	for (int i=0; i<len; i++) {
		draw_font_inside_frame(str[i], x, y, left_x, right_x, top_y, bottom_y);
		x += 8;
	}
}

void draw_char(unsigned char c, int x, int y) {
	draw_font(c, x, y);
}

void draw_char_inside_frame(unsigned char c, uint x, uint y, uint left_x, uint right_x, uint top_y, uint bottom_y) {
	draw_font_inside_frame(c, x, y, left_x, right_x, top_y, bottom_y);
}

void draw_hex(char b, int x, int y) {
	char* str = "0x00"; //placeholder for size
	char* loc = str++; //offset since beginning has 0x
	
	char key[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	int u = ((b & 0xf0) >> 4);
	int l = (b & 0x0f);
	*++str = key[u];
	*++str = key[l];

	//*++str = '\n'; //newline
	draw_string(loc, x, y);
}

// Prints a character in hex format, without the "0x"
void draw_hex2(char b, int x, int y) {
	char* str = "00"; //placeholder for size
	char* loc = str; //offset since beginning has 0x
	
	char key[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	int u = ((b & 0xf0) >> 4);
	int l = (b & 0x0f);
	*str++ = key[u];
	*str++ = key[l];

	//*++str = '\n'; //newline
	draw_string(loc, x, y);
}

// Prints the value of a pointer (in hex) at a particular
// row and column on the screen
void draw_ptr(void *ptr, int x, int y) {
	unsigned char *addr = (unsigned char *)&ptr;
	char *str = "0x00000000";
	char *loc = str++;

	char key[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	addr += 3;

	for (int i=0; i<4; i++) {
		unsigned char b = *addr--;
		int u = ((b & 0xf0) >> 4);
		int l = (b & 0x0f);
		*++str = key[u];
		*++str = key[l];
	}

	draw_string(loc, x, y);
}

int draw_int(int nb, int x, int y) {
	int nb_chars = 0;
	if (nb < 0) {
		draw_font('-', x, y);
		x += 8;
		nb_chars++;
		nb = -nb;
	}
	uint nb_ref = 1000000000;
	uint leading_zero = 1;
	uint digit;

	for (int i=0; i<=9; i++) {
		if (nb >= nb_ref) {
			digit = nb / nb_ref;
			draw_font('0' + digit, x, y);
			x += 8;
			nb_chars++;
			nb -= nb_ref * digit;

			leading_zero = 0;
		} else {
			if (!leading_zero) {
				draw_font('0', x, y);
				x += 8;
				nb_chars++;
			}
		}
		nb_ref /= 10;
	}	

	return nb_chars;
}


///////////////////////////////
// This "window" is when we want to print actions on
// the whole screen (for debugging purposes)
//////////////////////////////////

void gui_debug_scroll(Window *win) {
	getch();
	win->cursor_y = 0;
}

// Moves the cursor position to the next line (do not draw anything)
void gui_debug_next_line(Window *win) {
	win->cursor_x = win->left_x;
	win->cursor_y += 8;

/*	if (win->text_end % 80 > 0) {
		uint bytes_to_end = 80 - (win->text_end % 80);
		memset(win->text + win->text_end, 0, bytes_to_end);
		win->text_end += bytes_to_end;
	}
*/
	if (win->cursor_y + 8 > win->bottom_y) gui_debug_scroll(win);
}

void gui_debug_set_cursor(Window *win) { }
void gui_debug_remove_cursor(Window *win) { }
void gui_debug_draw_window_header(Window *win, int focus) { }
void gui_debug_set_focus(Window *win, Window *win_old) { }
void gui_debug_remove_focus(Window *win) { }

void gui_debug_init(Window *win, const char *title) {
/*	win->text = (char*)kmalloc(4800, 0);
	memset(win->text, 0, 4800);
	win->text_end = 0;*/
}

void gui_debug_cls(Window *win) {
	win->cursor_x = win->left_x;
	win->cursor_y = win->top_y;
//	win->text_end = 0;
}

void gui_debug_puts(Window *win, const char *msg) {
	uint substring_len;
	uint bytes_to_end;
	const char *msg_start = msg;
	uint len = strlen(msg);

	// While the line to print is larger than the number of characters left on the row
	// Cut the first part to fill the row and go to the next line
	while (win->cursor_x + len * 8 > win->right_x) {
		substring_len = (win->right_x - win->cursor_x) / 8;
		draw_string_n(msg_start, win->cursor_x, win->cursor_y,  substring_len);
//		memcpy(win->text + win->text_end, msg_start, substring_len);

		msg_start += substring_len;
//		win->text_end += substring_len;
		len -= substring_len;

		// This call updates win->cursor_x and win->cursor_y
		gui_debug_next_line(win);
	}

	draw_string(msg_start, win->cursor_x, win->cursor_y);
//	memcpy(win->text + win->text_end, msg_start, len);

//	win->text_end += len;
	win->cursor_x += len*8;
}

void gui_debug_putc(Window *win, char c) {
	draw_char(c, win->cursor_x, win->cursor_y);

	win->cursor_x += 8;
//	win->text[win->text_end++] = c;
	gui_debug_set_cursor(win);
}

void gui_debug_puti(Window *win, uint nb) {
	unsigned char *addr = (unsigned char *)&nb;
	char *str = "0x00000000";
	char *loc = str++;

	char key[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

	addr += 3;

	for (int i=0; i<4; i++) {
		unsigned char b = *addr--;
		int u = ((b & 0xf0) >> 4);
		int l = (b & 0x0f);
		*++str = key[u];
		*++str = key[l];
	}

	win->action->puts(win, loc);
}

void gui_debug_putnb(Window *win, int nb) {
	if (nb < 0) {
		win->action->putc(win, '-');
		nb = -nb;
	}
	uint nb_ref = 1000000000;
	uint leading_zero = 1;
	uint digit;

	for (int i=0; i<=9; i++) {
		if (nb >= nb_ref) {
			digit = nb / nb_ref;
			win->action->putc(win, '0' + digit);
			nb -= nb_ref * digit;

			leading_zero = 0;
		} else {
			if (!leading_zero) win->action->putc(win, '0');
		}
		nb_ref /= 10;
	}
}

void gui_debug_putnb_right(Window *win, int nb) {
	char number[12];
	uint negative = 0;
	number[11] = 0;
	number[0] = ' ';

	if (nb < 0) {
		negative = 1;
		nb = -nb;
	}
	uint nb_ref = 1000000000;
	uint leading_zero = 1;
	uint digit;

	for (int i=0; i<=9; i++) {
		if (nb >= nb_ref) {
			digit = nb / nb_ref;
			number[i+1] = '0' + digit;
			nb -= nb_ref * digit;

			if (!leading_zero && negative) number[i] = '-';
			leading_zero = 0;
		} else {
			if (!leading_zero) number[i+1] = '0';
			else number[i+1] = ' ';
		}
		nb_ref /= 10;
	}

	if (leading_zero) number[10] = '0';

	gui_debug_puts(win, number);
}

void gui_debug_backspace(Window *win) {
	win->cursor_x -= 8;
//	win->text_end--;
}

void gui_debug_putcr(Window *win) {
	gui_debug_next_line(win);
}

void gui_debug_redraw_frame(Window *win, uint left_x, uint right_x, uint top_y, uint bottom_y) { }
void gui_debug_redraw(Window *win, uint left_x, uint right_x, uint top_y, uint bottom_y) { }

struct WindowAction gui_debug_window_action = {
	.init = &gui_debug_init,
	.cls = &gui_debug_cls,
	.puts = &gui_debug_puts,
	.putc = &gui_debug_putc,
	.puti = &gui_debug_puti,
	.putnb = &gui_debug_putnb,
	.putnb_right = &gui_debug_putnb_right,
	.backspace = &gui_debug_backspace,
	.putcr = &gui_debug_putcr,
	.set_cursor = &gui_debug_set_cursor,
	.set_focus = &gui_debug_set_focus,
	.remove_focus = &gui_debug_remove_focus,
	.redraw = &gui_debug_redraw
};


Window gui_debug_win = {
	.left_x = 0,
	.right_x = 640,
	.top_y = 0,
	.bottom_y = 480,
	.action = &gui_debug_window_action
};