//  Created by Sophia Chang on 11/11/22.

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// this function works with the sqlite3_exec() function
// first parameter is data provided in the fourth parameter of the exec function which is usually not used, second parameter is the number of columns in th result, third parameter is the array of strings representing fields in the row, last parameter is the array of strings representing column names

int dontprintstatement = 0;
int countid;

int callback(void *NotUsed, int numcolumns, char **datatext, char **namecolumns)
{
    NotUsed = 0;
    
    if (dontprintstatement != 0)
        countid = atoi(datatext[numcolumns-1]);
    else
        // prints out the database
        for (int i = 0; i < numcolumns; i++)
        {
            printf("%s = %s\n", namecolumns[i], datatext[i] ? datatext[i] : "NULL"); // if there's no datatext[i], put NULL, else, print out string in datatext[i]
        }
    
    printf("\n");
    return 0;
}

int dontprintcallback(void *NotUsed, int numcolumns, char **datatext, char **namecolumns)
{
    NotUsed = 0;
    printf("%s", datatext[numcolumns-1]);
    return 0;
}

sqlite3 *opendatabaseconnection(void)
{
    // open up the database and get it connected
    sqlite3 *db; // used to get version of the SQLite library
    int status = sqlite3_open("mydata.db", &db); // opens the database connection
    if (status != SQLITE_OK)
    {
        printf("Error, cannot open: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return NULL;
    }
    return db;
}

/*
bool executesqlcommand(char *name)
{
    return true;
}*/

bool createtable(sqlite3 *db)
{
    // find/create the table list of tasks "DROP TABLE TODO;"
    char *createtable = "CREATE TABLE IF NOT EXISTS TODO(ID INT, TASK TEXT, STATUS TEXT);";
    int status = sqlite3_exec(db, createtable, callback, 0, 0); // this line speaks for all three commands of sqlite_prepare_test1(), sqlite3_step(), and sqlite3_finalize(), which prep the execution of commands; it basically evaluates the SQL statement
    if (status != SQLITE_OK)
    {
        printf("Error for finding/creating table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    
    return true;
}

bool seetable(sqlite3 *db)
{
    // select all the tasks
    char *selecttable = "SELECT * FROM TODO;";
    printf("\nTODO LIST:\n\n");
    int status = sqlite3_exec(db, selecttable, callback, 0, 0);
    if (status != SQLITE_OK)
    {
        printf("Error for selecting all rows in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    
    return true;
}


bool addtask(sqlite3 *db)
{
    // get input from user for a task
    char task[200];
    scanf("%[^\n]s", task);
    
    // insert the task into the table
    char *addtask = "INSERT INTO TODO(ID, TASK, STATUS) SELECT (COUNT(ID) + 1), ?, 'INCOMPLETE' FROM TODO;";
    sqlite3_stmt *stmt; // represents a single SQL statement that has been compiled into binary form and is ready to be evaluated
    
    // make sure addtask statement executes
    int status = sqlite3_prepare_v2(db, addtask, -1, &stmt, NULL);
    if (status != SQLITE_OK)
    {
        printf("Error for inserting task in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    else
    {
        if (sqlite3_bind_text(stmt, 1, task, -1, SQLITE_STATIC) != SQLITE_OK)
        {
            printf("Error in binding text: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // evaluates sql statement
        status = sqlite3_step(stmt);
        if (status != SQLITE_DONE) // indicates operation ran successfully, sqlite_done is mainly used for step commands
        {
            printf("Error in step: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // deletes the prepared statement
        status = sqlite3_finalize(stmt);
        if (status != SQLITE_OK)
        {
            printf("Error in finalize: %s", sqlite3_errmsg(db));
            return false;
        }
    }
    
    // display table
    seetable(db);
    
    printf("Task successfully added!\n");
    
    return true;
}

bool completestatus(sqlite3 *db, int id)
{
    // make a SQL statement to get it edited
    char *edited = "UPDATE TODO SET STATUS = 'COMPLETE' WHERE ID = ?;";
    sqlite3_stmt *stmt; // represents a single SQL statement that has been compiled into binary form and is ready to be evaluated
    
    // make sure edited statement executes
    int status = sqlite3_prepare_v2(db, edited, -1, &stmt, NULL);
    if (status != SQLITE_OK)
    {
        printf("Error for editing task in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    else
    {
        if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK)
        {
            printf("Error in binding integer: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // evaluates sql statement
        status = sqlite3_step(stmt);
        if (status != SQLITE_DONE) // indicates operation ran successfully, sqlite_done is mainly used for step commands
        {
            printf("Error in step: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // deletes the prepared statement
        status = sqlite3_finalize(stmt);
        if (status != SQLITE_OK)
        {
            printf("Error in finalize: %s", sqlite3_errmsg(db));
            return false;
        }
    }
    
    return true;
}

bool incompletestatus(sqlite3 *db, int id)
{
    // make a SQL statement to get it edited
    char *edited = "UPDATE TODO SET STATUS = 'INCOMPLETE' WHERE ID = ?;";
    sqlite3_stmt *stmt; // represents a single SQL statement that has been compiled into binary form and is ready to be evaluated
    
    // make sure edited statement executes
    int status = sqlite3_prepare_v2(db, edited, -1, &stmt, NULL);
    if (status != SQLITE_OK)
    {
        printf("Error for editing task in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    else
    {
        if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK)
        {
            printf("Error in binding integer: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // evaluates sql statement
        status = sqlite3_step(stmt);
        if (status != SQLITE_DONE) // indicates operation ran successfully, sqlite_done is mainly used for step commands
        {
            printf("Error in step: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // deletes the prepared statement
        status = sqlite3_finalize(stmt);
        if (status != SQLITE_OK)
        {
            printf("Error in finalize: %s", sqlite3_errmsg(db));
            return false;
        }
    }
    
    return true;
}


bool editstatus(sqlite3 *db)
{
    // ask which task to edit
    int id;
    printf("Edit status to COMPLETE/INCOMPLETE of which # task? ID: ");
    
    // make sure id is an integer
    if (scanf("%d", &id) != 1) // if it returns the correct type, it will return the value 1
    {
        printf("Failed to proceed. Response was not a number. Please try again.\n");
        return false;
    }
    
    // get number of id's in the table
    char *clarifyidworks = "SELECT COUNT(id) FROM TODO;";
    dontprintstatement = 1;
    int status = sqlite3_exec(db, clarifyidworks, callback, 0, 0);
    dontprintstatement = 0;
    
    if (status != SQLITE_OK)
    {
        printf("Error for counting all id(s) in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    
    // make sure the number of id's user input is less than equal to the number of id's in the table
    if (countid < id)
    {
        printf("Input is more than number of tasks you have put in. Please try again.\n");
        return false;
    }
    
    // ask user if you want it completed or not
    char change[2];
    printf("Switch status to COMPLETE? (Y/N) ");
    scanf("%s", &change);
    
    if (*change == 'Y' || *change == 'y')
        completestatus(db, id);
    else
        incompletestatus(db, id);
    
    // display table
    seetable(db);
    
    return true;
}

bool edittask(sqlite3 *db)
{
    // ask which task to edit
    int id;
    printf("Edit which # task? ID: ");
    
    // make sure id is an integer
    if (scanf("%d", &id) != 1) // if it returns the correct type, it will return the value 1
    {
        printf("Failed to proceed. Response was not a number. Please try again.\n");
        return false;
    }
    
    // get number of id's in the table
    char *clarifyidworks = "SELECT COUNT(id) FROM TODO;";
    dontprintstatement = 1;
    int status = sqlite3_exec(db, clarifyidworks, callback, 0, 0);
    dontprintstatement = 0;
    
    if (status != SQLITE_OK)
    {
        printf("Error for counting all id(s) in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    
    // make sure the number of id's user input is less than equal to the number of id's in the table
    if (countid < id)
    {
        printf("Input is more than number of tasks you have put in. Please try again.\n");
        return false;
    }
    
    // edit the task
    char newtask[200];
    printf("Print out your new task: ");
    scanf(" %[^\n]s", newtask); // FIX THIS LINE
    
    // make a SQL statement to get it edited
    char *edited = "UPDATE TODO SET TASK = ? WHERE ID = ?;";
    sqlite3_stmt *stmt; // represents a single SQL statement that has been compiled into binary form and is ready to be evaluated
    
    // make sure edited statement executes
    status = sqlite3_prepare_v2(db, edited, -1, &stmt, NULL);
    if (status != SQLITE_OK)
    {
        printf("Error for editing task in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    else
    {
        if (sqlite3_bind_text(stmt, 1, newtask, -1, SQLITE_STATIC) != SQLITE_OK)
        {
            printf("Error in binding text: %s", sqlite3_errmsg(db));
            return false;
        }
        
        if (sqlite3_bind_int(stmt, 2, id) != SQLITE_OK)
        {
            printf("Error in binding integer: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // evaluates sql statement
        status = sqlite3_step(stmt);
        if (status != SQLITE_DONE) // indicates operation ran successfully, sqlite_done is mainly used for step commands
        {
            printf("Error in step: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // deletes the prepared statement
        status = sqlite3_finalize(stmt);
        if (status != SQLITE_OK)
        {
            printf("Error in finalize: %s", sqlite3_errmsg(db));
            return false;
        }
    }
    
    // display table
    seetable(db);
     
    return true;
}

bool updateid(sqlite3 *db, int id)
{
    char *updateid = "UPDATE TODO SET id = id - 1 WHERE id > ?";
    sqlite3_stmt *stmt; // represents a single SQL statement that has been compiled into binary form and is ready to be evaluated
    
    // make sure edited statement executes
    int status = sqlite3_prepare_v2(db, updateid, -1, &stmt, NULL);
    if (status != SQLITE_OK)
    {
        printf("Error for updating id in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    else
    {
        if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK)
        {
            printf("Error in binding integer: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // evaluates sql statement
        status = sqlite3_step(stmt);
        if (status != SQLITE_DONE) // indicates operation ran successfully, sqlite_done is mainly used for step commands
        {
            printf("Error in step: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // deletes the prepared statement
        status = sqlite3_finalize(stmt);
        if (status != SQLITE_OK)
        {
            printf("Error in finalize: %s", sqlite3_errmsg(db));
            return false;
        }
    }
    
    return true;
}

bool deletetask(sqlite3 *db)
{
    // ask which task to delete
    int id;
    printf("Delete which # task? ID: ");
    
    // make sure id is an integer
    if (scanf("%d", &id) != 1) // if it returns the correct type, it will return the value 1
    {
        printf("Failed to proceed. Response was not a number. Please try again.\n");
        return false;
    }
    
    // get number of id's in the table
    char *clarifyidworks = "SELECT COUNT(id) FROM TODO;";
    dontprintstatement = 1;
    int status = sqlite3_exec(db, clarifyidworks, callback, 0, 0);
    dontprintstatement = 0;
    
    if (status != SQLITE_OK)
    {
        printf("Error for counting all id(s) in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    
    // delete the task
    char *deletetask = "DELETE FROM TODO WHERE ID = ?";
    sqlite3_stmt *stmt; // represents a single SQL statement that has been compiled into binary form and is ready to be evaluated
    
    // make sure edited statement executes
    status = sqlite3_prepare_v2(db, deletetask, -1, &stmt, NULL);
    if (status != SQLITE_OK)
    {
        printf("Error for deleting task in table: %s", sqlite3_errmsg(db));
        
        // must close when error
        sqlite3_close(db);
        
        return false;
    }
    else
    {
        if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK)
        {
            printf("Error in binding integer: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // evaluates sql statement
        status = sqlite3_step(stmt);
        if (status != SQLITE_DONE) // indicates operation ran successfully, sqlite_done is mainly used for step commands
        {
            printf("Error in step: %s", sqlite3_errmsg(db));
            return false;
        }
        
        // deletes the prepared statement
        status = sqlite3_finalize(stmt);
        if (status != SQLITE_OK)
        {
            printf("Error in finalize: %s", sqlite3_errmsg(db));
            return false;
        }
    }
    
    // update each id!
    updateid(db, id);
    
    // display table
    seetable(db);

    return true;
}

int main(int argc, const char *argv[]) {
    
    // opens up database connection
    sqlite3 *db = opendatabaseconnection();
    if (!db)
    {
        printf("Error in db");
        return 0; // returns false
    }
    
    // creates/finds a table
    createtable(db);
    
    // see all tables
    if (argc == 2 && strcmp(argv[1], "see") == 0)
        seetable(db);
    
    // add a task
    else if (argc == 2 && strcmp(argv[1], "add") == 0)
        addtask(db);
        
    else if (argc == 2 && strcmp(argv[1], "estatus") == 0)
        editstatus(db);
    
    else if (argc == 2 && strcmp(argv[1], "etask") == 0)
        edittask(db);
    
    // delete a task
    else if (argc == 2 && strcmp(argv[1], "del") == 0)
        deletetask(db);
    
    // quit
    else
    {
        printf("You did not use the correct commands. Please try again.\n");
        return 0;
    }
    
    sqlite3_close(db); // closes the database
    
    return 1;
}

// Goal 1: show database with this program without having extra commands: DONE 11/23

// Goal 2: be able to start the program in any directory no matter what: DONE 11/26
// export PATH=$PATH: [insert the pathway, use pwd to access current pathway directory]

// Goal 3: format the table
