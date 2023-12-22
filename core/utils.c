#include <stdlib.h>
#include <string.h>
#include "utils.h"

int STR_COPY(char *dest, int max, const char *src) {
    int i;
    for (i = 0; i < max - 1 && src[i]; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return i;
}

char *STR_DUMP(const char *str) {
    int length = strlen(str) + 1;
    char *buffer = (char *)malloc(length);
    STR_COPY(buffer, length + 1, str);
    return buffer;
}

double k_atof(const char *str)
{
  int sign;
  double number = 0.0, power = 1.0;

  while(*str == ' ' || *str == '\n' || *str == '\t' || *str == '\r')
    ++str;

  sign = (*str == '-') ? -1 : 1; // Save the sign

  if(*str == '-' || *str == '+') // Skip the sign
    str++;

  while(*str >= '0' && *str <= '9') // Digits before the decimal point
  {
    number = 10.0 * number + (*str - '0');
    str++;
  }

  if(*str == '.') // Skip the decimal point
    str++;

  while(*str >= '0' && *str <= '9') // Digits after the decimal point
  {
    number = 10.0 * number + (*str - '0');
    power *= 10.0;
    str++;
  }


   return sign * number / power;
}

#define MAX_PRECISION   (10)

static const double rounders[MAX_PRECISION + 1] =
{
    0.5,                // 0
    0.05,               // 1
    0.005,              // 2
    0.0005,             // 3
    0.00005,            // 4
    0.000005,           // 5
    0.0000005,          // 6
    0.00000005,         // 7
    0.000000005,        // 8
    0.0000000005,       // 9
    0.00000000005       // 10
};

char * k_ftoa(double f, char * buf, int precision) {
    char * ptr = buf;
    char * p = ptr;
    char * p1;
    char c;
    long intPart;

    // check precision bounds
    if (precision > MAX_PRECISION)
        precision = MAX_PRECISION;

    // sign stuff
    if (f < 0) {
        f = -f;
        *ptr++ = '-';
    }

    // negative precision == automatic precision guess
    if (precision < 0) {
        if (f < 1.0) precision = 6;
        else if (f < 10.0) precision = 5;
        else if (f < 100.0) precision = 4;
        else if (f < 1000.0) precision = 3;
        else if (f < 10000.0) precision = 2;
        else if (f < 100000.0) precision = 1;
        else precision = 0;
    }

    // round value according the precision
    if (precision)
        f += rounders[precision];

    // integer part...
    intPart = (long)f;
    f -= intPart;

    if (!intPart)
        *ptr++ = '0';
    else {
        // save start pointer
        p = ptr;

        // convert (reverse order)
        while (intPart) {
            *p++ = '0' + intPart % 10;
            intPart /= 10;
        }

        // save end pos
        p1 = p;

        // reverse result
        while (p > ptr) {
            c = *--p;
            *p = *ptr;
            *ptr++ = c;
        }

        // restore end pos
        ptr = p1;
    }

    // decimal part
    if (precision) {
        // place decimal point
        *ptr++ = '.';

        // convert
        while (precision--) {
            f *= 10.0;
            c = (int)f % 10;
            *ptr++ = '0' + c;
            f -= c;
        }
    }

    // terminating zero
    *ptr = 0;

    {
        int j = 0;
        char *cBuf = buf;
        for (j = strlen(cBuf) - 1; j > 0 && cBuf[j] == '0'; --j) {
            cBuf[j] = 0;
        }
        if (j >= 1 && cBuf[j] == '.') cBuf[j] = 0;
    }

    return buf;
}

long k_atoi(const char* S)
{
    long num = 0;
 
    int i = 0;
 
    // run till the end of the string is reached, or the
    // current character is non-numeric
    while (S[i] && (S[i] >= '0' && S[i] <= '9'))
    {
        num = num * 10 + (S[i] - '0');
        i++;
    }
 
    return num;
}

/* A utility function to reverse a string  */
void k_itoa_reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char t = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = t;
        start++;
        end--;
    }
}
 
// Implementation of itoa()
char* k_itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
 
    /* Handle 0 explicitly, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    k_itoa_reverse(str, i);
 
    return str;
}
