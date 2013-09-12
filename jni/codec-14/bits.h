//=============================================================================
//  CIDANA CONFIDENTIAL INFORMATION
//
//  THIS SOURCE CODE IS PROPRIETARY INFORMATION BELONGING TO CIDANA, INC.
//  ANY USE INCLUDING BUT NOT LIMITED TO COPYING OF CODE, CONCEPTS, AND/OR
//  ALGORITHMS IS PROHIBITED EXCEPT WITH EXPRESS WRITTEN PERMISSION BY THE 
//  COMPANY.
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 2011  Cidana, Inc.  All Rights Reserved.
//
//-----------------------------------------------------------------------------
#ifndef _CI_BITS_H_
#define _CI_BITS_H_

#if defined(UNDER_CE) || defined(WIN32)
#define CI_INLINE __forceinline
#pragma warning(disable:4311 4312)
#elif defined(__GNUC__)
#define CI_INLINE static __inline
#else
#error Unknown compiler
#endif

#ifdef __cplusplus
extern "C" {
#endif

    CI_INLINE CI_VOID StartBits(CI_BITS *pBITS, CI_U8 *pu8Buf, CI_U32 u32BufLen)
    {
        pBITS->pu8Start = pu8Buf;
        pBITS->u32BufLen = u32BufLen;
        pBITS->pu8Curr = (CI_U8 *)(((CI_U32)pu8Buf)&0xFFFFFFFC);
        pBITS->s32Bitn = (CI_S32)((((CI_U32)pu8Buf)&3)<<3);
        pBITS->u32CodeNext = (((CI_U32) *(pBITS->pu8Curr++))<<24);
        pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<16);
        pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<8);
        pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<0);
        pBITS->u32Code = pBITS->u32CodeNext<<pBITS->s32Bitn;
        if(pBITS->s32Bitn)
        {
            pBITS->s32Bitn = 32-pBITS->s32Bitn;
            pBITS->u32CodeNext = (((CI_U32) *(pBITS->pu8Curr++))<<24);
            pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<16);
            pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<8);
            pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<0);
            pBITS->u32Code |= pBITS->u32CodeNext>>pBITS->s32Bitn;
        }
    }

    CI_INLINE CI_VOID SyncBits(CI_BITS *pBITS)
    {
        if(pBITS->s32Bitn<0)
        {
            pBITS->u32Code = pBITS->u32CodeNext<<(-pBITS->s32Bitn);
            pBITS->u32CodeNext = (((CI_U32) *(pBITS->pu8Curr++))<<24);
            pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<16);
            pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<8);
            pBITS->u32CodeNext += (((CI_U32) *(pBITS->pu8Curr++))<<0);
            pBITS->s32Bitn += 32;
        }
        pBITS->u32Code |= pBITS->u32CodeNext>>pBITS->s32Bitn;
    }

    CI_INLINE CI_VOID StopBits(CI_BITS *pBITS, CI_U8 **ppu8Buf)
    {
        SyncBits(pBITS);
        *ppu8Buf = pBITS->pu8Curr-4-(pBITS->s32Bitn>>3);
    }

    CI_INLINE CI_VOID Preview32Bits(CI_BITS *pBITS)
    {
        SyncBits(pBITS);
    }

    CI_INLINE CI_VOID AdvanceBits(CI_BITS *pBITS, CI_U32 u32BitsNum)
    {
        pBITS->u32Code <<= u32BitsNum;
        pBITS->s32Bitn -= u32BitsNum;
    }

    CI_INLINE CI_VOID ShowBits(CI_BITS *pBITS, CI_U32 u32BitsNum, CI_U32 *pu32Val)
    {
        *pu32Val = pBITS->u32Code >> (32-u32BitsNum);
    }

    CI_INLINE CI_VOID GetBits(CI_BITS *pBITS, CI_U32 u32BitsNum, CI_U32 *pu32Val)
    {
        ShowBits(pBITS, u32BitsNum, pu32Val);
        AdvanceBits(pBITS, u32BitsNum);
    }

    CI_INLINE CI_VOID ByteAlign(CI_BITS *pBITS)
    {
        SyncBits(pBITS);
        AdvanceBits(pBITS, pBITS->s32Bitn&7);
    }

    CI_INLINE CI_BOOL CheckRange(CI_BITS *pBITS)
    {
        SyncBits(pBITS);
        return (pBITS->pu8Curr - 4 - (pBITS->s32Bitn>>3)) >= (pBITS->pu8Start + pBITS->u32BufLen); 
    }

#ifdef __cplusplus
}
#endif

#endif
