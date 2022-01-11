
/* 
 * Copyright (C) 2019 Eyenix Corporation
 * dev-team2, Eyenix <support6@eyenix.com>
 */

#ifndef __ENX_ACAP_H__
#define __ENX_ACAP_H__

#include "enx_common.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct __ENX_AUD_REG_INFO {
	int i2s_mode;
	int txmode;		
	int txcodec;
	int txdw;
	int rxbyte;		
    int rddw;
    int rdlen;
    int txlr;

    int rxmode;		
	int rxcodec;
	int rxdw;
	int wrbyte;		
    int wrdw;
    int wrlen;
} ENX_AUD_REG_INFO_S;

Int32 ENX_ACAP_ENCODE_INIT(ENX_AUD_REG_INFO_S *arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// __ENX_ACAP_H__
