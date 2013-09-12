#ifndef _CI_HWDEC_WRAPPER_TYPE_
#define _CI_HWDEC_WRAPPER_TYPE_

// #include "platformDef.h"
#include "ci_supplier_type.h"
#define CI_COMMON_STARTCODE_LEN (3)
#define CI_SLEEP_TIME (500)
#define CI_TIMEOUT_TIME (5000)

// decoder status mask
#define RESET_REQUIRED_MASK  (0x00000001)
#define NOTIFY_DECERROR_MASK (0x00000010)
#define MODE_NOTSUPPORT_MASK (0x00000100)

typedef enum
{
    CI_COREDEC_OMX,
    CI_COREDEC_MTK,
    CI_COREDEC_OMAP4,
    CI_COREDEC_RENESAS,
    CI_COREDEC_IMX5X,
    CI_COREDEC_VPU_IMX5X,
    CI_COREDEC_IMX51,
    CI_COREDEC_PRIMA2,
    CI_COREDEC_OTHER,
}CI_COREDEC_TYPE;

typedef struct  
{
    CI_S32 asyncMode;
    CI_S32 configRequired;
    CI_S32 maxFrameNum;
    CI_COREDEC_TYPE dectype;
    CI_INPUT_TYPE   inputType;
}CI_COREDEC_CFG;

extern CI_PIC_TYPE mp4FrmType2CiFrmType[4];
extern CI_PIC_TYPE mp2FrmType2CiFrmType[5];
extern CI_PIC_TYPE h264FrmType2CiFrmType[];

typedef union
{
    CI_S32 poc;
    CI_PIC_TYPE frmType;
}FRMORDERINFO;

typedef struct _CI_BITS
{
    CI_U32 u32BufLen;
    CI_U32 u32Code;
    CI_U32 u32CodeNext;
    CI_S32 s32Bitn;
    CI_U8 *pu8Start;
    CI_U8 *pu8Curr;
}CI_BITS;

typedef struct
{
    CI_U32  code;
    CI_U8   *pBuf;
    CI_U8   *pDstBuf;
    CI_U32  bufLen;
    CI_S32  requiredStartCodeCnt;
}CI_SEARCHPARAM;

typedef struct
{
    CI_S32 inConsumed;
    CI_U8 *pNalStart;   // save start position of last NAL unit
    CI_U8 *pNextNalStart;   // save the start position of the new NAL unit
    CI_U8 *pCurrDstBuf;
}CI_SEARCHRESULT;

typedef struct
{
    //basic informations
    CI_U32 u32Size;
    CI_U32 u32Cookie;           // unique signature used to release frame
    CI_U32 u32FrameCounter;     // used to indicate presence of dropped frames.
    CI_U32 u32FrameType;        // frame storage type
    CI_U32 u32ErrorStatus;      // error level
    CI_U32 u32ChromaFormat;
    CI_U32 u32FrameRate;
    CI_U32 u32Duration;
    CI_F64 f64BitRate;
    CI_DECODE_PTS Pts;          // Time Stamp of this frame

    CI_U8 *apu8Frame[3];        // pointer to each buffer
    CI_U32 au32Width[3];        // width
    CI_U32 au32Height[3];       // height
    CI_U32 au32Stride[3];       // stride of each buffer

    CI_U32 u32ProgressiveFrame;
    CI_U32 u32TopFieldFirst;
    CI_U32 u32AspectRatioX;
    CI_U32 u32AspectRatioY;
}CI_VIDEO_FRAME;

#define HDR_EXIST        0x1
#define HDR_CORRUPTED    0x2
#define HDR_VALID        0x4

#define PIC_OK               0x0
#define PIC_HDR_CORRUPTED    0x1
#define PIC_SLICE_CORRUPTED  0x2

#define MARK_VALID(x)                           \
    do                                          \
    {                                           \
        x &= ~(HDR_CORRUPTED);                  \
        x |= HDR_VALID;                         \
    }while(0)

#define MARK_CORRUPTED(x)                       \
    do                                          \
    {                                           \
        x &= ~(HDR_VALID);                      \
        x |= (HDR_CORRUPTED);                   \
    }while(0)


// mpeg2 syntax
#define MPEG2_PICTURE_START_CODE      0x00
#define MPEG2_SLICE_START_CODE_MIN    0x01
#define MPEG2_SLICE_START_CODE_MAX    0xAF
#define MPEG2_USER_DATA_START_CODE    0xB2
#define MPEG2_SEQUENCE_HEADER_CODE    0xB3
#define MPEG2_SEQUENCE_ERROR_CODE     0xB4
#define MPEG2_EXTENSION_START_CODE    0xB5
#define MPEG2_SEQUENCE_END_CODE       0xB7
#define MPEG2_GROUP_START_CODE        0xB8
#define MPEG2_FRAME_START_CODE        0x00000100

#define SEQUENCE_EXTENSION_ID                    1
#define SEQUENCE_DISPLAY_EXTENSION_ID            2
#define QUANT_MATRIX_EXTENSION_ID                3
#define COPYRIGHT_EXTENSION_ID                   4
#define SEQUENCE_SCALABLE_EXTENSION_ID           5
#define PICTURE_DISPLAY_EXTENSION_ID             7
#define PICTURE_CODING_EXTENSION_ID              8
#define PICTURE_SPATIAL_SCALABLE_EXTENSION_ID    9
#define PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID  10
#define CAMERA_PARAMETERS_EXTENSION_ID          11
#define ITUT_EXTENSION_ID                       12

/* mpeg2 profiles */
#define MP2V_PROF_RESERVED   0x0
#define MP2V_PROF_HIGH       0x1
#define MP2V_PROF_SPATIALLY  0x2
#define MP2V_PROF_SNR        0x3
#define MP2V_PROF_MAIN       0x4
#define MP2V_PROF_SIMPLE     0x5

/* mpeg2 levels */
#define MP2V_LVL_HIGH         0x4
#define MP2V_LVL_HIGH_1440    0x6
#define MP2V_LVL_MAIN         0x8
#define MP2V_LVL_LOW          0xa

/* chroma format */
#define MP2V_CHROMA420        1
#define MP2V_CHROMA422        2
#define MP2V_CHROMA444        3

#define MP2V_MAX_HEIGHT       1088
#define MP2V_MAX_WIDTH        1920
#define MP2V_MAX_ASRATIO      0x100
#define MP2V_MAX_FRAMERATE    0x1000
typedef struct
{
    CI_U32 u32Valid;
    CI_U8 au8FCode[2][2];
    CI_U32 u32IntraDCPrecision;
    CI_U32 u32PictureStructure;
    CI_U32 u32TopFieldFirst;
    CI_U32 u32FramePredFrameDCT;
    CI_U32 u32ConcealmentMotionVectors;
    CI_U32 u32QScaleType;
    CI_U32 u32IntraVLCFormat;
    CI_U32 u32AlternateScan;
    CI_U32 u32RepeatFirstField;
    CI_U32 u32Chroma420Type;
    CI_U32 u32ProgressiveFrame;
    CI_U32 u32CompositeDisplayFlag;
    CI_U32 u32VAxis;
    CI_U32 u32FieldSequence;
    CI_U32 u32SubCarrier;
    CI_U32 u32BurstAmplitude;
    CI_U32 u32SubCarrierPhase;
    CI_U32 u32Status;
}MP2VPicCodeExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32Width;
    CI_U32 u32Height;
    CI_U32 u32AspectRatio;
    CI_U32 u32FrameRateCode;
    CI_U32 u32BitrateValue;
    CI_U32 u32VbvBufferSizeValue;
    CI_U32 u32ConstrainedParametersFlag;
    CI_U32 u32LoadIntraQuantiserMatrix;
    CI_U8 au8IntraQuantiserMatrix[64];
    CI_U32 u32LoadNonIntraQuantiserMatrix;
    CI_U8 au8NonIntraQuantiserMatrix[64];
    CI_U32 u32Status;        // flag to indicate if the header is corrupted
}MP2VSeqHdr;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32ProfileLevelIndication;
    CI_U32 u32ProgressiveSequence;
    CI_U32 u32ChromaFormat;
    CI_U32 u32HorizontalSizeExtension;
    CI_U32 u32VerticalSizeExtension;
    CI_U32 u32BitrateExtension;
    CI_U32 u32VbvBufferExtension;
    CI_U32 u32LowDelay;
    CI_U32 u32FramerateExtensionN;
    CI_U32 u32FramerateExtensionD;
    CI_U32 u32Status;
}MP2VSeqExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32VideoFormat;
    CI_U32 u32ColorDiscription;
    CI_U32 u32ColorPrimaries;
    CI_U32 u32TransferCharacteristics;
    CI_U32 u32MatrixCoefficients;
    CI_U32 u32DisplayHorizontalSize;
    CI_U32 u32DisplayVerticalSize;
    CI_U32 u32Status;
}MP2VSeqDispExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32LoadIntraQuantiserMatrix;
    CI_U8 au8IntraQuantiserMatrix[64];
    CI_U32 u32LoadNonIntraQuantiserMatrix;
    CI_U8 au8NonIntraQuantiserMatrix[64];
    CI_U32 u32LoadChromaIntraQuantiserMatrix;
    CI_U8 au8ChromaIntraQuantiserMatrix[64];
    CI_U32 u32LoadChromaNonIntraQuantiserMatrix;
    CI_U8 au8ChromaNonIntraQuantiserMatrix[64];
    CI_U32 u32Status;
}MP2VQuantMatrixExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32ScalableMode;
    CI_U32 u32LayerID;
    CI_U32 u32LowLayerPredictionHorizontalSize;
    CI_U32 u32LowLayerPredictionVerticalSize;
    CI_U32 u32HorizontalSubsamplingFactorM;
    CI_U32 u32HorizontalSubsamplingFactorN;
    CI_U32 u32VerticalSubsamplingFactorM;
    CI_U32 u32VerticalSubsamplingFactorN;
    CI_U32 u32PictureMuxEnable;
    CI_U32 u32MuxtoProgressiveSequence;
    CI_U32 u32PictureMuxOrder;
    CI_U32 u32PictureMuxFactor;
    CI_U32 u32Status;
}MP2VSeqScalableExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32NumberOfFrameCentreOffset;
    CI_U16 au16FrameCentreHorizontalOffset[3];
    CI_U16 au16FrameCentreVerticalOffset[3];
    CI_U32 u32Status;
}MP2VPicDispExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32LowLayerTemporalReference;
    CI_U32 u32LowLayerHorizontalOffset;
    CI_U32 u32LowLayerVerticalOffset;
    CI_U32 u32SpatialTemporalWeightCodeTableIndex;
    CI_U32 u32LowLayerProgressiveFrame;
    CI_U32 u32LowLayerDeinterlacedFieldSelect;
}MP2VSPATSCExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32ReferenceSelectCode;
    CI_U32 u32ForwardTemporalReference;
    CI_U32 u32BackwardTemporalReference;
}MP2VTEMPSCExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32CopyRightFlag;
    CI_U32 u32CopyRightIdentifier;
    CI_U32 u32OriginalOrCopy;
    CI_U32 u32CopyRightNumber1;
    CI_U32 u32CopyRightNumber2;
    CI_U32 u32CopyRightNumber3;
}MP2VCpyRghtExt;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32TimeCode;
    CI_U32 u32ClosedGOP;
    CI_U32 u32BrokenLink;
    CI_U32 u32Status;
}MP2VGOPHdr;

typedef struct
{
    CI_U32 u32Valid;
    CI_U32 u32TemporalReference;
    CI_U32 u32PictureCodingType;
    CI_U32 u32VbvDelay;
    CI_U32 u32FullPelForwardVector;
    CI_U32 u32ForwardFCode;
    CI_U32 u32FullPelBckwardVector;
    CI_U32 u32BackwardFCode;
    CI_DECODE_PTS Pts;
    CI_U32 u32Status;
}MP2VPicHdr;

typedef struct  
{
    CI_PIC_STRUCT picStruct;
    CI_PIC_TYPE   picType;
    CI_S32        width;
    CI_S32        height;
    CI_DECODE_PTS picPts;
    CI_S32       aspectRatioIdx;
    CI_S32       progressiveFrame;
#if PLATFORM_MTK3353 || PLATFORM_WIN32
    CI_PIC_STATISTIC picStatics;
#endif
    CI_U32 status;  // indicates if the picture is corrupt
}MPEG2PIC;

typedef struct
{
    MP2VGOPHdr gopHdr;
    MP2VPicHdr picHdr;
    MP2VSeqHdr seqHdr;
    MP2VSeqExt seqExt;
    MP2VPicCodeExt picExt;
    MPEG2PIC    currPic;
    CI_DECODE_PTS currPts;
    CI_S32      hasPicHdr;
    CI_S32      newMp2Pic;
    CI_S32 foundSeqHdr;
    CI_S32                   gotFirstPicPts; /* Do we have the pts of the first pic */
}CI_MP2_SYNTAX;

typedef struct _H263VHEADER
{
    CI_U32 u32Vaild;

    CI_U32 u32H263FLV;
    CI_U32 u32Width;
    CI_U32 u32Height;
    CI_U32 u32CodingType;
    CI_U32 u32Quant;
    CI_U32 u32LongVector;
    CI_U32 temporal_reference;
    CI_U32 split_screen_indicator;
    CI_U32 document_camera_indicator;
    CI_U32 full_picture_freeze_release;
    CI_U32 u32LostFrameNum;

    CI_U32 u32AspectRatioX;
    CI_U32 u32AspectRatioY;

#if PLATFORM_MTK3353 || PLATFORM_WIN32
    CI_PIC_STATISTIC picStatics;
#endif
    CI_U32 status;  // indicates if the picture is corrupt
}H263VHEADER;

typedef struct
{
    H263VHEADER currPic;
    CI_S32      hasOneIntra;
    CI_S32      hasOnePic;
}CI_H263_SYNTAX;


// mpeg4 syntax
//Start Code Value P.103
#define MPEG4_START_CODE_PREFIX         0x000001
#define MPEG4_START_CODE_PREFIX_MASK    0xFFFFFF

#define MPEG4_FRAME_START_CODE          0x000001B6
#define MPEG4_VO_START_CODE                 0
#define VO_START_CODE_MIN               0x00
#define VO_START_CODE_MAX               0x1F

#define VOL_START_CODE                  2
#define VOL_START_CODE_MIN              0x20
#define VOL_START_CODE_MAX              0x2F

#define MPEG4_VOS_START_CODE                    0xB0
#define MPEG4_VOS_END_CODE                      0xB1
#define MPEG4_USER_DATA_START_CODE              0xB2
#define MPEG4_GOV_START_CODE                    0xB3
#define MPEG4_SESSION_ERR_CODE                  0xB4
#define MPEG4_VISUAL_OBJECT_START_CODE          0xB5
#define MPEG4_VOP_START_CODE                    0xB6
#define MPEG4_SLICE_START_CODE                  0xB7
#define MPEG4_EXTENSION_START_CODE              0xB8
#define MPEG4_FGS_START_CODE                    0xB9
#define MPEG4_FBA_OBJECT_START_CODE             0xBA
#define MPEG4_FBA_OBJECT_PLANE_START_CODE       0xBB
#define MPEG4_MESH_OBJECT_START_CODE            0xBC
#define MPEG4_MESH_OBJECT_PLANE_START_CODE      0xBD
#define MPEG4_STILL_TEX_OBJECT_START_CODE       0xBE
#define MPEG4_TEX_SPATIAL_LAYER_START_CODE      0xBF
typedef struct _GOV_Struct_
{
    CI_U32 u32Size;
    CI_U8  u8Valid;

    //for b-vop 
    CI_U8 time_code_hours;
    CI_U8 time_code_minutes;
    CI_U8 time_code_seconds;
    CI_S8 closed_gov;
    CI_S8 broken_link;

    CI_S32 ModuloBaseDecd;
    CI_S32 ModuloBaseDisp;

    CI_U32 DisplayTimeInMSec; //Display Time in MillionSeconds
    CI_U32 DisplayTimeInSec; //Display Time in Seconds

} GOV_Struct;

typedef struct _MP4SE_VOP
{
    CI_U32 u32Size;
    CI_U8  u8Valid;

    CI_S64 TimeCurrent;         
    CI_S16 LostFrmNum;

    CI_S8 vop_quant;

    CI_S16 Cur_Video_Packet_Num;
    CI_S8 vop_rounding_type;
    CI_S8 vop_coding_type;

    CI_U32 u32ModuloInc;

    CI_S32 vop_time_increment;
    CI_S8 vop_coded;    
    CI_S8 vop_reduced_resolution;
    //CI_U16 vop_width;
    //CI_U16 vop_height;
    //CI_S16 vop_horizontal_mc_spatial_ref; //13 bits
    //CI_S16 vop_vertical_mc_spatial_ref;  //13 bits
    CI_S8 background_composition;
    CI_S8 change_conv_ratio_disable;
    CI_S8 vop_constant_alpha;
    CI_U8 vop_constant_alpha_value; 
    //CI_S8 intra_dc_vlc_thr;
    CI_S8 top_field_first;
    //CI_S8 alternate_vertical_scan_flag;
    CI_S8 vop_fcode_forward;
    CI_S8 vop_fcode_backward;
    CI_S8 vop_shape_coding_type;

    CI_S8 ref_select_code;          

    CI_S8 header_extension_code;    

    //sprite
    CI_S32 du[4];
    CI_S32 dv[4];
    CI_S32 u[4];
    CI_S32 v[4];    
    CI_S8 brightness_change_factor_size;
    CI_S8 brightness_change_factor_code;

    // Complexity Estimation 
    CI_U8 iOpaque;
    CI_U8 iTransparent;
    CI_U8 iIntraCAE;
    CI_U8 iInterCAE;
    CI_U8 iNoUpdate;
    CI_U8 iUpsampling;
    CI_U8 iIntraBlocks;
    CI_U8 iInterBlocks;
    CI_U8 iInter4vBlocks;
    CI_U8 iNotCodedBlocks;
    CI_U8 iDCTCoefs;
    CI_U8 iDCTLines;
    CI_U8 iVLCSymbols;
    CI_U8 iVLCBits;
    CI_U8 iSadct;
    CI_U8 iAPM;
    CI_U8 iNPM;
    CI_U8 iInterpolateMCQ;
    CI_U8 iForwBackMCQ;
    CI_U8 iHalfpel2;
    CI_U8 iHalfpel4;
    CI_U8 iQuarterpel;

    //short header
    CI_S16 temporal_reference;
    CI_S8 split_screen_indicator;
    CI_S8 document_camera_indicator;
    CI_S8 full_picture_freeze_release;
    CI_S8 source_format;
    CI_S8 gob_header_empty; 
    CI_U8 psupp;
    CI_U8 gob_number;
    CI_U8 num_gobs_in_vop;
    CI_S8 gob_frame_id;
    CI_U16 num_macroblocks_in_gob;  

    CI_U32 field_info;  //
    CI_DECODE_PTS Pts;

}MP4SE_VOP;

typedef struct _MP4SE_VOL
{   
    CI_U32 u32Size;
    CI_U8  u8Valid;

    CI_S8 video_object_layer_id;
    CI_S8 short_video_header;
    CI_S8 random_accessible_vol;
    CI_U8 video_object_type_indication;
    CI_S8 is_object_layer_identifier;
    CI_S8 video_object_layer_verid;
    CI_S8 video_object_layer_priority;
    CI_S8 aspect_ratio_info;
    CI_U8 par_width;
    CI_U8 par_height;
    CI_S8 vol_control_parameters;
    CI_S8 chroma_format;
    CI_S8 low_delay;
    CI_S8 vbv_parameters;
    CI_S32 first_half_bit_rate; //30 bits
    CI_S32 latter_half_bit_rate; //30 bits
    CI_S32 first_half_vbv_buffer_size; //18 bits
    CI_S32 latter_half_vbv_buffer_size; //18 bits
    CI_S32 first_half_vbv_occupancy; //26 bits
    CI_S32 latter_half_vbv_occupancy; //26 bits
    CI_S8 video_object_layer_shape;
    CI_S8 video_object_layer_shape_extension;
    CI_S32 vop_time_increment_resolution; //16-bit unsigned 
    CI_S8 fixed_vop_rate;
    CI_S32 fixed_vop_time_increment;
    CI_U16 video_object_layer_width; //13 bits
    CI_U16 video_object_layer_height; //13 bits
    CI_S8 interlaced;
    CI_S8 obmc_disable;

    //sprite
    CI_S8 sprite_enable;
    CI_U16 sprite_width; //13 bits
    CI_U16 sprite_height; //13 bits
    CI_U16 sprite_left_coordinate; //13 bits
    CI_U16 sprite_top_coordinate; //13 bits
    CI_S8 no_of_sprite_warping_points;
    CI_S8 sprite_warping_accuracy;
    CI_S8 sprite_brightness_change;
    CI_S8 low_latency_sprite_enable;
    //~sprite

    //CI_S8 not_8_bit;
    //CI_S8 sadct_disable;
    //CI_S8 quant_precision;
    //CI_S8 bits_per_pixel;
    //CI_S8 no_gray_quant_update;
    //CI_S8 composition_method;
    //CI_S8 linear_composition;
    CI_S8 quant_type;
    //CI_S8 load_intra_quant_mat;
    //CI_U8 *intra_quant_mat;
    //CI_U8 MPEG4IntraQuantizerMatrix[64];
    //CI_S8 load_nonintra_quant_mat;
    //CI_U8 *nointra_quant_mat;
    //CI_U8 MPEG4InterQuantizerMatrix[64];
    //CI_S8 load_intra_quant_mat_grayscale;
    //CI_U8 *intra_quant_mat_grayscale;
    //CI_S8 load_nonintra_quant_mat_grayscale;
    //CI_U8 *nonintra_quant_mat_grayscale;
    CI_S8 quarter_sample;
    CI_S8 complexity_estimation_disable;
    CI_S8 estimation_method;
    CI_S8 shape_complexity_estimation_disable;
    CI_S8 opaque;
    CI_S8 transparent;
    CI_S8 intra_cae;
    CI_S8 inter_cae;
    CI_S8 no_update;
    CI_S8 upsampling;
    CI_S8 version2_complexity_estimation_disable;
    CI_S8 Sadct;
    CI_S8 Quarterpel;
    CI_S8 texture_complexity_estimation_set_1_disable;
    CI_S8 intra_blocks;
    CI_S8 inter_blocks;
    CI_S8 inter4v_blocks;
    CI_S8 not_coded_blocks;
    CI_S8 texture_complexity_estimation_set_2_disable;
    CI_S8 dct_coefs;
    CI_S8 dct_lines;
    CI_S8 vlc_symbols;
    CI_S8 vlc_bits;
    CI_S8 motion_compensation_complexity_disable;
    CI_S8 apm;
    CI_S8 npm;
    CI_S8 interpolate_mc_q;
    CI_S8 forw_back_mc_q;
    CI_S8 halfpel2;
    CI_S8 halfpel4;
    CI_S8 resync_marker_disable;
    CI_S8 data_partitioned;
    CI_S8 reversible_vlc;
    CI_S8 newpred_enable;
    CI_S8 requested_upstream_message_type;
    CI_S8 newpred_segment_type;
    CI_S8 reduced_resolution_vop_enable;
    //CI_S8 scalability;
    //CI_S8 hierarchy_type;
    //CI_S8 ref_layer_id;
    //CI_S8 ref_layer_sampling_direc;
    //CI_S8 hor_sampling_factor_n;
    //CI_S8 hor_sampling_factor_m;
    //CI_S8 vert_sampling_factor_n;
    //CI_S8 vert_sampling_factor_m;
    //CI_S8 enhancement_type;
    //CI_S8 use_ref_shape;
    //CI_S8 use_ref_texture;
    //CI_S8 shape_hor_sampling_factor_n;
    //CI_S8 shape_hor_sampling_factor_m;
    //CI_S8 shape_vert_sampling_factor_n;
    //CI_S8 shape_vert_sampling_factor_m;   

    CI_U16 real_video_object_layer_width; //13 bits
    CI_U16 real_video_object_layer_height; //13 bits

    CI_U32 u32BitNum_TimeIncr;
    CI_U32 u32BitNum_MBNum;
    CI_U32 u32MBXNum;
    CI_U32 u32MBYNum;
    CI_U32 u32MBNum;

}MP4SE_VOL;

typedef struct _MP4SE_VOS
{
    CI_U32 u32Size;
    CI_U8  u8Valid;

    CI_U8  profile_and_level_indication;
}MP4SE_VOS;

typedef struct _MP4SE_VO
{
    CI_U32 u32Size;
    CI_U8  u8Valid;

    CI_U8   is_visual_object_identifier;
    CI_U8   visual_object_verid;
    CI_U8   visual_object_priority;
    CI_U8   visual_object_type;

    //video_signal_type
    CI_U8   video_signal_type;
    CI_U8   video_format;
    CI_U8   video_range;
    CI_U8   colour_description;
    CI_U8   colour_primaries;
    CI_U8   transfer_characteristics;
    CI_U8   matrix_coefficients;
}MP4SE_VO;

typedef struct _MP4SE_GOV
{
    CI_U32  u32Size;
    CI_U8   u8Valid;

    //CI_U32    time_code;
    CI_U8   time_code_hours;
    CI_U8   time_code_minutes;
    CI_U8   time_code_seconds;
    CI_U8   closed_gov;
    CI_U8   broken_link;

    CI_U32  u32ModuloBaseSec;   // time code in seconds

}MP4SE_GOV;

typedef struct  
{
    CI_PIC_STRUCT picStruct;
    CI_PIC_TYPE   picType;
    CI_S32        width;
    CI_S32        height;
    CI_DECODE_PTS picPts;
#if PLATFORM_MTK3353 || PLATFORM_WIN32
    CI_PIC_STATISTIC picStatics;
#endif
    CI_U32 status;  // indicates if the picture is corrupt
}MP4PIC;

typedef struct {
    MP4SE_GOV gov;
    MP4SE_VO  vo;
    MP4SE_VOL vol;
    MP4SE_VOP vop;
    MP4SE_VOS vos;
    CI_S64 s64TimeCurrent;
    CI_U32 u32ModuloBaseDec;
    CI_U32 u32ModuloBaseDisp;
    CI_S64 s64TimeCurrent_copy;
    CI_U32 u32ModuloBaseDec_copy;
    CI_U32 u32ModuloBaseDisp_copy;
    CI_U32 u32BVOPCnt;
    CI_U32 u32BitNum_ResyncMarker;

    // unused data
    CI_U32 u32DispTimeMSec;
    CI_S64 s64TimeNextRef;
    CI_S64 s64TimePreRef;
    CI_U32 u32CurVopTimeInc;
    CI_U32 u32CurVopDispTick;
    CI_U32 u32DispTimeMSec_copy;
    CI_S64 s64TimeNextRef_copy;
    CI_S64 s64TimePreRef_copy;
    CI_U32 u32CurVopTimeInc_copy;
    CI_U32 u32CurVopDispTick_copy;

    MP4PIC currMp4Pic;
    CI_S32 newMp4Pic;
    CI_S32 foundSeqHdr;
    CI_S32 hasVopHdr;
    CI_DECODE_PTS currPts;
}CI_MP4_SYNTAX;

// h264 syntax
#define NALU_START_CODE    0x000001
#define NALU_TYPE_SLICE    1
#define NALU_TYPE_DPA      2
#define NALU_TYPE_DPB      3
#define NALU_TYPE_DPC      4
#define NALU_TYPE_IDR      5
#define NALU_TYPE_SEI      6
#define NALU_TYPE_SPS      7
#define NALU_TYPE_PPS      8
#define NALU_TYPE_PD       9
#define NALU_TYPE_EOSEQ    10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL     12

enum {
    P_SLICE = 0,
    B_SLICE,
    I_SLICE,
    SP_SLICE,
    SI_SLICE
};

//! definition of SEI payload type
typedef enum {
    SEI_BUFFERING_PERIOD = 0,
    SEI_PIC_TIMING,
    SEI_PAN_SCAN_RECT,
    SEI_FILLER_PAYLOAD,
    SEI_USER_DATA_REGISTERED_ITU_T_T35,
    SEI_USER_DATA_UNREGISTERED,
    SEI_RECOVERY_POINT,
    SEI_DEC_REF_PIC_MARKING_REPETITION,
    SEI_SPARE_PIC,
    SEI_SCENE_INFO,
    SEI_SUB_SEQ_INFO,
    SEI_SUB_SEQ_LAYER_CHARACTERISTICS,
    SEI_SUB_SEQ_CHARACTERISTICS,
    SEI_FULL_FRAME_FREEZE,
    SEI_FULL_FRAME_FREEZE_RELEASE,
    SEI_FULL_FRAME_SNAPSHOT,
    SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START,
    SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END,
    SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET,
    SEI_FILM_GRAIN_CHARACTERISTICS,
    SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE,
    SEI_STEREO_VIDEO_INFO,

    SEI_MAX_ELEMENTS  //!< number of maximum syntax elements
} SEI_type;

typedef struct
{
    CI_U32  cpb_cnt_minus1;                                   // ue(v)
    CI_U32  bit_rate_scale;                                   // u(4)
    CI_U32  cpb_size_scale;                                   // u(4)
    CI_U32  bit_rate_value_minus1 [32];    // ue(v)
    CI_U32  cpb_size_value_minus1 [32];    // ue(v)
    CI_U32  cbr_flag              [32];    // u(1)
    CI_U32  initial_cpb_removal_delay_length_minus1;          // u(5)
    CI_U32  cpb_removal_delay_length_minus1;                  // u(5)
    CI_U32  dpb_output_delay_length_minus1;                   // u(5)
    CI_U32  time_offset_length;                               // u(5)
} hrd_parameters_t;

typedef struct
{
    CI_U32 aspect_ratio_info_present_flag;                   // u(1)
    CI_U32  aspect_ratio_idc;                               // u(8)
    CI_U32  sar_width;                                    // u(16)
    CI_U32  sar_height;                                   // u(16)
    CI_U32 overscan_info_present_flag;                       // u(1)
    CI_U32 overscan_appropriate_flag;                      // u(1)
    CI_U32 video_signal_type_present_flag;                   // u(1)
    CI_U32  video_format;                                   // u(3)
    CI_U32 video_full_range_flag;                          // u(1)
    CI_U32 colour_description_present_flag;                // u(1)
    CI_U32  colour_primaries;                             // u(8)
    CI_U32  transfer_characteristics;                     // u(8)
    CI_U32  matrix_coefficients;                          // u(8)
    CI_U32 chroma_location_info_present_flag;                // u(1)
    CI_U32  chroma_sample_loc_type_top_field;               // ue(v)
    CI_U32  chroma_sample_loc_type_bottom_field;            // ue(v)
    CI_U32 timing_info_present_flag;                         // u(1)
    CI_U32  num_units_in_tick;                              // u(32)
    CI_U32  time_scale;                                     // u(32)
    CI_U32 fixed_frame_rate_flag;                          // u(1)
    CI_U32 nal_hrd_parameters_present_flag;                  // u(1)
    hrd_parameters_t nal_hrd_parameters;                      // hrd_paramters_t
    CI_U32 vcl_hrd_parameters_present_flag;                  // u(1)
    hrd_parameters_t vcl_hrd_parameters;                      // hrd_paramters_t
    // if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
    CI_U32 low_delay_hrd_flag;                             // u(1)
    CI_U32 pic_struct_present_flag;                        // u(1)
    CI_U32 bitstream_restriction_flag;                       // u(1)
    CI_U32 motion_vectors_over_pic_boundaries_flag;        // u(1)
    CI_U32  max_bytes_per_pic_denom;                        // ue(v)
    CI_U32  max_bits_per_mb_denom;                          // ue(v)
    CI_U32  log2_max_mv_length_vertical;                    // ue(v)
    CI_U32  log2_max_mv_length_horizontal;                  // ue(v)
    CI_U32  num_reorder_frames;                             // ue(v)
    CI_U32  max_dec_frame_buffering;                        // ue(v)
} vui_seq_parameters_t;

typedef struct
{
    CI_S32 Valid;                  // indicates the parameter set is valid
    CI_U32  profile_idc;                                      // u(8)
    CI_U32 constrained_set0_flag;                            // u(1)
    CI_U32 constrained_set1_flag;                            // u(1)
    CI_U32 constrained_set2_flag;                            // u(1)
    CI_U32 constrained_set3_flag;                            // u(1)
    CI_U32  level_idc;                                        // u(8)
    CI_U32  chroma_format_idc;                                // ue(v)
    CI_U32  separate_colour_plane_flag;                      // u(1)
    CI_U32  bit_depth_luma_minus8;                           // ue(v)
    CI_U32  bit_depth_chroma_minus8;                         // ue(v)
    CI_U32  qpprime_y_zero_transform_bypass_flag;            // u(1)
    CI_U32  seq_scaling_matrix_present_flag;                 // u(1)
    CI_U32  seq_scaling_list_present_flag[12];               // u(1)
    CI_S32  scalingList4x4[6][16];
    CI_S32  useDefaultScalingMatrix4x4Flag[6];
    CI_S32  scalingList8x8[6][64];
    CI_S32  useDefaultScalingMatrix8x8Flag[6];
    CI_U32  seq_parameter_set_id;                             // ue(v)
    CI_U32  log2_max_frame_num_minus4;                        // ue(v)
    CI_U32  pic_order_cnt_type;
    // if( pic_order_cnt_type == 0 ) 
    CI_U32  log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
    // else if( pic_order_cnt_type == 1 )
    CI_U32 delta_pic_order_always_zero_flag;               // u(1)
    CI_S32  offset_for_non_ref_pic;                         // se(v)
    CI_S32  offset_for_top_to_bottom_field;                 // se(v)
    CI_U32  num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
    // for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
    CI_S32  offset_for_ref_frame[255];   // se(v)
    CI_S32  expected_delta_per_picordercount_cycle;
    CI_U32  num_ref_frames;                                   // ue(v)
    CI_U32 gaps_in_frame_num_value_allowed_flag;             // u(1)
    CI_U32  pic_width_in_mbs_minus1;                          // ue(v)
    CI_U32  pic_height_in_map_units_minus1;                   // ue(v)
    CI_U32 frame_mbs_only_flag;                              // u(1)
    // if( !frame_mbs_only_flag ) 
    CI_U32 mb_adaptive_frame_field_flag;                   // u(1)
    CI_U32 direct_8x8_inference_flag;                        // u(1)
    CI_U32 frame_cropping_flag;                              // u(1)
    CI_U32  frame_cropping_rect_left_offset;                // ue(v)
    CI_U32  frame_cropping_rect_right_offset;               // ue(v)
    CI_U32  frame_cropping_rect_top_offset;                 // ue(v)
    CI_U32  frame_cropping_rect_bottom_offset;              // ue(v)
    CI_U32 vui_parameters_present_flag;                      // u(1)
    vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
    CI_U32  vui_framerate;
} seq_parameter_set_rbsp_t;

typedef struct
{
    CI_S32 Valid;                  // indicates the parameter set is valid
    CI_U32  pic_parameter_set_id;                             // ue(v)
    CI_U32  seq_parameter_set_id;                             // ue(v)
    CI_U32  entropy_coding_mode_flag;                         // u(1)
    CI_U32  pic_order_present_flag;                           // u(1)
    CI_U32  num_slice_groups_minus1;                          // ue(v)
    CI_U32  slice_group_map_type;                             // ue(v)
    CI_U32  run_length_minus1[8];    // ue(v)
    CI_U32  top_left[8];             // ue(v)
    CI_U32  bottom_right[8];         // ue(v)
    CI_U32 slice_group_change_direction_flag;                // u(1)
    CI_U32  slice_group_change_rate_minus1;                   // ue(v)
    CI_U32  num_slice_group_map_units_minus1;                 // ue(v)
    CI_U32  pic_size_in_map_units_minus1;                     // ue(v)
    //CI_U32  *slice_group_id;                                // complete MBAmap u(v)
    CI_U32  slice_group_id[8];       // modified by Jerry, not correct here! by joshua
    CI_U32  num_ref_idx_l0_active_minus1;                     // ue(v)
    CI_U32  num_ref_idx_l1_active_minus1;                     // ue(v)
    CI_U32 weighted_pred_flag;                               // u(1)
    CI_U32 weighted_bipred_idc;                              // u(2)
    CI_S32  pic_init_qp_minus26;                              // se(v)
    CI_S32  pic_init_qs_minus26;                              // se(v)
    CI_S32  chroma_qp_index_offset;                           // se(v)
    CI_U32 deblocking_filter_control_present_flag;           // u(1)
    CI_U32 constrained_intra_pred_flag;                      // u(1)
    CI_U32 redundant_pic_cnt_present_flag;                   // u(1)
} pic_parameter_set_rbsp_t;

typedef struct
{
    CI_S32 first_mb_in_slice;
    CI_S32 slice_type;
    CI_S32 pic_parameter_set_id;
    CI_U32 frame_num;
    CI_S32 colour_plane_id;
    CI_U32 field_pic_flag;
    CI_U32 bottom_field_flag;
    CI_S32 idr_pic_id;
    CI_U32 pic_order_cnt_lsb;
    CI_S32 delta_pic_order_cnt_bottom;
    CI_S32 delta_pic_order_cnt0;
    CI_S32 delta_pic_order_cnt1;
    CI_S32 redundant_pic_cnt;
    CI_U32 direct_spatial_mv_pred_flag;
    CI_U32 num_ref_idx_active_override_flag;
    CI_S32 num_ref_idx_l0_active_minus1;
    CI_S32 num_ref_idx_l1_active_minus1;

    // ref picture list reordering
    CI_U32 ref_pic_list_reordering_flag_l0;
    CI_S32 reordering_of_pic_nums_idc_l0[16];
    CI_S32 abs_diff_pic_num_minus1_l0[16];
    CI_S32 long_term_pic_num_l0[16];
    CI_U32 ref_pic_list_reordering_flag_l1;
    CI_S32 reordering_of_pic_nums_idc_l1[16];
    CI_S32 abs_diff_pic_num_minus1_l1[16];
    CI_S32 long_term_pic_num_l1[16];

    // pred weight table
    CI_S32 luma_log2_weight_denom;
    CI_S32 chroma_log2_weight_denom;
    CI_S32 luma_weight_l0_flag;
    CI_S32 luma_weight_l0[16];
    CI_S32 luma_offset_l0[16];
    CI_S32 chroma_weight_l0_flag;
    CI_S32 chroma_weight_l0[16][2];
    CI_S32 chroma_offset_l0[16][2];
    CI_S32 luma_weight_l1_flag;
    CI_S32 luma_weight_l1[16];
    CI_S32 luma_offset_l1[16];
    CI_S32 chroma_weight_l1_flag;
    CI_S32 chroma_weight_l1[16][2];
    CI_S32 chroma_offset_l1[16][2];

    // dec reference pic marking
    CI_U32 no_output_of_prior_pics_flag;
    CI_U32 long_term_reference_flag;
    CI_U32 adaptive_ref_pic_marking_mode_flag;
    CI_S32 memory_management_control_operation;
    CI_S32 difference_of_pic_nums_minus1;
    CI_S32 long_term_pic_num;
    CI_S32 long_term_frame_idx;
    CI_S32 max_long_term_frame_idx_plus1;

    CI_U32 cabac_init_idc;
    CI_S32 slice_qp_delta;
    CI_S32 nal_unit_type;
    CI_S32 nal_ref_idc;
    CI_S32 has_mmco_5;

    CI_S32 valid;
}slice_hdr_t;

typedef struct  
{
    CI_PIC_STRUCT picStruct;
    CI_S32 sliceType;
    CI_S32 TopFieldOrderCnt;      //poc for this top field // POC200301
    CI_S32 BottomFieldOrderCnt;   //poc of bottom field of frame
    CI_S32 PicOrderCnt;    //poc of this frame // POC200301
    CI_S32 pic_order_cnt_lsb;
    CI_S32 delta_pic_order_cnt_bottom;
    CI_S32 has_mmco_5;
    CI_S32 nal_unit_type;
    CI_S32 nal_ref_idc;
    CI_S32 delta_pic_order_cnt[2];
    CI_S32  frame_num;
    CI_S32  BufferingPeriodSei;
    CI_S32  cpb_removal_delay;
    CI_DECODE_PTS picPts;
    CI_S32 picWidth;
    CI_S32 picHeight;
    CI_S32 frameHeight;
    CI_S32 aspectRatioIdx;
    CI_S32 sarWidth;
    CI_S32 sarHeight;
    CI_S32 isRef;               // indicate whether it is a reference frame
    CI_U32 status;   // indicats whether the picture is corrupted
#if PLATFORM_WIN32 || PLATFORM_MTK3353
    CI_PIC_STATISTIC    picStatics;
#endif
}H264FRAME;

typedef struct
{
    CI_U32 forbidden_zero_bit;
    CI_U32 nal_ref_idc;
    CI_U32 nal_unit_type;
    CI_S32  newH264Pic;
    CI_S32  frmType;
    CI_S32  frame_num;
    slice_hdr_t              prevSliceHdr;
    slice_hdr_t              currSliceHdr;
    seq_parameter_set_rbsp_t SeqParSet[32];
    pic_parameter_set_rbsp_t PicParSet[255];
    seq_parameter_set_rbsp_t *active_sps;
    pic_parameter_set_rbsp_t *active_pps;

    CI_S32                   last_has_mmco_5;
    CI_S32                   PrevPicOrderCntMsb;
    CI_S32                   PrevPicOrderCntLsb;
    CI_PIC_STRUCT            prevRefPicType;
    CI_S32                   prevRefTopPoc;
    CI_S32                   prevFrameNumOffset;
    CI_S32                   prevFrameNum;
    H264FRAME                currFrame;
    CI_S32                   foundSPS;
    CI_S32                   picWidth;
    CI_S32                   picHeight;
    CI_S32                   frameHeight;
    
    CI_S32                  dispWidth;
    CI_S32                  dispHeight;
    CI_S32                   MaxFrameNum;
    CI_S32                   cpb_removal_delay;
    CI_S32                   BufferingPeriodSei;
    CI_S32                   prev_cpb_removal_delay;
    CI_S32                   gotFirstPicPts; /* Do we have the pts of the first pic */
    CI_S32                   currPicIsRef;   /* used for detect reference B */
    CI_DECODE_PTS            currPts;
}CI_H264_SYNTAX;

typedef union
{
    CI_MP2_SYNTAX mpeg2syntax;
    CI_MP4_SYNTAX mpeg4syntax;
    CI_H264_SYNTAX h264syntax;
    CI_H263_SYNTAX h263syntax;
}CI_VIDEO_SYNTAX;

typedef struct 
{
    CI_U8 *pSegmentalNalStart;
    CI_S32 segmentalNalLen;
    CI_U8 *pBufStart;
    CI_DECODE_PTS segPts;
}SEGMENTAL_NAL_INFO;

typedef struct  
{
    CI_S32 hasFrame;
    CI_S32 skipFrame;
    CI_S32 inConsumed;
    CI_U8   *pNextPicStart;
}PARSER_RESULT;

typedef struct
{
    CI_S32 width;
    CI_S32 height;
}STREAM_CONFIG;

typedef CI_RESULT (*PFN_GET_FRAME)(CI_SEARCHPARAM *pParam, CI_SEARCHRESULT *pResult);
typedef CI_RESULT (*PFN_SAVE_DECODED_FRAME)(CI_VIDEO_FRAME *pFrame, CI_VOID *pAppParam);
typedef CI_RESULT (*PFN_RELEASE_ALL_DISP_FRAME)(CI_VOID *pApp);
typedef CI_RESULT (*PFN_GET_PIC_INFO)(CI_VOID *pvParser, CI_VOID *pvOutPic);
typedef CI_RESULT (*PFN_GET_CONFIG_INFO)(CI_VOID *pvParser, CI_VOID *pvConfigInfo);

#define TEMP_BUF_SIZE (512)
typedef struct
{
    CI_VIDEO_SYNTAX videoSyntax;
    CI_S32      currPicCorrupted;

    CI_S32      bypassMode;
    CI_VSTD_TYPE vtype; 
    PFN_GET_FRAME pfnGetFrame;
    PFN_GET_PIC_INFO pfnGetPicInfo;
    PFN_GET_CONFIG_INFO pfnGetConfigInfo;
    CI_S32  firstFrm;
    CI_U8   *pFirstPicStart;
    CI_U8   tempBuf[TEMP_BUF_SIZE+4];
    CI_S32  tempBufLen;
    CI_DECODE_PTS   currInPts;
    CI_DECODE_PTS   bytePts[3];
#if PLATFORM_WIN32 || PLATFORM_MTK3353
    CI_S32  firstValidNalu;
    CI_PIC_STATISTIC    picStatics;
#endif
}CI_PARSER;

#endif
