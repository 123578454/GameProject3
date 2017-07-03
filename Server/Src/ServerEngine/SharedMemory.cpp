#include "stdafx.h"
#include "SharedMemory.h"
#include "Utility\CommonConvert.h"

ShareObject::ShareObject()
{
	m_dwCheckCode = BLOCK_CHECK_CODE;
	m_State = SMS_USE;
	m_updatetime = 0; 
}

void ShareObject::lock()
{
	m_State = SMS_LOCK;
}

BOOL ShareObject::islock() const
{
	return m_State == SMS_LOCK;
}

void ShareObject::unlock()
{
	m_updatetime = time(NULL);
	m_State = SMS_USE;
}

void ShareObject::useit()
{
	m_State = SMS_USE;
}

void ShareObject::release()
{
	m_State = SMS_RELEASE;
}

void ShareObject::destroy()
{
	m_State = SMS_DELETE;
}

BOOL ShareObject::isDestroy() const
{
	return (m_State == SMS_DELETE);
}

BOOL ShareObject::isRelease() const
{
	return (m_State == SMS_RELEASE);
}

time_t ShareObject::getLastMotifyTime()
{
	return m_updatetime;
}

BOOL ShareObject::isUse() const
{
	return m_State != SMS_NONE;
}

void ShareObject::reset()
{
	m_State = SMS_NONE;
	m_updatetime = time(NULL);
}

/******************************************************************
ÿ�鹲���ڴ涼���������������ͷ���,��������ǰ�棬����ͷ�ں���TΪ��HΪͷ
TTTTTTTTTTTTTTTTTTTTTTHHHHHHHHHHHHHHHHHHHHH
����������ݿ��������һ����
********************************************************************/


BOOL SharedMemoryBase::newPage()
{
	unsigned int size = m_countperPage*(m_space);
	std::string pagename = std::string(m_modulename) + CommonConvert::IntToString(m_pageCount);

	shareMemoryPage newpage;

	newpage.m_shm = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE, 0, size, pagename.c_str());
	if(newpage.m_shm == NULL)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	newpage.m_pdata = (CHAR*)MapViewOfFile(newpage.m_shm, FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
	if(newpage.m_pdata == NULL)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	newpage.m_pBlock = (_SMBlock*)(newpage.m_pdata + m_rawblockSize*m_countperPage);
	m_pageCount++;
	m_count += m_countperPage;
	initpage(newpage);
	m_ShareMemoryPageMapping.push_back(newpage);
	return TRUE;
}


//----------------------------------------------
void SharedMemoryBase::initpage(shareMemoryPage& rPage)
{
	UINT32 size = 0;
	size = m_countperPage * m_rawblockSize;
	char* pdata = rPage.m_pdata;
	memset(pdata, 0, size);

	//���÷�������
	for (INT32 i = 0; i != m_countperPage; ++i)
	{
		*(pdata + (m_rawblockSize)*i) = BLOCK_CHECK_CODE;
	}

	//m_pageCount�Ѿ�������ǰҳ�룬����Ҫ��1
	UINT32 startindex = m_countperPage*(m_pageCount - 1);
	for (UINT32 i = startindex; i < m_count; ++i)
	{
		_SMBlock* ptem = &(rPage.m_pBlock[i - startindex]);
		new(ptem)(_SMBlock);
		ptem->m_dwIndex = i;
		m_mapSMBlock.insert(std::make_pair(i, ptem));
		m_mapFreeSMBlock.insert(std::make_pair(i, ptem));
	}
}



void SharedMemoryBase::InitToMap()
{
	if (!isempty)
	{
		for (UINT32 i = 0; i < m_count; ++i)
		{
			_SMBlock* ptem = getSMBbyRawIndex(i);
			ShareObject* ptemdata = getObjectByRawindex(i);
			if (ptem->m_bUse && (ptemdata->m_State == SMS_USE || ptemdata->m_State == SMS_LOCK))
			{
				m_mapUsedSMBlock.insert(std::make_pair(ptemdata, ptem));
			}
			else
			{
				m_mapFreeSMBlock.insert(std::make_pair(i, ptem));
			}
			m_mapSMBlock.insert(std::make_pair(i, ptem));
		}
	}

	else
	{
		if (m_ShareMemoryPageMapping.size() < 1)
		{
			m_count = 0;
			return;
		}

		shareMemoryPage &page = m_ShareMemoryPageMapping[0];

		initpage(page);
	}
}

BOOL SharedMemoryBase::isFirstCreated()
{
	return isempty;
}

void SharedMemoryBase::importOtherPage()
{
	while (1)
	{
		std::string pagename = std::string(m_modulename) + CommonConvert::IntToString(m_pageCount);
	
		shareMemoryPage page;
		page.m_shm = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, pagename.c_str());
		if(page.m_shm == NULL)
		{
			break;
		}
		page.m_pdata = (CHAR*)MapViewOfFile(page.m_shm,FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
		if(page.m_pdata == NULL)
		{
			break;
		}
			///���ԭ�����Ѿ��������ʼ������
			///�ҵ�ͷ���ݿ��ͷ
		page.m_pBlock = (_SMBlock*)(page.m_pdata + m_rawblockSize*m_countperPage);
		m_ShareMemoryPageMapping.push_back(page);
		m_count += m_countperPage;
		m_pageCount++;
	}
}


//���ڴ��й������⹲���ڴ�ṹ
SharedMemoryBase::SharedMemoryBase(UINT32 rawblockSize, char* pdata, INT32 len)
{
	m_pageCount = 1;
	m_count = len / (rawblockSize + sizeof(_SMBlock));

	m_countperPage = m_count;

	m_rawblockSize = rawblockSize;

	shareMemoryPage firstpage;

	firstpage.m_pdata = pdata;

	firstpage.m_pBlock = (_SMBlock*)(firstpage.m_pdata + (m_rawblockSize)*m_count);

	firstpage.m_shm = NULL;

	m_ShareMemoryPageMapping.push_back(firstpage);

	isempty = false;
}


/**@param name �����ڴ����֣�����ͨ�������һ�,��ʱ�����ֻ���ڴ����Ѿ��еġ�
*@param count  T�ĸ���
*@param noCreate ��������
*/
SharedMemoryBase::SharedMemoryBase(const std::string& name, unsigned int rawblockSize, unsigned int count, BOOL noCreate/*=false*/)
:m_rawblockSize(rawblockSize), m_count(count), m_space(rawblockSize + sizeof(_SMBlock)), m_modulename(name)

{
	m_countperPage = m_count;
	unsigned int size = m_countperPage*(m_space);
	m_pageCount = 1;
	std::string pagename = std::string(m_modulename) + CommonConvert::IntToString(0);
	
	shareMemoryPage firstpage;
	firstpage.m_shm =OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, pagename.c_str());
	if(firstpage.m_shm != NULL)
	{
		firstpage.m_pdata = (CHAR*)MapViewOfFile(firstpage.m_shm,FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
		if(firstpage.m_pdata != NULL)
		{
			firstpage.m_pBlock = (_SMBlock*)(firstpage.m_pdata + m_rawblockSize*m_countperPage);
			m_ShareMemoryPageMapping.push_back(firstpage);
			importOtherPage();
			isempty = false;
		}
		else
		{
			UINT32 dwError = GetLastError();
			printf("---error---:%d", dwError);
		}
	}
	else
	{
		if (!noCreate)
		{
			shareMemoryPage firstpage;

			firstpage.m_shm = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE, 0, size, pagename.c_str());
			firstpage.m_pdata = (CHAR*)MapViewOfFile(firstpage.m_shm, FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);


			///�ҵ�ͷ���ݿ��ͷ
			//#ifdef SHARED_BLOCK_CHECK
			//			firstpage.m_pBlock=(_SMBlock*)(firstpage.m_pdata+(m_rawblockSize+BLOCK_FLAG_SIZE)*m_countperPage);
			//#else
			firstpage.m_pBlock = (_SMBlock*)(firstpage.m_pdata + m_rawblockSize*m_countperPage);
			//#endif
			///��������ڴ�;
			memset(firstpage.m_pdata, 0, size);
			initpage(firstpage);
			m_ShareMemoryPageMapping.push_back(firstpage);
		}

		isempty = true;
	}
}

SharedMemoryBase::~SharedMemoryBase()
{
	for (unsigned int r = 0; r < (unsigned int)m_ShareMemoryPageMapping.size(); r++)
	{
		SafeDelete(m_ShareMemoryPageMapping[r].m_shm);
	}
}

const unsigned int SharedMemoryBase::getCount() const
{
	return m_count;
}

unsigned int SharedMemoryBase::getFreeCount() const
{
	return m_mapFreeSMBlock.size();
}

unsigned int SharedMemoryBase::getUseCount() const
{
	return m_mapUsedSMBlock.size();
}

_SMBlock * SharedMemoryBase::getSMBbyRawIndex(int index)
{
	if (index < (int)m_count)
	{
		unsigned int whichpage = index / m_countperPage;
		unsigned int pageindex = index%m_countperPage;
		shareMemoryPage &page = m_ShareMemoryPageMapping[whichpage];
		return &(page.m_pBlock[pageindex]);
	}
	return NULL;
}

ShareObject* SharedMemoryBase::getObjectByRawindex(unsigned int index)
{
	if (index < m_count)
	{
		unsigned int whichpage = index / m_countperPage;
		unsigned int pageindex = index%m_countperPage;
		shareMemoryPage &page = m_ShareMemoryPageMapping[whichpage];
		return reinterpret_cast<ShareObject*>(page.m_pdata + m_rawblockSize*pageindex);
	}
	else
	{
// 		std::string str = "index<count,get sharObject error,index:" + Helper::CommonConvert(index) + "count:" + Helper::CommonConvert(m_count);
// 		str += "shareName:" + m_modulename;
// 		xLogMessager::getSingleton().logMessage(str, Log_ErrorLevel);///add by dsq
	}
	return NULL;
}


const unsigned int SharedMemoryBase::getRawMemoryBlockSize()
{
	importOtherPage();
	return m_count;
}

void SharedMemoryBase::processCleanDirtyData()
{
	mapUsedSMBlock::iterator it = m_mapUsedSMBlock.begin();
	while (it != m_mapUsedSMBlock.end())
	{
		_SMBlock* pBlock = it->second;
		ShareObject* pobject = static_cast<ShareObject*>(it->first);
		if (!pobject->isUse())
		{
			pobject->reset();
			_SMBlock* pblock = it->second;
			pblock->m_bUse = false;
			m_mapFreeSMBlock.insert(std::make_pair(pblock->m_dwIndex, pblock));
			it = m_mapUsedSMBlock.erase(it);
		}
		else
		{
			it++;
		}
	}
}

ShareObject* SharedMemoryBase::newOjbect(BOOL isNewBlock/*=false*/)
{
	///���δ�����ڴ�û����,��ʼ����������
	if (m_mapFreeSMBlock.size() == 0)
	{
		processCleanDirtyData();
	}
	if (m_mapFreeSMBlock.size() == 0)
	{
		if (newPage())
		{
			return newOjbect(isNewBlock);
		}
		else
		{
			return NULL;
		}
	}
	///��δ����Ķ������ó������뵽�ѷ�����е���
	mapFreeSMBlock::iterator it = m_mapFreeSMBlock.begin();
	while (it != m_mapFreeSMBlock.end())
	{
		_SMBlock* pBlock = it->second;;
		ShareObject* pobject = getObjectByRawindex(pBlock->m_dwIndex);
		if (pobject == NULL)///add by dsq
		{
			++it;
			continue;
		}
		///�ж��Ƿ���ɾ��״̬
		if (!pobject->isDestroy())
		{
			m_mapUsedSMBlock.insert(std::make_pair(pobject, pBlock));
			m_mapFreeSMBlock.erase(it);
			pBlock->m_bUse = true;
			pBlock->m_bNewBlock = isNewBlock;

			pobject->useit();

			return pobject;
		}
		it++;
	}

	////����Ϊ�¼ӵĴ��� zm
	processCleanDirtyData();

	if (newPage())
	{
		return newOjbect(isNewBlock);
	}

	return NULL;
}

BOOL SharedMemoryBase::destoryObject(ShareObject* pobject)
{
	if (pobject == NULL)
	{
		return false;
	}
	pobject->reset();
	mapUsedSMBlock::iterator it = m_mapUsedSMBlock.find(pobject);
	if (it == m_mapUsedSMBlock.end())
	{
		return false;
	}
	_SMBlock* pblock = it->second;
	pblock->m_bUse = false;
	m_mapUsedSMBlock.erase(it);
	m_mapFreeSMBlock.insert(std::make_pair(pblock->m_dwIndex, pblock));
	return true;

}

