#ifndef __MYSQL_HELPER_H__
#define __MYSQL_HELPER_H__

#include "mysql.h"

class CppMySQL3DB;

class CppMySQLQuery
{
	friend class CppMySQL3DB;
public:
	CppMySQLQuery();

	//��ִ�п������캯��������������Ѿ���Ч��������ʹ��
	CppMySQLQuery(CppMySQLQuery& rQuery);

	// ��ִ�и�ֵ���캯���� = �ұߵ����Ѿ���Ч��������ʹ��
	CppMySQLQuery& operator=(CppMySQLQuery& rQuery);

	virtual ~CppMySQLQuery();

	int numRow();

	int numFields();

	int fieldIndex(const char* szField);

	//0...n-1��
	const char* fieldName(int nCol);

	int seekRow(int offerset);
	int getIntField(int nField, int nNullValue = 0);
	int getIntField(const char* szField, int nNullValue = 0);

	INT64 getInt64Field(int nField, INT64 nNullValue = 0);
	INT64 getInt64Field(const char* szField, INT64 nNullValue = 0);

	double getFloatField(int nField, double fNullValue = 0.0);
	double getFloatField(const char* szField, double fNullValue = 0.0);

	//0...n-1��
	const char* getStringField(int nField, const char* szNullValue = "");
	const char* getStringField(const char* szField, const char* szNullValue = "");

	const unsigned char* getBlobField(int nField, int& nLen);
	const unsigned char* getBlobField(const char* szField, int& nLen);

	bool fieldIsNull(int nField);
	bool fieldIsNull(const char* szField);

	bool eof();
	void nextRow();

private:
	void freeRes();

private:
	MYSQL_RES*  m_MysqlRes;
	MYSQL_FIELD* _field;
	MYSQL_ROW  _row;
	int   _row_count;
	int   _field_count;
};

class CppMySQL3DB
{
public:
	CppMySQL3DB();
	virtual ~CppMySQL3DB();

	bool open(const char* host, const char* user, const char* passwd, const char* db,
	          unsigned int port = 0, unsigned long client_flag = 0);

	void close();

	/* ���ؾ�� */
	MYSQL* getMysql();

	/* �����ض��еĲ�ѯ������Ӱ������� */
	//������������Ϊ��CppMySQLQuery�ĸ�ֵ���캯����Ҫ�ѳ�Ա����_mysql_res��Ϊ��
	CppMySQLQuery& querySQL(const char* sql);

	/* ִ�зǷ��ؽ����ѯ */
	int execSQL(const char* sql);

	/* ����mysql�������Ƿ��� */
	int ping();

	/* �ر�mysql ������ */
	int shutDown();

	/* ��Ҫ����:��������mysql ������ */
	int reboot();

	/*˵��:����֧��InnoDB or BDB������*/
	/* ��Ҫ����:��ʼ���� */
	int startTransaction();

	/* ��Ҫ����:�ύ���� */
	int commit();

	/* ��Ҫ����:�ع����� */
	int rollback();

	/* �õ��ͻ���Ϣ */
	const char* getClientInfo();

	/* ��Ҫ����:�õ��ͻ��汾��Ϣ */
	const unsigned long  getClientVersion();

	/* ��Ҫ����:�õ�������Ϣ */
	const char* getHostInfo();

	/* ��Ҫ����:�õ���������Ϣ */
	const char* getServerInfo();

	/*��Ҫ����:�õ��������汾��Ϣ*/
	const unsigned long  getServerVersion();

	/*��Ҫ����:�õ� ��ǰ���ӵ�Ĭ���ַ���*/
	const char*   getCharacterSetName();

	/* ���������ݿ� */
	int createDB(const char* name);

	/* ɾ���ƶ������ݿ�*/
	int dropDB(const char* name);

private:
	CppMySQL3DB(const CppMySQL3DB& db);
	CppMySQL3DB& operator=(const CppMySQL3DB& db);

private:
	/* msyql ���Ӿ�� */
	MYSQL* _db_ptr;
	CppMySQLQuery _db_query;
};

#endif //__MYSQL_HELPER_H__