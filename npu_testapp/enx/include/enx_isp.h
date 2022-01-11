
/*
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_ISP_H__
#define __ENX_ISP_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#pragma pack(push, 2)

#pragma pack(pop)

typedef struct {
	BYTE bAction;
	BYTE bPosX;
	BYTE bPosY;
	BYTE bSizX;
	BYTE bSizY;
} UP_MENU_AREA_BOX_S;

typedef struct {
    BYTE In;
    BYTE Out;
    BYTE Font;
    BYTE PageSize;
    WORD Width;
    WORD Height;
} UP_MENU_YC_S;    // 8 byte

typedef struct {
	char ID[4];
	WORD VAL;
} VISP_PARAM_UNIT_S;

typedef struct {
	WORD Def;
	WORD Min;
	WORD Max;
} VISP_PARAM_ATTR_S;

typedef struct {
	BYTE SetByAPI; // enable, control, EnableAPI, CtlFontAttr, ApiEnable, APIEnable
	BYTE FontMute;
	BYTE BoadMode;

	UINT FontCntX;
	UINT FontCntY;
	UINT BlankV;
	UINT BlankH;
	UINT SizeV;
	UINT SizeH;
	UINT OffsetV;
	UINT OffsetH;

	UINT ColorData0;
	UINT ColorData1;
	UINT ColorData2;
	UINT ColorData3;
} UP_FONT_ATTR_S; // use for API

/**
    \brief ISP Param Set/Get ID
*/

typedef enum
{
	VISP_FONT_SetByAPI=0,
	VISP_FONT_FontMute,
	VISP_FONT_BoadMode,
	VISP_FONT_FontCntX,
	VISP_FONT_FontCntY,
	VISP_FONT_BlankV,
	VISP_FONT_BlankH,
	VISP_FONT_SizeV,
	VISP_FONT_SizeH,
	VISP_FONT_OffsetV,
	VISP_FONT_OffsetH,
	VISP_FONT_ColorData0,
	VISP_FONT_ColorData1,
	VISP_FONT_ColorData2,
	VISP_FONT_ColorData3,
	VISP_FONT_All,
} VISP_FONT_ATTR_E;

typedef enum
{
	VISP_BOX_Action=0,
	VISP_BOX_PosX,
	VISP_BOX_PosY,
	VISP_BOX_SizX,
	VISP_BOX_SizY,
	VISP_BOX_All,
} VISP_AREA_BOX_E;

typedef enum
{
	VISP_YC_In=0,
	VISP_YC_Out,
	VISP_YC_Font,
	VISP_YC_PageSize,
	VISP_YC_Width,
	VISP_YC_Height,
	VISP_YC_All,
} VISP_YC_E;

/**
    \brief ISP Dynamic Parameters
*/
typedef struct
{
	Int32 outfps;
} VISP_OUTFPS_PARAM_S;

/**
    \brief ISP Dynamic Parameters
*/
typedef struct
{
	Int32 sysfrq;
} VISP_SYSFRQ_PARAM_S;

/**
    \brief ISP Dynamic Parameters
*/
typedef struct
{
	Int32 wdrmode;
} VISP_WDR_PARAM_S;

/**
    \brief ISP Dynamic Parameters
*/
typedef struct
{
	Int32 cvbsmode;
} VISP_CVBS_PARAM_S;

/**
    \brief ISP Dynamic Parameters
*/
typedef struct
{
	Int32 dzoommode;
} VISP_DZOOM_PARAM_S;


typedef struct {
	Int32 chId;
	//Int32 ycenable;
	UP_MENU_YC_S ycchannel;
}  VISP_YC_PARAM_S;    // 8 byte

typedef struct
{
    SHORT x_min;
    SHORT x_max;
    SHORT y_min;
    SHORT y_max;
    SHORT npu_class;
    SHORT npu_score;

} NPU_OBJ_S;

typedef struct
{
    int cnt;
    NPU_OBJ_S obj[256];

} NPU_RESULT_S;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_ISP_H__

