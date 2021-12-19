#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <windows.h>
#include <ctime>
#include <cstdlib>
using namespace std;
HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);

//constant variables for columns 1,2,3
const int teamName = 0, teamTown = 1, teamStad = 2;
const int COLS = 3;

//***********************************
//	Creating function prototypes	*
//***********************************
void sortTeams(string[][COLS], int);
void rearrange(string[][COLS], int, bool = true);
void setTeams(string[][COLS], string[][COLS], string[][COLS], int[], int);
void setRandomPlaces(int[], int);
//Providing week header
void provideWeek(int&, int&, ofstream&);
//Creating a division for the week
void divideWeek(int&, ofstream&);

void derbySort(string[][COLS], string[][COLS], string[][COLS], int, int&);
void derbyMatches(string[][COLS], int, int, ofstream&);
void homeVSaway(string[][COLS], string[][COLS], int, int, ofstream&);
void fillTempArray(string[][COLS], string[][COLS], int);
//This function is used for accepting user's input
int enterOption(int, string, int, int);
//Display all the teams
void allTeamsDisplay(string[][COLS], int);
//Displaying information from a file
void showFileContent(string);
//Displaying a menu
void showMenu(string[], int);
//return to main menu
bool toMenu();

int main()
{
	//Creating read-from-drive variables
	ifstream readTeam, readTown, readStadium, readFixtures;
	//Creating an array for the teams
	string teams[10][COLS];
	//Creating variables which will hold data from the files
	string team_name, team_town, team_stad, fixtures;
	//a variable to hold options of the user
	char option='y';
	SetConsoleTextAttribute(color, 7);
	cout << "Welcome to the ABC Premier league fixtures app" << endl;

	//Opening the files
	readTeam.open("teams.txt");
	readTown.open("town.txt");
	readStadium.open("stadiums.txt");

	if (readTeam && readTown && readStadium) {
		int count = 0;
		//Transfering data from the file into the array
		while (getline(readTeam, team_name) && getline(readTown, team_town) && getline(readStadium, team_stad)) {
			teams[count][teamName] = team_name;
			teams[count][teamTown] = team_town;
			teams[count][teamStad] = team_stad;

			count++;
		}

		//Creating an array to hold menus
		SetConsoleTextAttribute(color, 11);
		string menu[] = {
			"Generate new fixtures",
			"Show Previous Fixtures",
			"Show All Teams",
			"Open A File In The Directory",
			"Exit Program"
		};
		int menu_size = sizeof(menu) / sizeof(string);

	show_menu: {
		//Asking user what he wants to do
		SetConsoleTextAttribute(color, 11);
		cout << "\nPlease select an option" << endl;
		showMenu(menu, menu_size);
		}

	//Receiving user's reply
	option = enterOption(option, "\nOption: ", 1, menu_size);
	system("CLS");
	bool to_menu;
	//using switch case to display value entered
	switch (option) {
	case 1: {
		//Sort and store the fixtures
		sortTeams(teams, count);
		while (getline(readFixtures, fixtures)) {
			cout << fixtures << endl;
		}
		to_menu = toMenu();
		if (to_menu) {
			goto show_menu;
		}
		break;
	}

	case 2: {
	    //try opening fixtures.txt
	    readFixtures.open("fixtures.txt");

		//show fixtures from the file
		if (!readFixtures) {
			char opt;
			SetConsoleTextAttribute(color, 4);
			cout << "Required file not found! \n\nDo you want to create the file 'fixtures.txt'?" << endl;
			cout << "Yes -> <y> \t No -> <n>\n";

		take: {
			SetConsoleTextAttribute(color, 11);
			cout << "\nOption: "; cin >> opt;
			}

		opt = tolower(opt);
		if (opt != 'y' && opt != 'n') {
			SetConsoleTextAttribute(color, 4);
			cout << "\nValue not accepted! Please provide 'y' or 'n'" << endl;
			cin.clear();
			cin.ignore(123, '\n');
			goto take;
		}
		if (opt == 'y') {
			sortTeams(teams, count);
			while (getline(readFixtures, fixtures)) {
				cout << fixtures << endl;
			}
		}
		}
		else {
			while (getline(readFixtures, fixtures)) {
				cout << fixtures << endl;
			}
		}
		to_menu = toMenu();
		if (to_menu) {
			goto show_menu;
		}
		break;
	}

	case 3: {
		//Show the details of the teams we have
		allTeamsDisplay(teams, count);
		to_menu = toMenu();
		if (to_menu) {
			goto show_menu;
		}
		break;
	}

	case 4: {
		//Let the user open a file he likes
		string file_name;
		cout << "Please provide a .txt or .csv filename\n\n";
		cout << "Filename: ";
		cin.sync();
		getline(cin, file_name);
		showFileContent(file_name);
		to_menu = toMenu();
		if (to_menu) {
			goto show_menu;
		}
		break;
	}

	default:
		SetConsoleTextAttribute(color, 2);
		cout << "The program has been successfully stopped\n"
			<< "See you soon :)\n";
	}
	//Closing the files to save memory
	readTeam.close();
	readTown.close();
	readStadium.close();
	readFixtures.close();
	}
	else if (!readTeam || !readTown || !readStadium) {
		if (!readTeam) {
			cout << "'teams.txt' was not found" << endl;
		}
		else if (!readTown) {
			cout << "'town.txt' was not found" << endl;
		}
		else {
			cout << "'stadium.txt' was not found" << endl;
		}
	}
	else {
		cout << "Needed files could not be found on the system drive" << endl;
	}


	return 0;
}

//*******************************
//	Defining the functions		*
//*******************************
void sortTeams(string array[][COLS], int size) {
	srand(time(0));
	int teamSelectors[10];
	string home_teams[10 / 2][COLS];
	string away_teams[10 / 2][COLS];

	//Create file to store fixtures
	ofstream writeFixtures;

	//Creating the file
	writeFixtures.open("fixtures.txt");

	if (!writeFixtures) {
		cout << "An error occured! 'fixtures.txt' could not be created" << endl;
		cout << "Check your current directory if its a read-only folder" << endl;
		exit(1);
	}

	//Inititalize the array selector with a negative member value
	//This is to prevent similar fixtures per run of program
	for (int i = 0; i < size; i++) {
		teamSelectors[i] = -1;
	}

	//Provide viable values to the team selector
	setRandomPlaces(teamSelectors, size);

	//Divide the main array in equal halves
	setTeams(array, home_teams, away_teams, teamSelectors, size);

	//Create variables to present the weeks in the program
	int week = 0, weekCount = 0;

	//Creating temporal arrays to serve as comparators
	string temp_home[10 / 2][COLS], temp_away[10 / 2][COLS];

	//filling the temporal arrays to serve as copies of the original
	fillTempArray(home_teams, temp_home, size / 2);
	fillTempArray(away_teams, temp_away, size / 2);

	//Swapping members of the temporal arrays
	rearrange(temp_home, size / 2);
	rearrange(temp_away, size / 2);

	//Creating an array to hold the derby
	string myDerby[10][COLS];
	//Serve as the pointer and counter of the array.
	//Check and provide the appropriate positions for the derby members
	int derbyCounter = 0;
	SetConsoleTextAttribute(color, 6);
	//Writing the heading into the file
	writeFixtures << "====================================================================================\n";
	writeFixtures << left << setw(44) << "Fixture";
	writeFixtures << left << setw(15) << "Home Town";
	writeFixtures << left << setw(20) << "Home Stadium";
	writeFixtures << left << setw(25) << " Leg " << endl;
	writeFixtures << left << setw(44) << "-------";
	writeFixtures << left << setw(15) << "---------";
	writeFixtures << left << setw(20) << "------------";
	writeFixtures << left << setw(25) << "-----" << endl;
	writeFixtures << "=====================================================================================\n";
	//***********************************
	//	Creating all fixtures			*
	//***********************************
	for (int i = 0; i < size; i++) {
		if (i < size / 2) {
			int j;
			//Now determine how the pointing should be done per the counts
			//Aim is to send the most closest of array positioning to the end of the count
			if (i % 2 == 0) {
				j = 0;
			}
			else {
				j = 1;
			}

			//Let the members play against themselves in their respective
			//arrays, removing all derbys as well
			while ((j + 1) <= size / 2) {
				if (home_teams[j][teamName] != temp_home[j][teamName]) {
					if (home_teams[j][teamTown] == temp_home[j][teamTown]) {
						derbySort(myDerby, home_teams, temp_home, j, derbyCounter);
					}
					else {
						provideWeek(week, weekCount, writeFixtures);
						homeVSaway(home_teams, temp_home, j, 1, writeFixtures);
						divideWeek(week, writeFixtures);
					}
				}

				if (away_teams[j][teamName] != temp_away[j][teamName]) {
					if (away_teams[j][teamTown] == temp_away[j][teamTown]) {
						derbySort(myDerby, away_teams, temp_away, j, derbyCounter);
					}
					else {
						provideWeek(week, weekCount, writeFixtures);
						homeVSaway(away_teams, temp_away, j, 1, writeFixtures);
						divideWeek(week, writeFixtures);
					}
				}
				j += 2;
			}
			j = 0;
			//Present the other games for the round
			while (j < size / 2) {
				rearrange(away_teams, size / 2);
				if (home_teams[j][teamTown] == away_teams[j][teamTown]) {
					derbySort(myDerby, home_teams, away_teams, j, derbyCounter);
				}
				else {
					provideWeek(week, weekCount, writeFixtures);
					homeVSaway(home_teams, away_teams, j, 1, writeFixtures);
					divideWeek(week, writeFixtures);
				}
				j++;
			}

			//Present the derby after everyone has equally played his part
			if (i == (size / 2) - 1) {
				for (int k = 1; k < derbyCounter; k += 2) {
					provideWeek(week, weekCount, writeFixtures);
					derbyMatches(myDerby, k, 1, writeFixtures);
					divideWeek(week, writeFixtures);
				}
				//reset derby counter to prepare it for the 2nd leg
				derbyCounter = 0;
			}

			//Swapping the team arrays
			int swap = (size / 2) - (size / 4);
			for (int i = 0; i < swap; i++) {
				//Making a swap on all original values from last - first
				rearrange(home_teams, size / 2, false);
				rearrange(away_teams, size / 2, false);

				//Making a swap on all temporal arrays from first - last
				rearrange(temp_home, size / 2);
				rearrange(temp_away, size / 2);
			}
		}
		else {
			//Giving the pointer j a standard starting point
			int j;

			//Now determine how the pointing should be done per the counts
			//Aim is to send the most closest of array positioning to the end of the count
			if (i % 2 == 0) {
				j = 0;
			}
			else {
				j = 1;
			}

			//Let the members play against themselves in their respective
			//arrays, removing all derbys as well
			while ((j + 1) <= size / 2) {
				if (home_teams[j][teamName] != temp_home[j][teamName]) {
					if (home_teams[j][teamTown] == temp_home[j][teamTown]) {
						derbySort(myDerby, home_teams, temp_home, j, derbyCounter);
					}
					else {
						provideWeek(week, weekCount, writeFixtures);
						homeVSaway(home_teams, temp_home, j, 2, writeFixtures);
						divideWeek(week, writeFixtures);
					}
				}

				if (away_teams[j][teamName] != temp_away[j][teamName]) {
					if (away_teams[j][teamTown] == temp_away[j][teamTown]) {
						derbySort(myDerby, away_teams, temp_away, j, derbyCounter);
					}
					else {
						provideWeek(week, weekCount, writeFixtures);
						homeVSaway(away_teams, temp_away, j, 2, writeFixtures);
						divideWeek(week, writeFixtures);
					}
				}
				j += 2;
			}

			//reset the pointer
			j = 0;

			//Present the other games for the round
			while (j < size / 2) {
				rearrange(away_teams, size / 2);
				if (home_teams[j][teamTown] == away_teams[j][teamTown]) {
					derbySort(myDerby, home_teams, away_teams, j, derbyCounter);
				}
				else {
					provideWeek(week, weekCount, writeFixtures);
					homeVSaway(home_teams, away_teams, j, 2, writeFixtures);
					divideWeek(week, writeFixtures);
				}
				j++;
			}

			//Present the derby after everyone has equally played his part
			if (i == size - 1) {
				for (int k = 1; k < derbyCounter; k += 2) {
					provideWeek(week, weekCount, writeFixtures);
					derbyMatches(myDerby, k, 2, writeFixtures);
					divideWeek(week, writeFixtures);
				}

				//reset derby counter to prepare it for the 2nd leg
				derbyCounter = 0;
			}

			//Swapping the team arrays
			int swap = (size / 2) - (size / 4);
			for (int i = 0; i < swap; i++) {
				//Making a swap on all original values from last - first
				rearrange(home_teams, size / 2, false);
				rearrange(away_teams, size / 2, false);

				//Making a swap on all temporal arrays from first - last
				rearrange(temp_home, size / 2);
				rearrange(temp_away, size / 2);
			}
		}
	}

	//Close the new fixtures file
	writeFixtures.close();
}

void rearrange(string array[][COLS], int size, bool first_last) {
	int i = 0;
	string temp[COLS];

	if (first_last) {
		//a loop to send the last number to the back
		while (i < size) {
			//catch the first value	and hold it in memory
			if (i == 0) {
				for (int l = 0; l < COLS; l++) {
					temp[l] = array[0][l];
				}
			}

			//transfer values to the immediate left
			if ((i + 1) < size) {
				for (int w = 0; w < COLS; w++) {
					array[i][w] = array[i + 1][w];
				}
			}

			//store the last element with the stored element
			if (i == size - 1) {
				for (int g = 0; g < COLS; g++) {
					array[size - 1][g] = temp[g];
				}
			}

			i++;
		}
	}
	else {
		//a loop to bring the last number to the front
		i = size - 1;
		while (i >= 0) {
			//catch the last value	and hold it in memory
			if (i == size - 1) {
				for (int l = 0; l < COLS; l++) {
					temp[l] = array[size - 1][l];
				}
			}

			//transfer values to the immediate right
			if ((i - 1) >= 0) {
				for (int w = 0; w < COLS; w++) {
					array[i][w] = array[i - 1][w];
				}
			}

			//store the first element with the stored element
			if (i == 0) {
				for (int g = 0; g < COLS; g++) {
					array[0][g] = temp[g];
				}
			}

			i--;
		}
	}
}

void setTeams(string fullTeam[][COLS], string home[][COLS], string away[][COLS], int selector[], int size) {
	//Track pointers
	int h = 0,
		a = 0,
		//Selector variable to point main array
		pointer;

	//Dividing main array into halves
	for (int i = 0; i < size; i++) {
		//Getting randomly selected main array positions
		pointer = selector[i];

		if (i % 2 == 0) {
			for (int v = 0; v < COLS; v++) {
				home[h][v] = fullTeam[pointer][v];
			}
			h++;
		}
		else {
			for (int v = 0; v < COLS; v++) {
				away[a][v] = fullTeam[pointer][v];
			}
			a++;
		}
	}
}

void setRandomPlaces(int array[], int size) {
	//Creating random position picking points
	for (int i = 0; i < size; i++) {
		//the num variable would be generating a random number
		int num = rand() % 10;

		//pointer to prevent repetition of values
		int d = 0;

		while (d < size) {
			//Checking for sameness
			if (array[d] == num) {
				//Be cautious of the boundary
				if (num + 1 < size) {
					num = num + 1;
				}
				else {
					num = 0;
				}
				//restart check to prevent another repetition
				d = 0;
			}
			else {
				//check next number if there is no sameness in the current position
				d++;
			}
		}
		//Store srutinized position selector in the array
		array[i] = num;
	}
}

void provideWeek(int& wk, int& w_count, ofstream& filename) {
	//Displaying the weekend number
	if (wk % 2 == 0) {
		w_count++;
		SetConsoleTextAttribute(color, 30);
		filename << "Weekend # " << w_count << endl;
	}
}

void divideWeek(int& wk, ofstream& filename) {
	if (wk % 2 == 1) {
		filename << endl;
	}
	wk++;
}

void homeVSaway(string home[][COLS], string away[][COLS], int selector, int leg, ofstream& filename) {
	if (leg == 1) {
		filename << left << setw(20) << home[selector][teamName];
		filename << left << setw(4) << " vs ";
		filename << left << setw(20) << away[selector][teamName];
		filename << left << setw(15) << home[selector][teamTown];
		filename << left << setw(20) << home[selector][teamStad];
		filename << " Leg 1 " << endl;
	}
	else {
		filename << left << setw(20) << away[selector][teamName];
		filename << left << setw(4) << " vs ";
		filename << left << setw(20) << home[selector][teamName];
		filename << left << setw(15) << away[selector][teamTown];
		filename << left << setw(20) << away[selector][teamStad];
		filename << " Leg 2 " << endl;
	}
}

void derbyMatches(string array[][COLS], int selector, int leg, ofstream& filename) {
	if (leg == 1) {
		filename << "Derby Match(Leg 1):" << endl;
		filename << left << setw(20) << array[selector - 1][teamName];
		filename << left << setw(4) << " vs ";
		filename << left << setw(20) << array[selector][teamName];
		filename << left << setw(15) << array[selector - 1][teamTown];
		filename << left << setw(20) << array[selector - 1][teamStad];
		filename << " Leg 1 " << endl;
	}
	else {
		filename << "Derby Matche(Leg 2):" << endl;
		filename << left << setw(20) << array[selector][teamName];
		filename << left << setw(4) << " vs ";
		filename << left << setw(20) << array[selector - 1][teamName];
		filename << left << setw(15) << array[selector][teamTown];
		filename << left << setw(20) << array[selector][teamStad];
		filename << " Leg  2" << endl;
	}
}

void derbySort(string derbyArray[][COLS], string home[][COLS], string away[][COLS], int pointer, int& counter) {
	int count = counter;
	//Filling the first port of the derbyArray
	for (int i = 0; i < COLS; i++) {
		derbyArray[count][i] = home[pointer][i];
	}

	//filling the second port of the derbyArray
	for (int i = 0; i < COLS; i++) {
		derbyArray[count + 1][i] = away[pointer][i];
	}

	//Increamenting the external counter
	counter += 2;
}

void fillTempArray(string main_array[][COLS], string temp_array[][COLS], int size) {
	for (int i = 0; i < size; i++) {
		for (int w = 0; w < COLS; w++) {
			temp_array[i][w] = main_array[i][w];
		}
	}
}

int enterOption(int value, string someText, int min_value, int max_value) {
	//Creating a checkpoint
receive_option: {
	cout << someText; cin >> value;
	}

//Check if the user is in the range
while (value < min_value || value > max_value) {
	cout << "\nValue is not accepted. Please enter a valid value in the range of\n"
		<< min_value << " and " << max_value;

	//reset the input buffer for other values
	cin.clear();
	cin.ignore(123, '\n');

	cout << endl;
	goto receive_option;
}

//If everything has been passed, then return the value
return value;
}

void allTeamsDisplay(string main_array[][COLS], int array_size) {
	//Display the teams and their respective infos
	cout << left << setw(20) << "Team Name";
	cout << left << setw(20) << "Team Town";
	cout << left << setw(20) << "Home Stadium" << endl;
	cout << left << setw(20) << "---------";
	cout << left << setw(20) << "---------";
	cout << left << setw(20) << "------------" << endl;

	for (int i = 0; i < array_size; i++) {
		for (int j = 0; j < 3; j++) {
			cout << left << setw(20) << main_array[i][j];
		}

		cout << endl;
	}
}

void showFileContent(string filename) {
	ifstream readFile;
	readFile.open(filename.c_str());

	string data;

	if (readFile) {
		cout << filename << endl << endl;
		while (getline(readFile, data)) {
			cout << data << endl;
		}
	}
	else {
		cout << "'" << filename << "' could not be found on your computer system\n"
			<< "Its either your filename is incorrect or the file could not be opened\n"
			<< "Check that the file is in the same directory as this program and try\n"
			<< "again.\n";

		//Stop the program
		exit(1);
	}

	readFile.close();
}

void showMenu(string menu_array[], int menu_size) {
	for (int i = 0; i < menu_size; i++) {
		cout << i + 1 << ". " << menu_array[i] << endl;
	}
}

bool toMenu()
{
	char choice;

	cout << "\nDo you want to return to the main menu?\n";
	cout << "Yes -> <y> \t No -> <n>\n\n";

receive: {
	cout << "Option: "; cin >> choice;
	}
choice = tolower(choice);

while (choice != 'y' && choice != 'n') {
	cout << "\nInvalid input! Please enter <y> for yes and <n> for no\n\n";
	cin.clear();
	cin.ignore(123, '\n');
	goto receive;
}

if (choice == 'y') {
	system("CLS");
	return true;
}
else {
	return false;
}
}
