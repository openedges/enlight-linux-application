
/*
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_COMMON_H__
#define __ENX_COMMON_H__

#include "enx_define.h"
#include "enx_types.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define ENX_DEV_NAME	"/dev/enx_msg"

// LINK_REG definition
#define LINK_REG_REQ_NONE	0
#define LINK_REG_REQ_StoM	1	// slave to master
#define LINK_REG_REQ_MtoS	2	// master to slave (Linux : master, ISP : slave)
#define LINK_REG_REQ_ALL	3

// LINK REG Command
#if 0
#define LINK_REG_CMD_NONE	0
#define LINK_REG_CMD_READY	1
#define LINK_REG_CMD_MSG	2
#define LINK_REG_CMD_DBG	3
#define LINK_REG_CMD_STRM_CH1	4
#define LINK_REG_CMD_STRM_CH2	5
#define LINK_REG_CMD_STRM_CH3	6
#define LINK_REG_CMD_STRM_CH4	7

#define LINK_REG_CMD_VCAP_CH1	8
#define LINK_REG_CMD_VCAP_CH2	9
#define LINK_REG_CMD_VCAP_CH3	10
#define LINK_REG_CMD_VCAP_CH4	11
#define LINK_REG_CMD_NPU_CH1	12		// For UYV capture
#define LINK_REG_CMD_DISP_VLOCK	16		// For Display vlock
#else
#define LINK_REG_CMD_NONE			0
#define LINK_REG_CMD_READY			0x00000001
#define LINK_REG_CMD_MSG			0x00000002
#define LINK_REG_CMD_DBG			0x00000004

#define LINK_REG_CMD_STRM_CH1		0x00000010
#define LINK_REG_CMD_STRM_CH2		0x00000020
#define LINK_REG_CMD_STRM_CH3		0x00000040
#define LINK_REG_CMD_STRM_CH4		0x00000080
#define LINK_REG_CMD_STRM_CH5		0x00000100

#define LINK_REG_CMD_VCAP_CH1		0x00000200
#define LINK_REG_CMD_VCAP_CH2		0x00000400
#define LINK_REG_CMD_VCAP_CH3		0x00000800
#define LINK_REG_CMD_VCAP_CH4		0x00001000
#define LINK_REG_CMD_NPU_CH1		0x00002000		// For UYV capture
#define LINK_REG_CMD_DISP_VLOCK		0x00004000		// For Display vlock
#endif


#define LINK_REG_DATA_NULL	0
#define LINK_REG_DATA_ADDR	1	// none null

#define LINK_REG_RSP_OK		0	// response ok
#define LINK_REG_RSP_ERR	1	// response error : none zero

// LINK_HEADER definition
#define LINK_HEAD_SRC_ID_NONE	0
#define LINK_HEAD_SRC_ID_SLV	1
#define LINK_HEAD_SRC_ID_MST	2

#define LINK_HEAD_DST_ID_NONE	0
#define LINK_HEAD_DST_ID_SLV	1
#define LINK_HEAD_DST_ID_MST	2

// Link header id definition
#define MAX_LINK_HEAD_ID		30
#define LINK_HEAD_DATA_ID_HDR	0
#define LINK_HEAD_DATA_ID_MSG	1
#define LINK_HEAD_DATA_ID_SYS	2
#define LINK_HEAD_DATA_ID_CAP	3
#define LINK_HEAD_DATA_ID_ENC	4
#define LINK_HEAD_DATA_ID_DEC	5
#define LINK_HEAD_DATA_ID_UYV	6
#define LINK_HEAD_DATA_ID_NPU	7
#define LINK_HEAD_DATA_ID_FONT_TEXT		8	// 256 bytes
#define LINK_HEAD_DATA_ID_FONT_COLOR	9	// 0 bytes


#define LINK_HEAD_DATA_CMD_NONE		0
#define LINK_HEAD_DATA_CMD_NEW		1

//VSYS CMD LIST/////////////////////////////////////////[[
#define LINK_SYS_CMD_GET_PARAMS					(0x100)
#define LINK_SYS_CMD_SET_PARAMS					(0x101)
#define LINK_SYS_CMD_RST_PARAMS					(0x102)

#define LINK_SYS_CMD_GET_DYNAMIC_PARAMS			(0x120)
#define LINK_SYS_CMD_SET_DYNAMIC_PARAMS			(0x121)

#define LINK_SYS_CMD_INIT						(0x130)
#define LINK_SYS_CMD_EXIT						(0x131)

#define LINK_SYS_CMD_VER						(0x140)
////////////////////////////////////////////////////////]]

//VISP CMD LIST/////////////////////////////////////////[[
#define LINK_ISP_CMD_GET_PARAMS					(0x200)
#define LINK_ISP_CMD_SET_PARAMS					(0x201)
#define LINK_ISP_CMD_RST_PARAMS					(0x202)
#define LINK_ISP_CMD_SINGLE_PARAM				(0x203)
#define LINK_ISP_CMD_PARAM_ATTR					(0x204)

#define LINK_ISP_CMD_GET_PVC					(0x210)
#define LINK_ISP_CMD_SET_PVC					(0x211)
#define LINK_ISP_CMD_GET_IMD					(0x212)
#define LINK_ISP_CMD_SET_IMD					(0x213)
#define LINK_ISP_CMD_GET_YC						(0x214)
#define LINK_ISP_CMD_SET_YC						(0x215)

#define LINK_ISP_CMD_SET_FONT_TEXT				(0x216)
#define LINK_ISP_CMD_SET_FONT_COLOR				(0x217)
#define LINK_ISP_CMD_SET_FONT_ATTR				(0x218)
#define LINK_ISP_CMD_SET_FONT_CHAR				(0x219)


#define LINK_ISP_CMD_INIT						(0x290)
#define LINK_ISP_CMD_EXIT						(0x291)
////////////////////////////////////////////////////////]]

//VENC CMD LIST/////////////////////////////////////////[[
#define LINK_ENC_CMD_GET_PARAMS					(0x300)
#define LINK_ENC_CMD_SET_PARAMS					(0x301)
#define LINK_ENC_CMD_RST_PARAMS					(0x302)

#define LINK_ENC_CMD_GET_PARAMS					(0x300)
#define LINK_ENC_CMD_SET_PARAMS					(0x301)
#define LINK_ENC_CMD_RST_PARAMS					(0x302)

#define LINK_ENC_CMD_CH_CREATE					(0x310)
#define LINK_ENC_CMD_CH_DESTROY					(0x311)
#define LINK_ENC_CMD_CH_START					(0x312)
#define LINK_ENC_CMD_CH_STOP					(0x313)
#define LINK_ENC_CMD_CH_GET_STATUS				(0x314)
#define LINK_ENC_CMD_CH_GET_PICNUM_STATUS		(0x315)
#define LINK_ENC_CMD_CH_GET_BITSTREAM			(0x316)
#define LINK_ENC_CMD_CH_RELEASE_BITSTREAM		(0x317)
#define LINK_ENC_CMD_CH_GET_PARAMS				(0x318)
#define LINK_ENC_CMD_CH_SET_PARAMS				(0x319)
#define LINK_ENC_CMD_CH_RST_PARAMS				(0x320)
#define LINK_ENC_CMD_CH_GET_DYNAMIC_PARAMS		(0x321)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_PARAMS		(0x322)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_FPS			(0x323)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_BPS			(0x324)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_IDR			(0x325)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_ROI			(0x326)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_SBG			(0x327)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_HDR			(0x328)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_DNR			(0x329)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_QP			(0x330)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_CVBR		(0x331)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_DBLK		(0x332)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_TRANS		(0x333)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_ENTRP		(0x334)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_ENIDR		(0x335)
#define LINK_ENC_CMD_CH_SET_DYNAMIC_RQIDR		(0x336)

#define LINK_ENC_CMD_INIT						(0x380)
#define LINK_ENC_CMD_EXIT						(0x381)

#define LINK_ENC_CMD_DEBUG						(0x390)
////////////////////////////////////////////////////////]]

//VDEC CMD LIST/////////////////////////////////////////[[
#define LINK_DEC_CMD_GET_PARAMS					(0x400)
#define LINK_DEC_CMD_SET_PARAMS					(0x401)
#define LINK_DEC_CMD_RST_PARAMS					(0x402)

#define LINK_DEC_CMD_CH_CREATE       			(0x410)
#define LINK_DEC_CMD_CH_DESTROY      			(0x411)
#define LINK_DEC_CMD_CH_START       			(0x412)
#define LINK_DEC_CMD_CH_STOP       				(0x413)
#define LINK_DEC_CMD_CH_GET_STATUS      		(0x414)
#define LINK_DEC_CMD_CH_GET_BITSTREAM      		(0x415)
#define LINK_DEC_CMD_CH_RELEASE_BITSTREAM   	(0x416)
#define LINK_DEC_CMD_CH_GET_PARAMS				(0x417)
#define LINK_DEC_CMD_CH_SET_PARAMS				(0x418)
#define LINK_DEC_CMD_CH_RST_PARAMS				(0x419)
#define LINK_DEC_CMD_CH_GET_DYNAMIC_PARAMS		(0x420)
#define LINK_DEC_CMD_CH_SET_DYNAMIC_PARAMS		(0x421)

#define LINK_DEC_CMD_INIT						(0x430)
#define LINK_DEC_CMD_EXIT						(0x431)
////////////////////////////////////////////////////////]]

//VCAP CMD LIST/////////////////////////////////////////[[
#define LINK_VCAP_CMD_INIT						(0x500)
#define LINK_VCAP_CMD_EXIT						(0x501)

#define LINK_VCAP_CMD_START						(0x510)
#define LINK_VCAP_CMD_STOP						(0x511)

#define LINK_VCAP_CMD_GET_ATTR					(0x512)
#define LINK_VCAP_CMD_SET_ATTR					(0x513)
////////////////////////////////////////////////////////]]

//VCDA CMD LIST/////////////////////////////////////////[[
#define LINK_VDA_CMD_INIT						(0x600)
#define LINK_VDA_CMD_EXIT						(0x601)

#define LINK_VDA_CMD_START						(0x610)
#define LINK_VDA_CMD_STOP						(0x611)

#define LINK_VDA_CMD_GET						(0x620)
#define LINK_VDA_CMD_RELEASE					(0x621)
////////////////////////////////////////////////////////]]

//NPU VISP CMD LIST/////////////////////////////////////////[[
#define LINK_NPU_CMD_RESULT_SET					(0x700)
#define LINK_NPU_CMD_RESULT_OBJ					(0x701) // Not Response !!!
////////////////////////////////////////////////////////]]

//UYV Capture CMD LIST/////////////////////////////////////////[[
#define LINK_UYV_CAPTURE_CMD_INIT				(0x800)
#define LINK_UYV_CAPTURE_CMD_EXIT				(0x801)

#define LINK_UYV_CAPTURE_CMD_START				(0x810)
#define LINK_UYV_CAPTURE_CMD_STOP				(0x811)

#define LINK_UYV_CAPTURE_CMD_GET_ATTR			(0x812)
#define LINK_UYV_CAPTURE_CMD_SET_ATTR			(0x813)
////////////////////////////////////////////////////////]]

//NPU VISP CMD LIST/////////////////////////////////////////[[
#define LINK_DISP_CMD_INIT						(0x900)
#define LINK_DISP_CMD_EXIT						(0x901)
#define LINK_DISP_CMD_GET_TRANSP				(0x902)
#define LINK_DISP_CMD_SET_TRANSP				(0x903)
#define LINK_DISP_CMD_LAYER1					(0x904)
#define LINK_DISP_CMD_LAYER2					(0x905)
////////////////////////////////////////////////////////]]



#define LINK_HEAD_RST_OK	0
#define LINK_HEAD_RST_ERR	1

#define LINK_HEAD_OPT_NONE	0
#define LINK_HEAD_OPT_ADDR	1

typedef struct LNK_HDR {
	ULONG srcid;
	ULONG dstid;
	ULONG linkid;
	ULONG linkcmd;
	ULONG result;
	ULONG ldsize;
	ULONG waitack;
} LNK_HDR_S;

typedef struct LNK_DATA {
	ULONG base;
	ULONG size;
	ULONG end;
} LNK_DATA_S;

typedef struct LNK_MSG {
	LNK_HDR_S lh;
	LNK_DATA_S ld[MAX_LINK_HEAD_ID];
} LNK_MSG_S;

typedef struct LNK_OBJ {
	LNK_MSG_S lm;
	ULONG base;
	ULONG size;
	ULONG link_on;
	ULONG link_cnt;
} LNK_OBJ_S;

typedef struct LNK_REG {
	UINT cmd;		// (LINK_REG_REG << 16) | LINK_REG_CMD
	UINT addr;		// Data Address
	UINT rsp;	// Response
} LNK_REG_S;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_COMMON_H__

