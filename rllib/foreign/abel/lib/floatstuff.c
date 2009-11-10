#include "libabplc5.h"

float itof (unsigned int data1, unsigned int data2)
{
union cdata c;

c.idata[1] = data1;
c.idata[0] = data2;
return c.fdata;
}

struct floatstuff ftoi (float data)
{
struct floatstuff outdata;
union cdata c;
c.fdata = data;
outdata.data1 = c.idata[1];
outdata.data2 = c.idata[0];
return outdata;
}
