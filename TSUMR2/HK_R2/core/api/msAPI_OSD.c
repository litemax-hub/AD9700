////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//  [Doxygen]
/// @file msAPI_OSD.h
/// This file includes MStar application interface for OSD
/// @brief API for OSD functions
/// @author MStar Semiconductor, Inc.
//
////////////////////////////////////////////////////////////////////////////////

#define MSAPI_OSD_C

/********************************************************************************/
/*                    Header Files                                              */
/********************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "datatype.h"
#include "MsTypes.h"
#include "debug.h"
#include "Global.h"
 // FIXME: nameing of GE if
#include "apiGOP.h"
#include "msAPI_OSD.h"
#include "msAPI_Memory.h"
#include "msAPI_Font.h"
//#include "msAPI_Timer.h"
#include "debug.h"
#include "MsTypes.h"
#include "drvBDMA.h"
#include "msAPI_OSD_Resource.h"
#include "sysinfo.h"
#include "msAPI_OCP.h"
#if (OBA2)
#include "p_misc.h"
#endif

#if (ENABLE_THAI_OSD)
#include "msAPI_Thai_Shaping.h"
#endif
#if (ENABLE_ARABIC_OSD)
#include "msAPI_Arabic_ushape.h"
#endif

#define APIOSD_DBG(x) //x

#define ENABLE_DEFAULT_KERNING 1
#define ENABLE_SHORT_STRING_BUFFER 1
#define MAX_STRING_BYTES    1000 // 350  //  255

/********************************************************************************/
/*                     Local                                                    */
/********************************************************************************/
//static GEClrPoint cpoint;
//static GEPoint point;
//static GEClrLine clrline;
//static GERGBColor b_clr,f_clr;
//static GELineFmt lineFmt;
//static GEBitmapFmt bmpfmt;
//static GERectFmt rfmt;
//static GERoundFmt roundfmt;
//static GELineFmt lfmt;
//static GEPESettings    GEPE;
//static GELineRectInfo  GELineRect;
//static GEPESettings  PESet;
//U16 au16InfoScrollLineInx[100];
U16 u16NextLineStartInx = 0;
bit bStopScrollDown= true;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
static BOOLEAN bDivxSubtitleMode = FALSE;
//static BOOLEAN bDivxSubtitleText = FALSE;
static BOOLEAN bHebrewMixedMode = FALSE;
static EN_OSDAPI_LANGUAGE enLanguage;
#endif

#if ENABLE_DEFAULT_KERNING
#include "kerning.h"
#else
static const U8 code _dummy = 0;
#endif //ENABLE_DEFAULT_KERNING

#if 0
#define STROSD  256
#else // Modified by converity_636
#define STROSD  512//256
#endif

typedef union
{
	U8  U8Str[STROSD*2];
	U16 U16Str[STROSD];
} OSDStringType;

typedef union
{
	U8  U8Str[STROSD*4];
	U16 U16Str[STROSD*2];
} OSDStringTypeDouble;

OSDStringType TempStr;
MSAPI_FontGWin gstFontGwin[GWIN_LAYER_MAX];
BOOLEAN isFontGwinInit[GWIN_LAYER_MAX] = {FALSE,FALSE};

/********************************************************************************/
/*                   Functions                                                  */
/********************************************************************************/
#if (ENABLE_ARABIC_OSD)
static void msAPI_OSD_KurdishFilter(U16 *inp, U8 len);
static U8 msAPI_OSD_IsArabicVowel(U16 inputc);
#endif

//*************************************************************************
//Function name:        msAPI_OSD_u16Strlen
//Passing parameter:    U16 *pu16Str : string pointor
//Return parameter:     U16 : string length
//Description:          Transfer numer into string
//*************************************************************************


void msAPI_OSD_GET_resource()
{
    //printf("OSD GET RESOURCE \n");
   // MApi_GFX_SetShareMode(GFX_NOSHARE);
}

void msAPI_OSD_Free_resource()
{
    //printf("OSD FREE RESOURCE \n");
   // MApi_GFX_SetShareMode(GFX_SHARE);
}

/******************************************************************************/
/// API for Get string length::
/// Get string length
/// @param pu16Str \b IN string pointor
/// @return U16: String length
/******************************************************************************/
U16 msAPI_OSD_u16Strlen(U16 *pu16Str)
{
    U16 u16Count;

    u16Count = 0;

    // Add it by coverity_0537
    if (pu16Str == NULL)
    {
        return u16Count;
    }

    while( (*pu16Str++ != 0) && (u16Count < MAX_STRING_BYTES) )
    {
        u16Count++;
    }
    return u16Count;
}

#if 1//(KEEP_UNUSED_FUNC == 1 || defined(ZUI))

//*************************************************************************
//Function name:        msAPI_OSD_u16Strcpy
//Passing parameter:    U16 *pu16Dest: destnation
//                      U16 *pu16Src: source
//Return parameter:     none
//Description:          string copy
//*************************************************************************

/******************************************************************************/
/// API for string copy::
/// @param *pu16Dest \b OUT string destination
/// @param *pu16Src \b IN string source
/******************************************************************************/
void msAPI_OSD_u16Strcpy(U16 *pu16Dest, U16 *pu16Src)
{
    memcpy(pu16Dest, pu16Src, (msAPI_OSD_u16Strlen(pu16Src)+1) * 2);
}

/******************************************************************************/
/// API for string concatenation::
/// @param pu16Dest \b OUT string destination
/// @param pu16Src \b IN string source
/******************************************************************************/
void msAPI_OSD_u16Strcat(U16 *pu16Dest, U16 *pu16Src)
{
    memcpy(pu16Dest + msAPI_OSD_u16Strlen(pu16Dest), pu16Src, (msAPI_OSD_u16Strlen(pu16Src)+1) * 2);
}
#endif

#if (KEEP_UNUSED_FUNC == 1)
/******************************************************************************/
/// API for string compare::
/// get compared result
/// @param *u16strA \b IN string destination
/// @param *u16strB \b IN string source
/******************************************************************************/
S8 msAPI_OSD_u16Strcmp(U16 *u16strA, U16 *u16strB)
{
    U16 strlen_a, strlen_b;

    strlen_a = msAPI_OSD_u16Strlen(u16strA);
    strlen_b = msAPI_OSD_u16Strlen(u16strB);
    return memcmp((U8 *) u16strA, (U8 *) u16strB, MAX(strlen_a, strlen_b));
}
#endif
/******************************************************************************/
/// API for get string length::
/// Get string length
/// @param *pu8Str \b IN pointer to string
/// @param *u8NoOfBytesPerChar \b IN btyes per char
/// @return U16: String length
/******************************************************************************/
U16 msAPI_OSD_GetStrLength(U8 *pu8Str, EN_CHAR_IDX_LENGTH u8NoOfBytesPerChar)
{
    U16 length;
    U16 i, u16tmp[MAX_STRING_BYTES];

    if(u8NoOfBytesPerChar == CHAR_IDX_1BYTE)
    {
         length = strlen((char *)pu8Str);

        if (length > MAX_STRING_BYTES)
        {
            length = MAX_STRING_BYTES;
        }
    }
    else //(u8NoOfBytesPerChar == CHAR_IDX_2BYTE)
    {
        for (i=0; i<MAX_STRING_BYTES; i++)
        {
            u16tmp[i] = (pu8Str[i*2+1]<<8) + pu8Str[i*2];
            if (u16tmp[i]==0)
                break;
        }
        length = msAPI_OSD_u16Strlen(u16tmp);
    }

    return length;
}

/******************************************************************************/
/// API for get string width::
/// Get string width
/// @param font_handle \b IN font handle
/// @param *pu8Str \b IN pointer to string
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/// @return U16: String width
/******************************************************************************/
U16 msAPI_OSD_GetStrWidth(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn)
{
    U16 i;
    U16 length, width;
    OSD_RESOURCE_FONT_INFO finfo;
    OSDSRC_GLYPH_BBOX_X BBox_X_Info;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringType TempBuf;
	OSDStringTypeDouble outbuf;
#if ( ENABLE_ARABIC_OSD )
	U8 bMirrorLang = FALSE;
    U8 bArabicDisplay = FALSE;
    U8 bHebrew = FALSE;
#endif
#if ( ENABLE_THAI_OSD )
	U8 bThaiDisplay = FALSE;
#endif
    U16 nFontID;
#endif

    width = 0;
    length = msAPI_OSD_GetStrLength(pu8Str, pclrBtn->bStringIndexWidth);

        msAPI_OSD_GET_resource();

    if( length )
    {
    #if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

    	switch(enLanguage)
    	{
    		case LANGUAGE_OSD_ARABIC:
    		case LANGUAGE_OSD_PARSI:
    		case LANGUAGE_OSD_KURDISH:
    			nFontID = FontHandleIndex[font_handle];
    			if ( nFontID < FONT_MULTILANG_BEGIN )
    			{
    /*
    				if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
    				{
    					bHebrew = TRUE;
    				}
    				else
    */
    				{
    					if ( bHebrewMixedMode )
    					{
    						bHebrew = TRUE;
    					}
    					else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
    					{
    						font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;
    					}

    					if ( !bDivxSubtitleMode )
    					{
    						pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
    					}
    				}
    			}

    			if ( bHebrew == FALSE )
    			{
    				bArabicDisplay = TRUE;
    			}
    			break;
    		case LANGUAGE_OSD_HEBREW:
    			bHebrew = TRUE;
    			if ( !bDivxSubtitleMode )
    			{
    				pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
    			}
    			break;

    #if ( ENABLE_THAI_OSD )
    		case LANGUAGE_OSD_THAI:
    			nFontID = FontHandleIndex[font_handle];
    			if ( nFontID < FONT_MULTILANG_BEGIN )
    			{
    /*
    				if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
    				{
    					bHebrew = TRUE;
    				}
    				else
    */
    				{
    					if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
    						font_handle = Font[nFontID+FONT_THAI_0].fHandle;
    				}
    			}

    	#if ( ENABLE_ARABIC_OSD )
    			if ( bHebrew == FALSE )
    	#endif
    			{
    				bThaiDisplay = TRUE;
    			}
    			break;
    #endif
    /*
    		case LANGUAGE_OSD_CHINESE:
    			nFontID = FontHandleIndex[font_handle];
    			if ( nFontID < FONT_MULTILANG_BEGIN )
    			{
    				if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
    				{
    					bHebrew = TRUE;
    				}
    				else
    				{
    					if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
    					{
    						font_handle = Font[nFontID+18].fHandle;
    					}
    					else if ( nFontID >= FONT_EXT_BEGIN )
    					{
    						//Do Chinese icon unicode conversion here
    					}
    				}
    			}
    			break;
    */
    		default:
    			break;
    	}
    #endif

	#if (ENABLE_ARABIC_OSD)
		if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
		{
			bMirrorLang = TRUE;
		}
	#endif

		if(pclrBtn->bStringIndexWidth==CHAR_IDX_2BYTE)
		{
	    #if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
			if (length >= sizeof(TempBuf.U8Str)/2)
			{
				length = sizeof(TempBuf.U8Str)/2 - 1;
			}
	    #endif
	    #if (ENABLE_ARABIC_OSD)
			if ( bMirrorLang )
			{
				memcpy(TempBuf.U8Str,pu8Str,length*2);
				TempBuf.U16Str[length] = 0;

				//For Subtitle
				if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
				{
					S32 errorCode = U_ZERO_ERROR;

					ArabicParser(TempBuf.U16Str, length, outbuf.U16Str, STROSD,
							 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
							 &errorCode);
					//Every language is possible, you can't skip this filter here
					msAPI_OSD_KurdishFilter(outbuf.U16Str, length);
				}
				else if ( bArabicDisplay ) //for OSD
				{
					 S32 errorCode = U_ZERO_ERROR;

					ArabicParser(TempBuf.U16Str, length, outbuf.U16Str, STROSD,
							 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
							 &errorCode);
					//just skip this function for non Kurdish condition
					if ( enLanguage == LANGUAGE_OSD_KURDISH )
					{
						msAPI_OSD_KurdishFilter(outbuf.U16Str, length);
					}
				}
				else    // for Hebrew
				{
					memcpy(outbuf.U8Str, pu8Str, length*2);
				}

				outbuf.U16Str[length] = 0;

				pu8Str = outbuf.U8Str;
			}
	    #endif

	    #if (ENABLE_THAI_OSD)
			else if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
			{
				//if(pclrBtn->bStringIndexWidth == CHAR_IDX_2BYTE)
				//{
					memcpy(TempBuf.U8Str,pu8Str,length*2);
					TempBuf.U16Str[length] = 0;

					//pu16Str = (U16 *)pu8Str;

					length = Thai_compose(TempBuf.U16Str, 0, length, STROSD, outbuf.U16Str);
					outbuf.U16Str[length] = 0x0000;

					pu8Str = outbuf.U8Str;
				//}
			}
			else
	    #endif
			{
	//            memcpy(outbuf.U8Str,pu8Str,length*2);

	//            outbuf.U16Str[length] = 0;

	//            pu8Str = outbuf.U8Str;
			}
		}

        if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
        {
            if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
            {
                for(i=0; i<length; i++)
                {
                    msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu8Str[i], &BBox_X_Info);
                    width += BBox_X_Info.u8Width;
                }
            }
            else //(pclrBtn->bStringIndexWidth == CHAR_IDX_2BYTE)
            {
                //pu16Str = (U16*)pu8Str;

	#if (ENABLE_ARABIC_OSD)
					//For Subtitle
					if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || (External Subtitle Language is Arabic) */ ) )
					{
						U16 u16Unicode, u16Unicode2;

						for(i=0;i<length;i++)
						{
							// Check if it is in Arabic Vowel unicode range
							u16Unicode2 = (pu8Str[i*2+1]<<8) + pu8Str[i*2];

							if ( i > 0 )
							{
								u16Unicode = (pu8Str[i*2-1]<<8) + pu8Str[i*2-2];

								//if ( msAPI_OSD_IsArabicVowel( pu16Str[i-1] ) )
								if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
									width -= BBox_X_Info.u8Width;
									//msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[i], &BBox_X_Info);
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode2, &BBox_X_Info);
									width += BBox_X_Info.u8Width;
								}
								else
								{
									//msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[i], &BBox_X_Info);
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode2, &BBox_X_Info);
									width += BBox_X_Info.u8Width;
								}
							}
							else
							{
								//msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[i], &BBox_X_Info);
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode2, &BBox_X_Info);
								width += BBox_X_Info.u8Width;
							}
						}
					}
					else if ( bArabicDisplay ) //for OSD
					{
						U16 u16Unicode;

                for(i=0; i<length; i++)
                {
							u16Unicode = (pu8Str[i*2+1]<<8) + pu8Str[i*2];

							// Check if it is in Arabic Vowel unicode range
							if ( i > 0 )
							{
								//if ( msAPI_OSD_IsArabicVowel( pu16Str[i] ) )
								if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
								{
										//skip current vowel character width
										//do nothing here
								}
								else
								{
									//msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[i], &BBox_X_Info);
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
									width += BBox_X_Info.u8Width;
								}
							}
							else
							{
								//msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[i], &BBox_X_Info);
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
                    width += BBox_X_Info.u8Width;
                }
            }
					}
					else
	#endif
	#if (ENABLE_THAI_OSD)
					if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
					{
						U16 u16Unicode;

						for(i=0;i<length;i++)
						{
							// Check Primary Thai unicode range
							u16Unicode = (pu8Str[i*2+1]<<8) + pu8Str[i*2];

							if ( ( u16Unicode <= 0x0E5B ) && ( u16Unicode >= 0x0E01 ) )
							{
								if ( MinusDrawTable[ u16Unicode - 0x0E01 ] )
								{
									//Minus coordinate, don't add width
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
									width += BBox_X_Info.u8Width;
								}
							}
							else if ( ( u16Unicode <= 0xF71A ) && ( u16Unicode >= 0xF700 ) )
							{
								if ( MinusDrawTablePersentation[ u16Unicode - 0xF700 ] )
								{
									//Minus coordinate, don't add width
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
									width += BBox_X_Info.u8Width;
								}
							}
							else
							{
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
								width += BBox_X_Info.u8Width;
							}
						}
					}
					else
	#endif
					{
                        U16 u16Unicode;

                        for(i=0; i<length; i++)
                        {
                            u16Unicode = (pu8Str[i*2+1]<<8) + pu8Str[i*2];

                            msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
                            width += BBox_X_Info.u8Width;
                        }
                    }
                }

	#if (ENABLE_ARABIC_OSD)
			if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
			{
				pclrBtn->Fontfmt.ifont_gap = 0;
			}
	#endif

            if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
            {
                width += ((length-1) * pclrBtn->Fontfmt.ifont_gap);
            }
        }
        else
        {
            msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);
            width = length * finfo.width;
        }
    }

      msAPI_OSD_Free_resource();

    return width;
}



#if 1
/******************************************************************************/
/// API for draw line::
/// @param p_clr_line \b IN line color
/******************************************************************************/
void msAPI_OSD_DrawLine(OSDClrLine *p_clr_line)
{

#if GE_DRIVER_TEST

    GELineRect.LineRectCoordinate.v0_x = p_clr_line->x1;
    GELineRect.LineRectCoordinate.v0_y = p_clr_line->y1;
    GELineRect.LineRectCoordinate.v1_x = p_clr_line->x2;
    GELineRect.LineRectCoordinate.v1_y = p_clr_line->y2;
    GELineRect.Gradient_Color_Flag     = GECOLOR_FLAG_CONSTANT;
    GELineRect.ARGBClr.ForeGroundColor.u32Num
    = (p_clr_line->u32LineColor&0xff00ff00) + ((p_clr_line->u32LineColor&0xff)<<16) +((p_clr_line->u32LineColor>>16)&0xff);
    MDrv_GE_LineDraw(&GELineRect);

#else

    GFX_DrawLineInfo pline;

    msAPI_OSD_GET_resource();
    pline.x1 = p_clr_line->x1;
    pline.x2 = p_clr_line->x2;
    pline.y1 = p_clr_line->y1;
    pline.y2 = p_clr_line->y2;
    memcpy(&pline.colorRange.color_s, &p_clr_line->u32LineColor , 4);
    memcpy(&pline.colorRange.color_e, &p_clr_line->u32LineColor , 4);

    pline.flag = 0;
//  memcpy (&clrline, p_clr_line, sizeof(GEClrLine));
//    lfmt.width = p_clr_line->u8LineWidth;
//    MDrv_GE_LineTo(&clrline, &lfmt);
    pline.fmt = GFX_FMT_RGB565;
    pline.width = p_clr_line->u8LineWidth;

    MApi_GFX_DrawLine(&pline);
    msAPI_OSD_Free_resource();

#endif

}
#else
/******************************************************************************/
/// API for draw line::
/// @param pclrBtn \b IN color
/******************************************************************************/
void msAPI_OSD_DrawLine(OSDClrBtn *pclrBtn)
{
/*
    if((pclrBtn->x + pclrBtn->width) > NEWUI_MAIN_MENU_GWIN_WIDTH)
    {
        return;
    }
    else if((pclrBtn->y + pclrBtn->height) > NEWUI_MAIN_MENU_GWIN_HEIGHT)
    {
        return;
    }
*/
#if GE_DRIVER_TEST

    GELineRect.LineRectCoordinate.v0_x = pclrBtn->x;
    GELineRect.LineRectCoordinate.v0_y = pclrBtn->y;
    GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width;
    GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height;
    GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_CONSTANT;
    GELineRect.ARGBClr.ForeGroundColor.u32Num
    = (pclrBtn->b_clr&0xff00ff00) + ((pclrBtn->b_clr&0xff)<<16) +((pclrBtn->b_clr>>16)&0xff);
    MDrv_GE_LineDraw(&GELineRect);

#else

    clrline.x1 = pclrBtn->x;
    clrline.y1 = pclrBtn->y;
    clrline.x2 = pclrBtn->x + pclrBtn->width;
    clrline.y2 = pclrBtn->y + pclrBtn->height;
    lfmt.width = pclrBtn->bFrameWidth;
    memcpy(&clrline.clr, &pclrBtn->b_clr, 4);
    MDrv_GE_LineTo(&clrline, &lfmt);
#endif

}
#endif
/*****************************************************************************************************/

#if ENABLE_DEFAULT_KERNING
static U8 kerningX_(U8 char1, U8 char2)
{
    U8 i;

    if (char1 == 0 || char2 == 0 || char1 > 'Z')
    {
        return 0;
    }

    for (i = 0; i < kerningTblSize; i++)
    {
        if (char1 == kerningTable[i].char1 && char2 == kerningTable[i].char2)
        {
            return kerningTable[i].kVal;
        }
    }
    return 0;
}
#endif //ENABLE_DEFAULT_KERNING

#if ( ENABLE_ARABIC_OSD || ENABLE_THAI_OSD )
void msAPI_OSD_SetOSDLanguage(EN_OSDAPI_LANGUAGE eLanguage)
{
	enLanguage = eLanguage;
}

void msAPI_OSD_Set_H_Mirror(BOOLEAN bEnableMirror)
{
	static U8 bMirrorStatus = FALSE;

	if ( bMirrorStatus != bEnableMirror )
	{
		MApi_GOP_GWIN_SetHMirror(bEnableMirror);
		bMirrorStatus = bEnableMirror;
	}
}
#endif

#if (ENABLE_ARABIC_OSD)

#define PURE_ARABIC_CHAR    0
#define ASCII_MARK_CHAR     1
#define PURE_ENGNUM_CHAR    2

static U8 msAPI_OSD_IsNotInArabicHebrew(U16 inputc)
{
    // Arabic Characters unicode range:
    // 0600~ 06FF
    // 0750~ 077F
    // FB50~ FDFF
    // FE70~ FEFF

    // Hebrew Characters unicode range:
    // 0590~ 05FF
    // FB00~ FB4F   not support here

    if ( inputc < 0x0030 )
        return ASCII_MARK_CHAR;

    if ( inputc < 0x003A )  // 0~9
        return PURE_ENGNUM_CHAR;

    if ( inputc < 0x0041 )
        return ASCII_MARK_CHAR;

    if ( inputc < 0x005B )  // A~Z
        return PURE_ENGNUM_CHAR;

    if ( inputc < 0x0061 )
        return ASCII_MARK_CHAR;

    if ( inputc < 0x007B )  // a~z
        return PURE_ENGNUM_CHAR;

    if ( inputc < 0x0590 )
        return ASCII_MARK_CHAR;

    if ( ( inputc > 0x06FF ) && ( inputc < 0x0750 ) )
        return ASCII_MARK_CHAR;

    if ( ( inputc > 0x077F ) && ( inputc < 0xFB50 ) )
        return ASCII_MARK_CHAR;

    if ( ( inputc > 0xFDFF ) && ( inputc < 0xFE70 ) )
        return ASCII_MARK_CHAR;

    if ( inputc > 0xFEFF )
        return ASCII_MARK_CHAR;

    return PURE_ARABIC_CHAR;
}

static U8 msAPI_OSD_IsArabicVowel(U16 inputc)
{
    //Arabic Vowel characters presentation form
    //0xFE70 0xFE72 0xFE74 0xFE76 0xFE78 0xFE7A
    switch(inputc)
    {
    case 0xFE70:
    case 0xFE72:
    case 0xFE74:
    case 0xFE76:
    case 0xFE78:
    case 0xFE7A:
    case 0xFE7C:
    case 0xFE7E:
        return TRUE;
    default:
        return FALSE;
    }
}

static void msAPI_OSD_ReverseAllCharacters(U16 *inp, U16 *outp, U8 len)
{
    U8 i=0;

    while( (i<len) && ( inp[i] != 0x0000 ) )
    {
        outp[len-i-1] = inp[i];
        i++;
    }
}

static void msAPI_OSD_ASCII_MarkReplace( U16 inValue, U16 *outp, U8 i, U8 *indexp, U8 bMirrorLang )
{
	//case for other mirror language
	//dumy code here
    bMirrorLang = bMirrorLang;

    switch( inValue )
    {
    case 0x0028:
        *outp = 0x0029;
        indexp[0] = i;
        break;
    case 0x0029:
        *outp = 0x0028;
        indexp[1] = i;
        break;
    case 0x003C:
        *outp = 0x003E;
        indexp[2] = i;
        break;
    case 0x003E:
        *outp = 0x003C;
        indexp[3] = i;
        break;
    case 0x005B:
        *outp = 0x005D;
        indexp[4] = i;
        break;
    case 0x005D:
        *outp = 0x005B;
        indexp[5] = i;
        break;
    case 0x007B:
        *outp = 0x007D;
        indexp[6] = i;
        break;
    case 0x007D:
        *outp = 0x007B;
        indexp[7] = i;
        break;
/*
    case Other unicode pair 1:
        if ( bMirrorLang )
        {
            *outp = Other unicode pair 2;
            indexp[8] = i;
        }
        else
        {
            *outp = inValue;
        }
        break;
    case Other unicode pair 2:
        if ( bMirrorLang )
        {
            *outp = Other unicode pair 1;
            indexp[9] = i;
        }
        else
        {
            *outp = inValue;
        }
        break;
*/
    default:
        *outp = inValue;
        break;
    }
}

#define WINXP_COMPATIBILITY_MODE    FALSE

static void msAPI_OSD_ReverseNonArabicHebrewCharacters(U16 *inp, U16 *outp, U8 len)
{
    U8 i=0, j=0, k=0, nResult, nStartSearchPos, bMirrorLang = FALSE;
#if (!WINXP_COMPATIBILITY_MODE)
    U8 nResultNext = PURE_ARABIC_CHAR, nPureArabicPos = 0xFF, nPureEngPos = 0xFF, bASCIIFound = FALSE;
#endif
    U8 ASCII_Mark_Index[10];
    U8 ASCII_Mark_Index_InLoop[4];

	switch(enLanguage)
    {
    case LANGUAGE_OSD_ARABIC:
    case LANGUAGE_OSD_PARSI:
    case LANGUAGE_OSD_KURDISH:
    case LANGUAGE_OSD_HEBREW:
        bMirrorLang = TRUE;
        break;
    default:
        break;
    }
/*
    if ( bDivxSubtitleMode )
    {
        switch ( External Subtitle Language )
        {
        case LANGUAGE_OSD_HEBREW:
        case LANGUAGE_OSD_ARABIC:
            bMirrorLang = TRUE;
            break;
        }
    }
*/
    for( i = 0; i < len; i++ )
    {
        if ( msAPI_OSD_IsNotInArabicHebrew(inp[i]) == PURE_ARABIC_CHAR )
        {
            break;
        }
    }
    if ( i == len )
    {
        msAPI_OSD_ReverseAllCharacters(inp, outp, len);
/*
        if ( bMirrorLang )
        {
            for(j=0;j<len;j++)
            {
                switch(outp[j])
                {
                case Other unicode pair 1:
                    outp[j] = Other unicode pair 2;
                    break;
                case Other unicode pair 2:
                    outp[j] = Other unicode pair 1;
                    break;
                default:
                    break;
                }
            }
        }
*/
    }
    else
    {
        i = 0;
        memset(ASCII_Mark_Index, 0xFF, sizeof(ASCII_Mark_Index));

        while( (i<len) && ( inp[i] != 0x0000 ) )
        {
            if( ( (nResult = msAPI_OSD_IsNotInArabicHebrew(inp[i])) == PURE_ENGNUM_CHAR) || ( ((i+1) < len) && ( (inp[i] == 0x0025) || (inp[i] == 0x0022) ) && ( inp[i+1] >= 0x0030 ) && ( inp[i+1] <= 0x0039 ) ) )
            {
#if (!WINXP_COMPATIBILITY_MODE)

BackToReversePath:

                nStartSearchPos = i;

                if ( nResult == PURE_ENGNUM_CHAR )
                {
                    nPureEngPos = i;
                }
#endif

                j = 0;
                memset(ASCII_Mark_Index_InLoop, 0xFF, sizeof(ASCII_Mark_Index_InLoop));

                while( (i<len) && ((nResult = msAPI_OSD_IsNotInArabicHebrew(inp[i])) > PURE_ARABIC_CHAR) && ( inp[i] != 0x0000 ) )
                {
#if (!WINXP_COMPATIBILITY_MODE)
                    if ( nResult == PURE_ENGNUM_CHAR )
                    {
                        nPureEngPos = i;
                    }

                    if ( ( (i+1) < len ) && ( nResult == ASCII_MARK_CHAR ) && ( ( nResultNext = msAPI_OSD_IsNotInArabicHebrew(inp[i+1]) ) == PURE_ARABIC_CHAR ) )
#else
                    if ( ( (i+1) < len ) && ( nResult == ASCII_MARK_CHAR ) && ( ( msAPI_OSD_IsNotInArabicHebrew(inp[i+1]) == PURE_ARABIC_CHAR ) || ( inp[i+1] == 0x0020 ) ) )
#endif
                        break;

                    if ( ( i == (len-1) ) && ( nResult == ASCII_MARK_CHAR ) )
                    {
                        switch(inp[i])
                        {
                        case 0x0028:
                            //search 0x0029
                            if ( ( ASCII_Mark_Index_InLoop[0] < nStartSearchPos ) || ( ASCII_Mark_Index_InLoop[0] == 0xFF ) )
                            {
                                goto OutWhileNormal;
                            }
                            break;
                        case 0x003C:
                            //search 0x003E
                            if ( ( ASCII_Mark_Index_InLoop[1] < nStartSearchPos ) || ( ASCII_Mark_Index_InLoop[1] == 0xFF ) )
                            {
                                goto OutWhileNormal;
                            }
                            break;
                        case 0x005B:
                            //search 0x005D
                            if ( ( ASCII_Mark_Index_InLoop[2] < nStartSearchPos ) || ( ASCII_Mark_Index_InLoop[2] == 0xFF ) )
                            {
                                goto OutWhileNormal;
                            }
                            break;
                        case 0x007B:
                            //search 0x007D
                            if ( ( ASCII_Mark_Index_InLoop[3] < nStartSearchPos ) || ( ASCII_Mark_Index_InLoop[3] == 0xFF ) )
                            {
                                goto OutWhileNormal;
                            }
                            break;
                        default:
                            goto OutWhileNormal;
                        }
                    }

                    switch( inp[i] )
                    {
#if (!WINXP_COMPATIBILITY_MODE)
                    case 0x002E:
                        if ( ( (i+1) < len ) && ( ( inp[i+1] == 0x0020 ) || ( nResultNext == PURE_ARABIC_CHAR ) ) )
                        {
                            goto OutWhile;
                        }
                        break;
#endif
                    case 0x0020:
                    case 0x0028:
                    case 0x0029:
                    case 0x003C:
                    case 0x003E:
                    case 0x005B:
                    case 0x005D:
                    case 0x007B:
                    case 0x007D:
#if (!WINXP_COMPATIBILITY_MODE)
                        if ( nPureArabicPos == 0xFF )
                        {
#endif
                            k = i+1;
                            while( k < len )
                            {
                                if ( msAPI_OSD_IsNotInArabicHebrew(inp[k]) == PURE_ARABIC_CHAR )
                                {
#if (!WINXP_COMPATIBILITY_MODE)
                                    nPureArabicPos = k;
#endif
                                    break;
                                }
                                k++;
                            }
#if (!WINXP_COMPATIBILITY_MODE)
                        }
#endif
                        if ( ( k == (i+1)) && ( k != len ) )
                        {
                            goto OutWhile;
                        }
                        switch(inp[i])
                        {
                        case 0x0029:
                            if ( ASCII_Mark_Index_InLoop[0] == 0xFF )
                            {
                                ASCII_Mark_Index_InLoop[0] = i;
                            }
                            break;
                        case 0x003E:
                            if ( ASCII_Mark_Index_InLoop[1] == 0xFF )
                            {
                                ASCII_Mark_Index_InLoop[1] = i;
                            }
                            break;
                        case 0x005D:
                            if ( ASCII_Mark_Index_InLoop[2] == 0xFF )
                            {
                                ASCII_Mark_Index_InLoop[2] = i;
                            }
                            break;
                        case 0x007D:
                            if ( ASCII_Mark_Index_InLoop[3] == 0xFF )
                            {
                                ASCII_Mark_Index_InLoop[3] = i;
                            }
                            break;
                        }
                        break;

#if (!WINXP_COMPATIBILITY_MODE)
                    default:
                        if ( nResult == ASCII_MARK_CHAR )
                        {
                            if ( nPureArabicPos == 0xFF )
                            {
                                k = i+1;
                                while( k < len )
                                {
                                    if ( msAPI_OSD_IsNotInArabicHebrew(inp[k]) == PURE_ARABIC_CHAR )
                                    {
                                        nPureArabicPos = k;
                                        break;
                                    }
                                    k++;
                                }
                            }

                            bASCIIFound = TRUE;
                        }
                        break;
#endif
                    }

                    j++;
                    i++;
                }

OutWhileNormal:

#if (!WINXP_COMPATIBILITY_MODE)
                if ( ( bASCIIFound ) && ( i > (nPureEngPos+1) ) && ( nPureArabicPos != 0xFF ) )
                {
                    j -= (i - (nPureEngPos+1));
                    i = nPureEngPos+1;
                }
#endif

OutWhile:

                for(k=0;k<j;k++)
                {
                    outp[i-j+k] = inp[i-1-k];
                }
            }
            else
            {
#if (!WINXP_COMPATIBILITY_MODE)
                if ( ( !( ( i == 0 ) && ( inp[i] == 0x002E ) ) ) &&
                ( nResult != PURE_ARABIC_CHAR ) &&
                ( inp[i] != 0x0020 ) &&
                ( inp[i] != 0x0021 ) &&
                ((i+1) < len) &&
                (msAPI_OSD_IsNotInArabicHebrew(inp[i+1]) == PURE_ENGNUM_CHAR) )
                    goto BackToReversePath;
#endif
                if ( ( i == 0 ) || ( i == (len-1) ) )
                {
                    msAPI_OSD_ASCII_MarkReplace( inp[i], &(outp[i]), i, ASCII_Mark_Index, bMirrorLang );
                }
                else
                {
#if (!WINXP_COMPATIBILITY_MODE)
                    U8 bMarkChange = TRUE;

                    if ( (i > 2) && ( msAPI_OSD_IsNotInArabicHebrew(inp[i-2]) == PURE_ENGNUM_CHAR ) )
                    {
                        bMarkChange = FALSE;
                    }

                    if ( ( i > 1 ) && ( inp[i] != 0x0020 ) && (bMarkChange) )
#else
                    if ( ( i > 1 ) && ( inp[i] != 0x0020 ) )
#endif
                    {
                        switch( outp[i-1] )
                        {
                        case 0x0028:
                            if ( ASCII_Mark_Index[1] != (i-1) )
                            {
                                outp[i-1] = 0x0029;
                                ASCII_Mark_Index[0] = i-1;
                            }
                            break;
                        case 0x0029:
                            if ( ASCII_Mark_Index[0] != (i-1) )
                            {
                                outp[i-1] = 0x0028;
                                ASCII_Mark_Index[1] = i-1;
                            }
                            break;
                        case 0x003C:
                            if ( ASCII_Mark_Index[3] != (i-1) )
                            {
                                outp[i-1] = 0x003E;
                                ASCII_Mark_Index[2] = i-1;
                            }
                            break;
                        case 0x003E:
                            if ( ASCII_Mark_Index[2] != (i-1) )
                            {
                                outp[i-1] = 0x003C;
                                ASCII_Mark_Index[3] = i-1;
                            }
                            break;
                        case 0x005B:
                            if ( ASCII_Mark_Index[5] != (i-1) )
                            {
                                outp[i-1] = 0x005D;
                                ASCII_Mark_Index[4] = i-1;
                            }
                            break;
                        case 0x005D:
                            if ( ASCII_Mark_Index[4] != (i-1) )
                            {
                                outp[i-1] = 0x005B;
                                ASCII_Mark_Index[5] = i-1;
                            }
                            break;
                        case 0x007B:
                            if ( ASCII_Mark_Index[7] != (i-1) )
                            {
                                outp[i-1] = 0x007D;
                                ASCII_Mark_Index[6] = i-1;
                            }
                            break;
                        case 0x007D:
                            if ( ASCII_Mark_Index[6] != (i-1) )
                            {
                                outp[i-1] = 0x007B;
                                ASCII_Mark_Index[7] = i-1;
                            }
                            break;
/*
                        case Other unicode pair 1:
                            if ( ( ASCII_Mark_Index[9] != (i-1) ) && ( bMirrorLang ) )
                            {
                                outp[i-1] = Other unicode pair 2;
                                ASCII_Mark_Index[8] = i-1;
                            }
                            break;
                        case Other unicode pair 2:
                            if ( ( ASCII_Mark_Index[8] != (i-1) ) && ( bMirrorLang ) )
                            {
                                outp[i-1] = Other unicode pair 1;
                                ASCII_Mark_Index[9] = i-1;
                            }
                            break;
*/
                        }
                    }

                    msAPI_OSD_ASCII_MarkReplace( inp[i], &(outp[i]), i, ASCII_Mark_Index, bMirrorLang );
                }
                i++;

                if ( ( i < len ) && ( outp[i-1] != 0x0020 ) )
                {
                    switch( inp[i] )
                    {
                    case 0x0028:
                        outp[i] = 0x0029;
                        ASCII_Mark_Index[0] = i;
                        i++;
                        break;
                    case 0x0029:
                        outp[i] = 0x0028;
                        ASCII_Mark_Index[1] = i;
                        i++;
                        break;
                    case 0x003C:
                        outp[i] = 0x003E;
                        ASCII_Mark_Index[2] = i;
                        i++;
                        break;
                    case 0x003E:
                        outp[i] = 0x003C;
                        ASCII_Mark_Index[3] = i;
                        i++;
                        break;
                    case 0x005B:
                        outp[i] = 0x005D;
                        ASCII_Mark_Index[4] = i;
                        i++;
                        break;
                    case 0x005D:
                        outp[i] = 0x005B;
                        ASCII_Mark_Index[5] = i;
                        break;
                    case 0x007B:
                        outp[i] = 0x007D;
                        ASCII_Mark_Index[6] = i;
                        break;
                    case 0x007D:
                        outp[i] = 0x007B;
                        ASCII_Mark_Index[7] = i;
                        break;
/*
                    case Other unicode pair 1:
                        if ( bMirrorLang )
                        {
                            outp[i] = Other unicode pair 2;
                            ASCII_Mark_Index[8] = i;
                        }
                        break;
                    case Other unicode pair 2:
                        if ( bMirrorLang )
                        {
                            outp[i] = Other unicode pair 1;
                            ASCII_Mark_Index[9] = i;
                        }
                        break;
*/
                    }
                }
            }
        }
    }
}

static void msAPI_OSD_KurdishFilter(U16 *inp, U8 len)
{
    #define INITIAL 0
    #define MEDIAL  1
    #define FINAL   2

    U8 i, bPrev = 0, bNext = 0, KurdishIndex;
    U8 CharAttribute[STROSD];
    static code U16 UnicodeArray[5][3] =
    {
    //  initial, medial, final
    //0x06B5
        { 0xFBBA, 0xFBBB, 0xFBBC },
    //0x06B6
        { 0xFBBD, 0xFBBE, 0xFBBF },
    //0x06B7
        { 0xFBC0, 0xFBC1, 0xFBC2 },
    //0x06CE
        { 0xFBC3, 0xFBC4, 0xFBC5 },
    //0x06A4
        { 0xFBC6, 0xFBC7, 0xFBC8 }
    };

    for(i=0;i<len;i++)
    {
        if ( msAPI_OSD_IsNotInArabicHebrew(inp[i]) == PURE_ARABIC_CHAR )
        {
            CharAttribute[i] = 1;
        }
        else
        {
            CharAttribute[i] = 0;
        }
    }

    for(i=0;i<len;i++)
    {
        switch(inp[i])
        {
        case 0x06B5:
        case 0x06B6:
        case 0x06B7:
        case 0x06CE:
        case 0x06A4:
            switch(inp[i])
            {
            case 0x06CE:
              KurdishIndex = 3;
              break;
            case 0x06A4:
              KurdishIndex = 4;
              break;
            default:
              KurdishIndex = inp[i] - 0x06B5;
              break;
            }

            if (i == 0)
            {
                if ( CharAttribute[i+1] )
                {
                    //It is initial, do shape
                    inp[i] = UnicodeArray[KurdishIndex][INITIAL];
                }
                else
                {
                    //It is isolated, do nothing
                }
            }
            else if (i == (len-1))
            {
                if ( CharAttribute[i-1] )
                {
                    //It is final, do shape
                    inp[i] = UnicodeArray[KurdishIndex][FINAL];
                }
                else
                {
                    //It is isolated, do nothing
                }
            }
            else
            {
                bPrev = 0;
                bNext = 0;

                if ( CharAttribute[i-1] )
                {
                    //It has previous character
                    bPrev = 1;
                }
                if ( CharAttribute[i+1] )
                {
                    //It has next character
                    bNext = 1;
                }
                if ( ( bPrev ) && ( bNext ) )
                {
                    //It is medial, do shape
                    inp[i] = UnicodeArray[KurdishIndex][MEDIAL];
                }
                else if ( bPrev )
                {
                    //It is final, do shape
                    inp[i] = UnicodeArray[KurdishIndex][FINAL];
                }
                else if ( bNext )
                {
                    //It is initial, do shape
                    inp[i] = UnicodeArray[KurdishIndex][INITIAL];
                }
                else
                {
                    //It is isolated, do nothing
                }
            }
            break;

        default:
            break;
        }
    }
}

void msAPI_OSD_EnterDivxSubtitleMode(BOOLEAN bDivxSubtitle)
{
    bDivxSubtitleMode = bDivxSubtitle;
}

BOOLEAN msAPI_OSD_GetDivxSubtitleMode(void)
{
    return bDivxSubtitleMode;
}
/*
void msAPI_OSD_DisplayDivxSubtitleText(BOOLEAN bTextMode)
{
    bDivxSubtitleText = bTextMode;
}
*/

// for mixed language display usage in OSD language selection screen
void msAPI_OSD_SetHebrewLangMixedMode(BOOLEAN bHebrewMixed)
{
    bHebrewMixedMode = bHebrewMixed;
}

#endif

/******************************************************************************/
/// API for draw text::
/// @param font_handle \b IN font handle
/// @param pu8Str \b IN pointer to string
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawText(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn)
{
    U16 i, len, u16StrWidth;
    U16 scale;
    OSD_RESOURCE_FONT_INFO finfo;
    GFX_TextOutInfo text_info;
    OSDSRC_GLYPH_DISP_INFO *pGlyphDispInfo=NULL;
    //U8  TempStr[STROSD*2];

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringType TempBuf;
	OSDStringTypeDouble outbuf;
	#if ( ENABLE_ARABIC_OSD )
		OSDStringType out;
		U8 bMirrorLang = FALSE;
	#endif
    U8 bArabicDisplay = FALSE;
    U8 bHebrew = FALSE;
	#if ( ENABLE_THAI_OSD )
		U8 bThaiDisplay = FALSE;
	#endif
    U16 nFontID;
#endif

     msAPI_OSD_GET_resource();

    /* check range */
    /*if((pclrBtn->x + pclrBtn->width) > NEWUI_MAIN_MENU_GWIN_WIDTH)
    {
        return;
    }
    else if((pclrBtn->y + pclrBtn->height) > NEWUI_MAIN_MENU_GWIN_HEIGHT)
    {
        return;
    }
    else*/ if((len = msAPI_OSD_GetStrLength(pu8Str, pclrBtn->bStringIndexWidth)) == 0)
    {
        msAPI_OSD_Free_resource();
        return;
    }

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

	switch(enLanguage)
    {
        case LANGUAGE_OSD_ARABIC:
        case LANGUAGE_OSD_PARSI:
        case LANGUAGE_OSD_KURDISH:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
					msAPI_OSD_Set_H_Mirror(FALSE);
					//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
                }
                else
            */
                {
                    if ( bHebrewMixedMode )
					{
                        bHebrew = TRUE;
					}
                    else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
					{
                        font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;
					}

                    if ( !bDivxSubtitleMode )
                    {
                        pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                        msAPI_GE_SetMirror(1, 0);    //patch GE driver
						msAPI_OSD_Set_H_Mirror(TRUE);
						//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), TRUE);
                    }
                    else
                    {
						msAPI_OSD_Set_H_Mirror(FALSE);
						//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
                    }
                }
            }

            if ( bHebrew == FALSE )
			{
                bArabicDisplay = TRUE;
			}
            break;
        case LANGUAGE_OSD_HEBREW:
            bHebrew = TRUE;
            if ( !bDivxSubtitleMode )
            {
                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                msAPI_GE_SetMirror(1, 0);    //patch GE driver
				msAPI_OSD_Set_H_Mirror(TRUE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), TRUE);
            }
            else
            {
                msAPI_OSD_Set_H_Mirror(FALSE);
            }
            break;

#if ( ENABLE_THAI_OSD )
        case LANGUAGE_OSD_THAI:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_THAI_0].fHandle;
                }
            }
	#if ( ENABLE_ARABIC_OSD )
            if ( bHebrew == FALSE )
	#endif
            {
                bThaiDisplay = TRUE;
				msAPI_OSD_Set_H_Mirror(FALSE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            }
            break;
#endif
        /*
        case LANGUAGE_OSD_CHINESE:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
                    {
                        font_handle = Font[nFontID+18].fHandle;
                    }
                    else if ( nFontID >= FONT_EXT_BEGIN )
                    {
                        //Do Chinese icon unicode conversion here
                    }
                }
            }
            if ( bHebrew == FALSE )
            {
				msAPI_OSD_Set_H_Mirror(FALSE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            }
            break;
        */
        default:
			msAPI_OSD_Set_H_Mirror(FALSE);
			//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            break;
    }
#endif

#if (ENABLE_ARABIC_OSD)
    if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
    {
        bMirrorLang = TRUE;
    }
#endif

    if(pclrBtn->bStringIndexWidth==CHAR_IDX_1BYTE)
    {
        if (len >= sizeof(TempStr.U8Str))
        {
            len = sizeof(TempStr.U8Str) - 1;
        }
    #if (ENABLE_ARABIC_OSD)
        if ( bMirrorLang )
        {
            memcpy(TempBuf.U8Str, pu8Str, len);
            for(i = 0; i < len; i++)
            {
                TempStr.U8Str[i] = TempBuf.U8Str[(len-1)-i];
            }
        }
        else
        {
    #endif

        memcpy(TempStr.U8Str,pu8Str,len);

    #if (ENABLE_ARABIC_OSD)
        }
    #endif
        TempStr.U8Str[len] = 0;
    }
    else
    {
        if (len >= sizeof(TempStr.U8Str)/2)
        {
            len = sizeof(TempStr.U8Str)/2 - 1;
        }
    #if (!ENABLE_ARABIC_OSD)
        memcpy(TempStr.U8Str,pu8Str,len*2);
        //((U16 *)TempStr)[len] = 0;
        //TempStr[len*2] = TempStr[len*2+1] = 0;
		TempStr.U16Str[len] = 0;
    #else
        if ( bMirrorLang )
        {
            U8 bReverse = TRUE;

            memcpy(TempStr.U8Str,pu8Str,len*2);
            //((U16 *)TempStr)[len] = 0;
			TempStr.U16Str[len] = 0;

            //For Subtitle
            if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
            {
                S32 errorCode = U_ZERO_ERROR;

                ArabicParser(TempStr.U16Str, len, outbuf.U16Str, STROSD,
                         U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
                         &errorCode);
				//Every language is possible, you can't skip this filter here
                msAPI_OSD_KurdishFilter(outbuf.U16Str, len);
                msAPI_OSD_ReverseAllCharacters(outbuf.U16Str, out.U16Str, len);
                msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, TempStr.U16Str, len);
                bReverse = FALSE;
            }
            else if ( bArabicDisplay ) //for OSD
            {
                S32 errorCode = U_ZERO_ERROR;

                ArabicParser(TempStr.U16Str, len, outbuf.U16Str, STROSD,
                         U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
                         &errorCode);
				//just skip this function for non Kurdish condition
                if ( enLanguage == LANGUAGE_OSD_KURDISH )
                {
                    msAPI_OSD_KurdishFilter(outbuf.U16Str, len);
                }
                msAPI_OSD_ReverseAllCharacters(outbuf.U16Str, out.U16Str, len);
                msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, outbuf.U16Str, len);
            }
            else    // for Hebrew
            {

                if ( bDivxSubtitleMode )    // Divx subtitle
                {
                    msAPI_OSD_ReverseAllCharacters(TempStr.U16Str, outbuf.U16Str, len);
                    msAPI_OSD_ReverseNonArabicHebrewCharacters(outbuf.U16Str, out.U16Str, len);
                    msAPI_OSD_ReverseAllCharacters(out.U16Str, outbuf.U16Str, len);
                }
                else if ( bHebrew )         // Hebrew OSD display
                {
                    msAPI_OSD_ReverseAllCharacters(TempStr.U16Str, out.U16Str, len);
                    msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, outbuf.U16Str, len);
                }
                else                        // Other conditions
                {
                    msAPI_OSD_ReverseAllCharacters(TempStr.U16Str, outbuf.U16Str, len);
                    msAPI_OSD_ReverseNonArabicHebrewCharacters(outbuf.U16Str, out.U16Str, len);
                    msAPI_OSD_ReverseAllCharacters(out.U16Str, outbuf.U16Str, len);
                }
            }

            if ( bReverse )
            {
                for(i=0;i<len;i++)
                {
                    TempStr.U16Str[i] = *(outbuf.U16Str+len-i-1);
                }
            }
        }
        else
        {
			memcpy(TempStr.U8Str,pu8Str,len*2);
        }

        //((U16 *)TempStr)[len] = 0;
		TempStr.U16Str[len] = 0;
    #endif
    }

    msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);

    scale = 100;

    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = ((U16)pclrBtn->Fontfmt.height*100 / (U16)finfo.height);
        pclrBtn->Fontfmt.height = scale * finfo.height /100;

        scale = (U8)((U16)pclrBtn->Fontfmt.width*100 / (U16)finfo.width);
        pclrBtn->Fontfmt.width = scale * finfo.width /100;
    }

#if (ENABLE_THAI_OSD)
    if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
    {
        if(pclrBtn->bStringIndexWidth == CHAR_IDX_2BYTE)
        {
            //pu16Str = (U16 *)pu8Str;
			//temp solution for compiler warning
			memcpy(TempBuf.U8Str, pu8Str, len*2);

            len = Thai_compose(TempBuf.U16Str, 0, len, STROSD, outbuf.U16Str);
            outbuf.U16Str[len] = 0x0000;

            pu8Str = outbuf.U8Str;
        }
    }
#endif

    pGlyphDispInfo = msAPI_Memory_Allocate( sizeof(OSDSRC_GLYPH_DISP_INFO) * len,BUF_ID_GLYPH_DISP_INFO);
    if(pGlyphDispInfo == NULL)
    {
        __ASSERT(0);
        APIOSD_DBG(printf("E_DrawText>> pGlyphDispInfo == NULL\n"));
        msAPI_OSD_Free_resource();
        return;
    }

    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
        if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
        {
            for (i=0; i<len; i++)
            {
            #if (ENABLE_ARABIC_OSD)
                if ( bMirrorLang )
                    msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, TempStr.U8Str[i], &pGlyphDispInfo[i]);
                else
            #endif
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, pu8Str[i], &pGlyphDispInfo[i]);
                pGlyphDispInfo[i].u8Width = pGlyphDispInfo[i].u8Width * scale / 100;
            }
        }
        else
        {
			U8 *pu8TempPtr;

#if (ENABLE_ARABIC_OSD)
            if ( bMirrorLang )
			{
                //pu16Str = (U16*) TempStr;
				pu8TempPtr = TempStr.U8Str;
			}
            else
#endif
			{
            //pu16Str = (U16*) pu8Str;
				pu8TempPtr = pu8Str;
			}
            for (i=0; i<len; i++)
            {
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, (pu8TempPtr[i*2+1]<<8) + pu8TempPtr[i*2], &pGlyphDispInfo[i]);
                pGlyphDispInfo[i].u8Width = (pGlyphDispInfo[i].u8Width * scale
#if ENABLE_DEFAULT_KERNING
                    //- kerningX_(pu16Str[i], pu16Str[i+1])
                    - kerningX_((pu8TempPtr[i*2+1]<<8) + pu8TempPtr[i*2], (pu8TempPtr[i*4+1]<<8) + pu8TempPtr[i*4])
#endif //ENABLE_DEFAULT_KERNING
                    ) / 100;
            }

        #if (ENABLE_ARABIC_OSD)
            //For Subtitle, if OSD is not mirrored status, go this way, the same with subtitle
  		    if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || (External Subtitle Language is Arabic) */ ) )
            {
				U16 u16Unicode;

                for(i=0;i<len;i++)
                {
                    // Check if it is in Arabic Vowel unicode range
                    if ( i > 0 )
                    {
						u16Unicode = (pu8TempPtr[i*2-1]<<8) + pu8TempPtr[i*2-2];											
                        //if ( msAPI_OSD_IsArabicVowel( pu16Str[i-1] ) )
						if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
                        {
                            pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                    }
                    else
                    {
                        //coordinate no need change
                    }
				}
            }
            else if ( bArabicDisplay ) //for OSD
			{
				U16 u16Unicode;

                for(i=0;i<len;i++)
                {
                    // Check if it is in Arabic Vowel unicode range
                    if ( i > 0 )
                    {
						u16Unicode = (pu8TempPtr[i*2+1]<<8) + pu8TempPtr[i*2];						
                        //if ( msAPI_OSD_IsArabicVowel( pu16Str[i] ) )
						if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
                        {
							S8 nOffset = pGlyphDispInfo[i-1].u8Width - pGlyphDispInfo[i].u8Width;

							if ( nOffset > 0 )
								pGlyphDispInfo[i].overlap_info.s8Xoffset = nOffset;
							else
                                pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                    }
                    else
                    {
                        //coordinate no need change
                    }
                }
			}
			else
        #endif
        #if (ENABLE_THAI_OSD)
            if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
            {
                static U8 prevState = 0;
				U16 u16Unicode;

                prevState = 0;

                for(i=0;i<len;i++)
                {
                    // Check Primary Thai unicode range
					u16Unicode = (pu8TempPtr[i*2+1]<<8) + pu8TempPtr[i*2];
                    if ( ( u16Unicode <= 0x0E5B ) && ( u16Unicode >= 0x0E01 ) )
                    {
                        if ( MinusDrawTable[ u16Unicode - 0x0E01 ] )
                        {
                            if ( i > 0 )
                            {
								u16Unicode = (pu8TempPtr[i*2-1]<<8) + pu8TempPtr[i*2-2];
                                if ( ( prevState == 1 ) && ( MinusDrawTable[ u16Unicode - 0x0E01 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else if ( ( prevState == 2 ) && ( MinusDrawTablePersentation[ u16Unicode - 0xF700 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-1].u8X0 + pGlyphDispInfo[i-1].u8Width + 1) - (finfo.width);
                            }
                            else
                            {
                                pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            }

                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                        else
                        {
                            //coordinate no need change
                        }

                        prevState = 1;
                    }
                    else if ( ( u16Unicode <= 0xF71A ) && ( u16Unicode >= 0xF700 ) )
                    {
                        if ( MinusDrawTablePersentation[ u16Unicode - 0xF700 ] )
                        {
                            if ( i > 0 )
                            {
								u16Unicode = (pu8TempPtr[i*2-1]<<8) + pu8TempPtr[i*2-2];								
                                if ( ( prevState == 1 ) && ( MinusDrawTable[ u16Unicode - 0x0E01 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else if ( ( prevState == 2 ) && ( MinusDrawTablePersentation[ u16Unicode - 0xF700 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-1].u8X0 + pGlyphDispInfo[i-1].u8Width + 1) - (finfo.width);
                            }
                            else
                            {
                                pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            }

                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                        else
                        {
                            //coordinate no need change
                        }

                        prevState = 2;
                    }
                    else
                    {
                        //coordinate no need change
                        prevState = 0;
                    }
                }
            }
        #endif
            {
            }
        }
    }
    else
    {
        if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
        {
            for (i=0; i<len; i++)
            {
            #if (ENABLE_ARABIC_OSD)
                if ( bMirrorLang )
                    msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, TempStr.U8Str[i], &pGlyphDispInfo[i]);
                else
            #endif
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, pu8Str[i], &pGlyphDispInfo[i]);
                MS_DEBUG_MSG(printf("3 %x:%x\n", pu8Str[i], pGlyphDispInfo[i].u16Index));
            }
        }
        else
        {
			U8 *pu8TempPtr;

#if (ENABLE_ARABIC_OSD)
            if ( bMirrorLang )
			{
                //pu16Str = (U16*) TempStr;
				pu8TempPtr = TempStr.U8Str;
			}
            else
#endif
			{
            //pu16Str = (U16*) pu8Str;
				pu8TempPtr = pu8Str;
			}
            for (i=0; i<len; i++)
            {
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, (pu8TempPtr[i*2+1]<<8) + pu8TempPtr[i*2], &pGlyphDispInfo[i]);
                MS_DEBUG_MSG(printf("4 %x:%x\n", pu8Str[i], pGlyphDispInfo[i].u16Index));
            }
        }
    }

    u16StrWidth = 0;
    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
        for (i=0; i< len; i++)
        {
            u16StrWidth += pGlyphDispInfo[i].u8Width;
        }

    #if (ENABLE_ARABIC_OSD)
        if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
        {
            pclrBtn->Fontfmt.ifont_gap = 0;
        }
    #endif

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            u16StrWidth += ((len-1) * pclrBtn->Fontfmt.ifont_gap);
        }
    }
    else  // gap/compact + italic
    {
        // italic or default
        u16StrWidth = finfo.width * scale * len / 100;

        // with gap or compact, only one of two
        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            u16StrWidth +=  ((len-1) * pclrBtn->Fontfmt.ifont_gap);
        }
        else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
        {
            u16StrWidth -=  ((len-1) * pclrBtn->Fontfmt.dis);
        }
    }

    /* draw string */
    switch(pclrBtn->enTextAlign)
    {
        case EN_ALIGNMENT_DEFAULT: //without BTN_TEXT_GAP
            text_info.dstblk.x = pclrBtn->x;
            break;

        case EN_ALIGNMENT_LEFT:
            text_info.dstblk.x = pclrBtn->x + BTN_TEXT_GAP;
            break;

        case EN_ALIGNMENT_RIGHT:
            if (u16StrWidth + BTN_TEXT_GAP > pclrBtn->width)
                text_info.dstblk.x = pclrBtn->x;
            else
                text_info.dstblk.x = pclrBtn->x + pclrBtn->width - BTN_TEXT_GAP - u16StrWidth;
            break;

        case EN_ALIGNMENT_CENTER:
            if (u16StrWidth > pclrBtn->width)
                text_info.dstblk.x = pclrBtn->x;
            else
                text_info.dstblk.x = pclrBtn->x + (pclrBtn->width - u16StrWidth) / 2;
            break;
    }

    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        text_info.dstblk.y = pclrBtn->y + (S16)(pclrBtn->height - finfo.height * scale / 100) / 2;
    }
    else
    {
        text_info.dstblk.y = pclrBtn->y + (S16)(pclrBtn->height - finfo.height) / 2;
    }
    if (text_info.dstblk.y > 32767)
    {
        //overflow
        text_info.dstblk.y = 0;
    }
    #if 0
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        S16 s16Tmp;
        s16Tmp = ((S16)pclrBtn->height - finfo.height * scale / 100) / 2;
        s16Tmp = pclrBtn->y + s16Tmp;
        if (s16Tmp < 0)
        {
            s16Tmp = 0;
        }
        cpoint.y = s16Tmp;
    }
    else
    {
        S16 s16Tmp;
        s16Tmp = ((S16)pclrBtn->height - finfo.height) / 2;
        s16Tmp = pclrBtn->y + s16Tmp;
        if (s16Tmp < 0)
        {
            s16Tmp = 0;
        }
        cpoint.y = s16Tmp;
    }
    #endif

//    memcpy(&cpoint.clr, &pclrBtn->t_clr, 4);

#if GE_DRIVER_TEST
    FontInfo.FontCoordinate.v0_x =  cpoint.x;
    FontInfo.FontCoordinate.v0_y =  cpoint.y;

    FontInfo.FontCoordinate.width = pclrBtn->Fontfmt.width;
    FontInfo.FontCoordinate.height = pclrBtn->Fontfmt.height;

    FontInfo.Font_SCK_Flag = GESCK_FLAG_SELE_TURNON | GESCK_FLAG_SELE_FOREGROUND;
    FontInfo.Font_Bitblt_Flag = GE_I1_FONT_BITBLT | GE_FONT_FLAG_VARWIDTH | GE_FONT_FLAG_GAP;
    FontInfo.BasicFontInfo.ifont_gap = 2;
    FontInfo.ARGBClr.ForeGroundColor.u32Num
    = (pclrBtn->t_clr&0xff00ff00) + ((pclrBtn->t_clr&0xff)<<16) +((pclrBtn->t_clr>>16)&0xff);
    //FontInfo.ARGBClr.ForeGroundColor.u32Num = (pclrBtn->t_clr&0xff00);

    FontInfo.ARGBClr.BackGroundColor.u32Num = 0;
    FontInfo.BasicFontInfo.fhandle = font_handle;
    FontInfo.Font_Type_Flag          = GEFONT_OSD_TYPE;
     #if 1
      MDrv_GE_FontBitBlt(&FontInfo, pGlyphDispInfo, TempStr.U8Str,pclrBtn->bStringIndexWidth);
     #else
    MDrv_GE_FontBitBlt(&FontInfo,fontAttr, pu8Str,pclrBtn->bStringIndexWidth);
     #endif
    PESet.PE_Enable_Flag = GE_PE_FLAG_DEFAULT;
    MDrv_GE_PixelEngine(&PESet);
#else
    if (pclrBtn->Fontfmt.flag&GEFONT_FLAG_BOLD)
    {
        pclrBtn->Fontfmt.flag |= GFXFONT_FLAG_BOLD;
    }

    text_info.dstblk.width = pclrBtn->width;
    text_info.dstblk.height = pclrBtn->height;
    text_info.flag = pclrBtn->Fontfmt.flag;
    memcpy(&text_info.color, &pclrBtn->t_clr, 4);
    text_info.pGlyphDispInfo = (GFX_GlyphDispInfo*)pGlyphDispInfo;
    text_info.gap = pclrBtn->Fontfmt.ifont_gap;

#if (ENABLE_THAI_OSD)
    if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && ( External Subtitle Language is Thai ))*/ )
    {
        MApi_GFX_TextOut(font_handle, pu8Str, pclrBtn->bStringIndexWidth+1, &text_info);
    }
    else
#endif
    {
		MApi_GFX_TextOut(font_handle, TempStr.U8Str, pclrBtn->bStringIndexWidth+1, &text_info);
    }
#endif

    if(pGlyphDispInfo != NULL)
    {
        MSAPI_MEMORY_FREE(pGlyphDispInfo,BUF_ID_GLYPH_DISP_INFO);
    }

    msAPI_OSD_Free_resource();
}

/******************************************************************************/
/// API for draw Punctuated text::
/// @param font_handle \b IN font handle
/// @param pu16Str \b IN pointer to string
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/// @param max_row_num \b IN max row count
/******************************************************************************/
void msAPI_OSD_DrawPunctuatedString(FONTHANDLE font_handle, U16 *pu16Str, OSDClrBtn *pclrBtn, U8 max_row_num)
{

    S8 gap;
    U8 scale, i, offset_len, cut_strlen;
    U16 total_strlen, width, u16Char;
    U16 max_width, buf_y;
    OSD_RESOURCE_FONT_INFO finfo;
    OSDSRC_GLYPH_BBOX_X BBox_X_Info;
    //For Last one Space character(ASCII code 0x20) of current string to be displayed in current OSD line.
    BOOLEAN bGetLastSpaceinSingleLine;
    U8 u8LastSpaceIndex;
    U16 u16LastSpace_Width;
    U8 bExceed = FALSE;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringTypeDouble outbuf;
	#if ( ENABLE_THAI_OSD )
		U8 bThaiDisplay = FALSE;
	#endif
    U16 nFontID;
	#if ( ENABLE_ARABIC_OSD )
		U8 bMirrorLang = FALSE;
		U8 bArabicDisplay = FALSE;
		U8 bHebrew = FALSE;
		U16 CharIndex;
	#endif
#endif
    FONTHANDLE Org_font_handle = font_handle;

    /*prevent error*/
    if (pclrBtn->bStringIndexWidth != CHAR_IDX_2BYTE)
    {
        APIOSD_DBG(printf("Only support 2 bytes strubg\n"));
        return;
    }

    //For Last one Space character(ASCII code 0x20) of current string to be displayed in current OSD line.
    if (max_row_num == 0)
    {
        APIOSD_DBG(printf("E_DrawSmartText>> max_row_num = 0\n"));
        return;
    }
    else if ((total_strlen = msAPI_OSD_GetStrLength((U8*)pu16Str, pclrBtn->bStringIndexWidth)) == 0)
    {
        APIOSD_DBG(printf("E_DrawSmartText>> strlen = 0\n"));
        return;
    }


    msAPI_OSD_GET_resource();


    msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);
    scale = 1;
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = (U8)((U16)pclrBtn->Fontfmt.height / (U16)finfo.height);
        pclrBtn->Fontfmt.height = finfo.height * scale;

        scale = (U8)((U16)pclrBtn->Fontfmt.width / (U16)finfo.width);
        pclrBtn->Fontfmt.width = finfo.width * scale;
    }

	if (total_strlen >= sizeof(TempStr.U8Str)/2)
	{
		total_strlen = sizeof(TempStr.U8Str)/2 - 1;
	}		        

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

	switch(enLanguage)
    {
#if ( ENABLE_ARABIC_OSD )
        case LANGUAGE_OSD_ARABIC:
        case LANGUAGE_OSD_PARSI:
        case LANGUAGE_OSD_KURDISH:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( bHebrewMixedMode )
                        bHebrew = TRUE;
                    else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;

                    if ( !bDivxSubtitleMode )
                    {
                        pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                    }
                }
            }

			for(CharIndex=0;CharIndex<total_strlen;CharIndex++)
			{
				if ( msAPI_OSD_IsNotInArabicHebrew(pu16Str[CharIndex]) == PURE_ARABIC_CHAR )
					break;
			}
			if ( CharIndex == total_strlen )
			{
				bHebrew = FALSE;
				bArabicDisplay = FALSE;
				if ( bHebrew == FALSE )
					pclrBtn->Fontfmt.ifont_gap = 0;
			}
			else
			{
				if ( bHebrew == FALSE )
					bArabicDisplay = TRUE;
			}
            break;
        case LANGUAGE_OSD_HEBREW:
            bHebrew = TRUE;
            if ( !bDivxSubtitleMode )
            {
                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
            }
            break;
#endif
#if ( ENABLE_THAI_OSD )
        case LANGUAGE_OSD_THAI:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_THAI_0].fHandle;
                }
            }

	#if ( ENABLE_ARABIC_OSD )
            if ( bHebrew == FALSE )
	#endif
            {
                bThaiDisplay = TRUE;
            }
            break;
#endif
        /*
        case LANGUAGE_OSD_CHINESE:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
                    {
                        font_handle = Font[nFontID+18].fHandle;
                    }
                    else if ( nFontID >= FONT_EXT_BEGIN )
                    {
                        //Do Chinese icon unicode conversion here
                    }
                }
            }
            break;
        */
        default:
            break;
    }
#endif


#if (ENABLE_ARABIC_OSD)

    if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
    {
        bMirrorLang = TRUE;
    }
#endif

#if (ENABLE_ARABIC_OSD)

	if ( bMirrorLang )
	{
		//For Subtitle
		if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
		{
			S32 errorCode = U_ZERO_ERROR;

			ArabicParser(pu16Str, total_strlen, outbuf.U16Str, STROSD,
					 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
					 &errorCode);
			//Every language is possible, you can't skip this filter here
			msAPI_OSD_KurdishFilter(outbuf.U16Str, total_strlen);
		}
		else if ( bArabicDisplay ) //for OSD
		{
			 S32 errorCode = U_ZERO_ERROR;

			ArabicParser(pu16Str, total_strlen, outbuf.U16Str, STROSD,
					 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
					 &errorCode);
			//just skip this function for non Kurdish condition
			if ( enLanguage == LANGUAGE_OSD_KURDISH )
			{
				msAPI_OSD_KurdishFilter(outbuf.U16Str, total_strlen);
			}
		}
		else    // for Hebrew
		{
			memcpy(outbuf.U8Str, (U8 *)pu16Str, total_strlen*2);
		}

		outbuf.U16Str[total_strlen] = 0;

		pu16Str = outbuf.U16Str;
	}
	else
#endif

#if (ENABLE_THAI_OSD)
	if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
	{
		//pu16Str = (U16 *)pu8Str;

		total_strlen = Thai_compose(pu16Str, 0, total_strlen, STROSD, outbuf.U16Str);
		outbuf.U16Str[total_strlen] = 0x0000;

		pu16Str = outbuf.U16Str;
	}
	else
#endif
    {
        //memcpy(outbuf.U8Str,(U8 *)pu16Str,total_strlen*2);

        //outbuf.U16Str[total_strlen] = 0;

        //pu16Str = outbuf.U16Str;
	}

    gap = 0;
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
    #if (ENABLE_ARABIC_OSD)
		if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
		{
			pclrBtn->Fontfmt.ifont_gap = 0;
		}
    #endif

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
    }
    else
    {
        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
        else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
        {
            gap = -pclrBtn->Fontfmt.dis;
        }
    }

    offset_len = 1; //2;
    buf_y = pclrBtn->y;
    max_width = pclrBtn->width - 2*BTN_TEXT_GAP;

    for (i=0; i<max_row_num && total_strlen; i++)
    {
        width = 0;
        cut_strlen = 0;
        bGetLastSpaceinSingleLine=FALSE;//Reset variable
        u8LastSpaceIndex = 0;//Reset variable
        u16LastSpace_Width = 0; //Reset variable
        bExceed = FALSE;

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
        {
            while (width<max_width && cut_strlen<total_strlen)
            {
            #if (ENABLE_ARABIC_OSD)
				//For Subtitle
				if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || (External Subtitle Language is Arabic) */ ) )
				{
					// Check if it is in Arabic Vowel unicode range
					if ( cut_strlen > 0 )
					{
						if ( msAPI_OSD_IsArabicVowel( pu16Str[cut_strlen-1] ) )
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen-1], &BBox_X_Info);
							width -= (BBox_X_Info.u8Width*scale + gap);
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else
					{
						msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
						width += (BBox_X_Info.u8Width*scale + gap);
					}
				}
				else if ( bArabicDisplay ) //for OSD
				{
					// Check if it is in Arabic Vowel unicode range
					if ( cut_strlen > 0 )
					{
						if ( msAPI_OSD_IsArabicVowel( pu16Str[cut_strlen] ) )
						{
								//skip current vowel character width
								//do nothing here
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else
					{
						msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
						width += (BBox_X_Info.u8Width*scale + gap);
					}
				}
				else
            #endif
            #if (ENABLE_THAI_OSD)
				if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
				{
					// Check Primary Thai unicode range
					if ( ( pu16Str[cut_strlen] <= 0x0E5B ) && ( pu16Str[cut_strlen] >= 0x0E01 ) )
					{
						if ( MinusDrawTable[ pu16Str[cut_strlen] - 0x0E01 ] )
						{
							//Minus coordinate, don't add width
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else if ( ( pu16Str[cut_strlen] <= 0xF71A ) && ( pu16Str[cut_strlen] >= 0xF700 ) )
					{
						if ( MinusDrawTablePersentation[ pu16Str[cut_strlen] - 0xF700 ] )
						{
							//Minus coordinate, don't add width
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else
					{
						msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
						width += (BBox_X_Info.u8Width*scale + gap);
					}
				}
				else
            #endif
                {
                    msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
                    width += (BBox_X_Info.u8Width*scale + gap);
                }

				cut_strlen++;

                if ((pu16Str[(cut_strlen-1)]==0x20)||((pu16Str[(cut_strlen-1)]>=0x80)&&(pu16Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                {
                    u8LastSpaceIndex =  cut_strlen;//Set the LastSpaceIndex to current string index.
                    u16LastSpace_Width = width; //Set the LastSpace_Width to current string width.
                    bGetLastSpaceinSingleLine = TRUE;

                    if ( pu16Str[(cut_strlen-1)]==0x8A )
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                        break;
                    }
                    else
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                    }
                }
            }
        }
        else
        {
            while(width<max_width && cut_strlen<total_strlen)
            {
                cut_strlen++;
                width += (finfo.width + gap);
                if ((pu16Str[(cut_strlen-1)]==0x20)||((pu16Str[(cut_strlen-1)]>=0x80)&&(pu16Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                {
                    u8LastSpaceIndex =  cut_strlen;//Set the LastSpaceIndex to current string index.
                    u16LastSpace_Width = width; //Set the LastSpace_Width to current string width.
                    bGetLastSpaceinSingleLine = TRUE;

                    if ( pu16Str[(cut_strlen-1)]==0x8A )
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                        break;
                    }
                    else
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                    }
                }
            }
        }

        if (width >= max_width)
        {
            //cut_strlen--;
            bExceed = TRUE;
        }
#if 0   // Display long string as possible; Sync from DMKIM
        //Set new cut string length to last Space index
        if (bGetLastSpaceinSingleLine&&u8LastSpaceIndex&&(total_strlen > cut_strlen))
        {
            #if (Very_Long_Word_Case==1)
            if ( u16LastSpace_Width>((U16)(max_width/2)) )
            #endif
        {
               //if (i < max_row_num-1)
            cut_strlen = u8LastSpaceIndex;
        }
        }
        //Set new cut string length to last Space index
#endif

        APIOSD_DBG(printf("DrawSmartText>> cut_strlen = %bu\n", cut_strlen));

        if (cut_strlen == 0)
        {
            break;
        }
        else
        {
            if (bExceed && i == max_row_num-1)
            {
                //pu16Str[cut_strlen-1] = 0x01; // ...
                pu16Str[cut_strlen-2] = CHAR_DOT; // .
                pu16Str[cut_strlen-1] = CHAR_DOT; // .
            }
            u16Char = pu16Str[cut_strlen];
            pu16Str[cut_strlen] = 0;
            msAPI_OSD_DrawText(Org_font_handle, (U8*)pu16Str, pclrBtn);
            pu16Str[cut_strlen] = u16Char;

            total_strlen -= cut_strlen;
            pu16Str += (cut_strlen*offset_len);
        }
        pclrBtn->y += pclrBtn->height;
    }

    pclrBtn->y = buf_y;

    msAPI_OSD_Free_resource();

}


#if 1//def ZUI
/******************************************************************************/
/// API for estimate height of Punctuated text::
/// @param font_handle \b IN font handle
/// @param pu16Str \b IN pointer to string
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/// @param max_row_num \b IN max row count
/// @param pu8rows \b OUT row count
/// @param pu16height \b OUT height (pixels)
/******************************************************************************/
void msAPI_OSD_GetPunctuatedStringHeight(FONTHANDLE font_handle, U16 *pu16Str, OSDClrBtn *pclrBtn, U8 max_row_num,
    U8 * pu8rows, U16 * pu16height)
{

    S8 gap;
    U8 scale, i, offset_len, cut_strlen;
    U16 total_strlen, width; //, u16Char;
    U16 max_width, buf_y;
    OSD_RESOURCE_FONT_INFO finfo;
    OSDSRC_GLYPH_BBOX_X BBox_X_Info;
    //For Last one Space character(ASCII code 0x20) of current string to be displayed in current OSD line.
    BOOLEAN bGetLastSpaceinSingleLine;
    U8 u8LastSpaceIndex;
    U16 u16LastSpace_Width;
    U8 bExceed = FALSE;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringTypeDouble outbuf;
	#if ( ENABLE_THAI_OSD )
		U8 bThaiDisplay = FALSE;
	#endif
    U16 nFontID;
	#if ( ENABLE_ARABIC_OSD )
		U8 bMirrorLang = FALSE;
		U8 bArabicDisplay = FALSE;
		U8 bHebrew = FALSE;
		U16 CharIndex;
	#endif
#endif

    *pu8rows = 0;
    *pu16height = 0;

    /*prevent error*/
    if (pclrBtn->bStringIndexWidth != CHAR_IDX_2BYTE)
    {
        APIOSD_DBG(printf("Only support 2 bytes strubg\n"));
        return;
    }

    /*/For Last one Space character(ASCII code 0x20) of current string to be displayed in current OSD line.
    if (max_row_num == 0)
    {
        APIOSD_DBG(printf("E_DrawSmartText>> max_row_num = 0\n"));
        return;
    }
    else*/ if ((total_strlen = msAPI_OSD_GetStrLength((U8*)pu16Str, CHAR_IDX_2BYTE/*pclrBtn->bStringIndexWidth*/)) == 0)
    {
        APIOSD_DBG(printf("E_DrawSmartText>> strlen = 0\n"));
        return;
    }

    msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);

    scale = 1;
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = (U8)((U16)pclrBtn->Fontfmt.height / (U16)finfo.height);
        pclrBtn->Fontfmt.height = finfo.height * scale;

        scale = (U8)((U16)pclrBtn->Fontfmt.width / (U16)finfo.width);
        pclrBtn->Fontfmt.width = finfo.width * scale;
    }

	if (total_strlen >= sizeof(TempStr.U8Str)/2)
	{
		total_strlen = sizeof(TempStr.U8Str)/2 - 1;
	}		        

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

	switch(enLanguage)
    {
    #if ( ENABLE_ARABIC_OSD )
        case LANGUAGE_OSD_ARABIC:
        case LANGUAGE_OSD_PARSI:
        case LANGUAGE_OSD_KURDISH:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( bHebrewMixedMode )
                        bHebrew = TRUE;
                    else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;

                    if ( !bDivxSubtitleMode )
                    {
                        pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                    }
                }
            }

			for(CharIndex=0;CharIndex<total_strlen;CharIndex++)
			{
				if ( msAPI_OSD_IsNotInArabicHebrew(pu16Str[CharIndex]) == PURE_ARABIC_CHAR )
					break;
			}
			if ( CharIndex == total_strlen )
			{
				bHebrew = FALSE;
				bArabicDisplay = FALSE;
				if ( bHebrew == FALSE )
					pclrBtn->Fontfmt.ifont_gap = 0;
			}
			else
			{
				if ( bHebrew == FALSE )
					bArabicDisplay = TRUE;
			}
            break;
        case LANGUAGE_OSD_HEBREW:
            bHebrew = TRUE;
            if ( !bDivxSubtitleMode )
            {
                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
            }
            break;
    #endif
    #if ( ENABLE_THAI_OSD )
        case LANGUAGE_OSD_THAI:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_THAI_0].fHandle;
                }
            }
	#if ( ENABLE_ARABIC_OSD )
            if ( bHebrew == FALSE )
	#endif
            {
                bThaiDisplay = TRUE;
            }
            break;
    #endif
    /*
        case LANGUAGE_OSD_CHINESE:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
                    {
                        font_handle = Font[nFontID+18].fHandle;
                    }
                    else if ( nFontID >= FONT_EXT_BEGIN )
                    {
                        //Do Chinese icon unicode conversion here
                    }
                }
            }
            break;
    */
        default:
            break;
    }
#endif

#if (ENABLE_ARABIC_OSD)
    if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
    {
        bMirrorLang = TRUE;
    }
#endif

#if (ENABLE_ARABIC_OSD)

	if ( bMirrorLang )
	{
		//For Subtitle
		if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
		{
			S32 errorCode = U_ZERO_ERROR;

			ArabicParser(pu16Str, total_strlen, outbuf.U16Str, STROSD,
					 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
					 &errorCode);
			//Every language is possible, you can't skip this filter here
			msAPI_OSD_KurdishFilter(outbuf.U16Str, total_strlen);
		}
		else if ( bArabicDisplay ) //for OSD
		{
			 S32 errorCode = U_ZERO_ERROR;

			ArabicParser(pu16Str, total_strlen, outbuf.U16Str, STROSD,
					 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
					 &errorCode);
			//just skip this function for non Kurdish condition
			if ( enLanguage == LANGUAGE_OSD_KURDISH )
			{
				msAPI_OSD_KurdishFilter(outbuf.U16Str, total_strlen);
			}
		}
		else    // for Hebrew
		{
			memcpy(outbuf.U8Str, (U8 *)pu16Str, total_strlen*2);
		}

		outbuf.U16Str[total_strlen] = 0;

		pu16Str = outbuf.U16Str;
	}
	else
#endif

#if (ENABLE_THAI_OSD)
	if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
	{
		//pu16Str = (U16 *)pu8Str;

		total_strlen = Thai_compose(pu16Str, 0, total_strlen, STROSD, outbuf.U16Str);
		outbuf.U16Str[total_strlen] = 0x0000;

		pu16Str = outbuf.U16Str;
	}
	else
#endif
	{
    //memcpy(outbuf.U8Str,(U8 *)pu16Str,total_strlen*2);

    //outbuf.U16Str[total_strlen] = 0;

    //pu16Str = outbuf.U16Str;
	}

    gap = 0;
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
    #if (ENABLE_ARABIC_OSD)
		if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
		{
			pclrBtn->Fontfmt.ifont_gap = 0;
		}
    #endif

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
    }
    else
    {
        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
        else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
        {
            gap = -pclrBtn->Fontfmt.dis;
        }
    }

    offset_len = 1; //2;
    buf_y = pclrBtn->y;
    max_width = pclrBtn->width - 2*BTN_TEXT_GAP;

    for (i=0; i<max_row_num && total_strlen; i++)
    {
        width = 0;
        cut_strlen = 0;
        bGetLastSpaceinSingleLine=FALSE;//Reset variable
        u8LastSpaceIndex = 0;//Reset variable
        u16LastSpace_Width = 0; //Reset variable
        bExceed = FALSE;

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
        {
            while (width<max_width && cut_strlen<total_strlen)
            {
            #if (ENABLE_ARABIC_OSD)
				//For Subtitle
				if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || (External Subtitle Language is Arabic) */ ) )
				{
					// Check if it is in Arabic Vowel unicode range
					if ( cut_strlen > 0 )
					{
						if ( msAPI_OSD_IsArabicVowel( pu16Str[cut_strlen-1] ) )
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen-1], &BBox_X_Info);
							width -= (BBox_X_Info.u8Width*scale + gap);
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else
					{
						msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
						width += (BBox_X_Info.u8Width*scale + gap);
					}
				}
				else if ( bArabicDisplay ) //for OSD
				{
					// Check if it is in Arabic Vowel unicode range
					if ( cut_strlen > 0 )
					{
						if ( msAPI_OSD_IsArabicVowel( pu16Str[cut_strlen] ) )
						{
								//skip current vowel character width
								//do nothing here
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else
					{
						msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
						width += (BBox_X_Info.u8Width*scale + gap);
					}
				}
				else
            #endif
            #if (ENABLE_THAI_OSD)
				if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
				{
					// Check Primary Thai unicode range
					if ( ( pu16Str[cut_strlen] <= 0x0E5B ) && ( pu16Str[cut_strlen] >= 0x0E01 ) )
					{
						if ( MinusDrawTable[ pu16Str[cut_strlen] - 0x0E01 ] )
						{
							//Minus coordinate, don't add width
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else if ( ( pu16Str[cut_strlen] <= 0xF71A ) && ( pu16Str[cut_strlen] >= 0xF700 ) )
					{
						if ( MinusDrawTablePersentation[ pu16Str[cut_strlen] - 0xF700 ] )
						{
							//Minus coordinate, don't add width
						}
						else
						{
							msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
							width += (BBox_X_Info.u8Width*scale + gap);
						}
					}
					else
					{
						msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
						width += (BBox_X_Info.u8Width*scale + gap);
					}
				}
				else
            #endif
                {
                    msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu16Str[cut_strlen], &BBox_X_Info);
                    width += (BBox_X_Info.u8Width*scale + gap);
                }

				cut_strlen++;

                if ((pu16Str[(cut_strlen-1)]==0x20)||((pu16Str[(cut_strlen-1)]>=0x80)&&(pu16Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                {
                    u8LastSpaceIndex =  cut_strlen;//Set the LastSpaceIndex to current string index.
                    u16LastSpace_Width = width; //Set the LastSpace_Width to current string width.
                    bGetLastSpaceinSingleLine = TRUE;

                    if ( pu16Str[(cut_strlen-1)]==0x8A )
                    {
                        //don't modify...pu16Str[(cut_strlen-1)]=0x20;
                        break;
                    }
                    else
                    {
                        //don't modify...pu16Str[(cut_strlen-1)]=0x20;
                    }
                }
            }
        }
        else
        {
            while(width<max_width && cut_strlen<total_strlen)
            {
                cut_strlen++;
                width += (finfo.width + gap);
                if ((pu16Str[(cut_strlen-1)]==0x20)||((pu16Str[(cut_strlen-1)]>=0x80)&&(pu16Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                {
                    u8LastSpaceIndex =  cut_strlen;//Set the LastSpaceIndex to current string index.
                    u16LastSpace_Width = width; //Set the LastSpace_Width to current string width.
                    bGetLastSpaceinSingleLine = TRUE;

                    if ( pu16Str[(cut_strlen-1)]==0x8A )
                    {
                        //don't modify...pu16Str[(cut_strlen-1)]=0x20;
                        break;
                    }
                    else
                    {
                        //don't modify...pu16Str[(cut_strlen-1)]=0x20;
                    }
                }
            }
        }

        if (width >= max_width)
        {
            //cut_strlen--;
            bExceed = TRUE;
        }
#if 0   // Display long string as possible; Sync from DMKIM
        //Set new cut string length to last Space index
        if (bGetLastSpaceinSingleLine&&u8LastSpaceIndex&&(total_strlen > cut_strlen))
        {
            #if (Very_Long_Word_Case==1)
            if ( u16LastSpace_Width>((U16)(max_width/2)) )
            #endif
            {
               //if (i < max_row_num-1)
               cut_strlen = u8LastSpaceIndex;
            }
        }
        //Set new cut string length to last Space index
#endif

        APIOSD_DBG(printf("DrawSmartText>> cut_strlen = %bu\n", cut_strlen));

        if (cut_strlen == 0)
        {
            break;
        }
        else
        {
            /*
            if (bExceed && i == max_row_num-1)
            {
                //pu16Str[cut_strlen-1] = 0x01; // ...
                pu16Str[cut_strlen-2] = CHAR_DOT; // .
                pu16Str[cut_strlen-1] = CHAR_DOT; // .
            }
            u16Char = pu16Str[cut_strlen];
            pu16Str[cut_strlen] = 0;
            msAPI_OSD_DrawText(Org_font_handle, (U8*)pu16Str, pclrBtn);
            pu16Str[cut_strlen] = u16Char;
            */

            *(pu8rows) += 1;
            *(pu16height) += pclrBtn->height;

            total_strlen -= cut_strlen;
            pu16Str += (cut_strlen*offset_len);
        }
        pclrBtn->y += pclrBtn->height;
    }

    pclrBtn->y = buf_y;
}
#endif //ZUI


#if 0
/******************************************************************************/
void msAPI_OSD_DrawPunctuatedString_S1(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn, U8 max_row_num)
{
    S8 gap;
    S16 i;
    U8 scale, offset_len;
    U16 cut_strlen, u16LastSpaceIndex;
    U16 total_strlen, strlen, width, bytes_num, total_width;
    U16 max_width, buf_y;
    U8 *pStr;
    U16 *pu16Str;
    FONT_INFO finfo;
    GFX_GlyphBbox_X BBox_X_Info;
//For Last one Space character(ASCII code 0x20) of current string to be displayed in current OSD line.
    BOOLEAN bGetLastSpaceinSingleLine;
    U16 u16LastSpace_Width;
//For Last one Space character(ASCII code 0x20) of current string to be displayed in current OSD line.
       BOOLEAN bIsEmphasis =false;
       BOOLEAN bControlCode0x86 = false;
       BOOLEAN bControlCode0x87 = false;
       BOOLEAN bControlCodeSHY = false;
       BOOLEAN bControlCodeSHY_NewLine = false;
       BOOLEAN bLastSHY = false;
    BOOLEAN bControlCodeSHY_With_NewLine = false;
      // BOOLEAN bNewLine= true;

       U16 u16OrignalxPosition = pclrBtn->x;
       U16 u16OrignalFontfmtFlag = pclrBtn ->Fontfmt.flag;
       U8 pSHYStr[2];
    U32 _timer = 0;
       pSHYStr[0]=0x2d;
       pSHYStr[1]=0;




    _timer = msAPI_Timer_GetTime0();

    if(max_row_num == 0)
    {
        APIOSD_DBG(printf("E_DrawSmartText>> max_row_num = 0\n"));
        return;
    }
    else if ((total_strlen = msAPI_OSD_GetStrLength(pu8Str, pclrBtn->bStringIndexWidth)) == 0)
    {
        APIOSD_DBG(printf("E_DrawSmartText>> strlen = 0\n"));
        return;
    }

    msAPI_OSD_GET_resource();

        strlen = total_strlen;
    MDrv_GE_GetFontInfo(font_handle, &finfo);
    scale = 1;
    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = (U8)((U16)pclrBtn->Fontfmt.height / (U16)finfo.height);
        pclrBtn->Fontfmt.height = finfo.height * scale;

        scale = (U8)((U16)pclrBtn->Fontfmt.width / (U16)finfo.width);
        pclrBtn->Fontfmt.width = finfo.width * scale;
    }

    gap = 0;
    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
        if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
    }
    else
    {
        if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
              else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
        {
            gap = -pclrBtn->Fontfmt.dis;
        }
    }

    if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
    {
        offset_len = 1;
    }
    else
    {
        offset_len = 2;
    }

    buf_y = pclrBtn->y;

    max_width = pclrBtn->width - 2*BTN_TEXT_GAP;
       //width=0;
       total_width =0;
    for(i=0; i<max_row_num && total_strlen; i++)
    {

        width = 0;

        cut_strlen = 0;
        bGetLastSpaceinSingleLine=FALSE;//Reset variable
        u16LastSpaceIndex = 0;//Reset variable
        u16LastSpace_Width = 0; //Reset variable
        bControlCodeSHY_NewLine = false;
        bControlCodeSHY_With_NewLine = false;
        if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
        {
            if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
            {
                while(total_width<max_width && cut_strlen<total_strlen)
                {
                    MDrv_GE_GetBBox_X(font_handle, pu8Str[cut_strlen++], &BBox_X_Info);
                    width += (BBox_X_Info.u8Width*scale + gap);
                    total_width += (BBox_X_Info.u8Width*scale + gap);
                    if ((pu8Str[(cut_strlen-1)]==0x20)||((pu8Str[(cut_strlen-1)]>=0x80)&&(pu8Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                    {
                        u16LastSpaceIndex =  cut_strlen;//Set the LastSpaceIndex to current string index.
                        u16LastSpace_Width = width; //Set the LastSpace_Width to current string width.
                        bGetLastSpaceinSingleLine = TRUE;

                        if ( pu8Str[(cut_strlen-1)]==0x8A )
                        {
                            pu8Str[(cut_strlen-1)]=0x20;
                            break;
                        }
                        else if ( pu8Str[(cut_strlen-1)]==0x86 )
                        {

                            bIsEmphasis =true;
                            bControlCode0x86 =true;
                            break;
                        }
                        else if ( pu8Str[(cut_strlen-1)]==0x87 )
                        {

                            bControlCode0x87 = true;

                            break;
                        }
                        else
                        {
                            pu8Str[(cut_strlen-1)]=0x20;
                        }
                    }
                }
            }
            else //(pclrBtn->bStringIndexWidth == CHAR_IDX_2BYTE)
            {
                pu16Str = (U16*)pu8Str;

                        if (i==1)
                            u16NextLineStartInx = (strlen -total_strlen);
                        //printf("i = %bu \n", i);
                        //printf("total_strlen = %u \n", total_strlen);
                        //printf("u16NextLineStartInx= %u \n", u16NextLineStartInx);
                while(total_width<max_width && cut_strlen<total_strlen)
                {

                    MDrv_GE_GetBBox_X(font_handle, pu16Str[cut_strlen++], &BBox_X_Info);
                    width += (BBox_X_Info.u8Width*scale + gap);

                    total_width+=(BBox_X_Info.u8Width*scale + gap);
                    // printf("max_width =%u, total_width = %u,  pu16Str[%bu]= %x \n",max_width,total_width,cut_strlen,pu16Str[cut_strlen]);


                    if (total_width>=max_width &&  bLastSHY)
                    {

                        bControlCodeSHY_NewLine =true;

                        bLastSHY = false;
                        //bControlCodeSHY = true;
                    }

                    if ((pu16Str[(cut_strlen-1)]==0x0020)||(pu16Str[(cut_strlen-1)]==0x00AD)||((pu16Str[(cut_strlen-1)]>=0xE080)&&(pu16Str[(cut_strlen-1)]<=0xE09F)) )//if Space Char or Control code
                    {
                        u16LastSpaceIndex =  cut_strlen;//Set the LastSpaceIndex to current string index.

                        u16LastSpace_Width = width; //Set the LastSpace_Width to current string width.
                        bGetLastSpaceinSingleLine = TRUE;

                        if ( pu16Str[(cut_strlen-1)]==0xE08A )
                        {
                            pu16Str[(cut_strlen-1)]=0x0020;

                            break;
                        }
                        else if ( pu16Str[(cut_strlen-1)]==0xE086 )
                        {

                            bControlCode0x86 =true;

                            break;
                        }
                        else if ( pu16Str[(cut_strlen-1)]==0xE087 )
                        {

                            bControlCode0x87 = true;

                            break;
                        }
                        else if (pu16Str[(cut_strlen-1)]==0x00AD)
                        {
                            bControlCodeSHY = true;


                            break;
                         }
                        else
                        {
                            pu16Str[(cut_strlen-1)]=0x0020;
                        }
                    }
                }
            }
        }
        else
        {
            while(total_width<max_width && cut_strlen<total_strlen)
            {
                cut_strlen++;
                width += (finfo.width + gap);
                total_width += (finfo.width + gap);
                if ((pu8Str[(cut_strlen-1)]==0x20)||((pu8Str[(cut_strlen-1)]>=0x80)&&(pu8Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                {
                    u16LastSpaceIndex =  cut_strlen;//Set the LastSpaceIndex to current string index.
                    u16LastSpace_Width = width; //Set the LastSpace_Width to current string width.
                    bGetLastSpaceinSingleLine = TRUE;

                    if ( pu8Str[(cut_strlen-1)]==0x8A )
                    {
                        pu8Str[(cut_strlen-1)]=0x20;
                        break;
                    }
                    else
                        pu8Str[(cut_strlen-1)]=0x20;
                }
            }
        }

        if(width > max_width)
        {
            //if (!bControlCodeSHY_NewLine)
                cut_strlen--;
        }

        //Set new cut string length to last Space index
        if (bGetLastSpaceinSingleLine&&u16LastSpaceIndex&&(total_strlen > cut_strlen))
        {
            cut_strlen = u16LastSpaceIndex;
        }
        //Set new cut string length to last Space index

        APIOSD_DBG(printf("DrawSmartText>> cut_strlen = %bu\n", cut_strlen));

        if(cut_strlen == 0)
        {
            break;
        }
        else
        {
            bytes_num = (cut_strlen+1) * offset_len;
            pStr = msAPI_Memory_Allocate(bytes_num,BUF_ID_DRAW_TEXT);
            if(pStr)
            {
                memcpy(pStr, pu8Str, bytes_num);

                if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
                {
                    pStr[cut_strlen] = 0;
                }
                else
                {

                                   ((U16*)pStr)[cut_strlen]=0;
                                    if (bControlCodeSHY)
                                        ((U16*)pStr)[cut_strlen-1]=0x0020;
                }
                            if (bControlCode0x87)
                            {

                                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_BOLD;
                            }
                            if (bControlCodeSHY_NewLine)
                            {

                                pclrBtn ->bStringIndexWidth = CHAR_IDX_1BYTE;
                                msAPI_OSD_DrawText(font_handle, pSHYStr, pclrBtn);
                                bControlCodeSHY_With_NewLine = bControlCodeSHY;
                                 bControlCodeSHY = false;
                                 pclrBtn->y += pclrBtn->height;
                                 pclrBtn ->x = u16OrignalxPosition;
                                 bLastSHY=false;
                                pclrBtn ->bStringIndexWidth = CHAR_IDX_2BYTE;

                            }

                            if( i != max_row_num-1 ||  !bControlCodeSHY_NewLine )
                    msAPI_OSD_DrawText(font_handle, pStr, pclrBtn);
                else
                    bControlCodeSHY_With_NewLine = false;
                msAPI_Memory_Free(pStr,BUF_ID_DRAW_TEXT);
            }

            total_strlen -= cut_strlen;
            pu8Str += (cut_strlen*offset_len);
        }

        if (bControlCode0x86)
              {
                    bControlCode0x86 = false;
                    bIsEmphasis =true;
                    pclrBtn->x += u16LastSpace_Width;
                     bLastSHY=false;
                     i--; // no need to increase max_row_num
              }
              else if ( bControlCode0x87)
              {

                    bControlCode0x87 = false;
                    bIsEmphasis =false;
                    //pclrBtn->t_clr = 0x003C3C3C;
                    pclrBtn->Fontfmt.flag = u16OrignalFontfmtFlag;
                    pclrBtn->x += u16LastSpace_Width;
                    bLastSHY=false;
                    i--; // no need to increase max_row_num
              }
              else if (bControlCodeSHY)
              {
                    if (bControlCodeSHY_NewLine)
                    {
                        bControlCodeSHY_NewLine =false;
                        bControlCodeSHY = false;
                         bLastSHY=false;
                    }
                    else
                    {

                         bControlCodeSHY = false;
                         bLastSHY=true;
                        //pclrBtn ->x = u16OrignalxPosition;
                        MDrv_GE_GetBBox_X(font_handle, 0x00AD, &BBox_X_Info);
                        pclrBtn->x += u16LastSpace_Width;
                  pclrBtn->x -=(BBox_X_Info.u8Width*scale + gap);
                        if(total_width > (BBox_X_Info.u8Width*scale + gap))
                            total_width-=(BBox_X_Info.u8Width*scale + gap);
                        else
                            total_width=0;
                        i--;
                     }

               }
              else
              {
                   if (!bControlCodeSHY_NewLine)
                    {
                        pclrBtn->y += pclrBtn->height;
                        pclrBtn ->x = u16OrignalxPosition;
                        total_width=0;
                        bLastSHY=false;
                        //width=0;
                    }
            else if(bControlCodeSHY_With_NewLine)
            {
                bControlCodeSHY_NewLine = bControlCodeSHY_With_NewLine = false;
                MDrv_GE_GetBBox_X(font_handle, 0x0020, &BBox_X_Info);
                pclrBtn->x += width;
                if(pclrBtn->x >= (BBox_X_Info.u8Width*scale + gap))
                {
                    pclrBtn->x -=(BBox_X_Info.u8Width*scale + gap);
                    if(pclrBtn->x < u16OrignalxPosition)
                        pclrBtn ->x = u16OrignalxPosition;
                }
                else
                    pclrBtn ->x = u16OrignalxPosition;
                total_width = width;
                if(total_width > (BBox_X_Info.u8Width*scale + gap))
                    total_width-=(BBox_X_Info.u8Width*scale + gap);
                else
                    total_width=0;
            }
                   else
                    {
                        bControlCodeSHY_NewLine=false;
                        pclrBtn->x +=width;
                        total_width = width;
                    }
              }
              if(total_strlen)
                bStopScrollDown =false;
              else
                bStopScrollDown =true;

    }

    pclrBtn->y = buf_y;

    msAPI_OSD_Free_resource();
    printf ("Time = %lu\n", _timer = msAPI_Timer_DiffTimeFromNow(_timer));

}

#else
//BOOLEAN g_EpgTest;

/******************************************************************************/
/// API for draw 2 Bytes text::
/// @param font_handle \b IN font handle
/// @param pu8Str \b IN pointer to string
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_Draw2BytesText(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn)
{
    U16 i, len, u16StrWidth;
    U16 scale;
    OSD_RESOURCE_FONT_INFO finfo;
    GFX_TextOutInfo text_info;

    OSDSRC_GLYPH_DISP_INFO *pGlyphDispInfo=NULL;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringType TempBuf;
	OSDStringTypeDouble outbuf;
	#if ( ENABLE_ARABIC_OSD )
		OSDStringType out;
		U8 bMirrorLang = FALSE;
		U8 bArabicDisplay = FALSE;
		U8 bHebrew = FALSE;
	#endif
	#if ( ENABLE_THAI_OSD )
		U8 bThaiDisplay = FALSE;
	#endif
    U16 nFontID;
#endif

     msAPI_OSD_GET_resource();

    /* check range */
    /*if((pclrBtn->x + pclrBtn->width) > NEWUI_MAIN_MENU_GWIN_WIDTH)
    {
        return;
    }
    else if((pclrBtn->y + pclrBtn->height) > NEWUI_MAIN_MENU_GWIN_HEIGHT)
    {
        return;
    }
    else*/ if((len = msAPI_OSD_GetStrLength(pu8Str, pclrBtn->bStringIndexWidth)) == 0)
    {
        msAPI_OSD_Free_resource();
        return;
    }

#if 0
    if(pclrBtn->bStringIndexWidth==CHAR_IDX_1BYTE)
    {
        if (len >= sizeof(TempStr.U8Str))
        {
            len = sizeof(TempStr.U8Str) - 1;
        }
        memcpy(TempStr.U8Str,pu8Str,len);
        TempStr.U8Str[len] = 0;

    }
    else
    {
        if (len >= sizeof(TempStr.U8Str)/2)
        {
            len = sizeof(TempStr.U8Str)/2 - 1;
        }
        memcpy(TempStr.U8Str,pu8Str,len*2);
        ((U16 *)TempStr.U8Str)[len] = 0;
    }
#else
#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

	switch(enLanguage)
    {
#if ( ENABLE_ARABIC_OSD )
        case LANGUAGE_OSD_ARABIC:
        case LANGUAGE_OSD_PARSI:
        case LANGUAGE_OSD_KURDISH:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
/*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
					msAPI_OSD_Set_H_Mirror(FALSE);
					//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
                }
                else
*/
                {
                    if ( bHebrewMixedMode )
					{
                        bHebrew = TRUE;
					}
                    else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
					{
                        font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;
					}

                    if ( !bDivxSubtitleMode )
                    {
                        pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                        msAPI_GE_SetMirror(1, 0);    //patch GE driver
						msAPI_OSD_Set_H_Mirror(TRUE);
						//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), TRUE);
                    }
                    else
                    {
						msAPI_OSD_Set_H_Mirror(FALSE);
						//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
                    }
                }
            }

            if ( bHebrew == FALSE )
			{
                bArabicDisplay = TRUE;
			}
            break;
        case LANGUAGE_OSD_HEBREW:
            bHebrew = TRUE;
            if ( !bDivxSubtitleMode )
            {
                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                msAPI_GE_SetMirror(1, 0);    //patch GE driver
				msAPI_OSD_Set_H_Mirror(TRUE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), TRUE);
            }
            else
            {
                msAPI_OSD_Set_H_Mirror(FALSE);
            }
            break;
#endif
#if ( ENABLE_THAI_OSD )
        case LANGUAGE_OSD_THAI:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
/*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
*/
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_THAI_0].fHandle;
                }
            }

	#if ( ENABLE_ARABIC_OSD )
            if ( bHebrew == FALSE )
	#endif
            {
                bThaiDisplay = TRUE;
				msAPI_OSD_Set_H_Mirror(FALSE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            }
            break;
#endif
/*
        case LANGUAGE_OSD_CHINESE:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
                    {
                        font_handle = Font[nFontID+18].fHandle;
                    }
                    else if ( nFontID >= FONT_EXT_BEGIN )
                    {
                        //Do Chinese icon unicode conversion here
                    }
                }
            }
            if ( bHebrew == FALSE )
            {
				msAPI_OSD_Set_H_Mirror(FALSE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            }
            break;
*/
        default:
			msAPI_OSD_Set_H_Mirror(FALSE);
			//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            break;
    }
#endif

#if (ENABLE_ARABIC_OSD)

    if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
    {
        bMirrorLang = TRUE;
    }
#endif

    if(pclrBtn->bStringIndexWidth==CHAR_IDX_2BYTE)
    {
#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
        if (len >= sizeof(TempBuf.U8Str)/2)
        {
            len = sizeof(TempBuf.U8Str)/2 - 1;
        }
#endif
#if (!ENABLE_ARABIC_OSD)
//        memcpy(TempBuf.U8Str,pu8Str,len*2);
//		TempBuf.U16Str[len] = 0;
#else
        if ( bMirrorLang )
        {
            U8 bReverse = TRUE;

            memcpy(TempBuf.U8Str,pu8Str,len*2);
			TempBuf.U16Str[len] = 0;

            //For Subtitle
            if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
            {
                S32 errorCode = U_ZERO_ERROR;

                ArabicParser(TempBuf.U16Str, len, outbuf.U16Str, STROSD,
                         U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
                         &errorCode);
				//Every language is possible, you can't skip this filter here
                msAPI_OSD_KurdishFilter(outbuf.U16Str, len);
                msAPI_OSD_ReverseAllCharacters(outbuf.U16Str, out.U16Str, len);
                msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, TempBuf.U16Str, len);
                bReverse = FALSE;
            }
            else if ( bArabicDisplay ) //for OSD
            {
                S32 errorCode = U_ZERO_ERROR;

                ArabicParser(TempBuf.U16Str, len, outbuf.U16Str, STROSD,
                         U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
                         &errorCode);
				//just skip this function for non Kurdish condition
                if ( enLanguage == LANGUAGE_OSD_KURDISH )
                {
                    msAPI_OSD_KurdishFilter(outbuf.U16Str, len);
                }
                msAPI_OSD_ReverseAllCharacters(outbuf.U16Str, out.U16Str, len);
                msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, outbuf.U16Str, len);
            }
            else    // for Hebrew
            {

                if ( bDivxSubtitleMode )    // Divx subtitle
                {
                    msAPI_OSD_ReverseAllCharacters(TempBuf.U16Str, outbuf.U16Str, len);
                    msAPI_OSD_ReverseNonArabicHebrewCharacters(outbuf.U16Str, out.U16Str, len);
                    msAPI_OSD_ReverseAllCharacters(out.U16Str, outbuf.U16Str, len);
                }
                else if ( bHebrew )         // Hebrew OSD display
                {
                    msAPI_OSD_ReverseAllCharacters(TempBuf.U16Str, out.U16Str, len);
                    msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, outbuf.U16Str, len);
                }
                else                        // Other conditions
                {
                    msAPI_OSD_ReverseAllCharacters(TempBuf.U16Str, outbuf.U16Str, len);
                    msAPI_OSD_ReverseNonArabicHebrewCharacters(outbuf.U16Str, out.U16Str, len);
                    msAPI_OSD_ReverseAllCharacters(out.U16Str, outbuf.U16Str, len);
                }
            }

            if ( bReverse )
            {
                for(i=0;i<len;i++)
                {
                    TempBuf.U16Str[i] = *(outbuf.U16Str+len-i-1);
                }
            }
        }
        else
        {
			memcpy(TempBuf.U8Str,pu8Str,len*2);
        }

		TempBuf.U16Str[len] = 0;
#endif
     }

#endif
    msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);

    scale = 100;

    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = ((U16)pclrBtn->Fontfmt.height*100 / (U16)finfo.height);
        pclrBtn->Fontfmt.height = scale * finfo.height /100;

        scale = (U8)((U16)pclrBtn->Fontfmt.width*100 / (U16)finfo.width);
        pclrBtn->Fontfmt.width = scale * finfo.width /100;
    }

#if (ENABLE_THAI_OSD)
    if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
    {
        if(pclrBtn->bStringIndexWidth == CHAR_IDX_2BYTE)
        {
            //pu16Str = (U16 *)pu8Str;
			//temp solution for compiler warning
			memcpy(TempBuf.U8Str, pu8Str, len*2);

            len = Thai_compose(TempBuf.U16Str, 0, len, STROSD, outbuf.U16Str);
            outbuf.U16Str[len] = 0x0000;

            pu8Str = outbuf.U8Str;
        }
    }
#endif

    pGlyphDispInfo = msAPI_Memory_Allocate( sizeof(OSDSRC_GLYPH_DISP_INFO) * len,BUF_ID_GLYPH_DISP_INFO);
    if(pGlyphDispInfo == NULL)
    {
        __ASSERT(0);
        APIOSD_DBG(printf("E_DrawText>> pGlyphDispInfo == NULL\n"));
        msAPI_OSD_Free_resource();
        return;
    }

    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
        if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
        {
            for (i=0; i<len; i++)
            {
            #if (ENABLE_ARABIC_OSD)
                if ( bMirrorLang )
                    msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, TempBuf.U8Str[i], &pGlyphDispInfo[i]);
                else
            #endif
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, pu8Str[i], &pGlyphDispInfo[i]);
                pGlyphDispInfo[i].u8Width = pGlyphDispInfo[i].u8Width * scale / 100;
            }
        }
        else
        {
			U8 *pu8TempPtr;
			
#if (ENABLE_ARABIC_OSD)
            if ( bMirrorLang )
			{
				pu8TempPtr = TempBuf.U8Str;
			}
            else
#endif
			{
				pu8TempPtr = pu8Str;
			}
            for (i=0; i<len; i++)
            {
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, (pu8TempPtr[i*2+1]<<8) | pu8TempPtr[i*2], &pGlyphDispInfo[i]);
                pGlyphDispInfo[i].u8Width = (pGlyphDispInfo[i].u8Width * scale
#if ENABLE_DEFAULT_KERNING
                    //- kerningX_(pu16Str[i], pu16Str[i+1])
                    - kerningX_((pu8TempPtr[i*2+1]<<8) | pu8TempPtr[i*2], (pu8TempPtr[i*4+1]<<8) | pu8TempPtr[i*4])
#endif //ENABLE_DEFAULT_KERNING
                    ) / 100;
            }

        #if (ENABLE_ARABIC_OSD)

            //For Subtitle, if OSD is not mirrored status, go this way, the same with subtitle
  		    if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || (External Subtitle Language is Arabic) */ ) )
            {
				U16 u16Unicode;

                for(i=0;i<len;i++)
                {
                    // Check if it is in Arabic Vowel unicode range
                    if ( i > 0 )
                    {
						u16Unicode = (pu8TempPtr[i*2-1]<<8) + pu8TempPtr[i*2-2];											
                        //if ( msAPI_OSD_IsArabicVowel( pu16Str[i-1] ) )
						if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
                        {
                            pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                    }
                    else
                    {
                        //coordinate no need change
                    }
				}
            }
            else if ( bArabicDisplay ) //for OSD
			{
				U16 u16Unicode;

                for(i=0;i<len;i++)
                {
                    // Check if it is in Arabic Vowel unicode range
                    if ( i > 0 )
                    {
						u16Unicode = (pu8TempPtr[i*2+1]<<8) + pu8TempPtr[i*2];						
                        //if ( msAPI_OSD_IsArabicVowel( pu16Str[i] ) )
						if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
                        {
							S8 nOffset = pGlyphDispInfo[i-1].u8Width - pGlyphDispInfo[i].u8Width;

							if ( nOffset > 0 )
								pGlyphDispInfo[i].overlap_info.s8Xoffset = nOffset;
							else
                                pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                    }
                    else
                    {
                        //coordinate no need change
                    }
                }
			}
			else
        #endif
        #if (ENABLE_THAI_OSD)
            if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
            {
                static U8 prevState = 0;
				U16 u16Unicode;

                prevState = 0;

                for(i=0;i<len;i++)
                {
                    // Check Primary Thai unicode range
					u16Unicode = (pu8TempPtr[i*2+1]<<8) + pu8TempPtr[i*2];
                    if ( ( u16Unicode <= 0x0E5B ) && ( u16Unicode >= 0x0E01 ) )
                    {
                        if ( MinusDrawTable[ u16Unicode - 0x0E01 ] )
                        {
                            if ( i > 0 )
                            {
								u16Unicode = (pu8TempPtr[i*2-1]<<8) + pu8TempPtr[i*2-2];
                                if ( ( prevState == 1 ) && ( MinusDrawTable[ u16Unicode - 0x0E01 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else if ( ( prevState == 2 ) && ( MinusDrawTablePersentation[ u16Unicode - 0xF700 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-1].u8X0 + pGlyphDispInfo[i-1].u8Width + 1) - (finfo.width);
                            }
                            else
                            {
                                pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            }

                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                        else
                        {
                            //coordinate no need change
                        }

                        prevState = 1;
                    }
                    else if ( ( u16Unicode <= 0xF71A ) && ( u16Unicode >= 0xF700 ) )
                    {
                        if ( MinusDrawTablePersentation[ u16Unicode - 0xF700 ] )
                        {
                            if ( i > 0 )
                            {
								u16Unicode = (pu8TempPtr[i*2-1]<<8) + pu8TempPtr[i*2-2];								
                                if ( ( prevState == 1 ) && ( MinusDrawTable[ u16Unicode - 0x0E01 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else if ( ( prevState == 2 ) && ( MinusDrawTablePersentation[ u16Unicode - 0xF700 ] ) )
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-2].u8X0 + pGlyphDispInfo[i-2].u8Width + 1) - (finfo.width);
                                else
                                    pGlyphDispInfo[i].overlap_info.s8Xoffset = (pGlyphDispInfo[i-1].u8X0 + pGlyphDispInfo[i-1].u8Width + 1) - (finfo.width);
                            }
                            else
                            {
                                pGlyphDispInfo[i].overlap_info.s8Xoffset = 1;
                            }

                            pGlyphDispInfo[i].overlap_info.s8Yoffset = 0;
                        }
                        else
                        {
                            //coordinate no need change
                        }

                        prevState = 2;
                    }
                    else
                    {
                        //coordinate no need change
                        prevState = 0;
                    }
                }
            }
        #endif
            {
            }
        }
    }
    else
    {
        if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
        {
            for (i=0; i<len; i++)
            {
            #if (ENABLE_ARABIC_OSD)
                if ( bMirrorLang )
                    msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, TempBuf.U8Str[i], &pGlyphDispInfo[i]);
                else
            #endif
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, pu8Str[i], &pGlyphDispInfo[i]);

            }
        }
        else
        {
			U8 *pu8TempPtr;
			
#if (ENABLE_ARABIC_OSD)
            if ( bMirrorLang )
			{
				pu8TempPtr = TempBuf.U8Str;
			}
            else
#endif
			{
				pu8TempPtr = pu8Str;
			}
            for (i=0; i<len; i++)
            {
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, (pu8TempPtr[i*2+1]<<8) | pu8TempPtr[i*2], &pGlyphDispInfo[i]);
            }
        }
    }

    u16StrWidth = 0;
    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
        for (i=0; i< len; i++)
        {
            u16StrWidth += pGlyphDispInfo[i].u8Width;
        }

    #if (ENABLE_ARABIC_OSD)
        if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
        {
            pclrBtn->Fontfmt.ifont_gap = 0;
        }
    #endif

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            u16StrWidth += ((len-1) * pclrBtn->Fontfmt.ifont_gap);
        }
    }
    else  // gap/compact + italic
    {
        // italic or default
        u16StrWidth = finfo.width * scale * len / 100;

        // with gap or compact, only one of two
        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            u16StrWidth +=  ((len-1) * pclrBtn->Fontfmt.ifont_gap);
        }
        else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
        {
            u16StrWidth -=  ((len-1) * pclrBtn->Fontfmt.dis);
        }
    }

    /* draw string */
    switch(pclrBtn->enTextAlign)
    {
        case EN_ALIGNMENT_DEFAULT: //without BTN_TEXT_GAP
            text_info.dstblk.x = pclrBtn->x;
            break;

        case EN_ALIGNMENT_LEFT:
            text_info.dstblk.x = pclrBtn->x + BTN_TEXT_GAP;
            break;

        case EN_ALIGNMENT_RIGHT:
            if (u16StrWidth + BTN_TEXT_GAP > pclrBtn->width)
                text_info.dstblk.x = pclrBtn->x;
            else
                text_info.dstblk.x = pclrBtn->x + pclrBtn->width - BTN_TEXT_GAP - u16StrWidth;
            break;

        case EN_ALIGNMENT_CENTER:
            if (u16StrWidth > pclrBtn->width)
                text_info.dstblk.x = pclrBtn->x;
            else
                text_info.dstblk.x = pclrBtn->x + (pclrBtn->width - u16StrWidth) / 2;
            break;
    }

    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        text_info.dstblk.y = pclrBtn->y + (S16)(pclrBtn->height - finfo.height * scale / 100) / 2;
    }
    else
    {
        text_info.dstblk.y = pclrBtn->y + (S16)(pclrBtn->height - finfo.height) / 2;
    }
    if (text_info.dstblk.y > 32767)
    {
        //overflow
        text_info.dstblk.y = 0;
    }
    #if 0
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        S16 s16Tmp;
        s16Tmp = ((S16)pclrBtn->height - finfo.height * scale / 100) / 2;
        s16Tmp = pclrBtn->y + s16Tmp;
        if (s16Tmp < 0)
        {
            s16Tmp = 0;
        }
        cpoint.y = s16Tmp;
    }
    else
    {
        S16 s16Tmp;
        s16Tmp = ((S16)pclrBtn->height - finfo.height) / 2;
        s16Tmp = pclrBtn->y + s16Tmp;
        if (s16Tmp < 0)
        {
            s16Tmp = 0;
        }
        cpoint.y = s16Tmp;
    }
    #endif

    if (pclrBtn->Fontfmt.flag&GEFONT_FLAG_BOLD)
    {
        pclrBtn->Fontfmt.flag |= GFXFONT_FLAG_BOLD;
    }

    text_info.dstblk.width = pclrBtn->width;
    text_info.dstblk.height = pclrBtn->height;
    text_info.flag = pclrBtn->Fontfmt.flag;
    memcpy(&text_info.color, &pclrBtn->t_clr, 4);
    text_info.pGlyphDispInfo = (GFX_GlyphDispInfo*)pGlyphDispInfo;
    text_info.gap = pclrBtn->Fontfmt.ifont_gap;
#if (ENABLE_THAI_OSD)
	if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && ( External Subtitle Language is Thai ))*/ )
	{
        MApi_GFX_TextOut(font_handle, pu8Str, pclrBtn->bStringIndexWidth+1, &text_info);
	}
	else
#endif
#if (ENABLE_ARABIC_OSD)
	if ( bMirrorLang )
	{
        MApi_GFX_TextOut(font_handle, TempBuf.U8Str, pclrBtn->bStringIndexWidth+1, &text_info);
	}
	else
#endif
    {
        MApi_GFX_TextOut(font_handle, pu8Str, pclrBtn->bStringIndexWidth+1, &text_info);
    }

    if(pGlyphDispInfo != NULL)
    {
        MSAPI_MEMORY_FREE(pGlyphDispInfo,BUF_ID_GLYPH_DISP_INFO);
    }

    msAPI_OSD_Free_resource();
}

#define DBG_SMART_TEXT(x)   //x

/******************************************************************************/
void msAPI_OSD_DrawPunctuatedString_S1(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn, U8 max_row_num)
{
    S8  gap;
    U16 iRow;
    U8  scale, offset_len;
    U16 input_strlen;
    U16 input_char_count, input_last_char_count, output_char_count, output_last_char_count;
    U16 input_char_width=0, output_total_width, output_last_total_width, max_row_width;
    //U16 *u8TempStr;
    U8 u8TempStr[STROSD*2];
    U16 input_char_data;

    OSD_RESOURCE_FONT_INFO finfo;
    OSDSRC_GLYPH_BBOX_X BBox_X_Info;
    BOOLEAN bIsOneByteString;
    BOOLEAN bSetNextLineStartInx    = FALSE;
	BOOLEAN bIsSpecialCharacterUsed = FALSE;
    BOOLEAN bIsChar_CRLF    = FALSE;
    BOOLEAN bIsChar_0x86    = FALSE;
    BOOLEAN bIsChar_0x87    = FALSE;
    BOOLEAN bIsChar_SHY     = FALSE;
    BOOLEAN bIsChar_LastSHY = FALSE;
    BOOLEAN bIsChar_Normal  = FALSE;

    U16 u16OriginalPosition_x   = pclrBtn->x;
    U16 u16OriginalPosition_y   = pclrBtn->y;
    U16 u16OriginalFontfmtFlag  = pclrBtn ->Fontfmt.flag;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringType TempBuf;
	OSDStringTypeDouble outbuf;
	#if ( ENABLE_ARABIC_OSD )
		OSDStringType out;
		U8 bMirrorLang = FALSE;
		U8 bArabicDisplay = FALSE;
		U8 bHebrew = FALSE;
	#endif
	#if ( ENABLE_THAI_OSD )
		U8 bThaiDisplay = FALSE;
	#endif
    U16 nFontID;
#endif
    FONTHANDLE Org_font_handle = font_handle;

//    U32 _timer = msAPI_Timer_GetTime0();

    // Abnormal condition
    if(max_row_num == 0)
    {
        DBG_SMART_TEXT(printf("E_DrawSmartText>> max_row_num = 0\n"););
        return;
    }
    else if ((input_strlen = msAPI_OSD_GetStrLength(pu8Str, pclrBtn->bStringIndexWidth)) == 0)
    {
        DBG_SMART_TEXT(printf("E_DrawSmartText>> strlen = 0\n"););
        return;
    }


    // To handle GE Mutex for HKMCU and MHEG engine
    msAPI_OSD_GET_resource();
    //u8TempStr = (U16*)TempStr;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

	switch(enLanguage)
    {
    #if ( ENABLE_ARABIC_OSD )
        case LANGUAGE_OSD_ARABIC:
        case LANGUAGE_OSD_PARSI:
        case LANGUAGE_OSD_KURDISH:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( bHebrewMixedMode )
					{
                        bHebrew = TRUE;
					}
                    else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
					{
                        font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;
					}

                    if ( !bDivxSubtitleMode )
                    {
                        pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                    }
                }
            }

            if ( bHebrew == FALSE )
			{
                bArabicDisplay = TRUE;
			}
            break;
        case LANGUAGE_OSD_HEBREW:
            bHebrew = TRUE;
            if ( !bDivxSubtitleMode )
            {
                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
            }
            else
            {
                msAPI_OSD_Set_H_Mirror(FALSE);
            }
            break;
    #endif
    #if ( ENABLE_THAI_OSD )
        case LANGUAGE_OSD_THAI:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_THAI_0].fHandle;
                }
            }

	#if ( ENABLE_ARABIC_OSD )
            if ( bHebrew == FALSE )
	#endif
            {
                bThaiDisplay = TRUE;
            }
            break;
    #endif
    /*
        case LANGUAGE_OSD_CHINESE:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
                    {
                        font_handle = Font[nFontID+18].fHandle;
                    }
                    else if ( nFontID >= FONT_EXT_BEGIN )
                    {
                        //Do Chinese icon unicode conversion here
                    }
                }
            }
            break;
    */
        default:
            break;
    }
#endif

#if (ENABLE_ARABIC_OSD)
    if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
    {
        bMirrorLang = TRUE;
    }

#endif

    if(pclrBtn->bStringIndexWidth==CHAR_IDX_1BYTE)
    {
        if (input_strlen >= sizeof(TempStr.U8Str))
        {
            input_strlen = sizeof(TempStr.U8Str) - 1;
        }

    #if (ENABLE_ARABIC_OSD)
        if ( bMirrorLang )
        {
			U16 i;

            memcpy(out.U8Str, pu8Str, input_strlen);
            for(i = 0; i < input_strlen; i++)
            {
                TempBuf.U8Str[i] = out.U8Str[(input_strlen-1)-i];
            }
			TempBuf.U8Str[input_strlen] = 0;
			pu8Str = TempBuf.U8Str;
        }
        else
        {
    #endif
			//memcpy(TempBuf.U8Str,pu8Str,len);
    #if (ENABLE_ARABIC_OSD)
        }
    #endif
    }
    else
	{
	#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
		if (input_strlen >= sizeof(TempBuf.U8Str)/2)
		{
			input_strlen = sizeof(TempBuf.U8Str)/2 - 1;
		}
	#endif

	#if (ENABLE_ARABIC_OSD)
		if ( bMirrorLang )
		{
			memcpy(TempBuf.U8Str,pu8Str,input_strlen*2);
			TempBuf.U16Str[input_strlen] = 0;

			//For Subtitle
			if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
			{
				S32 errorCode = U_ZERO_ERROR;

				ArabicParser(TempBuf.U16Str, input_strlen, outbuf.U16Str, STROSD,
						 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
						 &errorCode);
				//Every language is possible, you can't skip this filter here
				msAPI_OSD_KurdishFilter(outbuf.U16Str, input_strlen);
			}
			else if ( bArabicDisplay ) //for OSD
			{
				S32 errorCode = U_ZERO_ERROR;

				ArabicParser(TempBuf.U16Str, input_strlen, outbuf.U16Str, STROSD,
						 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
						 &errorCode);
				//just skip this function for non Kurdish condition
				if ( enLanguage == LANGUAGE_OSD_KURDISH )
				{
					msAPI_OSD_KurdishFilter(outbuf.U16Str, input_strlen);
				}
			}
			else    // for Hebrew
			{
                            memcpy(outbuf.U8Str, TempBuf.U8Str, input_strlen*2);
			}
			
			       outbuf.U16Str[input_strlen] = 0;
			       pu8Str = outbuf.U8Str;						 
			}
		
	#endif
	}

    msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);
    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = (U8)(pclrBtn->Fontfmt.height / finfo.height);
        pclrBtn->Fontfmt.height = finfo.height * scale;

        scale = (U8)(pclrBtn->Fontfmt.width / finfo.width);
        pclrBtn->Fontfmt.width = finfo.width * scale;
    }
    else
    {
        scale = 1;
    }

#if (ENABLE_THAI_OSD)
	if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
	{
		//temp solution for compiler warning
		memcpy(TempBuf.U8Str, pu8Str, input_strlen*2);

		input_strlen = Thai_compose(TempBuf.U16Str, 0, input_strlen, STROSD, outbuf.U16Str);
		outbuf.U16Str[input_strlen] = 0x0000;

		pu8Str = outbuf.U8Str;
	}
#endif

    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
    {
    #if (ENABLE_ARABIC_OSD)
        if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
        {
            pclrBtn->Fontfmt.ifont_gap = 0;
        }
    #endif

        gap = pclrBtn->Fontfmt.ifont_gap;
    }
    else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
    {
        gap = -pclrBtn->Fontfmt.dis;
    }
    else
    {
        gap = 0;
    }

    bIsOneByteString        = (pclrBtn->bStringIndexWidth & CHAR_IDX_1BYTE) ? TRUE : FALSE;
    max_row_width           = pclrBtn->width - 2*BTN_TEXT_GAP;
    input_last_char_count   = 0;
    output_total_width      = 0;

    DBG_SMART_TEXT(printf("< Start > gap = %bd, input_strlen = %u\n", gap, input_strlen););

    // Row handle loop
    for (iRow=0, input_char_count=0; (iRow<max_row_num) && (input_char_count < input_strlen);)
    {
        if (iRow==1 && bSetNextLineStartInx == FALSE)
        {
            if(0xE08A == ReadU16LE(&pu8Str[input_char_count*2]))
            {
                u16NextLineStartInx = (input_char_count+1);
            }
            else
            {
                u16NextLineStartInx = input_char_count;
            }

            bSetNextLineStartInx = TRUE;
        }

        //if (g_EpgTest)
        {
            DBG_SMART_TEXT(printf("< iRow = %u > input_char_count = %u, input_strlen = %u\n", iRow, input_char_count, input_strlen););
        }

        // Reset variable
        output_char_count       = 0;
        output_last_char_count  = 0;
        output_last_total_width = 0;
        bIsChar_CRLF            = FALSE;
        bIsChar_SHY             = FALSE;
        bIsChar_LastSHY         = FALSE;
        bIsChar_0x86            = FALSE;
        bIsChar_0x87            = FALSE;
        bIsChar_Normal          = FALSE;

        // Filter one word (end with space or control code)
        while ( (output_total_width<max_row_width) && (input_char_count < input_strlen) && ( output_char_count < (STROSD-1)))
        {
            if (bIsOneByteString)   // One byte string
            {
                input_char_data = pu8Str[input_char_count];
                // Contrl code, SHY or space char
                if ((0x20==input_char_data) || ((0x80<=input_char_data)&&(input_char_data<=0x9F)) )
                {
                    input_last_char_count   = input_char_count;
                    output_last_char_count  = output_char_count;    // Save the char count to last control char
                    output_last_total_width = output_total_width;   // Save the display width to last control char

                    if ( 0x8A==input_char_data )            // CR/LF
                    {
                        bIsChar_CRLF        = TRUE;
                    }
                    else if ( 0x86==input_char_data )       // character emphasis on
                    {
                        bIsChar_0x86        = TRUE;
                    }
                    else if ( 0x87==input_char_data )       // character emphasis off
                    {
                        bIsChar_0x87        = TRUE;
                    }
                    else                                    // space char and other
                    {
                        bIsChar_Normal      = TRUE;
                        pu8Str[input_char_count]  = 0x20;
                    }
                }
                else                                        // Normal char
                {
                    bIsChar_Normal      = TRUE;
                }
            }
            else    // Two bytes string
            {
                //input_char_data = pu16Str[input_char_count];
                input_char_data = (pu8Str[input_char_count*2+1]<<8) + pu8Str[input_char_count*2];
                if ( 0x00AD==input_char_data)               // SHY - soft hyphen
                {
					  bIsSpecialCharacterUsed=TRUE;					
                    DBG_SMART_TEXT(printf ("<< SHY [%2u] 0x%04X >>\n", input_char_count, input_char_data););

                    bIsChar_SHY     = TRUE;
                    bIsChar_LastSHY = TRUE;
                }
                else if ((0x0020==input_char_data) || ( (0xE080<=input_char_data) && (input_char_data<=0xE09F) ) )
                {
					   bIsSpecialCharacterUsed=TRUE;					
                    DBG_SMART_TEXT(printf ("<< Control Code [%2u] = 0x%04X >>\n", input_char_count, input_char_data););

                    bIsChar_LastSHY         = FALSE;
                    input_last_char_count   = input_char_count;
                    output_last_char_count  = output_char_count;    // Save the char count to last control char
                    output_last_total_width = output_total_width;   // Save the display width to last control char

                    if ( 0xE08A==input_char_data )          // CR/LF
                    {
                        bIsChar_CRLF        = TRUE;
                    }
                    else if ( 0xE086==input_char_data )     // character emphasis on
                    {
                        bIsChar_0x86        = TRUE;
                    }
                    else if ( 0xE087==input_char_data )     // character emphasis off
                    {
                        bIsChar_0x87        = TRUE;
                    }
                    else                                    // space char and other
                    {
                        bIsChar_Normal      = TRUE;
                        //pu16Str[input_char_count] = 0x0020;
                        pu8Str[input_char_count*2] = 0x20;
                        pu8Str[input_char_count*2+1] = 0x00;
                    }
                }
				  else if (bIsSpecialCharacterUsed==FALSE && (output_total_width + input_char_width >= max_row_width))
				  {

						
                    bIsChar_LastSHY         = FALSE;
					  input_last_char_count   = input_char_count-1;
					  output_last_char_count  = output_char_count;    // Save the char count to last control char
					  output_last_total_width = output_total_width;   // Save the display width to last control char
					  bIsChar_CRLF        = TRUE;				
				  }
                else                                         // Normal char
                {
                    bIsChar_Normal          = TRUE;
                }
            }

            // Retrieve input char width or add null char
            if (bIsChar_Normal || bIsChar_SHY)
            {
                if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
                {
                    if (bIsOneByteString)   // One byte string
                    {
                        msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu8Str[input_char_count], &BBox_X_Info);
                    }
                    else
                    {
		            #if (ENABLE_ARABIC_OSD)
						//For Subtitle
						if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || (External Subtitle Language is Arabic) */ ) )
						{
							U16 u16Unicode;

							// Check if it is in Arabic Vowel unicode range
							if ( input_char_count > 0 )
							{
								u16Unicode = (pu8Str[input_char_count*2-1]<<8) + pu8Str[input_char_count*2-2];

								if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
									//in Arabic language mode, gap is always zero, so we can forget gap value
									output_total_width -= (BBox_X_Info.u8Width*scale+gap);
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else
							{
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
								input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);								
							}
						}
						else if ( bArabicDisplay ) //for OSD
						{
							// Check if it is in Arabic Vowel unicode range
							if ( input_char_count > 0 )
							{
								if ( msAPI_OSD_IsArabicVowel( input_char_data ) )
								{
									//skip current vowel character width
									//do nothing here
									input_char_width = 0;
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else
							{
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
								input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);								
							}
						}
						else
		            #endif
		            #if (ENABLE_THAI_OSD)
						if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
						{
							// Check Primary Thai unicode range
							if ( ( input_char_data <= 0x0E5B ) && ( input_char_data >= 0x0E01 ) )
							{
								if ( MinusDrawTable[ input_char_data - 0x0E01 ] )
								{
									//Minus coordinate, don't add width
									input_char_width = 0;
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else if ( ( input_char_data <= 0xF71A ) && ( input_char_data >= 0xF700 ) )
							{
								if ( MinusDrawTablePersentation[ input_char_data - 0xF700 ] )
								{
									//Minus coordinate, don't add width
									input_char_width = 0;
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else
							{
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
								input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);
							}
						}
						else
		            #endif
		                {
                            msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
                            input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);
                        }
                    }
                }
                else
                {
                    input_char_width = finfo.width;
                    input_char_width = (output_char_count==0) ? (finfo.width) : (finfo.width*scale+gap);
                }

                bIsChar_Normal = FALSE;
            }
            else
            {
                if(bIsOneByteString)
                {
                    //((U8*)u8TempStr)[output_char_count] = 0;
                    u8TempStr[output_char_count] = 0;
                }
                else
                {
                    //u8TempStr[output_char_count] = 0;
                    u8TempStr[output_char_count*2] = 0;
                    u8TempStr[output_char_count*2+1] = 0;
                }

                input_char_count++;
                break;
            }

            // check if string width larger than OSD display width
            if ((output_total_width+input_char_width) < max_row_width)
            {
                if (bIsChar_SHY)    // add normal char and space
                {
                    // Save temporary Last SHY char position for display
                    bIsChar_SHY             = FALSE;
                    input_last_char_count   = input_char_count;
                    output_last_char_count  = output_char_count;    // Save the char count to last control char
                    output_last_total_width = output_total_width;   // Save the display width to last control char
                }
                else                // space and other char
                {
                    if(bIsOneByteString)
                    {
                        //((U8*)u8TempStr)[output_char_count++] = pu8Str[input_char_count];
                        u8TempStr[output_char_count++] = pu8Str[input_char_count];
                    }
                    else
                    {
                        //u8TempStr[output_char_count++] = pu16Str[input_char_count];
                        u8TempStr[(output_char_count)*2] = pu8Str[input_char_count*2];
                        u8TempStr[(output_char_count)*2+1] = pu8Str[input_char_count*2+1];
                        output_char_count++;
                    }

                    output_total_width += input_char_width;
                }

                //if (g_EpgTest)
                {
                    DBG_SMART_TEXT(printf("pu16Str[%2u]= 0x%04x(%c), input_char_width = %02u, last_total_width = %4u, output_total_width = %4u, max_row_width =%4u\n", input_char_count, pu16Str[input_char_count], (U8)pu16Str[input_char_count], input_char_width, output_last_total_width, output_total_width, max_row_width););
                }

                input_char_count++;
                if (input_char_count>=input_strlen)     // Draw to the last char
                {
                    bIsChar_LastSHY = FALSE;
                    break;
                }
            }
            else
            {
                if(bIsOneByteString)
                {
                    //((U8*)u8TempStr)[output_last_char_count] = 0;
                    u8TempStr[output_last_char_count] = 0;
                }
                else
                {
                    //u8TempStr[output_last_char_count] = 0;
                    u8TempStr[output_last_char_count*2] = 0;
                    u8TempStr[output_last_char_count*2+1] = 0;
                }
                break;
            }
        }           // End of while loop

        //if (g_EpgTest)
        {
            DBG_SMART_TEXT(printf("1 >> In: char_count= %2u(0x%4X), last_char_count = %2u, Out: char_count = %2u, last_char_count = %2u\n", input_char_count, input_char_data, input_last_char_count, output_char_count, output_last_char_count););
        }

        // Draw text to last control char or space char
        //if ( (output_last_char_count > 0) || (output_char_count > 0))
        if ( output_char_count < (STROSD-1))        //coverity ,prevent out of bounds read for u8TempStr[] 
        {
            // Control code handle
            if (bIsChar_0x86 || bIsChar_0x87)          // character emphasis on/off
            {
                msAPI_OSD_Draw2BytesText(Org_font_handle, u8TempStr, pclrBtn);

                pclrBtn->x = u16OriginalPosition_x + output_last_total_width;

                if (bIsChar_0x86)
                {
                    pclrBtn->Fontfmt.flag |= GEFONT_FLAG_BOLD;
                }
                else
                {
                    pclrBtn->Fontfmt.flag = u16OriginalFontfmtFlag;
                }
            }
            else
            {
                if(bIsChar_LastSHY && ((pu8Str[input_last_char_count*2+1]<<8) + pu8Str[input_last_char_count*2] == 0x20) )
                    bIsChar_LastSHY = FALSE;

                if (bIsChar_LastSHY)        // Last SHY - soft hyphen
                {
                    u8TempStr[output_last_char_count*2]   = 0xAD; //0x00AD
                    u8TempStr[(output_last_char_count*2)+1] = 0x00;
                    u8TempStr[(output_last_char_count*2)+2] = 0x00; //0x0000
                    u8TempStr[(output_last_char_count*2)+3] = 0x00;

                }
                else                        // Space char or other
                {
                    u8TempStr[(output_char_count*2)]   = 0;
                    u8TempStr[(output_char_count*2)+1]   = 0;
                }

                msAPI_OSD_Draw2BytesText(font_handle, u8TempStr, pclrBtn);
                pclrBtn->x  = u16OriginalPosition_x;
                pclrBtn->y += pclrBtn->height;
                output_total_width     = 0;
				  bIsSpecialCharacterUsed = FALSE;
                iRow++;
            }
            offset_len = 1; // the var needs to be initialized
            if (input_char_count < input_strlen)
            {
                input_char_count = input_last_char_count+1;
            }
        }

        //if (g_EpgTest)
        {
            DBG_SMART_TEXT(printf("2 >> In: char_count= %2u(0x%4X), last_char_count = %2u, Out: char_count = %2u, last_char_count = %2u\n", input_char_count, input_char_data, input_last_char_count, output_char_count, output_last_char_count););
            DBG_SMART_TEXT(printf ("<< (x, y) = (%4u, %4u) >>\n", pclrBtn->x, pclrBtn->y););
        }

        if (input_char_count < input_strlen)
        {
            bStopScrollDown = FALSE;
        }
        else
        {
            bStopScrollDown = TRUE;
        }
    }

    pclrBtn->y = u16OriginalPosition_y;

    // To handle GE Mutex for HKMCU and MHEG engine
    msAPI_OSD_Free_resource();

    //printf ("Time = %lu\n", _timer = msAPI_Timer_DiffTimeFromNow(_timer));
}
#endif

void msAPI_OSD_DrawPunctuatedString_S2(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn, U8 max_row_num)
{
    S8  gap;
    U16 iRow;
    U8  scale, offset_len;
    U16 input_strlen;
    U16 input_char_count, input_last_char_count, output_char_count, output_last_char_count;
    U16 input_char_width, output_total_width, output_last_total_width, max_row_width;
    //U16 *u8TempStr;
    U8 u8TempStr[STROSD*2];
    U16 input_char_data;

    OSD_RESOURCE_FONT_INFO finfo;
    OSDSRC_GLYPH_BBOX_X BBox_X_Info;
    BOOLEAN bIsOneByteString;
    BOOLEAN bSetNextLineStartInx    = FALSE;
    BOOLEAN bIsChar_CRLF    = FALSE;
    BOOLEAN bIsChar_0x86    = FALSE;
    BOOLEAN bIsChar_0x87    = FALSE;
    BOOLEAN bIsChar_SHY     = FALSE;
    BOOLEAN bIsChar_LastSHY = FALSE;
    BOOLEAN bIsChar_Normal  = FALSE;

    U16 u16OriginalPosition_x   = pclrBtn->x;
    U16 u16OriginalPosition_y   = pclrBtn->y;
    U16 u16OriginalFontfmtFlag  = pclrBtn ->Fontfmt.flag;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringType TempBuf;
	OSDStringTypeDouble outbuf;
	#if ( ENABLE_ARABIC_OSD )
		OSDStringType out;
		U8 bMirrorLang = FALSE;
		U8 bArabicDisplay = FALSE;
		U8 bHebrew = FALSE;
	#endif
	#if ( ENABLE_THAI_OSD )
		U8 bThaiDisplay = FALSE;
	#endif
    U16 nFontID;
#endif
    FONTHANDLE Org_font_handle = font_handle;

//    U32 _timer = msAPI_Timer_GetTime0();

    // Abnormal condition
    if(max_row_num == 0)
    {
        DBG_SMART_TEXT(printf("E_DrawSmartText>> max_row_num = 0\n"););
        return;
    }
    else if ((input_strlen = msAPI_OSD_GetStrLength(pu8Str, pclrBtn->bStringIndexWidth)) == 0)
    {
        DBG_SMART_TEXT(printf("E_DrawSmartText>> strlen = 0\n"););
        return;
    }


    // To handle GE Mutex for HKMCU and MHEG engine
    msAPI_OSD_GET_resource();
    //u8TempStr = (U16*)TempStr;

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	switch(enLanguage)
    {
    #if ( ENABLE_ARABIC_OSD )
        case LANGUAGE_OSD_ARABIC:
        case LANGUAGE_OSD_PARSI:
        case LANGUAGE_OSD_KURDISH:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( bHebrewMixedMode )
					{
                        bHebrew = TRUE;
					}
                    else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
					{
                        font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;
					}

                    if ( !bDivxSubtitleMode )
                    {
                        pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                    }
                }
            }

            if ( bHebrew == FALSE )
			{
                bArabicDisplay = TRUE;
			}
            break;
        case LANGUAGE_OSD_HEBREW:
            bHebrew = TRUE;
            if ( !bDivxSubtitleMode )
            {
                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
            }
            else
            {
                msAPI_OSD_Set_H_Mirror(FALSE);
            }
            break;
    #endif
    #if ( ENABLE_THAI_OSD )
        case LANGUAGE_OSD_THAI:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
            /*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
            */
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_THAI_0].fHandle;
                }
            }

	#if ( ENABLE_ARABIC_OSD )
            if ( bHebrew == FALSE )
	#endif
            {
                bThaiDisplay = TRUE;
            }
            break;
    #endif
    /*
        case LANGUAGE_OSD_CHINESE:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
                    {
                        font_handle = Font[nFontID+18].fHandle;
                    }
                    else if ( nFontID >= FONT_EXT_BEGIN )
                    {
                        //Do Chinese icon unicode conversion here
                    }
                }
            }
            break;
    */
        default:
            break;
    }
#endif

#if (ENABLE_ARABIC_OSD)
    if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
    {
        bMirrorLang = TRUE;
    }
#endif

    if(pclrBtn->bStringIndexWidth==CHAR_IDX_1BYTE)
    {
        if (input_strlen >= sizeof(TempStr.U8Str))
        {
            input_strlen = sizeof(TempStr.U8Str) - 1;
        }

    #if (ENABLE_ARABIC_OSD)
        if ( bMirrorLang )
        {
			U16 i;

            memcpy(out.U8Str, pu8Str, input_strlen);
            for(i = 0; i < input_strlen; i++)
            {
                TempBuf.U8Str[i] = out.U8Str[(input_strlen-1)-i];
            }
			TempBuf.U8Str[input_strlen] = 0;
			pu8Str = TempBuf.U8Str;
        }
        else
        {
    #endif
    		//memcpy(TempBuf.U8Str,pu8Str,len);
    #if (ENABLE_ARABIC_OSD)
        }
    #endif
    }
    else
	{
	#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

		if (input_strlen >= sizeof(TempBuf.U8Str)/2)
		{
			input_strlen = sizeof(TempBuf.U8Str)/2 - 1;
		}

	#endif

	#if (ENABLE_ARABIC_OSD)

		if ( bMirrorLang )
		{
			memcpy(TempBuf.U8Str,pu8Str,input_strlen*2);
			TempBuf.U16Str[input_strlen] = 0;

			//For Subtitle
			if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
			{
				S32 errorCode = U_ZERO_ERROR;

				ArabicParser(TempBuf.U16Str, input_strlen, outbuf.U16Str, STROSD,
						 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
						 &errorCode);
				//Every language is possible, you can't skip this filter here
				msAPI_OSD_KurdishFilter(outbuf.U16Str, input_strlen);
			}
			else if ( bArabicDisplay ) //for OSD
			{
				S32 errorCode = U_ZERO_ERROR;

				ArabicParser(TempBuf.U16Str, input_strlen, outbuf.U16Str, STROSD,
						 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
						 &errorCode);
				//just skip this function for non Kurdish condition
				if ( enLanguage == LANGUAGE_OSD_KURDISH )
				{
					msAPI_OSD_KurdishFilter(outbuf.U16Str, input_strlen);
				}
			}
			else    // for Hebrew
			{
                memcpy(outbuf.U8Str, TempBuf.U8Str, input_strlen*2);
			}

			outbuf.U16Str[input_strlen] = 0;
			pu8Str = outbuf.U8Str;
		}

	#endif
	}

    msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);
    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = (U8)(pclrBtn->Fontfmt.height / finfo.height);
        pclrBtn->Fontfmt.height = finfo.height * scale;

        scale = (U8)(pclrBtn->Fontfmt.width / finfo.width);
        pclrBtn->Fontfmt.width = finfo.width * scale;
    }
    else
    {
        scale = 1;
    }

#if (ENABLE_THAI_OSD)
	if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
	{
		//temp solution for compiler warning
		memcpy(TempBuf.U8Str, pu8Str, input_strlen*2);

		input_strlen = Thai_compose(TempBuf.U16Str, 0, input_strlen, STROSD, outbuf.U16Str);
		outbuf.U16Str[input_strlen] = 0x0000;

		pu8Str = outbuf.U8Str;
	}
#endif

    if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
    {
    #if (ENABLE_ARABIC_OSD)
        if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
        {
            pclrBtn->Fontfmt.ifont_gap = 0;
        }
    #endif

        gap = pclrBtn->Fontfmt.ifont_gap;
    }
    else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
    {
        gap = -pclrBtn->Fontfmt.dis;
    }
    else
    {
        gap = 0;
    }

    bIsOneByteString        = (pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE) ? TRUE : FALSE;
    max_row_width           = pclrBtn->width - 2*BTN_TEXT_GAP;
    input_last_char_count   = 0;
    output_total_width      = 0;

    DBG_SMART_TEXT(printf("< Start > gap = %bd, input_strlen = %u\n", gap, input_strlen););

    // Row handle loop
    for (iRow=0, input_char_count=0; (iRow<max_row_num) && (input_char_count < input_strlen);)
    {
        if (iRow==1  && bSetNextLineStartInx == FALSE)
        {
            u16NextLineStartInx = input_char_count;
            bSetNextLineStartInx = TRUE;
        }

        //if (g_EpgTest)
        {
            DBG_SMART_TEXT(printf("< iRow = %u > input_char_count = %u, input_strlen = %u\n", iRow, input_char_count, input_strlen););
        }

        // Reset variable
        output_char_count       = 0;
        output_last_char_count  = 0;
        output_last_total_width = 0;
        bIsChar_CRLF            = FALSE;
        bIsChar_SHY             = FALSE;
        bIsChar_LastSHY         = FALSE;
        bIsChar_0x86            = FALSE;
        bIsChar_0x87            = FALSE;
        bIsChar_Normal          = FALSE;

        // Filter one word (end with space or control code)
        while ( (output_total_width<max_row_width) && (input_char_count < input_strlen) && ( output_char_count < (STROSD-1)))
        {
            if (bIsOneByteString)   // One byte string
            {
                input_char_data = pu8Str[input_char_count];
                // Contrl code, SHY or space char
                if ((0x20==input_char_data) || ((0x80<=input_char_data)&&(input_char_data<=0x9F)) )
                {
                    input_last_char_count   = input_char_count;
                    output_last_char_count  = output_char_count;    // Save the char count to last control char
                    output_last_total_width = output_total_width;   // Save the display width to last control char

                    if ( 0x8A==input_char_data )            // CR/LF
                    {
                        bIsChar_CRLF        = TRUE;
                    }
                    else if ( 0x86==input_char_data )       // character emphasis on
                    {
                        bIsChar_0x86        = TRUE;
                    }
                    else if ( 0x87==input_char_data )       // character emphasis off
                    {
                        bIsChar_0x87        = TRUE;
                    }
                    else                                    // space char and other
                    {
                        bIsChar_Normal      = TRUE;
                        pu8Str[input_char_count]  = 0x20;
                    }
                }
                else                                        // Normal char
                {
                    bIsChar_Normal      = TRUE;
                }
            }
            else    // Two bytes string
            {
                //input_char_data = pu16Str[input_char_count];
                input_char_data = (pu8Str[input_char_count*2+1]<<8) + pu8Str[input_char_count*2];
                if ( 0x00AD==input_char_data)               // SHY - soft hyphen
                {
                    DBG_SMART_TEXT(printf ("<< SHY [%2u] 0x%04X >>\n", input_char_count, input_char_data););

                    bIsChar_SHY     = TRUE;
                    bIsChar_LastSHY = TRUE;
                }
                else if ((0x0020==input_char_data) || ( (0xE080<=input_char_data) && (input_char_data<=0xE09F) ) )
                {
                    DBG_SMART_TEXT(printf ("<< Control Code [%2u] = 0x%04X >>\n", input_char_count, input_char_data););

                    bIsChar_LastSHY         = FALSE;
                    input_last_char_count   = input_char_count;
                    output_last_char_count  = output_char_count;    // Save the char count to last control char
                    output_last_total_width = output_total_width;   // Save the display width to last control char

                    if ( 0xE08A==input_char_data )          // CR/LF
                    {
                        bIsChar_CRLF        = TRUE;
                    }
                    else if ( 0xE086==input_char_data )     // character emphasis on
                    {
                        bIsChar_0x86        = TRUE;
                    }
                    else if ( 0xE087==input_char_data )     // character emphasis off
                    {
                        bIsChar_0x87        = TRUE;
                    }
                    else                                    // space char and other
                    {
                        bIsChar_Normal      = TRUE;
                        //pu16Str[input_char_count] = 0x0020;
                        pu8Str[input_char_count*2] = 0x20;
                        pu8Str[input_char_count*2+1] = 0x00;
                    }
                }
                /////////////////////////////////////////////////////////////////////////////////
                //else if( (0x000D == input_char_data) && (0x000A == pu16Str[input_char_count+1]) )
                else if( (0x000D == input_char_data) && (0x000A == ((pu8Str[input_char_count*2+3]<<8) + pu8Str[input_char_count*2])) )
                {
                    bIsChar_LastSHY = TRUE;
                }
                /////////////////////////////////////////////////////////////////////////////////
                else                                        // Normal char
                {
                    bIsChar_Normal          = TRUE;
                }
            }

            // Retrieve input char width or add null char
            if (bIsChar_Normal || bIsChar_SHY)
            {
                if(pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
                {
                    if (bIsOneByteString)   // One byte string
                    {
                        msAPI_OSD_RESOURCE_GetBBox_X(font_handle, pu8Str[input_char_count], &BBox_X_Info);
                    }
                    else
                    {
		            #if (ENABLE_ARABIC_OSD)
						//For Subtitle
						if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || (External Subtitle Language is Arabic) */ ) )
						{
							U16 u16Unicode;

							// Check if it is in Arabic Vowel unicode range
							if ( input_char_count > 0 )
							{
								u16Unicode = (pu8Str[input_char_count*2-1]<<8) + pu8Str[input_char_count*2-2];

								if ( msAPI_OSD_IsArabicVowel( u16Unicode ) )
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, u16Unicode, &BBox_X_Info);
									//in Arabic language mode, gap is always zero, so we can forget gap value
									output_total_width -= (BBox_X_Info.u8Width*scale+gap);
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else
							{
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
								input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);								
							}
						}
						else if ( bArabicDisplay ) //for OSD
						{
							// Check if it is in Arabic Vowel unicode range
							if ( input_char_count > 0 )
							{
								if ( msAPI_OSD_IsArabicVowel( input_char_data ) )
								{
									//skip current vowel character width
									//do nothing here
									input_char_width = 0;
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else
							{
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
								input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);								
							}
						}
						else
		            #endif
		            #if (ENABLE_THAI_OSD)
						if ( ( bThaiDisplay ) /*|| ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
						{
							// Check Primary Thai unicode range
							if ( ( input_char_data <= 0x0E5B ) && ( input_char_data >= 0x0E01 ) )
							{
								if ( MinusDrawTable[ input_char_data - 0x0E01 ] )
								{
									//Minus coordinate, don't add width
									input_char_width = 0;
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else if ( ( input_char_data <= 0xF71A ) && ( input_char_data >= 0xF700 ) )
							{
								if ( MinusDrawTablePersentation[ input_char_data - 0xF700 ] )
								{
									//Minus coordinate, don't add width
									input_char_width = 0;
								}
								else
								{
									msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
									input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);									
								}
							}
							else
							{
								msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
								input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);
							}
						}
						else
            		#endif
            		    {
                            msAPI_OSD_RESOURCE_GetBBox_X(font_handle, input_char_data, &BBox_X_Info);
                            input_char_width = (output_char_count==0) ? (BBox_X_Info.u8Width*scale) : (BBox_X_Info.u8Width*scale+gap);
            		    }
                    }
                }
                else
                {
                    input_char_width = finfo.width;
                    input_char_width = (output_char_count==0) ? (finfo.width) : (finfo.width*scale+gap);
                }

                bIsChar_Normal = FALSE;
            }
            else
            {
                if(bIsOneByteString)
                {
                    //((U8*)u8TempStr)[output_char_count] = 0;
                    u8TempStr[output_char_count] = 0;
                }
                else
                {
                    //u8TempStr[output_char_count] = 0;
                    u8TempStr[output_char_count*2] = 0;
                    u8TempStr[output_char_count*2+1] = 0;
                    /////////////////////////////////////////////////////////////////////////////////
                    input_char_count++;
                    output_last_char_count   = output_char_count;
                    input_last_char_count = input_char_count;
                    break;
                    /////////////////////////////////////////////////////////////////////////////////
                }

                input_char_count++;
                break;
            }

            // check if string width larger than OSD display width
            if ((output_total_width+input_char_width) < max_row_width)
            {
                if (bIsChar_SHY)    // add normal char and space
                {
                    // Save temporary Last SHY char position for display
                    bIsChar_SHY             = FALSE;
                    input_last_char_count   = input_char_count;
                    output_last_char_count  = output_char_count;    // Save the char count to last control char
                    output_last_total_width = output_total_width;   // Save the display width to last control char
                }
                else                // space and other char
                {
                    if(bIsOneByteString)
                    {
                        //((U8*)u8TempStr)[output_char_count++] = pu8Str[input_char_count];
                        u8TempStr[output_char_count++] = pu8Str[input_char_count];
                    }
                    else
                    {
                        //u8TempStr[output_char_count++] = pu16Str[input_char_count];
                        u8TempStr[(output_char_count++)*2] = pu8Str[input_char_count*2];
                        u8TempStr[(output_char_count++)*2+1] = pu8Str[input_char_count*2+1];

                        /////////////////////////////////////////////////////////////////////////////////
                        output_last_char_count   = output_char_count;
                        input_last_char_count = input_char_count;
                        /////////////////////////////////////////////////////////////////////////////////
                    }

                    output_total_width += input_char_width;
                }

                //if (g_EpgTest)
                {
                    DBG_SMART_TEXT(printf("pu16Str[%2u]= 0x%04x(%c), input_char_width = %02u, last_total_width = %4u, output_total_width = %4u, max_row_width =%4u\n", input_char_count, pu16Str[input_char_count], (U8)pu16Str[input_char_count], input_char_width, output_last_total_width, output_total_width, max_row_width););
                }

                input_char_count++;
                if (input_char_count>=input_strlen)     // Draw to the last char
                {
                    bIsChar_LastSHY = FALSE;
                    break;
                }
            }
            else
            {
                if(bIsOneByteString)
                {
                    //((U8*)u8TempStr)[output_last_char_count] = 0;
                    u8TempStr[output_last_char_count] = 0;
                }
                else
                {
                    //u8TempStr[output_last_char_count] = 0;
                    u8TempStr[output_last_char_count*2] = 0;
                    u8TempStr[output_last_char_count*2+1] = 0;
                }
                break;
            }
        }           // End of while loop

        //if (g_EpgTest)
        {
            DBG_SMART_TEXT(printf("1 >> In: char_count= %2u(0x%4X), last_char_count = %2u, Out: char_count = %2u, last_char_count = %2u\n", input_char_count, input_char_data, input_last_char_count, output_char_count, output_last_char_count););
        }

        // Draw text to last control char or space char
        //if ( (output_last_char_count > 0) || (output_char_count > 0))
        {
            // Control code handle
            if (bIsChar_0x86 || bIsChar_0x87)          // character emphasis on/off
            {
                msAPI_OSD_Draw2BytesText(Org_font_handle, u8TempStr, pclrBtn);

                pclrBtn->x = u16OriginalPosition_x + output_last_total_width;

                if (bIsChar_0x86)
                {
                    pclrBtn->Fontfmt.flag |= GEFONT_FLAG_BOLD;
                }
                else
                {
                    pclrBtn->Fontfmt.flag = u16OriginalFontfmtFlag;
                }
            }
            else
            {
                if (bIsChar_LastSHY)        // Last SHY - soft hyphen
                {
                    u8TempStr[output_last_char_count*2]   = 0xAD; //0x00AD
                    u8TempStr[(output_last_char_count*2)+1] = 0x00;
                    u8TempStr[(output_last_char_count*2)+2] = 0x00; //0x0000   
                    u8TempStr[(output_last_char_count*2)+3] = 0x00;
                }
                else                        // Space char or other
                {
                    u8TempStr[(output_char_count*2)]   = 0;
                    u8TempStr[(output_char_count*2)+1]   = 0;
                }

                msAPI_OSD_Draw2BytesText(Org_font_handle, u8TempStr, pclrBtn);
                pclrBtn->x  = u16OriginalPosition_x;
                pclrBtn->y += pclrBtn->height;
                output_total_width     = 0;
                iRow++;
            }
			
			if(bIsOneByteString)
				offset_len = 1;
			else
				offset_len = 2; // the var needs to be initialized
            if (input_char_count < input_strlen)
            {
                input_char_count = input_last_char_count+1;
                pu8Str += (input_last_char_count*offset_len);
            }
        }

        //if (g_EpgTest)
        {
            DBG_SMART_TEXT(printf("2 >> In: char_count= %2u(0x%4X), last_char_count = %2u, Out: char_count = %2u, last_char_count = %2u\n", input_char_count, input_char_data, input_last_char_count, output_char_count, output_last_char_count););
            DBG_SMART_TEXT(printf ("<< (x, y) = (%4u, %4u) >>\n", pclrBtn->x, pclrBtn->y););
        }

        if (input_char_count < input_strlen)
        {
            bStopScrollDown = FALSE;
        }
        else
        {
            bStopScrollDown = TRUE;
        }
    }

    pclrBtn->y = u16OriginalPosition_y;

    // To handle GE Mutex for HKMCU and MHEG engine
    msAPI_OSD_Free_resource();

    //printf ("Time = %lu\n", _timer = msAPI_Timer_DiffTimeFromNow(_timer));
}

void msAPI_OSD_DrawClippedString(FONTHANDLE font_handle, U16 *pu16Str, OSDClrBtn *pclrBtn, EN_STRING_ENDING_TYPE enEndingType)
{
    S8 gap;
    U8 cut_strlen,dot_count;
    U16 total_strlen, width, u16Char, scale;
    U16 width_offset = 0x00;
    U16 max_width, dot_width;
    GFX_TextOutInfo text_info;
    OSD_RESOURCE_FONT_INFO finfo;
//    OSDSRC_GLYPH_BBOX_X BBox_X_Info;

    //GFX_GlyphBbox_X BBox_X_Info;
    U8 bExceed = FALSE;
    OSDSRC_GLYPH_DISP_INFO *pGlyphDispInfo = NULL;
    OSDSRC_GLYPH_DISP_INFO pglyphDot[3];
//    GEClrPoint cpoint;
    U16 pu16dotStr[4];

#if ENABLE_SHORT_STRING_BUFFER //speed up drawing string
    OSDSRC_GLYPH_DISP_INFO _s_temp_glyph[32];
#endif

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )
	OSDStringType TempBuf;
	OSDStringTypeDouble outbuf;
	#if ( ENABLE_ARABIC_OSD )
		OSDStringType out;
		U8 bMirrorLang = FALSE;
		U8 bArabicDisplay = FALSE;
		U8 bHebrew = FALSE;
	#endif
	#if ( ENABLE_THAI_OSD )
		U8 bThaiDisplay = FALSE;
	#endif
    U16 nFontID;
#endif

    msAPI_OSD_GET_resource();
//printf("3 strwid =%x, CHAR_IDX_2BYTE =%x\n", pclrBtn->bStringIndexWidth, CHAR_IDX_2BYTE);
    /*prevent error*/
    if (pclrBtn->bStringIndexWidth != CHAR_IDX_2BYTE)
    {
        APIOSD_DBG(printf("Only support 2 bytes strubg\n"));
        msAPI_OSD_Free_resource();
        return;
    }

    if ((total_strlen = msAPI_OSD_GetStrLength((U8*)pu16Str, pclrBtn->bStringIndexWidth)) == 0)
    {
        APIOSD_DBG(printf("E_DrawSmartText>> strlen = 0\n"));
        msAPI_OSD_Free_resource();
        return;
    }

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

	switch(enLanguage)
    {
#if ( ENABLE_ARABIC_OSD )
        case LANGUAGE_OSD_ARABIC:
        case LANGUAGE_OSD_PARSI:
        case LANGUAGE_OSD_KURDISH:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
/*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
					msAPI_OSD_Set_H_Mirror(FALSE);
					//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
                }
                else
*/
                {
                    if ( bHebrewMixedMode )
					{
                        bHebrew = TRUE;
					}
                    else if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
					{
                        font_handle = Font[nFontID+FONT_ARABIC_0].fHandle;
					}

                    if ( !bDivxSubtitleMode )
                    {
                        pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                        msAPI_GE_SetMirror(1, 0);    //patch GE driver
						//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), TRUE);
						msAPI_OSD_Set_H_Mirror(TRUE);
                    }
                    else
                    {
						msAPI_OSD_Set_H_Mirror(FALSE);
						//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
                    }
                }
            }

            if ( bHebrew == FALSE )
			{
                bArabicDisplay = TRUE;
			}
            break;
        case LANGUAGE_OSD_HEBREW:
            bHebrew = TRUE;
            if ( !bDivxSubtitleMode )
            {
                pclrBtn->Fontfmt.flag |= GEFONT_FLAG_MIRROR;
                msAPI_GE_SetMirror(1, 0);    //patch GE driver
				msAPI_OSD_Set_H_Mirror(TRUE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), TRUE);
            }
            else
            {
                msAPI_OSD_Set_H_Mirror(FALSE);
            }
            break;
#endif
#if ( ENABLE_THAI_OSD )
        case LANGUAGE_OSD_THAI:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
/*
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
*/
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) /*&& ( !bDivxSubtitleText )*/ )
                        font_handle = Font[nFontID+FONT_THAI_0].fHandle;
                }
            }

	#if ( ENABLE_ARABIC_OSD )
            if ( bHebrew == FALSE )
	#endif
            {
                bThaiDisplay = TRUE;
				msAPI_OSD_Set_H_Mirror(FALSE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            }
            break;
#endif
/*
        case LANGUAGE_OSD_CHINESE:
            nFontID = FontHandleIndex[font_handle];
            if ( nFontID < FONT_MULTILANG_BEGIN )
            {
                if ( (bDivxSubtitleMode) && ( External Subtitle Language is Hebrew ) )
                {
                    bHebrew = TRUE;
                }
                else
                {
                    if ( ( nFontID < FONT_EXT_BEGIN ) && ( !bDivxSubtitleText ) )
                    {
                        font_handle = Font[nFontID+18].fHandle;
                    }
                    else if ( nFontID >= FONT_EXT_BEGIN )
                    {
                        //Do Chinese icon unicode conversion here
                    }
                }
            }
            if ( bHebrew == FALSE )
            {
				msAPI_OSD_Set_H_Mirror(FALSE);
				//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            }
            break;
*/
        default:
			msAPI_OSD_Set_H_Mirror(FALSE);
			//MApi_GOP_Enable_GWIN_HMirror(MApi_GOP_GWIN_GetCurrentGOP(), FALSE);
            break;
    }
#endif

#if (ENABLE_ARABIC_OSD)

    if ( (bArabicDisplay) || (bHebrew) /*|| ( External Subtitle Language is Arabic ) || ( External Subtitle Language is Hebrew )*/ )
    {
        bMirrorLang = TRUE;
    }

#endif

#if ( (ENABLE_ARABIC_OSD) || (ENABLE_THAI_OSD) )

	if (total_strlen >= sizeof(TempBuf.U8Str)/2)
	{
		total_strlen = sizeof(TempBuf.U8Str)/2 - 1;
	}

	#if	( ENABLE_ARABIC_OSD )
	if ( bMirrorLang )
	{
		U8 bReverse = TRUE;
		U16 i;

		memcpy(TempBuf.U8Str,(U8 *)pu16Str,total_strlen*2);
		//((U16 *)TempBuf)[total_strlen] = 0;
		TempBuf.U16Str[total_strlen] = 0;

		//For Subtitle
		if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /*|| ( External Subtitle Language is Arabic )*/ ) )
		{
			S32 errorCode = U_ZERO_ERROR;

			ArabicParser(TempBuf.U16Str, total_strlen, outbuf.U16Str, STROSD,
					 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
					 &errorCode);
			//Every language is possible, you can't skip this filter here
			msAPI_OSD_KurdishFilter(outbuf.U16Str, total_strlen);
			msAPI_OSD_ReverseAllCharacters(outbuf.U16Str, out.U16Str, total_strlen);
			msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, TempBuf.U16Str, total_strlen);
			bReverse = FALSE;
		}
		else if ( bArabicDisplay ) //for OSD
		{
			S32 errorCode = U_ZERO_ERROR;

			ArabicParser(TempBuf.U16Str, total_strlen, outbuf.U16Str, STROSD,
					 U_SHAPE_LETTERS_SHAPE|U_SHAPE_LENGTH_FIXED_SPACES_AT_END|U_SHAPE_PRESERVE_PRESENTATION,
					 &errorCode);
			//just skip this function for non Kurdish condition
			if ( enLanguage == LANGUAGE_OSD_KURDISH )
			{
				msAPI_OSD_KurdishFilter(outbuf.U16Str, total_strlen);
			}
			msAPI_OSD_ReverseAllCharacters(outbuf.U16Str, out.U16Str, total_strlen);
			msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, outbuf.U16Str, total_strlen);
		}
		else    // for Hebrew
		{
			if ( bDivxSubtitleMode )    // Divx subtitle
			{
				msAPI_OSD_ReverseAllCharacters(TempBuf.U16Str, outbuf.U16Str, total_strlen);
				msAPI_OSD_ReverseNonArabicHebrewCharacters(outbuf.U16Str, out.U16Str, total_strlen);
				msAPI_OSD_ReverseAllCharacters(out.U16Str, outbuf.U16Str, total_strlen);
			}
			else if ( bHebrew )         // Hebrew OSD display
			{
				msAPI_OSD_ReverseAllCharacters(TempBuf.U16Str, out.U16Str, total_strlen);
				msAPI_OSD_ReverseNonArabicHebrewCharacters(out.U16Str, outbuf.U16Str, total_strlen);
			}
			else                        // Other conditions
			{
				msAPI_OSD_ReverseAllCharacters(TempBuf.U16Str, outbuf.U16Str, total_strlen);
				msAPI_OSD_ReverseNonArabicHebrewCharacters(outbuf.U16Str, out.U16Str, total_strlen);
				msAPI_OSD_ReverseAllCharacters(out.U16Str, outbuf.U16Str, total_strlen);
			}
		}

		if ( bReverse )
		{
			for(i=0;i<total_strlen;i++)
			{
				TempBuf.U16Str[i] = *(outbuf.U16Str+total_strlen-i-1);
			}
		}

		//((U16 *)TempBuf)[total_strlen] = 0;
		TempBuf.U16Str[total_strlen] = 0;

		pu16Str = TempBuf.U16Str;
	}
	#endif
#endif

    msAPI_OSD_RESOURCE_GetFontInfo(font_handle, &finfo);

    if ( finfo.height > pclrBtn->height )
    {
        APIOSD_DBG(printf("[]ERR:hei=%bu,%bu,[%c][%c][%c]\n", (U8)finfo.height,(U8)pclrBtn->height,(U8)pu16Str[0],(U8)pu16Str[1],(U8)pu16Str[2]));
        msAPI_OSD_Free_resource();
        return;
    }

    scale = 100;
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
    {
        scale = ((U16)pclrBtn->Fontfmt.height*100 / (U16)finfo.height);
        pclrBtn->Fontfmt.height = scale * finfo.height /100;

        scale = (U8)((U16)pclrBtn->Fontfmt.width*100 / (U16)finfo.width);
        pclrBtn->Fontfmt.width = scale * finfo.width /100;
    }

#if (ENABLE_THAI_OSD)
	if ( ( bThaiDisplay ) /* || ((bDivxSubtitleMode) && (External Subtitle Language is Thai))*/ )
	{
		total_strlen = Thai_compose(pu16Str, 0, total_strlen, STROSD, outbuf.U16Str);
		outbuf.U16Str[total_strlen] = 0x0000;

		pu16Str = outbuf.U16Str;
	}
#endif

#if ENABLE_SHORT_STRING_BUFFER //speed up drawing string
    if (total_strlen <= COUNTOF(_s_temp_glyph))
        pGlyphDispInfo = _s_temp_glyph;
    else
#endif
        pGlyphDispInfo = msAPI_Memory_Allocate(sizeof(OSDSRC_GLYPH_DISP_INFO) * total_strlen , BUF_ID_GLYPH_DISP_INFO);
    if (pGlyphDispInfo == NULL)
    {
        __ASSERT(0);
        APIOSD_DBG(printf("E_DrawText>> pGlyphDispInfo == NULL\n"));
        msAPI_OSD_Free_resource();
        return;
    }

    gap = 0;
    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
    {
    #if (ENABLE_ARABIC_OSD)
        if ( (bArabicDisplay) /*|| ( ( External Subtitle Language is Arabic ) && (bDivxSubtitleMode == TRUE) )*/ )
        {
            pclrBtn->Fontfmt.ifont_gap = 0;
        }
    #endif

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
    }
    else
    {
        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_GAP)
        {
            gap = pclrBtn->Fontfmt.ifont_gap;
        }
        else if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_COMPACT)
        {
            gap = -pclrBtn->Fontfmt.dis;
        }
    }

    if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_BOLD)
    {
        pclrBtn->Fontfmt.flag |= GFXFONT_FLAG_BOLD;
    }

    max_width = pclrBtn->width;

    /*/refine..
    dot_count=0;
    while(dot_count<enEndingType)
        MDrv_GE_GetGlyphDispInfo(font_handle, CHAR_DOT, &pglyphDot[enEndingType-1-dot_count++]);
    */
    if (enEndingType != EN_STRING_ENDING_NONE)
    {
        msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, CHAR_DOT, &pglyphDot[0]);
        for (dot_count = 1; dot_count < enEndingType; dot_count++)
        {
            memcpy(&pglyphDot[dot_count], &pglyphDot[0], sizeof(OSDSRC_GLYPH_DISP_INFO));
        }
    }

    dot_width =0;
    switch (enEndingType)
    {
        case EN_STRING_ENDING_NONE:
            break;
        case EN_STRING_ENDING_3_DOT:
            dot_width += (pglyphDot[2].u8Width + gap);
        case EN_STRING_ENDING_2_DOT:
            dot_width += (pglyphDot[1].u8Width + gap);
        case EN_STRING_ENDING_1_DOT:
            dot_width += (pglyphDot[0].u8Width + gap);

    }
    max_width -= dot_width;

    if(pclrBtn->enTextAlign==EN_ALIGNMENT_LEFT ||pclrBtn->enTextAlign==EN_ALIGNMENT_RIGHT)
       max_width -= BTN_TEXT_GAP;

    {
        width = 0;
        cut_strlen = 0;

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
        {
			static U8 prevState = 0;
		
			prevState = 0;					
			
            while (width<(max_width+dot_width) && cut_strlen<total_strlen)
            {
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, pu16Str[cut_strlen], &pGlyphDispInfo[cut_strlen]);
                pGlyphDispInfo[cut_strlen].u8Width = (pGlyphDispInfo[cut_strlen].u8Width * scale
#if ENABLE_DEFAULT_KERNING
                    - kerningX_(pu16Str[cut_strlen], pu16Str[cut_strlen+1])
#endif //ENABLE_DEFAULT_KERNING
                    ) / 100;
                /*/refine..
                MDrv_GE_GetBBox_X(font_handle, pu16Str[cut_strlen++], &BBox_X_Info);
                if ( scale == 100 )
                {
                    width += (BBox_X_Info.u8Width + gap);
                }
                else
                {
                    width += (BBox_X_Info.u8Width*scale/100 + gap);
                }
                */
				
            #if (ENABLE_ARABIC_OSD)
				//For Subtitle
				if ( (bDivxSubtitleMode == TRUE) && ( (bArabicDisplay) /* || ( External Subtitle Language is Arabic ) */ ) )
				{
					// Check if it is in Arabic Vowel unicode range
					if ( cut_strlen > 0 )
					{
						if ( msAPI_OSD_IsArabicVowel( pu16Str[cut_strlen-1] ) )
						{
							width -= pGlyphDispInfo[cut_strlen-1].u8Width;

							pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = 1;
							pGlyphDispInfo[cut_strlen].overlap_info.s8Yoffset = 0;
						}
						else
						{
							//coordinate no need change
						}
					}
					else
					{
						//coordinate no need change
					}

					width += (pGlyphDispInfo[cut_strlen].u8Width + gap);
				}
				else if ( bArabicDisplay ) //for OSD
				{
					// Check if it is in Arabic Vowel unicode range
					if ( cut_strlen > 0 )
					{
						if ( msAPI_OSD_IsArabicVowel( pu16Str[cut_strlen] ) )
						{
							//skip current vowel character width
							//do nothing here
							S8 nOffset = pGlyphDispInfo[cut_strlen-1].u8Width - pGlyphDispInfo[cut_strlen].u8Width;

							if ( nOffset > 0 )
								pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = nOffset;
							else
								pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = 1;
							pGlyphDispInfo[cut_strlen].overlap_info.s8Yoffset = 0;
						}
						else
						{
							width += (pGlyphDispInfo[cut_strlen].u8Width + gap);

							//coordinate no need change
						}
					}
					else
					{
						width += (pGlyphDispInfo[cut_strlen].u8Width + gap);

						//coordinate no need change
					}
				}
				else
            #endif
            #if (ENABLE_THAI_OSD)
				if ( ( bThaiDisplay ) || ((bDivxSubtitleMode) /* && (External Subtitle Language is Thai) */ ) )
				{
					// Check Primary Thai unicode range
					if ( ( pu16Str[cut_strlen] <= 0x0E5B ) && ( pu16Str[cut_strlen] >= 0x0E01 ) )
					{
						if ( MinusDrawTable[ pu16Str[cut_strlen] - 0x0E01 ] )
						{
							if ( cut_strlen > 0 )
							{
								if ( ( prevState == 1 ) && ( MinusDrawTable[ pu16Str[cut_strlen-1] - 0x0E01 ] ) )
									pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = (pGlyphDispInfo[cut_strlen-2].u8X0 + pGlyphDispInfo[cut_strlen-2].u8Width + 1) - (finfo.width);
								else if ( ( prevState == 2 ) && ( MinusDrawTablePersentation[ pu16Str[cut_strlen-1] - 0xF700 ] ) )
									pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = (pGlyphDispInfo[cut_strlen-2].u8X0 + pGlyphDispInfo[cut_strlen-2].u8Width + 1) - (finfo.width);
								else
									pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = (pGlyphDispInfo[cut_strlen-1].u8X0 + pGlyphDispInfo[cut_strlen-1].u8Width + 1) - (finfo.width);
							}
							else
							{
								pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = 1;
							}

							pGlyphDispInfo[cut_strlen].overlap_info.s8Yoffset = 0;

							//Minus coordinate, don't add width
						}
						else
						{
							width += (pGlyphDispInfo[cut_strlen].u8Width + gap);

							//coordinate no need change
						}

						prevState = 1;
					}
					else if ( ( pu16Str[cut_strlen] <= 0xF71A ) && ( pu16Str[cut_strlen] >= 0xF700 ) )
					{
						if ( MinusDrawTablePersentation[ pu16Str[cut_strlen] - 0xF700 ] )
						{
							if ( cut_strlen > 0 )
							{
								if ( ( prevState == 1 ) && ( MinusDrawTable[ pu16Str[cut_strlen-1] - 0x0E01 ] ) )
									pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = (pGlyphDispInfo[cut_strlen-2].u8X0 + pGlyphDispInfo[cut_strlen-2].u8Width + 1) - (finfo.width);
								else if ( ( prevState == 2 ) && ( MinusDrawTablePersentation[ pu16Str[cut_strlen-1] - 0xF700 ] ) )
									pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = (pGlyphDispInfo[cut_strlen-2].u8X0 + pGlyphDispInfo[cut_strlen-2].u8Width + 1) - (finfo.width);
								else
									pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = (pGlyphDispInfo[cut_strlen-1].u8X0 + pGlyphDispInfo[cut_strlen-1].u8Width + 1) - (finfo.width);
							}
							else
							{
								pGlyphDispInfo[cut_strlen].overlap_info.s8Xoffset = 1;
							}

							pGlyphDispInfo[cut_strlen].overlap_info.s8Yoffset = 0;

							//Minus coordinate, don't add width

							prevState = 2;
						}
						else
						{
							width += (pGlyphDispInfo[cut_strlen].u8Width + gap);

							//coordinate no need change
							prevState = 0;
						}
					}
					else
					{
						width += (pGlyphDispInfo[cut_strlen].u8Width + gap);

						//coordinate no need change
					}
				}
				else
    		#endif
    		    {
                    width += (pGlyphDispInfo[cut_strlen].u8Width + gap);
    		    }
                cut_strlen++;

                if ( (pu16Str[(cut_strlen-1)]==0x20) || ((pu16Str[(cut_strlen-1)]>=0x80) && (pu16Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                {
                    if ( pu16Str[(cut_strlen-1)]==0x8A )
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                        break;
                    }
                    else
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                    }
                }
            }
        }
        else
        {
            while(width<(max_width+dot_width) && cut_strlen<total_strlen)
            {
                msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, pu16Str[cut_strlen], &pGlyphDispInfo[cut_strlen]);

                cut_strlen++;
                width += (finfo.width + gap);
                if ( (pu16Str[(cut_strlen-1)]==0x20) || ((pu16Str[(cut_strlen-1)]>=0x80) && (pu16Str[(cut_strlen-1)]<=0x9F)) )//if Space Char or Control code
                {
                    if ( pu16Str[(cut_strlen-1)]==0x8A )
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                        break;
                    }
                    else
                    {
                        pu16Str[(cut_strlen-1)]=0x20;
                    }
                }
            }
        }

        if (width >= (max_width+dot_width))
        {

            if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_VARWIDTH)
            {
                while(width>=max_width)
                {
                    /*/refine..
                    MDrv_GE_GetBBox_X(font_handle, pu16Str[--cut_strlen], &BBox_X_Info);
                    if ( scale == 100 )
                    {
                        width -= (BBox_X_Info.u8Width + gap);
                    }
                    else
                    {
                        width -=  (BBox_X_Info.u8Width*scale/100 + gap);
                    }
                    */
					
					// Maybe we need to consider combination case here for Arabic and Thai languages
					// If the cut point is at the middle of combination words unicode, 
					// we may see strange words to be displayed or other side effects
					// We can enhance this case in the future
                    cut_strlen--;
                    if( max_width == 0)
                    {
                        break;
                    }
                    else
                        width -= (pGlyphDispInfo[cut_strlen].u8Width + gap);
                }
            }
            else
            {
                while(width>=max_width)
                {
                    cut_strlen--;
                    width -= (finfo.width + gap);
                }
            }
            bExceed = TRUE;
            width_offset = width;
            width += dot_width;
        }

        switch (pclrBtn->enTextAlign)
        {
            case EN_ALIGNMENT_DEFAULT: //without BTN_TEXT_GAP
                text_info.dstblk.x = pclrBtn->x;
                break;

            case EN_ALIGNMENT_LEFT:
                text_info.dstblk.x = pclrBtn->x + BTN_TEXT_GAP;
                break;

            case EN_ALIGNMENT_RIGHT:
                    text_info.dstblk.x = pclrBtn->x + pclrBtn->width - BTN_TEXT_GAP - width;
                break;

            case EN_ALIGNMENT_CENTER:
                    text_info.dstblk.x = pclrBtn->x + (pclrBtn->width - width) / 2;
                break;
        }

        if (pclrBtn->Fontfmt.flag & GEFONT_FLAG_SCALE)
        {
            text_info.dstblk.y = pclrBtn->y + (S16)(pclrBtn->height - finfo.height * scale / 100) / 2;
        }
        else
        {
            text_info.dstblk.y = pclrBtn->y + (S16)(pclrBtn->height - finfo.height) / 2;
        }
//        memcpy(&cpoint.clr, &pclrBtn->t_clr, 4);

        if (cut_strlen > 0)
        {

            //U8 ptrEnd = cut_strlen;

            if (bExceed)
            {
            #if (ENABLE_ARABIC_OSD)
				if ( ( bMirrorLang ) && ( bDivxSubtitleMode == FALSE ) )
				{
					S16 i;
					U16 localwidth = 0;
					S16 index1=0,index2=0;

					for(i=0;i<cut_strlen;i++)
					{
						if ( msAPI_OSD_IsNotInArabicHebrew(pu16Str[i]) == PURE_ARABIC_CHAR )
							break;
					}
					if ( i == cut_strlen )
					{
						// No mirror characters found
						//msAPI_OSD_ReverseAllCharacters(pu16Str, out.U16Str, total_strlen);
						//memcpy(out.U8Str, (U8 *)pu16Str, total_strlen*2);
						cut_strlen = 0;
						for(i=total_strlen-1;i>=0;i--)
						{
							localwidth += pGlyphDispInfo[i].u8Width;
							if ( localwidth >= (max_width+dot_width) )
							{
								// Maybe we need to consider combination case here for Arabic and Thai languages
								// If the cut point is at the middle of combination words unicode,
								// we may see strange words to be displayed or other side effects
								// We can enhance this case in the future
								localwidth -= (pGlyphDispInfo[i].u8Width + gap);
								break;
							}
							out.U16Str[cut_strlen++] = pu16Str[i];
						}

						switch (enEndingType)
						{
							case EN_STRING_ENDING_3_DOT:
								pu16Str[2] = CHAR_DOT; // .
								pu16Str[1] = CHAR_DOT; // .
								pu16Str[0] = CHAR_DOT; // .
								i = 3;
								break;

							case EN_STRING_ENDING_2_DOT:
								pu16Str[1] = CHAR_DOT; // .
								pu16Str[0] = CHAR_DOT; // .
								i = 2;
								break;

							case EN_STRING_ENDING_1_DOT:
								pu16Str[0] = CHAR_DOT; // .
								i = 1;
								break;

							case EN_STRING_ENDING_NONE:
								i = 0;
								break;
						}

						for(index1=(S16)(0),index2=(S16)(cut_strlen-1);index1<index2;index1++,index2--)
						{
							pu16Str[index1+i] = out.U16Str[index2];
							pu16Str[index2+i] = out.U16Str[index1];
						}
						if ( index1 == index2 )
						{
							pu16Str[index1+i] = out.U16Str[index1];
						}
						cut_strlen += i;
						pu16Str[cut_strlen] = 0;

						for(i=0;i<cut_strlen;i++)
						{
							msAPI_OSD_RESOURCE_GetGlyphDispInfo(font_handle, pu16Str[i], &pGlyphDispInfo[i]);
							pGlyphDispInfo[i].u8Width = (pGlyphDispInfo[i].u8Width * scale
			#if ENABLE_DEFAULT_KERNING
								- kerningX_(pu16Str[i], pu16Str[i+1])
			#endif //ENABLE_DEFAULT_KERNING
								) / 100;
						}

						MApi_GFX_TextOut(font_handle, (MS_U8*)pu16dotStr, pclrBtn->bStringIndexWidth+1, &text_info);

						goto Exit_Display;
					}

					// goto normal path
				}
            #endif
                switch (enEndingType)
                {
                    case EN_STRING_ENDING_3_DOT:
                             pu16dotStr[3] = 0; // .
                             pu16dotStr[2] = CHAR_DOT; // .
                             pu16dotStr[1] = CHAR_DOT; //
                             pu16dotStr[0] = CHAR_DOT; // .
                        break;

                    case EN_STRING_ENDING_2_DOT:
                             pu16dotStr[2] = 0; // .
                             pu16dotStr[1] = CHAR_DOT; //
                             pu16dotStr[0] = CHAR_DOT; // .
                        break;

                    case EN_STRING_ENDING_1_DOT:
                             pu16dotStr[1] = 0; // .
                             pu16dotStr[0] = CHAR_DOT; // .
                        break;

                    case EN_STRING_ENDING_NONE:
                        break;
                }
                if (enEndingType != EN_STRING_ENDING_NONE)
                {
                    text_info.dstblk.x += width_offset;

                    #ifndef UI_DRAW_TEXT_I8
//                    text_info.dstblk.x = cpoint.x;
//                    text_info.dstblk.y = cpoint.y;
                    text_info.dstblk.width = pclrBtn->width;
                    text_info.dstblk.height = pclrBtn->height;
                    text_info.flag = pclrBtn->Fontfmt.flag;
                    memcpy(&text_info.color, &pclrBtn->t_clr, 4);
                    text_info.pGlyphDispInfo = (GFX_GlyphDispInfo*)&pglyphDot;
                    text_info.gap = pclrBtn->Fontfmt.ifont_gap;
                    MApi_GFX_TextOut(font_handle, (MS_U8*)pu16dotStr, pclrBtn->bStringIndexWidth+1, &text_info);

//                    MApi_GFX_TextOut(font_handle, (U8 *)pu16dotStr, (U8)pclrBtn->bStringIndexWidth, (GEClrPoint *)&cpoint,  (GEFontFmt *)&pclrBtn->Fontfmt, (GLYPH_DISP_INFO *)pglyphDot);
                    #else
                    //FIXME
//                    MApi_GFX_SetAlpha(false, COEF_ASRC, ABL_FROM_ASRC, 0x00000000);
//                    TextOut_common(font_handle, (U8*)pu16dotStr, pclrBtn->bStringIndexWidth, &cpoint,  &pclrBtn->Fontfmt, pglyphDot, 0);
                    #endif

                    text_info.dstblk.x -=  width_offset;
                }

            }
            u16Char = pu16Str[cut_strlen];
            pu16Str[cut_strlen] = 0;
//            text_info.dstblk.x = cpoint.x;
//            text_info.dstblk.y = cpoint.y;
            text_info.dstblk.width = pclrBtn->width;
            text_info.dstblk.height = pclrBtn->height;
            text_info.flag = pclrBtn->Fontfmt.flag;
            text_info.pGlyphDispInfo = (GFX_GlyphDispInfo*)pGlyphDispInfo;
            text_info.gap = pclrBtn->Fontfmt.ifont_gap;
            memcpy(&text_info.color, &pclrBtn->t_clr, 4);
            MApi_GFX_TextOut(font_handle, (MS_U8*)pu16Str, pclrBtn->bStringIndexWidth+1, &text_info);

            //MApi_GFX_TextOut(font_handle, (U8*)pu16Str, (U8)pclrBtn->bStringIndexWidth, (GEClrPoint *)&cpoint,  (GEFontFmt *)&pclrBtn->Fontfmt, pGlyphDispInfo);
            //msAPI_OSD_DrawText(font_handle, (U8*)pu16Str, pclrBtn);
            pu16Str[cut_strlen] = u16Char;

        }

    }

#if (ENABLE_ARABIC_OSD)
Exit_Display:
#endif

    if (pGlyphDispInfo != NULL)
    {
#if ENABLE_SHORT_STRING_BUFFER     //speed up drawing string
        if (total_strlen <= COUNTOF(_s_temp_glyph))
            ; //do nothing: pGlyphDispInfo = _s_glyph;
        else
#endif
            MSAPI_MEMORY_FREE(pGlyphDispInfo, BUF_ID_GLYPH_DISP_INFO);
    }

        msAPI_OSD_Free_resource();
}

/******************************************************************************/
/// API for draw frame:
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawFrame(OSDClrBtn *pclrBtn)
{
    /* check range */
/*
    if(pclrBtn->bFrameWidth==0)
    {
        return;
    }
    else if((pclrBtn->x + pclrBtn->width) > NEWUI_MAIN_MENU_GWIN_WIDTH)
    {
        return;
    }
    else if((pclrBtn->y + pclrBtn->height) > NEWUI_MAIN_MENU_GWIN_HEIGHT)
    {
        return;
    }
*/
#if GE_DRIVER_TEST
    GELineRect.LineRectCoordinate.direction = 0x00;
    GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_CONSTANT ;
    GELineRect.ARGBClr.ForeGroundColor.u32Num
    = (pclrBtn->f_clr&0xff00ff00) + ((pclrBtn->f_clr&0xff)<<16) +((pclrBtn->f_clr>>16)&0xff);

    // draw upper rectangle fill
    GELineRect.LineRectCoordinate.direction = 0x00;
    GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_CONSTANT ;
    GELineRect.LineRectCoordinate.v0_x = pclrBtn->x;
    GELineRect.LineRectCoordinate.v0_y = pclrBtn->y;
    GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width - 1;
    GELineRect.LineRectCoordinate.v1_y = pclrBtn->y +  pclrBtn->bFrameWidth-1;
    MDrv_GE_RectDraw(&GELineRect);

    // draw lower rectangle fill
    GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_NONCHANGE;
    GELineRect.LineRectCoordinate.v0_y = pclrBtn->y + pclrBtn->height - pclrBtn->bFrameWidth;
    GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height - 1;
    MDrv_GE_RectDraw(&GELineRect);

    // draw left rectangle fill
    GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_NONCHANGE;
    GELineRect.LineRectCoordinate.v0_y = pclrBtn->y;
    GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->bFrameWidth - 1;
    GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height -1 ;
    MDrv_GE_RectDraw(&GELineRect);

    // draw right rectangle fill
    GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_NONCHANGE;
    GELineRect.LineRectCoordinate.v0_x = pclrBtn->x + pclrBtn->width - pclrBtn->bFrameWidth;
    GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width - 1;
    MDrv_GE_RectDraw(&GELineRect);

#else
    GFX_DrawLineInfo pline;
    GFX_RectFillInfo pfillblock;

    msAPI_OSD_GET_resource();

    pfillblock.dstBlock.x = pclrBtn->x;
    pfillblock.dstBlock.y = pclrBtn->y;
    pfillblock.dstBlock.width = pclrBtn->width;
    pfillblock.dstBlock.height = pclrBtn->height;
    memcpy(&pfillblock.colorRange.color_s, &pclrBtn->f_clr, 4);
    memcpy(&pfillblock.colorRange.color_e, &pclrBtn->f_clr, 4);
    pfillblock.flag = 0;

    pline.width = pclrBtn->bFrameWidth;
#if 0
    clrBlock.x = pclrBtn->x;
    clrBlock.y = pclrBtn->y;
    clrBlock.width = pclrBtn->width;
    clrBlock.height = pclrBtn->height;
    memcpy(&clrBlock.clr, &pclrBtn->f_clr, 4);
    lineFmt.width = pclrBtn->bFrameWidth;
#endif
    MApi_GFX_RectFrame(&pfillblock, &pline);

    msAPI_OSD_Free_resource();
#endif
}
/******************************************************************************/
/// API for draw block:
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawBlock(OSDClrBtn *pclrBtn)
{
    /* check range */
/*
    if((pclrBtn->x + pclrBtn->width) > NEWUI_MAIN_MENU_GWIN_WIDTH)
    {
        return;
    }
    else if((pclrBtn->y + pclrBtn->height) > NEWUI_MAIN_MENU_GWIN_HEIGHT)
    {
        return;
    }
*/

    /* set start point coordinate */
    GFX_DrawLineInfo pline;
    GFX_RectFillInfo pfillblock;

    msAPI_OSD_GET_resource();

    /* Initialize the GE format for default setting(RGB565)*/
    pfillblock.fmt = GFX_FMT_RGB565; // 20091215

    pfillblock.dstBlock.x = pclrBtn->x;
    pfillblock.dstBlock.y = pclrBtn->y;


//    GELineRect.LineRectCoordinate.v0_x = pclrBtn->x;
//    GELineRect.LineRectCoordinate.v0_y = pclrBtn->y;

#if GE_DRIVER_TEST
    switch(pclrBtn->u8Gradient)
    {
        default:
        case CONSTANT_COLOR:
            GELineRect.LineRectCoordinate.direction = 0x0;
            GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width -1;
            GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height -1;
            GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_CONSTANT;
            GELineRect.ARGBClr.ForeGroundColor.u32Num
            = (pclrBtn->b_clr&0xff00ff00) + ((pclrBtn->b_clr&0xff)<<16) +((pclrBtn->b_clr>>16)&0xff);
            MDrv_GE_RectDraw(&GELineRect);
            break;

        case GRADIENT_X_COLOR:
            GELineRect.LineRectCoordinate.direction = 0x0;
            GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width-1;
            GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height-1;
            GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_GRADIENT_X;
            GELineRect.ARGBClr.ForeGroundColor.u32Num
            = (pclrBtn->b_clr&0xff00ff00) + ((pclrBtn->b_clr&0xff)<<16) +((pclrBtn->b_clr>>16)&0xff);
            GELineRect.ARGBClr.GradientXColor.u32Num
            = (pclrBtn->g_clr&0xff00ff00) + ((pclrBtn->g_clr&0xff)<<16) +((pclrBtn->g_clr>>16)&0xff);
            MDrv_GE_RectDraw(&GELineRect);
            break;

        case GRADIENT_Y_COLOR:
            GELineRect.LineRectCoordinate.direction = 0x0;
            GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width-1;
            GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height-1;
            GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_GRADIENT_Y;
            GELineRect.ARGBClr.ForeGroundColor.u32Num
            = (pclrBtn->b_clr&0xff00ff00) + ((pclrBtn->b_clr&0xff)<<16) +((pclrBtn->b_clr>>16)&0xff);
            GELineRect.ARGBClr.GradientYColor.u32Num
            = (pclrBtn->g_clr&0xff00ff00) + ((pclrBtn->g_clr&0xff)<<16) +((pclrBtn->g_clr>>16)&0xff);
            MDrv_GE_RectDraw(&GELineRect);
            break;

        case GRADIENT_X_CENTER_COLOR:
            GELineRect.LineRectCoordinate.direction = 0x0;
            GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + (pclrBtn->width>>1) - 1;
            GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height -1 ;
            GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_GRADIENT_X;
            GELineRect.ARGBClr.ForeGroundColor.u32Num
            = (pclrBtn->b_clr&0xff00ff00) + ((pclrBtn->b_clr&0xff)<<16) +((pclrBtn->b_clr>>16)&0xff);
            GELineRect.ARGBClr.GradientXColor.u32Num
            = (pclrBtn->g_clr&0xff00ff00) + ((pclrBtn->g_clr&0xff)<<16) +((pclrBtn->g_clr>>16)&0xff);
            MDrv_GE_RectDraw(&GELineRect);

            GELineRect.LineRectCoordinate.v0_x = pclrBtn->x + pclrBtn->width-1;
            GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_NONCHANGE;
            GELineRect.LineRectCoordinate.direction = 0x01; //x direction change

            MDrv_GE_RectDraw(&GELineRect);
            break;

        case GRADIENT_Y_CENTER_COLOR:
            GELineRect.LineRectCoordinate.direction = 0x0;
            GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width -1;
            GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + (pclrBtn->height>>1) -1;
            GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_GRADIENT_Y;
            GELineRect.ARGBClr.ForeGroundColor.u32Num
            = (pclrBtn->b_clr&0xff00ff00) + ((pclrBtn->b_clr&0xff)<<16) +((pclrBtn->b_clr>>16)&0xff);
            GELineRect.ARGBClr.GradientYColor.u32Num
            = (pclrBtn->g_clr&0xff00ff00) + ((pclrBtn->g_clr&0xff)<<16) +((pclrBtn->g_clr>>16)&0xff);
            MDrv_GE_RectDraw(&GELineRect);

            GELineRect.LineRectCoordinate.direction = 0x02;  //y direction change
            GELineRect.LineRectCoordinate.v0_y = pclrBtn->y + pclrBtn->height - 1;
            GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_NONCHANGE;
            MDrv_GE_RectDraw(&GELineRect);
            break;

    }


    /* draw rectangle frame */
    if(pclrBtn->fHighLight && pclrBtn->bFrameWidth)
    {
        GELineRect.ARGBClr.ForeGroundColor.u32Num
        = (pclrBtn->f_clr&0xff00ff00) + ((pclrBtn->f_clr&0xff)<<16) +((pclrBtn->f_clr>>16)&0xff);

        // draw upper rectangle fill
        GELineRect.LineRectCoordinate.direction = 0x00;
        GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_CONSTANT ;
        GELineRect.LineRectCoordinate.v0_x = pclrBtn->x;
        GELineRect.LineRectCoordinate.v0_y = pclrBtn->y;
        GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width - 1;
        GELineRect.LineRectCoordinate.v1_y = pclrBtn->y +  pclrBtn->bFrameWidth-1;
        MDrv_GE_RectDraw(&GELineRect);

        // draw lower rectangle fill
        GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_NONCHANGE;
        GELineRect.LineRectCoordinate.v0_y = pclrBtn->y + pclrBtn->height - pclrBtn->bFrameWidth;
        GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height - 1;
        MDrv_GE_RectDraw(&GELineRect);

        // draw left rectangle fill
        GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_NONCHANGE;
        GELineRect.LineRectCoordinate.v0_y = pclrBtn->y;
        GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->bFrameWidth - 1;
        GELineRect.LineRectCoordinate.v1_y = pclrBtn->y + pclrBtn->height -1 ;
        MDrv_GE_RectDraw(&GELineRect);

        // draw right rectangle fill
        GELineRect.Gradient_Color_Flag = GECOLOR_FLAG_NONCHANGE;
        GELineRect.LineRectCoordinate.v0_x = pclrBtn->x + pclrBtn->width - pclrBtn->bFrameWidth;
        GELineRect.LineRectCoordinate.v1_x = pclrBtn->x + pclrBtn->width - 1;
        MDrv_GE_RectDraw(&GELineRect);
    }

#else
     switch(pclrBtn->u8Gradient)
    {
        default:
        case CONSTANT_COLOR:

            /* draw rectangle */
            pfillblock.dstBlock.width = pclrBtn->width;
            pfillblock.dstBlock.height = pclrBtn->height;
            /* set gradient color */
            pfillblock.flag = GFXRECT_FLAG_COLOR_CONSTANT ;
            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->b_clr, 4);

            MApi_GFX_RectFill(&pfillblock);
            break;

        case GRADIENT_X_COLOR:
            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
            pfillblock.dstBlock.width = pclrBtn->width;
            pfillblock.dstBlock.height = pclrBtn->height;
            /* set gradient color */
            pfillblock.flag = GFXRECT_FLAG_COLOR_GRADIENT_X ;
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->g_clr, 4);

            /* draw rectangle */
            MApi_GFX_RectFill(&pfillblock);

            break;

        case GRADIENT_Y_COLOR:
            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
            pfillblock.dstBlock.width = pclrBtn->width;
            pfillblock.dstBlock.height = pclrBtn->height;
            /* set gradient color */
            pfillblock.flag = GFXRECT_FLAG_COLOR_GRADIENT_Y ;
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->g_clr, 4);

            /* draw rectangle */
            MApi_GFX_RectFill(&pfillblock);

            break;

        case GRADIENT_XY_COLOR:
            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
            pfillblock.dstBlock.width = pclrBtn->width;
            pfillblock.dstBlock.height = pclrBtn->height;
            /* set gradient color */
            pfillblock.flag = GFXRECT_FLAG_COLOR_GRADIENT_X|GFXRECT_FLAG_COLOR_GRADIENT_Y ;
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->g_clr, 4);

            /* draw rectangle */
            MApi_GFX_RectFill(&pfillblock);


            break;

        case GRADIENT_X_CENTER_COLOR:
            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
            pfillblock.dstBlock.width = pclrBtn->width / 2 ;
            pfillblock.dstBlock.height = pclrBtn->height;
            /* set gradient color */
            pfillblock.flag = GFXRECT_FLAG_COLOR_GRADIENT_X ;
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->g_clr, 4);

            /* draw rectangle */
            MApi_GFX_RectFill(&pfillblock);


            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->g_clr, 4);
            pfillblock.dstBlock.x = pfillblock.dstBlock.x + pfillblock.dstBlock.width ;
            /* set gradient color */
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->b_clr, 4);
            /* draw rectangle */
            MApi_GFX_RectFill(&pfillblock);


            break;

        case GRADIENT_Y_CENTER_COLOR:
            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
            pfillblock.dstBlock.width = pclrBtn->width  ;
            pfillblock.dstBlock.height = pclrBtn->height / 2;
            /* set gradient color */
            pfillblock.flag = GFXRECT_FLAG_COLOR_GRADIENT_Y ;
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->g_clr, 4);

            /* draw rectangle */
            MApi_GFX_RectFill(&pfillblock);


            memcpy(&pfillblock.colorRange.color_s, &pclrBtn->g_clr, 4);
            pfillblock.dstBlock.y = pfillblock.dstBlock.y + pfillblock.dstBlock.height ;
            /* set gradient color */
            memcpy(&pfillblock.colorRange.color_e, &pclrBtn->b_clr, 4);
            /* draw rectangle */
            MApi_GFX_RectFill(&pfillblock);

            break;
    }

    /* draw rectangle frame */
    if(pclrBtn->fHighLight && pclrBtn->bFrameWidth)
    {
        pfillblock.dstBlock.x = pclrBtn->x;
        pfillblock.dstBlock.y = pclrBtn->y;
        pfillblock.dstBlock.width = pclrBtn->width;
        pfillblock.dstBlock.height = pclrBtn->height;
        memcpy(&pfillblock.colorRange.color_s, &pclrBtn->f_clr, 4);
        memcpy(&pfillblock.colorRange.color_e, &pclrBtn->f_clr, 4);
        pfillblock.flag = 0;

        pline.width = pclrBtn->bFrameWidth;
        MApi_GFX_RectFrame(&pfillblock, &pline);
    }



    msAPI_OSD_Free_resource();
//853YE
#endif

}

/******************************************************************************/
/// API for MMI only draw block:
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawMMIBlock(OSDClrBtn *pclrBtn)
{
    U8 u8FrameLineWidth = 0;
    GFX_RectFillInfo pfillblock;

    /* Initialize the GE format for default setting(RGB565)*/
    pfillblock.fmt = GFX_FMT_RGB565; // 20091215

    /* set start point coordinate */
    pfillblock.dstBlock.x = pclrBtn->x;
    pfillblock.dstBlock.y = pclrBtn->y;

//    GELineRect.LineRectCoordinate.v0_x = pclrBtn->x;
//    GELineRect.LineRectCoordinate.v0_y = pclrBtn->y;

    msAPI_OSD_GET_resource();

    /* draw rectangle frame */
    if(pclrBtn->fHighLight && pclrBtn->bFrameWidth)
    {
        u8FrameLineWidth = pclrBtn->bFrameWidth;


        pfillblock.dstBlock.width = pclrBtn->width;
        pfillblock.dstBlock.height = pclrBtn->height;
        /* set gradient color */
        pfillblock.flag = GFXRECT_FLAG_COLOR_CONSTANT ;
        memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
        memcpy(&pfillblock.colorRange.color_e, &pclrBtn->b_clr, 4);

        MApi_GFX_RectFill(&pfillblock);

    }



    pfillblock.dstBlock.x = pclrBtn->x + u8FrameLineWidth;
    pfillblock.dstBlock.y = pclrBtn->y + u8FrameLineWidth;
    pfillblock.dstBlock.width = pclrBtn->width-(U16)(u8FrameLineWidth<<1);
    pfillblock.dstBlock.height = pclrBtn->height-(U16)(u8FrameLineWidth<<1);
    /* set gradient color */
    pfillblock.flag = GFXRECT_FLAG_COLOR_CONSTANT ;
    memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
    memcpy(&pfillblock.colorRange.color_e, &pclrBtn->b_clr, 4);

    MApi_GFX_RectFill(&pfillblock);




    msAPI_OSD_Free_resource();

}

#if 1   //(KEEP_UNUSED_FUNC == 1)
/******************************************************************************/
/// API for draw block:
/// @param font_handle \b IN font handle
/// @param *pu8Str \b IN pointer to string
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawButton(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn)
{
    /* check range */
/*
    if((pclrBtn->x + pclrBtn->width) > NEWUI_MAIN_MENU_GWIN_WIDTH)
    {
        return;
    }
    else if((pclrBtn->y + pclrBtn->height) > NEWUI_MAIN_MENU_GWIN_HEIGHT)
    {
        return;
    }
*/

    /*draw background rectangle*/

    msAPI_OSD_GET_resource();

    msAPI_OSD_DrawBlock(pclrBtn);

    /* draw text*/
    if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
    {
        if(*pu8Str == 0)
        {
            msAPI_OSD_Free_resource();
            return;
        }
    }
    else
    {
        //if(*((U16*)pu8Str) == 0)
        if(((pu8Str[1]<<8) + pu8Str[0]) == 0)
        {
            msAPI_OSD_Free_resource();
            return;
        }
    }

    msAPI_OSD_DrawText(font_handle, pu8Str, pclrBtn);

    msAPI_OSD_Free_resource();
}
#endif

/*****************************************************************************************************/

//static U16 angular_increment;
//static U16 normalised_angle;
static U16 x, y, last_y;
static U16 u16Temp1, u16Temp2, u16Temp3, u16Temp4, u16Temp5;
//static U32 u32Loop;


/******************************************************************************/
/// API for draw round frame:
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawRoundFrame(OSDClrBtn *pclrBtn)
{
    GFX_RectFillInfo pfillblock;

    if ( pclrBtn->radius > pclrBtn->width / 2 || pclrBtn->radius > pclrBtn->height / 2 )
    {
        return;
    }

    msAPI_OSD_GET_resource();

    /* Initialize the GE format for default setting(RGB565)*/
    pfillblock.fmt = GFX_FMT_RGB565; // 20091215

    if ( ( pfillblock.dstBlock.width = pclrBtn->width - 2 * pclrBtn->radius ) > 0 )
    {
        /* draw top frame */
        pfillblock.dstBlock.x = pclrBtn->x + pclrBtn->radius;
        pfillblock.dstBlock.y = pclrBtn->y ;
        pfillblock.dstBlock.height = pclrBtn->bFrameWidth;
        /* set gradient color */
        pfillblock.flag = GFXRECT_FLAG_COLOR_CONSTANT ;
        memcpy(&pfillblock.colorRange.color_s, &pclrBtn->f_clr, 4);
        memcpy(&pfillblock.colorRange.color_e, &pclrBtn->f_clr, 4);

        MApi_GFX_RectFill(&pfillblock);


        /* draw bottom frame */
        pfillblock.dstBlock.y = pclrBtn->y + pclrBtn->height - pclrBtn->bFrameWidth - 1;
        MApi_GFX_RectFill(&pfillblock);

    }

    if ( ( pfillblock.dstBlock.height = pclrBtn->height - 2 * pclrBtn->radius ) > 0 )
    {
        /* draw left frame */
        pfillblock.dstBlock.x = pclrBtn->x;
        pfillblock.dstBlock.y = pclrBtn->y + pclrBtn->radius - 1;
        pfillblock.dstBlock.width = pclrBtn->bFrameWidth;
        pfillblock.dstBlock.height += 2;
        MApi_GFX_RectFill(&pfillblock);

        /* draw right frame */
        pfillblock.dstBlock.x = pclrBtn->x + pclrBtn->width - pclrBtn->bFrameWidth;
        MApi_GFX_RectFill(&pfillblock);

    }

    u16Temp1 = pclrBtn->x + pclrBtn->radius - 1;
    u16Temp2 = pclrBtn->y + pclrBtn->radius - 1;
    u16Temp3 = pclrBtn->x + pclrBtn->width - pclrBtn->radius - pclrBtn->bFrameWidth + 1;
    u16Temp4 = pclrBtn->y + pclrBtn->height - pclrBtn->radius - pclrBtn->bFrameWidth;
    last_y = 0;


    u16Temp5 = (pclrBtn->radius*pclrBtn->radius)<<2;
    for (y = 1; ;y++)
    {
        u16Temp5 = u16Temp5 - (y<<3) - 4;

        x = int_sqrt(u16Temp5);
        x = (x - 1)>>1;
        pfillblock.dstBlock.x = u16Temp1 - x;
        pfillblock.dstBlock.y = u16Temp2 - y;
        pfillblock.dstBlock.width = pclrBtn->bFrameWidth;
        pfillblock.dstBlock.height = pclrBtn->bFrameWidth;
        MApi_GFX_RectFill(&pfillblock);

        /* draw left-bottom quarter */
        pfillblock.dstBlock.y = u16Temp4 + y;
        MApi_GFX_RectFill(&pfillblock);

        /* draw right-bottom quarter */
        pfillblock.dstBlock.x = u16Temp3 + x;
        MApi_GFX_RectFill(&pfillblock);

        /* draw right-top quarter */
        pfillblock.dstBlock.y = u16Temp2 - y;
        MApi_GFX_RectFill(&pfillblock);

        /* symmetrical */
        pfillblock.dstBlock.x = u16Temp1 - y;
        pfillblock.dstBlock.y = u16Temp2 - x;
        MApi_GFX_RectFill(&pfillblock);

        /* draw left-bottom quarter */
        pfillblock.dstBlock.y = u16Temp4 + x;
        MApi_GFX_RectFill(&pfillblock);

        /* draw right-bottom quarter */
        pfillblock.dstBlock.x = u16Temp3 + y;
        MApi_GFX_RectFill(&pfillblock);

        /* draw right-top quarter */
        pfillblock.dstBlock.y = u16Temp2 - x;
        MApi_GFX_RectFill(&pfillblock);

        if (x <= y)
            break;
    }

    msAPI_OSD_Free_resource();

}
/******************************************************************************/
/// API for draw round block:
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawRoundBlock(OSDClrBtn *pclrBtn)
{
    GFX_RectFillInfo pfillblock;

    if ( pclrBtn->radius > pclrBtn->width / 2 || pclrBtn->radius > pclrBtn->height / 2 )
    {
        return;
    }

    msAPI_OSD_GET_resource();

    /* Initialize the GE format for default setting(RGB565)*/
    pfillblock.fmt = GFX_FMT_RGB565; // 20091215

    pfillblock.dstBlock.x = pclrBtn->x;
    pfillblock.dstBlock.y = pclrBtn->y + pclrBtn->radius - 1;
    pfillblock.dstBlock.width = pclrBtn->width;
    pfillblock.dstBlock.height = pclrBtn->height - (2*pclrBtn->radius) + 1;
    memcpy(&pfillblock.colorRange.color_s, &pclrBtn->b_clr, 4);
    memcpy(&pfillblock.colorRange.color_e, &pclrBtn->g_clr, 4);
    pfillblock.flag = 0;
    MApi_GFX_RectFill(&pfillblock);


//    GELineRect.Gradient_Color_Flag        = GECOLOR_FLAG_NONCHANGE;
    /* draw top and bottom round */
    u16Temp1 = pclrBtn->x + pclrBtn->radius - 1;
    u16Temp2 = pfillblock.dstBlock.y;
    u16Temp3 = pclrBtn->width - 2 * pclrBtn->radius + 2;
    u16Temp4 = pclrBtn->y + pclrBtn->height - pclrBtn->radius;
    last_y = 0;

    u16Temp5 = (pclrBtn->radius*pclrBtn->radius)<<2;
    for ( y=1; ; y++ )
    {
        u16Temp5 = u16Temp5 - (y<<3) - 4;
        x = int_sqrt(u16Temp5);
        x = (x - 1)>>1;

        /* draw top round */
        pfillblock.dstBlock.x = u16Temp1 - x;
        pfillblock.dstBlock.y = u16Temp2 - y;
        pfillblock.dstBlock.width = u16Temp3 + (x<<1) ; // +1 for compensation of original formula
        pfillblock.dstBlock.height = (y - last_y);
        MApi_GFX_RectFill(&pfillblock);


        /* draw bottom round */
        //GELineRect.LineRectCoordinate.direction = 0x0;
        if(u16Temp4 + y - 1  <   u16Temp4 + last_y)
        {
            pfillblock.dstBlock.y = u16Temp4 + y - 1;
            pfillblock.dstBlock.height = u16Temp4 + last_y - pfillblock.dstBlock.y + 1;
        }
        else
        {
            pfillblock.dstBlock.y = u16Temp4 + last_y;
            pfillblock.dstBlock.height = u16Temp4 + y - pfillblock.dstBlock.y;
        }
        MApi_GFX_RectFill(&pfillblock);



        /* draw top round */
        //GELineRect.LineRectCoordinate.direction = 0x0;
        pfillblock.dstBlock.x = u16Temp1 - y;
        pfillblock.dstBlock.y = u16Temp2 - x;
        pfillblock.dstBlock.width =  u16Temp3 + (y<<1);
        pfillblock.dstBlock.height = u16Temp2 - y +1 - pfillblock.dstBlock.y;
        MApi_GFX_RectFill(&pfillblock);

        /* draw bottom round */
        //GELineRect.LineRectCoordinate.direction = 0x0;
        if(u16Temp4 + last_y  <   u16Temp4 + x - 1)
        {
            pfillblock.dstBlock.y = u16Temp4 + last_y;
            pfillblock.dstBlock.height = u16Temp4 + x - pfillblock.dstBlock.y;
        }
        else
        {
            pfillblock.dstBlock.y = u16Temp4 + x - 1;
            pfillblock.dstBlock.height = u16Temp4 + last_y  - pfillblock.dstBlock.y + 1;
        }

        MApi_GFX_RectFill(&pfillblock);



        last_y = y;

        if (x <= y)
            break;
    }

    if(pclrBtn->fHighLight && pclrBtn->bFrameWidth)
    {
        msAPI_OSD_DrawRoundFrame( pclrBtn );
    }
    else
    {

        /* anti-alias */
        last_y = 0;

//      PESet.PE_Enable_Flag = GE_PE_FLAG_ALPHABLENDING;
//      PESet.ABL.scoef = PESet.ABL.dcoef = 3;
//      PESet.ABL.abl_const = 0x66666666;
        //MDrv_GE_PixelEngine(&PESet);
        //MDrv_GE_SetAlpha(1,3,3,0x55555555);

        MApi_GFX_SetAlphaBlending(COEF_CONST, 0x66);
        MApi_GFX_SetAlphaSrcFrom(ABL_FROM_CONST);
        MApi_GFX_EnableAlphaBlending(TRUE);


        u16Temp5 = (pclrBtn->radius*pclrBtn->radius + (pclrBtn->radius>>1))<<2;

        for ( y=1; ; y++ )
        {
            u16Temp5 = u16Temp5 - (y<<3) - 4;
            x = int_sqrt(u16Temp5);
            x = (x - 1)>>1;

            /* draw top round */
            pfillblock.dstBlock.x = u16Temp1 - x;
            pfillblock.dstBlock.y = u16Temp2 - y;
            pfillblock.dstBlock.width = u16Temp3 + (x<<1) ;
            pfillblock.dstBlock.height = (y - last_y) + 1 ;
            MApi_GFX_RectFill(&pfillblock);


            /* draw bottom round */
            //GELineRect.LineRectCoordinate.direction = 0x0;
            if(u16Temp4 + last_y < u16Temp4 + y - 1)
            {
                pfillblock.dstBlock.y = u16Temp4 + last_y;
                pfillblock.dstBlock.height = u16Temp4 + y - pfillblock.dstBlock.y;
            }
            else
            {
                pfillblock.dstBlock.y = u16Temp4 + y - 1;
                pfillblock.dstBlock.height = u16Temp4 + last_y - pfillblock.dstBlock.y + 1;
            }
            MApi_GFX_RectFill(&pfillblock);


            /* draw top round */
            //GELineRect.LineRectCoordinate.direction = 0x0;
            pfillblock.dstBlock.x = u16Temp1 - y;
            pfillblock.dstBlock.y = u16Temp2 - x;
            pfillblock.dstBlock.width = u16Temp3 + (y<<1);
            pfillblock.dstBlock.height = u16Temp2 - y - pfillblock.dstBlock.y + 1;
            MApi_GFX_RectFill(&pfillblock);




            /* draw bottom round */
            //GELineRect.LineRectCoordinate.direction = 0x0;
            if( u16Temp4 + last_y < u16Temp4 + x - 1 )
            {
                pfillblock.dstBlock.y = u16Temp4 + last_y;
                pfillblock.dstBlock.height = u16Temp4 + x - pfillblock.dstBlock.y;
            }
            else
            {
                pfillblock.dstBlock.y = u16Temp4 + x - 1 ;
                pfillblock.dstBlock.height = u16Temp4 + last_y - pfillblock.dstBlock.y + 1;
            }

            MApi_GFX_RectFill(&pfillblock);


            last_y = y;

            if (x <= y)
                break;
        }

//        PESet.PE_Enable_Flag = GE_PE_FLAG_DEFAULT;
        //MDrv_GE_PixelEngine(&PESet);
        MApi_GFX_EnableAlphaBlending(FALSE);
        MApi_GFX_SetAlphaBlending(COEF_ASRC,0x66);

        /* end AA */
    }

        msAPI_OSD_Free_resource();

}
/******************************************************************************/
/// API for draw round button:
/// @param font_handle \b IN font handle
/// @param *pu8Str \b IN pointer to string
/// @param *pclrBtn \b IN: pointer to OSD component structure
/// -@see OSDClrBtn
/******************************************************************************/
void msAPI_OSD_DrawRoundButton(FONTHANDLE font_handle, U8 *pu8Str, OSDClrBtn *pclrBtn)
{
    /*
        GERGBColor b_clr,f_clr;
        GEPoint point;
        GEClrLine clrline;
        GELineFmt lineFmt;
        LONG32_BYTE u32clr;
        U16 radius;
    */

    /* check range */
    /*if ( ( pclrBtn->x + pclrBtn->width ) > NEWUI_MAIN_MENU_GWIN_WIDTH )
    {
        return;
    }
    else if ( ( pclrBtn->y + pclrBtn->height ) > NEWUI_MAIN_MENU_GWIN_HEIGHT )
    {
        return;
    }
    else*/ if ( pclrBtn->radius > pclrBtn->width / 2 || pclrBtn->radius > pclrBtn->height / 2 )
    {
        return;
    }

    msAPI_OSD_GET_resource();

    /* draw round block */
    msAPI_OSD_DrawRoundBlock(pclrBtn);

    /* draw text */
    if(pclrBtn->bStringIndexWidth == CHAR_IDX_1BYTE)
    {
        if(*pu8Str == 0)
               {
                     msAPI_OSD_Free_resource();
            return;
                }
    }
    else
    {
        //if(*((U16*)pu8Str) == 0)
        if(((pu8Str[1]<<8) + pu8Str[0]) == 0)
        {
            msAPI_OSD_Free_resource();
            return;
        }
    }

    pclrBtn->x += pclrBtn->radius;
    pclrBtn->width -= 2 * pclrBtn->radius;
       msAPI_OSD_DrawText(font_handle, pu8Str, pclrBtn);
    pclrBtn->x -= pclrBtn->radius;
    pclrBtn->width += 2 * pclrBtn->radius;

       msAPI_OSD_Free_resource();

}

#if 0
/*****************************************************************************************************/
void msAPI_OSD_DrawBitmap(BMPHANDLE Handle, U16 u16StartX, U16 u16StartY, GEBitmapFmt bmpfmt)
{
    GEPoint v0;
    v0.x = u16StartX;
    v0.y = u16StartY;

    if(bmpfmt.bBmpColorKeyEnable)
        MDrv_GE_SetSCK(ENABLE, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);

    //bmpfmt.bScale = FALSE;
    MDrv_GE_DrawBitmap(Handle, &v0, &bmpfmt);

    MDrv_GE_SetSCK(DISABLE, NULL, NULL);
}
#endif


/******************************************************************************/
/// API for draw block::
/// @param Handle \b IN bitmap handle
/// @param u16StartX \b IN x coordinate
/// @param u16StartY \b IN y coordinate
/// @param bmpfmt \b IN bitmap format
/******************************************************************************/
void msAPI_OSD_DrawBitmap(BMPHANDLE Handle, U16 u16StartX, U16 u16StartY, GEBitmapFmt bmpfmt)
{
    msAPI_OCP_DecompressHandle(Handle);

#if GE_DRIVER_TEST
    //if(bmpfmt.bBmpColorKeyEnable)
    //    MDrv_GE_SetSCK(ENABLE, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);

    PESet.PE_Enable_Flag = GE_PE_FLAG_SCK  ;
    //PESet.PE_Enable_Flag  = GE_PE_FLAG_ALPHABLENDING ;
    PESet.sck_set.sck_hth.u32Num = 0xff00ff;
    PESet.sck_set.sck_lth.u32Num = 0xff00ff;
    PESet.sck_set.sck_mode = 0x11;

    //PESet.ABL.abl_const = 0x808080;
    //PESet.ABL.dcoef = PESet.ABL.scoef = 0x3;

    MDrv_GE_PixelEngine(&PESet);

    //MDrv_GE_SetAlpha(ENABLE,0x3, 0x3,0x80808080);

    BitbltInfo.bmphandle = Handle;
    BitbltInfo.BitbltCoordinate.v0_x = u16StartX;
    BitbltInfo.BitbltCoordinate.v0_y = u16StartY;
    BitbltInfo.BitbltCoordinate.width = bmpfmt.width;
    BitbltInfo.BitbltCoordinate.height = bmpfmt.height;
    BitbltInfo.BitbltCoordinate.v2_x = 0;
    BitbltInfo.BitbltCoordinate.v2_y = 0;
    BitbltInfo.dst_fm = 0x17;
    BitbltInfo.src_fm = 0x17;
    if(bmpfmt.bScale)
        BitbltInfo.BmpFlag = GEBBT_FLAG_STBB;
    else
        BitbltInfo.BmpFlag = GFXDRAW_FLAG_DEFAULT;

    MDrv_GE_DrawBitmap2(&BitbltInfo);
    //MDrv_GE_SetSCK(DISABLE, NULL, NULL);

    PESet.PE_Enable_Flag = GE_PE_FLAG_DEFAULT ;
    MDrv_GE_PixelEngine(&PESet);

#else

//    GEPoint v0;
    GFX_DrawBmpInfo gfx_bmpfmt;

    msAPI_OSD_GET_resource();

    gfx_bmpfmt.x = u16StartX;
    gfx_bmpfmt.y = u16StartY;



    if (bmpfmt.bBmpColorKeyEnable)
    {
        MApi_GFX_SetSrcColorKey(TRUE, CK_OP_EQUAL, GFX_FMT_ARGB8888, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);
    }



    bmpfmt.clrrange.color_s.r = 0xeF;
    bmpfmt.clrrange.color_s.g = 0x0;
    bmpfmt.clrrange.color_s.b = 0xeF;
    bmpfmt.clrrange.color_e.r = 0xFF;
    bmpfmt.clrrange.color_e.g = 0x0;
    bmpfmt.clrrange.color_e.b = 0xFF;
    //MDrv_GE_SetSCK(ENABLE, CK_OP_EQUAL, GFX_FMT_ARGB8888,  &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);
    MApi_GFX_SetSrcColorKey(TRUE, CK_OP_EQUAL, GFX_FMT_ARGB8888, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);

    MApi_GFX_SetNearestMode(false);
    MApi_GFX_SetPatchMode(true);

    gfx_bmpfmt.height = bmpfmt.height;
    gfx_bmpfmt.width = bmpfmt.width;
//    gfx_bmpfmt.x = v0.x;
//    gfx_bmpfmt.y = v0.y;
    gfx_bmpfmt.bScale= bmpfmt.bScale;

    //bmpfmt.bScale = FALSE;
    MApi_GFX_DrawBitmap(Handle, &gfx_bmpfmt);
    //MDrv_GE_DrawBitmap(Handle, &v0, &bmpfmt);
    MApi_GFX_SetNearestMode(false);
    MApi_GFX_SetPatchMode(false);
//    MDrv_GE_SetSCK(DISABLE, CK_OP_EQUAL, GFX_FMT_ARGB8888,  NULL, NULL);
    MApi_GFX_SetSrcColorKey(FALSE, CK_OP_EQUAL, GFX_FMT_ARGB8888, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);

    msAPI_OSD_Free_resource();

#endif
#if 0
    MsOS_DelayTask(500);

    BitbltInfo.BmpFlag = GFXDRAW_FLAG_DEFAULT;

    MDrv_GE_DrawBitmap2(&BitbltInfo);

    MsOS_DelayTask(500);

    if(bmpfmt.bScale)
        BitbltInfo.BmpFlag = GEBBT_FLAG_STBB;
    else
        BitbltInfo.BmpFlag = GFXDRAW_FLAG_DEFAULT;

    MDrv_GE_DrawBitmap2(&BitbltInfo);
#endif
}



/******************************************************************************/
/// API for draw bitmap specifically for subtitle:
/// @param Handle \b IN bitmap handle
/// @param u16StartX \b IN x coordinate
/// @param u16StartY \b IN y coordinate
/// @param bmpfmt \b IN bitmap format
/******************************************************************************/
void msAPI_OSD_DrawBitmap_Subtitle(BMPHANDLE Handle, U16 u16StartX, U16 u16StartY, GEBitmapFmt bmpfmt)
{

//    GEPoint v0;
    GFX_DrawBmpInfo gfx_bmpfmt;

    gfx_bmpfmt.x = u16StartX;
    gfx_bmpfmt.y = u16StartY;

    msAPI_OSD_GET_resource();

    if (bmpfmt.bBmpColorKeyEnable)
    {
        //MDrv_GE_SetSCK(ENABLE, CK_OP_EQUAL, GE_FMT_I8, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);
        MApi_GFX_SetSrcColorKey(TRUE, CK_OP_EQUAL, GFX_FMT_I8, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);

    }

    gfx_bmpfmt.height = bmpfmt.height;
    gfx_bmpfmt.width = bmpfmt.width;
//    gfx_bmpfmt.x = v0.x;
//    gfx_bmpfmt.y = v0.y;
    gfx_bmpfmt.bScale= bmpfmt.bScale;

    MApi_GFX_SetNearestMode(true);
    MApi_GFX_SetPatchMode(true);

    //bmpfmt.bScale = FALSE;
    MApi_GFX_DrawBitmap(Handle, &gfx_bmpfmt);
    MApi_GFX_SetNearestMode(false);
    MApi_GFX_SetPatchMode(false);
//    MDrv_GE_SetSCK(DISABLE, CK_OP_EQUAL, GE_FMT_I8,  NULL, NULL);
    MApi_GFX_SetSrcColorKey(FALSE, CK_OP_EQUAL, GFX_FMT_I8, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);

    msAPI_OSD_Free_resource();

}

/******************************************************************************/
/// API for drawing colorkey bitmap::
/// @param Handle \b IN handle to the bitmap
/// @param u16StartX \b IN start position X
/// @param u16StartY \b IN start position Y
/// @param bmpfmt \b IN bitmap format
/******************************************************************************/
void msAPI_OSD_DrawColorKeyBitmap(BMPHANDLE Handle, U16 u16StartX, U16 u16StartY, MSAPI_GEBitmapFmt bmpfmt)
{
  //  GEPoint v0;
    GFX_DrawBmpInfo gfx_bmpfmt;
    msAPI_OCP_DecompressHandle(Handle);

    msAPI_OSD_GET_resource();

    gfx_bmpfmt.x = u16StartX;
    gfx_bmpfmt.y = u16StartY;

    gfx_bmpfmt.height = bmpfmt.height;
    gfx_bmpfmt.width = bmpfmt.width;
    //gfx_bmpfmt.x = v0.x;
    //gfx_bmpfmt.y = v0.y;
    gfx_bmpfmt.bScale= bmpfmt.bScale;

    if (bmpfmt.bBmpColorKeyEnable)
    {
        //MDrv_GE_SetSCK(ENABLE, CK_OP_EQUAL, GFX_FMT_ARGB8888,  (GERGBColor *)&bmpfmt.clrrange.color_s, (GERGBColor *)&bmpfmt.clrrange.color_e);
        MApi_GFX_SetSrcColorKey(TRUE, CK_OP_EQUAL, GFX_FMT_ARGB8888, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);
    }

    MApi_GFX_SetNearestMode(false);
    MApi_GFX_SetPatchMode(true);

    MApi_GFX_DrawBitmap(Handle, &gfx_bmpfmt);

    MApi_GFX_SetNearestMode(false);
    MApi_GFX_SetPatchMode(false);

    //MDrv_GE_SetSCK(DISABLE, CK_OP_EQUAL, GFX_FMT_ARGB8888,  NULL, NULL);
    MApi_GFX_SetSrcColorKey(FALSE, CK_OP_EQUAL, GFX_FMT_ARGB8888, &bmpfmt.clrrange.color_s, &bmpfmt.clrrange.color_e);
    msAPI_OSD_Free_resource();
}

/******************************************************************************/
/// API for set clip window::
/// @param x0 \b IN start x coordinate
/// @param y0 \b IN start y coordinate
/// @param x1 \b IN end x coordinate + 1 (not included in window)
/// @param y1 \b IN end y coordinate + 1 (not included in window)
/******************************************************************************/
void msAPI_OSD_SetClipWindow(U16 u16X0, U16 u16Y0, U16 u16X1, U16 u16Y1)
{
    GFX_Point v0, v1;
    msAPI_OSD_GET_resource();

    ClipWindow.x0 = u16X0;
    ClipWindow.y0 = u16Y0;
    ClipWindow.x1 = u16X1;
    ClipWindow.y1 = u16Y1;
    v0.x = u16X0;
    v0.y = u16Y0;
    v1.x = u16X1;
    v1.y = u16Y1;
    MApi_GFX_SetClip(&v0, &v1);
    msAPI_OSD_Free_resource();

}

/******************************************************************************/
/// API for restore the backuped clipwindow::
/******************************************************************************/
void msAPI_OSD_RestoreBackupClipWindow()
{
    GFX_Point v0, v1;

    msAPI_OSD_GET_resource();

    ClipWindow.x0 = ClipWindowBackup.x0;
    ClipWindow.y0 = ClipWindowBackup.y0;
    ClipWindow.x1 = ClipWindowBackup.x1;
    ClipWindow.y1 = ClipWindowBackup.y1;
//    MDrv_GE_SetClipWindow(ClipWindow.x0, ClipWindow.y0, ClipWindow.x1 - 1, ClipWindow.y1 - 1);
    v0.x = ClipWindowBackup.x0;
    v0.y = ClipWindowBackup.y0;
    v1.x = ClipWindowBackup.x1;
    v1.y = ClipWindowBackup.y1;
    MApi_GFX_SetClip(&v0, &v1);

    msAPI_OSD_Free_resource();

}

void msAPI_OSD_GetClipWindow(U16 *pX0, U16 *pY0, U16 *pX1, U16 *pY1)
{

    msAPI_OSD_GET_resource();

    *pX0 = ClipWindow.x0;
    *pY0 = ClipWindow.y0;
    *pX1 = ClipWindow.x1;
    *pY1 = ClipWindow.y1;
    //MDrv_GE_GetClipWindow(pX0, pY0, pX1, pY1);

    msAPI_OSD_Free_resource();

}

void msAPI_OSD_BackupClipWindow(void)
{
    ClipWindowBackup.x0 = ClipWindow.x0;
    ClipWindowBackup.y0 = ClipWindow.y0;
    ClipWindowBackup.x1 = ClipWindow.x1;
    ClipWindowBackup.y1 = ClipWindow.y1;
}

/******************************************************************************/
/// API for set Dither::
/// @param bEnable \b IN turn on/off
/******************************************************************************/
void msAPI_OSD_SetDither(BOOLEAN bEnable)
{
    msAPI_OSD_GET_resource();

    MApi_GFX_SetDither(bEnable);

    msAPI_OSD_Free_resource();
}
#if (KEEP_UNUSED_FUNC == 1)
/******************************************************************************/
/// API for set rotation::
/// @param locrotate \b IN local rotation
/// @param glorotate \b IN global rotation
/******************************************************************************/
void msAPI_OSD_SetRotation(U8 locrotate,U8 glorotate)
{
       msAPI_OSD_GET_resource();

       MApi_GFX_SetRotate(locrotate);

       msAPI_OSD_Free_resource();
}
#endif

//*
//*****************************************************************
//void msAPI_OSD_FontGWinInit()
//
//*****************************************************************
#if CHIP_ID == MST9U4
BOOLEAN msAPI_OSD_FontGWinInit(MSAPI_FontGWin* FontGwin,U16 FbFmt,MSAPI_GWINLAYER Layer)
{
	BOOLEAN nRet = FALSE;
	U8 backFB = MApi_GOP_FB_Get();
	U8 u8Gop,u8GopRight;

	if(isFontGwinInit[Layer])
		return FALSE;
    MApi_GOP_GWIN_SetForceWrite(TRUE);	

	if( Layer == GWIN_LAYER_BTM)
	{
		#if !ZUI
		u8Gop = E_GOP_OSD;
		u8GopRight = E_GOP_OSD_RIGHT;
		#else
		return FALSE;
		#endif
		MApi_GOP_FB_SEL(0); // switch to GOP_WINFB_POOL1
	}
	else if(Layer == GWIN_LAYER_TOP)	
	{
		u8Gop = E_GOP_FOSD;
		u8GopRight = E_GOP_FOSD_RIGHT;
		MApi_GOP_FB_SEL(1); // switch to GOP_WINFB_POOL2
	}
	else
		return FALSE;// not avaliable

	MApi_GOP_GWIN_SwitchGOP(u8Gop);

	//Create GWin
	#if !MST9U_ASIC_1P
	nRet = MApi_GOP_GWIN_Create2Win(FontGwin->Rect.width, FontGwin->Rect.height, FbFmt, &FontGwin->u8GWinID, &FontGwin->u8GWin2ID, u8GopRight);	
	gstFontGwin[Layer].u8GWin2ID= FontGwin->u8GWin2ID;
	#else
	FontGwin->u8GWinID = MApi_GOP_GWIN_CreateWin(FontGwin->Rect.width,FontGwin->Rect.height,FbFmt);
	#endif
    FontGwin->u8MainFbID = MApi_GOP_GWIN_GetFBfromGWIN(FontGwin->u8GWinID);

	gstFontGwin[Layer].u8MainFbID = FontGwin->u8MainFbID;
	gstFontGwin[Layer].u8GWinID = FontGwin->u8GWinID;
	MApi_GOP_GWIN_Switch2Gwin(FontGwin->u8GWinID);
	// draw clear screen by OSDW
	MApi_GOP_GWIN_EnableTransClr(GOPTRANSCLR_FMT0, FALSE);	
	MApi_GOP_GWIN_SetBlending(FontGwin->u8GWinID, TRUE, 0xB1); // per pixel alpha, decided by OSDW
    FontGwin->u8SubFbID= MApi_GOP_GWIN_GetFreeFBID();
	gstFontGwin[Layer].u8SubFbID = FontGwin->u8SubFbID;
	nRet = MApi_GOP_GWIN_CreateFB(FontGwin->u8SubFbID, 0, 0, FontGwin->Rect.width, FontGwin->Rect.height, FbFmt);
	#if !MST9U_ASIC_1P
	MApi_GOP_GWIN_SwitchGOP(u8GopRight);
	MApi_GOP_GWIN_Switch2Gwin(FontGwin->u8GWin2ID);
	MApi_GOP_GWIN_EnableTransClr(GOPTRANSCLR_FMT0, FALSE);	
	MApi_GOP_GWIN_SetBlending(FontGwin->u8GWin2ID, TRUE, 0xB1); // per pixel alpha, decided by OSDW
	MApi_GOP_GWIN_MapFBTo2Win(FontGwin->u8SubFbID,FontGwin->u8GWinID,FontGwin->u8GWin2ID);
    MApi_GOP_GWIN_Set2WinPosition(FontGwin->u8GWinID, FontGwin->u8GWin2ID, FontGwin->Rect.x, FontGwin->Rect.y);
	MApi_GOP_GWIN_Switch2Gwin(FontGwin->u8GWinID);
	#else
	MApi_GOP_GWIN_MapFB2Win(FontGwin->u8SubFbID,FontGwin->u8GWinID);
    MApi_GOP_GWIN_SetWinPosition(FontGwin->u8GWinID, FontGwin->Rect.x, FontGwin->Rect.y);
	#endif
	// draw clean screen by OSDW
	
	APIOSD_DBG(printf("FontGwin->u8SubFbID: 0x%X\n",FontGwin->u8SubFbID));
	APIOSD_DBG(printf("FontGwin->u8MainFBID: 0x%X\n",FontGwin->u8MainFbID));
	APIOSD_DBG(printf("FontGwin->u8GWinID: 0x%X\n",FontGwin->u8GWinID));
	APIOSD_DBG(printf("FontGwin->u8GWin2ID: 0x%X\n",FontGwin->u8GWin2ID));
    MApi_GOP_GWIN_SetForceWrite(FALSE);	

	MApi_GOP_FB_SEL(backFB);

	isFontGwinInit[Layer] = TRUE;

	return nRet;
}
//*/
//**************************************************************
// Terminate Gwin for OSDW
//
//**************************************************************
BOOLEAN msAPI_OSD_FontGWinTerminate(MSAPI_GWINLAYER Layer)
{
	GOP_GwinFBAttr fbAttr;
	if(isFontGwinInit[Layer] == FALSE)
		return FALSE;
	#if !MST9U_ASIC_1P	
	MApi_GOP_GWIN_UpdateRegAllBankOnceEx(TRUE);
	#endif
	MApi_GOP_GWIN_Enable(gstFontGwin[Layer].u8GWinID, FALSE);
	#if !MST9U_ASIC_1P	
	MApi_GOP_GWIN_Enable(gstFontGwin[Layer].u8GWin2ID, FALSE);
	//MApi_GOP_SetPINPON(gstFontGwin[Layer].u8GWin2ID,DISABLE,GOP_PINPON_DWIN);
	MApi_GOP_GWIN_UpdateRegAllBankOnceEx(FALSE);
	#endif
	msAPI_OSD_SetFontGWinHWPinpon(Layer,DISABLE);

    //clear Frame Buffer to prevent show previous UI when OSD start up
    MApi_GOP_GWIN_GetFBInfo(gstFontGwin[Layer].u8MainFbID, &fbAttr);
	// OSDW to clear buffer
	#if !MST9U_ASIC_1P
    MApi_GOP_GWIN_Delete2Win(gstFontGwin[Layer].u8GWinID, gstFontGwin[Layer].u8GWin2ID);
	#else
	MApi_GOP_GWIN_DeleteWin(gstFontGwin[Layer].u8GWinID);
	#endif
    MApi_GOP_GWIN_DeleteFB(gstFontGwin[Layer].u8MainFbID);
    gstFontGwin[Layer].u8MainFbID = 0xFF;
	MApi_GOP_GWIN_DeleteFB(gstFontGwin[Layer].u8SubFbID);
	gstFontGwin[Layer].u8SubFbID= 0xFF;	
	isFontGwinInit[Layer] = FALSE;
    return TRUE;
}
void msAPI_OSD_SetFontGWinHWPinpon(MSAPI_GWINLAYER Layer,BOOLEAN enable)
{
	if(isFontGwinInit[Layer] == FALSE)
		return;	
	#if !MST9U_ASIC_1P
	MApi_GOP_SetPINPON(gstFontGwin[Layer].u8GWinID,enable,GOP_PINPON_DWIN);
	MApi_GOP_SetPINPON(gstFontGwin[Layer].u8GWin2ID,enable,GOP_PINPON_DWIN);
	#else
	MApi_GOP_SetPINPON(gstFontGwin[Layer].u8GWinID,enable,GOP_PINPON_DWIN);
	#endif	
}
void msAPI_OSD_SetGwinPosition(MSAPI_GWINLAYER Layer,U16 u16x,U16 u16y)	
{
#if !MST9U_ASIC_1P
	MApi_GOP_GWIN_Set2WinPosition(gstFontGwin[Layer].u8GWinID, gstFontGwin[Layer].u8GWin2ID, u16x, u16y);
#else	
	MApi_GOP_GWIN_SetWinPosition(gstFontGwin[Layer].u8GWinID, u16x, u16y);
#endif
}
U32 msAPI_OSD_GetFbAddr(U8 u8FbIndex)
{
	GOP_GwinFBAttr fbAttr;
	MApi_GOP_GWIN_Get32FBInfo(u8FbIndex,&fbAttr);
	return fbAttr.addr;
}
void msAPI_OSD_PreFBSwitch(MSAPI_GWINLAYER Layer)
{
	U8 temp;
	temp = gstFontGwin[Layer].u8MainFbID;
	gstFontGwin[Layer].u8MainFbID = gstFontGwin[Layer].u8SubFbID;
	gstFontGwin[Layer].u8SubFbID = temp;
}
void msAPI_OSD_PostFBSwitch(MSAPI_GWINLAYER Layer)
{
#if !MST9U_ASIC_1P
	MApi_GOP_GWIN_MapFBTo2Win(gstFontGwin[Layer].u8MainFbID, gstFontGwin[Layer].u8GWinID, gstFontGwin[Layer].u8GWin2ID);
#else
	MApi_GOP_GWIN_MapFB2Win(gstFontGwin[Layer].u8MainFbID, gstFontGwin[Layer].u8GWinID);
#endif //#if (CHIP_ID==MST9U2)
	MApi_GOP_GWIN_Switch2Gwin(gstFontGwin[Layer].u8GWinID);
}
void msAPI_OSD_FontGWinOpen(MSAPI_GWINLAYER Layer)
{
#if !MST9U_ASIC_1P	
	MApi_GOP_GWIN_UpdateRegAllBankOnceEx(TRUE);
#endif
	MApi_GOP_GWIN_Enable(gstFontGwin[Layer].u8GWinID, TRUE);
#if !MST9U_ASIC_1P	
	MApi_GOP_GWIN_Enable(gstFontGwin[Layer].u8GWin2ID, TRUE);
	MApi_GOP_GWIN_UpdateRegAllBankOnceEx(FALSE);
#endif
}
#endif
// Wrapper
BMPHANDLE MDrv_GE_LoadBitmap(MS_U32 addr, MS_U32 u32len, MS_U16 u16width, MS_U16 u16height, MS_U8 dstloc)
{
  #if 0
    dstloc = 0;
  #else // Modified by coverity_0467
    UNUSED(dstloc);
  #endif

    return msAPI_OSD_RESOURCE_LoadBitmap(addr, u32len, u16width, u16height, BMP_FMT_ARGB1555);
}

void MDrv_GE_DrawBitmap(BMPHANDLE handle, GEPoint *ppoint, GEBitmapFmt *pbmpfmt)
{
  //  GEPoint v0;
    GFX_DrawBmpInfo gfx_bmpfmt;

    gfx_bmpfmt.x = ppoint->x;
    gfx_bmpfmt.y = ppoint->y;

    gfx_bmpfmt.height = pbmpfmt->height;
    gfx_bmpfmt.width = pbmpfmt->width;

    gfx_bmpfmt.bScale= pbmpfmt->bScale;
    MApi_GFX_DrawBitmap(handle, &gfx_bmpfmt);

}


void MDrv_GE_BitBlt(GEBitBltInfo *BitbltInfo, GEPitBaseInfo *PitBaseInfo)
{

    GFX_DrawRect bitbltInfo;
    GFX_BufferInfo srcbuf_bak, dstbuf_bak, srcbuf, dstbuf;
    MS_U32 flag = 0;

    bitbltInfo.srcblk.x = BitbltInfo->BitbltCoordinate.v2_x;
    bitbltInfo.srcblk.y = BitbltInfo->BitbltCoordinate.v2_y;
    bitbltInfo.srcblk.width = BitbltInfo->src_width;
    bitbltInfo.srcblk.height = BitbltInfo->src_height;

    bitbltInfo.dstblk.x = BitbltInfo->BitbltCoordinate.v0_x;
    bitbltInfo.dstblk.y = BitbltInfo->BitbltCoordinate.v0_y;
    bitbltInfo.dstblk.width = BitbltInfo->BitbltCoordinate.width;
    bitbltInfo.dstblk.height = BitbltInfo->BitbltCoordinate.height;

    MApi_GFX_GetBufferInfo(&srcbuf_bak, &dstbuf_bak);

    // there is no such source format, set to legacy 1555.
    if( GFX_FMT_ARGB1555_DST == BitbltInfo->src_fm )
    {
        srcbuf.u32ColorFmt = GFX_FMT_ARGB1555;
        MApi_GFX_SetAlpha_ARGB1555(0xFF); //for alpha channel of ARGB1555 bitblt
    }
    else
        srcbuf.u32ColorFmt = (GFX_Buffer_Format)BitbltInfo->src_fm;
    srcbuf.u32Addr = PitBaseInfo->sb_base;
    srcbuf.u32Pitch = PitBaseInfo->sb_pit;
    MApi_GFX_SetSrcBufferInfo(&srcbuf, 0);

    dstbuf.u32ColorFmt = (GFX_Buffer_Format)BitbltInfo->dst_fm;
    dstbuf.u32Addr = PitBaseInfo->db_base;
    dstbuf.u32Pitch = PitBaseInfo->db_pit;
    MApi_GFX_SetDstBufferInfo(&dstbuf, 0);

//printf("MDrv_GE_BitBlt: %x, %x, %x, %x, %x, %x\n", srcbuf.u32ColorFmt, srcbuf.u32Addr, srcbuf.u32Pitch, dstbuf.u32ColorFmt,
//        dstbuf.u32Addr, dstbuf.u32Pitch );

    if( (bitbltInfo.srcblk.width != bitbltInfo.dstblk.width) ||
        (bitbltInfo.srcblk.height != bitbltInfo.dstblk.height))
        flag = GFXDRAW_FLAG_SCALE;

    {
        MApi_GFX_BitBlt(&bitbltInfo, flag);
    }

    MApi_GFX_SetSrcBufferInfo(&srcbuf_bak, 0);
    MApi_GFX_SetDstBufferInfo(&dstbuf_bak, 0);


}


#undef MSAPI_OSD_C
