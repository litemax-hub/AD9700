
#ifndef _APPMSTAR_H_
#define _APPMSTAR_H_

typedef void ( *fpappmStarCb )                          (void);
typedef void ( *fpappmStarCustomizeIPOptionInitCb )     (void);
typedef BOOL ( *fbappmstarCustomizeFakeSleepConditionCb)(void);
typedef BYTE ( *fbappmstarCustomizeWinColorConditionCb)(void);
// call back function type
enum
{
    eCB_appmStar_CustomizeIPOptionInit,
    eCB_appmStar_CustomizeFakeSleepCondition,
    eCB_appmStar_CustomizeWinColorCondition,
    // etc ..
    eCB_appmStar_CBMaxNum,
};

extern fpappmStarCb appmStar_CB_FuncList[eCB_appmStar_CBMaxNum];


extern Bool appmStar_SetPanelTiming(void);
extern WORD mSTar_GetInputHTotal(void);
extern void appmStar_CbAttach(BYTE CbType, void *CbFunc);
extern void appmStar_CbInit(void);
extern bool appmStar_CbGetInitFlag(void);
extern void appmStar_CbSetInitFlag(bool bInit);

#endif
