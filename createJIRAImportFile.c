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
#define minMatchLength 6
#define userNamesColumns 3
#define jiraColumns 6
#define QJTColumns 8
#define epicsColumns 6
#define primarySize 5

/******************************************
Struct definitions
*******************************************/
struct jiraInputData
 {
     int entryNumber;
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
     int entryNumber;
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
     int entryNumber;
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
     int entryNumber;
     char *demandID;
     char *issueKey;
     char *summary;
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
    char *pad;
   };

struct userNamesData
 {
     int entryNumber;
     char *userName;
     char *jiraUserName;
     char *importOutputPath;
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
void createPerUserOutputFileForJIRAImport (void);
void createFinalStructureForOutput(void);

int lookupQJTEntry (struct ImportQJTFields *, struct jiraInputData *, int);
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
void validateFinalOutputFile(char *);
int matchPattern(char *, char *);
void validateSizeofQJTFields(struct ImportQJTFields*);
void checkMalloc(char *);
void checkSizeofStructure(void);

//variables used to hold main passed in arguments
char* jiraFilename;
char* QJTFilename;
char* userNamesFilename;
char epicFilename[NumOfEpicFiles][pathSize] ={
                         "File1                               ",
                         "File2                               ",
                         "File3                               "
                     };
char finalOuputFilename[pathSize];
char* programInputPath;
FILE *fpError;
char* myCWD;
char debugflag[10]="0";
//check for null pointer after malloc of memory
void checkMalloc(char * ptr)
{
if( ptr == NULL)
    {
    puts("\n\e[4;36;40mInternal Memory Allocation Error\e[0m \n");
    fclose(fpError);
    exit(1);
    }
}

/******************************************
Function:
Description:
*******************************************/
int main(int argc, char *argv[])

{
char outputErrors[pathSize];

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

// make sure we have enough arguments
if(argc < 9)
    {
    printf("\e[4;36;40m Check script to make sure all 8 arguments are passed in correctly \e[0m\n\n");
    fclose(fpError); 
    exit(1);
    }

//assign all passed in argument to filenames we are going to sue
jiraFilename = argv[1];
QJTFilename = argv[2];
userNamesFilename = argv[3];
strcpy(epicFilename[0], argv[4]);
strcpy(epicFilename[1], argv[5]);
strcpy(epicFilename[2], argv[6]);
programInputPath = argv[7];
strcpy(finalOuputFilename, programInputPath);
strcat(finalOuputFilename, "JIRAImportData.txt");

// get the current working directory so we can create and use absolute path names for files
myCWD =  argv[8];
if(argv[9] != NULL)
	strcpy(debugflag,argv[9]);

// open file to capture processing errors
strcpy(outputErrors, programInputPath);
strcat(outputErrors, "ProcessingErrors.txt");
//printf("\n FILE %s \n",outputErrors);
// open new file or delete it if it already exists
fpError = fopen(outputErrors,"w+");
if(fpError == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file %s \e[0m \n","ProcessingErrors.txt");
    printf("\n Please make sure permission to create file is available\n");
    }
fclose(fpError);
// re-open file so we can append errors during processing
fpError = fopen(outputErrors,"a");
if(fpError == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file %s \e[0m \n","ProcessingErrors.txt");
    printf("\n Please make sure permission to create file is available\n");
    }
printf("*************************************\n");
printf("Welcome to JIRA import file generator\n");
printf("*************************************\n");

//get information from username file and store it into an internal structure
fillFromUserNames(); 
//get information from JIRA Smartsheet file and store it into an internal structure
fillFromAllJiraBugs();
//we need to create a Primary key with suffixes to match up with QJT file 
createPrimaryModEntry();
//get information from QJT file and store it into an internal structure
fillFromQJT();
//get information from Epic files and store it into an internal structure
fillFromAllEpics();
//get data from internal structures and create a final structure that will be used to create final output that will be used to import into JIRA
// This process cross references the PrimaryMod numbers create to find related entries in the QJT file
// It then looks up the reporter by name and does a lookup top find the V42 compatible version to be used by JIRA
// Next it cross references the DemandID to find the JIRA epic link key to be used by JIRA when importing 
createFinalStructureForOutput();
// formats and outputs the final file that will be used to upload into JIRA with importer
if(strcmp(debugflag,"2") != 0)
    createOutputFileForJIRAImport();
//this creates a separate file for each users that can be uploaded
if(strcmp(debugflag,"2") == 0)
    createPerUserOutputFileForJIRAImport ();
// Looks thru individual final output files to make sure all the data required is in the final file
//
// if debug flag is passed in then output some extra info 
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
fclose(fpError);
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
int structLineCnt=0,structColumnEntry=0;
size_t recordSize=0;

//setup current to be first structure
userNamesCurrent = userNamesFirst;

FILE *fstream = fopen(userNamesFilename,"r");

if(fstream == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file '%s' \e[0m \n",userNamesFilename);
    printf("\n Please correct and try again\n");
    fclose(fpError);
    exit (1) ;
    }
printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Processing '%s' **********\n",userNamesFilename);
//walk thru the input file and pull out all the columns and populate structure members
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);

    userNamesCurrent->entryNumber = structLineCnt;

    while(record != NULL)
        {
        recordSize=strlen(record);
        if(structColumnEntry == userNameInputDataNAME_COLUMN)
            {
            userNamesCurrent->userName = (char *)malloc(recordSize+2);
            checkMalloc(userNamesCurrent->userName);
            memset(userNamesCurrent->userName,0,recordSize+2);
            strlcpy(userNamesCurrent->userName , record, recordSize+1);
            }
        else if(structColumnEntry == userNameInputDataJIRANAME_COLUMN)
            {
            userNamesCurrent->jiraUserName = (char *)malloc(recordSize+2);
            checkMalloc(userNamesCurrent->jiraUserName);
            memset(userNamesCurrent->jiraUserName,0,recordSize+2);
            strlcpy(userNamesCurrent->jiraUserName , record, recordSize+1);
           }
        else if(structColumnEntry == userNameInputDataPAD_COLUMN)
            {
            userNamesCurrent->pad = (char *)malloc(recordSize+2);
            checkMalloc(userNamesCurrent->pad);
            memset(userNamesCurrent->pad,0,recordSize+2);
            strlcpy(userNamesCurrent->pad , "N/A", recordSize+1);
            }
        record = strsep(&entry,token);
        structColumnEntry++;
      }
      // make sure each entry has the correct number of columns
    if(structColumnEntry < userNamesColumns)
        {
        printf("\e[4;36;40m Not enough columns in file %s line %d \e[0m\n\n",userNamesFilename,structLineCnt+1);
        fprintf(fpError,"Not enough columns in file %s line %d \n\n",userNamesFilename,structLineCnt+1);
        fclose(fpError);
        exit (1);
        }
    //malloc a new structure for next line
    userNamesNewone = createUserNamesStruct();
    userNamesCurrent->next = userNamesNewone;
    userNamesCurrent=userNamesNewone;
    structLineCnt++;
    structColumnEntry=0;
    }

printf("\e[1;32m %d \e[0m users found in '%s'\n",structLineCnt,userNamesFilename);
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
//setup current to be first structure
jiraInputCurrent = jiraInputFirst;

FILE *fstream = fopen(jiraFilename,"r");

if(fstream == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file '%s' \e[0m \n",jiraFilename);
    printf("\n Please correct and try again\n");
    fclose(fpError);
    exit (1) ;
    }
printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Processing '%s' **********\n",jiraFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);

    jiraInputCurrent->entryNumber = structLineCnt;
    while(record != NULL)
       {
       recordSize=strlen(record);

       if(structColumnEntry == jiraInputDataSHEETNAME_COLUMN)
            {
            jiraInputCurrent->sheetName = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->sheetName);
            memset(jiraInputCurrent->sheetName,0,recordSize+2);
            strlcpy(jiraInputCurrent->sheetName , record, recordSize+1);
            }
	   else if(structColumnEntry == jiraInputDataPRIMARY_COLUMN)
            {
            jiraInputCurrent->primary = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->primary);
            memset(jiraInputCurrent->primary,0,recordSize+2);
            strlcpy(jiraInputCurrent->primary , record, recordSize+1);
            }
 	   else if(structColumnEntry == jiraInputDataCHAPTER_COLUMN)
            {
            jiraInputCurrent->chapter = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->chapter);
            memset(jiraInputCurrent->chapter,0,recordSize+2);
            strlcpy(jiraInputCurrent->chapter , record, recordSize+1);
            }
	   else if(structColumnEntry == jiraInputDataSUMMARY_COLUMN)
            {
            jiraInputCurrent->summary = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->summary);
            memset(jiraInputCurrent->summary,0,recordSize+2);
            strlcpy(jiraInputCurrent->summary , record, recordSize+1); 
            }
	   else if(structColumnEntry == jiraInputDataADDITIONAL_COLUMN)
            {
            jiraInputCurrent->additional = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->additional);
            memset(jiraInputCurrent->additional,0,recordSize+2);
            strlcpy(jiraInputCurrent->additional, record, recordSize+1);
            }
        //this is reading in the 6th columns but not using the data.
        //initialize until createPrimaryMod function runs to update the data
        else if(structColumnEntry == jiraInputDataPRIMARYMOD_COLUMN)
            {
            jiraInputCurrent->primaryMod = (char *)malloc(recordSize+2);
            checkMalloc(jiraInputCurrent->primaryMod);
            memset(jiraInputCurrent->primaryMod,0,recordSize+2);
            strlcpy(jiraInputCurrent->primaryMod, "N/A", recordSize+1);
            }
        record = strsep(&entry,token);
        structColumnEntry++;
        } 
    // make sure each entry has the correct number of columns
    if(structColumnEntry < jiraColumns)
        {
        printf("\e[4;36;40m Not enough columns in file %s line %d \e[0m\n\n",jiraFilename,structLineCnt+1);
        fprintf(fpError, "Not enough columns in file %s line %d\n\n",jiraFilename,structLineCnt+1);
        fclose(fpError);
        exit (1);
        }
    //malloc a new structure for next line
    jiraInputNewone = createJiraStruct();
    jiraInputCurrent->next = jiraInputNewone;
    jiraInputCurrent=jiraInputNewone;
    structLineCnt++;
    structColumnEntry=0;
    }
printf("\e[1;32m %d \e[0m Jira entries found in '%s'\n",structLineCnt-1, jiraFilename);
printf("--------------------------------------------------------------------------------------------------------------------\n");
fclose(fstream);
}

/******************************************
Function:
Description:read from ALLJIraBugs file and create the structure to be used for final Import file
*******************************************/
void fillFromAllEpics(void)
{
char lineBuffer[lineBufferSize] ;
//using Tab token for all files
const char token[3] = "\t";
char *record,*entry;
int structLineCnt=0,structColumnEntry=0 ,fileNum=0;
size_t recordSize=0;
//setup current to be first structure
FILE *fstream;

epicInputCurrent = epicInputFirst;

for(fileNum = 0 ;fileNum < NumOfEpicFiles; fileNum++ )
    {
    fstream = fopen(epicFilename[fileNum],"r");

    if(fstream == NULL)
        {
        printf("\n \e[4;36;40m Failed to open file '%s'\e[0m \n",epicFilename[fileNum]);
        printf("\n Please correct and try again\n");
        fclose(fpError);
        exit (1) ;
        }
    printf("\n--------------------------------------------------------------------------------------------------------------------\n");
    printf("********** Processing '%s' **********\n",epicFilename[fileNum]);
     //process EpicsQUA first since the data is in different than PM and PRODN
  
     //process EpicsPM and EpicsPRODN 
 
        while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
            {
            record = strsep(&entry,token);
            epicInputCurrent->entryNumber = structLineCnt;
            // read line until you have the required number of columns then goto next row
            while((record != NULL) && (structColumnEntry < epicsColumns+1))
               {
               recordSize=strlen(record);
               // always copy in summary which might contain the demandID
                if(structColumnEntry == EpicInputDataISSUEKEY_COLUMN)
                    {
                    epicInputCurrent->issueKey = (char *)malloc(recordSize+2);
                    checkMalloc(epicInputCurrent->issueKey);
                    memset(epicInputCurrent->issueKey,0,recordSize+2);
                    strlcpy(epicInputCurrent->issueKey , record, recordSize+1);
                    }
                else if(structColumnEntry == EpicInputDataSUMMARY_COLUMN)
                    {
                    epicInputCurrent->summary = (char *)malloc(recordSize+2);
                    checkMalloc(epicInputCurrent->summary);
                    memset(epicInputCurrent->summary,0,recordSize+2);
                    strlcpy(epicInputCurrent->summary , record, recordSize+1);
                    }
               else if(structColumnEntry == EpicInputDataCUSTOM3_COLUMN)
                    {
                    epicInputCurrent->demandID = (char *)malloc(recordSize+2);
             		checkMalloc(epicInputCurrent->demandID);
                    memset(epicInputCurrent->demandID,0,recordSize+2);
                    strlcpy(epicInputCurrent->demandID , record, recordSize+1);
                    }
                record = strsep(&entry,token);
                structColumnEntry++;
                }
            // make sure each entry has the correct number of columns
            if(structColumnEntry < epicsColumns)
                {
                printf("\e[4;36;40m Not enough columns in file %s line %d \e[0m\n\n",epicFilename[fileNum],structLineCnt+1);
                fprintf(fpError, "Not enough columns in file %s line %d\n\n",epicFilename[fileNum],structLineCnt+1);
                fclose(fpError);
                exit (1);
                }
            //malloc a new structure for next line
            epicInputNewone = createEpicStruct();
            epicInputCurrent->next = epicInputNewone;
            epicInputCurrent=epicInputNewone;
            structLineCnt++;
            structColumnEntry=0;
            }
    printf("\e[1;32m %d \e[0m Epics found in '%s'\n",structLineCnt-1, epicFilename[fileNum]);
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

//setup current to be first structure
QJTInputCurrent = QJTInputFirst;

FILE *fstream = fopen(QJTFilename,"r");

if(fstream == NULL)
    {
    printf("\n\e[4;36;40m Failed to open file '%s' \e[0m \n",QJTFilename);
    printf("\n Please correct and try again\n");
    fclose(fpError);
    exit (1) ;
    }
printf("\n--------------------------------------------------------------------------------------------------------------------\n");
printf("********** Processing '%s' **********\n",QJTFilename);
while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
    {
    record = strsep(&entry,token);
    QJTInputCurrent->entryNumber = structLineCnt;
    while(record != NULL)
       {
       recordSize=strlen(record);
       if(structColumnEntry == QJTInputDataPRIMARY_COLUMN)
            {
            QJTInputCurrent->primary = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->primary);
            memset(QJTInputCurrent->primary,0,recordSize+2);
            strlcpy(QJTInputCurrent->primary , record, recordSize+1);
            }
        else if(structColumnEntry == QJTInputDataASSIGNEDPM_COLUMN)
            {
            QJTInputCurrent->assignedPM = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->assignedPM);
            memset(QJTInputCurrent->assignedPM,0,recordSize+2);
            strlcpy(QJTInputCurrent->assignedPM , record, recordSize+1);
            }
        else if(structColumnEntry == QJTInputDataTITLE_COLUMN)
            {
            QJTInputCurrent->title = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->title);
            memset(QJTInputCurrent->title,0,recordSize+2);
            strlcpy(QJTInputCurrent->title , record, recordSize+1);
            }
       else if(structColumnEntry == QJTInputDataEDITION_COLUMN)
            {
            QJTInputCurrent->edition = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->edition);
            memset(QJTInputCurrent->edition,0,recordSize+2);
            strlcpy(QJTInputCurrent->edition , record, recordSize+1); 
           }
       else if(structColumnEntry == QJTInputDataISBN_COLUMN)
            {
            QJTInputCurrent->ISBN = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->ISBN);
            memset(QJTInputCurrent->ISBN,0,recordSize+2);
            strlcpy(QJTInputCurrent->ISBN , record, recordSize+1); 
            }
       else if(structColumnEntry == QJTInputDataDEMANDID_COLUMN)
            {
            QJTInputCurrent->demandID = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->demandID);
            memset(QJTInputCurrent->demandID,0,recordSize+2);
            strlcpy(QJTInputCurrent->demandID , record, recordSize+1); 
            }
       else if(structColumnEntry == QJTInputDataAUTHOR_COLUMN)
            {
            QJTInputCurrent->author = (char *)malloc(recordSize+2);
            checkMalloc(QJTInputCurrent->author);
            memset(QJTInputCurrent->author,0,recordSize+2);
            strlcpy(QJTInputCurrent->author , record, recordSize+1); 
            }
        record = strsep(&entry,token);
        structColumnEntry++;
        }
    // make sure each entry has the correct number of columns
    if(structColumnEntry < QJTColumns)
        {
        printf("\e[4;36;40m Not enough columns in file %s line %d \e[0m\n\n",QJTFilename,structLineCnt+1);
        fprintf(fpError, "Not enough columns in file %s line %d\n\n",QJTFilename,structLineCnt+1);
        }   
     //malloc a new structure for next line
    QJTInputNewone = createQJTStruct();
    QJTInputCurrent->next = QJTInputNewone;
    QJTInputCurrent=QJTInputNewone; 
    structLineCnt++;
    structColumnEntry=0;
    }
printf("\e[1;32m %d \e[0m QJT entries found in '%s'\n", structLineCnt-1,QJTFilename);
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
int epicMatches=0, epicMatchesNotFound=0;
int primaryNotFound=0;
//temporary buffer structure for copying
struct ImportQJTFields *ImportQJTFieldsPtr;
ImportQJTFieldsPtr = (struct ImportQJTFields *)malloc(sizeof(struct ImportQJTFields));
if( ImportQJTFieldsPtr == NULL)
    {
    puts("\n\e[4;36;40m Internal Memory Allocation Error\e[0m \n");
    fclose(fpError);
    exit (1);
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

// look thru all JIRA entries
while(jiraInputCurrent->next != NULL)
    {
    // lookupQJTEntry will return a pointer to temporary structure with field from QJT required to fill in Import structure
    entryfound = lookupQJTEntry(ImportQJTFieldsPtr, jiraInputCurrent,  structLineCnt);
    if(entryfound==1)
        {
        structLineCnt++;
        ImportInputCurrent->entryNumber = structLineCnt;
        // look thru all the epic file info to see if we can match the DemandID
        while(epicInputCurrent->next != NULL)
            {
            epicFound=-1;
            // check to make sure we are working on a valid demandID
            stringSize = strlen(ImportQJTFieldsPtr->demandID);
            // if the length if demandID is not valid then don't walk the entire epic file looking for it
            if(stringSize >= minMatchLength)
                {
                // if DemandID is found this function will return the JIRA key for Epic Link
            	epicFound =  matchPattern(epicInputCurrent->demandID,ImportQJTFieldsPtr->demandID);
                //if we didn't find a match in the DenamdID column look for it embedded in the summary
                if(epicFound == -1)
                    {
                    epicFound =  matchPattern(epicInputCurrent->summary,ImportQJTFieldsPtr->demandID);
                    }
                }
            else
            	{
                printf("\e[0;35m DemandID   '%-12s' line %3d in '%s' not found in Jira Epic files\e[0m \n",ImportQJTFieldsPtr->demandID,jiraInputCurrent->entryNumber+1,jiraFilename);
                fprintf(fpError, "DemandID   '%-12s' line %3d in '%s' not found in Jira Epic files\n",ImportQJTFieldsPtr->demandID,jiraInputCurrent->entryNumber+1,jiraFilename);
				break;
				}
            // if matchEpic return a match then stop looking through the list
            if(epicFound != -1)
                {
                break;
                }
            epicInputCurrent = epicInputCurrent->next;
            }
        // if epic match was found this means  we need to create a valid final output structure member
        if(epicFound != -1)
            {   
            epicMatches++;
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
            }
        else
            {
            //keep track of  how many Epic did not get a match
            printf("\e[0;35m DemandID   '%-12s' line %3d in '%s' not found in Jira Epic files\e[0m \n",ImportQJTFieldsPtr->demandID,jiraInputCurrent->entryNumber+1,jiraFilename);
            fprintf(fpError, "DemandID   '%-12s' line %3d in '%s' not found in Jira Epic files\n",ImportQJTFieldsPtr->demandID,jiraInputCurrent->entryNumber+1,jiraFilename);
            epicMatchesNotFound++;
            }
        }
    else
        {  
        //if the demandID is not found then flag user. these will be included in the epics not found count 
        primaryNotFound++;      
        printf("\e[0;31m Primary    '%-12s' line %3d in '%s' not found in '%s' \e[0m \n",jiraInputCurrent->primary,jiraInputCurrent->entryNumber+1,jiraFilename,QJTFilename);
        fprintf(fpError, "Primary    '%-12s' line %3d in '%s' not found in '%s'\n",jiraInputCurrent->primary,jiraInputCurrent->entryNumber+1,jiraFilename,QJTFilename);
        structLineCnt++; 
        }
    epicInputCurrent = epicInputFirst;        
    jiraInputCurrent = jiraInputCurrent->next; 
    }

//plus 1 on line count because first row is the header on not processed
printf("\n\e[1;32m %3d \e[0m JIRA Primary entries searched for in '%s'\n", structLineCnt,QJTFilename);
// if primary was not matched out error to user indicating that this entry will not be included in the final output
if(primaryNotFound != 0)
    {
    printf("\e[1;37;41m %3d \e[0m JIRA Primary entries NOT found in    '%s' and will NOT be included in JIRA Import file\n", primaryNotFound,QJTFilename);
    fprintf(fpError, "%3d JIRA Primary entries NOT found in    '%s' and will NOT be included in JIRA Import file\n", primaryNotFound,QJTFilename);
    }
printf("\e[1;32m %3d \e[0m JIRA Primary entries found in        '%s'\n", structLineCnt-primaryNotFound,QJTFilename);
// if Epic was not matched out error to user indicating that this entry will not be included in the final output
if(epicMatchesNotFound !=0)
    {
    printf("\e[1;37;41m %3d \e[0m DemandIDs NOT found in JIRA Epic files and will NOT be included in JIRA Import file\n",epicMatchesNotFound); 
    fprintf(fpError, " %3d  DemandIDs NOT found in JIRA Epic files and will NOT be included in JIRA Import file\n",epicMatchesNotFound); 
    }
printf("\e[1;32m %3d \e[0m DemandIDs found in JIRA Epic files which resolve to JIRA Epic Links\n",epicMatches);   
printf("\e[1;32m %3d \e[0m Total JIRA bugs will be created in the JIRA Import file   '%s'\n", epicMatches,finalOuputFilename);
printf("--------------------------------------------------------------------------------------------------------------------\n");
}

/******************************************
Function:
Description: This function looks up the "PrimaryMod" number from the JIRA file and sees if there's a match in the QJT file.
			If it finds a match it returns a filled in structure with information required. If not, it returns an error.
*******************************************/
int lookupQJTEntry (struct ImportQJTFields *ImportQJTFieldsPtr, struct jiraInputData *jiraPtr, int lineNum)
{
int nameLookupFound=0, entryfound=0;
char name[lineBufferSize];
size_t stringSize=0;

struct QJTInputData *localQJTInputCurrent;
localQJTInputCurrent = QJTInputFirst;
struct userNamesData *localUserNamesCurrent;
localUserNamesCurrent = userNamesFirst;

//check for valid primaryMod key and leave
stringSize = strlen(jiraPtr->primaryMod);
//check to make sure PrimaryMod is valid
if(stringSize >= 5)
    {

    while(localQJTInputCurrent->next != NULL)
        {
        // Look for a match between jira Primary and QJT primary which usually have a suffix "-C, -P, -I"   
        if(strcmp(localQJTInputCurrent->primary,jiraPtr->primaryMod) == 0)
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
            // if a user match was not found then we will assigned default user to JIRA entry. The default user is the first entry in the unsername file
            if(nameLookupFound == 0)
                {
                strcpy(ImportQJTFieldsPtr->reporter, userNamesFirst->jiraUserName);
                printf("\e[0;36m AssingedPM '%-20s' line %3d of '%s' not found in '%s', Assigning to %s\e[0m \n",name, lineNum+2,jiraFilename, userNamesFilename,userNamesFirst->userName);
                fprintf(fpError,"AssingedPM '%-20s' line %3d of '%s' not found in '%s', Assigning to %s \n",name, lineNum+2,jiraFilename, userNamesFilename,userNamesFirst->userName);
               }
            strcpy(ImportQJTFieldsPtr->ISBN, localQJTInputCurrent->ISBN);
            strcpy(ImportQJTFieldsPtr->demandID, localQJTInputCurrent->demandID);
            strcpy(ImportQJTFieldsPtr->author, localQJTInputCurrent->author);
            strcpy(ImportQJTFieldsPtr->edition, localQJTInputCurrent->edition);
            //now that we found the entry stop looking through the QJT linked list
            break; 
            }
        // Look like the Primary in QJT might not have a suffix so look for a match between jira Primary and QJT primary without suffix  
        else if(strcmp(localQJTInputCurrent->primary, jiraPtr->primary) == 0)
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
            // if a user match was not found then we will assigned default user to JIRA entry. The default user is the first entry in the unsername file
            if(nameLookupFound == 0)
                {
                strcpy(ImportQJTFieldsPtr->reporter, userNamesFirst->jiraUserName);
                printf("\e[0;36m AssingedPM '%-20s' line %3d of '%s' not found in '%s', Assigning to %s\e[0m \n",name, lineNum+2,jiraFilename, userNamesFilename,userNamesFirst->userName);
                fprintf(fpError,"AssingedPM '%-20s' line %3d of '%s' not found in '%s', Assigning to %s \n",name, lineNum+2,jiraFilename, userNamesFilename,userNamesFirst->userName);
               }
            strcpy(ImportQJTFieldsPtr->ISBN, localQJTInputCurrent->ISBN);
            strcpy(ImportQJTFieldsPtr->demandID, localQJTInputCurrent->demandID);
            strcpy(ImportQJTFieldsPtr->author, localQJTInputCurrent->author);
            strcpy(ImportQJTFieldsPtr->edition, localQJTInputCurrent->edition);
            //now that we found the entry stop looking through the QJT linked list
            break; 
            }
        else
            {
            // didn't get a match which is fine . the else is here to delcare that its intentional that this will not do anything            
            }
        localQJTInputCurrent=localQJTInputCurrent->next;
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
	puts("\n\e[4;36;40m Internal Memory Allocation Error\e[0m \n");
    fclose(fpError);
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
    puts("\n\e[4;36;40m Internal Memory Allocation Error\e[0m \n");
    fclose(fpError);
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
    puts("\n\e[4;36;40mInternal Memory Allocation Error\e[0m \n");
    fclose(fpError);
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
    puts("\n\e[4;36;40mInternal Memory Allocation Error\e[0m \n");
    fclose(fpError);
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
    puts("\n\e[4;36;40mInternal Memory Allocation Error\e[0m \n");
    fclose(fpError);
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
strcat(localCwd,"/temp/NamesOutput.txt");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
if(fp == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file %s \e[0m \n","NamesOutput.txt");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
   fprintf(fp,"%d\t%s\t%s\t%s\t%s\t\n",ptr->entryNumber,ptr->userName, ptr->jiraUserName, ptr->importOutputPath, ptr->pad);
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
strcat(localCwd,"/temp/JIRAOutput.txt");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
if(fp == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file %s \e[0m \n","JIRAOutput.csv");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d\t%s\t%s\t%s\t%s\t%s\t%s\t\n",ptr->entryNumber,ptr->sheetName, ptr->primary, ptr->chapter, ptr->summary, ptr->additional,ptr->primaryMod);
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
strcat(localCwd,"/temp/EPICOutput.txt");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
if(fp == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file %s \e[0m \n","EPICOutput.csv");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d\t%s\t%s\t%s\t\n",ptr->entryNumber,ptr->issueKey, ptr->demandID, ptr->summary);
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
strcat(localCwd,"/temp/QJTOutput.txt");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
if(fp == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file %s \e[0m \n","QJTOutput.csv");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",ptr->entryNumber, ptr->primary, ptr->assignedPM, ptr->title, ptr->edition, ptr->ISBN, ptr->demandID, ptr->author);
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
strcat(localCwd,"/temp/ImportOutput.txt");
printf("\n FILE %s \n",localCwd);

fp = fopen(localCwd,"w+");
if(fp == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file %s \e[0m \n","ImportOutput.txt");
    printf("\n Please make sure permission to create file is available\n");
    }
//printf("print all lines \n");
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%d\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",ptr->entryNumber, ptr->summary,ptr->description,ptr->epicLink,ptr->reporter, ptr->ISBN, ptr->demandID, ptr->author,ptr->chapter,ptr->edition);
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
int matchFound = -1;
// skip the first entry in structure sine its the header from the file
jiraInputCurrent=jiraInputCurrent->next;
while(jiraInputCurrent->next != NULL)
    {
    stringSize = strlen(jiraInputCurrent->primary);
    jiraInputCurrent->primaryMod = (char *)malloc(stringSize+4);
    memset(jiraInputCurrent->primaryMod,0,stringSize+4);

// check to make sure we have a valid primary entry
    if(stringSize == primarySize)
        {
        // the information that determines which suffix to add is in the JIRA input file
        strcpy(jiraInputCurrent->primaryMod,jiraInputCurrent->primary);
        matchFound =  matchPattern(jiraInputCurrent->sheetName,"Copyediting");
        if (matchFound != -1)
            {
            strcat(jiraInputCurrent->primaryMod,"-C");
            //printf("==========Copyediting\n");
            }
        matchFound =  matchPattern(jiraInputCurrent->sheetName,"Indexing");
        if (matchFound != -1)
            {
            strcat(jiraInputCurrent->primaryMod,"-I");
            //printf("==========Indexing\n");
            }
        matchFound =  matchPattern(jiraInputCurrent->sheetName,"Proofreading");
        if (matchFound != -1)
            {   
            strcat(jiraInputCurrent->primaryMod,"-P");
            //printf("==========Proofreading\n");
            }
        }
    matchFound = -1;
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
int rowCnt=0;
FILE *fp;

fp = fopen(finalOuputFilename,"w+");
if(fp == NULL)
    {
    printf("\n \e[4;36;40m Failed to open file '%s' \e[0m \n",finalOuputFilename);
    printf("\n Please make sure permission to create file is available\n");
    }
printf("\n-----------------------------------------------------------------------------------------------------------------------\n");
printf("********** Creating '%s' **********\n",finalOuputFilename);
//print file header
fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n","Project","Issue Type","Summary","Description","Epic Link","Severity","Reporter","Found In Environment","Found In Phase","Labels","Priority","Activity","Custom1","Custom2","Custom3","Custom4","Custom5","Edition","Primary","Title");

//generate all the line for final output line
while(ptr->next != NULL)
    {
    //remove entry number before shipping
    fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n","CSC_Pearson_Quality","Bug",ptr->summary,ptr->description,ptr->epicLink,"Medium",ptr->reporter,"QA","Production","L3","Minor","QA","SPi", ptr->ISBN, ptr->demandID, ptr->author,ptr->chapter,ptr->edition,ptr->primary,ptr->title);
    rowCnt++;
    ptr = ptr->next;
    }
printf("\e[1;32m %d \e[0m JIRA entries written to '%s'\n", rowCnt,finalOuputFilename);
//printf("------------------------------------------------------------------------------------------------------------------------\n");
fclose(fp);
validateFinalOutputFile(finalOuputFilename);
}


/******************************************
Function:
Description: This function creates the output file that will be used by JIRA to import information.
*******************************************/
void createPerUserOutputFileForJIRAImport (void)
{
struct ImportInputData *ptr = ImportInputFirst;
int rowCnt=0, ret=0;
FILE *fp;
char tempName[pathSize];
size_t recordSize=0;

userNamesCurrent = userNamesFirst;
printf("\n-----------------------------------------------------------------------------------------------------------------------\n");

while (userNamesCurrent->next != NULL)
    {
    //strcpy(tempName,myCWD);
    strcpy(tempName,finalOuputFilename);
    strcat(tempName,userNamesCurrent->jiraUserName);
    strcat(tempName,".txt");
    //Walk all the users and create a separate file for each one
    recordSize=strlen(tempName);
    userNamesCurrent->importOutputPath = (char *)malloc(recordSize+2);
    checkMalloc(userNamesCurrent->importOutputPath);
    memset(userNamesCurrent->importOutputPath,0,recordSize+2);
    strcpy(userNamesCurrent->importOutputPath , tempName);

    fp = fopen(tempName,"w+");
    if(fp == NULL)
        {
        printf("\n \e[4;36;40m Failed to open file '%s' \e[0m \n",tempName);
        printf("\n Please make sure permission to create file is available\n");
        }
    //print file header
    fprintf(fp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n","Project","Issue Type","Summary","Description","Epic Link","Severity","Reporter","Found In Environment","Found In Phase","Labels","Priority","Activity","Custom1","Custom2","Custom3","Custom4","Custom5","Edition","Primary","Title");

    //generate all the line for final output line
    while(ptr->next != NULL)
        {
        if(strcmp(userNamesCurrent->jiraUserName, ptr->reporter) == 0)
            {
            //remove entry number before shipping
            fprintf(fp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\n","CSC_Pearson_Quality","Bug",ptr->summary,ptr->description,ptr->epicLink,"Medium",ptr->reporter,"QA","Production","L3","Minor","QA","SPi", ptr->ISBN, ptr->demandID, ptr->author,ptr->chapter,ptr->edition,ptr->primary,ptr->title);
            rowCnt++;
            }
        ptr = ptr->next;
        }
    fclose(fp);
    // if file is empty then delete file
    if(rowCnt == 0)
        ret = remove(tempName);
    else{
        printf("********** Creating '%s' **********\n",tempName);
        printf("\e[1;32m %d \e[0m JIRA entries written to '%s'\n", rowCnt,tempName);
        validateFinalOutputFile(tempName);
        }
    ptr = ImportInputFirst;
    userNamesCurrent = userNamesCurrent->next;
    rowCnt=0; 
    }
}

/******************************************
Function:
Description: Looks through final output file and looks for empty cells. reports back with row and column if a empty is found.
*******************************************/

void validateFinalOutputFile(char * outputPath)
{
char lineBuffer[lineBufferSize] ;
const char token[2] = ",";
int structLineCnt=0,structColumnEntry=0;
char *record,*entry;
int emptyCellCnt=0;

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
                     "Custom2-ISBN",
                     "Custom3-DemandID",
                     "Custom4-Author",
                     "Custom5-Chapter",
                     "Edition",
                     "Primary",
                     "Title"
                  };

            
    FILE *fstream = fopen(outputPath,"r");

    if(fstream == NULL)
        {
        printf("\n\e[4;36;40m Failed to open file '%s' \e[0m \n",outputPath);
        printf("\n Please correct and try again\n");
        fclose(fpError);
        exit (1) ;
        }
    //printf("\n--------------------------------------------------------------------------------------------------------------------\n");
    printf("********** Scanning Final Output File '%s' **********\n",outputPath);
    fprintf(fpError, "********** Scanning Final Output File '%s' **********\n",outputPath);

    while((entry=fgets(lineBuffer,sizeof(lineBuffer),fstream))!=NULL)
        {
        record = strsep(&entry,token);
        while(record != NULL)
            {
            if(strcmp (record, "")== 0)
                {
                emptyCellCnt++;
                printf("empty column value found at line \e[0;33m %3d \e[0m column \e[0;33m'%20s'\e[0m of file '%s'\n", structLineCnt+1,ImportOutputArray[structColumnEntry],finalOuputFilename);          
                fprintf(fpError, "  empty column value found at line %3d column '%s' of file '%s'\n", structLineCnt+1,ImportOutputArray[structColumnEntry],finalOuputFilename);          
                }
            record = strsep(&entry,token);
            if(structColumnEntry == ImportOutputMaxColumns-1)
                // done scanning this row for empty column value
                break;
            structColumnEntry++;
            }    
        structLineCnt++;
        structColumnEntry=0;
        }
    printf("\e[1;32m %d \e[0m empty entries in '%s'\n", emptyCellCnt,outputPath);
    printf("--------------------------------------------------------------------------------------------------------------------\n");
    printf("--------------------------------------------------------------------------------------------------------------------\n");
    fclose(fstream);
}


/******************************************
Function:
Description: This function is used to find and Epic match using the DemandID. It looks through tEpic info supplied for a match. Pattern length must be at least minMatchLength long
Returns: Function returns position character index if found a match . Ex. return 4 if the  pattern was found in the string 4 characters into it.
*******************************************/ 
int matchPattern(char* ptrText, char* ptrPattern) 
{
int textLoop, patternIndex, textIndex, text_length, pattern_length, position = -1;
text_length    = strlen(ptrText);
pattern_length = strlen(ptrPattern);

//make sure the pattern we are looking for is not longer than string we are searching through
// also make sure that the demandID is a valid length
if (pattern_length > text_length || pattern_length < minMatchLength)
	{
    return -1;
    }
 //this loop walks through the text, make sure DemandID is valid and the DemandID is not longer than string we are comparing against
for (textLoop = 0; textLoop <= text_length - pattern_length; textLoop++) 
    {
    position = textIndex = textLoop;
 //this loop compares text and pattern by character. if there's a match we have success
    for (patternIndex = 0; patternIndex < pattern_length; patternIndex++) 
        {
        if (ptrPattern[patternIndex] == ptrText[textIndex])  
            textIndex++;
         else 
            break;
        }
    if (patternIndex == pattern_length) 
        return position;
    }
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
    printf("\n\e[4;36;40m Failed to open file '%s' \e[0m \n",QJTFilename);
    printf("\n Please correct and try again\n");
    fclose(fpError);
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
            // done scanning this row for empty column value
            break;
        structColumnEntry++;
        }    
    structLineCnt++;
    structColumnEntry=0;
    }
ImportQJTFieldsData->reporter = (char *)malloc(QJTFieldSizeData.assignedPM+mallocPad);
checkMalloc(ImportQJTFieldsData->reporter);
memset(ImportQJTFieldsData->reporter,0,QJTFieldSizeData.assignedPM+mallocPad);

ImportQJTFieldsData->epicLink = (char *)malloc(QJTFieldSizeData.title+mallocPad);
checkMalloc(ImportQJTFieldsData->epicLink);
memset(ImportQJTFieldsData->epicLink,0,QJTFieldSizeData.title+mallocPad);

ImportQJTFieldsData->edition = (char *)malloc(QJTFieldSizeData.edition+mallocPad);
checkMalloc(ImportQJTFieldsData->edition);
memset(ImportQJTFieldsData->edition,0,QJTFieldSizeData.edition+mallocPad);

ImportQJTFieldsData->ISBN = (char *)malloc(QJTFieldSizeData.ISBN+mallocPad);
checkMalloc(ImportQJTFieldsData->ISBN);
memset(ImportQJTFieldsData->ISBN,0,QJTFieldSizeData.ISBN+mallocPad);

ImportQJTFieldsData->demandID = (char *)malloc(QJTFieldSizeData.demandID+mallocPad);
checkMalloc(ImportQJTFieldsData->demandID);
memset(ImportQJTFieldsData->demandID,0,QJTFieldSizeData.demandID+mallocPad);

ImportQJTFieldsData->author = (char *)malloc(QJTFieldSizeData.author+mallocPad);
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
            THis is debug only looking for abnormally big structure members
*******************************************/

void checkSizeofStructure()
{
ImportInputCurrent = ImportInputFirst;
int  stringSize=0,totalSize=0, biggestSize=0;
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
printf("%d Structure nodes processed totaling %d characters  SizeOF biggest node = %d\n", structLineCnt,totalSize,biggestSize);
}
