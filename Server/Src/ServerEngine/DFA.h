#ifndef __D_F_A_H__
#define __D_F_A_H__

class DFANode;
typedef wchar_t keytype;
typedef std::map<keytype, DFANode*> DFANodeMap;
typedef std::set<keytype> DFANodeSet;

class DFANode
{
public:
	DFANode(keytype key);

	~DFANode();

	//�����˸��ӽڵ�
	DFANode* addNode(keytype key);
	bool addNode(const keytype* key, int len);

	//��ȡ���� key���ӽڵ�
	DFANode* getNode(keytype key) const;

	/**�ж��Ƿ����ָ����key*/
	bool hasKeyWord(const keytype* pdata, int len, bool isreturn = true);

	//���عؼ����ļ�
	bool loadFile(const std::string& filename);

	//�ж��Ƿ��йؼ��� �ǵĻ�true
	bool isFileterWord(const std::string& word);
	bool isFileterWord(const std::wstring& word);

	//���ַ����е��������дʶ��滻��"***"
	bool filterKeyWords(std::string& word, const std::wstring& dest = L"***");

private:
	void getKeyPos(INT32& nStart, INT32& nLen);

	//��սڵ�����
	void clearChildNode();

	//�ж��ӽڵ��Ƿ�����β��
	bool hasEnding();

	//��ȡ�ӽڵ����
	inline INT32 getChildCount() const { return (INT32)_childNodeSet.size(); }
protected:
	keytype	_key;
	INT32 _start;
	DFANodeMap _childNodeMap;		//�����ӽڵ�
	DFANodeSet _childNodeSet;		//�����ӽڵ��ַ�����

	static INT32 s_length;		//��¼ƥ�䵽�ĳ���
	static INT32 s_maxlen;		//��¼���ƥ�䳤��
	static std::wstring s_str;	//��̬�������滻���ַ���
};

#endif