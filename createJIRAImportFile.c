//created by Paul Marichal 12/12/2018

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define jiraInputDataSHEETNAME_COLUMN 0
#define jiraInputDataPRIMARY_COLUMN 1
#define jiraInputDataCHAPTER_COLUMN 2
#define jiraInputDataSUMMARY_COLUMN 3
#define jiraInputDataADDITIONAL_COLUMN 4
#define jiraInputDataPAD_COLUMN 5


#define QJTInputDataPRIMARY_COLUMN 0
#define QJTInputDataASSIGNEDPM_COLUMN 1
#define QJTInputDataTITLE_COLUMN 2
#define QJTInputDataEDITION_COLUMN 3
#define QJTInputDataISBN_COLUMN 4
#define QJTInputDataDEMANDID_COLUMN 5
#define QJTInputDataAUTHOR_COLUMN 6
#define QJTInputDataADDITIONAL_COLUMN 7
#define QJTInputDataPAD_COLUMN 8

#define ImportOutputMaxColumns 19
#define lineBufferSize 2048

const char *ImportOutputArray []= 
                    {"Project",
                     "Issue Type",
                     "Summary",
                     "Desciption",
                     "Epic Link",
                     "Severity",
                     "Reporter",
                     "Found in Environment",
                     "Found in Phase",
                     "Labels",
                     "Priority",
                     "Activity",
                     "Custom1",
                     "Custom2",
                     "Custom3",
                     "Custom4",
                     "Custom5",
                     "Edition",
                     "Primary"
                  };
/******************************************
GLOBALS
*******************************************/
struct jiraInputData
 {
     int numOfEntries;
     char *sheetName;
     char *primary;
     char *chapter;
     char *summary;
     char *additional;
     char *primaryMod;
     char *pad;
     struct jiraInputData *next;
} ;

struct QJTInputData
 {
     int numOfEntries;
     char *primary;
     char *assignedPM;
     char *title;
     char *edition;
     char *ISBN;
     char *demandID;
     char *author;
     char *pad;
 //    char *additional;
     struct QJTInputData *next;
} ;

struct ImportInputData
 {
     int numOfEntries;
     char *summary;
     char *description;
     char *epicLink;
     char *reporter;
     char *ISBN;
     char *demandID;
     char *author;
     char *chapter;
     char *edition;
     char *primary;
     char *pad;
     struct ImportInputData *next;
} ;

struct ImportQJTFields
    {
    char epicLink[200];
    char reporter[100];
    char ISBN[50];
    char demandID[20];
    char author[75];
    char edition[20];
   };

struct userNamesData
 {
     int numOfEntries;
     char *userName;
     char *jiraUserName;
     char *pad;
     struct userNamesData *next;
} ;

// ALLJira functions and global variables delclarations
void fillFromAllJiraBugs(char*);
struct jiraInputData *createJiraStruct(void);
struct jiraInputData *jiraInputFirst;
struct jiraInputData *jiraInputCurrent;
struct jiraInputData *jiraInputNewone;
void createPrimaryModEntry(void);
void saveJiraToFile(void);
// QJT  functions and global variables declarations
void fillFromQJT(char*);
struct QJTInputData *createQJTStruct(void);
struct QJTInputData *QJTInputFirst;
struct QJTInputData *QJTInputCurrent;
struct QJTInputData *QJTInputNewone;
void saveQJTToFile(void);

// Import  functions and global variables declarations
void createOutputFileForJIRAImport (void);
void createFinalStructureForOutput(void);
int lookupQJTEntry (struct ImportQJTFields *, char*, int);
struct ImportInputData *createImportStruct(void);
struct ImportInputData *ImportInputFirst;
struct ImportInputData *ImportInputCurrent;
struct ImportInputData *ImportInputNewone;
void saveImportToFile(void);

// User Names  functions and global variables declarations
void fillFromUserNames (void);
struct userNamesData *createUserNamesStruct(void);
struct userNamesData *userNamesFirst;
struct userNamesData *userNamesCurrent;
struct userNamesData *userNamesNewone;
void saveUserNamesToFile(void);

//General functions and globals
void printCurrentStructAddresses(struct jiraInputData*);
void createOutputFileForJIRAImport (void);
void validateFinalOutputFile(void);

//variables used to hold main passed in arguments
char* jiraFilename;
char* QJTFilename;
char* debugflag;
char* finalOuputFilename="ImportFinalOutput.csv";
char* userNamesFilename="UserNames.txt";

/******************************************
Function:
Description:
*******************************************/
//int main(int argc, char *argv[],char *agrv[])
int main(int argc, char *argv[])

{
/* createJiraStruct first jira structure */
jiraInputNewone = createJiraStruct();
jiraInputFirst = jiraInputNewone;
jiraInputCurrent = jiraInputNewone;
jiraInputCurrent->next=NULL;

/* createQJTStruct first jira structure */
QJTInputNewone = createQJTStruct();
QJTInputFirst = QJTInputNewone;
QJTInputCurrent = QJTInputNewone;
QJTInputCurrent->next=NULL;

/* createImportInputStruct first jira structure */
ImportInputNewone = createImportStruct();
ImportInputFirst = ImportInputNewone;
ImportInputCurrent = ImportInputNewone;
ImportInputCurrent->next=NULL;

/* createUserNamesStruct first jira structure */
userNamesNewone = createUserNamesStruct();
userNamesFirst = userNamesNewone;
userNamesCurrent = userNamesNewone;
userNamesCurrent->next=NULL;

jiraFilename = argv[1];
QJTFilename = argv[2];

if(argc <3)
    {
    printf("Need Jira and QJT filenames\n"); 
    exit(1);
    }
printf("*************************************\n");
printf("Welcome to JIRA import file generator\n");
printf("*************************************\n");

fillFromUserNames(); 
fillFromAllJiraBugs(jiraFilename);
createPrimaryModEntry();
fillFromQJT(QJTFilename);

createFinalStructureForOutput();
createOutputFileForJIRAImport();
validateFinalOutputFile();

if(argv[3] != NULL)
    {   
    if(strcmp(argv[3],"1") == 0)
        {
        printf("CREATING debug files\n");
        saveUserNamesToFile();
        saveJiraToFile();
        saveQJTToFile();
        saveImportToFile();
        }
    } 
return 0 ;
}

/******************************************
Function: 
Description:create username lookup structure to be used in final process to convert username to Jira 
            encoded names
*******************************************/

void fillFromUserNames (void)
{
char lineBuffer[lineBufferSize] ;
const char token[3] = "\t";
char *record,*entry;
int structLineCnt=0,structColumnEntry=0,columnsFound=0;
size_t recordSize;
size_t entrySize;
//size_t stringSize=0;
//setup current to be first structure
userNamesCurrent = userNamesFirst;

FILE *fstream = fopen(userNamesFilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",userNamesFilename);
    printf("\n Please correct and try again\n");
    exit (1) ;
    }
printf("\n********** Processing '%s' **********\n",userNamesFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strtok(entry,token);
    userNamesCurrent->numOfEntries = structLineCnt;
    columnsFound=0;

    while(record != NULL)
        {
        recordSize=strlen(record);
        if(structColumnEntry == 0)
            {
            userNamesCurrent->userName = (char *)malloc(recordSize+2);
            memset(userNamesCurrent->userName,0,recordSize+2);
            entrySize = strlcpy(userNamesCurrent->userName , record, recordSize+1);
            columnsFound++;
            }
        else if(structColumnEntry == 1)
            {
            userNamesCurrent->jiraUserName = (char *)malloc(recordSize+2);
            memset(userNamesCurrent->jiraUserName,0,recordSize+2);
            entrySize = strlcpy(userNamesCurrent->jiraUserName , record, recordSize+1);
            columnsFound++;
           }
        else if(structColumnEntry == 2)
            {
            userNamesCurrent->pad = (char *)malloc(recordSize+2);
            memset(userNamesCurrent->pad,0,recordSize+2);
            entrySize = strlcpy(userNamesCurrent->pad , record, recordSize+1);
            columnsFound++;
            }
        record = strtok(NULL,token);
        structColumnEntry++;
      }
    //malloc a new structure for next line
    userNamesNewone = createUserNamesStruct();
    userNamesCurrent->next = userNamesNewone;
    userNamesCurrent=userNamesNewone;
    structLineCnt++;
    structColumnEntry=0;
    }
if(columnsFound < 3)
    {
    printf("NOT ENOUGH Columns in '%s'\n",userNamesFilename);
    exit(1);
    }

printf("%d users found in '%s'\n",structLineCnt,userNamesFilename);
fclose(fstream);
}


/******************************************
Function:
Description:read from ALLJIraBugs file and create the structure to be used for final IMport file
*******************************************/

void fillFromAllJiraBugs(char* jiraFilename)
{
char lineBuffer[lineBufferSize] ;
//using Tab token for all files
const char token[3] = "\t";
char *record,*entry;
int structLineCnt=0,structColumnEntry=0;
size_t recordSize;
size_t entrySize;
//setup current to be first structure
jiraInputCurrent = jiraInputFirst;

FILE *fstream = fopen(jiraFilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",jiraFilename);
    printf("\n Please correct and try again\n");
        exit (1) ;
    }
printf("\n********** Processing '%s' **********\n",jiraFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strtok(entry,token);
    jiraInputCurrent->numOfEntries = structLineCnt;
    while(record != NULL)
       {
       recordSize=strlen(record);

       if(structColumnEntry == jiraInputDataSHEETNAME_COLUMN)
            {
            jiraInputCurrent->sheetName = (char *)malloc(recordSize+2);
            memset(jiraInputCurrent->sheetName,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->sheetName , record, recordSize+1);
            }
	   else if(structColumnEntry == jiraInputDataPRIMARY_COLUMN)
            {
            jiraInputCurrent->primary = (char *)malloc(recordSize+2);
            memset(jiraInputCurrent->primary,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->primary , record, recordSize+1);
            }
 	   else if(structColumnEntry == jiraInputDataCHAPTER_COLUMN)
            {
            jiraInputCurrent->chapter = (char *)malloc(recordSize+2);
            memset(jiraInputCurrent->chapter,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->chapter , record, recordSize+1);
            }
	   else if(structColumnEntry == jiraInputDataSUMMARY_COLUMN)
            {
            jiraInputCurrent->summary = (char *)malloc(recordSize+2);
            memset(jiraInputCurrent->summary,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->summary , record, recordSize+1); 
            }
	   else if(structColumnEntry == jiraInputDataADDITIONAL_COLUMN)
            {
            jiraInputCurrent->additional = (char *)malloc(recordSize+2);
            memset(jiraInputCurrent->additional,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->additional, record, recordSize+1);
            }

        record = strtok(NULL,token);
        structColumnEntry++;
        } 
    //malloc a new structure for next line
    jiraInputNewone = createJiraStruct();
    jiraInputCurrent->next = jiraInputNewone;
    jiraInputCurrent=jiraInputNewone;
    structLineCnt++;
    structColumnEntry=0;
    }
printf("%d entries found in '%s'\n",structLineCnt-1, jiraFilename);
fclose(fstream);
}


/* read from QJT file*/
/******************************************
Function:
Description:read from QJT file file and create the structure to be used for final Import file
            This file contains multiple fields like ISBN and Reported that will be used in final output
*******************************************/
void fillFromQJT(char* QJTfilename)
{
char lineBuffer[lineBufferSize] ;
const char token[3] = "\t";
char *record,*entry;
int structLineCnt=0,structColumnEntry=0;
size_t recordSize;
size_t entrySize;

//setup current to be first structure
QJTInputCurrent = QJTInputFirst;

FILE *fstream = fopen(QJTfilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",QJTFilename);
    printf("\n Please correct and try again\n");
    exit (1) ;
    }
printf("\n********** Processing '%s' **********\n",QJTFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
   record = strtok(entry,token);
   QJTInputCurrent->numOfEntries = structLineCnt;

    while(record != NULL)
       {
       recordSize=strlen(record);


       if(structColumnEntry == QJTInputDataPRIMARY_COLUMN)
            {
            QJTInputCurrent->primary = (char *)malloc(recordSize+2);
            memset(QJTInputCurrent->primary,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->primary , record, recordSize+1);
            }
        else if(structColumnEntry == QJTInputDataASSIGNEDPM_COLUMN)
            {
            QJTInputCurrent->assignedPM = (char *)malloc(recordSize+2);
            memset(QJTInputCurrent->assignedPM,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->assignedPM , record, recordSize+1);
            }
        else if(structColumnEntry == QJTInputDataTITLE_COLUMN)
            {
            QJTInputCurrent->title = (char *)malloc(recordSize+2);
            memset(QJTInputCurrent->title,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->title , record, recordSize+1);
            }
       else if(structColumnEntry == QJTInputDataEDITION_COLUMN)
            {
            QJTInputCurrent->edition = (char *)malloc(recordSize+2);
            memset(QJTInputCurrent->edition,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->edition , record, recordSize+1); 
           }
       else if(structColumnEntry == QJTInputDataISBN_COLUMN)
            {
            QJTInputCurrent->ISBN = (char *)malloc(recordSize+2);
            memset(QJTInputCurrent->ISBN,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->ISBN , record, recordSize+1); 
            }
       else if(structColumnEntry == QJTInputDataDEMANDID_COLUMN)
            {
            QJTInputCurrent->demandID = (char *)malloc(recordSize+2);
            memset(QJTInputCurrent->demandID,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->demandID , record, recordSize+1); 
            }
       else if(structColumnEntry == QJTInputDataAUTHOR_COLUMN)
            {
            QJTInputCurrent->author = (char *)malloc(recordSize+2);
            memset(QJTInputCurrent->author,0,recordSize+2);
            //copy 2 less characters to elimanate the CR at end of line
            entrySize = strlcpy(QJTInputCurrent->author , record, recordSize+1); 
            }
        record = strtok(NULL,token);
        structColumnEntry++;
        }   
     //malloc a new structure for next line
    QJTInputNewone = createQJTStruct();
    QJTInputCurrent->next = QJTInputNewone;
    QJTInputCurrent=QJTInputNewone; 
    structLineCnt++;
    structColumnEntry=0;
    }
printf("%d QJT entries found in '%s'\n", structLineCnt-1,QJTFilename);

fclose(fstream);
}


/* create final output structure*/
/******************************************
Function:
Description:uses the JIRA structure as the beggining point. It does lookups to find a match of the "primary" field 
            int the JIRA file finds a match in the QJT structures. It fills in the Import structures with required
            fields to complate the import ouput.
*******************************************/
void createFinalStructureForOutput()
{
int structLineCnt=0;
int entryfound=0;
size_t stringSize;
//temporary buffer structure for copying
struct ImportQJTFields ImportQJTFieldsData;
struct ImportQJTFields *ImportQJTFieldsPtr;
ImportQJTFieldsPtr=&ImportQJTFieldsData;
//initialize pointers
jiraInputCurrent = jiraInputFirst;
QJTInputCurrent = QJTInputFirst;
//start at second JIRA & QJT entry to get by text on first line
jiraInputCurrent = jiraInputCurrent->next;
QJTInputCurrent = QJTInputCurrent->next;

ImportInputCurrent = ImportInputFirst;

printf("\n********** Cross Referencing file '%s' and '%s' **********\n",QJTFilename,jiraFilename);


while(jiraInputCurrent->next != NULL)
    {
    // lookupQJTEntry will return a pointer to temporary structure with field from QJT reuiqred to fill in Import structure
    entryfound = lookupQJTEntry(ImportQJTFieldsPtr, jiraInputCurrent->primaryMod,jiraInputCurrent->numOfEntries);
    if(entryfound==1)
        {
        stringSize = strlen(ImportQJTFieldsPtr->epicLink);
        ImportInputCurrent->epicLink = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->epicLink,0,stringSize+2);
        strcpy(ImportInputCurrent->epicLink, ImportQJTFieldsPtr->epicLink);

        stringSize = strlen(jiraInputCurrent->summary);
        ImportInputCurrent->summary = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->summary,0,stringSize+2);
        strcpy(ImportInputCurrent->summary, jiraInputCurrent->summary);

        stringSize = strlen(jiraInputCurrent->additional);
        ImportInputCurrent->description = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->description,0,stringSize+2);
        strcpy(ImportInputCurrent->description, jiraInputCurrent->additional);

        stringSize = strlen(ImportQJTFieldsPtr->reporter);
        ImportInputCurrent->reporter = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->reporter,0,stringSize+2);
        strcpy(ImportInputCurrent->reporter, ImportQJTFieldsPtr->reporter);

        stringSize = strlen(ImportQJTFieldsPtr->ISBN);
        ImportInputCurrent->ISBN = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->ISBN,0,stringSize+2);
        strcpy(ImportInputCurrent->ISBN, ImportQJTFieldsPtr->ISBN);

        stringSize = strlen(ImportQJTFieldsPtr->demandID);
        ImportInputCurrent->demandID = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->demandID,0,stringSize+2);
        strcpy(ImportInputCurrent->demandID, ImportQJTFieldsPtr->demandID);

        stringSize = strlen(ImportQJTFieldsPtr->author);
        ImportInputCurrent->author = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->author,0,stringSize+2);
        strcpy(ImportInputCurrent->author, ImportQJTFieldsPtr->author);

        stringSize = strlen(jiraInputCurrent->chapter);
        ImportInputCurrent->chapter = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->chapter,0,stringSize+2);
        strcpy(ImportInputCurrent->chapter, jiraInputCurrent->chapter);

        stringSize = strlen(ImportQJTFieldsPtr->edition);
        ImportInputCurrent->edition = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->edition,0,stringSize+2);
        strcpy(ImportInputCurrent->edition, ImportQJTFieldsPtr->edition);

        stringSize = strlen(jiraInputCurrent->primary);
        ImportInputCurrent->primary = (char *)malloc(stringSize+2);
        memset(ImportInputCurrent->primary,0,stringSize+2);
        strcpy(ImportInputCurrent->primary, jiraInputCurrent->primary);
        //malloc a new structure for next line
        ImportInputNewone = createImportStruct();
        ImportInputCurrent->next = ImportInputNewone;
        ImportInputCurrent=ImportInputNewone;
        structLineCnt++;
        }
    else
        {        
        printf("Primary entry   %5s   line number %d in '%s' file not found in '%s' \n",jiraInputCurrent->primary,jiraInputCurrent->numOfEntries+1,jiraFilename,QJTFilename); 
        }
    jiraInputCurrent = jiraInputCurrent->next; 
    }
//plus 1 on line count because first row is the header on not processed
printf("%d JIRA entries found in '%s'\n", structLineCnt,QJTFilename);
}
/******************************************
Function:
Description:
*******************************************/
int lookupQJTEntry (struct ImportQJTFields *ImportQJTFieldsPtr, char* jiraPrimaryMod, int rowNumber)
{
int i=0,x=0, nameLookupFound=0, entryfound=0;
char name[1000];
size_t stringSize;

struct QJTInputData *localQJTInputCurrent;
localQJTInputCurrent = QJTInputFirst;
struct userNamesData *localUserNamesCurrent;
struct userNamesData *localUserNamesFirst;

localUserNamesCurrent = userNamesFirst;
localUserNamesFirst=userNamesFirst;

//check for valid primaryMod key and leave
stringSize = strlen(jiraPrimaryMod);

if(stringSize >= 5)
    {

    while(localQJTInputCurrent->next != NULL)
        {
        if(strcmp(localQJTInputCurrent->primary,jiraPrimaryMod) == 0)
            {
            entryfound=1;
            strcpy(ImportQJTFieldsPtr->epicLink, localQJTInputCurrent->title);
            strcpy(name, localQJTInputCurrent->assignedPM);
            while(localUserNamesCurrent->next != NULL)
                {
                //set this to 1 tp get us out of loop
                if(strcmp(name, localUserNamesCurrent->userName) == 0 )
                    { 
                    nameLookupFound=1;              
                    strcpy(ImportQJTFieldsPtr->reporter, localUserNamesCurrent->jiraUserName);
                    // we found the name entry so break out and look up next record.
                    break;
                    }
                i++;
                localUserNamesCurrent = localUserNamesCurrent->next;
                }
            if(nameLookupFound == 0)
                {
                strcpy(ImportQJTFieldsPtr->reporter, userNamesFirst->jiraUserName);
                printf("\nUsername lookup failed for AssingedPM '%s' line %d of '%s' Primary %s\n",name, x+1,QJTFilename,jiraPrimaryMod);
                printf("Assigning to %s\n\n",ImportQJTFieldsPtr->reporter);
                }
            strcpy(ImportQJTFieldsPtr->ISBN, localQJTInputCurrent->ISBN);
            strcpy(ImportQJTFieldsPtr->demandID, localQJTInputCurrent->demandID);
            strcpy(ImportQJTFieldsPtr->author, localQJTInputCurrent->author);
            strcpy(ImportQJTFieldsPtr->edition, localQJTInputCurrent->edition); 
            }
        x++;
        localQJTInputCurrent=localQJTInputCurrent->next;
        //now that we found the entry stop lookking thru the QJT linked list
        if(entryfound ==1) 
                break;
        }
    }
return(entryfound);
}
/******************************************
Function:
Description: creates new structure and malloc memory for linked list
*******************************************/

/* Allocate storage for one new structure */
struct jiraInputData *createJiraStruct(void)
{
struct jiraInputData *ptr;

ptr = (struct jiraInputData *)malloc(sizeof(struct jiraInputData));
memset(ptr,0,sizeof(struct jiraInputData));

if( ptr == NULL)
	{
	puts("\nInternal Memory Allocation Error\n");
	exit(1);
	}
ptr->next = NULL;
return(ptr);
}
/******************************************
Function:
Description:creates new structure and malloc memory for linked list
*******************************************/

/* Allocate storage for one new structure */
struct QJTInputData *createQJTStruct(void)
{
struct QJTInputData *ptr;

ptr = (struct QJTInputData *)malloc(sizeof(struct QJTInputData));
memset(ptr,0,sizeof(struct QJTInputData));

if( ptr == NULL)
    {
    puts("\nInternal Memory Allocation Error\n");
    exit(1);
    }
 ptr->next = NULL;
return(ptr);
}
/******************************************
Function:
Description:creates new structure and malloc memory for linked list
*******************************************/
struct ImportInputData *createImportStruct(void)
{
struct ImportInputData *ptr;
ptr = (struct ImportInputData *)malloc(sizeof(struct ImportInputData));
memset(ptr,0,sizeof(struct ImportInputData));

if( ptr == NULL)
    {
    puts("\nInternal Memory Allocation Error\n");
    exit(1);
    }
ptr->next = NULL;
return(ptr);
}

/******************************************
Function:
Description:creates new structure and malloc memory for linked list
*******************************************/

struct userNamesData *createUserNamesStruct(void)
{
struct userNamesData *ptr;

ptr = (struct userNamesData *)malloc(sizeof(struct userNamesData));
memset(ptr,0,sizeof(struct userNamesData));

if( ptr == NULL)
    {
    puts("\nInternal Memory Allocation Error\n");
    exit(1);
    }
ptr->next = NULL;
return(ptr);
}
/******************************************
Function:
Description: create output file used for debugging
*******************************************/
void saveUserNamesToFile(void)
{
struct userNamesData *ptr = userNamesFirst;

FILE *fp;
int i=0;
fp = fopen("NamesOutput.txt","w+");
if(fp == NULL)
    {
    printf("\n Failed to open file %s \n","NamesOutput.txt");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
   fprintf(fp,"%d,%s,%s,%s",ptr->numOfEntries,ptr->userName, ptr->jiraUserName,ptr->pad);
   ptr = ptr->next;
    i++;
    }
//printf("print all lines %d \n",i);

fclose(fp);
}
/******************************************
Function:
Description:create output file used for debugging
*******************************************/
void saveJiraToFile(void)
{
struct jiraInputData *ptr = jiraInputFirst;

FILE *fp;

fp = fopen("JIRAOutput.csv","w+");
if(fp == NULL)
    {
    printf("\n Failed to open file %s \n","JIRAOutput.csv");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d,%s,%s,%s,%s,%s,%s,\n",ptr->numOfEntries,ptr->sheetName, ptr->primary, ptr->chapter, ptr->summary, ptr->additional,ptr->primaryMod);
    ptr = ptr->next;
    }
 //printf("print all lines %d \n",i);

fclose(fp);
}
/******************************************
Function:
Description:create output file used for debugging
*******************************************/
void saveQJTToFile(void)
{
struct QJTInputData *ptr = QJTInputFirst;

FILE *fp;
fp = fopen("QJTOutput.csv","w+");
if(fp == NULL)
    {
    printf("\n Failed to open file %s \n","QJTOutput.csv");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d,%s,%s,%s,%s,%s,%s,%s\n",ptr->numOfEntries, ptr->primary, ptr->assignedPM, ptr->title, ptr->edition, ptr->ISBN, ptr->demandID, ptr->author);
    ptr = ptr->next;
    }
fclose(fp);
}
/******************************************
Function:
Description:create output file used for debugging
*******************************************/
void saveImportToFile(void)
{
struct ImportInputData *ptr = ImportInputFirst;

FILE *fp;
fp = fopen("ImportOutput.csv","w+");
if(fp == NULL)
    {
    printf("\n Failed to open file %s \n","ImportOutput.csv");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",ptr->numOfEntries, ptr->summary,ptr->description,ptr->epicLink,ptr->reporter, ptr->ISBN, ptr->demandID, ptr->author,ptr->chapter,ptr->edition);
    ptr = ptr->next;
    }
fclose(fp);
}
/******************************************
Function:
Description: This function takes "primary" field from JIRA file and appends a "-C, -P, -I" basaed on Sheetname.
            This allows for a match in the QJT file which contains the suffixes.
*******************************************/
void createPrimaryModEntry(void)
{
jiraInputCurrent = jiraInputFirst;
size_t stringSize=0;
// skip the first entry in structure sine its the header from the file
jiraInputCurrent=jiraInputCurrent->next;
while(jiraInputCurrent->next != NULL)
    {
    stringSize = strlen(jiraInputCurrent->primary);
    jiraInputCurrent->primaryMod = (char *)malloc(stringSize+4);
    memset(jiraInputCurrent->primaryMod,0,stringSize+4);
// check to make sure we have a valid primary entry
    if(stringSize == 5)
        {
        strcpy(jiraInputCurrent->primaryMod,jiraInputCurrent->primary);
        if (strcmp(jiraInputCurrent->sheetName ,"JIRA Copyediting") == 0)
            strcat(jiraInputCurrent->primaryMod,"-C");
        else if (strcmp(jiraInputCurrent->sheetName,"JIRA Indexing") == 0)
            strcat(jiraInputCurrent->primaryMod,"-I");
        else if (strcmp(jiraInputCurrent->sheetName, "JIRA Proofreading") == 0)
            strcat(jiraInputCurrent->primaryMod,"-P");
        }
    jiraInputCurrent = jiraInputCurrent->next;
    } 
}
/******************************************
Function:
Description:
*******************************************/
void createOutputFileForJIRAImport (void)
{
struct ImportInputData *ptr = ImportInputFirst;
int i=0;
FILE *fp;
fp = fopen(finalOuputFilename,"w+");
if(fp == NULL)
    {
    printf("\n Failed to open file '%s' \n",finalOuputFilename);
    printf("\n Please make sure permission to create file is available\n");
    }
printf("\n********** Creating '%s' **********\n",finalOuputFilename);

fprintf(fp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n","Project","Issue Type","Summary","Description","Epic Link","Severity","Reporter","Found In Environment","Found In Phase","Labels","Priority","Activity","Custom1","Custom2","Custom3","Custom4","Custom5","Edition","Primary");

//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n","CSC_Pearson_Quality","Bug",ptr->summary,ptr->description,ptr->epicLink,"Medium",ptr->reporter,"QA","Production","L3","Minor","QA","SPi", ptr->ISBN, ptr->demandID, ptr->author,ptr->chapter,ptr->edition,ptr->primary);

//printf("NUMBER of ENTRIES for QJT file=%d\n",i);
    i++;
    ptr = ptr->next;
    }
printf("%d JIRA entries written to '%s'\n", i,QJTFilename);

fclose(fp);
}
/******************************************
Function:
Description: for debugging
*******************************************/

void printCurrentStructAddresses(struct jiraInputData  *structPtr)
{
printf("\n\nstructptr=%p \n",structPtr);
printf("numOfEntries=%p \n",&structPtr->numOfEntries);
printf("sheetName=%p \n",structPtr->sheetName);
printf("primary=%p \n",structPtr->primary);
printf("chapter=%p \n",structPtr->chapter);
printf("summary=%p \n",structPtr->summary);
printf("additional=%p \n",structPtr->additional);
printf("structnext=%p \n\n\n",structPtr->next);
}

/******************************************
Function:
Description: for debugging
*******************************************/

void validateFinalOutputFile(void)
{
char lineBuffer[lineBufferSize] ;
const char token[2] = ",";
int structLineCnt=0,structColumnEntry=0;
size_t  recordSize=0;
char *record,*entry;

FILE *fstream = fopen(finalOuputFilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",finalOuputFilename);
    printf("\n Please correct and try again\n");
    exit (1) ;
    }
printf("\n********** Scanning Final Output File '%s' **********\n",finalOuputFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
   record = strtok(entry,token);
    while(record != NULL)
        {
        recordSize=strlen(record);
        if(strcmp (record, " ")== 0)
            { 
            printf("Blank column value found at line %d column %s of file '%s'\n", structLineCnt+1,ImportOutputArray[structColumnEntry],finalOuputFilename);          
            }
        record = strtok(NULL,token);
        if(structColumnEntry == ImportOutputMaxColumns-1)
            // done scanning this row for blank column value
            break;
        structColumnEntry++;
        }    
    structLineCnt++;
    structColumnEntry=0;
    }
printf("%d lines processed in '%s'\n", structLineCnt-1,finalOuputFilename);

fclose(fstream);
}