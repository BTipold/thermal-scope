#include "test.h"
#include <math.h>
#include <stdlib.h>     //exit()
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("please input LCD type!\r\n");
        printf("example: sudo ./main -1.3\r\n");
        exit(1);
    }
    
    double size;
    sscanf(argv[1],"%lf",&size);
    size = (fabs(size));
    
    if(size<0.1||size>10){
        printf("error: bad size\r\n");
        exit(1);
    }
    else {
        printf("%.2lf inch LCD Moudule\r\n",size);
    }
    
    LCD_1IN28_test();
    sleep(10);

}
