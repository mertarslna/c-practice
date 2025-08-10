#ifndef COURSE1_H
#define COURSE1_H

void printNameSurname() {
    char name[50],surname[50];
    printf("Please enter your name: ");
    scanf("%s", &name);
    printf("Please enter your surname: ");
    scanf("%s", &surname);
    printf(name);
    printf(" ");
    printf(surname);
}

void print1()
{
    printf("David says , \" Programming is fun !\"\n");
    printf("Good day!");
}

void print2()
{
    printf("David says , \" Programming is fun !\".\n");
    printf("**Conditions apply , \"Offers valid until tomorrow\"\n");
    printf("C:\\My computer\\My folder\n");
    printf("D:/My documents/My file\n");
    printf("\\ \\ \\ \\ Today is holiday \\ \\ \\ \\\n");
    printf("This is a triple quoted string \"\"\" This month has 30 days \"\"\"\n");
}

void print3() {
    /*
     Add new printf function here
    */
}

void print() {
    int index;
    printf("Please enter the index: ");
    scanf("%d", &index);
    switch (index) {
        case 0:printNameSurname();
            break;
        case 1:print1();
            break;
        case 2:print2();
            break;
        case 3:print3();
            break;
            // Add new case here
        default:printf("Undefined index");
    }
}

#endif
