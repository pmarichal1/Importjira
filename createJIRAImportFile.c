/*
Copyright (c) 2018 Paul Marichal

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define jiraInputDataSHEETNAME_COLUMN 0
#define jiraInputDataPRIMARY_COLUMN 1
#define jiraInputDataCHAPTER_COLUMN 2
#define jiraInputDataSUMMARY_COLUMN 3
#define jiraInputDataADDITIONAL_COLUMN 4
#define jiraInputDataPRIMARYMOD_COLUMN 5
#define jiraInputDataPAD_COLUMN 6

#define QJTInputDataPRIMARY_COLUMN 0
#define QJTInputDataASSIGNEDPM_COLUMN 1
#define QJTInputDataTITLE_COLUMN 2
#define QJTInputDataEDITION_COLUMN 3
#define QJTInputDataISBN_COLUMN 4
#define QJTInputDataDEMANDID_COLUMN 5
#define QJTInputDataAUTHOR_COLUMN 6
#define QJTInputDataADDITIONAL_COLUMN 7
#define QJTInputDataPAD_COLUMN 8

#define userNameInputDataNAME_COLUMN 0
#define userNameInputDataJIRANAME_COLUMN 1
#define userNameInputDataPAD_COLUMN 2

#define EpicInputDataISSUEKEY_COLUMN 1
#define EpicInputDataSUMMARY_COLUMN 3
#define EpicInputDataCUSTOM3_COLUMN 4

#define ImportOutputMaxColumns 20
#define lineBufferSize 4000
#define pathSize 1000
#define NumOfEpicFiles 3
#define mallocPad 20
#define demandIDLength 6

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
     char *title;
     char *pad;
     struct ImportInputData *next;
} ;

struct epicInputData
 {
     int numOfEntries;
     char *demandID;
     char *issueKey;
     char *pad;
     struct epicInputData *next;
} ;

struct ImportQJTFields
    {
    char *epicLink;
    char *reporter;
    char *ISBN;
    char *demandID;
    char *author;
    char *edition;
   };

struct userNamesData
 {
     int numOfEntries;
     char *userName;
     char *jiraUserName;
     char *pad;
     struct userNamesData *next;
} ;

// ALLJira functions and global variables declarations
void fillFromAllJiraBugs(void);
struct jiraInputData *createJiraStruct(void);
struct jiraInputData *jiraInputFirst;
struct jiraInputData *jiraInputCurrent;
struct jiraInputData *jiraInputNewone;
void createPrimaryModEntry(void);
void saveJiraToFile(void);
// QJT  functions and global variables declarations
void fillFromQJT(void);
struct QJTInputData *createQJTStruct(void);
struct QJTInputData *QJTInputFirst;
struct QJTInputData *QJTInputCurrent;
struct QJTInputData *QJTInputNewone;
void saveQJTToFile(void);

// Import  functions and global variables declarations
void createOutputFileForJIRAImport (void);
void createFinalStructureForOutput(void);
int lookupQJTEntry (struct ImportQJTFields *, char*);
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

// ALLJira functions and global variables declarations
void fillFromAllEpics();
struct epicInputData *createEpicStruct(void);
struct epicInputData *epicInputFirst;
struct epicInputData *epicInputCurrent;
struct epicInputData *epicInputNewone;
void saveEpicToFile(void);

//General functions and globals
void printCurrentStructAddresses(struct jiraInputData*);
void createOutputFileForJIRAImport (void);
void validateFinalOutputFile(void);
int matchEpic(char *, char *);
void validateSizeofQJTFields(struct ImportQJTFields*);
void checkMalloc(char *);
void checkSizeofStructure(void);

//check fo null pointer after malloc of memory
void checkMalloc(char * ptr)
{
if( ptr == NULL)
    {
    puts("\nInternal Memory Allocation Error\n");
    exit(1);
    }
}

//variables used to hold main passed in arguments
char* jiraFilename;
char* QJTFilename;
char* userNamesFilename;
char epicFilename[NumOfEpicFiles][pathSize] ={
                         "File1                               ",
                         "File2                               ",
                         "File3                               "
                     };
char* finalOuputFilename;
char* myCWD;
char debugflag[10]="0";

/******************************************
Function:
Description:
*******************************************/
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

/* createJiraStruct first jira structure */
epicInputNewone = createEpicStruct();
epicInputFirst = epicInputNewone;
epicInputCurrent = epicInputNewone;
epicInputCurrent->next=NULL;

jiraFilename = argv[1];
QJTFilename = argv[2];
userNamesFilename = argv[3];
//get the 3 filenames passed from the script

strcpy(epicFilename[0], argv[4]);
strcpy(epicFilename[1], argv[5]);
strcpy(epicFilename[2], argv[6]);

finalOuputFilename = argv[7];
myCWD =  argv[8];
if(argv[9] != NULL)
	strcpy(debugflag,argv[9]);
if(argc <8)
    {
    printf("Check script to make sure all 8 arguments are passed in correctly \n"); 
    exit(1);
    }
printf("*************************************\n");
printf("Welcome to JIRA import file generator\n");
printf("*************************************\n");

fillFromUserNames(); 
fillFromAllJiraBugs();
createPrimaryModEntry();
fillFromQJT();
fillFromAllEpics();

createFinalStructureForOutput();
createOutputFileForJIRAImport();
validateFinalOutputFile();
 
if(strcmp(debugflag,"1") == 0)
  	{
  	printf("CREATING debug files\n");
    printf("EPIC files being used = %s  %s  %s     \n",epicFilename[0],epicFilename[1],epicFilename[2]); 
  	saveUserNamesToFile();
  	saveJiraToFile();
  	saveImportToFile();
  	saveEpicToFile();
    saveQJTToFile();
    checkSizeofStructure();
  	}
printf("Current dir %s\n\n", myCWD);
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
size_t recordSize=0;
size_t entrySize=0;
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
printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Processing '%s' **********\n",userNamesFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);

    userNamesCurrent->numOfEntries = structLineCnt;
    columnsFound=0;

    while(record != NULL)
        {
        recordSize=strlen(record);
        if(structColumnEntry == userNameInputDataNAME_COLUMN)
            {
            userNamesCurrent->userName = (char *)malloc(recordSize+2);
            checkMalloc(userNamesCurrent->userName);
            memset(userNamesCurrent->userName,0,recordSize+2);
            entrySize = strlcpy(userNamesCurrent->userName , record, recordSize+1);
            columnsFound++;
            }
        else if(structColumnEntry == userNameInputDataJIRANAME_COLUMN)
            {
            userNamesCurrent->jiraUserName = (char *)malloc(recordSize+2);
            checkMalloc(userNamesCurrent->jiraUserName);
            memset(userNamesCurrent->jiraUserName,0,recordSize+2);
            entrySize = strlcpy(userNamesCurrent->jiraUserName , record, recordSize+1);
            columnsFound++;
           }
        else if(structColumnEntry == userNameInputDataPAD_COLUMN)
            {
            userNamesCurrent->pad = (char *)malloc(recordSize+2);
            checkMalloc(userNamesCurrent->pad);
            memset(userNamesCurrent->pad,0,recordSize+2);
            entrySize = strlcpy(userNamesCurrent->pad , "N/A", recordSize+1);
            columnsFound++;
            }
        record = strsep(&entry,token);
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
    printf("NOT Enough Columns in '%s'\n",userNamesFilename);
    exit(1);
    }

printf("%d users found in '%s'\n",structLineCnt,userNamesFilename);
printf("--------------------------------------------------------------------------------------------------------------------\n");
fclose(fstream);
}


/******************************************
Function:
Description:read from ALLJIraBugs file and create the structure to be used for final IMport file
*******************************************/

void fillFromAllJiraBugs(void)
{
char lineBuffer[lineBufferSize] ;
//using Tab token for all files
const char token[3] = "\t";
char *record,*entry;
int structLineCnt=0,structColumnEntry=0;
size_t recordSize=0;
size_t entrySize=0;
//setup current to be first structure
jiraInputCurrent = jiraInputFirst;

FILE *fstream = fopen(jiraFilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",jiraFilename);
    printf("\n Please correct and try again\n");
        exit (1) ;
    }
printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Processing '%s' **********\n",jiraFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);

    jiraInputCurrent->numOfEntries = structLineCnt;
    while(record != NULL)
       {
       recordSize=strlen(record);

       if(structColumnEntry == jiraInputDataSHEETNAME_COLUMN)
            {
            jiraInputCurrent->sheetName = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->sheetName);
            memset(jiraInputCurrent->sheetName,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->sheetName , record, recordSize+1);
            }
	   else if(structColumnEntry == jiraInputDataPRIMARY_COLUMN)
            {
            jiraInputCurrent->primary = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->primary);
            memset(jiraInputCurrent->primary,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->primary , record, recordSize+1);
            }
 	   else if(structColumnEntry == jiraInputDataCHAPTER_COLUMN)
            {
            jiraInputCurrent->chapter = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->chapter);
            memset(jiraInputCurrent->chapter,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->chapter , record, recordSize+1);
            }
	   else if(structColumnEntry == jiraInputDataSUMMARY_COLUMN)
            {
            jiraInputCurrent->summary = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->summary);
            memset(jiraInputCurrent->summary,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->summary , record, recordSize+1); 
            }
	   else if(structColumnEntry == jiraInputDataADDITIONAL_COLUMN)
            {
            jiraInputCurrent->additional = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->additional);
            memset(jiraInputCurrent->additional,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->additional, record, recordSize+1);
            }
        else if(structColumnEntry == jiraInputDataPRIMARYMOD_COLUMN)
            {
            jiraInputCurrent->primaryMod = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->primaryMod);
            memset(jiraInputCurrent->primaryMod,0,recordSize+2);
            entrySize = strlcpy(jiraInputCurrent->primaryMod, "N/A", recordSize+1);
            }
        record = strsep(&entry,token);
        structColumnEntry++;
        } 
    //malloc a new structure for next line
    jiraInputNewone = createJiraStruct();
    jiraInputCurrent->next = jiraInputNewone;
    jiraInputCurrent=jiraInputNewone;
    structLineCnt++;
    structColumnEntry=0;
    }
printf("%d Jira entries found in '%s'\n",structLineCnt-1, jiraFilename);
printf("--------------------------------------------------------------------------------------------------------------------\n");
fclose(fstream);
}

/******************************************
Function:
Description:read from ALLJIraBugs file and create the structure to be used for final IMport file
*******************************************/
void fillFromAllEpics(void)
{
char lineBuffer[lineBufferSize] ;
//using Tab token for all files
const char token[3] = "\t";
char *record,*entry;
int structLineCnt=0,structColumnEntry=0 ,fileNum=0;
size_t recordSize=0;
size_t entrySize=0; 
//setup current to be first structure
FILE *fstream;

epicInputCurrent = epicInputFirst;

for(fileNum = 0 ;fileNum < NumOfEpicFiles; fileNum++ )
    {
    fstream = fopen(epicFilename[fileNum],"r");

    if(fstream == NULL)
        {
        printf("\n Failed to open file '%s' \n",epicFilename[fileNum]);
        printf("\n Please correct and try again\n");
            exit (1) ;
        }
    printf("\n--------------------------------------------------------------------------------------------------------------------\n");
    printf("********** Processing '%s' **********\n",epicFilename[fileNum]);
     //process EpicsQUA first since the data is in different than PM and PRODN
  
     //process EpicsPM and EpicsPRODN 
 
        while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
            {
            record = strsep(&entry,token);
            epicInputCurrent->numOfEntries = structLineCnt;
            while(record != NULL)
               {
               recordSize=strlen(record);
               // always copy in summary which might contain the demandID
                if(structColumnEntry == EpicInputDataSUMMARY_COLUMN)
                    {
                    epicInputCurrent->demandID = (char *)malloc(recordSize+2);
                    checkMalloc(epicInputCurrent->demandID);
                    memset(epicInputCurrent->demandID,0,recordSize+2);
                    entrySize = strlcpy(epicInputCurrent->demandID , record, recordSize+1);
                    }
               else if(structColumnEntry == EpicInputDataCUSTOM3_COLUMN)
                    {
                    // if the Custom3 field contains a valid demandID then free up old memory and get new pointer
                    if(recordSize >= demandIDLength)
                        {
                        //free previously malloc memory for this pointer
                        free(epicInputCurrent->demandID);
                        epicInputCurrent->demandID = (char *)malloc(recordSize+2);
             		    checkMalloc(epicInputCurrent->demandID);
                        memset(epicInputCurrent->demandID,0,recordSize+2);
                        entrySize = strlcpy(epicInputCurrent->demandID , record, recordSize+1);
                        }
                    }
               else if(structColumnEntry == EpicInputDataISSUEKEY_COLUMN)
                    {
                    epicInputCurrent->issueKey = (char *)malloc(recordSize+2);
              		checkMalloc(epicInputCurrent->issueKey);
                    memset(epicInputCurrent->issueKey,0,recordSize+2);
                    entrySize = strlcpy(epicInputCurrent->issueKey , record, recordSize+1);
                    }
                record = strsep(&entry,token);
                structColumnEntry++;
                } 
            //malloc a new structure for next line
            epicInputNewone = createEpicStruct();
            epicInputCurrent->next = epicInputNewone;
            epicInputCurrent=epicInputNewone;
            structLineCnt++;
            structColumnEntry=0;
            }
    printf("%d Epics found in '%s'\n",structLineCnt-1, epicFilename[fileNum]);
    fclose(fstream);
    }
printf("--------------------------------------------------------------------------------------------------------------------\n");
}

/* read from QJT file*/
/******************************************
Function:
Description:read from QJT file file and create the structure to be used for final Import file
            This file contains multiple fields like ISBN and Reported that will be used in final output
*******************************************/
void fillFromQJT(void)
{
char lineBuffer[lineBufferSize] ;
const char token[3] = "\t";
char *record,*entry;
int structLineCnt=0,structColumnEntry=0;
size_t recordSize=0;
size_t entrySize=0;

//setup current to be first structure
QJTInputCurrent = QJTInputFirst;

FILE *fstream = fopen(QJTFilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",QJTFilename);
    printf("\n Please correct and try again\n");
    exit (1) ;
    }
printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Processing '%s' **********\n",QJTFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);
    QJTInputCurrent->numOfEntries = structLineCnt;
    while(record != NULL)
       {
       recordSize=strlen(record);
       if(structColumnEntry == QJTInputDataPRIMARY_COLUMN)
            {
            QJTInputCurrent->primary = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->primary);
            memset(QJTInputCurrent->primary,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->primary , record, recordSize+1);
            }
        else if(structColumnEntry == QJTInputDataASSIGNEDPM_COLUMN)
            {
            QJTInputCurrent->assignedPM = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->assignedPM);
            memset(QJTInputCurrent->assignedPM,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->assignedPM , record, recordSize+1);
            }
        else if(structColumnEntry == QJTInputDataTITLE_COLUMN)
            {
            QJTInputCurrent->title = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->title);
            memset(QJTInputCurrent->title,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->title , record, recordSize+1);
            }
       else if(structColumnEntry == QJTInputDataEDITION_COLUMN)
            {
            QJTInputCurrent->edition = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->edition);
            memset(QJTInputCurrent->edition,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->edition , record, recordSize+1); 
           }
       else if(structColumnEntry == QJTInputDataISBN_COLUMN)
            {
            QJTInputCurrent->ISBN = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->ISBN);
            memset(QJTInputCurrent->ISBN,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->ISBN , record, recordSize+1); 
            }
       else if(structColumnEntry == QJTInputDataDEMANDID_COLUMN)
            {
            QJTInputCurrent->demandID = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->demandID);
            memset(QJTInputCurrent->demandID,0,recordSize+2);
            entrySize = strlcpy(QJTInputCurrent->demandID , record, recordSize+1); 
            }
       else if(structColumnEntry == QJTInputDataAUTHOR_COLUMN)
            {
            QJTInputCurrent->author = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->author);
            memset(QJTInputCurrent->author,0,recordSize+2);
            //copy 2 less characters to eliminate the CR at end of line
            entrySize = strlcpy(QJTInputCurrent->author , record, recordSize+1); 
            }
        record = strsep(&entry,token);
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
printf("--------------------------------------------------------------------------------------------------------------------\n");
fclose(fstream);
}

/* create final output structure*/
/******************************************
Function:
Description:uses the JIRA structure as the beginning point. It does lookups to find a match of the "primary" field 
            int the JIRA file finds a match in the QJT structures. It fills in the Import structures with required
            fields to complete the import output.
*******************************************/
void createFinalStructureForOutput()
{
int structLineCnt=0;
int entryfound=0;
int epicFound=0;
size_t stringSize=0;
int epicMatches=0;
int primaryNotFound=0;

//temporary buffer structure for copying

struct ImportQJTFields *ImportQJTFieldsPtr;
ImportQJTFieldsPtr = (struct ImportQJTFields *)malloc(sizeof(struct ImportQJTFields));
if( ImportQJTFieldsPtr == NULL)
    {
    puts("\nInternal Memory Allocation Error\n");
    exit(1);
    }
memset(ImportQJTFieldsPtr,0,sizeof(struct ImportQJTFields));
validateSizeofQJTFields(ImportQJTFieldsPtr);

//initialize pointers
jiraInputCurrent = jiraInputFirst;
QJTInputCurrent = QJTInputFirst;
epicInputCurrent = epicInputFirst;

//start at second JIRA & QJT entry to get by text on first line
jiraInputCurrent = jiraInputCurrent->next;
QJTInputCurrent = QJTInputCurrent->next;
epicInputCurrent = epicInputCurrent->next;

ImportInputCurrent = ImportInputFirst;

printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Cross Referencing file '%s' and '%s' **********\n\n",QJTFilename,jiraFilename);


while(jiraInputCurrent->next != NULL)
    {
    // lookupQJTEntry will return a pointer to temporary structure with field from QJT required to fill in Import structure
    entryfound = lookupQJTEntry(ImportQJTFieldsPtr, jiraInputCurrent->primaryMod);
    if(entryfound==1)
        {
        ImportInputCurrent->numOfEntries = structLineCnt;
        while(epicInputCurrent->next != NULL)
            {
            epicFound=-1;
            // check to make sure we are working on a valid demandID
            stringSize = strlen(ImportQJTFieldsPtr->demandID);
            if(stringSize >= demandIDLength)
            	epicFound =  matchEpic(epicInputCurrent->demandID,ImportQJTFieldsPtr->demandID);
            else
            	{
            	//printf("+++++++++++++++++++++++   DemandID = %s,  PrimaryMod= %s \n",ImportQJTFieldsPtr->demandID,jiraInputCurrent->primaryMod);
				break;
				}
            if(epicFound != -1)
                {
                epicMatches++;
                break;
                }
            epicInputCurrent = epicInputCurrent->next;
            }
        if(epicFound != -1)
            {
            stringSize = strlen(epicInputCurrent->issueKey);
            ImportInputCurrent->epicLink = (char *)malloc(stringSize+2);
            checkMalloc(ImportInputCurrent->epicLink);
            memset(ImportInputCurrent->epicLink,0,stringSize+2);
            strcpy(ImportInputCurrent->epicLink, epicInputCurrent->issueKey);

            stringSize = strlen(ImportQJTFieldsPtr->epicLink);
            ImportInputCurrent->title = (char *)malloc(stringSize+2);
            checkMalloc(ImportInputCurrent->title);
            memset(ImportInputCurrent->title,0,stringSize+2);
            strcpy(ImportInputCurrent->title, ImportQJTFieldsPtr->epicLink);
            }
        else{
        	printf("DemandID '%6s' line %3d in '%s' not found in Jira Epic files\n",ImportQJTFieldsPtr->demandID,jiraInputCurrent->numOfEntries+1,jiraFilename);
            stringSize = strlen(ImportQJTFieldsPtr->epicLink);
            ImportInputCurrent->title = (char *)malloc(stringSize+2);
            checkMalloc(ImportInputCurrent->title);
            memset(ImportInputCurrent->title,0,stringSize+2);
            strcpy(ImportInputCurrent->title, ImportQJTFieldsPtr->epicLink);

            stringSize = strlen(ImportQJTFieldsPtr->epicLink);
            ImportInputCurrent->epicLink = (char *)malloc(stringSize+2);
            checkMalloc(ImportInputCurrent->epicLink);
            memset(ImportInputCurrent->epicLink,0,stringSize+2);
            strcpy(ImportInputCurrent->epicLink, ImportQJTFieldsPtr->epicLink);
            }
        stringSize = strlen(jiraInputCurrent->summary);
        ImportInputCurrent->summary = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->summary);
        memset(ImportInputCurrent->summary,0,stringSize+2);
        strcpy(ImportInputCurrent->summary, jiraInputCurrent->summary);

        stringSize = strlen(jiraInputCurrent->additional);
        ImportInputCurrent->description = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->description);
        memset(ImportInputCurrent->description,0,stringSize+2);
        strcpy(ImportInputCurrent->description, jiraInputCurrent->additional);

        stringSize = strlen(ImportQJTFieldsPtr->reporter);
        ImportInputCurrent->reporter = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->reporter);
        memset(ImportInputCurrent->reporter,0,stringSize+2);
        strcpy(ImportInputCurrent->reporter, ImportQJTFieldsPtr->reporter);

        stringSize = strlen(ImportQJTFieldsPtr->ISBN);
        ImportInputCurrent->ISBN = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->ISBN);
        memset(ImportInputCurrent->ISBN,0,stringSize+2);
        strcpy(ImportInputCurrent->ISBN, ImportQJTFieldsPtr->ISBN);

        stringSize = strlen(ImportQJTFieldsPtr->demandID);
        ImportInputCurrent->demandID = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->demandID);
        memset(ImportInputCurrent->demandID,0,stringSize+2);
        strcpy(ImportInputCurrent->demandID, ImportQJTFieldsPtr->demandID);

        stringSize = strlen(ImportQJTFieldsPtr->author);
        ImportInputCurrent->author = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->author);
        memset(ImportInputCurrent->author,0,stringSize+2);
        strcpy(ImportInputCurrent->author, ImportQJTFieldsPtr->author);

        stringSize = strlen(jiraInputCurrent->chapter);
        ImportInputCurrent->chapter = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->chapter);
        memset(ImportInputCurrent->chapter,0,stringSize+2);
        strcpy(ImportInputCurrent->chapter, jiraInputCurrent->chapter);

        stringSize = strlen(ImportQJTFieldsPtr->edition);
        ImportInputCurrent->edition = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->edition);
        memset(ImportInputCurrent->edition,0,stringSize+2);
        strcpy(ImportInputCurrent->edition, ImportQJTFieldsPtr->edition);

        stringSize = strlen(jiraInputCurrent->primary);
        ImportInputCurrent->primary = (char *)malloc(stringSize+2);
        checkMalloc(ImportInputCurrent->primary);
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
        primaryNotFound++;      
        printf("Primary  '%5s'  line %3d in '%s' not found in '%s' \n",jiraInputCurrent->primary,jiraInputCurrent->numOfEntries+1,jiraFilename,QJTFilename); 
        }
    epicInputCurrent = epicInputFirst;        
    jiraInputCurrent = jiraInputCurrent->next; 
    }

//plus 1 on line count because first row is the header on not processed
printf("\n%3d JIRA entries found in '%s'\n", structLineCnt,QJTFilename);
printf("%3d JIRA entries NOT found in '%s'\n", primaryNotFound,QJTFilename);
printf("%3d Epic matches %3d Epics NOT found\n",epicMatches,structLineCnt-epicMatches);
printf("--------------------------------------------------------------------------------------------------------------------\n");
}

/******************************************
Function:
Description: This function looks up the "Primary" number from the JIRA file and sees if there's a match in the QJT file.
			If it finds a match it returns a filled in structure with information required. If not, it returns an error.
*******************************************/
int lookupQJTEntry (struct ImportQJTFields *ImportQJTFieldsPtr, char* jiraPrimaryMod)
{
int x=0, nameLookupFound=0, entryfound=0;
char name[lineBufferSize];
size_t stringSize=0;

struct QJTInputData *localQJTInputCurrent;
localQJTInputCurrent = QJTInputFirst;
struct userNamesData *localUserNamesCurrent;
localUserNamesCurrent = userNamesFirst;

//check for valid primaryMod key and leave
stringSize = strlen(jiraPrimaryMod);
//check to make sure PrimaryMod is valid
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
                //set this to 1 to get us out of loop
                if(strcmp(name, localUserNamesCurrent->userName) == 0 )
                    { 
                    nameLookupFound=1;              
                    strcpy(ImportQJTFieldsPtr->reporter, localUserNamesCurrent->jiraUserName);
                    // we found the name entry so break out and look up next record.
                    break;
                    }
                localUserNamesCurrent = localUserNamesCurrent->next;
                }
            if(nameLookupFound == 0)
                {
                strcpy(ImportQJTFieldsPtr->reporter, userNamesFirst->jiraUserName);
                //printf("AssingedPM '%s' line %3d in '%s' not found, Assigning to %s\n",name, x+1,QJTFilename,ImportQJTFieldsPtr->reporter);
                printf("AssingedPM '%s' not found in %s, Assigning to %s\n",name, userNamesFilename,ImportQJTFieldsPtr->reporter);
               }
            strcpy(ImportQJTFieldsPtr->ISBN, localQJTInputCurrent->ISBN);
            strcpy(ImportQJTFieldsPtr->demandID, localQJTInputCurrent->demandID);
            strcpy(ImportQJTFieldsPtr->author, localQJTInputCurrent->author);
            strcpy(ImportQJTFieldsPtr->edition, localQJTInputCurrent->edition); 
            }
        x++;
        localQJTInputCurrent=localQJTInputCurrent->next;
        //now that we found the entry stop looking thru the QJT linked list
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

struct epicInputData *createEpicStruct(void)
{
struct epicInputData *ptr;
ptr = (struct epicInputData *)malloc(sizeof(struct epicInputData));
memset(ptr,0,sizeof(struct epicInputData));

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
char localCwd[pathSize];

strcpy(localCwd,myCWD);
strcat(localCwd,"/NamesOutput.csv");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
if(fp == NULL)
    {
    printf("\n Failed to open file %s \n","NamesOutput.txt");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
   fprintf(fp,"%d,%s,%s,%s,\n",ptr->numOfEntries,ptr->userName, ptr->jiraUserName,ptr->pad);
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
char localCwd[pathSize];

strcpy(localCwd,myCWD);
strcat(localCwd,"/JIRAOutput.csv");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
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
void saveEpicToFile(void)
{
struct epicInputData *ptr = epicInputFirst;
FILE *fp;
char localCwd[pathSize];

strcpy(localCwd,myCWD);
strcat(localCwd,"/EPICOutput.csv");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
if(fp == NULL)
    {
    printf("\n Failed to open file %s \n","EPICOutput.csv");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d,%s,%s,\n",ptr->numOfEntries,ptr->demandID, ptr->issueKey);
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
char localCwd[pathSize];

strcpy(localCwd,myCWD);
strcat(localCwd,"/QJTOutput.csv");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
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
char localCwd[pathSize];

strcpy(localCwd,myCWD);
strcat(localCwd,"/ImportOutput.csv");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
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
Description: This function creates the output file that will be used by JIRA to import information.
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
printf("\n-----------------------------------------------------------------------------------------------------------------------\n");
printf("********** Creating '%s' **********\n",finalOuputFilename);

fprintf(fp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n","Project","Issue Type","Summary","Description","Epic Link","Severity","Reporter","Found In Environment","Found In Phase","Labels","Priority","Activity","Custom1","Custom2","Custom3","Custom4","Custom5","Edition","Primary","Title");

//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n","CSC_Pearson_Quality","Bug",ptr->summary,ptr->description,ptr->epicLink,"Medium",ptr->reporter,"QA","Production","L3","Minor","QA","SPi", ptr->ISBN, ptr->demandID, ptr->author,ptr->chapter,ptr->edition,ptr->primary,ptr->title);

//printf("NUMBER of ENTRIES for QJT file=%d\n",i);
    i++;
    ptr = ptr->next;
    }
printf("%d JIRA entries written to '%s'\n", i,finalOuputFilename);
printf("------------------------------------------------------------------------------------------------------------------------\n");
fclose(fp);
}

/******************************************
Function:
Description: Looks thru final output file and looks for blank cells. reports back with row and column if a blank is found.
*******************************************/

void validateFinalOutputFile(void)
{
char lineBuffer[lineBufferSize] ;
const char token[2] = ",";
int structLineCnt=0,structColumnEntry=0;
size_t  recordSize=0;
char *record,*entry;

const char *ImportOutputArray []= 
                    {"Project",
                     "Issue Type",
                     "Summary",
                     "Description",
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
                     "Primary",
                     "Title"
                  };
               
FILE *fstream = fopen(finalOuputFilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",finalOuputFilename);
    printf("\n Please correct and try again\n");
    exit (1) ;
    }
printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Scanning Final Output File '%s' **********\n",finalOuputFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);
    while(record != NULL)
        {
        recordSize=strlen(record);
        if(strcmp (record, "")== 0)
            { 
            printf("Blank column value found at line %3d column '%12s' of file '%s'\n", structLineCnt+1,ImportOutputArray[structColumnEntry],finalOuputFilename);          
            }
        record = strsep(&entry,token);
        if(structColumnEntry == ImportOutputMaxColumns-1)
            // done scanning this row for blank column value
            break;
        structColumnEntry++;
 
        }    
    structLineCnt++;
    structColumnEntry=0;
    }
printf("%d lines processed in '%s'\n", structLineCnt-1,finalOuputFilename);
printf("--------------------------------------------------------------------------------------------------------------------\n");
fclose(fstream);
}

/******************************************
Function:
Description: This function is used to find and Epic match using the DemandID. It looks thru the 3 files supplied for a match.
*******************************************/ 
int matchEpic(char* ptrSummary, char* ptrDemandID) 
{
int c, d, e, text_length, pattern_length, position = -1;

//char text[400], pattern[100];
char *text, *pattern;

text_length    = strlen(ptrSummary);
pattern_length = strlen(ptrDemandID);

text = (char *)malloc(text_length+2);
checkMalloc(text);
pattern = (char *)malloc(pattern_length+2);
checkMalloc(pattern);

strcpy(text,ptrSummary);
strcpy(pattern,ptrDemandID);
if(pattern_length != demandIDLength)
printf("==================  text  '%d' pattern '%d' Primary=%s \n",text_length,pattern_length,ptrDemandID);
//printf("________________MATCHING  '%s' and '%s' \n",ptrDemandID,ptrSummary);
if (pattern_length > text_length || pattern_length < demandIDLength)
	{
   	free (text);
    free (pattern);
    return -1;
    }
 //this loop walks thru the text, make sure DemandID is valid and the DemandID is not longer than string we are comparing against
for (c = 0; c <= text_length - pattern_length; c++) 
    {
    position = e = c;
 //this loop compares text and pattern by character. if there's a match we have success
    for (d = 0; d < pattern_length; d++) 
        {
        if (pattern[d] == text[e])  
            {
            e++;
            }
        else 
            {
            break;
            }
        }
    if (d == pattern_length) 
        {
        free (text);
        free (pattern);
        return position;
        }
    }
free (text);
free (pattern);
return -1;
}

/******************************************
Function:
Description: this function calculate the biggest string for each field in the QJT file . it then mallocs the size required to 
handle the biggest size element for each structure member.
Returns pointer to temp buffer to be used for processing

*******************************************/

void validateSizeofQJTFields(struct ImportQJTFields* ImportQJTFieldsData)
{
char lineBuffer[lineBufferSize] ;
const char token[3] = "\t";
int structLineCnt=0,structColumnEntry=0;
int  recordSize=0;
char *record,*entry;

struct QJTFieldSize
 {
     int primary;
     int assignedPM;
     int title;
     int edition;
     int ISBN;
     int demandID;
     int author;
     int pad;
	} ;
  
struct QJTFieldSize QJTFieldSizeData ={0,0,0,0,0,0,0,0};

FILE *fstream = fopen(QJTFilename,"r");

if(fstream == NULL)
    {
    printf("\n Failed to open file '%s' \n",QJTFilename);
    printf("\n Please correct and try again\n");
    exit (1) ;
    }
if(strcmp(debugflag,"1") == 0)
	{
	printf("\n--------------------------------------------------------------------------------------------------------------------\n");
	printf("********** Scanning QJT File for sizes '%s' **********\n",QJTFilename);
	}
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);
    while(record != NULL)
        {
        recordSize=strlen(record);
        if(structColumnEntry==0)
        	if (recordSize > QJTFieldSizeData.primary)
        		QJTFieldSizeData.primary = recordSize;
        if(structColumnEntry==1)
        	if (recordSize > QJTFieldSizeData.assignedPM)
        		QJTFieldSizeData.assignedPM = recordSize;
        if(structColumnEntry==2)
        	if (recordSize > QJTFieldSizeData.title)
        		QJTFieldSizeData.title = recordSize;
        if(structColumnEntry==3)
        	if (recordSize > QJTFieldSizeData.edition)
        		QJTFieldSizeData.edition = recordSize;
        if(structColumnEntry==4)
        	if (recordSize > QJTFieldSizeData.ISBN)
        		QJTFieldSizeData.ISBN = recordSize;
        if(structColumnEntry==5)
        	if (recordSize > QJTFieldSizeData.demandID)
        		QJTFieldSizeData.demandID = recordSize;
        if(structColumnEntry==6)
        	if (recordSize > QJTFieldSizeData.author)
        		QJTFieldSizeData.author = recordSize;
        record = strsep(&entry,token);
        if(structColumnEntry == 6)
            // done scanning this row for blank column value
            break;
        structColumnEntry++;
        }    
    structLineCnt++;
    structColumnEntry=0;
    }
ImportQJTFieldsData->reporter = malloc(QJTFieldSizeData.assignedPM+mallocPad);
checkMalloc(ImportQJTFieldsData->reporter);
memset(ImportQJTFieldsData->reporter,0,QJTFieldSizeData.assignedPM+mallocPad);

ImportQJTFieldsData->epicLink = malloc(QJTFieldSizeData.title+mallocPad);
checkMalloc(ImportQJTFieldsData->epicLink);
memset(ImportQJTFieldsData->epicLink,0,QJTFieldSizeData.title+mallocPad);

ImportQJTFieldsData->edition = malloc(QJTFieldSizeData.edition+mallocPad);
checkMalloc(ImportQJTFieldsData->edition);
memset(ImportQJTFieldsData->edition,0,QJTFieldSizeData.edition+mallocPad);

ImportQJTFieldsData->ISBN = malloc(QJTFieldSizeData.ISBN+mallocPad);
checkMalloc(ImportQJTFieldsData->ISBN);
memset(ImportQJTFieldsData->ISBN,0,QJTFieldSizeData.ISBN+mallocPad);

ImportQJTFieldsData->demandID = malloc(QJTFieldSizeData.demandID+mallocPad);
checkMalloc(ImportQJTFieldsData->demandID);
memset(ImportQJTFieldsData->demandID,0,QJTFieldSizeData.demandID+mallocPad);

ImportQJTFieldsData->author = malloc(QJTFieldSizeData.author+mallocPad);
checkMalloc(ImportQJTFieldsData->author);
memset(ImportQJTFieldsData->author,0,QJTFieldSizeData.author+mallocPad);
if(strcmp(debugflag,"1") == 0)
	{
	printf("%d lines processed in '%s'\n", structLineCnt-1,QJTFilename);
	printf("QJT primary max = %d \n",QJTFieldSizeData.primary);
	printf("QJT assignedPM max = %d  \n",QJTFieldSizeData.assignedPM);
	printf("QJT title max = %d  \n",QJTFieldSizeData.title);
	printf("QJT edition max = %d   \n",QJTFieldSizeData.edition);
	printf("QJT ISBN max = %d\n",QJTFieldSizeData.ISBN);
	printf("QJT demandID max = %d\n",QJTFieldSizeData.demandID);
	printf("QJT author max = %d\n",QJTFieldSizeData.author);
	}
printf("--------------------------------------------------------------------------------------------------------------------\n");
fclose(fstream);
}

/******************************************
Function: debug only
Description: see how big the final structure is
*******************************************/

void checkSizeofStructure()
{
ImportInputCurrent = ImportInputFirst;
size_t stringSize=0,totalSize=0, biggestSize=0;
int structLineCnt=0;

while(ImportInputCurrent->next != NULL)
    {
    stringSize = strlen(ImportInputCurrent->epicLink);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;

    stringSize = strlen(ImportInputCurrent->title);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;

    stringSize = strlen(ImportInputCurrent->summary);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize; 

    stringSize = strlen(ImportInputCurrent->description);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;

    stringSize = strlen(ImportInputCurrent->reporter);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;

    stringSize = strlen(ImportInputCurrent->ISBN);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;  

    stringSize = strlen(ImportInputCurrent->demandID);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;  

    stringSize = strlen(ImportInputCurrent->author);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;  

    stringSize = strlen(ImportInputCurrent->chapter);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize; 

    stringSize = strlen(ImportInputCurrent->edition);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize;

    stringSize = strlen(ImportInputCurrent->primary);
    totalSize = totalSize+stringSize;
    if(stringSize >biggestSize)
        biggestSize=stringSize; 

    ImportInputCurrent = ImportInputCurrent->next;
    structLineCnt++;
    }
printf("\n************* Processing checkSizeofStructure ***************\n");    
printf("%d Structure nodes processed totaling %lu characters  SizeOF biggest node = %lu\n", (structLineCnt),totalSize,biggestSize);

}
