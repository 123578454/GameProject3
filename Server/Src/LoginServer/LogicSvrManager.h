#ifndef _LOGIC_SEVER_MANAGERH_
#define _LOGIC_SEVER_MANAGERH_

#include "AVLTree.h"
#include "DBInterface\CppMysql.h"

enum EServerStatue
{
	ESS_NONE		= 0,//δ֪
	ESS_GOOD		= 1,//����
	ESS_BUSY		= 2,//ӵ��
	ESS_MAINTAIN	= 3,//ά��
	ESS_SHUTDOWN	= 4,//�ر�
	ESS_REVIEW		= 5,//����
};

enum EServerFlag
{
	ESF_NONE	= 0, //�ޱ��
	ESS_NEW		= 1, //�·�
	ESS_SURGEST = 2, //�Ƽ�

};

struct LogicServerNode
{
	LogicServerNode()
	{
		m_dwConnID = 0;
		m_dwServerID = 0;
		m_dwPort = 0;
		m_Statue = ESS_GOOD;
		m_Flag = ESF_NONE;
	}

	BOOL CheckIP(UINT32 dwIPaddr)
	{
		if(m_CheckIpList.size() <= 0)
		{
			return TRUE;
		}

		return TRUE;
	}

	BOOL CheckChannel(UINT32 dwChannel)
	{
		if(m_CheckChannelList.size() <= 0)
		{
			return TRUE;
		}

		return TRUE;
	}

	BOOL CheckVersion(UINT32 dwVersion)
	{
		if(m_dwCheckVersion == 0)
		{
			return TRUE;
		}

		return TRUE;
	}

	UINT32		m_dwConnID;   //����ID
	UINT32		m_dwServerID;
	UINT32		m_dwPort;   ///�˿ں�
	UINT32		m_Statue;
	UINT32		m_Flag;
	UINT32		m_dwCheckVersion;
	std::string m_strSvrName;
	std::string m_strIpAddr;

	std::set<std::string> m_CheckIpList;
	std::set<INT32>       m_CheckChannelList; //���Կ���������

};

class LogicSvrManager : public std::map<UINT32, LogicServerNode>
{
public:
	LogicSvrManager(void);
	~LogicSvrManager(void);

public:
	BOOL	Init();

	BOOL	RegisterLogicServer(UINT32 dwConnID, UINT32 dwServerID, std::string strIpAddr, UINT32 dwPort);

	BOOL	UnregisterLogicServer(UINT32 dwConnID, UINT32 dwServerID);

	BOOL	IsReviewPackage(std::string strPackageName);

	BOOL	ReloadServerList();

	UINT32	GetLogicConnID(UINT32 dwServerID);

	LogicServerNode* GetRecommendServerInfo();

	LogicServerNode* GetLogicServerInfo(UINT32 dwServerID);
public:
	UINT32 m_dwRecommendSvrID;

	std::set<std::string> m_setReviewPackage;

	CppMySQL3DB 		m_DBConnection;
};

#endif //_LOGIC_SEVER_MANAGERH_