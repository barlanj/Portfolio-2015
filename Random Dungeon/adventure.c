/*********************************************************
 * filename: adventure.c
 * Author: barlanj
 * Description: A text adventure game
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// global constants
#define MAX_ROOMS 7
#define MAX_DOORS_PER_ROOM 6
#define MIN_DOORS_PER_ROOM 3
#define MAX_STEPS 30

// For Terminal Text coloring
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


// forward declare of structs
typedef struct Room Room;
typedef struct GameData GameData;
typedef struct FileRoom FileRoom;
typedef struct FileCollection FileCollection;

// the different types of rooms
typedef enum
{
    start_room = 0,
    mid_room = 1,
    end_room = 2,
} room_type;

/***************************************************
 * Room struct - used to create a room object
 * for iterating over to save into files
 ***************************************************/
struct Room
{
    char* room_name;
    room_type type;
    int doorCount;
    Room* doors_list[MAX_DOORS_PER_ROOM];
};

/***************************************************
 * GameData struct - contains Room structs. This
 * is used to manage the generated rooms.
 * Note: Can also be used to run the main loop
 * without loading files (not used for this purpose
 * in this main loop)
 ***************************************************/
struct GameData
{
    char path[100];
    int steps;
    Room* rooms[MAX_ROOMS];
    Room* currentRoom;
    Room* endRoom;
};

/***************************************************
 * FileRoom struct - used to load files saved
 * locally.
 ***************************************************/
struct FileRoom
{
    char* filepath;
    char* room_name;
    char* door_list[MAX_DOORS_PER_ROOM];
    char* type;
    int doorCount;
};

/***************************************************
 * FileCollection - used to manage the FileRoom
 * structs. This is also used in this game's main
 * loop to keep track of the session data.
 ***************************************************/
struct FileCollection
{
    FileRoom* rooms[MAX_ROOMS];
    FileRoom* currentRoom;
    int steps;
    FileRoom* beginning;
    FileRoom* end;
    FileRoom* history[MAX_STEPS];

};
/** [FUNCTION PROTOTYPES] *******************************************************/

void initFileCollection( FileCollection* fc);
void initGame( GameData* data );
void generatePaths( GameData* data );
void initRoom( Room* room, const char * name );
void connectRooms( Room* roomA, Room* roomB );
int hasConnection( Room* roomA, Room* roomB );
void saveData( GameData* data );
void saveRooms( GameData* data );
FileRoom* getRoom( FileCollection* data, const char* roomName );
void initFileRoom( FileRoom* room, const char * filepath, const char * name, const char * type, int count);
void createListOfRooms (const char* path, FileCollection* fc);
char * getStringRoomType( Room* room );
char* removeKeywords( char line[], const char* keyword, int extraRemove );
void shuffle(char** arr, int length );

/*****[ MAIN LOOP ] ************************************************************/
int main()
{
    char userInput[100];
    srand( time( NULL ) );

    // generate the game files.
    GameData* gd = malloc( sizeof( GameData ) );
    initGame( gd );
    saveData( gd );

    // load the files
    FileCollection* fc = malloc(sizeof(FileCollection));
    initFileCollection(fc);
    createListOfRooms(gd->path, fc);

    // set the current position and the ending position (for checking)
    FileRoom* currentRoom = fc->beginning;
    FileRoom* endRoom = fc->end;

    //bolean checks
    int pathFound = 0; // if winning path found
    int roomFound = 0; // if requested room exist

    do {

        // Show user Information about current room
        printf(ANSI_COLOR_GREEN "CURRENT LOCATION: %s \n" ANSI_COLOR_RESET, currentRoom->room_name);
        printf("POSSIBLE CONNECTIONS: \n");

        int i;
        for(i = 0; i < currentRoom->doorCount; i++)
        {
            printf("  %s", currentRoom->door_list[i]);
            if(i < currentRoom->doorCount - 1)
                printf(",\n");
            else
                printf(".\n");
        }

        // Ask User where to go
        printf("WHERE TO?>" ANSI_COLOR_MAGENTA);
        fgets(userInput, 255, stdin);
        printf(ANSI_COLOR_RESET);

        //remove trailing newline
        if ((strlen(userInput)>0) && (userInput[strlen (userInput) - 1] == '\n'))
            userInput[strlen (userInput) - 1] = '\0';

        // Check if the user input is a valid room
        for(i = 0; i < currentRoom->doorCount; i++)
        {
            if(strcmp(currentRoom->door_list[i], userInput) == 0)
            {
                roomFound = 1;
                break;
            }
        }

        // if room is valid then load and update game session data
        if(roomFound)
        {
            currentRoom = getRoom(fc, userInput);
            fc->history[fc->steps] = currentRoom;
            fc->steps++;
        }
        else
        {
            // if invalid, then prompt user again
            printf(ANSI_COLOR_RED "\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN. \n" ANSI_COLOR_RESET);
        }

        // check for winning state
        if(currentRoom == endRoom)
        {
            printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS! \n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", fc->steps);

            for(i = 0; i < fc->steps; i++)
                printf("%s \n", fc->history[i]->room_name);

            pathFound = 1;
        }


        //check if above steps allowed
        if( pathFound == 0 && fc->steps >= MAX_STEPS)
        {
            printf(ANSI_COLOR_RED "\nA WILD GEODUDE FELL ON YOUR HEAD. \n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR_RED "\nYOU DIED! \n" ANSI_COLOR_RESET);
            pathFound = 1;
        }

        // reset bool checks
        roomFound = 0;
        printf("\n\n");
    } while (!pathFound);


    //free all mallocs
    int i;
    for(i = 0; i < MAX_ROOMS; i++)
    {
        free(gd->rooms[i]);
        free(fc->rooms[i]);
    }

//    free(fc);
//    free(gd);

    return 0;
}


/**** [ FUNCTION DEFINITIONS ] *************************************************************/


/***************************************************************************
 * initFileCollection
 * Parameter: FileCollection - the struct to initialize
 * Description: This function initializes the given FileCollection object
 ***************************************************************************/
void initFileCollection( FileCollection* fc)
{
    int i;
    for(i = 0; i < MAX_ROOMS; i++)
        fc->rooms[i] = malloc( sizeof( FileRoom ) );

    fc->steps = 0;
    fc->beginning = malloc(sizeof(FileRoom));
    fc->end = malloc(sizeof(FileRoom));
    fc->currentRoom = malloc(sizeof(FileRoom));
}

/***************************************************************************
 * initGame
 * Parameter: GameData - the struct to initialize
 * Description: This function initializes the given GameData object.
 * This is used to create the rooms. This will call the function
 * generatePath to create paths between rooms.
 ***************************************************************************/
void initGame( GameData* data )
{
    char* roomNames[10];
    // insert names into array
    roomNames[0] = "Holy Ground";
    roomNames[1] = "Far Thunder";
    roomNames[2] = "Spiral";
    roomNames[3] = "Great Seal";
    roomNames[4] = "Nothingness";
    roomNames[5] = "Touchstone";
    roomNames[6] = "Twin Hills";
    roomNames[7] = "Fertile Land";
    roomNames[8] = "Melody";
    roomNames[9] = "Remnant";

    // shuffle arrays for randomization
    int n;
    n = sizeof( roomNames ) / sizeof( roomNames[0] );
    shuffle( roomNames, n );
    //init the current Room
    data->currentRoom = malloc( sizeof( Room ) );
    //create the rooms (random name)
    int i;

    for( i = 0; i < MAX_ROOMS; i++ )
    {
        data->rooms[i] = malloc( sizeof( Room ) );
        initRoom( data->rooms[i], roomNames[i] );
    }

    //setup the start and end rooms
    data->rooms[0]->type = start_room;
    data->rooms[MAX_ROOMS - 1]->type = end_room;
    //set up the game first position
    data->currentRoom = data->rooms[0];
    data->endRoom = data->rooms[MAX_ROOMS - 1];
    //make a winning path
    generatePaths( data );
    data->steps = 0;
}

/***************************************************************************
 * generatePaths
 * Parameter: GameData (already initialized and must contain a rooms)
 * Description: This function will create a winning path between the
 * START and END rooms, as well as generate random doors between
 * other rooms.
 ***************************************************************************/
void generatePaths( GameData* data )
{
    int i = 0, j = 0, k = 0, l = 0;
    // get the number of rooms between start and end
    int deadCount = rand() % ( MAX_ROOMS / 2 );
    int pathCount = MAX_ROOMS - 2 - deadCount;

    for( i = 0; i < pathCount; i++ )
    {
        connectRooms( data->rooms[i], data->rooms[i + 1] );
    }

    // link the last room to the end room
    connectRooms( data->rooms[i], data->rooms[MAX_ROOMS - 1] );

    // add more doors to each room
    // check each room and see if they have at least 3 doors
    for( i = 0; i < MAX_ROOMS; i++ )
    {
        //if there's less than the min, add more doors until min reached
        while( data->rooms[i]->doorCount < MIN_DOORS_PER_ROOM )
        {
            k =  rand() % MAX_ROOMS;
            connectRooms( data->rooms[i], data->rooms[k] );
        }

        if( data->rooms[i]->doorCount >= MIN_DOORS_PER_ROOM )
        {
            //if greater than min and less than max
            if( data->rooms[i]->doorCount < MAX_DOORS_PER_ROOM )
            {
                //add some random amount of doors
                //addition of door is not guaranteed (if random room already connected)
                l = rand() % ( MAX_ROOMS - data->rooms[i]->doorCount );

                for( j = 0; j < l; j++ )
                {
                    k =  rand() % MAX_ROOMS;
                    connectRooms( data->rooms[i], data->rooms[k] );
                }
            }
        }
    }
}

/***************************************************************************
 * initRoom
 * Parameter: room - a room struct to initialize
 *            name - the room's name to give the object
 * Description: This function initializes a given room struct
 ***************************************************************************/
void initRoom( Room* room, const char * name )
{
    int buffer_size = 50;
    room->room_name = malloc( sizeof( char ) * buffer_size );
    sprintf( room->room_name, "%s", name );
    room->type = mid_room;
    room->doorCount = 0;
}


/***************************************************************************
 * connectRooms
 * Parameter: roomA - the first room
 *            roomB - the second room
 * Description: This function create a door/connection between two room.
 * This will only generate a connection if the rooms have available
 * space to add dorrs (both must have it), as well as be different rooms.
 ***************************************************************************/
void connectRooms( Room* roomA, Room* roomB )
{
    // make sure we are not over the limit
    if( roomA->doorCount >= MAX_DOORS_PER_ROOM || roomB->doorCount >= MAX_DOORS_PER_ROOM )
    {
        return;
    }

    //check if it already has connection
    if( hasConnection( roomA, roomB ) )
    {
        return;
    }

    //if no connection exist, then connect it
    roomA->doors_list[roomA->doorCount] = roomB; // add roomB to roomA list
    roomA->doorCount++;
    roomB->doors_list[roomB->doorCount] = roomA; // add roomA to roomB list
    roomB->doorCount++;
    return;
}

/***************************************************************************
 * hasConnection
 * Parameter: roomA - the first room
 *            roomB - the second room
 * Description: This will check if there is a door/connection that connects
 * the two given room. Will return 1 (true) if found. Note that this
 * will return true if room A and room B is the same identical rooms.
 * This function should be used inside connectRooms().
 ***************************************************************************/
// check if there exist a door between two rooms
int hasConnection( Room* roomA, Room* roomB )
{
    //check if roomA and roomB are the same room
    if( roomA == roomB )
    {
        return 1;
    }

    int i;

    for( i = 0; i < roomA->doorCount; ++i )
    {
        if( roomA->doors_list[i] == roomB )
            return 1;
    }

    return 0;
}

/***************************************************************************
 * saveData
 * Parameter: GameData - the struct containing all the generated
 *            data (rooms)
 * Description: This function will create a folder in the root director of
 * the executable. The folder is named "barlanj.rooms.<PID>" with PID being
 * the processID. This will call saveRooms() to create the individual
 * room files.
 ***************************************************************************/
void saveData( GameData* data )
{
    int process_id = getpid();
    sprintf( data->path, "barlanj.rooms.%d", process_id );
    //create the directory
    mkdir( data->path, 0700 );
    saveRooms( data );
}

/***************************************************************************
 * saveRooms
 * Parameter: GameData - the struct containing info about the rooms
 * Description: This function will generate files pertaining
 * to the rooms generated inside the GameData struct. This function
 * should be called only after saveData has finished creating a folder.
 * This should only be called inside saveData() to prevent errors.
 ***************************************************************************/
void saveRooms( GameData* data )
{
    FILE* pFile;
    char filename[50];
    char lineBuffer[50];
    char* typeBuffer;
    //create the files
    int i, j;

    for( i = 0; i < MAX_ROOMS; i++ )
    {
        //sprintf( filename, "%s/%s_%d", data->path, "room", i + 1 );
        sprintf( filename, "%s/room_%d", data->path, i+1 );
        pFile = fopen( filename, "w" );

        if( pFile != NULL )
        {
            //room name
            sprintf( lineBuffer, "ROOM NAME: %s\n", data->rooms[i]->room_name );
            fputs( lineBuffer, pFile );

            //connections
            for( j = 0; j < data->rooms[i]->doorCount; j++ )
            {
                sprintf( lineBuffer, "CONNECTION %d: %s\n", j + 1, data->rooms[i]->doors_list[j]->room_name );
                fputs( lineBuffer, pFile );
            }

            //room type
            typeBuffer = getStringRoomType( data->rooms[i] );
            sprintf( lineBuffer, "ROOM TYPE: %s\n", typeBuffer );
            fputs( lineBuffer, pFile );
            //close the file
            fclose( pFile );
        }
    }
}


/***************************************************************************
 * getRoom
 * Parameter:  FileCollection - the struct containing the FileRooms
               roomName - the room to search retrieve.
 * Description: This functions will search the FileCollection for the
 *              wanted room. This will return a pointer to the room.
 * Other Notes: This should be changed to receive GameData struct if using
 *              GameData for the main loop of the game.
 ***************************************************************************/
FileRoom* getRoom( FileCollection* data, const char* roomName )
{
    int i;

    for( i = 0; i < MAX_ROOMS; i++ )
    {
        if( strcmp( data->rooms[i]->room_name, roomName ) == 0 )
        {
            return data->rooms[i];
        }
    }

    return NULL;
}

/***************************************************************************
 * initFileRoom
 * Parameter: room - a FileRoom to initialize
 *            filepath - a char array containing the file's path
 *            name - the room's name
 *            type - a char array containing the type {MIDDLE_ROOM, END_ROOM, START_ROOM}
 *            count - the number of doors/connection to other rooms
 * Description: This function initializes the given FileRoom struct.
 ***************************************************************************/
void initFileRoom( FileRoom* room, const char * filepath, const char * name, const char * type, int count)
{
    int buffer_size = 50;
    room->filepath = malloc( sizeof( char ) * buffer_size );
    sprintf( room->filepath, "%s", filepath );

    room->room_name = malloc( sizeof( char ) * buffer_size );
    sprintf( room->room_name, "%s", name );

    room->type = malloc( sizeof( char ) * buffer_size );
    sprintf( room->type, "%s", type );

    room->doorCount = count;
}

/***************************************************************************
 * createListOfRooms
 * Parameter: path - the folder path generated by saveData()
 *            fc - the FileCollection struct to contains the rooms loaded
 *                 from local files
 * Description: This function geneates FileRooms and attaches them to the
 *              FileCollection. The FileRooms' data are populated from
 *              files saved locally by saveData().
 ***************************************************************************/
void createListOfRooms (const char* path, FileCollection* fc)
{
    FILE* file;
    char filepath[100];
    int doorCount;
    char type[20];
    char* ret_str;
    char name[20];
    char line[255];
    char keywords[20];

    int i, j;
    for(i=0; i < MAX_ROOMS; i++)
    {
        doorCount=0;
        sprintf(filepath, "%s/room_%d", path, i+1);

        //open the file
        file = fopen(filepath, "r");

        //get the first line
        while(fgets(line, sizeof(line), file) != NULL) {

            //remove the newline char
            char *pos;
            if ((pos=strchr(line, '\n')) != NULL)
                *pos = '\0';

            //check for keywords and remove them
            if(strstr(line, "ROOM NAME:"))
            {
                ret_str = removeKeywords(line, "ROOM NAME: ", 0);
                sprintf(name, "%s", ret_str);
            }
            else if (strstr(line, "CONNECTION "))
            {
                ret_str = removeKeywords(line, "CONNECTION", 4);
                char* str = malloc(sizeof(char) * strlen(ret_str) + 1);
                sprintf(str, "%s", ret_str);
                fc->rooms[i]->door_list[doorCount] = str;
                doorCount++;
            }
            else if (strstr(line, "ROOM TYPE:"))
            {
                ret_str = removeKeywords(line, "ROOM TYPE: ", 0);
                sprintf(type, "%s", ret_str);

                // setup the start and end rooms
                if(strcmp(ret_str, "START_ROOM") == 0)
                    fc->beginning = fc->rooms[i];
                else if (strcmp(ret_str, "END_ROOM") == 0)
                    fc->end = fc->rooms[i];
           }
        }

        //close the file
        fclose(file);

        //create the FileRoom with the retrieved data
        initFileRoom(fc->rooms[i], filepath, name, type, doorCount);
    }

}

/***************************************************************************
 * getStringRoomType
 * Parameter: room - the room which contains the type
 * Description: This function retrieves the enumerated room type and
 *              returns a string equivalent.
 ***************************************************************************/
char * getStringRoomType( Room* room )
{
    char* typeBuffer = ( char * )malloc( sizeof( char ) * 20 );

    if( room->type == 1 )
        sprintf( typeBuffer, "MID_ROOM" );
    else if( room->type == 0 )
        sprintf( typeBuffer, "START_ROOM" );
    else
        sprintf( typeBuffer, "END_ROOM" );

    return typeBuffer;
}

/***************************************************************************
 * removeKeywords
 * Parameter: line - a string to be searched
 *             keyword - the word to search in 'line'
 *             extraRemove - string position to add to keyword length.
 * Description: This is a utility function to remove keywords from a
 *              given string. This will return a pointer to the new
 *              string (without the keyword and any char in the extra
 *              positions)
 ***************************************************************************/
char* removeKeywords( char line[], const char* keyword, int extraRemove )
{
    //char* str = malloc(sizeof(char) * 50);
    char* str = malloc( sizeof( char ) * 20 );
    char c;
    int i, j;
    char* p = strstr( line, keyword );

    //strcpy(str, "");
    if( p != NULL )
    {
        i = strlen( keyword ) + extraRemove;
        j = 0;
        c = line[i];

        while( c != '\0' )
        {
            str[j] = line[i];
            i++;
            j++;
            c = line[i];
        }

        str[j] = '\0';
    }

    return str;
}


/***************************************************************************
 * swap
 * This is a utility function for shuffle(). This swaps two character by
 * reference.
 ***************************************************************************/
void shuffle( char* arr[], int length )
{
    int i, n = length;

    for( i = n - 1; i > 0; i-- )
    {
        int j = rand() % ( i + 1 );

        char* temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}
