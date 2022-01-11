
/* 
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_CODEC_H__
#define __ENX_CODEC_H__

#include "enx_define.h"
#include "enx_types.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */


#define VENC_SUCCESS 		(0)
#define VENC_OK		 		(0)
#define VENC_FAILED 		(100)
#define VENC_NOTSUPPORTED	(101)
#define VENC_ERROR_UNUSED 	(200)
#define VENC_ERROR_CHNUM 	(201)
#define VENC_ERROR_RUNNING 	(202)
#define VENC_ERROR_CHMAX 	(203)


//VENC LINK//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define VENC_CH_MAX         (4)//(5) 5Chnnrl mode
#define VENC_CURRENT_MAX_ROI        (20)

/**
    \brief Profile Identifier for Encoder
*/
typedef enum
{
	VENC_CH_BASELINE_PROFILE = 0,
	/**< BaseLine Profile	*/

	VENC_CH_MAIN_PROFILE	  = 1,
	/**< Main Profile		*/

	VENC_CH_HIGH_PROFILE	  = 2,
	/**< High Profile		*/

	VENC_CH_MAIN_HEVC_PROFILE	 = 3,
	/**< MAIN HEVC Profile	  */

	VENC_CH_MAIN_HEVC_STILL_PICTURE = 5,
	/**< MAIN HEVC STILL_PICTURE Profile	*/

	VENC_CH_JPEG = 6,
	/**< JPEG Profile	 */

	VENC_CH_JPEG_STILL_PICTURE = 7,
	/**< JPEG STILL PICTUREProfile	  */

} VENC_CODEC_TYPE_E;


/**
	\brief Encoded frame type
*/
typedef enum 
{
	FRAME_TYPE_I		= 0,	/**< I picture */
	FRAME_TYPE_P		= 1,	/**< P picture */
	FRAME_TYPE_VPS		= 32,
	FRAME_TYPE_SPS		= 33,
	FRAME_TYPE_PPS		= 34,
	FRAME_TYPE_SEI		= 39,
	FRAME_TYPE_VCL		= 40,
	FRAME_TYPE_AUD		= 41,
	FRAME_TYPE_EOS		= 42,
	FRAME_TYPE_EOB		= 43,
	FRAME_TYPE_VUI		= 44,
	FRAME_TYPE_JPEG		= 60,	/**< JPEG */
	FRAME_TYPE_MAX				/**< No Meaning */

} VENC_FRAME_TYPE_E;


/**
    \brief Encoder Rate-control type
*/
typedef enum
{
	VENC_RATE_CTRL_VBR = 0,
	/**< Variable Bitrate: For Local Storage*/

	VENC_RATE_CTRL_CBR = 1,
	/**< Constant Bitrate, Low Delay: For Video Conferencing*/

	VENC_RATE_CTRL_CVBR = 2,
	/**< Constant&Variable Bitrate, Adaptive Quality: For Network Video*/

} VENC_RATE_CTRL_TYPE_E;


/**
    \brief Encode Param Set/Get ID
*/
typedef enum
{
	VENC_ALL = 0,

	VENC_BITRATE,
	/**< Get/Set Bitrate in bits/second */

	VENC_FRAMERATE,
	/**< Get/Set frame-rate */

	VENC_IDR,
	/**< Get/Set Intra-frame interval */

	VENC_RATE_CTRL,
	/**< Get/Set Rate Control Algo */

	VENC_QP,
	/**< Get/Set MIN/MAX/INIT QP */

	VENC_ROI,
	/**< Get/Set ROI parameters */

	VENC_SMARTBG,
	/**< Start Background Detection Algorithm on/off */

	VENC_HEADERINC,
	/**< Frame/Stream mode on/off */

	VENC_DNR3D,
	/**< 3D DNR on/off */

	VENC_DBLK,
	/**< Deblocking filter on/off for avc/hevc */

	VENC_AVC_TRANS,
	/**< Transform 8x8 on/off, AVC ONLY */

	VENC_AVC_ENTROPY,
	/**< Entropy encoding 'CABAC' on/off, AVC ONLY */

	VENC_ENABLE_IDR,
	/**< Generate IDR frame on/off */

	VENC_REQUEST_IDR,
	/**< Request IDR frame */

} VENC_DYNAMIC_PARAM_E;

/**
    \brief Encode Channel Specific Static Parameters
*/
typedef struct
{
    Int32 chId;

    Int32 frameRate;
    /**< Frame-rate */

} VENC_CH_FRAMERATE_PARAM_S;

typedef struct
{
    Int32 chId;

    Int32 targetBitRate;
    /**< required bitrate in bits/second */

} VENC_CH_BITRATE_PARAM_S;

typedef struct
{
    Int32 chId;

    Int32 intraFrameInterval;
    /**< I to P ratio or I frame interval */

} VENC_CH_IDR_PARAM_S;

typedef struct
{
    Int32 chId;

    VENC_RATE_CTRL_TYPE_E rcType;
    /**< Rate-control type */
	
    Int32 vbrDuration;
    /**< VBRDuration for CVBR */

    Int32 vbrSensitivity;
    /**< VBRSensitivity for CVBR */

} VENC_CH_RATECTRL_PARAM_S;

typedef struct
{
    Int32 chId;

    Int32 qpMin;
    /**< QP Min value */

    Int32 qpMax;
    /**< QP Max value */

    Int32 qpInit;
    /**< QP Init value */

} VENC_CH_QP_PARAM_S;

typedef struct
{
	Int32 roiEnable;
	
    Int32 roiNumOfRegion;
    /**< Number of ROI's */
	
    Int32 roiAverageQp;

    Int32 roiStartX[VENC_CURRENT_MAX_ROI];
    /**< starting location X coordinate of this region */

    Int32 roiStartY[VENC_CURRENT_MAX_ROI];
    /**< starting location Y coordinate of this region */

    Int32 roiWidth[VENC_CURRENT_MAX_ROI];
    /**< Width of this ROI */

    Int32 roiHeight[VENC_CURRENT_MAX_ROI];
    /**< Height of this ROI */

    Int32 roiType[VENC_CURRENT_MAX_ROI];
    /**< ROI type */

    Int32 roiQuality[VENC_CURRENT_MAX_ROI];
    /**< ROI Priority/Color */

} VENC_CH_ROI_PARAM_S;

typedef struct
{
    Int32 chId;

    VENC_CH_ROI_PARAM_S roiParams;
    /**< ROI Privacy masking parameters */

} VENC_ROI_PARAMS_S;

typedef struct
{
    Int32 chId;

	Int32 smartBackground;
    /**< smartBackground parameters */

} VENC_CH_SMARTBG_PARAM_S;

typedef struct
{
    Int32 chId;

	Int32 headerInclude;

} VENC_CH_HEADERINC_PARAM_S;

typedef struct
{
	Int32 nrYEnable;
	Int32 nrCbEnable;
	Int32 nrCrEnable;
	Int32 nrNoiseEstEnable;
	Int32 nrNoiseSigmaY;
	Int32 nrNoiseSigmaCb;
	Int32 nrNoiseSigmaCr;

	Int32 nrIntraWeightY;
	Int32 nrIntraWeightCb;
	Int32 nrIntraWeightCr;
	Int32 nrInterWeightY;
	Int32 nrInterWeightCb;
	Int32 nrInterWeightCr;

} VENC_NOISE_REDUCTION_S;

typedef struct
{
    Int32 chId;

	Int32 Dnr3DOn;

	Int32 EnableCustomNr;
	/**< NOISE_REDUCTION custom on/off !! Internal reserved param */

	VENC_NOISE_REDUCTION_S NoiseRd;
	/**< NOISE_REDUCTION custom settings !! Internal reserved param */

} VENC_CH_3DNR_PARAM_S;

typedef struct
{
	Int32 chId;

	Int32 Dblk;

} VENC_CH_DBLK_PARAM_S;

typedef struct
{
	Int32 chId;

	Int32 AvcTrans;

} VENC_CH_AVC_TRANS_PARAM_S;

typedef struct
{
	Int32 chId;

	Int32 AvcEntropy;

} VENC_CH_AVC_ENTROPY_PARAM_S;

typedef struct
{
	Int32 chId;

	Int32 EnableIdr;

} VENC_CH_ENABLE_IDR_PARAM_S;

typedef struct
{
	Int32 chId;

	Int32 RequestIdr;

} VENC_CH_REQUEST_IDR_PARAM_S;


/**
    \brief Encode Channel Specific Crop Parameters
*/
typedef struct
{
    Int32 CropStartX;
    /**< starting location X coordinate of this region */

    Int32 CropStartY;
    /**< starting location Y coordinate of this region */

    Int32 CropWidth;
    /**< Width of this ROI */

    Int32 CropHeight;
    /**< Height of this ROI */

    Int32 CropType;
    /**< ROI type */
	
    Int32 CropEn;
    /**< ROI Enable */

} VENC_CH_CROP_PARAM_S;

typedef struct
{
    Int32 chId;

	VENC_CH_CROP_PARAM_S cropParams;
    /**< Crop parameters */

} VENC_CROP_PARAMS_S;

typedef struct
{
    UInt32 chId;
    /**< VENC channel number */

} VENC_LINK_CHINFO_S;

typedef struct
{
	Int32 chId;
	/**< channel Id */

	Int32 videoInputId;
	/**< Input Video Id */

	Int32 videoWidth;
	/**< Read only: Input Video Width */

	Int32 videoHeight;
	/**< Read only: Input Video Height */

	Int32 maxBitRate; //Kbps
	/**< Maximum Bit Rate for Encoder*/

	VENC_RATE_CTRL_TYPE_E rcType;
	/**< Rate-control type */

	Int32 frameRate;
	/**< Frame-rate */

	Int32 targetBitRate; //Kbps
	/**< required bitrate in bits/second */

	Int32 intraFrameInterval;
	/**< I to P ratio or I frame interval */

	Int32 inputFrameRate;
	/**< Frame rate of the incoming content */

	Int32 rcAlg;
	/**< Rate-control Algorithm type */

	Int32 qpMin;
	/**< QP Min value */

	Int32 qpMax;
	/**< QP Max value */

	Int32 qpInit;
	/**< QP Init value */

	Int32 vbrDuration;
	/**< VBRDuration for CVBR */

	Int32 vbrSensitivity;
	/**< VBRSensitivity for CVBR */

	VENC_CH_ROI_PARAM_S roiParams;
	/**< ROI Privacy masking parameters */

	VENC_CH_CROP_PARAM_S cropParams;
	/**< Crop parameters */

	VENC_CODEC_TYPE_E hProfile;
	/**< Codec profile */

	Int32 snapshotMode;

	Int32 smartBackground;

	Int32 headerInclude;

	Int32 Dnr3DOn;

	Int32 rotAngle;

	Int32 mirDir;

	Int32 bitStreamBase; //read only

	Int32 bitStreamEnd; //read only

} VENC_CH_PARAM_S;

typedef struct {

    UInt32 numChannel;
    /**< Number of encoder channels */

    VENC_CH_PARAM_S encChannelParams[VENC_CH_MAX];
    /**< Channel Specific Parameters */

} VENC_PARAMS_S;

typedef struct
{
	Int32 chId;

	Int32 frameRate;
	/**< Frame-rate */

	Int32 targetBitRate;
	/**< required bitrate in k-bits/second */

	Int32 intraFrameInterval;
	/**< I to P ratio or I frame interval */

	VENC_RATE_CTRL_TYPE_E rcType;
	/**< Rate-control type */

	Int32 qpMin;
	/**< QP Min value */

	Int32 qpMax;
	/**< QP Max value */

	Int32 qpInit;
	/**< QP Init value */

	Int32 vbrDuration;
	/**< VBRDuration for CVBR */

	Int32 vbrSensitivity;
	/**< VBRSensitivity for CVBR */

	VENC_CH_ROI_PARAM_S roiParams;
	/**< ROI Privacy masking parameters */

	Int32 smartBackground;
	/**< Start Background Detection Algorithm on/off */

	Int32 headerInclude;
	/**< Frame/Stream mode on/off */

	Int32 Dnr3DOn;	
	/**< 3D DNR on/off */

	Int32 Dblk;
	/**< Deblocking filter on/off for avc/hevc */

	Int32 AvcTrans;
	/**< Transform 8x8 on/off for Avc only */

	Int32 AvcEntropy;
	/**< Entropy encoding 'CABAC' on/off for Avc only */

	Int32 EnableIdr;
	/**< Generate IDR frame on/off */

	Int32 RequestIdr;
	/**< Request IDR frame */

	Int32 EnableCustomNr;
	/**< NOISE_REDUCTION custom on/off */

	VENC_NOISE_REDUCTION_S NoiseRd;
	/**< NOISE_REDUCTION custom settings */

	int change_options;
	/**< Bit Command of encoder change params !! Internal reserved param */

} VENC_CH_DYNAMIC_PARAM_S;


//VENC LINK=====================================================================================================




//BITS LINK/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    \brief Video Bitstream Buffer Information

    This structure is used to get information of encoded frames from VENC
    OR to pass information of encoded frames to VDEC
*/

typedef struct
{
	UInt32 sei_offset;
	UInt32 sei_size;
	UInt32 vps_offset;
	UInt32 vps_size;
	UInt32 sps_offset;
	UInt32 sps_size;
	UInt32 pps_offset;
	UInt32 pps_size;
	UInt32 aud_offset;
	UInt32 aud_size;
} NAL_INFO_S;

typedef struct
{
	Int32 videoInputId;
	Int32 videoInputWidth;
	Int32 videoInputHeight;
	ULONG videoInputPTS;
	ULONG videoInputFrameNum;

	/**< Read only */
	UInt32 reservedData1;
	UInt32 reservedData2;
	Int32 reservedData3;

} BITS_STREAMHDR_S;

typedef struct
{
	ULONG lastPicNum;	// last encoded frame Pic number
	ULONG currentPicNum;	// current read frame Pic number
	UInt32 leftFrameBytes;	// remain Frame bytes
	Int32 framePicCnt;	// remain Frame Pic count

} BITS_STREAMSTATUS_S;

typedef struct {
	BITS_STREAMHDR_S	stStreamHdr;
	NAL_INFO_S stNalInfo;

	Int32 chId;
	Int32 result;
	int	errorReason;
	int	warnInfo;
	VENC_FRAME_TYPE_E	picType;
	UInt32	frameSize;
	UInt32	framePhysAddr;
	BYTE	*pframeVirtAddr;
	Int32	frameOffset;
	ULONG	framePTS;
	ULONG	encPicNum;
	Int32	frameFps;
	Int32	encodeWidth;
	Int32	encodeHeight;

	UInt32	instIndex;
	UInt32	encPicPoc;
	UInt32	encSrcIdx;
	int	numOfSlices;
	int	reconFrameIndex;
	int	numOfIntra;
	int	numOfMerge;
	int	numOfSkipBlock;
	int	avgCtuQp;
	int	encPicByte;
	int	encGopPicIdx;

	/**< Read only */
	UInt32 reservedData1;
	UInt32 reservedData2;

} BITS_FRAMEBUF_S;

typedef struct
{
    Int32 framePicCnt;
    BITS_FRAMEBUF_S *pBitsFrameBuf;

} BITS_STREAMBUF_S;

typedef struct
{
    BYTE *pVirtAddr;
    ULONG physAddr;
    UInt32 streamSize;

} BITS_STREAMINFO_S;

//BITS LINK=====================================================================================================



//FRAM LINK/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    \brief Video Bitstream Buffer Information

    This structure is used to get information of encoded frames from VENC
    OR to pass information of encoded frames to VDEC
*/
typedef struct {
	
	Int32 			result;  
	/**< The return value */
	
    UInt32              chnId;
    /**< input channel ID */
		
    VENC_FRAME_TYPE_E 	picType;            
	/** PicType */

    UInt32 				pts;
	/**< The PTS(Presentation Timestamp) */
	
	int 				videoPicCnt;			
	/**< The picture number */
	
    UInt32              videoBuffer;
    /**< Physical Address */
	
    UInt32 				videoSize;   
	/**< The byte size of video */

} FRAME_BUF_S;


typedef struct
{
    Int32 videoInputId;
    /**< Video Id */
	
    Int32 videoWidth;
    /**< Read only: Encoded Video Width */

    Int32 videoHeight;
    /**< Read only: Encoded Video Height */

	Int32 videoOutputId; //0,1,2,3,4,5,6,7 channel
	/**< Bitstream Id */

	Int32 videoMaxOutputSize; //byte
	/**< Bitstream Size */

	Int32 videoOutputType; //0 NALUNIT
	/**< Bitstream Type */

    FRAME_BUF_S    videoBuf;
    /**< Bistream information */

} FRAME_PARAMS_S;

//FRAM LINK======================================================================================================


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_CODEC_H__
