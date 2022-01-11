
/*
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_VISP_H__
#define __ENX_VISP_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "enx_common.h"
#include "enx_isp.h"


Int32 ENX_VISP_DefaultReset(void);

Int32 ENX_VISP_GetPar(VISP_PARAM_UNIT_S *pParStr, BYTE bParamCnt);
Int32 ENX_VISP_SetPar(VISP_PARAM_UNIT_S *pParStr, BYTE bParamCnt);
Int32 ENX_VISP_GetParAttr(VISP_PARAM_UNIT_S *pParStr, VISP_PARAM_ATTR_S *pParAttr, BYTE bParamCnt);

Int32 ENX_VISP_GetPrivacyArea(Int32 nPvcNum, UP_MENU_AREA_BOX_S *pVispPVCArea);
Int32 ENX_VISP_SetPrivacyArea(Int32 nPvcNum, VISP_AREA_BOX_E eId, UP_MENU_AREA_BOX_S *pVispPVCArea);
Int32 ENX_VISP_GetMotionArea(Int32 nImdNum, UP_MENU_AREA_BOX_S *pVispIMDArea);
Int32 ENX_VISP_SetMotionArea(Int32 nImdNum, VISP_AREA_BOX_E eId, UP_MENU_AREA_BOX_S *pVispIMDArea);
Int32 ENX_VISP_GetYCInfo(Int32 nYcNum, UP_MENU_YC_S *pVispYCInfo);
Int32 ENX_VISP_SetYCInfo(Int32 nYcNum, VISP_YC_E eId, UP_MENU_YC_S *pVispYCInfo);
Int32 ENX_VISP_SetFontText(char* cStr, Int32 nPosX, Int32 nPosY);
Int32 ENX_VISP_SetFontColor(const Int32 nColorNum, Int32 nPosX, Int32 nPosY, Int32 nLen);
Int32 ENX_VISP_GetFontAttr(VISP_FONT_ATTR_E eId, UP_FONT_ATTR_S *pVispFontAttr);
Int32 ENX_VISP_SetFontAttr(VISP_FONT_ATTR_E eId, UP_FONT_ATTR_S *pVispFontAttr);
Int32 ENX_VISP_SetFontChar(UINT *pFontChar, BYTE bCharNum, BYTE bCharCnt);
Int32 ENX_VISP_SetNPUObjects(Int32 nType, Int32 nStart);
Int32 ENX_VISP_SendNPUObjects(Int32 nCh, NPU_RESULT_S *pNPUResult);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_VISP_H__

