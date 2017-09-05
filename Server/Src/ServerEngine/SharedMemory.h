#ifndef __SHARE_MEMORY_H__
#define __SHARE_MEMORY_H__
#include "../ServerData/ServerStruct.h"

#define BLOCK_CHECK_CODE	0x5A

/**�����ڴ��״̬
*/
enum SharedMemoryState
{
	SMS_NONE,  ///δʹ�ã�����״̬
	SMS_USE,    //�Ѿ�ʹ���ˣ����ݿ���������Զ�ȡ�޸�д�����ݿ�
	SMS_LOCK,///��ס״̬���߼�����������д��
	SMS_RELEASE,///�߼��������Ѿ��ͷ��ˡ����ݿ������д���޸ĺ������ΪSMS_NONE״̬
	SMS_DELETE,///ɾ����־
};

///���зŵ�sharedMemory���Ԫ�ض������Ǵ�ShareObject������
struct ShareObject
{
	ShareObject();

	///��ʼ�޸ģ����Ϊ��ռ��
	void lock();

	///�ж��Ƿ�ռ����
	BOOL islock()const;

	///���Ϊ������ɡ�
	void unlock();

	void useit();

	///���Ϊ�Ѿ��ͷ���
	void release();

	void destroy();

	BOOL isDestroy() const;

	BOOL isRelease() const;

	time_t getLastMotifyTime();

	///�Ƿ���ʹ��
	BOOL isUse() const;

	void reset();

	UINT32					   m_dwCheckCode;
	SharedMemoryState          m_State;
	time_t                     m_updatetime;	///���һ���޸�ʱ��
};

///��¼ÿ��T���״̬
struct _SMBlock
{
	UINT32			m_dwIndex;      //���ݵ�ǰ���
	BOOL			m_bUse;         //�Ƿ���ʹ��true������ʹ�ã�false��û��ʹ��
	BOOL			m_bNewBlock;	///�Ƿ��Ǹո��´���������
	time_t			m_beforeTime;   //DS������������ɺ��д����Ϣʱ�䡣
	time_t          m_afterTime;
	_SMBlock()
	{
		m_dwIndex = 0;
		m_bUse = false;
		m_beforeTime = 0;
		m_afterTime = 0;
		m_bNewBlock = false;
	}
};

///�����ڴ�ҳ�ṹ
struct shareMemoryPage
{
	char*        m_pdata;///ָ�������ڴ��ַ
	_SMBlock*    m_pBlock;///���ݿ��ͷλ��
	HANDLE       m_shm;///
};

class SharedMemoryBase
{
public:
	SharedMemoryBase(const std::string& name, UINT32 rawblockSize, UINT32 count, BOOL noCreate = false);

	SharedMemoryBase(UINT32 rawblockSize, char* pdata, INT32 len);

	virtual ~SharedMemoryBase();
protected:

	typedef std::vector<shareMemoryPage> ShareMemoryPageMapping;
	///�����ڴ�ҳӳ��.
	ShareMemoryPageMapping m_ShareMemoryPageMapping;

	UINT32			m_countperPage;///ҳ������T��������
	UINT32			m_pageCount;///ҳ����
	UINT32			m_count;///T���͵��ܸ���,T���ͱ����Ƕ����ġ�
	UINT32			m_space;///ÿ��Ԫ�صĿ��
	UINT32			m_rawblockSize;
	std::string		m_modulename;
	BOOL			isempty;

	///��������ͷ�ļ���
	typedef  std::map<INT32, _SMBlock*>    mapSMBlock;
	mapSMBlock                     m_mapSMBlock;///�������ݿ�ͷ��Ϣ


	typedef std::map<void*, _SMBlock*>  mapUsedSMBlock;
	mapUsedSMBlock                 m_mapUsedSMBlock;	///����ʹ���˿��������Ϣ


	typedef std::map<INT32, _SMBlock*> mapFreeSMBlock;
	mapFreeSMBlock                 m_mapFreeSMBlock;///���п��еĿ���Ϣ
private:

	///����һ����ҳ
	BOOL NewPage();


	/**
	* @brief		��ʼ����������
	* @details		������0�������ñ�������
	* @param[in]	rPage : �����ڴ�ҳ
	* @return		void
	* @remarks
	*/
	void InitPage(shareMemoryPage& rPage);


public:

	///���ݿ����������Ҫ��ʼ��map,�߼�����������Ҫ,���Էֿ�
	void InitToMap();


	/**�Ƿ����״������ڴ�*/
	BOOL IsFirstCreated();


	/**�ӹ����ڴ���ָ�����ҳ*/
	void ImportOtherPage();

	/**��ȡ����*/
	const UINT32 GetCount()const;

	/**��ȡ���ж��ٿ�����ڴ�
	*/
	UINT32 GetFreeCount()const;

	///��ȡ�Ѿ�ʹ���˶��ٿ�
	UINT32 GetUseCount()const;

	/**ͨ��id��ȡԭʼ�ڴ��е�������ָ��
	*/
	virtual _SMBlock* GetSMBbyRawIndex(INT32 index);

	/**ͨ��id��ȡԭʼ�ڴ��е�������ָ��
	*/
	virtual ShareObject*  GetObjectByRawindex(UINT32 index);


	const UINT32 GetRawMemoryBlockSize();

	const INT32 GetBlockSize() { return m_rawblockSize; }

	/*�������������б����ݿ�������ͷŵ�����*/
	void ProcessCleanDirtyData();

	/*�ӿ����ڴ��з���һ����,���û���˷��ؿ�
	@param isNewBlock Ϊtrueʱ���ڱ����ڵ���saveobject ��Create�麯��
	*/
	virtual ShareObject* NewObject(BOOL isNewBlock = false);

	/**�ͷ�һ���Ѿ�����ʹ�õ��ڴ�
	*/
	virtual BOOL DestoryObject(ShareObject* pobject);
};

template<typename T>
class SharedMemory : public SharedMemoryBase
{
public:
	SharedMemory(const std::string& name, UINT32 count, BOOL noCreate = false)
		: SharedMemoryBase(name, sizeof(T), count, noCreate)
	{

	}

	//SharedMemory(const std::string& name, char* pdata, INT32 len)
	//	: SharedMemoryBase(sizeof(T), pdata, len)
	//{

	//}

	T*  GetObjectByRawindex(UINT32 index)
	{
		return static_cast<T*>(SharedMemoryBase::GetObjectByRawindex(index));
	}

	T* NewObject(BOOL isNewBlock = false)
	{
		T* pTmp = static_cast<T*>(SharedMemoryBase::NewObject(isNewBlock));
		if (pTmp == NULL)
		{
			return NULL;
		}

		new(pTmp)(T);
		return pTmp;
	}

	_SMBlock* GetSMBbyRawIndex(INT32 index)
	{
		return SharedMemoryBase::GetSMBbyRawIndex(index);
	}

	BOOL DestoryObject(T* pobject)
	{
		return SharedMemoryBase::DestoryObject(pobject);
	}
};


template <typename T> class DataWriter
{
public:
	DataWriter(const std::string& modulename)
	{
		m_MemoryPool = NULL;
		m_moduleName = modulename;
		m_count = 1024;
	}
	~DataWriter()
	{
		delete m_MemoryPool;
		m_MemoryPool = NULL;
	}

	/**���ݿ��޸�*/
	BOOL SaveModifyToDB(IDataBase* pdb)
	{
		///�����ڴ治����ֱ�ӷ���
		if (m_MemoryPool == NULL)
		{
			m_MemoryPool = new SharedMemory<T>(m_moduleName, m_count, true);
		}
		if (m_MemoryPool == NULL)
		{
			return false;
		}

		if (m_MemoryPool->IsFirstCreated())
		{
			///�����ڴ滹û����
			delete m_MemoryPool;
			m_MemoryPool = NULL;
			return false;
		}

		INT32 newtimes = 0, writetimes = 0, deletetimes = 0, releasetime = 0;
		BOOL hasOprate = false; //�Ƿ��в���
		///��ȡ�����޸Ĺ�������,getRawMemoryBlockSize�����¼������й���飬
		UINT32 temblockSize = m_MemoryPool->GetRawMemoryBlockSize();
		for (UINT32 r = 0; r < temblockSize; r++)
		{
			_SMBlock* pBlock = m_MemoryPool->GetSMBbyRawIndex(r);
			if (pBlock == NULL)
			{
				continue;
			}
			if (pBlock->m_bUse == false)
			{
				continue;
			}
			T* pdata = m_MemoryPool->GetObjectByRawindex(r);
			if (pdata == NULL)
			{
				continue;
			}
			if (pdata->m_dwCheckCode != BLOCK_CHECK_CODE)
			{
				continue;
			}
			if (!pdata->isUse())
			{
				continue;
			}
			///�����޸�����,����
			if (pdata->islock())
			{
				continue;
			}
			///���Ȼص�ɾ��
			if (pdata->isDestroy())
			{
				///����һ�����������첽ִ��,�������Ч��
				pdata->Delete(pdb);
				m_MemoryPool->DestoryObject(pdata);
				hasOprate = true;
				deletetimes++;
				continue;
			}
			///��λص��½�
			if (pBlock->m_bNewBlock)
			{
				///����һ�����������첽ִ��,�������Ч��
				pBlock->m_beforeTime = time(NULL);
				pdata->Save(pdb);
				pBlock->m_bNewBlock = false;
				pBlock->m_afterTime = time(NULL);
				hasOprate = true;
				newtimes++;
				continue;
			}

			time_t lastMotifyTime;
			time_t beforeTime, afterTime;
			lastMotifyTime = pdata->getLastMotifyTime();
			beforeTime = pBlock->m_beforeTime;
			afterTime = pBlock->m_afterTime;
			BOOL needsave = false;
			///������ϵ�ʱ����ڱ���ǰ��ʱ��,��ô��һ�α���ɹ���
			if (afterTime >= beforeTime)
			{
				if (lastMotifyTime > beforeTime)
				{
					needsave = true;
				}
			}
			else
			{
				needsave = true;///��һ�α���ʧ��,��������
			}

			if (needsave)
			{
				///����һ�����������첽ִ��,�������Ч��
				pBlock->m_beforeTime = time(NULL);
				pdata->Save(pdb);
				hasOprate = true;
				writetimes++;
				pBlock->m_afterTime = time(NULL);
				continue;
			}


			if (pdata->isRelease())
			{
				///�ͷŵ�ʱ��ִ��һ�α���...����ϴ�û�б���ɹ����ߣ��ͷ�ǰ�޸��˾��ٱ���һ��
				if ((lastMotifyTime > 0) && (afterTime < beforeTime || lastMotifyTime > beforeTime)) ///change by dsq
				{
					pBlock->m_beforeTime = time(NULL);/// add by dsq
					pdata->Save(pdb);
					hasOprate = true;
					writetimes++;
					continue;
				}
				m_MemoryPool->DestoryObject(pdata);
				releasetime++;
			}

		}


		//xLogMessager::getSingleton().logMessage(std::string("sync db module:") + m_moduleName +
		//                                        std::string(" finished.write:") + Helper::IntToString(writetimes) +
		//                                        std::string(" .new:") + Helper::IntToString(newtimes) +
		//                                        std::string(" .desdroy:") + Helper::IntToString(deletetimes) +
		//                                        std::string(".release:") + Helper::IntToString(releasetime),
		//                                        Log_ErrorLevel
		//                                       );
		return hasOprate;
	}
private:
	SharedMemory<T>*       m_MemoryPool;///ģ���ڴ��
	UINT32		           m_count;///�����ڴ��С
	std::string m_moduleName;
};

#endif