#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

struct Command
{
    char action[20];
    int index;
    int type; // 0:action, 1:condition, 2:and, 3:if
    int realType;
};

struct Sentence
{
    int start;
    int end;
    int commandIndexes[20]; // Indexes of commands in commandList
    int commandSize;
};

char *keywords[] = {"sell", "buy", "go", "to", "from", "and", "at", "has", "if", "less",
                    "more", "than", "exit", "where", "total", "who", "NOBODY", "NOTHING", "NOWHERE"};

char *sentWords[] = {"sell", "buy", "go", "to", "from", "and", "at", "has", "if", "less",
                     "more", "than", "exit", "where", "total", "who"};

char *actionWords[] = {"sell", "buy", "go", "to", "from"};

char *conditionWords[] = {"at", "has", "if", "less", "more", "than"};

char *noWords[] = {"NOBODY", "NOTHING", "NOWHERE"};

char *questionWords[] = {"where", "total", "who"};

char *subjectArray[100];
char *subjectLocation[100];
char *itemArray[100];
int items[100][100];
int sizeItems = 0;
int sizeSubjects = 0;

// VALIDATING FUNCTIONS
int isNumber(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int isKeyWord(char *word)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (strcmp(word, keywords[i]) == 0)
            return 0;
    }
    return 1;
}

int isValid(char *theWord)
{
    if (isKeyWord(theWord) == 0)
        return 0;

    // Iterate over theWord until the null terminator is found
    for (int i = 0; theWord[i] != '\0'; i++)
    {
        if (isdigit(theWord[i]))
        {
            return 0; // Contains a digit, return 0
        }
        if (!isalpha(theWord[i]) && theWord[i] != '_')
        {
            return 0; // Contains a non-alphabetic character other than '_'
        }
    }
    return 1;
}

int findIsItQuestion(char **tempArray, int size)
{
    if (size == 3 && strcmp(tempArray[size - 2], "where") == 0)
    {
        return 1;
    }
    else if (size >= 4 && strcmp(tempArray[size - 3], "total") == 0)
    {
        return 2;
    }
    else if (size == 3 && strcmp(tempArray[size - 2], "total") == 0)
    {
        return 3;
    }
    else if (size == 4 && strcmp(tempArray[0], "who") == 0)
    {
        return 4;
    }
    else if (size >= 1 && strcmp(tempArray[size - 1], "?") == 0)
    {
        return -1;
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ACTION FUNCTIONS:

int getType(char *word)
{
    if (strcmp(word, "if") == 0)
    {
        return 3;
    }
    else if (strcmp(word, "and") == 0)
    {
        return 2;
    }
    for (int i = 0; i < 5; i++)
    {
        if (strcmp(word, actionWords[i]) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int findRealEnd(char **tempArray, struct Command commandList[], int textInd)
{
    char *hasThanStr = tempArray[textInd - 3];
    if (strcmp(hasThanStr, "has") == 0 || strcmp(hasThanStr, "than") == 0)
    {
        int flag = 0;
        while (flag == 0)
        {
            char *befItem = tempArray[textInd - 1];
            char *befDigit = tempArray[textInd - 2];
            char *aftItem = tempArray[textInd + 2];
            char *aftDigit = tempArray[textInd + 1];

            int resultBefItem = isValid(befItem);
            int resultBefDigit = isNumber(befDigit);
            int resultAftItem = isValid(aftItem);
            int resultAftDigit = isNumber(aftDigit);

            if (resultBefItem == 1 && resultBefDigit == 1 && resultAftItem == 1 && resultAftDigit == 1)
            {
                textInd = textInd + 3;
            }
            else
            {
                return textInd;
            }
        }
    }
    return textInd;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ACTION FUNCTIONS:

int returnSubject(char *name)
{ // returns -1 if its not found returns subject place
    for (int i = 0; i < sizeSubjects; i++)
    {
        if (strcmp(name, subjectArray[i]) == 0)
            return i;
    }
    return -1;
}

int initializeSubject(char *name)
{
    subjectArray[sizeSubjects] = strdup(name);
    sizeSubjects += 1;
    return returnSubject(name);
}

int returnItem(char *name)
{ // returns -1 if its not found returns item place
    for (int i = 0; i < sizeItems; i++)
    {
        if (strcmp(name, itemArray[i]) == 0)
            return i;
    }
    return -1;
}

int initializeItem(char *name)
{
    itemArray[sizeItems] = strdup(name);
    sizeItems += 1;
    return returnItem(name);
}

int goToAction(char **textArray, int *subjects, int nums, int location)
{
    for (int i = 0; i < nums; i++)
    {
        if (isValid(textArray[subjects[i]]) == 0)
            return 0;
    }
    if (isValid(textArray[location]) == 0)
        return 0;
    char *locationName = textArray[location];
    for (int i = 0; i < nums; i++)
    {
        char *subjectName = textArray[subjects[i]];
        int subjectPlace = returnSubject(subjectName);
        if (subjectPlace == -1)
        {
            initializeSubject(subjectName);
        }
        subjectPlace = returnSubject(subjectName);
        subjectLocation[subjectPlace] = strdup(locationName);
    }
    return 1;
}

int onlySellAction(char **textArray, int *subjects, int nums, int *itemz, int numi)
{ // returns 0 if invalid//Subject(s) sell Item(s)
    for (int i = 0; i < nums; i++)
    { // checks if subject names are valid
        if (isValid(textArray[subjects[i]]) == 0)
            return 0;
        if (returnSubject(textArray[subjects[i]]) == -1)
        { // returns if subject doesn't exist
            return 1;
        }
    }
    for (int i = 0; i < numi; i++)
    { // checks if item names are valid
        if (i % 2 == 1 && isValid(textArray[itemz[i]]) == 0)
            return 0;
        if (i % 2 == 1 && returnItem(textArray[itemz[i]]) == -1)
            return 1;
    }
    for (int i = 0; i < nums; i++)
    { // cheks if you can decrease the item by the amount
        char *name = textArray[subjects[i]];
        int subjectPlace = returnSubject(name);
        for (int j = 0; j < numi; j++)
        {
            int decrease = atoi(textArray[itemz[2 * j]]);
            char *itemDecrease = textArray[itemz[2 * j + 1]];
            int itemPlace = returnItem(itemDecrease);
            int normalCount = items[subjectPlace][itemPlace];
            if (normalCount < decrease)
                return 1;
        }
    }

    for (int i = 0; i < nums; i++)
    { // decreases the item
        char *name = textArray[subjects[i]];
        int subjectPlace = returnSubject(name);
        for (int j = 0; j < numi; j++)
        {
            int decrease = atoi(textArray[itemz[2 * j]]);
            char *itemDecrease = textArray[itemz[2 * j + 1]];
            int itemPlace = returnItem(itemDecrease);
            items[subjectPlace][itemPlace] -= decrease;
        }
    }
    return 1;
}

int onlyBuyAction(char **textArray, int *subjects, int nums, int *itemz, int numi)
{
    for (int i = 0; i < nums; i++)
    { // checks if subjects are valid
        if (isValid(textArray[subjects[i]]) == 0)
            return 0;
        if (returnSubject(textArray[subjects[i]]) == -1)
        {
            initializeSubject(textArray[subjects[i]]);
        }
    }

    for (int i = 1; i < numi; i += 2)
    { // checks if items are valid
        if (isValid(textArray[itemz[i]]) == 0)
            return 0;
        if (returnItem(textArray[itemz[i]]) == -1)
            initializeItem(textArray[itemz[i]]);
    }

    for (int i = 0; i < nums; i++)
    { // for every subject
        char *name = textArray[subjects[i]];
        int subjectPlace = returnSubject(name);
        for (int j = 0; j < numi; j += 2)
        { // increase the items by given amount for every item
            int count = atoi(textArray[itemz[j]]);
            char *itemName = textArray[itemz[j + 1]];
            int itemPlace = returnItem(itemName);
            items[subjectPlace][itemPlace] += count;
        }
    }
    return 1;
}

int buyFromAction(char **textArray, int *subjects, int nums, int *itemz, int numi, int seller)
{ // Subject(s) buy Item(s) from Seller
    if (isValid(textArray[seller]) == 0)
        return 0;
    if (returnSubject(textArray[seller]) == -1)
        return 1; // returns if seller doesn't exist
    for (int i = 0; i < nums; i++)
    { // checks if subject names are valid
        if (isValid(textArray[subjects[i]]) == 0)
            return 0;
        if (returnSubject(textArray[subjects[i]]) == -1)
        { // initializes if subject doesn't exist
            initializeSubject(textArray[subjects[i]]);
        }
    }
    for (int i = 0; i < numi; i++)
    { // checks if item names are valid
        if (i % 2 == 1 && isValid(textArray[itemz[i]]) == 0)
            return 0;
        if (i % 2 == 1 && returnItem(textArray[itemz[i]]) == -1)
            return 1;
    }
    // cheks if you can decrease the item by the amount * subject count
    char *sellerName = textArray[seller];
    int sellerPlace = returnSubject(sellerName);
    for (int j = 0; j < numi; j++)
    {
        int decrease = atoi(textArray[itemz[2 * j]]);
        char *itemDecrease = textArray[itemz[2 * j + 1]];
        int itemPlace = returnItem(itemDecrease);
        int normalCount = items[sellerPlace][itemPlace];
        if (normalCount < decrease * nums)
            return 1;
    }

    sellerName = textArray[seller];
    sellerPlace = returnSubject(sellerName);
    for (int j = 0; j < numi; j++)
    { // decreases items by given count * itemCount
        int decrease = atoi(textArray[itemz[2 * j]]);
        char *itemDecrease = textArray[itemz[2 * j + 1]];
        int itemPlace = returnItem(itemDecrease);
        items[sellerPlace][itemPlace] -= decrease * nums;
        for (int i = 0; i < nums; i++)
        { // for every subject increases the item by given value
            char *name = textArray[subjects[i]];
            int subjectPlace = returnSubject(name);
            for (int j = 0; j < numi; j += 2)
                items[subjectPlace][itemPlace] += decrease;
        }
    }
    return 1;
}

int sellToAction(char **textArray, int *subjects, int nums, int *itemz, int numi, int buyer)
{
    if (isValid(textArray[buyer]) == 0)
        return 0;
    if (returnSubject(textArray[buyer]) == -1)
        initializeSubject(textArray[buyer]);
    for (int i = 0; i < nums; i++)
    { // checks if subject names are valid
        if (isValid(textArray[subjects[i]]) == 0)
            return 0;
        if (returnSubject(textArray[subjects[i]]) == -1)
        {
            return 1;
        }
    }
    for (int i = 0; i < numi; i++)
    { // checks if item names are valid
        if (i % 2 == 1 && isValid(textArray[itemz[i]]) == 0)
            return 0;
        if (i % 2 == 1 && returnItem(textArray[itemz[i]]) == -1)
            return 1;
    }
    for (int i = 0; i < nums; i++)
    { // cheks if you can decrease the item by the amount
        char *subjectName = textArray[subjects[i]];
        int subjectPlace = returnSubject(subjectName);
        for (int j = 0; j < numi; j++)
        { // for every item
            int decrease = atoi(textArray[2 * j]);
            char *itemName = textArray[2 * j + 1];
            int itemPlace = returnItem(itemName);
            int normalCount = items[subjectPlace][itemPlace];
            if (normalCount < decrease)
                return 1;
        }
    }
    for (int i = 0; i < nums; i++)
    { // decreases the item by the amount
        char *subjectName = textArray[subjects[i]];
        int subjectPlace = returnSubject(subjectName);
        for (int j = 0; j < numi; j++)
        { // for every item
            int decrease = atoi(textArray[2 * j]);
            char *itemName = textArray[2 * j + 1];
            int itemPlace = returnItem(itemName);
            items[subjectPlace][itemPlace] -= decrease;
        }
    }

    char *buyerName = textArray[buyer]; // increases items of the buyer
    int buyerPlace = returnSubject(buyerName);
    if (buyerPlace == -1)
        return 0;
    for (int j = 0; j < numi; j++)
    { // for every item
        int decrease = atoi(textArray[2 * j]);
        char *itemName = textArray[2 * j + 1];
        int itemPlace = returnItem(itemName);
        items[buyerPlace][itemPlace] += decrease * nums;
    }
    return 1;
}

// CONDITION FUNCTIONS:
int hasCondition(char **textArray, int *subjects, int nums, int *itemz, int numi)
{
    for (int i = 0; i < numi / 2; i++)
    {
        if (isValid(textArray[itemz[2 * i + 1]]) == 0)
            return -1;
        if (returnItem(textArray[itemz[2 * i + 1]]) == -1)
            return 0;
    }
    for (int i = 0; i < nums; i++)
    {
        char *subjectName = textArray[subjects[i]];
        if (isValid(subjectName) == 0)
            return -1; // if name is not valid return -1
        if (returnSubject(subjectName) == -1)
            return 0;
    }

    for (int i = 0; i < numi / 2; i++)
    {
        int itemCount = atoi(textArray[itemz[2 * numi]]);
        char *itemName = textArray[itemz[2 * numi + 1]];
        int itemPlace = returnItem(itemName);
        for (int j = 0; j < nums; j++)
        {
            char *subjectName = textArray[subjects[j]];
            int subjectPlace = returnSubject(subjectName);
            int normalCount = items[subjectPlace][itemPlace];
            if (normalCount != itemCount)
                return 0;
        }
    }
    return 1;
}

int hasMoreCondition(char **textArray, int *subjects, int nums, int *itemz, int numi)
{
    for (int i = 0; i < numi / 2; i++)
    {
        if (isValid(textArray[itemz[2 * i + 1]]) == 0)
            return -1;
        if (returnItem(textArray[itemz[2 * i + 1]]) == -1)
            return 0;
    }
    for (int i = 0; i < nums; i++)
    {
        char *subjectName = textArray[subjects[i]];
        if (isValid(subjectName) == 0)
            return -1; // if name is not valid return -1
        if (returnSubject(subjectName) == -1)
            return 0;
    }

    for (int i = 0; i < numi / 2; i++)
    {
        int itemCount = atoi(textArray[itemz[2 * numi]]);
        char *itemName = textArray[itemz[2 * numi + 1]];
        int itemPlace = returnItem(itemName);
        for (int j = 0; j < nums; j++)
        {
            char *subjectName = textArray[subjects[j]];
            int subjectPlace = returnSubject(subjectName);
            int normalCount = items[subjectPlace][itemPlace];
            if (normalCount <= itemCount)
                return 0;
        }
    }
    return 1;
}

int hasLessCondition(char **textArray, int *subjects, int nums, int *itemz, int numi)
{
    for (int i = 0; i < numi / 2; i++)
    {
        if (isValid(textArray[itemz[2 * i + 1]]) == 0)
            return -1;
        if (returnItem(textArray[itemz[2 * i + 1]]) == -1)
            initializeItem(textArray[itemz[2 * i + 1]]);
    }
    for (int i = 0; i < nums; i++)
    {
        char *subjectName = textArray[subjects[i]];
        if (isValid(subjectName) == 0)
            return -1; // if name is not valid return -1
        if (returnSubject(subjectName) == -1)
            initializeSubject(subjectName);
    }

    for (int i = 0; i < numi / 2; i++)
    {
        int itemCount = atoi(textArray[itemz[2 * numi]]);
        char *itemName = textArray[itemz[2 * numi + 1]];
        int itemPlace = returnItem(itemName);
        for (int j = 0; j < nums; j++)
        {
            char *subjectName = textArray[subjects[j]];
            int subjectPlace = returnSubject(subjectName);
            int normalCount = items[subjectPlace][itemPlace];
            if (normalCount >= itemCount)
                return 0;
        }
    }
    return 1;
}

int atCondition(char **textArray, int *subjects, int nums, int locationIndex)
{
    char *location = textArray[locationIndex];
    if (isValid(location) == 0)
        return -1;
    for (int i = 0; i < nums; i++)
    {
        if (isValid(textArray[subjects[i]]) == 0)
            return -1;
        if (returnSubject(textArray[subjects[i]]) == -1)
            return 0;
    }
    for (int i = 0; i < nums; i++)
    {
        char *subjectName = textArray[subjects[i]];
        int subjectPlace = returnSubject(subjectName);
        if (strcmp(subjectLocation[subjectPlace], location) != 0)
            return 0;
    }
    return 1;
}

int main()
{

    char line[1025];
    int index = 0;
    while (1)
    {
        int printCheck = 0;
        printf(">> ");
        fflush(stdout);

        if (fgets(line, 1025, stdin) == NULL)
        {
            break;
        }

        if (strcmp(line, "exit\n") == 0)
        {
            break;
        }

        // Remove trailing newline character, if any
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';
        char *pointerText = strtok(line, " ");
        int numCommands = 1000;

        // Allocate memory for the array of structs
        struct Command *commandList = malloc(numCommands * sizeof(struct Command));

        if (commandList == NULL)
        {
            // Memory allocation failed
            printf("Memory allocation failed!\n");
            return 1;
        }

        char *textArray[10000];
        int ind = 0;
        int size = 0;
        int commandListSize = 0;

        //---------------------------------------------------------------------------------------------------------------------------------
        // READ THE TEXT AND CREATE THE ARRAY OF TEXT
        while (pointerText != NULL)
        {
            // printf("Kelime: %s\n", pointerText);
            textArray[size] = pointerText;
            size++;
            if (strcmp(pointerText, "NOWHERE") == 0 || strcmp(pointerText, "NOWBODY") == 0 || strcmp(pointerText, "NOTHING") == 0)
            {
                printf("INVALID\n");
                fflush(stdout);
                printCheck = 1;
                break;
            }
            for (int i = 0; i < 16; i++)
            {
                if (strcmp(pointerText, sentWords[i]) == 0)
                {
                    
                    strcpy(commandList[commandListSize].action, sentWords[i]);
                    commandList[commandListSize].index = size - 1;
                    commandList[commandListSize].type = getType(pointerText);
                    commandListSize++;
                    ind = size;
                    break;
                }
            }
            pointerText = strtok(NULL, " ");
        }
        if(printCheck == 1) continue;
        //---------------------------------------------------------------------------------------------------------------------------------
        //---------------------------------------------------------------------------------------------------------------------------------
        // CHECK THE QUESTION AND PRINT NECESSARY OUTPUTS
        int isItQuestion = findIsItQuestion(textArray, size); // -1:Wrong Questıon, 0:Not Question, 1:where, 2:Subjects+TOTAL+item, 3:Subject+TOTAL, 4:who
        if (isItQuestion == -1)
        {
            printf("INVALID\n");
            printCheck = 1;
            fflush(stdout);
            break;
        }

        else if (isItQuestion > 0)
        {
            if (isItQuestion == 1)
            {
                // subject where?
                if (isValid(textArray[0]) == 0)
                {
                    printf("INVALID\n");
                    printCheck = 1;
                    fflush(stdout);
                }
                else
                {
                    int subjectPlace = returnSubject(textArray[0]);
                    if (subjectLocation[subjectPlace] == NULL)
                    {
                        printf("NOWHERE\n");
                        fflush(stdout);
                    }
                    else
                    {
                        char *location = subjectLocation[subjectPlace];
                        printf("%s\n", location);
                        fflush(stdout);
                    }
                }
            }

            if (isItQuestion == 2)
            {
                // subject(s) total item?
                char *item = textArray[size - 2];
                int itemPlace = returnItem(item);
                if ((isValid(item) == 0) || (size % 2 != 0))
                {
                    printf("INVALID\n");
                    fflush(stdout);
                }
                else if ((returnItem(item) == -1))
                {
                    printf("0\n");
                }
                else
                {
                    int j = 1;
                    int count = 0;
                    for (int i = 0; i < size - 3; i++)
                    {
                        char *wordCheck = textArray[i];
                        if ((i % 2 == 1) && (strcmp(wordCheck, "and") == 1))
                        {
                            printf("INVALID\n");
                            fflush(stdout);
                            j = 0;
                            break;
                        }
                        else if (i % 2 == 0)
                        {
                            if (isValid(wordCheck) == 0)
                            {
                                printf("INVALID\n");
                                fflush(stdout);
                                j = 0;
                                break;
                            }
                            int subjectPlace = returnSubject(wordCheck);
                            count += items[subjectPlace][itemPlace];
                        }
                    }
                    if (j == 1)
                    {
                        printf("%d\n", count);
                        fflush(stdout);
                    }
                }
            }

            if (isItQuestion == 3)
            {
                // subject total?
                if (isValid(textArray[0]) == 0)
                {
                    printf("INVALID\n");
                    fflush(stdout);
                }
                else
                {
                    int subjectPlace = returnSubject(textArray[0]);
                    if (subjectPlace == -1)
                    {
                        printf("NOTHING\n");
                        fflush(stdout);
                    }
                    else
                    {
                        int k = 0;
                        for (int i = 0; i < sizeItems; i++)
                        {
                            if (items[subjectPlace][i] != 0)
                            {
                                k++;
                            }
                        }
                        if (k == 0)
                        {
                            printf("NOTHING\n");
                            fflush(stdout);
                        }
                        else
                        {
                            for (int i = 0; i < sizeItems; i++)
                            {
                                if (items[subjectPlace][i] != 0)
                                {
                                    k--;
                                    printf("%d ", items[subjectPlace][i]);
                                    fflush(stdout);
                                    printf("%s", itemArray[i]);
                                    fflush(stdout);
                                    if (k != 0)
                                    {
                                        printf(" and ");
                                        fflush(stdout);
                                    }
                                    else
                                    {
                                        printf("\n");
                                        fflush(stdout);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (isItQuestion == 4)
            {
                // who at location?
                if (isValid(textArray[2]) == 0)
                {
                    printf("INVALID\n");
                    fflush(stdout);
                }
                else
                {
                    char *location = textArray[2];
                    int count = 0;
                    for (int i = 0; i < sizeSubjects; i++)
                    {
                        if (subjectLocation[i] != NULL && strcmp(subjectLocation[i], location) == 0)
                        {
                            count++;
                        }
                    }
                    if (count == 0)
                    {
                        printf("NOBODY\n");
                        fflush(stdout);
                    }
                    else
                    {
                        for (int i = 0; i < sizeSubjects; i++)
                        {
                            if (subjectLocation[i] != NULL && strcmp(subjectLocation[i], location) == 0)
                            {
                                count--;
                                printf("%s", subjectArray[i]);
                                fflush(stdout);
                                if (count != 0)
                                {
                                    printf(" and ");
                                    fflush(stdout);
                                }
                                else
                                {
                                    printf("\n");
                                    fflush(stdout);
                                }
                            }
                        }
                    }
                }
            }

            continue;
        }
        //---------------------------------------------------------------------------------------------------------------------------------
        //  MAKE THE SENTENCES
        int numSentences = 10000;

        // Allocate memory for the array of structs
        struct Sentence *sentenceList = malloc(numSentences * sizeof(struct Sentence));

        if (sentenceList == NULL)
        {
            // Memory allocation failed
            printf("Memory allocation failed!\n");
            return 1;
        }
        int sentenceListSize = 0;
        int sentenceListTextPointer = 0;
        int loopPointer = 0;
        int flag1 = 0;
        int flag2 = 0;
        int commandIndex = 0;

        while (flag1 == 0)
        {
            if (loopPointer >= (commandListSize - 1))
            {
                flag1 = 1;
                break;
            }
            struct Command tempCommand = commandList[loopPointer];
            if (tempCommand.type == 3)
            {
                flag2 = 1;
            }
            loopPointer++;
            while (flag2 == 1)
            {
                if (loopPointer >= (commandListSize))
                {
                    flag2 = 0;
                    flag1 = 1;
                    break;
                }
                struct Command tempSecCommand = commandList[loopPointer];
                struct Command endCommand;
                if (tempSecCommand.type == 0)
                {
                    int realEnd = findRealEnd(textArray, commandList, endCommand.index);

                    sentenceList[sentenceListSize].start = sentenceListTextPointer;
                    sentenceList[sentenceListSize].end = realEnd;
                    sentenceListTextPointer = realEnd;
                    sentenceListSize++;
                    flag2 = 0;
                }
                else if (tempSecCommand.type == 1)
                {
                    // Function To Handle Many And Conditions
                    endCommand = commandList[loopPointer + 1];
                }
                loopPointer++;
            }
        }
        if (sentenceList[sentenceListSize].end != size)
        {
            if (sentenceListSize == 0)
            {
                sentenceList[sentenceListSize].start = 0;
                sentenceList[sentenceListSize].end = size;
                sentenceListSize++;
            }
            else
            {
                sentenceList[sentenceListSize].start = sentenceList[sentenceListSize - 1].end;
                sentenceList[sentenceListSize].end = size;
                sentenceListSize++;
            }
        }

        // MAKE THE COMMAND INDEXES IN SENTENCE
        int commandFindPointer = 0;
        for (int i = 0; i < sentenceListSize; i++)
        {
            int startIndex = sentenceList[i].start;
            int endIndex = sentenceList[i].end;
            int counter = 0;
            for (int i2 = commandFindPointer; i2 < commandListSize; i2++)
            {
                int commandIndex = commandList[i2].index;
                if (commandIndex == endIndex)
                {
                    sentenceList[i].commandIndexes[counter] = i2;
                    commandFindPointer = i2;
                    counter++;
                    sentenceList[i].commandSize = counter;
                    break;
                }
                else if (commandIndex >= startIndex && commandIndex < endIndex)
                {
                    sentenceList[i].commandIndexes[counter] = i2;
                    counter++;
                    sentenceList[i].commandSize = counter;
                }
            }
        }

        int invalidChecker = 0;
        int invalidSecondChecker = 0;
        for (int i = 0; i < sentenceListSize; i++)
        {
            if (invalidChecker == -1)
            {
                invalidSecondChecker = -1;
            }
            int invalidChecker = 0;

            while (invalidChecker == 0)
            {
                int *ptrArr = sentenceList[i].commandIndexes;
                int actionInSentence[20];
                int actionIndex = 0;

                int conditionInSentence[20];
                int conditionIndex = 0;
                int isIfContains = 0;
                int isIfValid = 0;
                int indexOfIf = -1;

                for (int j = 0; j < sentenceList[i].commandSize; j++)
                {
                    if (commandList[ptrArr[j]].type == 0)
                    {
                        actionInSentence[actionIndex] = commandList[ptrArr[j]].index;
                        actionIndex++;
                    }
                    else if (commandList[ptrArr[j]].type == 1 && (strcmp(commandList[ptrArr[j]].action, "has") == 0 || strcmp(commandList[ptrArr[j]].action, "at") == 0))
                    {
                        conditionInSentence[conditionIndex] = commandList[ptrArr[j]].index;
                        conditionIndex++;
                    }
                    else if (commandList[ptrArr[j]].type == 3)
                    {
                        isIfContains = 1;
                        isIfValid = 1;
                        indexOfIf = commandList[ptrArr[j]].index;
                    }
                }
                int conditionstartIndexText = indexOfIf + 1;

                if (conditionIndex == 0 && isIfContains == 1)
                {
                    invalidChecker = -1;
                    invalidSecondChecker = -1;
                    isIfValid = -1;
                    invalidSecondChecker = -1;
                    // printf("invalid: %d\n", invalidChecker);
                    break;
                }
                int resultIfCondition = 0;
                if (isIfContains == 1)
                {
                    // printf("condIndex+: %d\n", conditionIndex);
                    if (conditionIndex == 0)
                    {
                        invalidChecker = -1;
                        invalidSecondChecker = -1;
                        isIfValid = -1;
                        // printf("invalid: %d\n", invalidChecker);
                        break;
                    }
                    for (int j = 0; j < conditionIndex; j++)
                    {

                        // HAS/HAS MORE/HAS LESS THAN PART:
                        if (strcmp(textArray[conditionInSentence[j]], "has") == 0 && (conditionInSentence[j] + 1) < (sentenceList[i].end))
                        {
                            // printf("%s\n", textArray[conditionInSentence[j + 1]]);
                            // printf("%s\n", textArray[conditionInSentence[j + 2]]);
                            int subjectArray[20];
                            int subjArrCounter = 0;
                            int itemtArray[40];
                            int itemArrCounter = 0;
                            // printf("sentenceList[i].end: %d conditionInSentence[j]: %d \n", sentenceList[i].end, conditionInSentence[j]);
                            if (sentenceList[i].end == conditionInSentence[j])
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                isIfValid = -1;
                                break;
                            }

                            //-------------------------------------------------------------------------------------------------------------------------------------
                            // HAS MORE THAN:
                            if (strcmp(textArray[conditionInSentence[j] + 1], "more") == 0 && (conditionInSentence[j] + 2) < (sentenceList[i].end))
                            {
                                if (strcmp(textArray[conditionInSentence[j] + 2], "than") != 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    isIfValid = -1;
                                    break;
                                }
                                if (strcmp(textArray[conditionInSentence[j] + 2], "than") == 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                {
                                    for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                                    {
                                        if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                        {
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            isIfValid = -1;
                                            break;
                                        }
                                        if (isValid(textArray[k]) == 1)
                                        {
                                            // printf("Subject: %s\n", textArray[k]);
                                            subjectArray[subjArrCounter] = k;
                                            subjArrCounter++;
                                        }
                                        if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                        {
                                            // printf("Succesfull And to split Subjects\n");
                                        }
                                        else if (strcmp(textArray[k + 1], "has") == 0)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            isIfValid = -1;
                                            
                                            break;
                                        }
                                    }
                                    if ((subjArrCounter == 0) || invalidChecker == -1 || isIfValid == -1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        isIfValid = -1;
                                       
                                        break;
                                    }
                                    // printf("%s------\n", textArray[conditionInSentence[j] + 1]);
                                    for (int k = conditionInSentence[j] + 3; k < sentenceList[i].end; k = k + 3)
                                    {
                                        /* printf("%s\n",textArray[k]); */
                                        // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                        if ((k + 2) > sentenceList[i].end)
                                        {
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            isIfValid = -1;
                                        
                                            break;
                                        }
                                        if ((conditionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                        {
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                        if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                        {
                                            // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                            itemtArray[itemArrCounter] = k;
                                            itemtArray[itemArrCounter + 1] = k + 1;
                                            itemArrCounter += 2;
                                        }
                                        if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                        {
                                            // printf("Succesfull And to split Items\n");
                                            if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                            {
                                                conditionstartIndexText = k + 3;
                                                break;
                                            }
                                        }
                                        else if ((k + 2) == sentenceList[i].end)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            // printf("----\n");
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            isIfValid = -1;
                                            
                                            break;
                                        }
                                    }
                                }
                                // printf("itemNum: %d\n", itemArrCounter);
                                if ((itemArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                                // printf("-----1--------\n");
                                if (invalidSecondChecker == -1)
                                {
                                    invalidChecker = -1;
                                }
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                resultIfCondition = hasMoreCondition(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);
                            }
                            //-------------------------------------------------------------------------------------------------------------------------------------
                            // HAS LESS THAN:
                            else if (strcmp(textArray[conditionInSentence[j] + 1], "less") == 0 && (conditionInSentence[j] + 2) < (sentenceList[i].end))
                            {
                                if (strcmp(textArray[conditionInSentence[j] + 2], "than") != 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (strcmp(textArray[conditionInSentence[j] + 2], "than") == 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                {
                                    for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                                    {
                                        if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                        {
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                        if (isValid(textArray[k]) == 1)
                                        {
                                            // printf("Subject: %s\n", textArray[k]);
                                            subjectArray[subjArrCounter] = k;
                                            subjArrCounter++;
                                        }
                                        if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                        {
                                            // printf("Succesfull And to split Subjects\n");
                                        }
                                        else if (strcmp(textArray[k + 1], "has") == 0)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                    }
                                    if ((subjArrCounter == 0) || invalidChecker == -1 || isIfValid == -1)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }
                                    // printf("%s------\n", textArray[conditionInSentence[j] + 1]);
                                    for (int k = conditionInSentence[j] + 3; k < sentenceList[i].end; k = k + 3)
                                    {
                                        /* printf("%s\n",textArray[k]); */
                                        // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                        if ((k + 2) > sentenceList[i].end)
                                        {
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                        if ((conditionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                        {
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                        if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                        {
                                            // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                            itemtArray[itemArrCounter] = k;
                                            itemtArray[itemArrCounter + 1] = k + 1;
                                            itemArrCounter += 2;
                                        }
                                        if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                        {
                                            // printf("Succesfull And to split Items\n");
                                            if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                            {
                                                conditionstartIndexText = k + 3;
                                                break;
                                            }
                                        }
                                        else if ((k + 2) == sentenceList[i].end)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            // printf("----\n");
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                    }
                                }
                                // printf("itemNum: %d\n", itemArrCounter);
                                if ((itemArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }

                                if (invalidSecondChecker == -1)
                                {
                                    invalidChecker = -1;
                                }
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                resultIfCondition = hasLessCondition(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);

                                // printf("-----2--------\n");
                            }
                            //-------------------------------------------------------------------------------------------------------------------------------------
                            // HAS:
                            else
                            {
                                if (strcmp(textArray[conditionInSentence[j] + 1], "than") == 0 && (conditionInSentence[j] + 1) < (sentenceList[i].end))
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                                {
                                    if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }
                                    if (isValid(textArray[k]) == 1)
                                    {
                                        // printf("Subject: %s\n", textArray[k]);
                                        subjectArray[subjArrCounter] = k;
                                        subjArrCounter++;
                                    }
                                    if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Subjects\n");
                                    }
                                    else if (strcmp(textArray[k + 1], "has") == 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                      
                                        break;
                                    }
                                }
                                if ((subjArrCounter == 0) || invalidChecker == -1 || isIfValid == -1)
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                                // printf("%s------\n", textArray[conditionInSentence[j] + 1]);
                                for (int k = conditionInSentence[j] + 1; k < sentenceList[i].end; k = k + 3)
                                {
                                    /* printf("%s\n",textArray[k]); */
                                    // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                    if ((k + 2) > sentenceList[i].end)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                       
                                        break;
                                    }
                                    if ((conditionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                       
                                        break;
                                    }
                                    if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                    {
                                        // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                        itemtArray[itemArrCounter] = k;
                                        itemtArray[itemArrCounter + 1] = k + 1;
                                        itemArrCounter += 2;
                                    }
                                    if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Items\n");
                                        if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                        {
                                            conditionstartIndexText = k + 3;
                                            break;
                                        }
                                    }
                                    else if ((k + 2) == sentenceList[i].end)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        // printf("----\n");
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                       
                                        break;
                                    }
                                }
                                // printf("itemNum: %d\n", itemArrCounter);
                                if ((itemArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (invalidSecondChecker == -1)
                                {
                                    invalidChecker = -1;
                                }
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                resultIfCondition = hasCondition(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);

                                // printf("-----3--------\n");
                            }
                        }

                        //-----------------------------------------------------------------------------------------------------------------------------------------------------------
                        // AT PART:
                        else if (strcmp(textArray[conditionInSentence[j]], "at") == 0 && (conditionInSentence[j] + 1) < (sentenceList[i].end))
                        {
                            // printf("Cond: %s %d\n", textArray[conditionInSentence[j]], conditionInSentence[j]);
                            int subjectArray[20];
                            int subjArrCounter = 0;
                            int locationIndex = conditionInSentence[j] + 1;
                            if (locationIndex > (size - 1))
                            {
                                invalidChecker = -1;
                                isIfValid = -1;
                                invalidSecondChecker = -1;
                                
                                break;
                            }

                            for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                            {
                                if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (isValid(textArray[k]) == 1)
                                {
                                    // printf("Subject: %s\n", textArray[k]);
                                    subjectArray[subjArrCounter] = k;
                                    subjArrCounter++;
                                }
                                if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                {
                                    // printf("Succesfull And to split Subjects\n");
                                }
                                else if (strcmp(textArray[k + 1], "at") == 0)
                                {
                                    break;
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                  
                                    break;
                                }
                            }
                            if ((subjArrCounter == 0 || isValid(textArray[locationIndex]) != 1) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                isIfValid = -1;
                                invalidSecondChecker = -1;
                                
                                break;
                            }
                            // printf("Location: %s\n", textArray[locationIndex]);
                            conditionstartIndexText = locationIndex + 2;
                            if (invalidSecondChecker == -1)
                            {
                                invalidChecker = -1;
                            }
                            /*
                            EXPLANATION:
                            -----------
                            int subjectArray[20] = Subject(S) indexes of textArray
                            int subjArrCounter=0 = number of Subjects
                            int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                            int itemArrCounter=0 = number of Items
                            int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                            int locationIndex= index of Location of textArray
                            */
                            resultIfCondition = atCondition(textArray, subjectArray, subjArrCounter, locationIndex);
                            
                        }
                        else
                        {
                            invalidChecker = -1;
                            isIfValid = -1;
                            invalidSecondChecker = -1;
                        }
                    }
                }
                if (isIfValid == -1)
                {
                    break;
                }
                else if (isIfContains != 0)
                {
                    sentenceList[i].end = indexOfIf;
                }
                // printf("isIfContains: %d isIfValid: %d indexOfIf: %d resultIf: %d+ \n", isIfContains, isIfValid, indexOfIf, resultIfCondition);
                //-----------------------------------------------------------------------------------------------------------------------------------------------------------

                if (resultIfCondition == (-1) || (invalidChecker == (-1) || (invalidSecondChecker == (-1))))
                {
                    invalidChecker = -1;
                    invalidSecondChecker = -1;
                    break;
                }
                else if ((indexOfIf == (-1)) || (invalidChecker != (-1) && (invalidSecondChecker != (-1) && resultIfCondition == 1)))
                {
                    int actionstartIndexText = sentenceList[i].start;

                    if (strcmp(textArray[sentenceList[i].start], "and") == 0)
                    {
                        actionstartIndexText = sentenceList[i].start + 1;
                    }
                    if (actionIndex == 0 || (strcmp(textArray[0], "and") == 0) || size <= 1)
                    {
                        invalidChecker = -1;
                        invalidSecondChecker = -1;
                        break;
                    }

                    for (int j = 0; j < actionIndex; j++)
                    {
                        // printf("Action: %s\n", textArray[actionInSentence[j]]);
                        // printf("%d %d\n", sentenceList[i].start, sentenceList[i].end);
                        //  EXCPEPTION PART:
                        if (strcmp(textArray[actionInSentence[j]], "from") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                        {
                            invalidChecker = -1;
                            invalidSecondChecker = -1;
                            break;
                        }
                        if (strcmp(textArray[actionInSentence[j]], "to") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                        {
                            invalidChecker = -1;
                            invalidSecondChecker = -1;
                            break;
                        }

                        //-------------------------------------------------------------------------------------------------------------------------------------------
                        // ONLY SELL PART:
                        if (strcmp(textArray[actionInSentence[j]], "sell") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                        {
                            invalidChecker = -1;
                            invalidSecondChecker = -1;
                            break;
                        }
                        if (strcmp(textArray[actionInSentence[j]], "sell") == 0 && (actionInSentence[j] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "to") != 0)
                        {
                            int subjectArray[20];
                            int subjArrCounter = 0;
                            int itemtArray[40];
                            int itemArrCounter = 0;
                            for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                            {
                                if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (isValid(textArray[k]) == 1)
                                {
                                    // printf("Subject: %s\n", textArray[k]);
                                    subjectArray[subjArrCounter] = k;
                                    subjArrCounter++;
                                }
                                if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                {
                                    // printf("Succesfull And to split Subjects\n");
                                }
                                else if (strcmp(textArray[k + 1], "sell") == 0)
                                {
                                    break;
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                 
                                    break;
                                }
                            }
                            if ((subjArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                
                                break;
                            }
                            for (int k = actionInSentence[j] + 1; k < sentenceList[i].end; k = k + 3)
                            {
                                /* printf("%s\n",textArray[k]); */
                                // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                if ((k + 2) > sentenceList[i].end)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                                if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                {
                                    // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                    itemtArray[itemArrCounter] = k;
                                    itemtArray[itemArrCounter + 1] = k + 1;
                                    itemArrCounter += 2;
                                }
                                if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                {
                                    // printf("Succesfull And to split Items\n");
                                    if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                    {
                                        actionstartIndexText = k + 3;
                                        break;
                                    }
                                }
                                else if ((k + 2) == sentenceList[i].end)
                                {
                                    break;
                                }
                                else
                                {
                                    // printf("----\n");
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                            }
                            // printf("itemNum: %d\n", itemArrCounter);
                            if ((itemArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                
                                break;
                            }
                            /*
                            EXPLANATION:
                            -----------
                            int subjectArray[20] = Subject(S) indexes of textArray
                            int subjArrCounter=0 = number of Subjects
                            int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                            int itemArrCounter=0 = number of Items
                            int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                            */
                        }
                        //-------------------------------------------------------------------------------------------------------------------------------------------
                        // SELL+TO PART:
                        if (strcmp(textArray[actionInSentence[j]], "sell") == 0 && (actionInSentence[j + 1] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "to") == 0)
                        {
                            int subjectArray[20];
                            int subjArrCounter = 0;
                            int itemtArray[40];
                            int itemArrCounter = 0;
                            int buyerSubject;
                            for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                            {
                                if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                                if (isValid(textArray[k]) == 1)
                                {
                                    // printf("Subject: %s\n", textArray[k]);
                                    subjectArray[subjArrCounter] = k;
                                    subjArrCounter++;
                                }
                                if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                {
                                    // printf("Succesfull And to split Subjects\n");
                                }
                                else if (strcmp(textArray[k + 1], "sell") == 0)
                                {
                                    break;
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                              
                                    break;
                                }
                            }
                            if ((subjArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                              
                                break;
                            }
                            // printf("Star: %d End: %d\n", actionInSentence[j] + 1, sentenceList[i].end);
                            for (int k = actionInSentence[j] + 1; k < (actionInSentence[j + 1]); k = k + 3)
                            {
                                /* printf("%s\n",textArray[k]); */
                                if ((k + 2) > actionInSentence[j + 1])
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                              
                                    break;
                                }
                                if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                 
                                    break;
                                }
                                if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                {
                                    // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                    itemtArray[itemArrCounter] = k;
                                    itemtArray[itemArrCounter + 1] = k + 1;
                                    itemArrCounter += 2;
                                }
                                if ((k + 2) < actionInSentence[j + 1] && strcmp(textArray[k + 2], "and") == 0)
                                {
                                    // printf("Succesfull And to split Items\n");
                                    if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                    {
                                        actionstartIndexText = k + 3;
                                        break;
                                    }
                                }
                                else if ((k + 2) == actionInSentence[j + 1])
                                {
                                    break;
                                }
                                else
                                {
                                    // printf("----\n");
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                  
                                    break;
                                }
                            }
                            if ((itemArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                              
                                break;
                            }
                            if (isValid(textArray[actionInSentence[j + 1] + 1]) == 1)
                            {
                                buyerSubject = actionInSentence[j + 1] + 1;
                                if ((actionInSentence[j + 1] + 2 < sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1] + 2], "and") != 0)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                                actionstartIndexText = actionInSentence[j + 1] + 3;
                                // printf("Buyer: %s\n", textArray[buyerSubject]);
                            }
                            else
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                               
                                break;
                            }
                            /*
                            EXPLANATION:
                            -----------
                            int subjectArray[20] = Subject(S) indexes of textArray
                            int subjArrCounter=0 = number of Subjects
                            int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                            int itemArrCounter=0 = number of Items
                            int buyerSubject = index of buyer of textArray
                            int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                            */
                        }

                        //-------------------------------------------------------------------------------------------------------------------------------------------
                        // ONLY BUY PART:
                        if (strcmp(textArray[actionInSentence[j]], "buy") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                        {
                            invalidChecker = -1;
                            break;
                        }
                        if (strcmp(textArray[actionInSentence[j]], "buy") == 0 && (actionInSentence[j] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "from") != 0)
                        {
                            // printf("****\n");
                            // printf("Com: %s\n", textArray[actionInSentence[j]]);
                            // printf("Com: %s\n", textArray[actionInSentence[j + 1]]);

                            int subjectArray[20];
                            int subjArrCounter = 0;
                            int itemtArray[40];
                            int itemArrCounter = 0;
                            for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                            {
                                if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (isValid(textArray[k]) == 1)
                                {
                                    // printf("Subject: %s\n", textArray[k]);
                                    subjectArray[subjArrCounter] = k;
                                    subjArrCounter++;
                                }
                                if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                {
                                    // printf("Succesfull And to split Subjects\n");
                                }
                                else if (strcmp(textArray[k + 1], "buy") == 0)
                                {
                                    break;
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                
                                    break;
                                }
                            }
                            if ((subjArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                
                                break;
                            }
                            // printf("Star: %d End: %d\n", actionInSentence[j] + 1, sentenceList[i].end);
                            for (int k = actionInSentence[j] + 1; k < sentenceList[i].end; k = k + 3)
                            {
                                /* printf("%s\n",textArray[k]); */
                                if ((k + 2) > sentenceList[i].end)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                 
                                    break;
                                }
                                if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                {
                                    // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                    itemtArray[itemArrCounter] = k;
                                    itemtArray[itemArrCounter + 1] = k + 1;
                                    itemArrCounter += 2;
                                }
                                if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                {
                                    // printf("Succesfull And to split Items\n");
                                    if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                    {
                                        actionstartIndexText = k + 3;
                                        break;
                                    }
                                }
                                else if ((k + 2) == sentenceList[i].end)
                                {
                                    break;
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                            }
                            if ((itemArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                               
                                break;
                            }
                            /*
                            EXPLANATION:
                            -----------
                            int subjectArray[20] = Subject(S) indexes of textArray
                            int subjArrCounter=0 = number of Subjects
                            int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                            int itemArrCounter=0 = number of Items
                            int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                            */
                        }

                        //-------------------------------------------------------------------------------------------------------------------------------------------
                        // BUY+FROM PART:
                        if (strcmp(textArray[actionInSentence[j]], "buy") == 0 && (actionInSentence[j + 1] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "from") == 0)
                        {
                            int subjectArray[20];
                            int subjArrCounter = 0;
                            int itemtArray[40];
                            int itemArrCounter = 0;
                            int sellerSubject;
                            for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                            {
                                // printf("%s\n", textArray[k]);
                                if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (isValid(textArray[k]) == 1)
                                {
                                    // printf("Subject: %s\n", textArray[k]);
                                    subjectArray[subjArrCounter] = k;
                                    subjArrCounter++;
                                }
                                if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                {
                                    // printf("Succesfull And to split Subjects\n");
                                }
                                else if (strcmp(textArray[k + 1], "buy") == 0)
                                {
                                    break;
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                            }
                            if ((subjArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                           
                                break;
                            }
                            // printf("Star: %d End: %d\n", actionInSentence[j] + 1, sentenceList[i].end);
                            for (int k = actionInSentence[j] + 1; k < (actionInSentence[j + 1]); k = k + 3)
                            {
                                /* printf("%s\n",textArray[k]); */
                                if ((k + 2) > actionInSentence[j + 1])
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                {
                                    // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                    itemtArray[itemArrCounter] = k;
                                    itemtArray[itemArrCounter + 1] = k + 1;
                                    itemArrCounter += 2;
                                }
                                if ((k + 2) < actionInSentence[j + 1] && strcmp(textArray[k + 2], "and") == 0)
                                {
                                    // printf("Succesfull And to split Items\n");
                                    if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                    {
                                        actionstartIndexText = k + 3;
                                        break;
                                    }
                                }
                                else if ((k + 2) == actionInSentence[j + 1])
                                {
                                    break;
                                }
                                else
                                {
                                    // printf("----\n");
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                   
                                    break;
                                }
                            }
                            if ((itemArrCounter == 0) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                              
                                break;
                            }
                            if (isValid(textArray[actionInSentence[j + 1] + 1]) == 1)
                            {
                                sellerSubject = actionInSentence[j + 1] + 1;
                                if ((actionInSentence[j + 1] + 2 < sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1] + 2], "and") != 0)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                  
                                    break;
                                }
                                actionstartIndexText = actionInSentence[j + 1] + 3;
                                // printf("Seller: %s\n", textArray[sellerSubject]);
                            }
                            else
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                
                                break;
                            }

                            /*
                            EXPLANATION:
                            -----------
                            int subjectArray[20] = Subject(S) indexes of textArray
                            int subjArrCounter=0 = number of Subjects
                            int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                            int itemArrCounter=0 = number of Items
                            int sellerSubject = index of seller of textArray
                            int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                            */
                        }

                        //-------------------------------------------------------------------------------------------------------------------------------------------
                        // GO TO PART:
                        if (strcmp(textArray[actionInSentence[j]], "go") == 0)
                        {
                            if (sentenceList[i].end < (actionInSentence[j] + 2) || strcmp(textArray[actionInSentence[j] + 1], "to") != 0)
                            {
                                // printf("----\n");
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                break;
                            }
                            j = j + 1; // j becomes "to"
                            // printf("%d- %d\n", actionstartIndexText, actionInSentence[j]);
                            int subjectArray[20];
                            int subjArrCounter = 0;
                            int locationIndex = actionInSentence[j] + 1;
                            if (locationIndex > (size - 1))
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                
                                break;
                            }

                            for (int k = actionstartIndexText; k < (actionInSentence[j - 1]); k = k + 2)
                            {
                                if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                if (isValid(textArray[k]) == 1)
                                {
                                    // printf("Subject: %s\n", textArray[k]);
                                    subjectArray[subjArrCounter] = k;
                                    subjArrCounter++;
                                }
                                if ((k + 2) < actionInSentence[j - 1] && strcmp(textArray[k + 1], "and") == 0)
                                {
                                    // printf("Succesfull And to split Subjects\n");
                                }
                                else if (strcmp(textArray[k + 1], "go") == 0)
                                {
                                    break;
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                
                                    break;
                                }
                            }
                            if ((subjArrCounter == 0 || isValid(textArray[locationIndex]) != 1) || invalidChecker == -1)
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                               
                                break;
                            }
                            // printf("Location: %s\n", textArray[locationIndex]);
                            actionstartIndexText = locationIndex + 2;
                            /*
                            EXPLANATION:
                            -----------
                            int subjectArray[20] = Subject(S) indexes of textArray
                            int subjArrCounter=0 = number of Subjects
                            int location =  index of location of textArray
                            int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                            */
                            // printf("*******+++++++ %d %d\n", actionstartIndexText, indexOfIf);
                        }
                        //----------------------------------------------------------------------------------------------------------------------------------------------------------------
                        //
                    }
                }
                else
                {
                    invalidChecker = -1;
                    invalidSecondChecker = -1;
                    break;
                }
                // printf("Sentence Situation: %d\n", invalidChecker);
                invalidChecker = -1; // TESTER
            }
            invalidChecker = 0;
            if (invalidSecondChecker != -1)
            {
                while (invalidChecker == 0)
                {
                    int *ptrArr = sentenceList[i].commandIndexes;
                    int actionInSentence[20];
                    int actionIndex = 0;

                    int conditionInSentence[20];
                    int conditionIndex = 0;
                    int isIfContains = 0;
                    int isIfValid = 0;
                    int indexOfIf = -1;

                    for (int j = 0; j < sentenceList[i].commandSize; j++)
                    {
                        if (commandList[ptrArr[j]].type == 0)
                        {
                            actionInSentence[actionIndex] = commandList[ptrArr[j]].index;
                            actionIndex++;
                        }
                        else if (commandList[ptrArr[j]].type == 1 && (strcmp(commandList[ptrArr[j]].action, "has") == 0 || strcmp(commandList[ptrArr[j]].action, "at") == 0))
                        {
                            conditionInSentence[conditionIndex] = commandList[ptrArr[j]].index;
                            conditionIndex++;
                        }
                        else if (commandList[ptrArr[j]].type == 3)
                        {
                            isIfContains = 1;
                            isIfValid = 1;
                            indexOfIf = commandList[ptrArr[j]].index;
                        }
                    }

                    /*for (int o = 0; o < actionIndex; o++)
                    {
                        printf("%s\n", textArray[actionInSentence[o]]);
                    }

                    printf("%d\n", actionIndex);
                    printf("condIndex: %d\n", conditionIndex);*/
                    int conditionstartIndexText = indexOfIf + 1;

                    if (conditionIndex == 0 && isIfContains == 1)
                    {
                        invalidChecker = -1;
                        invalidSecondChecker = -1;
                        isIfValid = -1;
                        invalidSecondChecker = -1;
                        // printf("invalid: %d\n", invalidChecker);
                        break;
                    }
                    int resultIfCondition = 0;
                    if (isIfContains == 1)
                    {
                        // printf("condIndex+: %d\n", conditionIndex);
                        if (conditionIndex == 0)
                        {
                            invalidChecker = -1;
                            invalidSecondChecker = -1;
                            isIfValid = -1;
                            // printf("invalid: %d\n", invalidChecker);
                            break;
                        }
                        for (int j = 0; j < conditionIndex; j++)
                        {

                            // HAS/HAS MORE/HAS LESS THAN PART:
                            if (strcmp(textArray[conditionInSentence[j]], "has") == 0 && (conditionInSentence[j] + 1) < (sentenceList[i].end))
                            {
                                // printf("%s\n", textArray[conditionInSentence[j + 1]]);
                                // printf("%s\n", textArray[conditionInSentence[j + 2]]);
                                int subjectArray[20];
                                int subjArrCounter = 0;
                                int itemtArray[40];
                                int itemArrCounter = 0;
                                // printf("sentenceList[i].end: %d conditionInSentence[j]: %d \n", sentenceList[i].end, conditionInSentence[j]);
                                if (sentenceList[i].end == conditionInSentence[j])
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    isIfValid = -1;
                                    break;
                                }

                                //-------------------------------------------------------------------------------------------------------------------------------------
                                // HAS MORE THAN:
                                if (strcmp(textArray[conditionInSentence[j] + 1], "more") == 0 && (conditionInSentence[j] + 2) < (sentenceList[i].end))
                                {
                                    if (strcmp(textArray[conditionInSentence[j] + 2], "than") != 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        isIfValid = -1;
                                    
                                        break;
                                    }
                                    if (strcmp(textArray[conditionInSentence[j] + 2], "than") == 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                    {
                                        for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                                        {
                                            if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                            {
                                                invalidChecker = -1;
                                                invalidSecondChecker = -1;
                                                isIfValid = -1;
                                               
                                                break;
                                            }
                                            if (isValid(textArray[k]) == 1)
                                            {
                                                // printf("Subject: %s\n", textArray[k]);
                                                subjectArray[subjArrCounter] = k;
                                                subjArrCounter++;
                                            }
                                            if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                            {
                                                // printf("Succesfull And to split Subjects\n");
                                            }
                                            else if (strcmp(textArray[k + 1], "has") == 0)
                                            {
                                                break;
                                            }
                                            else
                                            {
                                                invalidChecker = -1;
                                                invalidSecondChecker = -1;
                                                isIfValid = -1;
                                               
                                                break;
                                            }
                                        }
                                        if ((subjArrCounter == 0) || invalidChecker == -1 || isIfValid == -1)
                                        {
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            isIfValid = -1;
                                         
                                            break;
                                        }
                                        // printf("%s------\n", textArray[conditionInSentence[j] + 1]);
                                        for (int k = conditionInSentence[j] + 3; k < sentenceList[i].end; k = k + 3)
                                        {
                                            /* printf("%s\n",textArray[k]); */
                                            // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                            if ((k + 2) > sentenceList[i].end)
                                            {
                                                invalidChecker = -1;
                                                invalidSecondChecker = -1;
                                                isIfValid = -1;
                                            
                                                break;
                                            }
                                            if ((conditionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                            {
                                                invalidChecker = -1;
                                                invalidSecondChecker = -1;
                                                
                                                break;
                                            }
                                            if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                            {
                                                // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                                itemtArray[itemArrCounter] = k;
                                                itemtArray[itemArrCounter + 1] = k + 1;
                                                itemArrCounter += 2;
                                            }
                                            if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                            {
                                                // printf("Succesfull And to split Items\n");
                                                if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                                {
                                                    conditionstartIndexText = k + 3;
                                                    break;
                                                }
                                            }
                                            else if ((k + 2) == sentenceList[i].end)
                                            {
                                                break;
                                            }
                                            else
                                            {
                                                // printf("----\n");
                                                invalidChecker = -1;
                                                invalidSecondChecker = -1;
                                                isIfValid = -1;
                                          
                                                break;
                                            }
                                        }
                                    }
                                    // printf("itemNum: %d\n", itemArrCounter);
                                    if ((itemArrCounter == 0) || invalidChecker == -1)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                
                                        break;
                                    }
                                    // printf("-----1--------\n");
                                    if (invalidSecondChecker == -1)
                                    {
                                        invalidChecker = -1;
                                    }
                                    /*
                                    EXPLANATION:
                                    -----------
                                    int subjectArray[20] = Subject(S) indexes of textArray
                                    int subjArrCounter=0 = number of Subjects
                                    int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                    int itemArrCounter=0 = number of Items
                                    int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                    */
                                    resultIfCondition = hasMoreCondition(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);
                                }
                                //-------------------------------------------------------------------------------------------------------------------------------------
                                // HAS LESS THAN:
                                else if (strcmp(textArray[conditionInSentence[j] + 1], "less") == 0 && (conditionInSentence[j] + 2) < (sentenceList[i].end))
                                {
                                    if (strcmp(textArray[conditionInSentence[j] + 2], "than") != 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                       
                                        break;
                                    }
                                    if (strcmp(textArray[conditionInSentence[j] + 2], "than") == 0 && (conditionInSentence[j] + 3) < (sentenceList[i].end))
                                    {
                                        for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                                        {
                                            if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                            {
                                                invalidChecker = -1;
                                                isIfValid = -1;
                                                invalidSecondChecker = -1;
                                             
                                                break;
                                            }
                                            if (isValid(textArray[k]) == 1)
                                            {
                                                // printf("Subject: %s\n", textArray[k]);
                                                subjectArray[subjArrCounter] = k;
                                                subjArrCounter++;
                                            }
                                            if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                            {
                                                // printf("Succesfull And to split Subjects\n");
                                            }
                                            else if (strcmp(textArray[k + 1], "has") == 0)
                                            {
                                                break;
                                            }
                                            else
                                            {
                                                invalidChecker = -1;
                                                isIfValid = -1;
                                                invalidSecondChecker = -1;
                                            
                                                break;
                                            }
                                        }
                                        if ((subjArrCounter == 0) || invalidChecker == -1 || isIfValid == -1)
                                        {
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                        
                                            break;
                                        }
                                        // printf("%s------\n", textArray[conditionInSentence[j] + 1]);
                                        for (int k = conditionInSentence[j] + 3; k < sentenceList[i].end; k = k + 3)
                                        {
                                            /* printf("%s\n",textArray[k]); */
                                            // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                            if ((k + 2) > sentenceList[i].end)
                                            {
                                                invalidChecker = -1;
                                                isIfValid = -1;
                                                invalidSecondChecker = -1;
                                                
                                                break;
                                            }
                                            if ((conditionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                            {
                                                invalidChecker = -1;
                                                invalidSecondChecker = -1;
                                                
                                                break;
                                            }
                                            if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                            {
                                                // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                                itemtArray[itemArrCounter] = k;
                                                itemtArray[itemArrCounter + 1] = k + 1;
                                                itemArrCounter += 2;
                                            }
                                            if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                            {
                                                // printf("Succesfull And to split Items\n");
                                                if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                                {
                                                    conditionstartIndexText = k + 3;
                                                    break;
                                                }
                                            }
                                            else if ((k + 2) == sentenceList[i].end)
                                            {
                                                break;
                                            }
                                            else
                                            {
                                                // printf("----\n");
                                                invalidChecker = -1;
                                                isIfValid = -1;
                                                invalidSecondChecker = -1;
                                               
                                                break;
                                            }
                                        }
                                    }
                                    // printf("itemNum: %d\n", itemArrCounter);
                                    if ((itemArrCounter == 0) || invalidChecker == -1)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }

                                    if (invalidSecondChecker == -1)
                                    {
                                        invalidChecker = -1;
                                    }
                                    /*
                                    EXPLANATION:
                                    -----------
                                    int subjectArray[20] = Subject(S) indexes of textArray
                                    int subjArrCounter=0 = number of Subjects
                                    int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                    int itemArrCounter=0 = number of Items
                                    int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                    */
                                    resultIfCondition = hasLessCondition(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);

                                    // printf("-----2--------\n");
                                }
                                //-------------------------------------------------------------------------------------------------------------------------------------
                                // HAS:
                                else
                                {
                                    if (strcmp(textArray[conditionInSentence[j] + 1], "than") == 0 && (conditionInSentence[j] + 1) < (sentenceList[i].end))
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }
                                    for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                                    {
                                        if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                        {
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                        if (isValid(textArray[k]) == 1)
                                        {
                                            // printf("Subject: %s\n", textArray[k]);
                                            subjectArray[subjArrCounter] = k;
                                            subjArrCounter++;
                                        }
                                        if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                        {
                                            // printf("Succesfull And to split Subjects\n");
                                        }
                                        else if (strcmp(textArray[k + 1], "has") == 0)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                    }
                                    if ((subjArrCounter == 0) || invalidChecker == -1 || isIfValid == -1)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }
                                    // printf("%s------\n", textArray[conditionInSentence[j] + 1]);
                                    for (int k = conditionInSentence[j] + 1; k < sentenceList[i].end; k = k + 3)
                                    {
                                        /* printf("%s\n",textArray[k]); */
                                        // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                        if ((k + 2) > sentenceList[i].end)
                                        {
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                        if ((conditionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                        {
                                            invalidChecker = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                        if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                        {
                                            // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                            itemtArray[itemArrCounter] = k;
                                            itemtArray[itemArrCounter + 1] = k + 1;
                                            itemArrCounter += 2;
                                        }
                                        if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                        {
                                            // printf("Succesfull And to split Items\n");
                                            if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                            {
                                                conditionstartIndexText = k + 3;
                                                break;
                                            }
                                        }
                                        else if ((k + 2) == sentenceList[i].end)
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            // printf("----\n");
                                            invalidChecker = -1;
                                            isIfValid = -1;
                                            invalidSecondChecker = -1;
                                            
                                            break;
                                        }
                                    }
                                    // printf("itemNum: %d\n", itemArrCounter);
                                    if ((itemArrCounter == 0) || invalidChecker == -1)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }
                                    if (invalidSecondChecker == -1)
                                    {
                                        invalidChecker = -1;
                                    }
                                    /*
                                    EXPLANATION:
                                    -----------
                                    int subjectArray[20] = Subject(S) indexes of textArray
                                    int subjArrCounter=0 = number of Subjects
                                    int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                    int itemArrCounter=0 = number of Items
                                    int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                    */
                                    resultIfCondition = hasCondition(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);

                                    // printf("-----3--------\n");
                                }
                            }

                            //-----------------------------------------------------------------------------------------------------------------------------------------------------------
                            // AT PART:
                            else if (strcmp(textArray[conditionInSentence[j]], "at") == 0 && (conditionInSentence[j] + 1) < (sentenceList[i].end))
                            {
                                // printf("Cond: %s %d\n", textArray[conditionInSentence[j]], conditionInSentence[j]);
                                int subjectArray[20];
                                int subjArrCounter = 0;
                                int locationIndex = conditionInSentence[j] + 1;
                                if (locationIndex > (size - 1))
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }

                                for (int k = conditionstartIndexText; k < (conditionInSentence[j]); k = k + 2)
                                {
                                    if (conditionstartIndexText == k && isValid(textArray[k]) != 1)
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }
                                    if (isValid(textArray[k]) == 1)
                                    {
                                        // printf("Subject: %s\n", textArray[k]);
                                        subjectArray[subjArrCounter] = k;
                                        subjArrCounter++;
                                    }
                                    if ((k + 2) < conditionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Subjects\n");
                                    }
                                    else if (strcmp(textArray[k + 1], "at") == 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        isIfValid = -1;
                                        invalidSecondChecker = -1;
                                        
                                        break;
                                    }
                                }
                                if ((subjArrCounter == 0 || isValid(textArray[locationIndex]) != 1) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    isIfValid = -1;
                                    invalidSecondChecker = -1;
                                    
                                    break;
                                }
                                // printf("Location: %s\n", textArray[locationIndex]);
                                conditionstartIndexText = locationIndex + 2;
                                if (invalidSecondChecker == -1)
                                {
                                    invalidChecker = -1;
                                }
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                int locationIndex= index of Location of textArray
                                */
                                resultIfCondition = atCondition(textArray, subjectArray, subjArrCounter, locationIndex);
                                
                            }
                            else
                            {
                                invalidChecker = -1;
                                isIfValid = -1;
                                invalidSecondChecker = -1;
                            }
                        }
                    }
                    if (isIfValid == -1)
                    {
                        break;
                    }
                    else if (isIfContains != 0)
                    {
                        sentenceList[i].end = indexOfIf;
                    }
                    // printf("isIfContains: %d isIfValid: %d indexOfIf: %d resultIf: %d+ \n", isIfContains, isIfValid, indexOfIf, resultIfCondition);
                    //-----------------------------------------------------------------------------------------------------------------------------------------------------------

                    if (resultIfCondition == (-1) || (invalidChecker == (-1) || (invalidSecondChecker == (-1))))
                    {
                        invalidChecker = -1;
                        invalidSecondChecker = -1;
                        break;
                    }
                    else if ((indexOfIf == (-1)) || (invalidChecker != (-1) && (invalidSecondChecker != (-1) && resultIfCondition == 1)))
                    {
                        int actionResult = 0;
                        int actionstartIndexText = sentenceList[i].start;

                        if (strcmp(textArray[sentenceList[i].start], "and") == 0)
                        {
                            actionstartIndexText = sentenceList[i].start + 1;
                        }
                        if (actionIndex == 0 || (strcmp(textArray[0], "and") == 0) || size <= 1)
                        {
                            invalidChecker = -1;
                            invalidSecondChecker = -1;
                            break;
                        }

                        for (int j = 0; j < actionIndex; j++)
                        {
                            // printf("Action: %s\n", textArray[actionInSentence[j]]);
                            // printf("%d %d\n", sentenceList[i].start, sentenceList[i].end);
                            //  EXCPEPTION PART:
                            if (strcmp(textArray[actionInSentence[j]], "from") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                break;
                            }
                            if (strcmp(textArray[actionInSentence[j]], "to") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                break;
                            }

                            //-------------------------------------------------------------------------------------------------------------------------------------------
                            // ONLY SELL PART:
                            if (strcmp(textArray[actionInSentence[j]], "sell") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                            {
                                invalidChecker = -1;
                                invalidSecondChecker = -1;
                                break;
                            }
                            if (strcmp(textArray[actionInSentence[j]], "sell") == 0 && (actionInSentence[j] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "to") != 0)
                            {
                                int subjectArray[20];
                                int subjArrCounter = 0;
                                int itemtArray[40];
                                int itemArrCounter = 0;
                                for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                                {
                                    if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                       
                                        break;
                                    }
                                    if (isValid(textArray[k]) == 1)
                                    {
                                        // printf("Subject: %s\n", textArray[k]);
                                        subjectArray[subjArrCounter] = k;
                                        subjArrCounter++;
                                    }
                                    if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Subjects\n");
                                    }
                                    else if (strcmp(textArray[k + 1], "sell") == 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                       
                                        break;
                                    }
                                }
                                if ((subjArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                for (int k = actionInSentence[j] + 1; k < sentenceList[i].end; k = k + 3)
                                {
                                    /* printf("%s\n",textArray[k]); */
                                    // printf("%d %d \n", (k + 1), sentenceList[i].end);
                                    if ((k + 2) > sentenceList[i].end)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                    {
                                        // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                        itemtArray[itemArrCounter] = k;
                                        itemtArray[itemArrCounter + 1] = k + 1;
                                        itemArrCounter += 2;
                                    }
                                    if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Items\n");
                                        if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                        {
                                            actionstartIndexText = k + 3;
                                            break;
                                        }
                                    }
                                    else if ((k + 2) == sentenceList[i].end)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        // printf("----\n");
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                // printf("itemNum: %d\n", itemArrCounter);
                                if ((itemArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                actionResult = onlySellAction(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);
                            }
                            //-------------------------------------------------------------------------------------------------------------------------------------------
                            // SELL+TO PART:
                            if (strcmp(textArray[actionInSentence[j]], "sell") == 0 && (actionInSentence[j + 1] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "to") == 0)
                            {
                                int subjectArray[20];
                                int subjArrCounter = 0;
                                int itemtArray[40];
                                int itemArrCounter = 0;
                                int buyerSubject;
                                for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                                {
                                    if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isValid(textArray[k]) == 1)
                                    {
                                        // printf("Subject: %s\n", textArray[k]);
                                        subjectArray[subjArrCounter] = k;
                                        subjArrCounter++;
                                    }
                                    if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Subjects\n");
                                    }
                                    else if (strcmp(textArray[k + 1], "sell") == 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                if ((subjArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                // printf("Star: %d End: %d\n", actionInSentence[j] + 1, sentenceList[i].end);
                                for (int k = actionInSentence[j] + 1; k < (actionInSentence[j + 1]); k = k + 3)
                                {
                                    /* printf("%s\n",textArray[k]); */
                                    if ((k + 2) > actionInSentence[j + 1])
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                    {
                                        // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                        itemtArray[itemArrCounter] = k;
                                        itemtArray[itemArrCounter + 1] = k + 1;
                                        itemArrCounter += 2;
                                    }
                                    if ((k + 2) < actionInSentence[j + 1] && strcmp(textArray[k + 2], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Items\n");
                                        if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                        {
                                            actionstartIndexText = k + 3;
                                            break;
                                        }
                                    }
                                    else if ((k + 2) == actionInSentence[j + 1])
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        // printf("----\n");
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                if ((itemArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                if (isValid(textArray[actionInSentence[j + 1] + 1]) == 1)
                                {
                                    buyerSubject = actionInSentence[j + 1] + 1;
                                    if ((actionInSentence[j + 1] + 2 < sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1] + 2], "and") != 0)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    actionstartIndexText = actionInSentence[j + 1] + 3;
                                    // printf("Buyer: %s\n", textArray[buyerSubject]);
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int buyerSubject = index of buyer of textArray
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                actionResult = sellToAction(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter, buyerSubject);
                            }

                            //-------------------------------------------------------------------------------------------------------------------------------------------
                            // ONLY BUY PART:
                            if (strcmp(textArray[actionInSentence[j]], "buy") == 0 && (actionInSentence[j] + 1) >= (sentenceList[i].end))
                            {
                                invalidChecker = -1;
                                break;
                            }
                            if (strcmp(textArray[actionInSentence[j]], "buy") == 0 && (actionInSentence[j] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "from") != 0)
                            {
                                // printf("****\n");
                                // printf("Com: %s\n", textArray[actionInSentence[j]]);
                                // printf("Com: %s\n", textArray[actionInSentence[j + 1]]);

                                int subjectArray[20];
                                int subjArrCounter = 0;
                                int itemtArray[40];
                                int itemArrCounter = 0;
                                for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                                {
                                    if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isValid(textArray[k]) == 1)
                                    {
                                        // printf("Subject: %s\n", textArray[k]);
                                        subjectArray[subjArrCounter] = k;
                                        subjArrCounter++;
                                    }
                                    if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Subjects\n");
                                    }
                                    else if (strcmp(textArray[k + 1], "buy") == 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                if ((subjArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                // printf("Star: %d End: %d\n", actionInSentence[j] + 1, sentenceList[i].end);
                                for (int k = actionInSentence[j] + 1; k < sentenceList[i].end; k = k + 3)
                                {
                                    /* printf("%s\n",textArray[k]); */
                                    if ((k + 2) > sentenceList[i].end)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                    {
                                        // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                        itemtArray[itemArrCounter] = k;
                                        itemtArray[itemArrCounter + 1] = k + 1;
                                        itemArrCounter += 2;
                                    }
                                    if ((k + 2) < sentenceList[i].end && strcmp(textArray[k + 2], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Items\n");
                                        if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                        {
                                            actionstartIndexText = k + 3;
                                            break;
                                        }
                                    }
                                    else if ((k + 2) == sentenceList[i].end)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                if ((itemArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                actionResult = onlyBuyAction(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter);
                            }

                            //-------------------------------------------------------------------------------------------------------------------------------------------
                            // BUY+FROM PART:
                            if (strcmp(textArray[actionInSentence[j]], "buy") == 0 && (actionInSentence[j + 1] + 1) < (sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1]], "from") == 0)
                            {
                                int subjectArray[20];
                                int subjArrCounter = 0;
                                int itemtArray[40];
                                int itemArrCounter = 0;
                                int sellerSubject;
                                for (int k = actionstartIndexText; k < (actionInSentence[j]); k = k + 2)
                                {
                                    // printf("%s\n", textArray[k]);
                                    if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isValid(textArray[k]) == 1)
                                    {
                                        // printf("Subject: %s\n", textArray[k]);
                                        subjectArray[subjArrCounter] = k;
                                        subjArrCounter++;
                                    }
                                    if ((k + 2) < actionInSentence[j] && strcmp(textArray[k + 1], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Subjects\n");
                                    }
                                    else if (strcmp(textArray[k + 1], "buy") == 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                if ((subjArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                // printf("Star: %d End: %d\n", actionInSentence[j] + 1, sentenceList[i].end);
                                for (int k = actionInSentence[j] + 1; k < (actionInSentence[j + 1]); k = k + 3)
                                {
                                    /* printf("%s\n",textArray[k]); */
                                    if ((k + 2) > actionInSentence[j + 1])
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if ((actionInSentence[j] + 1) == k && isNumber(textArray[k]) != 1 && isValid(textArray[k + 1]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isNumber(textArray[k]) == 1 && isValid(textArray[k + 1]) == 1)
                                    {
                                        // printf("Item: %s %s\n", textArray[k], textArray[k + 1]);
                                        itemtArray[itemArrCounter] = k;
                                        itemtArray[itemArrCounter + 1] = k + 1;
                                        itemArrCounter += 2;
                                    }
                                    if ((k + 2) < actionInSentence[j + 1] && strcmp(textArray[k + 2], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Items\n");
                                        if (isNumber(textArray[k + 3]) != 1 || isValid(textArray[k + 4]) != 1)
                                        {
                                            actionstartIndexText = k + 3;
                                            break;
                                        }
                                    }
                                    else if ((k + 2) == actionInSentence[j + 1])
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        // printf("----\n");
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                if ((itemArrCounter == 0) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                if (isValid(textArray[actionInSentence[j + 1] + 1]) == 1)
                                {
                                    sellerSubject = actionInSentence[j + 1] + 1;
                                    if ((actionInSentence[j + 1] + 2 < sentenceList[i].end) && strcmp(textArray[actionInSentence[j + 1] + 2], "and") != 0)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    actionstartIndexText = actionInSentence[j + 1] + 3;
                                    // printf("Seller: %s\n", textArray[sellerSubject]);
                                }
                                else
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }

                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int itemtArray[40]   = Item(S) indexes of textArray such that [3,4,9,10] ----> it means 3>>5, 4>>apple  and 9>>32, 4>>banana
                                int itemArrCounter=0 = number of Items
                                int sellerSubject = index of seller of textArray
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                actionResult = buyFromAction(textArray, subjectArray, subjArrCounter, itemtArray, itemArrCounter, sellerSubject);
                            }

                            //-------------------------------------------------------------------------------------------------------------------------------------------
                            // GO TO PART:
                            if (strcmp(textArray[actionInSentence[j]], "go") == 0)
                            {
                                if (sentenceList[i].end < (actionInSentence[j] + 2) || strcmp(textArray[actionInSentence[j] + 1], "to") != 0)
                                {
                                    // printf("----\n");
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                j = j + 1; // j becomes "to"
                                // printf("%d- %d\n", actionstartIndexText, actionInSentence[j]);
                                int subjectArray[20];
                                int subjArrCounter = 0;
                                int locationIndex = actionInSentence[j] + 1;
                                if (locationIndex > (size - 1))
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }

                                for (int k = actionstartIndexText; k < (actionInSentence[j - 1]); k = k + 2)
                                {
                                    if (actionstartIndexText == k && isValid(textArray[k]) != 1)
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                    if (isValid(textArray[k]) == 1)
                                    {
                                        // printf("Subject: %s\n", textArray[k]);
                                        subjectArray[subjArrCounter] = k;
                                        subjArrCounter++;
                                    }
                                    if ((k + 2) < actionInSentence[j - 1] && strcmp(textArray[k + 1], "and") == 0)
                                    {
                                        // printf("Succesfull And to split Subjects\n");
                                    }
                                    else if (strcmp(textArray[k + 1], "go") == 0)
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        invalidChecker = -1;
                                        invalidSecondChecker = -1;
                                        break;
                                    }
                                }
                                if ((subjArrCounter == 0 || isValid(textArray[locationIndex]) != 1) || invalidChecker == -1)
                                {
                                    invalidChecker = -1;
                                    invalidSecondChecker = -1;
                                    break;
                                }
                                // printf("Location: %s\n", textArray[locationIndex]);
                                actionstartIndexText = locationIndex + 2;
                                /*
                                EXPLANATION:
                                -----------
                                int subjectArray[20] = Subject(S) indexes of textArray
                                int subjArrCounter=0 = number of Subjects
                                int location =  index of location of textArray
                                int invalidChecker = if (-1)--->INVALID, (0)---->VALID
                                */
                                // printf("*******+++++++ %d %d\n", actionstartIndexText, indexOfIf);
                                actionResult = goToAction(textArray, subjectArray, subjArrCounter, locationIndex);
                            }
                            //----------------------------------------------------------------------------------------------------------------------------------------------------------------
                            //
                        }
                        if (actionResult == (0))
                        {
                            invalidChecker = -1;
                            invalidSecondChecker = -1;
                            break;
                        }
                    }
                    else
                    {
                        invalidChecker = -1;
                        invalidSecondChecker = -1;
                        break;
                    }
                    // printf("Sentence Situation: %d\n", invalidChecker);
                    invalidChecker = -1; // TESTER
                }
            }
        }
        //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        // CONSOLE WRITING:
        if ((invalidChecker == (-1) || invalidSecondChecker == (-1)))
        {
            printf("INVALID\n");
            fflush(stdout);
        }
        else
        {
            printf("OK\n");
            fflush(stdout);
        }
        // printf("Checker 1: %d\n", invalidChecker);
        // printf("Checker 2: %d\n", invalidSecondChecker);
        free(sentenceList);
        free(commandList);
    }

    return 0;
}
