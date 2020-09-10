#include <stdio.h>
#include <ncurses.h>
#include <math.h>
#include <string.h>


#define HEIGHT 16
#define WIDTH 32
#define STARTY ((LINES - HEIGHT) / 2)
#define STARTX ((COLS - WIDTH) / 2)

#define QEXIT 81
#define qEXIT 113



WINDOW *create_win(int height, int width, int starty, int startx){
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0, 0);
	wrefresh(local_win);

	return local_win;
};

void destroy_win(WINDOW *local_win){
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(local_win);
	delwin(local_win);
};


void draw_list(WINDOW *num_wins[16], int arr[][4], int length){
	for(int i=0; i < length; i++){
		for(int j=0; j < length; j++){
			refresh();
			int height = HEIGHT / length, width = WIDTH / length, starty = STARTY + i*height, startx = STARTX + j*width;
			
			int index = i*4+j;
			num_wins[index] = create_win(height, width, starty, startx);

			int digity = height / 2, digitx = width / 2 - 1;
			if(arr[i][j])
				mvwprintw(num_wins[index], digity, digitx, "%d", arr[i][j]);
			else
				mvwprintw(num_wins[index], digity, digitx, " ");
			
			wrefresh(num_wins[index]);
		};
	};
};

// If I use this function to destroy original window and recreate it and sub windows, it throws a segmentation fault in wborder function in destroy_win function
// without destroyed counter, that prevents from deleting when it is deleted, it throws seg fault at second call. With destroyed, it throws it at third call
// Without using this function, it doesn't throw seg fault no matter how many times it gets called, however it is redundant.
/*void destroy_list(WINDOW *win, int starty, int startx, WINDOW *sub_wins[16], int arr[][4], int length, int *destroyed){
	if(!(*destroyed)){
		destroy_win(win);
		*destroyed = 1;
	}else
		*destroyed  = 0;

	win = create_win(HEIGHT, WIDTH, starty, startx);
	draw_list(sub_wins, arr, length);
	wrefresh(win);
};*/

// Length = 16
void switch_pos(int *arr, int length, int *x_index, int direction){		// Direction: 1 -> UP; 2 -> RIGHT; 3 -> DOWN; 4 -> LEFT
	int target_index;
	int y = *x_index / 4, x = *x_index % 4; 	// x-axis, y-axis of X sub window

	switch(direction){
		case 1:
			target_index = *x_index - 4;
			break;
		case 2:
			target_index = *x_index + 1;
			if(target_index / 4 != y)
				target_index = *x_index;
			break;
		case 3:
			target_index = *x_index + 4;
			break;
		case 4:
			target_index = *x_index - 1;
			if(target_index / 4 != y)
				target_index = *x_index;
			break;
		default:
			return;
	};

	if(target_index < 0 || target_index >= length)
		return;
	// Switch positions
	int temp = *(arr + target_index);
	*(arr + target_index) = *(arr + *x_index);
	*(arr + *x_index) = temp;
	// Update X index
	*x_index = target_index;
};

void play(){
	int arr[4][4] = {1,4,15,7,8,10,2,11,14,3,6,13,12,9,5,0};
	int length = 4;
	int x_index = 15; 	// Position of 0, blank sub window
	
	WINDOW *win;
	WINDOW *sub_wins[16];

	int starty, startx;

	//Placement
	starty = STARTY;
	startx = STARTX;	
	
	printw("Press Q or q to exit\n");
	printw("\n");

	refresh();

	// Create original window
	win = create_win(HEIGHT, WIDTH, starty, startx);

	// Create sub windows for numbers	(1st)
	draw_list(sub_wins, arr, length);

	wrefresh(win);

	int ch = getch();
	while(ch != QEXIT && ch != qEXIT){
		switch(ch){
			case KEY_UP:
				destroy_win(win);
				win = create_win(HEIGHT, WIDTH, starty, startx);

				switch_pos(arr, length * length, &x_index, 1);
				
				draw_list(sub_wins, arr, length);
				wrefresh(win);
				break;
			case KEY_RIGHT:
				destroy_win(win);
				win = create_win(HEIGHT, WIDTH, starty, startx);

				switch_pos(arr, length * length, &x_index, 2);
				
				draw_list(sub_wins, arr, length);
				wrefresh(win);
				break;
			case KEY_DOWN:
				destroy_win(win);
				win = create_win(HEIGHT, WIDTH, starty, startx);

				switch_pos(arr, length * length, &x_index, 3);
				
				draw_list(sub_wins, arr, length);
				wrefresh(win);
				break;
			case KEY_LEFT:
				destroy_win(win);
				win = create_win(HEIGHT, WIDTH, starty, startx);

				switch_pos(arr, length * length, &x_index, 4);
				
				draw_list(sub_wins, arr, length);
				wrefresh(win);
				break;

		};
		ch = getch();
	};
};

int main(void){
	printf("\n");

	// Init ncurses
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	
	// Game mechanics
	play();

	endwin();

	return 0;
};
