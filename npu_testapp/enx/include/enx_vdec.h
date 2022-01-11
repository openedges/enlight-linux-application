
/* 
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_VDEC_H__
#define __ENX_VDEC_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "enx_common.h"
#include "enx_isp.h"
#include "enx_codec.h"

Int32 ENX_VDEC_Init();
Int32 ENX_VDEC_Start();
Int32 ENX_VDEC_Stop();
Int32 ENX_VDEC_GetParam();
Int32 ENX_VDEC_SetParam();
Int32 ENX_VDEC_SendBitstream();
Int32 ENX_VDEC_Debug();
Int32 ENX_VDEC_Exit();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_DEC_H__
