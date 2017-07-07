#ifndef __TYPE_DATA_PARSER_H__
#define __TYPE_DATA_PARSER_H__
#include "Sqlite\CppSQLite3.h"
#include "..\ConfigData\ConfigStruct.h"
class CConfigData;

typedef BOOL(CConfigData::*DataFunc)(CppSQLite3Query &QueryData);

struct DataFuncNode 
{
    std::string m_strTbName;
    DataFunc m_pDataFunc;

    DataFuncNode(std::string sValue, DataFunc func)
    {
        m_strTbName = sValue;
        m_pDataFunc = func;
    }
};

class CConfigData
{
    CConfigData();
	~CConfigData();
public:
	static CConfigData* GetInstancePtr();
	BOOL InitDataReader();
	BOOL ReadConfigData(std::string strDbFile); 

public:
	//�������Ʊ���
	///////////////////////////////////////////////
	BOOL ReadConstantValue(CppSQLite3Query &QueryData);
	StConstantValue m_ConstantValue;

	//��ɫ
	///////////////////////////////////////////////
    BOOL ReadRoleType(CppSQLite3Query &QueryData);
	
	//����
	///////////////////////////////////////////////
	BOOL ReadCopyBase(CppSQLite3Query &QueryData);
	std::map<UINT32, StCopyBase> m_mapCopybase; 
	StCopyBase* GetCopyBaseInfo(UINT32 dwCopyType);

	//�������Ʊ�
	///////////////////////////////////////////////
	BOOL ReadLanguage(CppSQLite3Query &QueryData);
	std::map<UINT32, StLocalString> m_mapLocalString;
	
	
	
public:

	CppSQLite3DB	m_DBConnection;

	std::vector<DataFuncNode> m_vtDataFuncList;
};



#endif //__TYPE_DATA_PARSER_H__
