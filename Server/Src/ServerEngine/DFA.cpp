#include "stdafx.h"
#include "DFA.h"
#include "CommonConvert.h"

INT32 DFANode::s_length = 0;
INT32 DFANode::s_maxlen = 0;
std::wstring DFANode::s_str = L"";

//--------------------------------------------------------------------------------------------------------
DFANode::DFANode(keytype key) : _key(key), _start(0)
{

}
//--------------------------------------------------------------------------------------------------------
DFANode::~DFANode()
{
	clearChildNode();
}
//--------------------------------------------------------------------------------------------------------
bool DFANode::addNode(const keytype* key, int len)
{
	if (key == NULL || len < 0)
	{
		return false;
	}

	int i = 0;
	DFANode* ptemNode = this;
	while (i < len)
	{
		ptemNode = ptemNode->addNode(key[i]);
		++i;
	}

	return true;
}
//--------------------------------------------------------------------------------------------------------
DFANode* DFANode::addNode(keytype key)
{
	DFANodeMap::iterator it = _childNodeMap.find(key);

	if (it == _childNodeMap.end())
	{
		DFANode* temnode = new DFANode(key);
		_childNodeMap.insert(std::make_pair(key, temnode));
		_childNodeSet.insert(key);
		return temnode;
	}
	else
	{
		return it->second;
	}
}
//--------------------------------------------------------------------------------------------------------
DFANode* DFANode::getNode(keytype key) const
{
	DFANodeMap::const_iterator  it = _childNodeMap.find(key);
	if (it == _childNodeMap.end())
	{
		return NULL;
	}
	else
	{
		return it->second;
	}

}
//--------------------------------------------------------------------------------------------------------
bool DFANode::hasKeyWord(const keytype* pdata, int len, bool isreturn/* = true*/)
{
	if (isreturn)
	{
		s_length = 0;
		s_maxlen = 0;
	}

	INT32 currentPos = 0;

	while (currentPos < len)
	{
		keytype p = pdata[currentPos];
		DFANode* pnode = getNode(p);
		++s_length;
		if (pnode == NULL)///���û���ҵ���������һ����ʼ��
		{
			if (isreturn == false)
			{ return false; }

			++currentPos;
			s_length = 0;
			continue;
		}
		else if (pnode->hasEnding())
		{
			//��¼���ƥ�䳤��
			s_maxlen = s_length;

			if (isreturn)
			{ _start = currentPos; }

			//���ֻ��һ���ӽڵ����û�нڵ���ֱ�ӷ���
			if (pnode->getChildCount() <= 1)
			{ return true; }
		}

		int startpos = currentPos + 1;
		if (startpos >= len)
		{ return s_maxlen != 0; }

		bool b = pnode->hasKeyWord(pdata + startpos, len - startpos, false);

		if (b == true)
		{
			if (isreturn)
			{ _start = currentPos; }
			return true;
		}
		else
		{
			if (isreturn == false)
			{ return false; }
			else if (s_maxlen != 0)
			{
				s_length = s_maxlen;
				return true;
			}

			++currentPos;
			s_length = 0;
			continue;
		}
	}

	return false;
}
//--------------------------------------------------------------------------------------------------------
bool DFANode::loadFile(const std::string& filename)
{
	clearChildNode();

	FILE* pFile = fopen(filename.c_str(), "r+");

	if (pFile == NULL)
	{
		return false;
	}

	char line[1024] = { 0 };
	std::string tmpline = "";
	do
	{
		fgets(line, 1024, pFile);

		if (strcmp(line, "*") == 0)
		{
			continue;
		}

		//ÿ���ַ����#��Ϊ��β��
		tmpline = line + std::string("#");
		std::wstring wline = CommonConvert::Utf8_To_Unicode(tmpline);
		addNode(wline.c_str(), (int)wline.size());

	}
	while (!feof(pFile));

	fclose(pFile);

	return true;
}
//--------------------------------------------------------------------------------------------------------
bool DFANode::isFileterWord(const std::string& word)
{
	std::wstring wpData;

	wpData = CommonConvert::Utf8_To_Unicode(word);

	//Сдת��д����ƥ��
	std::transform(wpData.begin(), wpData.end(), wpData.begin(), toupper);

	return hasKeyWord(wpData.c_str(), (int)wpData.size());
}

//--------------------------------------------------------------------------------------------------------
bool DFANode::isFileterWord(const std::wstring& word)
{
	//Сдת��д����ƥ��
	std::wstring tmpWord = word;
	std::transform(tmpWord.begin(), tmpWord.end(), tmpWord.begin(), toupper);

	return hasKeyWord(word.c_str(), (int)word.size());
}
//--------------------------------------------------------------------------------------------------------
bool DFANode::filterKeyWords(std::string& word, const std::wstring& dest /*= L"***"*/)
{
	if (word.empty())
	{
		return false;
	}

	//��������дʣ����滻��"***"
	std::wstring wpData;
	///�ж��ַ��Ƿ�Ϊutf8

	wpData = CommonConvert::Utf8_To_Unicode(word);

	//Сдת��д����ƥ��
	std::transform(wpData.begin(), wpData.end(), wpData.begin(), toupper);

	//������¼�ַ����Ĳ�ѯλ��
	int start = 0, strlen = 0;
	int lastPos = -1;	//�ϴ��滻��λ��
	bool keyflag = false;

	//�滻�ɵĿ��ַ��ͳ���
	while (hasKeyWord(wpData.c_str(), (int)wpData.size()))
	{
		//��ȡ���д����ַ����е�λ�úͳ��ȣ��ж��Ƿ����������дʣ���������滻Ϊ��
		getKeyPos(start, strlen);
		if (lastPos == start)
		{
			wpData.replace(wpData.begin() + start, wpData.begin() + start + strlen, L"");
			lastPos = start;
		}
		else
		{
			wpData.replace(wpData.begin() + start, wpData.begin() + start + strlen, dest.c_str());
			lastPos = start + (int)dest.size();
		}
		//��ʾ�������д�
		keyflag = true;
	}

	//��������дʣ�����Ҫʹ���滻����ַ���
	if (keyflag)
	{ word = CommonConvert::Unicode_To_Uft8(wpData); }

	return true;
}
//--------------------------------------------------------------------------------------------------------
void DFANode::getKeyPos(INT32& nStart, INT32& nLen)
{
	nStart = _start;
	nLen = s_length;

	_start = 0;
	s_length = 0;
}
//--------------------------------------------------------------------------------------------------------
void DFANode::clearChildNode()
{
	DFANodeMap::iterator  itend = _childNodeMap.end();
	for (DFANodeMap::iterator  it = _childNodeMap.begin(); it != itend; ++it)
	{
		delete (it->second);
	}

	_childNodeMap.clear();
	_childNodeSet.clear();
}
//--------------------------------------------------------------------------------------------------------
bool DFANode::hasEnding()
{
	return _childNodeSet.empty() ? true : _childNodeSet.find(L'#') != _childNodeSet.end();
}

