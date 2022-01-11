
/* 
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_AFD_H__
#define __ENX_AFD_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "enx_common.h"
#include "enx_isp.h"

typedef enum 
{
	TYPE_AF1_SUM1_LOCK = 0,		// AF window1 FIR1 integral value	
	TYPE_AF1_SUM2_LOCK,			// AF window1 FIR2 integral value	
	TYPE_AF2_SUM1_LOCK,			// AF window2 FIR1 integral value	
	TYPE_AF2_SUM2_LOCK,			// AF window2 FIR2 integral value	
	TYPE_AF1_CLP_SUM1_LOCK,		// AF window1 FIR1 integral value except clip area	
	TYPE_AF1_CLP_SUM2_LOCK,		// AF window1 FIR2 integral value except clip area	
	TYPE_AF2_CLP_SUM1_LOCK,		// AF window2 FIR1 integral value except clip area	
	TYPE_AF2_CLP_SUM2_LOCK,		// AF window2 FIR2 integral value except clip area	
	TYPE_AF1_YSUM1_LOCK,		// Y integral value of applied area to AF window1 FIR1 filter	
	TYPE_AF1_YSUM2_LOCK,		// Y integral value of applied area to AF window1 FIR2 filter	
	TYPE_AF2_YSUM1_LOCK,		// Y integral value of applied area to AF window2 FIR1 filter	
	TYPE_AF2_YSUM2_LOCK,		// Y integral value of applied area to AF window2 FIR2 filter	
	TYPE_AF1_CLCNT_LOCK,		// AF window1 clip count value	
	TYPE_AF2_CLCNT_LOCK,		// AF window2 clip count value	
	TYPE_VAF1_SUM_LOCK,			// AF window1 vertical integral value	
	TYPE_VAF2_SUM_LOCK,			// AF window2 vertical integral value

	TYPE_AFD_MAX,
} AFD_TYPE_E;

typedef struct 
{
	AFD_TYPE_E eAfdIndex;
    UInt32 nAfdValue;

} AFD_INFO_S;


Int32 ENX_AFD_Init(void);
void ENX_AFD_Exit(void);

Int32 ENX_AFD_GetValue(AFD_INFO_S *pAfdData);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_AFD_H__

