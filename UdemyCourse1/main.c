#include <stdio.h>

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
    printf("Lutfen bir sayi girin: ");
    scanf("%d", &index);
    switch (index) {
        case 0:print1();
            break;
        case 1:print2();
            break;
        // Add new case here
        default:printf("Undefined index");
    }
}

int main() {
    print();
    return 0;
}