#ifndef __CALLBACK_H_
#define __CALLBACK_H_

#include "TSPSolver.h"

void XPRS_CC cbmessage(XPRSprob prob, void* data, const char *sMsg, int nLen, int nMsgLvl);

void XPRS_CC cbintsol(XPRSprob prob, void* vContext);

void XPRS_CC errormsg(const char *sSubName, int nLineNo, int nErrCode);




#endif