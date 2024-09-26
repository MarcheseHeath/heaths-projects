// HEADER
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

void DecToBin();
void DecToIEEE();
void ConvertIEEE();
void BinToDec();
void Ones();
void Twos();
void BinAdd();
void Reduce();
void printFormat();

// INITIALIZE VARIABLES
bool run = true, sign = false, point = false; // Decimal specifiers
int var; // Create a temporary varible for selection and
double dWhole; // Temporary double
float dec; // Decimal input value
int whole; // Whole number component of decimal
float frac; // Fractional number component of decimal
char bin[100], bin2[50], fracBin[50]; // Binary character arrays
char dot = '.'; // Used to check for fractional binaries


// MAIN PROGRAM
int main()
{
    while(run)
    {
        printf("Make a selection: \n1. Decimal to Twos\n2. Decimal to IEEE754\n3. IEE754 to Decimal\n4. Binary to Decimal\n5. Binary Addition (No Fraction)\n6. End Program\n\nSelection: ");
        scanf(" %d",&var);

        switch (var)
        {
            case 1: Reduce(); DecToBin(); var = 0; break;
            case 2: Reduce(); DecToIEEE(); var = 0; break;
            case 3: ConvertIEEE(); var = 0; break;
            case 4: BinToDec(); var = 0; break;
            case 5: BinAdd(); var = 0; break;
            case 6: run = false; var = 0; break;
            default: printf("\nPlease make a valid selection\n\n");
        }
    }
}

// CUSTOM FUNCTIONS
void DecToBin()
{
    int i = 0;
    while (whole > 0) // Create an array of Binary values (LSB to MSB)
    {
        if (whole % 2 == 1) // If remainder present result is '1'
        {
            bin[i] = '1';
        }
        else // Otherwise result must be '0'
        {
            bin[i] = '0';
        }
        whole = whole / 2; // Perform division until whole = 0
        i++;
    }

    if (strlen(bin) < 32) // Pad Binary with zeros in reverse if needed
    {
        for (int x = strlen(bin); x < 32; x++)
        {
            bin[x] = '0';
        }
    };

    if (point) // Create an array of fractional Binary values (LSB to MSB)
    {
        for (int x = 31; x >= 0; x--)
        {
            if ((frac * 2) >= 1) // Check if double current fraction is at least '1'
            {
                fracBin[x] = '1';
                frac = (frac * 2) - 1;
            }
            else // Otherwise it must be less than '0' so continue
            {
                fracBin[x] = '0';
                frac = (frac * 2) ;
            }
        }
    }

    if (sign) // Create twos complement if negative is indicated
    {
        Ones();
        Twos();
    }

    printFormat(); // Print determined binary representation
    sign = false; // Reset sign check
    point = false; // Reset fraction check
}

void DecToIEEE()
{
    memset(bin, '0', 32); // Set temp values for array

    if (sign)   // Determine sign bit
    {
        bin[31] = '1';
    }
    else
    {
        bin[31] = '0';
    }

    var = 127 + floor(log2(dec)); // Determine exponent
    whole = var - 127; // Save exponent for mantissa
    printf("exponent: %d -> %d\n",whole,var);

    if (var <= 0) // Subnormal number (exponent all zero)
    {
        for (int x = 23; x < 31; x++)
        {
            bin[x] = '0';
        }
    }
    else // Normal number exponent
    {
        int i = 0;
        while (var > 0) // Create a temp array of exponent Binary values (LSB to MSB)
        {
            if (var % 2 == 1) // If remainder present result is '1'
            {
                bin2[i] = '1';
            }
            else // Otherwise result must be '0'
            {
                bin2[i] = '0';
            }
            var = var / 2; // Perform division until whole = 0
            i++;
        }

        if (strlen(bin2) < 8) // Pad exponent Binary value if less than 8 bit
        {
            for (int x = strlen(bin2); x < 8; x++)
            {
                bin[x] = '0';
            }
        }
        for (int x = 0; x < 8; x++) // Append the exponent binary values to the Binary array
        {
            bin[30-x] = bin2[7-x];
        }
    }

    dec = (dec * pow(2,-whole)) - 1;
    var = round(dec * pow(2,23));
    printf("mantissa: %f -> %d\n\n",dec,var);

    int i = 0;
    while (var > 0) // Create a temp array of mantissa Binary values (LSB to MSB)
    {
        if (var % 2 == 1) // If remainder present result is '1'
        {
            bin2[i] = '1';
        }
        else // Otherwise result must be '0'
        {
            bin2[i] = '0';
        }
        var = var / 2; // Perform division until whole = 0
        i++;
    }

    if (strlen(bin2) < 23) // Pad mantissa Binary value if less than 23 bit
    {
        for (int x = strlen(bin2); x < 23; x++)
        {
            bin[x] = '0';
        }
    }
    for (int x = 0; x < 23; x++) // Append the mantissa binary values to the Binary array
    {
        bin[x] = bin2[x];
    }

    point = false; // Reset fraction check
    printFormat();
    sign = false; // Reset sign check
}

void ConvertIEEE()
{
    printf("\nIEE754: "); // Read an IEEE754 value
    scanf(" %s",bin);

    if (bin[0] == '1') // Check sign bit
    {
        sign = true;
    }

    var = 0;
    for (int x = 1; x < 9; x++) // Determine the exponent value
    {
        if (bin[x] == '1')
        {
            var = var + pow(2,8-x);
        }
    }
    var = var - 127;

    frac = 0;
    for (int x = 9; x < 32; x++) // Determine the mantissa value
    {
        if (bin[x] == '1')
        {
            frac = frac + pow(2,8-x);
        }
    }
    frac = frac + 1;

    dec = frac / pow(2,-var); // Find the decimal value
    if (sign)
    {
        dec = dec * (-1);
    }

    memset(bin, '\0', 32); // Clear binary
    sign = false;
    printf("\n%f\n\n",dec);
}

void BinToDec()
{
    printf("\nBinary: "); // Read a binary value
    scanf(" %s",bin);

    const char *ptr = strchr(bin, dot);
    dec = 0, frac = 0;

    if (ptr)
    {
        int index = ptr - bin;
        for (int x = 0; x < index; x++)
        {
            if (bin[x] == '1')
            {
                dec = dec + pow(2,index-1-x);
            }
        }
        for (int x = index + 1; x < strlen(bin); x++)
        {
            if (bin[x] == '1')
            {
                frac = frac + pow(2,-(x-index));
            }
        }
        dec = dec + frac;
    }
    else
    {
        for (int x = 0; x < strlen(bin); x++)
        {
            if (bin[x] == '1')
            {
                dec = dec + pow(2,strlen(bin)-1-x);
            }
        }
    }

    memset(bin, '\0', 32); // Clear binary 1
    memset(fracBin, '\0', 32); // Clear binary 1
    printf("\n%f\n\n",dec);
}

void Ones() // Create the one's complement of Binary value
{
    for (int x = 0; x < 32; x++) // Reverse all bits of the Binary value
    {
        if (bin[x] == '0')
        {
            bin[x] = '1';
        }
        else if (bin[x] == '1')
        {
            bin[x] = '0';
        }
        if (point)
        {
            if (fracBin[x] == '0')
            {
                fracBin[x] = '1';
            }
            else if (fracBin[x] == '1')
            {
                fracBin[x] = '0';
            }
        }
    }
}

void Twos() // Create the two's complement of Binary value
{
    var = 1; // Start with carry 1 to add to one's complement


    if (point) // Handle the fractional part if needed
    {
        for (int x = 0; x < 32; x++)
        {
            if (fracBin[x] == '1' && var == 1) // Flip bit and keep carry
            {
                fracBin[x] = '0';
            }
            else if (fracBin[x] == '0' && var == 1) // Flip bit and lose carry
            {
                fracBin[x] = '1';
                var = 0;
            }
        }
    }

    for (int x = 0; x < 32; x++) // Handle the integer part
    {
        if (bin[x] == '1' && var == 1) // Flip bit and keep carry if adding two ones
        {
            bin[x] = '0';
        }
        else if (bin[x] == '0' && var == 1) // Flip bit and lose carry if adding one and zero
        {
            bin[x] = '1';
            var = 0;
        }
    }
}

void BinAdd() // Add two binary values
{
    var = 0; // Start with no carry bit

    printf("\nFirst Binary: "); // Read first binary value (MSB to LSB)
    scanf(" %s",bin);
    printf("Second Binary: "); // Read second binary value (MSB to LSB)
    scanf(" %s",bin2);

    memmove(bin+(32 - strlen(bin)), bin, strlen(bin)); // Pad first Binary value
    memset(bin, '0', (32 - strlen(bin)));
    memmove(bin2+(32 - strlen(bin2)), bin2, strlen(bin2)); // Pad second Binary value
    memset(bin2, '0', (32 - strlen(bin2)));

    for (int x = 31; x >= 0; x--) // Perform addition
    {
        printf("b1:%c b2:%c c:%d ->",bin[x],bin2[x],var);
        if (bin[x] == '0' && bin2[x] == '0' && var == 1) // Set bit and lose carry
        {
            bin[x] = '1';
            var = 0;
        }
        else if (bin[x] == '1' && bin2[x] == '1' && var == 0) // Set carry and lose bit
        {
            bin[x] = '0';
            var = 1;
        }
        else if (((bin[x] == '0' && bin2[x] == '1') || (bin[x] == '1' && bin2[x] == '0')) && var == 1) // Lose bit
        {
            bin[x] = '0';
        }
        else if (((bin[x] == '0' && bin2[x] == '1') || (bin[x] == '1' && bin2[x] == '0')) && var == 0) // Set bit
        {
            bin[x] = '1';
        }
        printf("b1:%c b2:%c c:%d\n",bin[x],bin2[x],var);
    }

    char temp; // Temp value for array reversal
    for (int x = 0; x < 16; x++) // Reverse the Binary value (LSB to MSB) for printFormat
    {
        temp = bin[31 - x];
        bin[31 - x] = bin[x];
        bin[x] = temp;
    }

    printFormat();
}

void Reduce() // Break decimal value into component values and determine signage
{
    /*
    char buffer[20];
    var = 0;
    while (sscanf(buffer, "%f", &dec) != 1 )
    {
        if (var != 0)
        {
            printf("\nDecimal: ");
        }
        fgets(buffer, sizeof(buffer), stdin);
        var++;
    }
    */

    printf("\nDecimal: "); // Read a decimal value
    scanf(" %f", &dec);
    printf("\n");

    if (dec < 0) // Check if decimal is negative
    {
        sign = true;
        dec = dec * (-1);
    }

    frac = modf(dec, &dWhole); // Obtain fractional part of decimal
    whole = dWhole; // Obtain integer part of decimal

    if (frac > 0) // Check for a fractional part
    {
        point = true;
    }
}

void printFormat() // Print the formatted binary value
{
    for (int x = 31; x >= 0; x--) // Print the binary array in correct order (MSB to LSB)
    {
        printf("%c",bin[x]);
        var = x % 4; // Used to check if there should be space between four bits
        if (var == 0 && x != 0)
        {
            printf(" ");
        }
    }

    if (point) // If neccessary print the decimal array in correct order (MSB to LSB)
    {
        printf(".");
        for (int x = 31; x >= 0; x--)
        {
            printf("%c",fracBin[x]);
            var = x % 4; // Used to check if there should be space between four bits
            if (var == 0 && x != 0)
            {
                printf(" ");
            }
        }
    }

    memset(bin, '\0', 32); // Clear binary 1
    memset(bin2, '\0', 32); // Clear binary 2
    memset(fracBin, '\0', 32); // Clear fractional binary

    printf("\n\n");
}