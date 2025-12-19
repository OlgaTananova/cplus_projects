#include <iostream>
#include <iomanip>
#include <string>
#include "SeatTypes.h"

using namespace std;

//Constants
const int ROWS = 13;
const int COLS = 6;
const SeatClass FIRST{ 'F', 1, 2,  "First" };
const SeatClass BUSINESS{ 'B', 3, 7,  "Business" };
const SeatClass ECONOMY{ 'E', 8, 13, "Economy" };

//Function prototypes
char promptMenuChoice();
void clearInputLine();
void printSeatHeader();
void initializeSeats(char [ROWS][COLS]);
void printSeatingPlan(char [ROWS][COLS]);
bool isSeatAvailable(char[ROWS][COLS], int, char);
void bookSeat(char[ROWS][COLS], int, char);
bool isValidSeatLetter(char);
const SeatClass* getSeatClassByCode(char);
bool isRowInClassRange(int, const SeatClass&);
bool validateSeatChoice(int, char, const SeatClass&);
const SeatClass& promptTicketClass();
void promptDesiredSeat(int&, char&);
bool tryBookSeat(char [ROWS][COLS], const SeatClass&, int, char);


int main()
{
	//seed random number generator
	srand(time(0));

	char seats[ROWS][COLS]{};

	int row = 0;
	char col = 'A';
	bool successfullBooking = false;

	initializeSeats(seats); // initialize the seat map

	cout << "Airplane Seating Assignment\n";

	while (true) 
	{
		//Show menu with options
		char choice = promptMenuChoice();
		cout << "\n";

		//Validate choice and call functions
		if (choice == '0') {
			cout << "Bye!\n";
			break;
		}
		else if (choice == '1') 
		{
			printSeatingPlan(seats);
		}
		else if (choice == '2') 
		{
			printSeatingPlan(seats);

			//Prompt user to enter ticket class
			const SeatClass& sc = promptTicketClass();

			//Prompt the user to enter desired seat
			promptDesiredSeat(row, col);

			//Try to book a seat until success
			while (!tryBookSeat(seats, sc, row, col)) 
			{
				cout << "Please choose again.\n";
				printSeatingPlan(seats);
				promptDesiredSeat(row, col);
			}
		}
		else {
			cout << "Invalid menu choice. Try again.\n\n";
		}
	}

	return 0;

}

// ---------------------- Helper Function  ----------------------

void clearInputLine()
{
	//Clear the input buffer in case of invalid input
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ---------------------- Seat plan functions ----------------------

void initializeSeats(char seats[ROWS][COLS])
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			//randomly seed the grid
			if (rand() % 2 == 0)
			{
				seats[row][col] = '*';
			}
			else
			{
				seats[row][col] = 'X';
			}
			
		}
	}
}

void printSeatHeader()
{
	cout << '\t';
	for (char col = 'A'; col <= 'F'; col++) {
		cout << col << "\t";
	}
	cout << endl << endl;
}

void printSeatingPlan(char seats[ROWS][COLS])
{
	cout << "\nSeating plan (* = available, X = occupied)\n\n";
	printSeatHeader();

	for (int r = 0; r < ROWS; r++) {
		cout << "Row " << (r + 1) << '\t';
		for (int c = 0; c < COLS; c++) {
			cout << seats[r][c] << '\t';
		}
		cout << "\n";
	}
	cout << "\n";
}

bool isSeatAvailable(char seats[ROWS][COLS], int row1Based, char colLetter)
{
	//Converts to integers
	int r = row1Based - 1;
	int c = colLetter - 'A';
	
	//Checks if the seat is vacant
	return seats[r][c] == '*';
}

void bookSeat(char seats[ROWS][COLS], int row1Based, char colLetter)
{
	int r = row1Based - 1;
	int c = colLetter - 'A';
	seats[r][c] = 'X';
}

// ---------------------- Validation / Mapping ----------------------


bool isValidSeatLetter(char seatCol)
{
	return seatCol >= 'A' && seatCol <= 'F'; 
}

const SeatClass* getSeatClassByCode(char ticketCode)
{
	ticketCode = toupper(ticketCode);

	//Maps the ticket code to the corresponding object (struct)
	if (ticketCode == FIRST.code)    return &FIRST;
	if (ticketCode == BUSINESS.code) return &BUSINESS;
	if (ticketCode == ECONOMY.code)  return &ECONOMY;

	//returns null pointer if mapping has failed
	return nullptr;
}

bool isRowInClassRange(int row, const SeatClass& sc)
{
	return row >= sc.rowStart && row <= sc.rowEnd;
}

bool validateSeatChoice(int row, char col, const SeatClass& sc)
{
	if (row < 1 || row > ROWS) {
		cout << "Row must be between 1 and 13." << endl;
		return false;
	}

	if (!isValidSeatLetter(col)) {
		cout << "Seat letter must be between A and F." << endl;
		return false;
	}

	if (!isRowInClassRange(row, sc)) {
		cout << "The row is not in the class you entered." << endl;
		return false;
	}

	return true;
}

// ---------------------- Input functions ----------------------

const SeatClass& promptTicketClass()
{	
	char choice{};
	while (true) {
		cout << "Ticket type:\n"
			<< "  F - First Class (rows 1-2)\n"
			<< "  B - Business Class (rows 3-7)\n"
			<< "  E - Economy Class (rows 8-13)\n"
			<< "Enter choice (F/B/E): ";

		
		if (!(cin >> choice)) {
			clearInputLine();
			cout << "\nInvalid input. Try again.\n\n";
			continue;
		}

		choice = toupper(choice);

		//Map the choice to the object 
		const SeatClass* sc = getSeatClassByCode(choice);
		if (!sc) {
			cout << "Invalid ticket type. Please enter F, B, or E.\n\n";
			continue;
		}

		//return the ticket class object
		return *sc;
	}
}

void promptDesiredSeat(int& row, char& col)
{
	while (true) {
		cout << "Enter desired seat (examples: 1A, 10F): ";

		string s{};
		getline(cin, s);
		if (!(cin >> s)) {
			clearInputLine();
			cout << "Invalid input. Try again.\n\n";
			continue;
		}

		if (s.size() < 2) {
			cout << "Please enter a row number followed by a seat letter (e.g., 1A).\n\n";
			continue;
		}

		//The last character in the string is a seat letter
		char last = s.back();
		col = toupper(last);

		//The rest of the string is a numeric part
		string rowPart = s.substr(0, s.length() - 1);
		bool allDigits = true;

		//Check if all characters in the string are digits
		for (int i = 0; i < rowPart.length(); i++) 
		{
			if (!isdigit(rowPart[i])) 
			{
				allDigits = false;
				break;
			}
		}

		if (!allDigits) {
			cout << "Row must be numeric (example: 10F).\n\n";
			continue;
		}

		//Covert string to int
		row = stoi(rowPart);
		return;
	}
}

// ---------------------- Booking workflow ----------------------

bool tryBookSeat(char seats[ROWS][COLS], const SeatClass& sc, int row, char col)
{
	if (!validateSeatChoice(row, col, sc)) 
	{
		return false;
	}

	if (!isSeatAvailable(seats, row, col)) 
	{
		return false;
	}

	bookSeat(seats, row, col);
	cout << "\nSeat " << row << col << " booked in " << sc.name << ".\n\n";
	return true;
}

// ---------------------- Menu ----------------------

char promptMenuChoice()
{
	cout << "\nMenu:\n"
		<< "  1 - Display seating plan\n"
		<< "  2 - Book a seat\n"
		<< "  0 - Exit\n"
		<< "\nEnter choice: ";

	char choice{};
	cin >> choice;
	return choice;
}
