#ifndef _CI_SUPPLIER_TYPE_H
#define _CI_SUPPLIER_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum
    {
        PICTURE_I = 0,
        PICTURE_P,
        PICTURE_B,
        PICTURE_SPRITE,
        PICTURE_IDR,
        PICTURE_FORBIDDEN,
    }CI_PIC_TYPE;

    typedef enum
    {
        CI_TOP_FIELD = 0,
        CI_BOTTOM_FIELD,
        CI_FRAME
    }CI_PIC_STRUCT;

    typedef enum
    {
        CI_INPUT_FRAME = 0,     // input a whole frame once.
        CI_INPUT_PICTURE        // input a picture once
    }CI_INPUT_TYPE;

    typedef struct
    {
        CI_S16 type;
        CI_S16 refidc;
        CI_S32 len;
        CI_S32 vtype;
    }CI_BASIC_UNIT;

    typedef enum
    {
        CI_VSTD_H264 = 0,
        CI_VSTD_MPEG4,
        CI_VSTD_MPEG2,
        CI_VSTD_RV89,
        CI_VSTD_WMV,
        CI_VSTD_VP8,
        CI_VSTD_H263,
        CI_VSTD_UNKNOWN,
    }CI_VSTD_TYPE;

#if PLATFORM_WIN32 || PLATFORM_MTK3353
#define PIC_DESC_SPS_MASK (1<<1)
#define PIC_DESC_PPS_MASK (1<<2)
#define PIC_DESC_AUD_MASK (1<<3)
#define PIC_DESC_SEI_MASK (1<<4)
#define PIC_DESC_IDR_MASK (1<<5)
#define PIC_DESC_REF_MASK      (1<<6)
// mpeg2
#define PIC_DESC_MPEG2_SEQ_HDR_MASK (1<<8) 
#define PIC_DESC_MPEG2_GROUP_MASK (1<<9)
#define PIC_DESC_MPEG2_PIC_HDR_MASK (1<<10)
#define PIC_DESC_MPEG2_EXTENSION_MASK (1<<11)
#define PIC_DESC_MPEG2_SLICE_HDR_MASK (1<<12)
#define PIC_DESC_MPEG2_USER_DATA_MASK (1<<13)
#define PIC_DESC_MPEG2_UNKNOWN_MASK (1<<14)

// mpeg4
#define PIC_DESC_MPEG4_VOS_MASK (1<<16)
#define PIC_DESC_MPEG4_GOV_MASK (1<<17)
#define PIC_DESC_MPEG4_VOP_MASK (1<<18)
#define PIC_DESC_MPEG4_VISUAL_OBJECT_MASK (1<<19)
#define PIC_DESC_MPEG4_EXTENSION_MASK (1<<20)
#define PIC_DESC_MPEG4_USER_DATA_MASK (1<<21)
#define PIC_DESC_MPEG4_VOL_MASK (1<<22)
#define PIC_DESC_MPEG4_VIDEO_OBJECT_MASK (1<<23)

#define MAX_NALU_NUM_IN_PIC (128)
    typedef struct
    {
        CI_S32  picDesc;
        CI_S32  naluCnt;
        CI_BASIC_UNIT nalus[MAX_NALU_NUM_IN_PIC];
    }CI_PIC_STATISTIC;
#endif

    typedef struct  
    {
        CI_PIC_TYPE picType;
        CI_PIC_STRUCT picStruct;
        CI_PIC_TYPE firstFieldType;
        CI_U8       *pFrm;
        CI_U8       *pNextPicStart;
        CI_S32      frmLen;
        CI_S32      poc;
        CI_S64      timestamp;
        CI_S32      duration;
        CI_S32      frameNum;
        CI_S32      valid;
        CI_S32     aspectRatioX;
        CI_S32     aspectRatioY;
        CI_S32     isRef;
        CI_S32     progressiveFrame;
#if PLATFORM_WIN32 || PLATFORM_MTK3353
        CI_S32      width;
        CI_S32      height;
        CI_S32         profile_idc;
        CI_PIC_STATISTIC    picStatics;
#endif
    }CI_RAW_PIC;


#ifdef __cplusplus
}
#endif

#endif
