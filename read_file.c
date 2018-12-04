#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fptr = fopen("inv.lis", "r");
    fscanf(fptr, "\nCircuit: .options temp=75\n\n");
    fscanf(fptr, "Doing analysis at TEMP = 27.000000 and TNOM = 27.000000\n\n\n");
    fscanf(fptr, "Initial Transient Solution\n");
    fscanf(fptr, "--------------------------\n\n");
    fscanf(fptr,"Node                                   Voltage\n");
    fscanf(fptr, "----                                   -------\n");
    fscanf(fptr, "vdd                                          1\n");
    fscanf(fptr, "in                                           1\n");
    fscanf(fptr, "out                                          0\n");
    fscanf(fptr, "vin#branch                                   0\n");
    double temp;
    int temp_1;
    fscanf(fptr, "vdd#branch                        %le\n\n\n", &temp);
    fscanf(fptr, "No. of Data Rows : %d\n", &temp_1);
    fscanf(fptr,"             Transient Analysis\n\n");
    double rdelay = 0.0;
    double fdelay = 0.0;
    fscanf(fptr, "rdelay              =  %le targ=  %le trig=  %le\n", &rdelay, &temp, &temp);
    fscanf(fptr, "fdelay              =  %le targ=  %le trig=  %le\n", &fdelay, &temp, &temp);
    fclose(fptr);
    if(rdelay < fdelay) {
        return fdelay;
    }
    else{
        return rdelay;
    }
}