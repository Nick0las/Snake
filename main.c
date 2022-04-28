#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>

#define WINDOW_WIDTH 120
#define WINDOW_HIGHT 30
#define WINDOW_BORDER_LEFT 0
#define WINDOW_BORDER_RIGHT WINDOW_WIDTH-1
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_ESC 27

#define IMG_WIDTH 10
#define IMG_HEGHT 10
#define IMG_NUMBER 4

#define DRAW 1
#define ERASE 0

#define MENU_SIZE 3
#define MENU_ITEM_LENGHT 10

#define FIELD_BORDER_UP 0
#define FIELD_BORDER_DOWN WINDOW_HIGHT-10
#define FIELD_BORDER_LEFT 0
#define FIELD_BORDER_RIGHT WINDOW_WIDTH-1

#define SNAKE_SIZE 50

int Speed = 230;

char imges[IMG_HEGHT][IMG_WIDTH] = {
								0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,
								0,0,0,1,1,1,1,0,0,0,
								0,0,0,1,0,0,1,0,0,0,
								0,0,0,1,0,0,1,0,0,0,
								0,0,0,1,1,1,1,0,0,0,
								0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,
								0,0,0,0,0,0,0,0,0,0,


};

char Menu[MENU_SIZE][MENU_ITEM_LENGHT] = { "New Game", "Options", "Exit" }; // массив для меню

HANDLE hStdOut;

enum colors_enum
{
	Black = 0,
	Grey = FOREGROUND_INTENSITY,
	GreyBG = BACKGROUND_INTENSITY,
	LightCyanBG = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
	LightGrey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Blue = FOREGROUND_BLUE,
	Green = FOREGROUND_GREEN,
	Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
	Red = FOREGROUND_RED,
	Purple = FOREGROUND_RED | FOREGROUND_BLUE,
	LightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	LightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	LightCyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	LightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
	LightPurple = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Orange = FOREGROUND_RED | FOREGROUND_GREEN,
	Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
};

void ShowImg(char img[IMG_HEGHT][IMG_WIDTH], int row, int col, int mode); // функция показа картинки из символов
void gotoxy(int x, int y); //задать координаты курсора
void setcolor(WORD attr); //установить цвет
void cursorHide(); // скрыть курсор
void ShowSymbol(char ch, int x, int y, WORD color); // вывод любого символа с заданными координатами и цветом
void ShowMenu(int x, int y, int selected); // функция показа меню
void Game(); // функция, в которой проходит вся игра
void Options(); // опции игры (в разработке)
void ClrScr(); // очистка экрана
void ShowField(); // показ границ игрового поля

void ShowScore(int score, int x, int y, WORD color); // показ заработанных очков (количество съеденных "фруктов" змейкой)


void main()
{
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//Sleep(1000);
	//_kbhit();// была ли нажата какая-либо клавиша
	cursorHide();

	char ch;
	int menu_pos = 0;
	int isExit = 0;
	//char snake = '@';
	//**************************************************************//
   //            В ЦИКЛЕ ПРОХОДИТ УПРАВЛЕНИЕ ПОЗИЦИЕЙ МЕНЮ         //
  //              ВЫБОР С ПОМОЩЬЮ СТРЕЛОК И КЛАВИШИ ENTER         //
 //                          ESC ВЫХОД                           //
//**************************************************************//
	srand(time(NULL));
	while (!isExit)
	{
		ShowMenu(10, 5, menu_pos);
		ch = _getch();
		if (ch == -32)
		{
			ch = _getch();
		}


		switch (ch)
		{
		case KEY_UP:
			if (menu_pos > 0)
			{
				menu_pos--;
			}
			break;
		case KEY_DOWN:
			if (menu_pos < MENU_SIZE - 1)
			{
				menu_pos++;
			}
			break;
		case KEY_ESC:
			isExit = 1;
			break;
		case KEY_ENTER:
			switch (menu_pos)
			{
			case 0:
				ClrScr();
				Game();
				ClrScr();
				break;
			case 1:
				ClrScr();
				Options();
				ClrScr();
				break;
			case 2:
				isExit = 1;
				ClrScr();
				break;
			}
		}
		ClrScr();
	}




	//_getch();
}



void ClrScr()
{
	setcolor(Black);
	setcolor(White);
	system("cls");
}

void gotoxy(int x, int y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(hStdOut, c);
}

void setcolor(WORD attr)
{
	SetConsoleTextAttribute(hStdOut, attr);
}

void cursorHide()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 100;
	cursorInfo.bVisible = FALSE;

	SetConsoleCursorInfo(hStdOut, &cursorInfo);
}

void ShowMenu(int x, int y, int selected)
{
	int i;
	for (i = 0; i < MENU_SIZE; i++)
	{
		if (selected == i)
		{
			setcolor(Orange);
		}

		else
		{
			setcolor(Green);
		}

		gotoxy(x, y + i);
		printf("%s", Menu[i]);

	}
	cursorHide();
}

void ShowBoard(char ch, int x, int y, WORD color, int lenght)
{
	int i;
	for (i = 0; i < lenght; i++)
	{
		ShowSymbol(ch, x++, y, color);
	}
}

void Game()
{

	int step;
	int isExit = 0;
	int X = 0, Y = 0;

	//----------ФРУКТ--------------//
	char fruit = 'F';
	int fruit_X = 2 + rand(time(NULL)) % 116;
	int fruit_Y = 1 + rand(time(NULL)) % 19;
	WORD fruit_color = Orange;

	//-------ЗМЕЙКА--------//
	char snake = '#'; // голова змейки

	WORD snake_color = LightGreen; //цвет головы

	int body_lenght = 1;
	int snake_x[SNAKE_SIZE] = { 75,74,73,72 };
	int snake_y[SNAKE_SIZE] = { 15,15,15,15 };
	int tempSnake_x[SNAKE_SIZE] = { 0 };
	int tempSnake_y[SNAKE_SIZE] = { 0 };

	//-------------ЗАРАБОТАННЫЕ ОЧКИ----------------//
	int score = 1;

	int i;
	char ch;




	ShowField();

	while (!isExit)
	{



		ShowScore(score, 3, 22, Yellow); // ВЫВОД ЗАРАБОТАННЫХ ОЧКОВ В ЗАДАННЫХ КООРДИНАТАХ 

		ShowSymbol(fruit, fruit_X, fruit_Y, fruit_color);


		for (step = 1; step < 400; step++)
		{

			if (_kbhit())
			{
				ch = _getch();
				if (ch == -32)
				{
					ch = _getch();
				}

				switch (ch)
				{
				case KEY_LEFT:

					X = -1;
					Y = 0;

					break;
				case KEY_RIGHT:

					X = 1;
					Y = 0;

					break;
				case KEY_UP:

					X = 0;
					Y = -1;

					break;
				case KEY_DOWN:

					X = 0;
					Y = 1;

					break;
				case KEY_ESC:
					isExit = 1;
					break;
				}
			} // конец цикла if (_kbhit())

			for (i = 0; i < SNAKE_SIZE; i++)
			{

				if ((snake_x[0] == fruit_X) && (snake_y[0] == fruit_Y))
				{
					body_lenght += 1;
					fruit_X = 2 + rand(time(NULL)) % 116;
					fruit_Y = 1 + rand(time(NULL)) % 19;
					score++;
					ShowSymbol(fruit, fruit_X, fruit_Y, fruit_color);

				}
			}

			ShowSymbol(snake, snake_x[body_lenght - 1], snake_y[body_lenght - 1], Black);
			for (i = 0; i < body_lenght - 1; i++)
			{
				tempSnake_y[i] = snake_y[i];
				tempSnake_x[i] = snake_x[i];
			}

			snake_y[0] += Y;
			snake_x[0] += X;

			for (i = 1; i < body_lenght; i++)
			{
				snake_y[i] = tempSnake_y[i - 1];
				snake_x[i] = tempSnake_x[i - 1];
			}
			for (i = 0; i < body_lenght; i++)
			{
				ShowSymbol(snake, snake_x[i], snake_y[i], snake_color);
			}
			Sleep(Speed);
			break;


		} // конец цикла for (step = 1; step < 400; step++)

		  //---------------условие завершения игры (если врежемся в границы)----------------------//
		if (snake_x[0] <= FIELD_BORDER_LEFT + 1 || snake_x[0] >= FIELD_BORDER_RIGHT - 1)
		{
			ClrScr();
			setcolor(White);
			gotoxy(55, 10);
			printf("You Are Looser!");
			Sleep(1000);
			isExit = 1;
		}

		if (snake_y[0] <= FIELD_BORDER_UP || snake_y[0] >= FIELD_BORDER_DOWN)
		{
			ClrScr();
			setcolor(White);
			gotoxy(55, 10);
			printf("You Are Looser!");
			Sleep(1000);
			isExit = 1;
		}
		if (score == SNAKE_SIZE)
		{
			ClrScr();
			setcolor(White);
			gotoxy(55, 10);
			printf("You Win!");
			Sleep(2000);
			isExit = 1;
		}
	}//конец цикла while(!isExit)

} // конец Game

void Options()
{
	int speed;
	char ch;
	int isExit = 0;
	setcolor(Purple);
	gotoxy(10, 5);

	printf("Enter a speed level from 1-5 (1-low, 5 - high)\n");
	gotoxy(10, 7);
	scanf_s("%d", &speed);
	if (5 == speed)
	{
		Speed = 30;
	}
	if (4 == speed)
	{
		Speed = 50;
	}
	if (3 == speed)
	{
		Speed = 75;
	}
	if (2 == speed)
	{
		Speed = 100;
	}
	if (1 == speed)
	{
		Speed = 125;
	}

}

void ShowSymbol(char ch, int x, int y, WORD color)
{
	setcolor(color);
	gotoxy(x, y);
	printf("%c", ch);
}

void ShowScore(int score, int x, int y, WORD color)
{
	setcolor(color);
	gotoxy(x, y);
	printf("Score: %d", score);
}


void ShowField()
{
	int i;
	for (i = FIELD_BORDER_LEFT + 1; i <= FIELD_BORDER_RIGHT - 1; i++)
	{
		ShowSymbol(' ', i, FIELD_BORDER_UP, GreyBG);
		ShowSymbol(' ', i, FIELD_BORDER_DOWN, GreyBG);

	}

	for (i = FIELD_BORDER_UP + 1; i < FIELD_BORDER_DOWN; i++)
	{
		ShowSymbol(' ', FIELD_BORDER_LEFT + 1, i, GreyBG);
		ShowSymbol(' ', FIELD_BORDER_RIGHT - 1, i, GreyBG);
	}
}

