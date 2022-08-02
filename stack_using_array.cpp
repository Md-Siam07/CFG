#include<stdio.h>

int a[10] = {1,2,3,4,5};
int size = 5;

push(int value){
    if(size>=10){
        printf("you can not insert anymore");
    }
    a[size] = value;
    size++;
}

pop()
{
    //a[size-1] = -99999;
    size--;
}

print()
{
    for(int i=0;i<size;i++){
        printf("%d ", a[i]);
    }
    printf("\n");
}

int main()
{
    push(7);
    print();
    pop();
    print();
    pop();
    print();
}
