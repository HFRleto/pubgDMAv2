#pragma once

// Local no-op stub replacing the closed-source SProtect SPCloud SDK.
// Same API surface as the original header, but performs no networking,
// no machine fingerprinting and links against no closed-source .lib.
// Replace this file with a real implementation to re-add authentication.

#define SP_STUB_NO_AUTH 1

#include <cstdlib>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifdef _WIN64
#define SPAPI
#else
#define SPAPI __stdcall
#endif

	enum SPErrorCode {
		SPCODE_NOERROR = 0,
		SPCODE_NOINIT = -1,
		SPCODE_WSAFAILED = -2,
		SPCODE_CONNECTFAILED = -3,
		SPCODE_DATAERROR = -4,
		SPCODE_NOLOADCLOUDDLL = -5,
		SPCODE_PACKERROR = -6,
		SPCODE_EXPIREDTOKEN = -15,
		SPCODE_UNKNOWNERROR = -16,
		SPCODE_UNKNOWNAGENTSTATE = -17,
		SPCODE_INVALIDCARD = -21,
		SPCODE_EXPIREDCARD = -22,
		SPCODE_BANNEDCARD = -23,
		SPCODE_INVALIDAGENT = -24,
		SPCODE_BANNEDAGENT = -25,
		SPCODE_MAXONLINE = -26,
		SPCODE_OFFLINE = -27,
		SPCODE_ANOTHERUSERUNBIND = -28,
		SPCODE_FAILEDACTIVATECARD = -29,
		SPCODE_QUERYAGENTEXCEPTION = -30,
		SPCODE_INVALIDPARAMETER = -31,
		SPCODE_FYINOTENOUGH = -32,
		SPCODE_DISABLETRIALCLOSE = -33,
		SPCODE_USERNAMEERROR = -34,
		SPCODE_RECHARGEFAILED = -35,
		SPCODE_SERVERFORCEOFFLINE = -36,
		SPCODE_INVALIDCONTEXT = -37,
		SPCODE_BINDMSGDIFF = -100,
		SPCODE_OPERATEEXCEPTION = -101,
		SPCODE_MAXREBINDCOUNT = -102,
		SPCODE_NOENOUGHTIME = -103,
		SPCODE_MAXUNBINDCOUNT = -104,
		SPCODE_NOALLOWREMOVECLIENT = -105,
		SPCODE_APPLIEDTRIAL = -106,
		SPCODE_EXPIREDTRIAL = -107,
		SPCODE_DISABLEREGISTER = -108,
		SPCODE_INVALIDUSER = -109,
		SPCODE_INVALIDPASS = -110,
		SPCODE_INVALIDRECHARGECARD = -111,
		SPCODE_DISABLERECHARGE = -112,
		SPCODE_TRIALCARDUSEDFORRECHARGING = -113,
		SPCODE_DISABLEMIXCARDRECHARGE = -114,
		SPCODE_DISABLEMIXAGENTRECHARGE = -115,
		SPCODE_BINDCARDUSEDFORRECHARGING = -116,
		SPCODE_USEDCARD = -117,
		SPCODE_CARDDISABLE = -118,
		SPCODE_INVALIDNEWPWD = -119,
		SPCODE_INVALIDCARDPARAMETER = -120,
		SPCODE_DISABLEGETUSERINFO = -121,
		SPCODE_CLOUDIDOUTOFRANGE = -122,
	};

#pragma pack(push)
#pragma pack(1)
	struct tagPCSignInfo {
		unsigned long long	u64BindTS;
		char*				szWinVer;
		char*				szRemark;
		char*				szComputerName;
		char*				szPCSign;
		unsigned long long	u64LastLoginTS;
		void*				Reserved[20];
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
	struct tagPCSignInfoHead {
		unsigned int		u32Count;
		tagPCSignInfo*	Info;
		unsigned int		u32BindIP;
		unsigned int		u32RestCount;
		unsigned long long	u64RefreshCountdownSeconds;
		unsigned int		u32Limit;
		void*			Reserved[19];
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
	struct tagOnlineInfo {
		unsigned int	u32CID;
		char*		szComputerName;
		char*		szWinVer;
		unsigned long long	u64CloudInitTS;
		void*		Reserved[20];
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
	struct tagOnlineInfoHead {
		unsigned int			u32Count;
		tagOnlineInfo*		Info;
		void*				Reserved[20];
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
	struct tagUserRechargedInfo {
		unsigned long long	u64OldExpiredTimeStamp;
		unsigned long long	u64NewExpiredTimeStamp;
		unsigned long long	u64OldFYI;
		unsigned long long	u64NewFYI;
		unsigned int	u32RechargeCount;
		void* Reserved[80];
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
	struct tagBasicInfo {
		unsigned int	ForbidTrial;
		unsigned int	ForbidLogin;
		unsigned int	ForbidRegister;
		unsigned int	ForbidRecharge;
		unsigned int	ForbidCloudGetCountinfo;
		unsigned int	ForbidRetrievePassword;
		unsigned int	Reserved[14];
	};
#pragma pack(pop)

	inline void* SPAPI SP_Cloud_Create() {
		static int s_stubContext = 0;
		return &s_stubContext;
	}

	inline void SPAPI SP_Cloud_Destroy(void* pContext) { (void)pContext; }

	inline bool SPAPI SP_CloudInit(void* pContext, int iTimeout, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)iTimeout;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline void SPAPI SP_CloudSetConnInfo(void* pContext, const char* szSoftwareName, const char* szIP, int wPort, int iTimeout, int iLocalVer, bool bPopMsg) {
		(void)pContext; (void)szSoftwareName; (void)szIP; (void)wPort; (void)iTimeout; (void)iLocalVer; (void)bPopMsg;
	}

	inline bool SPAPI SP_CloudLogin(void* pContext, const char* szCard, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szCard;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_CloudUserLogin(void* pContext, const char* szUser, const char* szPassword, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szPassword;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_CloudComputing(void* pContext, IN int dwCloudID, IN OPTIONAL unsigned char* pInBuffer, IN OPTIONAL unsigned int dwInLength, OUT OPTIONAL unsigned char** pOutBuffer, OUT OPTIONAL unsigned int* dwOutLength, OPTIONAL SPErrorCode* iError,
		unsigned int u32RetryCount = 0, unsigned int u32RetryIntervalMs = 0) {
		(void)pContext; (void)dwCloudID; (void)pInBuffer; (void)dwInLength; (void)u32RetryCount; (void)u32RetryIntervalMs;
		if (pOutBuffer) *pOutBuffer = 0;
		if (dwOutLength) *dwOutLength = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_Beat(void* pContext, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetCardAgent(void* pContext, char szAgent[44], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szAgent[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetCardType(void* pContext, char szCardType[36], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szCardType[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetIPAddress(void* pContext, char szIPAddress[44], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szIPAddress[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetRemarks(void* pContext, char szRemarks[132], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szRemarks[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetCreatedTimeStamp(void* pContext, __int64* iCreatedTimeStamp, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iCreatedTimeStamp) *iCreatedTimeStamp = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetActivatedTimeStamp(void* pContext, __int64* iActivatedTimeStamp, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iActivatedTimeStamp) *iActivatedTimeStamp = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetExpiredTimeStamp(void* pContext, __int64* iExpiredTimeStamp, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iExpiredTimeStamp) *iExpiredTimeStamp = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetLastLoginTimeStamp(void* pContext, __int64* iLastLoginTimeStamp, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iLastLoginTimeStamp) *iLastLoginTimeStamp = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetFYI(void* pContext, __int64* iFYI, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iFYI) *iFYI = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_DeductFYI(void* pContext, __int64 iFYICount, __int64* iSurplusFYI, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)iFYICount;
		if (iSurplusFYI) *iSurplusFYI = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_GetOpenMaxNum(void* pContext, int* iNum, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iNum) *iNum = 1;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetBind(void* pContext, int* iBind, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iBind) *iBind = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetBindTime(void* pContext, __int64* iBindTime, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iBindTime) *iBindTime = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetUnBindDeductTime(void* pContext, __int64* iDeductSec, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iDeductSec) *iDeductSec = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetUnBindMaxNum(void* pContext, int* iNum, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iNum) *iNum = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetUnBindCountTotal(void* pContext, int* iCountTotal, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iCountTotal) *iCountTotal = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetUnBindDeductTimeTotal(void* pContext, __int64* iDeductTimeTotal, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iDeductTimeTotal) *iDeductTimeTotal = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_Offline(void* pContext, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetNotices(void* pContext, char szNoteices[65535], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szNoteices[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetCard(void* pContext, char szCard[1 + 41], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szCard[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetUser(void* pContext, char szCard[1 + 32], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szCard[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline void SPAPI SP_Cloud_DisableCard(void* pContext, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iError) *iError = SPCODE_NOERROR;
	}

	inline bool SPAPI SP_Cloud_GetCID(void* pContext, OUT int* pCID, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (pCID) *pCID = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetOnlineCount(void* pContext, int* iCount, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iCount) *iCount = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_SetWinVer(void* pContext, const char* szWinVer, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szWinVer;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetPCSign(void* pContext, char szPCSign[33]) {
		(void)pContext;
		szPCSign[0] = 0;
		return false;
	}

	inline bool SPAPI SP_Cloud_GetUnBindCount(void* pContext, int* iCount, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iCount) *iCount = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetUpdateInfo(void* pContext,
		OUT OPTIONAL int* bForce,
		OUT OPTIONAL int* dwVer,
		OUT OPTIONAL int* bDirectUrl,
		OUT OPTIONAL char szUrl[2049],
		OUT OPTIONAL char szRunExe[101],
		OUT OPTIONAL char szRunCmd[129],
		OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (bForce) *bForce = 0;
		if (dwVer) *dwVer = 0;
		if (bDirectUrl) *bDirectUrl = 0;
		szUrl[0] = 0;
		szRunExe[0] = 0;
		szRunCmd[0] = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return false;
	}

	inline int SPAPI SP_Cloud_GetLocalVerNumber(void* pContext) { (void)pContext; return 0; }

	inline bool SPAPI SP_Cloud_GetOnlineTotalCount(void* pContext, unsigned int* iTotalCount, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iTotalCount) *iTotalCount = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetOnlineCardsCount(void* pContext, unsigned int* iTotalCount, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (iTotalCount) *iTotalCount = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_GetOnlineCountByCard(void* pContext, OPTIONAL const char* szCard, unsigned int* iTotalCount, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szCard;
		if (iTotalCount) *iTotalCount = 0;
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline bool SPAPI SP_Cloud_QueryPCSign(void* pContext, OPTIONAL const char* szCard, OUT tagPCSignInfoHead** Info, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szCard;
		if (Info) *Info = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_UserQueryPCSign(void* pContext, OPTIONAL const char* szUser, OPTIONAL const char* szPassword, OUT tagPCSignInfoHead** pInfo, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szPassword;
		if (pInfo) *pInfo = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_RemovePCSign(void* pContext, OPTIONAL const char* szCard, const char* szPCSign, unsigned int u32UnBindIP, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szCard; (void)szPCSign; (void)u32UnBindIP;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_UserRemovePCSign(void* pContext, const char* szUser, OPTIONAL const char* szPassword, const char* szPCSign, unsigned int u32UnBindIP, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szPassword; (void)szPCSign; (void)u32UnBindIP;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_QueryOnline(void* pContext, OPTIONAL const char* szCard, OUT tagOnlineInfoHead** pInfo, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szCard;
		if (pInfo) *pInfo = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_UserQueryOnline(void* pContext, const char* szUser, OPTIONAL const char* szPassword, OUT tagOnlineInfoHead** Info, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szPassword;
		if (Info) *Info = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_CloseOnlineByCID(void* pContext, OPTIONAL const char* szCard, unsigned int u32CID, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szCard; (void)u32CID;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_UserCloseOnlineByCID(void* pContext, const char* szUser, OPTIONAL const char* szPassword, unsigned int u32CID, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szPassword; (void)u32CID;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_ApplyTrialCard(void* pContext, OUT char szCard[42], OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		szCard[0] = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_UserRegister(void* pContext, const char* szUser, const char* szPassword, const char* szSuperPWD, const char* szRechargeCards, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szPassword; (void)szSuperPWD; (void)szRechargeCards;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_UserRecharge(void* pContext, const char* szUser, const char* szRechargeCards, tagUserRechargedInfo* pInfo, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szRechargeCards; (void)pInfo;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_UserChangePWD(void* pContext, const char* szUser, const char* szSuperPWD, const char* szNewPassword, OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szUser; (void)szSuperPWD; (void)szNewPassword;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_RetrievePassword(void* pContext, const char* szCard, OUT char szUser[33], OUT char szPassword[33], OUT char szSuperPWD[33], OPTIONAL SPErrorCode* iError) {
		(void)pContext; (void)szCard;
		szUser[0] = 0;
		szPassword[0] = 0;
		szSuperPWD[0] = 0;
		if (iError) *iError = SPCODE_OPERATEEXCEPTION;
		return false;
	}

	inline bool SPAPI SP_Cloud_GetBasicInfo(void* pContext, tagBasicInfo* pBasicInfo, OPTIONAL SPErrorCode* iError) {
		(void)pContext;
		if (pBasicInfo) {
			pBasicInfo->ForbidTrial = 0;
			pBasicInfo->ForbidLogin = 0;
			pBasicInfo->ForbidRegister = 0;
			pBasicInfo->ForbidRecharge = 0;
			pBasicInfo->ForbidCloudGetCountinfo = 0;
			pBasicInfo->ForbidRetrievePassword = 0;
		}
		if (iError) *iError = SPCODE_NOERROR;
		return true;
	}

	inline void* SPAPI SP_Cloud_Malloc(int iSize) { return malloc((size_t)iSize); }

	inline void SPAPI SP_Cloud_Free(void* pBuff) { free(pBuff); }

	inline bool SPAPI SP_Cloud_GetErrorMsg(int iError, OUT char szMsg[255]) {
		(void)iError;
		strcpy_s(szMsg, 255, "authentication removed (local stub)");
		return true;
	}

	inline void SPAPI SP_GetCpuid(int id, int abcd[4]) { (void)id; abcd[0] = abcd[1] = abcd[2] = abcd[3] = 0; }

#ifdef __cplusplus
}
#endif
