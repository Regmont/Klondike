#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <ctime>
#include <conio.h>

using namespace std;

//Перечисление принимаемых с клавиатуры сигналов
enum Move { LEFT = 97, UP = 119, RIGHT = 100, DOWN = 115, SPACE = 32, PAUSE = 112 };
//Масти карт
enum Suit { SUIT_SPACE = 0, HEARTS = 1, DIAMONDS = 2, SPADES = 3, CLUBS = 4 };

//Игральная карта
struct card
{
	int power = 0;			//Сила
	Suit suit = SUIT_SPACE;	//Масть
	bool closed = false;	//Открыта или нет
};

card restCards[24];				//Оставшиеся карты после распределения колоды по полю
card matr_cards[19][9];			//Матрица с расположением карт
card handCards[13];				//Карты на руках

card heartsStack[13];			//Стопка черви
card spadesStack[13];			//Стопка пики
card diamondsStack[13];			//Стопка буби
card clubsStack[13];			//Стопка крести

COORD coord = { 0, 0 };			//Координата местоположения
COORD previous = { 500, 500 };	//Координаты ячейки, из которой взяли карту
int returnType = 0;				//Способ возврата карт на предыдущие позиции
int returnAmount = 1;			//Количество возвращаемых карт
int restCardsAmount = 23;		//Количество карт, оставшихся в колоде

//Текущие элементы в стопках карт с одной мастью
int heartsElem = 12, spadesElem = 12, diamondsElem = 12, clubsElem = 12;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

void Input(bool& Game_Over, bool& Game_Pause);
void Recogn_Input(Move& selection);
void Show_plField();
void Show_matrics();
void Tutorial();
void Print_Main_Menu();
void Print_End_Menu();
void Print_Pause_Menu();
void ConvertDigitsToSymbols(card Card, COORD c);
void Print_CardCover(COORD c);
void Print_Cursor(COORD c, bool create);

//Меню
void Menu(bool& Game_Over, bool& Game_Close, int menu)
{
	//Функция выводит одно из трёх меню (1 - главное, 2 - паузы, 3 - победы)

	Move selection = Move(0);	//Возможность выбора
	int choice = 1;				//Выбранный элемент

	switch (menu)
	{
	case 1:
		Print_Main_Menu();			//Вывод главного меню
		break;
	case 2:
		Print_Pause_Menu();			//Вывод меню паузы
		break;
	case 3:
		Print_End_Menu();			//Вывод меню победы
		break;
	}

	//Ожидание нажатия "Enter"
	while (_kbhit && selection != 13)
	{
		selection = (Move)_getch();		//Считывание символа с клавиатуры
		Recogn_Input(selection);		//Опознание вводимых данных

		//Если не меню победы
		if (menu != 3)
		{
			//Перемещение между 2 полями выбора
			if (selection == UP || selection == DOWN)
				choice == 1 ? choice = 2 : choice = 1;

			//Стирание старого курсора
			SetConsoleCursorPosition(h, { 3, 4 });
			cout << ' ';
			SetConsoleCursorPosition(h, { 3, 7 });
			cout << ' ';

			//Вывод нового курсора
			if (choice == 1)
				SetConsoleCursorPosition(h, { 3, 4 });
			else
				SetConsoleCursorPosition(h, { 3, 7 });
			cout << char(254);

			//Перемещение курсора под меню
			SetConsoleCursorPosition(h, { 0, 11 });
		}
		else
		{
			//Перемещение между 2 полями выбора
			if (selection == LEFT || selection == RIGHT)
				choice == 1 ? choice = 2 : choice = 1;

			//Стирание старого курсора
			SetConsoleCursorPosition(h, { 10, 7 });
			for (int i = 0; i < 5; i++)
				cout << ' ';

			SetConsoleCursorPosition(h, { 16, 7 });
			for (int i = 0; i < 5; i++)
				cout << ' ';

			//Вывод нового курсора
			if (choice == 1)
				SetConsoleCursorPosition(h, { 10, 7 });
			else
				SetConsoleCursorPosition(h, { 16, 7 });
			for (int i = 0; i < 5; i++)
				cout << char(254);

			//Перемещение курсора под меню
			SetConsoleCursorPosition(h, { 0, 11 });
		}
	}

	system("cls");

	if (choice == 2)
	{
		switch (menu)
		{
		case 1:
			Tutorial();						//Вывод обучения
			Menu(Game_Over, Game_Close, 1);	//Выход в главное меню
			break;
		case 2:
			Game_Over = true;
			break;
		case 3:
			Game_Close = true;
			system("pause");
			break;
		}
	}
}


//Основная логика
void AdditionalLogic()
{
	//Вывод значения карты в ячейке (0; 1), когда требуется, и пробелов, когда не требуется
	ConvertDigitsToSymbols(matr_cards[1][0], { 11, 7 });

	//Вывод обложки на месте стопки оставшихся карт, когда требуется, и пробелов, когда не требуется
	if (restCards[0].power != 150 && restCards[0].power != 0)
		Print_CardCover({ 11, 3 });
	else
	{
		SetConsoleCursorPosition(h, { 11, 3 });
		cout << "   ";
	}

	//Вывод верхней карты в стопке черви или значка черви, если карт нет
	if (heartsElem != 12)
		ConvertDigitsToSymbols(heartsStack[heartsElem + 1], { 59, 3 });
	else
	{
		SetConsoleCursorPosition(h, { 59, 3 });
		cout << ' ' << char(3) << ' ';
	}

	//Вывод верхней карты в стопке пики или значка пики, если карт нет
	if (spadesElem != 12)
		ConvertDigitsToSymbols(spadesStack[spadesElem + 1], { 59, 7 });
	else
	{
		SetConsoleCursorPosition(h, { 59, 7 });
		cout << ' ' << char(6) << ' ';
	}

	//Вывод верхней карты в стопке буби или значка буби, если карт нет
	if (diamondsElem != 12)
		ConvertDigitsToSymbols(diamondsStack[diamondsElem + 1], { 59, 11 });
	else
	{
		SetConsoleCursorPosition(h, { 59, 11 });
		cout << ' ' << char(4) << ' ';
	}

	//Вывод верхней карты в стопке крести или значка крести, если карт нет
	if (clubsElem != 12)
		ConvertDigitsToSymbols(clubsStack[clubsElem + 1], { 59, 15 });
	else
	{
		SetConsoleCursorPosition(h, { 59, 15 });
		cout << ' ' << char(5) << ' ';
	}

	//Отображение пермещённых карт
	for (int i = 1; i < 20; i++)
		for (int j = 1; j < 8; j++)
			if (!matr_cards[i - 1][j].closed)
				ConvertDigitsToSymbols(matr_cards[i - 1][j], { short(j * 6 + 11), short(i * 4 - 1) });

	//Вывод карт в руке
	for (int i = 1; i < 14; i++)
		ConvertDigitsToSymbols(handCards[i - 1], { 74, short(i * 4 - 1) });
}

void RetCards()
{
	//Функция возвращает карты на исходную позицию при невозможности действия

	switch (returnType)	//Способ возврата
	{
	case 0:				//Возврат нескольких карт в основную часть поля
	{
		for (int i = 0; i < returnAmount; i++)
		{
			matr_cards[previous.Y + i][previous.X] = handCards[i];
			handCards[i] = { 0, SUIT_SPACE };
		}

		matr_cards[previous.Y + returnAmount][previous.X] = { 300, SUIT_SPACE };
		break;
	}
	case 1:				//Возврат карты в стопку черви
	{
		heartsStack[previous.X] = handCards[0];
		heartsElem--;
		handCards[0] = { 0, SUIT_SPACE };
		break;
	}
	case 2:				//Возврат карты в стопку пики
	{
		spadesStack[previous.X] = handCards[0];
		spadesElem--;
		handCards[0] = { 0, SUIT_SPACE };
		break;
	}
	case 3:				//Возврат карты в стопку буби
	{
		diamondsStack[previous.X] = handCards[0];
		diamondsElem--;
		handCards[0] = { 0, SUIT_SPACE };
		break;
	}
	case 4:				//Возврат карты в стопку крести
	{
		clubsStack[previous.X] = handCards[0];
		clubsElem--;
		handCards[0] = { 0, SUIT_SPACE };
		break;
	}
	case 5:				//Возврат карты в ячейку с элементом 150
	{
		restCards[restCardsAmount + 1] = matr_cards[1][0];
		matr_cards[1][0] = handCards[0];
		handCards[0] = { 0, SUIT_SPACE };
		restCardsAmount++;
		break;
	}
	}

	previous = { 500, 500 };	//Присвоение коодинате ячейки, откуда взяли карту, значения неиспользуемого числа
	returnType = 0;				//Приведение способа возврата карт к стандартному
	returnAmount = 1;			//Приведение количества возвращаемых карт к стандартному
}

void Logic(Move selection, COORD prev_cursor)
{
	//Функция выполняет логику пермещение по игровому полю и даёт возможность взять и положить карты

	if (selection == LEFT)
	{
		switch (coord.X)
		{
		case 0:			//Перемещение из 1 столбца в 9
			coord.X = 8;
			break;
		case 1:			//Перемещение из 2 столбца в 1
			if (coord.Y != 0)
			{
				//Если в ячейке с координатами (0; 1) есть карта, туда можно попасть
				if (matr_cards[1][0].power == 0 || matr_cards[1][0].power == 150)
					coord.Y = 0;
				else
					coord.Y = 1;
			}
			coord.X = 0;
			break;
		default:
			//Если на руках нет карт
			if (handCards[0].power == 0)
			{
				//Если слева есть карта
				if (matr_cards[coord.Y][coord.X - 1].power < 50)
				{
					//Поиск пустой игровой ячейки под картой слева
					for (coord.Y; coord.Y < 19; coord.Y++)
						if (!matr_cards[coord.Y][coord.X - 1].closed)
							break;
				}
				else
				{
					//Поиск карты над игровой ячейкой слева
					for (coord.Y; coord.Y > 0; coord.Y--)
						if (matr_cards[coord.Y][coord.X - 1].power < 50)
							break;
				}
			}
			//Если нижняя игровая ячейка в столбце слева пустая
			else if (matr_cards[18][coord.X - 1].power > 50)
			{
				//Поиск игровой ячейки с переменным изпользованием (300) в столбце слева
				for (coord.Y = 18; coord.Y > 0; coord.Y--)
					if (matr_cards[coord.Y][coord.X - 1].power == 300)
						break;
			}
			else
				coord.Y = 18;
			coord.X--;
			break;
		}
	}

	if (selection == RIGHT)
	{
		switch (coord.X)
		{
		case 8:			//Перемещение из 9 столбца в 1
			if (coord.Y != 0)
			{
				//Если в ячейке с координатами (0; 1) есть карта, туда можно попасть
				if (matr_cards[1][0].power == 0 || matr_cards[1][0].power == 150)
					coord.Y = 0;
				else
					coord.Y = 1;
			}
			coord.X = 0;
			break;		//Перемещение из 8 столбца в 9
		case 7:
			if (coord.Y < 4)
				coord.X = 8;
			else
			{
				coord.X = 8;
				coord.Y = 3;
			}
			break;
		default:
			//Если на руках нет карт
			if (handCards[0].power == 0)
			{
				//Если справа есть карта
				if (matr_cards[coord.Y][coord.X + 1].power < 50)
				{
					//Поиск пустой игровой ячейки под картой справа
					for (coord.Y; coord.Y < 19; coord.Y++)
						if (!matr_cards[coord.Y][coord.X + 1].closed)
							break;
				}
				else
				{
					//Поиск карты над игровой ячейкой справа
					for (coord.Y; coord.Y > 0; coord.Y--)
						if (matr_cards[coord.Y][coord.X + 1].power < 50)
							break;
				}
			}
			//Если нижняя игровая ячейка в столбце справа пустая
			else if (matr_cards[18][coord.X + 1].power > 50)
			{
				//Поиск игровой ячейки с переменным изпользованием (300) в столбце справа
				for (coord.Y = 18; coord.Y >= 0; coord.Y--)
					if (matr_cards[coord.Y][coord.X + 1].power == 300)
						break;
			}
			else
				coord.Y = 18;
			coord.X++;
			break;
		}
	}

	if (selection == UP)
	{
		//Если не верхняя строка
		if (coord.Y != 0)
		{
			//Если на руках нет карт
			if (handCards[0].power == 0)
			{
				//Если карта сверху не закрыта
				if (!matr_cards[coord.Y - 1][coord.X].closed)
					coord.Y--;
				else
				{
					//Поиск самой нижней открытой карты в текущем столбце
					for (coord.Y = 18; coord.Y > 0; coord.Y--)
						if (matr_cards[coord.Y][coord.X].power < 50)
							break;
				}
			}
			//Если 1 или 9 столбец
			else if (coord.X == 0 || coord.X == 8)
				coord.Y--;
		}
		else
		{
			switch (coord.X)
			{
			case 0:		//1 столбец
				//Если в ячейке с координатами (0; 1) есть карта, туда можно попасть
				if (!(matr_cards[1][0].power == 0 || matr_cards[1][0].power == 150))
					coord.Y = 1;
				break;
			case 8:		//9 столбец
				coord.Y = 3;
				break;
			default:
				//Если на руках нет карт
				if (handCards[0].power == 0)
				{
					//Поиск самой нижней открытой карты в текущем столбце
					for (coord.Y = 18; coord.Y > 0; coord.Y--)
						if (matr_cards[coord.Y][coord.X].power < 50)
							break;
				}
				break;
			}
		}
	}

	if (selection == DOWN)
	{
		switch (coord.X)
		{
		case 0:		//1 столбец
			//Если верхняя строка
			if (coord.Y == 0)
			{
				//Если в ячейке с координатами (0; 1) есть карта, туда можно попасть
				if (!(matr_cards[1][0].power == 0 || matr_cards[1][0].power == 150))
					coord.Y = 1;
			}
			else
				coord.Y = 0;
			break;
		case 8:		//9 столбец
			if (coord.Y != 3)
				coord.Y++;
			else
				coord.Y = 0;
			break;
		default:
			//Если на руках нет карт
			if (handCards[0].power == 0)
			{
				//Если снизу есть карта
				if (matr_cards[coord.Y + 1][coord.X].power < 50)
					coord.Y++;
				else
				{
					//Поиск самой верхней открытой карты в текущем столбце
					for (coord.Y = 0; coord.Y < 19; coord.Y++)
						if (!matr_cards[coord.Y][coord.X].closed)
							break;
				}
			}
			break;
		}
	}

	if (selection == SPACE)
	{
		switch (matr_cards[coord.Y][coord.X].power)
		{
		case 100:
			//Если на руках нет карт
			if (handCards[0].power == 0)
			{
				//Если остались карты в колоде оставшихся карт
				if (!(restCards[0].power == 0 && matr_cards[1][0].power == 150))
				{
					card restCardsElem;

					//Сдвиг элементов в массиве restCards на 1
					restCardsElem = matr_cards[1][0];
					matr_cards[1][0] = restCards[0];

					for (int i = 0; i < restCardsAmount; i++)
						restCards[i] = restCards[i + 1];

					restCards[restCardsAmount] = restCardsElem;

					//Уменьшение количества оставшихся карт на 1, если использовали взятую из массива restCards[] карту
					if (restCardsElem.power == 0)
						restCardsAmount--;
				}
			}
			else
				RetCards();
			break;
		case 500:
			int* selectedElem;
			card* selectedStack;
			Suit searchSuit;

			switch (coord.Y)
			{
			case 0:
				selectedElem = &heartsElem;
				selectedStack = heartsStack;
				searchSuit = HEARTS;
				break;
			case 1:
				selectedElem = &spadesElem;
				selectedStack = spadesStack;
				searchSuit = SPADES;
				break;
			case 2:
				selectedElem = &diamondsElem;
				selectedStack = diamondsStack;
				searchSuit = DIAMONDS;
				break;
			case 3:
				selectedElem = &clubsElem;
				selectedStack = clubsStack;
				searchSuit = CLUBS;
				break;
			default:
				selectedElem = nullptr;
				selectedStack = nullptr;
				searchSuit = SUIT_SPACE;
			}

			if (*selectedElem != 12)
			{
				if (handCards[0].power == 0)
				{
					handCards[0] = selectedStack[*selectedElem + 1];			//Берём верхнюю карту из стопки
					selectedStack[*selectedElem + 1] = { 0, SUIT_SPACE };		//Обнуляем элемент в стопке
					(*selectedElem)++;											//Перемещаемся на элемент ниже в стопке
					previous.X = *selectedElem;									//Запоминаем расположение взятой карты в стопке
					returnType = 1;												//Запоминаем, откуда взяли карту
				}
				else
				{
					//Если на руках 1 карта искомой масти
					if (handCards[1].power == 0 && handCards[0].suit == searchSuit)
					{
						//Если карта может ставиться на карту под ней в стопке
						if (handCards[0].power == selectedStack[*selectedElem + 1].power + 1)
						{
							selectedStack[*selectedElem] = handCards[0];	//Кладём карту из рук в стопку
							handCards[0] = { 0, SUIT_SPACE };				//Обнуляем значение карты в руке
							(*selectedElem)--;								//Перемещаемся на элемент выше в стопке
						}
						else
							RetCards();
					}
					else
						RetCards();
				}
			}
			//Если на руках есть карта
			else if (handCards[0].power != 0)
			{
				//Если на руках туз искомой масти
				if (handCards[0].power == 1 && handCards[0].suit == searchSuit)
				{
					selectedStack[*selectedElem] = handCards[0];		//Кладём карту из рук в стопку
					handCards[0] = { 0, SUIT_SPACE };					//Обнуляем значение карты в руке
					(*selectedElem)--;									//Перемещаемся на элемент выше в стопке
				}
				else
					RetCards();
			}
			break;
		default:
			//Если 1 столбец
			if (coord.X == 0)
			{
				//Если на руках нет карт
				if (handCards[0].power == 0)
				{
					//Если ячейка с координатами (0; 1) не имеет значения 150
					if (matr_cards[1][0].power != 150)
					{
						handCards[0] = matr_cards[1][0];				//Берём карту из ячейки (0; 1) в руки
						matr_cards[1][0] = restCards[restCardsAmount];	//...
						restCards[restCardsAmount] = { 0, SUIT_SPACE };	//...
						restCardsAmount--;

						previous.Y = 1;							//Запоминаем координату Y места возвращения
						previous.X = 0;							//Запоминаем координату X места возвращения
						returnType = 5;							//Запоминаем, откуда взяли карту
					}
				}
				else
					RetCards();
			}
			else
			{
				//Если на руках нет карт
				if (handCards[0].power == 0)
				{
					//Подсчёт количества карт для взятия
					int take_len = 0;

					for (int i = coord.Y; i < 19; i++)
						if (matr_cards[i][coord.X].power < 50)
							take_len++;
						else
							break;

					//Взятие карт в руки и замена мест, где были карты, числом 400
					for (int i = 0; i < take_len; i++)
					{
						handCards[i] = matr_cards[coord.Y + i][coord.X];
						matr_cards[coord.Y + i][coord.X] = { 400, SUIT_SPACE };
					}

					//Замена старого положения значения 300 на новое
					for (int i = 0; i < 19; i++)
						if (matr_cards[i][coord.X].power == 300)
							matr_cards[i][coord.X].power = 400;

					matr_cards[coord.Y][coord.X] = { 300, SUIT_SPACE };

					//Запоминаем куда и сколько вернуть карт при невозможности их поставить, куда надо
					previous.Y = coord.Y;
					previous.X = coord.X;
					returnAmount = take_len;
					returnType = 0;
				}
				else
				{
					//Если не верхняя и не нижняя строка
					if (coord.Y != 0 && coord.Y != 18)
					{
						//Если 1 карта на руках черви или буби
						if (handCards[0].suit == HEARTS || handCards[0].suit == DIAMONDS)
						{
							//Если карта в игровой ячейке сверху от текущей пики или крести
							if (matr_cards[coord.Y - 1][coord.X].suit == SPADES || matr_cards[coord.Y - 1][coord.X].suit == CLUBS)
							{
								//Если 1 карта на руках может ставиться под карту сверху
								if (handCards[0].power + 1 == matr_cards[coord.Y - 1][coord.X].power)
								{
									//Кладём карты из рук на стол
									for (int i = 0, j = coord.Y; i < returnAmount; i++, j++)
									{
										matr_cards[j][coord.X] = handCards[i];
										handCards[i] = { 0, SUIT_SPACE };
									}
									matr_cards[coord.Y + returnAmount][coord.X].power = 300;

									returnAmount = 1;	//Приведение количество возвращаемых карт к стандартному
									previous.Y = 500;	//Присваиваем начальному значению карты по оси Y значения неиспользуемого числа
									previous.X = 500;	//Присваиваем начальному значению карты по оси X значения неиспользуемого числа
								}
								else
									RetCards();
							}
							else
								RetCards();
						}
						else
						{
							//Если карта в игровой ячейке сверху от текущей черви или буби
							if (matr_cards[coord.Y - 1][coord.X].suit == HEARTS || matr_cards[coord.Y - 1][coord.X].suit == DIAMONDS)
							{
								//Если 1 карта на руках может ставиться под карту сверху
								if (handCards[0].power + 1 == matr_cards[coord.Y - 1][coord.X].power)
								{
									//Кладём карты из рук на стол
									for (int i = 0, j = coord.Y; i < returnAmount; i++, j++)
									{
										matr_cards[j][coord.X] = handCards[i];
										handCards[i] = { 0, SUIT_SPACE };
									}
									matr_cards[coord.Y + returnAmount][coord.X].power = 300;

									returnAmount = 1;	//Приведение количество возвращаемых карт к стандартному
									previous.Y = 500;	//Присваиваем начальному значению карты по оси Y значения неиспользуемого числа
									previous.X = 500;	//Присваиваем начальному значению карты по оси X значения неиспользуемого числа
								}
								else
									RetCards();
							}
							else
								RetCards();
						}
					}
					else if (coord.Y == 0)
					{
						//Если 1 карта на руках - король
						if (handCards[0].power == 13)
						{
							//Кладём карты из рук на стол
							for (int i = 0; i < returnAmount; i++)
							{
								matr_cards[i][coord.X] = handCards[i];
								handCards[i] = { 0, SUIT_SPACE };
							}
							matr_cards[returnAmount][coord.X].power = 300;

							returnAmount = 1;	//Приведение количество возвращаемых карт к стандартному
							previous.Y = 500;	//Присваиваем начальному значению карты по оси Y значения неиспользуемого числа
							previous.X = 500;	//Присваиваем начальному значению карты по оси X значения неиспользуемого числа
						}
						else
							RetCards();
					}
					else
					{
						//Если текущая игровая ячейка пустая
						if (matr_cards[18][coord.X].power > 50)
						{
							//Если 1 карта на руках черви или буби
							if (handCards[0].suit == HEARTS || handCards[0].suit == DIAMONDS)
							{
								//Если карта в игровой ячейке сверху от текущей пики или крести
								if (matr_cards[coord.Y - 1][coord.X].suit == SPADES || matr_cards[coord.Y - 1][coord.X].suit == CLUBS)
								{
									matr_cards[18][coord.X] = handCards[0];
									handCards[0] = { 0, SUIT_SPACE };

									returnAmount = 1;	//Приведение количество возвращаемых карт к стандартному
									previous.Y = 500;	//Присваиваем начальному значению карты по оси Y значения неиспользуемого числа
									previous.X = 500;	//Присваиваем начальному значению карты по оси X значения неиспользуемого числа
								}
								else
									RetCards();
							}
							else
							{
								//Если карта в игровой ячейке сверху от текущей черви или буби
								if (matr_cards[coord.Y - 1][coord.X].suit == HEARTS || matr_cards[coord.Y - 1][coord.X].suit == DIAMONDS)
								{
									matr_cards[18][coord.X] = handCards[0];
									handCards[0] = { 0, SUIT_SPACE };

									returnAmount = 1;	//Приведение количество возвращаемых карт к стандартному
									previous.Y = 500;	//Присваиваем начальному значению карты по оси Y значения неиспользуемого числа
									previous.X = 500;	//Присваиваем начальному значению карты по оси X значения неиспользуемого числа
								}
								else
									RetCards();
							}
						}
						else
							RetCards();
					}
				}
			}
		}

		//Открытие карт на поле, когда следует
		for (int i = 2; i < 8; i++)
			for (int j = 0; j < 6; j++)
				if (matr_cards[j][i].closed == true && matr_cards[j + 1][i].power == 300 && handCards[0].power == 0)
					matr_cards[j][i].closed = false;
	}

	if (selection != SPACE)
	{
		Print_Cursor({ short(prev_cursor.X * 6 + 9), short(prev_cursor.Y * 4 + 1) }, false);
		Print_Cursor({ short(coord.X * 6 + 9), short(coord.Y * 4 + 1) }, true);
	}
	else
		AdditionalLogic();
}

void Cards(card Deck[52])
{
	//Функция создаёт колоду карт

	srand(time(0));

	int matr[4][13];	//Матрица, хранящая 4 строки значений с 1 по 13 (под каждую масть)

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 13; j++)
			matr[i][j] = j + 1;

	int k = 0;

	//В цикле выбирается случайное значение из митрицы matr[][] и преобразуется в элемент массива cardDeck[]
	do
	{
		int i = rand() % 4;		//Случайный выбор масти карты
		int j = rand() % 13;	//Случайный выбор силы карты

		//Уже использованный элемент матрицы matr[][] заменяется нулём и не используется
		if (matr[i][j] != 0)
		{
			Deck[k] = { matr[i][j], Suit(i + 1) };

			matr[i][j] = 0;
			k++;
		}

	} while (k < 52);
}

void Create_matrics()
{
	//Функция создаёт матрицу со значениями, связанными с картами на поле

	card cardDeck[52];		//Колода карт

	Cards(cardDeck);		//Создаётся массив случайных неповторяющихся карт

	//Заполнение неиспользуемых полей значением 400
	for (int i = 0; i < 19; i++)
		for (int j = 0; j < 9; j++)
			matr_cards[i][j] = { 400, SUIT_SPACE };

	matr_cards[0][0].power = 100;			//Присвоение месту колоды значения 100
	matr_cards[1][0] = { 150, SUIT_SPACE };	//Присвоение полю для складывания карт значения 150

	//Присвоение границам значения -1
	for (int i = 2; i < 19; i++)
		matr_cards[i][0].power = -1;

	for (int i = 4; i < 19; i++)
		matr_cards[i][8].power = -1;

	//Присвоение полям значений карт
	for (int i = 0, k = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
			if (i < j)								//Закрытые карты
			{
				matr_cards[i][j + 1] = cardDeck[k];
				matr_cards[i][j + 1].closed = true;
				k++;
			}

	for (int i = 0, k = 21; i < 7; i++, k++)
		matr_cards[i][i + 1] = cardDeck[k];			//Открытые карты

	//Присвоение колоде значений оставшихся карт
	for (int i = 0, k = 28; i < 24; i++, k++)
		restCards[i] = cardDeck[k];

	//Присвоение полям значений стопок мастей (500)
	for (int i = 0; i < 4; i++)
		matr_cards[i][8].power = 500;

	//Присвоение значения 300 переменно используемым полям (на которых можно находиться, имея карты в руках)
	for (int i = 1; i < 8; i++)
		matr_cards[i][i] = { 300, SUIT_SPACE };
}

int main()
{
	bool Game_Close = false;	//Закрытие игры
	bool Game_Over = false;		//Конец игры
	bool Game_Pause = false;	//Пауза

	//Вывод начальный экран
	Menu(Game_Over, Game_Close, 1);

	//Создание матрица логики
	Create_matrics();

	//Отображание игрового поля
	Show_plField();

	//Пока не закрыта игра
	while (!Game_Close)
	{
		//Если новая игра
		if (Game_Over)
		{
			//Начало новой игры
			coord = { 0, 0 };
			previous = { 500, 500 };
			returnType = 0;
			returnAmount = 1;
			restCardsAmount = 23;
			heartsElem = spadesElem = diamondsElem = clubsElem = 12;

			for (int i = 0; i < 13; i++)
			{
				heartsStack[i] = { 0, SUIT_SPACE };
				spadesStack[i] = { 0, SUIT_SPACE };
				diamondsStack[i] = { 0, SUIT_SPACE };
				clubsStack[i] = { 0, SUIT_SPACE };
			}

			Game_Over = false;
			Game_Pause = false;

			Menu(Game_Over, Game_Close, 1);
			Create_matrics();
			Show_plField();

			//Отладочная информация
			/*SetConsoleCursorPosition(h, { 0, 80 });
			Show_matrics();*/
		}
		else
			Game_Pause = false;

		//Пока игра не завершена и не поставлена на паузу, производится ввод действий с клавиатуры 
		while (!(Game_Over || Game_Pause))
			Input(Game_Over, Game_Pause);

		//Если игра на паузе
		if (Game_Pause)
		{
			system("cls");
			Menu(Game_Over, Game_Close, 2);	//Переход в меню паузы

			//Если игру сняли с паузы
			if (!Game_Over)
			{
				Show_plField();
				AdditionalLogic();

				//Курсор устанавливается в удобную позицию
				SetConsoleCursorPosition(h, { 65, 0 });
				if (coord.Y < 4)
					SetConsoleCursorPosition(h, { 65, 17 });
				else if (coord.Y < 8)
					SetConsoleCursorPosition(h, { 65, 32 });
				else if (coord.Y < 12)
					SetConsoleCursorPosition(h, { 65, 48 });
				else if (coord.Y < 16)
					SetConsoleCursorPosition(h, { 65, 68 });
				else
					SetConsoleCursorPosition(h, { 65, 78 });
			}
		}
		/*Game_Over = true;
		Game_Pause = false;*/
		//Если игра пройдена
		if (Game_Over && !Game_Pause)
		{
			system("cls");
			Menu(Game_Over, Game_Close, 3);	//Переход в меню победы
		}
	}
}


//Ввод с клавиатуры
void Recogn_Input(Move& selection)
{
	//Функция преобразует нежелательный способ ввода в желательный (для enum)

	switch (selection)
	{
	case 68:	//Введено 'D' или 'В' или 'в'
	case 130:
	case 162:
	{
		selection = RIGHT;
		break;
	}
	case 65:	//Введено 'A' или 'Ф' или 'ф'
	case 148:
	case 228:
	{
		selection = LEFT;
		break;
	}
	case 87:	//Введено 'W' или 'Ц' или 'ц'
	case 150:
	case 230:
	{
		selection = UP;
		break;
	}
	case 83:	//Введено 'S' или 'Ы' или 'ы'
	case 155:
	case 235:
	{
		selection = DOWN;
		break;
	}
	case 80:	//Введено 'P' или 'з' или 'З'
	case 167:
	case 135:
	{
		selection = PAUSE;
		break;
	}
	}
}

void Input(bool& Game_Over, bool& Game_Pause)
{
	//Функция считывает, что нажал пользователь, и зупускает логику игры

	Move selection = Move(0);
	COORD prev_cursor = coord;		//Координата ячейки на поле, где до этого находился курсор

	if (_kbhit())
	{
		selection = (Move)_getch();		//Возможность выбора
		Recogn_Input(selection);		//Опознание вводимых данных

		//Если нажата пауза (p)
		if (selection == PAUSE)
			Game_Pause = true;
		else
		{
			Logic(selection, prev_cursor);	//Вызов логики

			//Курсор устанавливается в удобную позицию
			SetConsoleCursorPosition(h, { 65, 0 });
			if (coord.Y < 4)
				SetConsoleCursorPosition(h, { 65, 17 });
			else if (coord.Y < 8)
				SetConsoleCursorPosition(h, { 65, 32 });
			else if (coord.Y < 12)
				SetConsoleCursorPosition(h, { 65, 48 });
			else if (coord.Y < 16)
				SetConsoleCursorPosition(h, { 65, 68 });
			else
				SetConsoleCursorPosition(h, { 65, 78 });

			//Отладочная информация
			/*SetConsoleCursorPosition(h, { 0, 80 });
			Show_matrics();	*/

			//Проверка на победу
			if (heartsStack[0].power == 13 && spadesStack[0].power == 13 && diamondsStack[0].power == 13 && clubsStack[0].power == 13)
				Game_Over = true;
		}
	}
}


//Вывод данных на экран
void ConvertDigitsToSymbols(card Card, COORD c)
{
	//Функция преобразовывает силу и масть карт из числового значения в символьное

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(h, c);

	//Вывод силы карты
	if (Card.power > 1 && Card.power < 10)			//Двойка - девятка
		cout << ' ' << char(Card.power + 48);
	else if (Card.power == 10)						//Десятка
		cout << "10";
	else if (Card.power > 100 || Card.power == 0)	//Пустая карта
		cout << "   ";
	else											//Туз, валет - король
	{
		cout << ' ';

		switch (Card.power)
		{
		case 1:								//Туз
			cout << 'A';
			break;
		case 11:							//Валет
			cout << 'J';
			break;
		case 12:							//Дама
			cout << 'Q';
			break;
		case 13:							//Король
			cout << 'K';
			break;
		}
	}

	//Вывод масти карты
	switch (Card.suit)
	{
	case HEARTS:
		cout << char(3);
		break;
	case DIAMONDS:
		cout << char(4);
		break;
	case SPADES:
		cout << char(6);
		break;
	case CLUBS:
		cout << char(5);
		break;
	}
}

void PrintCell(COORD c, int length)
{
	//Функция выводит ячейку заданной длины

	SetConsoleCursorPosition(h, c);

	//Строка 1
	cout << char(201);

	for (int i = 0; i < length; i++)
		cout << char(205);

	cout << char(187);

	c.Y++;
	SetConsoleCursorPosition(h, c);

	//Строка 2
	cout << char(186);

	for (int i = 0; i < length; i++)
		cout << ' ';

	cout << char(186);

	c.Y++;
	SetConsoleCursorPosition(h, c);

	//Строка 3
	cout << char(200);

	for (int i = 0; i < length; i++)
		cout << char(205);

	cout << char(188);
}

void PrintSuit(COORD c, char s)
{
	//Функция выводит значок масти

	SetConsoleCursorPosition(h, c);

	cout << s;
}

void Print_CardCover(COORD c)
{
	//Функция выводит обложку карты

	SetConsoleCursorPosition(h, c);

	for (int i = 0; i < 3; i++)
		cout << char(178);
}

void Print_Cursor(COORD c, bool create)
{
	//Функция выводит новый курсор на игровом поле либо удаляет старый

	SetConsoleCursorPosition(h, c);

	char symb;

	//Удаление или создание курсора
	if (create)
		symb = 176;
	else
		symb = ' ';

	//Создание обводки в виде прямоугольника
	for (int i = 0; i < 7; i++)
		cout << symb;

	c.Y += 4;

	SetConsoleCursorPosition(h, c);

	for (int i = 0; i < 7; i++)
		cout << symb;

	c.Y -= 3;

	for (int i = 0; i < 3; i++, c.Y++)
	{
		SetConsoleCursorPosition(h, c);

		cout << symb;
		c.X += 6;

		SetConsoleCursorPosition(h, c);

		cout << symb;
		c.X -= 6;
	}
}

void Print_Main_Menu()
{
	//Функция выводит главный экран

	cout << endl;
	cout << "     KLONDIKE" << endl << endl;

	//Рамки кнопок
	PrintCell({ 4, 3 }, 8);
	PrintCell({ 4, 6 }, 8);

	//Название 1 кнопки
	SetConsoleCursorPosition(h, { 7, 4 });
	cout << "play";

	//Название 2 кнопки
	SetConsoleCursorPosition(h, { 5, 7 });
	cout << "tutorial";

	//Курсор
	SetConsoleCursorPosition(h, { 3, 4 });
	cout << char(254);


	SetConsoleCursorPosition(h, { 0, 10 });
	cout << "use W, S to switch";

	SetConsoleCursorPosition(h, { 0, 11 });
}

void Print_Pause_Menu()
{
	//Функция выводит экран паузы

	cout << endl;
	cout << "       PAUSE" << endl << endl;

	//Рамки кнопок
	PrintCell({ 4, 3 }, 9);
	PrintCell({ 4, 6 }, 9);

	//Название 1 кнопки
	SetConsoleCursorPosition(h, { 5, 4 });
	cout << "continue";

	//Название 2 кнопки
	SetConsoleCursorPosition(h, { 5, 7 });
	cout << "main menu";

	//Курсор
	SetConsoleCursorPosition(h, { 3, 4 });
	cout << char(254);

	SetConsoleCursorPosition(h, { 0, 10 });
	cout << "use W, S to switch";

	SetConsoleCursorPosition(h, { 0, 11 });
}

void Print_End_Menu()
{
	//Функция выводит экран победы

	cout << endl;
	cout << "\tCONGRATULATIONS!" << endl;
	cout << "\t  You've won!" << endl;
	cout << "\t  Play again?" << endl << endl;

	//Рамки кнопок
	PrintCell({ 10, 4 }, 3);
	PrintCell({ 16, 4 }, 3);

	//Название 1 кнопки
	SetConsoleCursorPosition(h, { 11, 5 });
	cout << "yes";

	//Название 2 кнопки
	SetConsoleCursorPosition(h, { 17, 5 });
	cout << "no!";

	//Курсор
	SetConsoleCursorPosition(h, { 10, 7 });
	for (int i = 0; i < 5; i++)
		cout << char(254);

	SetConsoleCursorPosition(h, { 0, 10 });
	cout << "use W, S to switch";

	SetConsoleCursorPosition(h, { 0, 11 });
}

void Tutorial()
{
	//Функция выводит обучение

	cout << "WASD - move\n";
	cout << "Space - take/put cards\n";
	cout << "P - pause the game\n\n";

	cout << "Note: it is recommended to maximize the console window to full screen\n";
	cout << "Close the game window to exit the game\n\n";
	cout << "Warning: the game is not saved when exiting the main menu or the application!" << endl << endl;

	system("pause");
	system("cls");
}

void Show_plField()
{
	//Функция выводит игровое поле на экран

	//1 строка
	cout << '\t' << char(201);

	for (int i = 0; i < 55; i++)
		cout << char(205);

	cout << char(187) << endl;

	//Строки 2 - 77
	for (int i = 1; i < 78; i++)
	{
		cout << '\t' << char(186);

		for (int j = 0; j < 55; j++)
			cout << ' ';

		cout << char(186) << endl;
	}

	//78 строка (последняя)
	cout << '\t' << char(200);

	for (int i = 0; i < 55; i++)
		cout << char(205);

	cout << char(188) << endl;

	//Игровые ячейки в левом столбце
	PrintCell({ 10, 2 }, 3);
	PrintCell({ 10, 6 }, 3);

	//Игровые ячейки в правом столбце
	for (int i = 2; i < 15; i += 4)
		PrintCell({ 58, short(i) }, 3);

	//Остальные игровые ячейки
	for (int i = 2; i < 75; i += 4)
		for (int j = 16; j < 55; j += 6)
			PrintCell({ short(j), short(i) }, 3);

	//Масти стопок карт
	PrintSuit({ 60, 3 }, 3);
	PrintSuit({ 60, 7 }, 6);
	PrintSuit({ 60, 11 }, 4);
	PrintSuit({ 60, 15 }, 5);

	//Рубашка колоды карт
	Print_CardCover({ 11, 3 });

	//Рубашки закрытых карт
	for (int i = 1; i <= 6; i++)
		for (int j = 1; j <= 6; j++)
			if (i <= j)
				Print_CardCover({ short(j * 6 + 17), short(i * 4 - 1) });

	//Открытые карты
	for (int i = 1; i < 8; i++)
		ConvertDigitsToSymbols(matr_cards[i - 1][i], { short(i * 6 + 11), short(i * 4 - 1) });

	//Курсор
	Print_Cursor({ short(coord.X * 6 + 9), short(coord.Y * 4 + 1) }, true);

	//Карты на руках
	SetConsoleCursorPosition(h, { 71, 1 });
	cout << "your hand";

	for (int i = 1; i < 14; i++)
		PrintCell({ 73, short(i * 4 - 2) }, 3);

	//Перемещение курсора консоли в нужное начальное положение
	SetConsoleCursorPosition(h, { 65, 0 });
	SetConsoleCursorPosition(h, { 65, 17 });
}



//Отладка
void Show_Properties(card arr[13])
{
	//Функция выводит силу и масть 13 карт из переданного массива

	for (int i = 0; i < 13; i++)
		cout << arr[i].power << ' ' << arr[i].suit << '\t';

	cout << endl;
}

void Show_matrics()
{
	//Функция отображает отладочную матрицу игры

	//Значения карт на поле
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			cout << matr_cards[i][j].power << ' ' << (int)matr_cards[i][j].suit;
			//cout << ' ' << matr_cards[i][j].closed;	//Отображение, какие карты закрыты, а какие нет
			cout << '\t';
		}

		cout << endl;
	}
	cout << endl;

	//Отображение стопок карт мастей (массивов)
	Show_Properties(heartsStack);
	Show_Properties(spadesStack);
	Show_Properties(diamondsStack);
	Show_Properties(clubsStack);
	cout << endl;

	Show_Properties(handCards);		//Отображение карт на руках 	

	cout << "\nY = " << coord.Y << "  X = " << coord.X;		//вывод координат курсора

	//Вывод координат места взятой карты (500 - нет карт на руках)
	cout << "\nprevCell_Y = " << previous.Y;
	cout << "\nprevCell_X = " << previous.X;
	cout << "\n\n";

	//Отображение оставшихся в колоде карт
	for (int i = 0; i < 24; i++)
	{
		if (i % 8 == 0)
			cout << endl;

		cout << restCards[i].power << ' ' << restCards[i].suit << '\t';
	}

	cout << endl << "retCardsAmount = " << returnAmount;
}