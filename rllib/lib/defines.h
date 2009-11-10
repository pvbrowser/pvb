/* prevent multiple inclusion */
#ifndef   DRVSUDEF_H_INCLUDED
  #define DRVSUDEF_H_INCLUDED

/* Device state flags */
#define DRV_OPEN                0x8000
#define DEV_INIT                0x0100
#define DEV_INFO                0x0200
#define DEV_INFO_FI             0x0400
#define HOST_RDY                0x0800
#define BOARD_0                 0x0001
#define BOARD_1                 0x0002
#define BOARD_2                 0x0004
#define BOARD_3                 0x0008

#endif  // DRVSUDEF_H_INCLUDED
