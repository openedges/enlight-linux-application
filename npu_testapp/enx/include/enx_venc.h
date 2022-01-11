
/* 
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_VENC_H__
#define __ENX_VENC_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "enx_common.h"
#include "enx_codec.h"

//The two functions operate in pairs.
Int32 ENX_VENC_Init(VENC_PARAMS_S *pVencParams);
void ENX_VENC_Exit(void);

//The two functions operate in pairs.
Int32 ENX_VENC_CH_Create(Int32 nCh, VENC_CH_PARAM_S *pVencChParams);
Int32 ENX_VENC_CH_Destroy(Int32 nCh);

//The two functions operate in pairs.
Int32 ENX_VENC_CH_Start(Int32 nCh);
Int32 ENX_VENC_CH_Stop(Int32 nCh);

// Get Streaming data functions
Int32 ENX_VENC_CH_GetFd(Int32 nCh);
Int32 ENX_VENC_CH_GetBitsStreamInfo(Int32 nCh, BITS_STREAMINFO_S *pBitsStreamInfo);
Int32 ENX_VENC_CH_GetStatus(Int32 nCh, BITS_STREAMSTATUS_S *pStreamStatus);
Int32 ENX_VENC_CH_GetBitsStream(Int32 nCh, BITS_STREAMBUF_S *pBitsStreamBuf);
void ENX_VENC_CH_ReleaseBitsStream(Int32 nCh);

// Managed VENC Parameters
Int32 ENX_VENC_GetParams(VENC_PARAMS_S *pVencParams);
Int32 ENX_VENC_SetParams(VENC_PARAMS_S *pVencParams);

Int32 ENX_VENC_CH_GetParam(Int32 nCh, VENC_CH_PARAM_S *pVencChParams);
Int32 ENX_VENC_CH_SetParam(Int32 nCh, VENC_CH_PARAM_S *pVencChParams);

// VENC Dynamic Parameters
Int32 ENX_VENC_CH_GetDynamicParam(Int32 nCh, VENC_CH_DYNAMIC_PARAM_S *pVencChParams);
Int32 ENX_VENC_CH_SetDynamicParam(VENC_CH_DYNAMIC_PARAM_S *pVencChParams);

Int32 ENX_VENC_CH_GetFrameRate(Int32 nCh, VENC_CH_FRAMERATE_PARAM_S *pVencFrameRate);
Int32 ENX_VENC_CH_GetTargetBitRate(Int32 nCh, VENC_CH_BITRATE_PARAM_S *pVencBitRate);
Int32 ENX_VENC_CH_GetIdr(Int32 nCh, VENC_CH_IDR_PARAM_S *pVencIdr);
Int32 ENX_VENC_CH_GetRateCtrl(Int32 nCh, VENC_CH_RATECTRL_PARAM_S *pVencRateCtrl);
Int32 ENX_VENC_CH_GetQPParam(Int32 nCh, VENC_CH_QP_PARAM_S *pVencQP);
Int32 ENX_VENC_CH_GetRoiParam(Int32 nCh, VENC_ROI_PARAMS_S *pVencRoi);
Int32 ENX_VENC_CH_GetSmartBackground(Int32 nCh, VENC_CH_SMARTBG_PARAM_S *pVencSmartBG);
Int32 ENX_VENC_CH_GetHeaderInc(Int32 nCh, VENC_CH_HEADERINC_PARAM_S *pVencHeaderInc);
Int32 ENX_VENC_CH_Get3DNR(Int32 nCh, VENC_CH_3DNR_PARAM_S *pVenc3DNR);
Int32 ENX_VENC_CH_GetDblk(Int32 nCh, VENC_CH_DBLK_PARAM_S *pVencDblk);
Int32 ENX_VENC_CH_GetAvcTrans(Int32 nCh, VENC_CH_AVC_TRANS_PARAM_S *pVencAvcTrans);
Int32 ENX_VENC_CH_GetAvcEntropy(Int32 nCh, VENC_CH_AVC_ENTROPY_PARAM_S *pVencAvcEntropy);
Int32 ENX_VENC_CH_GetEnableIdr(Int32 nCh, VENC_CH_ENABLE_IDR_PARAM_S *pVencEnableIdr);
Int32 ENX_VENC_CH_GetRequestIdr(Int32 nCh, VENC_CH_REQUEST_IDR_PARAM_S *pVencRequestIdr);
Int32 ENX_VENC_CH_GetUserOpt(Int32 nCh, Int32 nUserOpt);

Int32 ENX_VENC_CH_SetFrameRate(VENC_CH_FRAMERATE_PARAM_S *pVencFrameRate);
Int32 ENX_VENC_CH_SetTargetBitRate(VENC_CH_BITRATE_PARAM_S *pVencBitRate);
Int32 ENX_VENC_CH_SetIdr(VENC_CH_IDR_PARAM_S *pVencIdr);
Int32 ENX_VENC_CH_SetRateCtrl(VENC_CH_RATECTRL_PARAM_S *pVencRateCtrl);
Int32 ENX_VENC_CH_SetQPParam(VENC_CH_QP_PARAM_S *pVencQP);
Int32 ENX_VENC_CH_SetRoiParam(VENC_ROI_PARAMS_S *pVencRoi);
Int32 ENX_VENC_CH_SetSmartBackground(VENC_CH_SMARTBG_PARAM_S *pVencSmartBG);
Int32 ENX_VENC_CH_SetHeaderInc(VENC_CH_HEADERINC_PARAM_S *pVencHeaderInc);
Int32 ENX_VENC_CH_Set3DNR(VENC_CH_3DNR_PARAM_S *pVenc3DNR);
Int32 ENX_VENC_CH_SetDblk(VENC_CH_DBLK_PARAM_S *pVencDblk);
Int32 ENX_VENC_CH_SetAvcTrans(VENC_CH_AVC_TRANS_PARAM_S *pVencAvcTrans);
Int32 ENX_VENC_CH_SetAvcEntropy(VENC_CH_AVC_ENTROPY_PARAM_S *pVencAvcEntropy);
Int32 ENX_VENC_CH_SetEnableIdr(VENC_CH_ENABLE_IDR_PARAM_S *pVencEnableIdr);
Int32 ENX_VENC_CH_SetRequestIdr(VENC_CH_REQUEST_IDR_PARAM_S *pVencRequestIdr);
Int32 ENX_VENC_CH_SetUserOpt(Int32 nCh, Int32 nUserOpt);


// Test Function
Int32 ENX_VENC_Debug(Int32 nDebugLevel);
Int32 ENX_VENC_DefaultReset(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_VENC_H__

