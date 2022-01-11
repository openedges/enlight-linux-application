
#ifndef __DEV_DISP_H__
#define __DEV_DISP_H__

#include <linux/ioctl.h>
#include "enx_common.h"
#include "enx_types.h"
#include "enx_define.h"
#include "enx_disp.h"

typedef struct _DEV_DISP_DATA
{
	int posx;
	int posy;
	int width;
	int height;
	int depth;	// bits per pixel
	unsigned int PhysAddr;
	unsigned long size;

} DEV_DISP_DATA_S;



#define DISP_DRV_NAME  "enx_disp"

#define DISP_DRV_MAGICNUM       (0xEA)

#define CMD_DISP_INIT				_IOR(DISP_DRV_MAGICNUM, 10, DEV_DISP_DATA_S)
#define CMD_DISP_EXIT				_IO(DISP_DRV_MAGICNUM, 11)


#define DISP_CMD_MAKE(cmd)     ( _IO( DISP_DRV_MAGICNUM, cmd) )
#define DISP_CMD_GET(cmd)      ( _IOC_NR(cmd) )
#define DISP_CMD_IS_VALID(cmd) ( (_IOC_TYPE(cmd) == DISP_DRV_MAGICNUM ) ? 1 : 0)


#endif // #define __DEV_DISP_H__
