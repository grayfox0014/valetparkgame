/*
   CITS1210 Project 1
   Name:		<Pearson, Scott>
   Student number:	<20954512>
   Date:		<16/09/11>
*/

#include "valetpark.h"

/****************************************************************/
/* Global variable definitions */
CARPARK carpark;

/* DO NOT ADD ANY OTHERS */

/****************************************************************/
/* Functions */

/*
   isValidLocation(l) returns true iff l represents a location within
   the bounds of the carpark.
*/
static bool isValidLocation(LOCATION l)
{
	if(l.row > carpark.nRows || l.col > carpark.nCols)
	{
		return false;
	}
	else if(l.row < 0 || l.col < 0)
	{
		return false;
	}
	
	return true;
}

/*
   isValidCarparkEntry(c) returns true iff c is a valid character that
   can appear in the "grid part" of a carpark-file.
*/
static bool isValidCarparkEntry(char c)
{
	int found = 0;

	for(int i = 0; i < carpark.nRows; i++)
	{
		for(int j = 0; j < carpark.nCols; j++)
		{	
			if(c == TARGETCAR || carpark.grid[i][j] == c)
			{
				found++;
			}
		}	
	}
	
	if(found == 0)
	{
		return false;
	}

	return true;
}

/*
   trimLine(line) removes any trailing new-line or carriage-return
   characters from the end of line.
*/
static void trimLine(char line[])
{
	int i = 0;
	
	while(line[i] != '\0') 
	{
		if(line[i] == '\r' || line[i] == '\n')
		{
			line[i] = '\0';
			break;
		}
	
		i++;
	}
}

/*
   readCarparkFile(filename) reads the contents of the file named
   filename as a carpark-file, "filling" the global carpark variable with
   information read from the file.  The function terminates the program
   if the file is not a valid carpark-file or if the corresponding grid
   would be too large to store in the global carpark variable.
*/
static void readCarparkFile(char filename[])
{
	FILE *fp;	// Create a file pointer
	char line[BUFSIZ];
	int i = 0;
	int j = 0;

	fp = fopen(filename, "r");	// Open the file and assign to file pointer
	
	if(fp == NULL)	// If file is empty then display error
	{
		fprintf(stderr, "The file %s: cannot be found\n", filename);
		exit(EXIT_FAILURE);
	}

	fscanf(fp, "EXIT: %d %d\n", &carpark.exit.row, &carpark.exit.col);	// scan through file for the exit location and assign to exit variables

	while(fgets(line, sizeof line, fp) != NULL)		// while not at the end of a line
	{
		if(strlen(line) > MAXCARPARKSIZE)	// if row is bigger than MAXCARPARKSIZE then display error
		{
			fprintf(stderr, "The file %s: is too large for the grid\n", filename);
			exit(EXIT_FAILURE);
		}
		
		if(i > MAXCARPARKSIZE)	// if col is bigger than MAXCARPARKSIZE then display error
		{
			fprintf(stderr, "The file %s: is too large for the grid\n", filename);
			exit(EXIT_FAILURE);
		}
		
		trimLine(line);	// trim the line
		
		j = 0;

		while(line[j] != '\0')	// while character does not equal last in row
		{	
			carpark.grid[i][j] = line[j];
			j++;
		}	
			
		i++;
		carpark.nCols = j;
	}
	
	carpark.nRows = i;
	fclose(fp); // close the file
}

/*
   getValue(l) returns the character representing the car segment
   occupying location l in the carpark.  The function terminates the program
   iff the location is not valid (i.e. outside the bounds of the carpark).
*/
static char getValue(LOCATION l)
{		
	if(!isValidLocation(l))	// check the location is valid
	{
		printf("The location l is not a valid carpark location\n");
		exit(EXIT_FAILURE);
	}
	
	return carpark.grid[l.row][l.col];
}

/*
   isEmpty(l) returns true iff location l is empty (i.e. no car segment
   resides at that location in the grid).
*/
static bool isEmpty(LOCATION l)
{
	if(getValue(l) != EMPTY)
	{
		return false;
	}
	
	return true;
}

/*
   findCar(car) locates car within the carpark, returning information
   about the location, orientation, and length of the car.  The function
   terminates the program iff the car is not located within the carpark.
*/
static CARINFO findCar(char car)
{
	CARINFO c;
	int rowLength = 1;
	int colLength = 1;

	for(int i = 0; i < carpark.nRows; i++)
	{
		for(int j = 0; j < carpark.nCols; j++)
		{
			if(carpark.grid[i][j] == car && rowLength == 1 && colLength == 1)	// assign the front of a car to CARINFO c
			{
				c.front.row = i;
				c.front.col = j;
			}
			
			if(carpark.grid[i][j] == car && carpark.grid[i][j] == carpark.grid[i][j+1])	// check the orientation is EASTWEST
			{
				c.orientation = EASTWEST;
				rowLength++;
			}
			
			if(carpark.grid[i][j] == car && carpark.grid[i][j] == carpark.grid[i+1][j])	// check the orientation is NORTHSOUTH
			{
				c.orientation = NORTHSOUTH;
				colLength++;
			}
		}
	}
	
	if(rowLength == 1 && colLength == 1)	// if the car cannot be located display an error
	{
		printf("The car %c: cannot be located in the carpark\n", car);
		exit(EXIT_FAILURE);
	}

	if(rowLength > colLength) // assign the length of the car
	{
		c.length = rowLength;
	}
	else if(colLength > rowLength)
	{
		c.length = colLength;
	}
	
	return c;
}

/*
 This method will check the orientation matches the direction specified
 and return true iff the orientation matches the direction and false iff
 otherwise
 */
static bool checkDirection(DIRECTION d, CARINFO c)
{
	if(c.orientation == NORTHSOUTH && d == EAST)	// check that the direction is valid
	{
		return false;
	}
	else if(c.orientation == NORTHSOUTH && d == WEST)
	{
		return false;
	}
	
	if(c.orientation == EASTWEST && d == NORTH)
	{
		return false;
	}
	else if(c.orientation == EASTWEST && d == SOUTH)
	{
		return false;
	}
	
	return true;
} 

/*
   hasExited(void) returns true if the target car has exited (i.e. any
   segment of the target car is at the exit location).
*/
static bool hasExited(void)
{	
	LOCATION l;
	l.row = carpark.exit.row;
	l.col = carpark.exit.col;

	if(getValue(l) != TARGETCAR)	// check that the exit location does not contain the TARGETCAR
	{
		return false;
	}

	return true;
}

/*
   printPrompt(void) prints the prompt for movement commands.
   This function is complete - DO NOT MODIFY IT.
*/
static void printPrompt(void)
{
	if (isatty(0))
	{
		static int moveno = 0;

		printf("move %d> ", ++moveno);
		fflush(stdout);
	}
}

/*
 setValue(l, value) sets location l in the carpark to value.
 The function terminates the program iff the location is not valid
 (i.e. outside the bounds of the carpark).  Additionally, unless the
 function is used to "free-up" the location (set the location to EMPTY),
 the function also terminates the program iff the location is already
 occupied by another car.
 */
static void setValue(LOCATION l, char value)
{
	if(!isValidLocation(l))	// check the location is valid and return error if not
	{
		printf("The location l is outside the bounds of the carpark\n");
		exit(EXIT_FAILURE);
	}	
	
	if(!isEmpty(l) && value != EMPTY)	// check the location is empty or the value is empty returning false if both are negative
	{
		printf("The location: %d %d is not valid\n", l.row, l.col);
		exit(EXIT_FAILURE);
	}
	
	carpark.grid[l.row][l.col] = value;
}

/*
 printCarpark(void) prints the current contents of the carpark, printing
 the rectangular grid of characters held in the global carpark variable.
 */
static void printCarpark(void)
{
	
/* Uncomment the following preprocessor directive if you want the 
 graphical interface under MacOSX in CSSE Lab 2.01 */

	/*
	#if defined(__APPLE__)
	carparkGUI();
	#endif		
	*/

	printf("Exit: %d %d\n", carpark.exit.row, carpark.exit.col);	// print the exit location
 
	for(int i = 0; i < carpark.nRows; i++)
	{
		for(int j = 0; j < carpark.nCols; j++)
		{
			printf("%c", carpark.grid[i][j]);	// print the grid
		}
		printf("\n");
	}

	printf("\n");
}

/*
 moveCarOneBay(car, d, c) attempts to move car by one bay in the
 direction of d, returning true iff the car was able to successfully
 complete the move.  The requested move is performed (and the carpark
 updated to reflect the move) if the car can move in the given
 direction, the movement does not take the car outside the bounds of
 the carpark, and there is no other car at the destination location.
 */
static bool moveCarOneBay(char car, DIRECTION d, CARINFO c)
{
	LOCATION l;

	if(!isValidCarparkEntry(car))	// check the car is defined in the carpark
	{
		return false;
	}
	
	if(!checkDirection(d, c))	// check the direction is valid
	{
		return false;
	}

	l.row = c.front.row;
	l.col = c.front.col;
	
	if(c.length == 2)	// if car length is equal to 2
	{
		/*
		 The following code moves the car relative to the direction specified
		 provided the car length is equal to 2.
		 */
		
		if(d == SOUTH && carpark.grid[c.front.row + 2][c.front.col] == EMPTY)
		{
			carpark.grid[c.front.row + 2][c.front.col] = car;
			setValue(l, EMPTY);
			c.front.row = c.front.row + 1;
			return true;
		}
		
		if(d == NORTH && carpark.grid[c.front.row - 1][c.front.col] == EMPTY)
		{	
			carpark.grid[c.front.row - 1][c.front.col] = car;
			l.row = c.front.row + 1;
			setValue(l, EMPTY);		
			c.front.row = c.front.row - 1;		
			return true;
		}

		if(d == EAST && carpark.grid[c.front.row][c.front.col + 2] == EMPTY)
		{
			carpark.grid[c.front.row][c.front.col + 2] = car;
			setValue(l, EMPTY);
			c.front.col = c.front.col + 1;
			return true;
		}
	
		if(d == WEST && carpark.grid[c.front.row][c.front.col - 1] == EMPTY)
		{
			carpark.grid[c.front.row][c.front.col - 1] = car;
			l.col = c.front.col + 1;
			setValue(l, EMPTY);
			c.front.col = c.front.col - 1;
			return true;
		}
	}

	if(c.length == 3)	// if car length is equal to 3
	{					
		/*
		 The following code moves the car relative to the direction specified
		 provided the car length is equal to 3.
		 */
		
		if(d == SOUTH && carpark.grid[c.front.row + 3][c.front.col] == EMPTY)
		{
			carpark.grid[c.front.row + 3][c.front.col] = car;
			setValue(l, EMPTY);
			c.front.row = c.front.row + 1;
			return true;
		}
		
		if(d == NORTH && carpark.grid[c.front.row - 1][c.front.col] == EMPTY)
		{	
			carpark.grid[c.front.row - 1][c.front.col] = car;
			l.row = c.front.row + 2;
			setValue(l, EMPTY);		
			c.front.row = c.front.row - 1;		
			return true;
		}

		if(d == EAST && carpark.grid[c.front.row][c.front.col + 3] == EMPTY)
		{
			carpark.grid[c.front.row][c.front.col + 3] = car;
			setValue(l, EMPTY);
			c.front.col = c.front.col + 1;
			return true;
		}
	
		if(d == WEST && carpark.grid[c.front.row][c.front.col - 1] == EMPTY)
		{
			carpark.grid[c.front.row][c.front.col - 1] = car;
			l.col = c.front.col + 2;
			setValue(l, EMPTY);
			c.front.col = c.front.col - 1;
			return true;
		}
	}				
	
	return false;
}

/*
   move(car, d, amount) attempts to move car by amount in the direction
   of d, returning the actual number of grid cells the car is able to
   move in that direction.  The requested movement is performed (and
   the carpark updated to reflect the movement) if the corresponding
   car can perform the requested move, otherwise no movement of the car
   is performed (and zero is returned).  The requested movement is also
   constrained to ensure the car does not collide with any other car
   and that the car remains within the bounds of the carpark.
*/
static int moveCar(char car, DIRECTION d, int amount)
{
	int move = 1;

	if(amount > carpark.nRows && amount > carpark.nCols)	// check amount is not greater than carpark size
	{
		return 0;
	}
	
	for(int i = 0; i < amount; i++)
	{	
		CARINFO c = findCar(car);	// update CARINFO with new car location
		moveCarOneBay(car, d, c);	// move the car
		move++;	// count users moves
	}
	
	return move;
}

/*
   processMoves(void) reads movement commands, processing each
   command, in turn to move the corresponding car within the carpark.
   Each movement command is constrained to ensure the corresponding car
   does not collide with any other car and that the car remains within
   the bounds of the carpark.  A movement command is performed (and the
   carpark updated to reflect the movement) if the corresponding car
   can move in the given direction.  At the completion of each movement
   command (regardless of whether the command lead to actual movement),
   the current state of the carpark is printed.  The function completes
   immediately if a movement command leads to the target car exiting.
   The function terminates the program if any command is invalid.
*/
static void processMoves(void)
{
	char line[BUFSIZ];
	char car;
	char direction;
	int amount;
	DIRECTION d;
	int i = 0;
	
	while(!hasExited())	// make sure the TARGETCAR is not at the exit location
	{
		printPrompt();	// prompt the user for a move
		fgets(line, sizeof line, stdin);
		trimLine(line);
		
		sscanf(line, "%c %c %d", &car, &direction, &amount);	// assign the users move to appropiate variables
		
		if(!isValidCarparkEntry(car))	// check the car is defined in the carpark
		{
			fprintf(stderr, "Fatal Error: car %c was not found in the grid\n", car);
			exit(EXIT_FAILURE);
		}

		/*
		 The following code sets the enum value from the user input
		 */
		
		if(direction == 'N' || direction == 'n')
		{
			d = NORTH;
		}
		else if(direction == 'S' || direction == 's')
		{
			d = SOUTH;
		}
		else if(direction == 'E' || direction == 'e')
		{
			d = EAST;
		}
		else if(direction == 'W' || direction == 'w')
		{
			d = WEST;
		}
		
		moveCar(car, d, amount);
		i = moveCar(car, d, amount);
		printf("Processed move: %c %c %d\n", car, direction, amount);
		printCarpark();
	}
	
	printf("The target car is now free after %d moves!\n", i);
}

/****************************************************************/
/* The main function */

int main(int argc, char *argv[])
{
	/* Exit with an error if the the number of arguments (including
	   the name of the executable) is not 2 */
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s carpark-file\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	else
	{
		/* Read the initial state of the carpark from file */
		readCarparkFile(argv[1]);
		printf("The initial carpark from '%s' is:\n", argv[1]);
	
		/* Print the contents of the carpark */
		printCarpark();

		/* Process user-provided moves, updating the state of
		   the carpark after each valid move */
		processMoves();

		/* Print the contents of the final carpark */
		printf("The final carpark is:\n");
		printCarpark();

		/* Finished successfully! */
		exit(EXIT_SUCCESS);
	}

	return 0;
}
