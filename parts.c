#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include <stdlib.h>
#include <form.h>
#include <ctype.h>
#include <assert.h>

void finish_with_error(MYSQL *con)
{
	fprintf(stderr, "%s\n",mysql_error(con));
	mysql_close(con);
	exit(1);
}

static char* trim_whitespaces(char *str)
{
	char *end;

	// trim leading space
	while(isspace(*str))
		str++;

	if(*str == 0) // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && isspace(*end))
		end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

//////////////////////////////////////////////////////////////////////////

///  Order by...0 = num, 1 = manu
	MYSQL_RES* get_data(int order_by)
{
	MYSQL *con = mysql_init(NULL);
	MYSQL_RES *res;
	if(mysql_real_connect(con, "localhost", "shop", "shop", "equip", 0, NULL, 0) == NULL)
	{
		finish_with_error(con);
	}
	if(mysql_query(con, "SELECT id, num, manu, unit, descr, location, quantity FROM parts ORDER BY num;"))
	{
		finish_with_error(con);
	}
	res = mysql_store_result(con);
	if (res == NULL)
		finish_with_error(con);
	int num_rows = mysql_num_rows(res);
	//printf("Num Rows: %d\n", num_rows);
	mysql_close(con);
	return(res);
}

WINDOW* make_pad(MYSQL_RES *result)
{
	MYSQL_ROW lrow;
	char foo[90];
	int num_rows = mysql_num_rows(result);
	WINDOW *new_pad = newpad(num_rows + 1, 85);
	while(lrow = mysql_fetch_row(result))
	{
		sprintf(foo, "%-15s%-20s%-25.20s%-14s%-8s", lrow[1], lrow[2], lrow[4], lrow[5], lrow[6]);
		wprintw(new_pad, "%s\n", foo);
	}
	return (new_pad);
}

/////////////////////////////////////////////////////////////////

void search( WINDOW *pad, WINDOW *win, int mypadpos) {

				WINDOW *search_win = newwin(3,96,31,2);
				keypad(search_win, true);
				int ch;
				box(search_win, 0, 0);
				mvwprintw(search_win, 1, 1, "howdy");
				wrefresh(search_win);
				//ch = wgetch(search_win);
				while((ch = getch()) != KEY_F(2)){
	       			switch(ch) {
					default:
						putchar(ch);
						break;

				}
			}
			//	mvwprintw(win, max_rows - 3, 4, search_term);
				delwin(search_win);	
				touchwin(stdscr);
				touchwin(win);

				keypad(pad, TRUE);
				wmove(pad, 2, 2);
				refresh();
				wrefresh(win);
				prefresh(pad, mypadpos, 0, 6, 5, 28, 80);
				

				return;
	//
}

////////////////////////////////////////////////////////////////////
//Insert Update
void insert_update(char *sql) {

	MYSQL *con2 = mysql_init(NULL);
	if(mysql_real_connect(con2, "localhost", "shop", "shop", "equip", 0, NULL, 0) == NULL)
	{
		finish_with_error(con2);
	}

	if(mysql_query(con2, sql))
	{
		finish_with_error(con2);
	}
	int i = mysql_affected_rows(con2);
	char r[40];
	int ch;
	sprintf(r, "%d rows affected", i);
	WINDOW *ok_win = newwin(5,20,20,20);
	//keypad(ok_win, true);
	box(ok_win, 0, 0);
	mvwprintw(ok_win, 2, 2, r);
	wrefresh(ok_win); 
	mysql_close(con2);
	keypad(ok_win, true);
	//while((ch = wgetch(ok_win)) != KEY_F(2)) // OK Window
	       //switch(ch)
	       //{

	      // };	       
	      ch = getch();
       delwin(ok_win);
	return;
}

/////////////////////////////////////////////////////////////////////////

int main()
{
	MYSQL_RES *result;
	result = get_data(0);
	MYSQL_ROW row;
	WINDOW *win, *popwin, *pad, *search_win;
	int max_cols, max_rows;

	initscr();
	//atexit(quit);
	clear();
	cbreak();
	noecho();
	getmaxyx(stdscr, max_rows, max_cols);
	keypad(stdscr, true);
	wattron(stdscr,A_BOLD);
	mvwprintw(stdscr,1,30,"Rhodes/Duncan Crane Parts");
	wattroff(stdscr,A_BOLD);
	win = newwin(max_rows -4, max_cols-4, 2,2); // 30, 90 rows,cols, x, y
	box(win,0,0);
	//scrollok(win, TRUE);
	//idlok(win, TRUE);
	refresh();
	wrefresh(win);
	int ch;
	int x = 2;
	int count = 2;
	char foo[90];
	char search_term[50];
	char sql[400];
	int cur_x = 0;
	int cur_y = 0;
	int form_rows = 0;
	int form_cols = 0;
	int num_rows;
	wattron(win, A_BOLD);
	mvwprintw(win, 2, 4, "%-15s%-20s%-25s%-10s%-5s", "Number", "Manufacturer", "Description", "Location", "Quantity");
	wattroff(win, A_BOLD);
	pad = make_pad(result);
	num_rows = mysql_num_rows(result);
/*	pad = newpad(num_rows + 1, 85);
	while(row = mysql_fetch_row(result))
	{
		sprintf(foo, "%-15s%-20s%-25.20s%-14s%-8s", row[1], row[2], row[4], row[5], row[6]);
		wprintw(pad, "%s\n", foo);
	}*/
	int mypadpos = 0;
	keypad(pad, true);
	wmove(pad, cur_x, cur_y); 
	//wmove(win, 2, 2);
	//refresh();
	wrefresh(win);
	prefresh(pad, mypadpos, 0, 6, 5, 28, 80); // pmin_row, pmin_cols, smin_row, smin_cols, smax_row, smax_col 
	while((ch = getch()) != KEY_F(2))
	{
		switch(ch)
		{
			case '/':
				search(pad, win, &mypadpos);
				wmove(pad, 2,2);
				break;
			case KEY_DOWN:
				if(cur_x < 19) {
				wmove(pad,++cur_x, cur_y);
				} else if(mypadpos < (num_rows - 20)) 
					{
						wmove(pad, ++cur_x,0);
					++mypadpos;
					}
				
			//	wmove(win, cur_x, cur_y++);
				wrefresh(win);
				prefresh(pad, mypadpos, 0, 6, 5, 28, 80);
				//printf("cur_x: %d\n",cur_x);
				break;
			case KEY_UP:
				if(mypadpos > 0) {
					wmove(pad, --cur_x, 0);
					--mypadpos;
				} else if (cur_x > 0)
				{
				wmove(pad,--cur_x, cur_y);
				} 
				prefresh(pad, mypadpos, 0, 6, 5, 28, 80);
				//wrefresh(win);
				break;
			case KEY_F(9):
			case 10:
				popwin = newwin(20,80,4,4);
				keypad(popwin, TRUE);
				FIELD *field[8];
				FORM *my_form;
				FILE *fptr;
				int x;
				int q;
				char *p[7];
				field[0] = new_field(1, 11, 1, 10, 0, 0); //num
				field[1] = new_field(1, 19, 1, 29, 0, 0); //manu
				field[2] = new_field(3, 52, 3, 10, 0, 0); //desc
				field[3] = new_field(1, 11, 1, 63, 0, 0); //loc
				field[4] = new_field(1, 4, 8, 15, 0, 0);  //quan
				field[5] = new_field(1, 11, 8, 32, 0, 0); //unit
				field[6] = new_field(1, 11, 8, 62, 0, 0);
				field[7] = NULL;
				field_opts_off(field[0], O_AUTOSKIP);
				set_field_back(field[0], A_UNDERLINE);
				field_opts_off(field[1], O_AUTOSKIP);
				set_field_back(field[1], A_UNDERLINE);
				field_opts_off(field[2], O_AUTOSKIP);
				set_field_back(field[2], A_UNDERLINE);
				field_opts_off(field[3], O_AUTOSKIP);
				field_opts_on(field[3], O_WRAP);
				set_field_back(field[3], A_UNDERLINE);
				field_opts_off(field[4], O_AUTOSKIP);
				set_field_back(field[4], A_UNDERLINE);
				field_opts_off(field[5], O_AUTOSKIP);
				set_field_back(field[5], A_UNDERLINE);
				field_opts_off(field[6], O_AUTOSKIP);
				set_field_back(field[6], A_UNDERLINE);
				my_form = new_form(field);
				scale_form(my_form, &form_rows, &form_cols);
				set_form_win(my_form, popwin);
				set_form_sub(my_form,derwin(popwin,form_rows+2,form_cols + 2, 1, 1));
				post_form(my_form);
				wattron(popwin,A_BOLD);
				mvwprintw(popwin, 2, 4, "Number");
				mvwprintw(popwin, 2, 25, "Manu");
				mvwprintw(popwin, 2, 55, "Location");
				mvwprintw(popwin, 4, 4, "Desc");
				mvwprintw(popwin, 9, 4, "Quantity");
				mvwprintw(popwin, 9, 25, "Unit");
				mvwprintw(popwin, 9, 60, "ID");
				mvwprintw(popwin, 12, 10, "Save <Enter>   Update <F4>   Clear <F8>    Exit <F2>");

				wattroff(popwin,A_BOLD);
				mysql_data_seek(result,cur_x);
				row = mysql_fetch_row(result);
				//sprintf(foo, "%-15s%-20s%-25.20s%-10s%-5.5s", row[0], row[1], row[2], row[3], row[4]);
				//mvwprintw(popwin,2, 4, foo);
				//mvwprintw(popwin, 4, 4, "Padpos: %d", mypadpos);
				set_field_buffer(field[0], 0, row[1]);
				set_field_buffer(field[1], 0, row[2]);
				set_field_buffer(field[2], 0, row[4]);
				set_field_buffer(field[3], 0, row[5]);
				set_field_buffer(field[4], 0, row[6]);
				set_field_buffer(field[5], 0, row[3]);
				set_field_buffer(field[6], 0, row[0]);
				
				for(x=0;x<7;x++)
				{
					p[x] = trim_whitespaces(field_buffer(field[x], 0));
					set_field_buffer(field[x], 0, p[x]);
				}

				box(popwin, 0, 0);
				wrefresh(popwin);
				while((ch = getch()) != KEY_F(2))
				{
					switch(ch)
					{
						case KEY_LEFT:
							form_driver(my_form, REQ_PREV_CHAR);
							break;
						case KEY_RIGHT:
							form_driver(my_form,REQ_NEXT_CHAR);
							break;
						case KEY_BACKSPACE:
							form_driver(my_form, REQ_DEL_PREV);
							break;

						case KEY_F(3):
							//char s[] = sprintf("%d",strlen(field_buffer(field[6],0)));
							//set_field_buffer(field[2],0,s);
							//if(strlen(field_buffer(field[6],0)) > 0) printf(stderr, "Not Empty"); 
							//fptr = fopen("/home/shop/parts/file.txt", "w");
							//fprintf(fptr, "strlen Field  6: %s", strlen(field[6]));
							//fclose(fptr);
							break;
						case KEY_F(4):
							form_driver(my_form, REQ_NEXT_FIELD);
							sprintf(sql, "UPDATE parts SET num = '%s', manu = '%s', unit = '%s', descr = '%s', location = '%s', quantity = '%s' WHERE id = '%s'", field_buffer(field[0],0), field_buffer(field[1], 0), field_buffer(field[5],0), field_buffer(field[2],0), field_buffer(field[3],0), field_buffer(field[4],0), field_buffer(field[6],0));
							insert_update(sql);
							result = get_data(1);
							delwin(pad);
							delwin(popwin);
							pad = make_pad(result);
							keypad(pad,true);
							touchwin(stdscr);
							mypadpos = 0;
							cur_x = 0;
							cur_y = 0;
							wmove(pad,cur_x, cur_y);
							wrefresh(win);
							prefresh(pad, mypadpos, 0, 6, 5, 28, 80);
							refresh();
							break;
						case KEY_F(8):
							for(x=0;x<7;x++)
							{
							set_current_field(my_form, field[x]);
							form_driver(my_form,REQ_CLR_FIELD);
							set_field_buffer(field[x], 0, "");
							}
							form_driver(my_form, REQ_FIRST_FIELD);
							break;
						case 9:
							form_driver(my_form,REQ_NEXT_FIELD);
							break;	
						case 10:
							sprintf(sql, "INSERT INTO parts(`num`,`manu`,`unit`,`descr`,`location`,`quantity`) VALUES ('%s','%s','%s','%s','%s','%s')", field_buffer(field[0],0), field_buffer(field[1],0), field_buffer(field[5], 0), field_buffer(field[2], 0), field_buffer(field[3], 0), field_buffer(field[4], 0));

							
	//						set_field_buffer(field[2], 0, sql);
	/*						FILE *fptr;
							fptr = fopen("/home/shop/parts/file.txt", "w");
							fprintf(fptr, "Field 1: %s", p[1]);
							fclose(fptr);*/
	//
	MYSQL *con2 = mysql_init(NULL);
	if(mysql_real_connect(con2, "localhost", "shop", "shop", "equip", 0, NULL, 0) == NULL)
	{
		finish_with_error(con2);
	}

	if(mysql_query(con2, sql))
	{
		finish_with_error(con2);
	}
	int i = mysql_affected_rows(con2);
	char r[40];
	sprintf(r, "%d rows affected", i);
	WINDOW *ok_win = newwin(10,20,20,20);
	keypad(ok_win, true);
	box(ok_win, 0, 0);
	mvwprintw(ok_win, 2, 2, r);
	wrefresh(ok_win);
	mysql_close(con2);
	ch = getch();
	//while((ch = getch()) != KEY_F(2)) // OK Window
	  //     switch(ch);
	    //   {

	      // }	       
		delwin(ok_win);
		//delwin(popwin);
		delwin(pad);
		free(result);
		result = get_data(0); 
		pad = make_pad(result);
		num_rows = mysql_num_rows(result);
		refresh();
		touchwin(stdscr);
		wrefresh(win);
		prefresh(pad, mypadpos, 0, 6, 5, 28, 80);
		//touchwin(popwin);
		//wrefresh(popwin);
				break;


						default:
					form_driver(my_form, ch);
				//mvwprintw(popwin, 18, 18, ch);	
				break; //ok_win loop
					}

				refresh();
				wrefresh(win);
				if(popwin) wrefresh(popwin);
				//if(popwin) printf("popwin");
				}
				keypad(stdscr, TRUE);
				delwin(popwin);
				touchwin(win);
				refresh();
				wrefresh(win);
				prefresh(pad, mypadpos, 0, 6, 5, 28, 80);
				break; // pad loop
		}
		//wrefresh(win);
		//refresh();
		
	}


	delwin(win);
	delwin(pad);
	endwin();
	return(0);
}
