
/* 
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_VSYS_H__
#define __ENX_VSYS_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>

#include "enx_common.h"

Int32 ENX_VSYS_Init(void);
void ENX_VSYS_Exit(void);

Int32 ENX_VSYS_GetCurrentTimeStamp(ULONG *pCurPts);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_VSYS_H__
