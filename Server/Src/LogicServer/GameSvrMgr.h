#include "..\Message\Msg_Login.pb.h"
#ifndef __GAME_SVR_MGR__
#define __GAME_SVR_MGR__

struct GameSvrInfo
{
	GameSvrInfo(UINT32 svrID, UINT32 conID)
	{
		dwSvrID = svrID;
		dwConnID = conID;
	}
	UINT32 dwSvrID;
	UINT32 dwConnID;
	UINT32 dwLoad;		//����ֵ
};

class CGameSvrMgr
{
private:
	CGameSvrMgr(void);
	~CGameSvrMgr(void);
public:
	static CGameSvrMgr* GetInstancePtr();
public:

	BOOL	DispatchPacket( NetPacket *pNetPacket);

	UINT32  GetServerIDByCopyID(UINT32 dwCopyGuid);

	UINT32	GetFreeGameServerID();

	BOOL	SendCreateSceneCmd(UINT32 dwServerID, UINT32 dwCopyID, UINT64 CreateParam, UINT32 dwPlayerNum);

	UINT32  GetConnIDBySvrID(UINT32 dwServerID);

	BOOL	GetMainScene(UINT32 &dwServerID, UINT32 &dwConnID, UINT32 &dwCopyGuid);
    BOOL    SendPlayerToCopy(UINT64 u64RoleID, UINT32 dwCopyID, UINT32 dwCopyGuid, UINT32 dwSvrID);
    BOOL	CreateScene(UINT32 dwCopyID, UINT64 CreateParam, UINT32 dwPlayerNum );


public:
	BOOL	OnCreateMainCopy(CreateNewSceneAck &Ack);


public:
	BOOL	OnCloseConnect(UINT32 dwConnID);
	BOOL	OnMsgGameSvrRegister(NetPacket *pNetPacket); //��Ӧ����������ע��
	BOOL	OnMsgCreateSceneAck(NetPacket *pNetPacket);  //��Ӧ���������ɹ�
    BOOL    OnMsgTransRoleDataAck(NetPacket *pNetPacket);//��Ӧ��ɫ���ݴ���ɹ�
    BOOL    OnMsgEnterSceneReq(NetPacket *pNetPacket);
	//BOOL	OnMsgCopyReportReq(NetPacket *pNetPacket);
	
	

public:
	std::map<UINT32, GameSvrInfo> m_mapGameSvr; //������ID-->������������Ϣ
};

#endif
