/************************************
Code/Design (50%): 94/100
	- No Input Validation (-5)
      I know you're planning to add this for the Game Show so I won't harp on it too much, but input validation
      that makes sure the player is attacking a square on the grid would be a great idea. It would also be critical
      to make sure that the ships get placed correctly and there are no ships that are too long, too short, or 
      overlapping with other ships.

    - Warnings at Compilation (-1)
      You get three warnings at compilation because the lines
        ship[2][0] == 0;    and     ship[2][i] == 0;        (lines 207, 230, and 248)
      aren't actually doing anything. Did you mean
        ship[2][0] = 0;     and     ship[2][i] = 0; ?

Demo Video (20%): 100/100
	- You do a great job of explaining this game and what makes it unique from the original Battleship!
    - M.L.: Nice demo video.
Webpage/Game Instructions (15%): 100/100
    - M.L.: The instructions are clear.

M.L.: This is an excellent game. The interface looks fantastic! 
      Great work! 
************************************/

//William Knoff, 12/8/22, Navy Battlegrounds game.
//This version of the game does not have input validation. I can add it in time for the game show on 12/13

#include <iostream>
#include <cstdlib>

void print(int grid[][8], int enemygrid[][8]); //prints the grid
void shipsetup(int ship[][5], int grid[][8], int identifier); //sets up ship positions
int turn(int grid[][8], int enemygrid[][8], int enemycarrier[][5], int enemybattle[][5], int enemysub[][5], int shiphits[], int & extralife);

int main(int argc, char const *argv[])
{
    std::cout << "Welcome to Navy Battlegrounds!\nThe objective is to sink your opponents ships before they can sink yours." << std::endl;
    std::cout << "Player 1 sets their three ships up first, then player 2 does the same. The position of your ships is visible to you but not your opponent" << std::endl;
    std::cout << "When you land a shot on an enemy ship, you are notified of which ship you hit, and it is marked on the grid as an 'X'. Misses are marked as 'O'." << std::endl;
    std::cout << "After a ship is shot five times, it is sunk. After all three of a players ships are sunk, they lose." << std::endl;
    std::cout << "\nPress enter to see each ships abilities.";
    while((std::cin.get()) != '\n');
    
    std::cout << "\nThe battleship and submarine have special abilities." << std::endl;
    std::cout << "Deep Water Camo (submarine): the first time your submarine is hit, the shot counts but the enemy is not notified of a hit." << std::endl;
    std::cout << "Last Stand (battleship): when down to one life, the battleship takes two shots to kill" << std::endl;
    std::cout << "\nPress enter to start the game.";
    while((std::cin.get()) != '\n');

    int gridstatus1[8][8]; //stores data on which ship fills which squares in [row][col] form
    int gridstatus2[8][8];
    //each ship has 1 row for its row coords (letters), column coords (#s), and whether that peg has been hit (0 or 1)
    int carrier1[2][5]; //identified as 1 on the grid
    int battleship1[2][5]; //identified as 2 on the grid
    int submarine1[2][5]; //identified as 3 on the grid
    int carrier2[2][5];
    int battleship2[2][5];
    int submarine2[2][5];
    int shiphits1[3] = {0, 0, 0}; //tracks health of player 1 ships: slot 1 is for carrier, slot 2 for battleship, slot 3 for submarine
    int shiphits2[3] = {0, 0, 0}; //tracks health of player 2 ships

    for(int i = 0; i < 8; i++) //initializing the grids
    {
        for(int j = 0; j < 8; j++)
        {
            gridstatus1[i][j] = '#';
            gridstatus2[i][j] = '#';
        }
    }

    int win = 0;
    int count = 0; //tracks turn
    int sunk1 = 0; //tracks shps sunk
    int sunk2 = 0;
    int backuplife1 = 1; //used for last stand ability
    int backuplife2 = 1;
    while(win == 0)
    {
        if(count == 0)
        {
            print(gridstatus1, gridstatus2); //prints from player 1 POV
            
            std::cout << "\n\nPlayer 1, enter the coords of each endpoint of your ships to position them. Each ship is five squares long (Ex. A 1, A 5 or A 1, E 1)." << std::endl;
            std::cout << "Enter the capital row letter (A-H) and column # (1-8) of carrier endpoint 1 separated by a space: ";
            shipsetup(carrier1, gridstatus1, 1);
            std::cout << "\nEnter the capital row letter (A-H) and column # (1-8) of battleship endpoint 1 separated by a space: ";
            shipsetup(battleship1, gridstatus1, 2);
            std::cout << "\nEnter the capital row letter (A-H) and column # (1-8) of submarine endpoint 1 separated by a space: ";
            shipsetup(submarine1, gridstatus1, 3);

            print(gridstatus1, gridstatus2); //prints from player 1 POV
            std::cout << "\nThis is your ship setup. 1 corresponds to carrier, 2 to battleship, and 3 to submarine." << std::endl;
            std::cout << "Press enter for player 2 to take their turn.";
            while((std::cin.get()) != '\n');
        }
        else if(count == 1)
        {
            print(gridstatus2, gridstatus1); //prints from player 1 POV

            std::cout << "\n\nPlayer 2, enter the coords of each endpoint of your ships to position them. Each ship is five squares long (Ex. A 1, A 5 or A 1, E 1)." << std::endl;
            std::cout << "Enter the capital row letter (A-H) and column # (1-8) of carrier endpoint 1 separated by a space: ";
            shipsetup(carrier2, gridstatus2, 1);
            std::cout << "\nEnter the capital row letter (A-H) and column # (1-8) of battleship endpoint 1 separated by a space: ";
            shipsetup(battleship2, gridstatus2, 2);
            std::cout << "\nEnter the capital row letter (A-H) and column # (1-8) of submarine endpoint 1 separated by a space: ";
            shipsetup(submarine2, gridstatus2, 3);

            print(gridstatus2, gridstatus1); //prints from player 2 POV
            std::cout << "\nThis is your ship setup. 1 corresponds to carrier, 2 to battleship, and 3 to submarine." << std::endl;
            std::cout << "Press enter for player 1 to take their turn.";
            while((std::cin.get()) != '\n');
        }
        else
        {
            if(count % 2 == 0) //player 1 goes
            {
                sunk2 = turn(gridstatus1, gridstatus2, carrier2, battleship2, submarine2, shiphits2, backuplife2); //running turn
                std::cout << "\nTotal ships sunk: " << sunk2 << "/3"; 

                if(sunk2 == 3) //checking for a win
                {
                    win = 1;
                    break;
                }

                std::cout << "\nPress enter for player 2 to take their turn.";
                while((std::cin.get()) != '\n');
            }
            else //player 2 goes
            {
                sunk1 = turn(gridstatus2, gridstatus1, carrier1, battleship1, submarine1, shiphits1, backuplife1);
                std::cout << "\nTotal ships sunk: " << sunk1 << "/3"; 

                if(sunk1 == 3) //checking for a win
                {
                    win = 2;
                    break;
                }
                
                std::cout << "\nPress enter for player 2 to take their turn.";
                while((std::cin.get()) != '\n');
            }     
        }

        count +=1;
        system("CLS"); //clearing terminal so you can't see opponent's ships
    }
    
    //prints when a player wins
    std::cout << "\nPlayer " << win << " wins!" << std::endl;
    if(win == 1)
        print(gridstatus1, gridstatus2);
    else
        print(gridstatus2, gridstatus1);
    
    return 0;
}

void print(int grid[][8], int enemygrid[][8])
{
    std::cout << "Your ship grid:\t\t\t\tEnemy ship grid:" << std::endl;
    
    for(int i = 0; i < 2; i++) //Prints column labels
    {
        std::cout << "   ";
        for(int i = 1; i < 9; i++)
        {
            std::cout << "| " << i << " ";
        }
        std::cout << "\t";
    }

    for(int row = 'A'; row < 'I'; row++)
    {
        std::cout << "\n---|---|---|---|---|---|---|---|---\t---|---|---|---|---|---|---|---|---" << std::endl;
        std::cout << " " << char(row) << " "; //prints row labels
        for(int col = 1; col < 9; col++)
        {
            if(char(grid[row - 'A'][col-1]) == '#') //square not shot yet
                std::cout << "| # ";
            else if(grid[row - 'A'][col-1] == -1 || grid[row - 'A'][col-1] == -2 || grid[row - 'A'][col-1] == -3) //hit
                std::cout << "| X ";
            else if(grid[row - 'A'][col-1] < 0) //miss
                std::cout << "| O ";
            else
                std::cout << "| " << grid[row - 'A'][col - 1] << " "; //prints your ship #s
        }
        std::cout << "\t";
        std::cout << " " << char(row) << " ";
        for(int col = 1; col < 9; col++)
        {
            if(enemygrid[row - 'A'][col-1] > 0) //don't know status of this enemy square
                std::cout << "| ? ";
            else if(enemygrid[row - 'A'][col-1] == -1 || enemygrid[row - 'A'][col-1] == -2 || enemygrid[row - 'A'][col-1] == -3)
                std::cout << "| X "; //hit on enemy ship
            else
                std::cout << "| O "; //missed enemy ships
        }
    }
}

void shipsetup(int ship[][5], int grid[][8], int identifier)
{
    int tempswap;
    char chartoint; //used to read a char into an int var without error

    std::cin >> chartoint; //row
    ship[0][0] = int(chartoint);
    std::cin >> ship[1][0]; //col
    ship[2][0] == 0; //signifies not hit

    std::cout << "Enter the capital row letter (A-H) and column # (1-8) of endpoint 2 separated by a space: ";
    std::cin >> chartoint; //row
    ship[0][4] = chartoint;
    std::cin >> ship[1][4];
    while((std::cin.get()) != '\n');

    if(ship[0][0] == ship[0][4]) //if row endpoint == row other endpoint, the ship is horizontal
    {
        //if statements changes iteration direction based on which endpoint was entered first
        //if first endpoint column is greater than second endpoint column, reverse the columns so the ship reads left to right (12345 or ABCDE)
        if(ship[1][4] < ship[1][0]) 
        {
            tempswap = ship[1][4];
            ship[1][4] = ship[1][0];
            ship[1][0] = tempswap;
        }

        for(int i = 1; i < 4; i++)
        {
            ship[0][i] = ship[0][0]; //making each row value the same
            ship[1][i] = ship[1][0] + i;
            ship[2][i] == 0; //signifies not hit
        }
    }
    else //ship is vertical
    {
        //if first endpoint row is greater than second endpoint column, reverse the columns so the ship reads left to right
        //ex. resulting ship[0] (rows) should read ex. 12345 and ship[1] (cols) should read ex. ABCDE
        if(ship[0][4] < ship[0][0]) 
        {
            tempswap = ship[0][4];
            ship[0][4] = ship[0][0];
            ship[0][0] = tempswap;
        }
        
        for(int i = 1; i < 4; i++)
        {
            ship[1][i] = ship[1][0]; //making each col value the same
            ship[0][i] = ship[0][0] + i;
            ship[2][i] == 0; //signifies not hit
        }
    }

    for(int i = 0; i < 5; i++)
    {
        grid[ship[0][i] - 'A'][ship[1][i]-1] = identifier;
    }
}

int turn(int grid[][8], int enemygrid[][8], int enemycarrier[][5], int enemybattle[][5], int enemysub[][5], int shiphits[], int & extralife)
{
    char attackrow;
    int attackcol;
    int sunkships = 0;
    int templife = 0; //used for last stand ability

    print(grid, enemygrid); //prints from player 1 POV
    
    //reading in attack square
    std::cout << "\nEnter the the capital row letter (A-H) and column # (1-8) where you want to attack: ";
    std::cin >> attackrow;
    std::cin >> attackcol;
    while((std::cin.get()) != '\n');

    if(enemygrid[attackrow - 'A'][attackcol - 1] == 1)
        shiphits[0] += 1;
    else if(enemygrid[attackrow - 'A'][attackcol - 1] == 2)
    {
        if(shiphits[1] == 4 && extralife == 1) //used for last stand ability
        {
            extralife = 0;
            templife = 1;
            std::cout << "Shot blocked by last stand ability. Another shot there will sink the battleship!" << std::endl;
            std::cout << "\nPress enter to continue.";
            while((std::cin.get()) != '\n');
        }
        else
            shiphits[1] += 1;
    }
    else if(enemygrid[attackrow - 'A'][attackcol - 1] == 3)
        shiphits[2] += 1;

    if(enemygrid[attackrow - 'A'][attackcol - 1] > 0) //prevents the user from marking a spot as unhit by hitting it a second time
        enemygrid[attackrow - 'A'][attackcol - 1] *= -1; //negative value means a hit
    if(templife == 1) //used for last stand ability
    {
        enemygrid[attackrow - 'A'][attackcol - 1] *= -1;
        templife = 0;
    }

    std::cout << "\n";
    print(grid, enemygrid); //prints from player 1 POV

    if(enemygrid[attackrow - 'A'][attackcol - 1] == -1) //enemy carrier hit
        if(shiphits[0] != 5)
            std::cout << "\nHit on enemy carrier!";
        else
            std::cout << "\nEnemy carrier sunk!";
    else if(enemygrid[attackrow - 'A'][attackcol - 1] == -2) //enemy battleship hit
        if(shiphits[1] != 5)
            std::cout << "\nHit on enemy battleship!";
        else
            std::cout << "\nEnemy battleship sunk!";
    else if(enemygrid[attackrow - 'A'][attackcol - 1] == -3) //enemy sub hit
    {
        if(shiphits[2] == 5)
            std::cout << "\nEnemy submarine sunk!";
        else if(shiphits[2] != 1)
            std::cout << "\nHit on enemy submarine!";
    }

    for(int i = 0; i < 3; i++) //keeps track of how many ships have been sunks
    {
        if(shiphits[i] == 5)
            sunkships += 1;
    }

    return sunkships;
}
