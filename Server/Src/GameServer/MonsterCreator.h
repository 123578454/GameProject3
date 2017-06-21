#ifndef _GAME_MONSTER_CREATOR_H_
#define _GAME_MONSTER_CREATOR_H_













class monsterCreator
{
public:
	monsterCreator(xBattleScene* scene);
	~monsterCreator() {};

	//��������Ϣ
	bool parserWaveData(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize, uint32 hardlevel, bool bLoadItemDrop = true);

	//��������
	bool parserTrapMachine(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize, uint32 hardlevel);

	//����·��
	bool parserObstale(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize);

	///������ʯ;
	bool parseOreData(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize);

	///�������䣺
	bool parseTreasure(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize);

	///�����ѷ���λ
	bool parseFriendData(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize);

	///����������������
	bool parseMonsterLimit(rapidxml::xml_node<>* xmlFTGroup);


	//����������
	bool parserLoverData(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize, uint32 hardlevel, uint32& loverID );

	//�Ƿ����в���������
	bool getisWaveFinish()const;

	/// �Ƿ�Ϊ���һ��
	bool isLasterWave();

	///��õ�һ����С��ID
	uint32 getFirstBigWaveSmallWaveID();

	//����ƶ�
	void onPlayerMove(uint32 roleID, const Point2d& pos);

	//����֮���Ƿ񾭹�������
	void onPlayerMovePoint(uint32 roleID, const Point2d& stPos, const Point2d& enPos);

	//����һ��
	void onActiveCurrentWave(SM_BigWave& bigwave, uint32 roleID);

	//��������
	void onMonsterDie(uint32 id);

	//����
	int32 onPushMonsters(uint32 roleID, uint32 monsterID, uint32 count, uint32 type, uint32 camps, SM_WaveInfo* waveinfo);

	/// ���������
	bool onCalcRandPos();

	//��õ�ǰ������,���ܴ���һ��������NС��ͬʱ����
	bool getCurrentWaveData(SM_BigWave& bigwave, vecpsmallwave& data);

	//��ɵ�ǰ��
	void finishcurrentwave(SM_BigWave& bigwave);

	void update();

	bool isBossAllDie();

	void onPushFixMonsters();

	void onPushFixMonstersEx(bool bFlag);  //��ʿ��bossʹ�� bFlag Ϊtrueʱˢ��
	//��������ǰ�ȴ�����ʱ���Ĳ�
	void onActiveWaitWaveNow();

	const int32 getFirstSmallWaveID() const;

	///����֮��ר��,��õ�һ�󲨵ĵ�ǰС����
	uint32 getCurrentFloor();

	//ɱ������,����id,����typeid
	bool killObstacle(uint32 id, SM_ObstaleData& data);

	///��ʯ�ɼ����;����id,����typeid
	bool killOre(LONG64 id, SM_OreData& data);

	///�õ���ʯ����;
	void getOreMap(map_OreData& oreMap);

	///�õ���������;
	void getAllTreasure(map_TreasureData& treasureMap);

	///�ͻ��˼���һ�󲨹���
	void activeBigWave(int32 bigwaveNum, int32 roleid);

	///��ȡ����������Ϣ
	void getVecSamllSave(vecsmallwave& monsterInfo);

	//boss���ٶ������� ��ǿ�ʼˢ��
	void onBossComing(int32 bigwaveid, int32 smallwaveid);


	// ��ȡ��ǰ����ĵ���
	void getDropItemFromMonster(vector<ObjCountDrop>& droplist, uint32 monsterID);

	/// ȫ������
	void getAllDropItemFromMonster(vector<ObjCountDrop>& droplist);

	/// ��ȡ�������Ʒ
	void getDropItems(vector<ObjCountDrop>& droplist, uint32 monsterSceneID, bool bFlag = false);
	/// ��ȡ���ж�����Ʒ
	void getDecideDropItems(vector<ObjCountDrop>& droplist, bool bFlag = false, int32 nWave = -1);

	/// �ƶ��Ѿ��������Ʒ
	void removeDropItems(vector<ObjCountDrop>& droplist);

	// �жϹ����Ƿ����
	bool isKillAllMonster();

	// ��ǰ�����м�ֻ��
	int32 getMonsterCount();

	// ����������Ϣ
	void getMonsterList(GM_Monster_list& pMonsterList);

	// ��ǰ������ɱ���˼�ֻ��
	int32 getKillMonsterCount();

	// ɨ������
	void sweepFBDropItem(int32 nProfession, vector<ObjCountDrop>& vecDropItems);

	SM_BossData* getBossData() { return &m_bossdata; }

	//�Ƿ�Ϊ�ѷ���λ
	bool IsFriendId(int32 id);

	/// ��Ȩ�ؼ���
	int32 calcTotalWeight();
	/// ����ĳһ����Ȩ��
	void calcWaveTotalWeight();
	int32 getWaveTotalWeight(int32 nWave);
	int32 getCurWaveWeight(int32 nWave);
	void setWaveTotalWeight(int32 nWave, int32 nMonsterWeight, bool bFlag = true);

	/// ��ǰ�������Ľ�Ǯ
	void monsterDropCoin(const int32& nMonsterID, int32& nGold, int32& nDiamond);

	/// ���������İٷֱȵ���
	void poleyMonsterDrop(const int32& nMonsterID, int32& nDiamond);

	/// ��ǰ��������
	void monsterDropStack(const int32& nMonsterID, int32& nStackGold, int32& nStackDiamond);

	/// ��������
	mapdibm::iterator getMapdropItemsBindMonsterBegin() { return m_mapdropItemsBindMonster.begin(); }
	mapdibm::iterator getMapdropItemsBindMonsterEnd() { return m_mapdropItemsBindMonster.end(); }
	//�õ��Ѿ���λ�������ֵ��ID
	int32 GetMaxIndexFriendId();
	//�õ��Ѿ���λ��loverid index��100-200֮��
	int32 getFriendLoverId();

	///֪ͨ�������wave��Ϣ,bool ���Ƿ��ǿ�ʼ
	void notifyWave(bool isstart, const SM_BigWave& bigwave, bool isbigwavestartorend);

	void notifywaveStart(xBattleObject* obj, void* pData);

	void notifywaveEnd(xBattleObject* obj, void* pData);

	///��������
	void PushMonster(SM_WaveInfo* waveinfo);
	//ˢ�»���
	void PushTrapMachine();
	//�Ƿ��й������
	bool HaveMonsterAlive(SM_WaveInfo* info);
	//ʣ����Ҫˢ�¹�������
	uint32 GetMonsterLeftCount(SM_WaveInfo* info);
	void insertoneWave(const SM_WaveInfo& data);
	bool isspecialbigwave(const SM_BigWave& bigwave);
	bool CheckAllFinished(const SM_BigWave& bigwave);

	//��ȡ�󲨵���Ϣ
	SM_BigWave* getbigWave(uint32 bigwaveid);
	//��ȡС���Ƿ�Ϊboss��
	bool isBossSmallWave(int32 bigwaveid, int32 smallwaveid);
	//��ȡС���е��ӳ���Ϣ
	float GetSmallWaveDelayTime(int32 bigwaveid, int32 smallwaveid);
	//����С����boss�Ƿ�ˢ��
	void setBossStartRefresh(int32 bigwaveid, int32 smallwaveid);

	/// ͨ�������ID��ȡ����Ĳ���
	int32 getMonsterWaveFromMonsterSceneID(const uint32& monstersceneid);

	// �ѳ����е������Ʒ������
	void sceneDropItemBind(FirstDrop nFlag = FirstDrop::FD_FB_STATUS);

	// �ѳ����е������Ʒ������
	void sceneDropItemBind(int32 professional, int32 nFlag = FirstDrop::FD_FB_STATUS);
	// ��¼��ǰ��������������Ĺ�
	void recordMonster();
	void recordMonster_fix();

	// ��������¼
	void recordMonsterNum(int32 nMonsterID, int32 nCount, int32 nMonsterWave, int32& nFluence);

	// ���ݹ������Ͷ�����Ʒ�ĵ������
	void calcItemsDropRateFromMonsterType(vec_scene& vecSceneDrop);
	// ��ȡ������Ʒ��Ӧ�Ĺ�������
	bool getDropItemMonsterType(MonsterType& mtData, scene_drop vecSceneDrop, int32 nItemID, int32 nMonsterWave);
	// ������Ʒ�󶨵���������
	void dropItemBindMonster(MonsterType& mtTmp, scene_drop sdData);

	void sortBigWave();

	void sortSmallWave();

	void takesort();

	void onStartBigWave(SM_BigWave& bigwave, uint32 roleID);

	//��¼����С����Ӧ�Ĺ�������
	void RecordChessSmallWaveCount(int32 smallwave, int32 count);

	//�õ����̵Ĺ��������Ͳ�����Ϣ
	monsterWaveCount& GetChessWaveCount();
	///��ȡ��ǰ�󲨼����
	bool getCurrentWaveActiveWaveRect(vecpsmallwave vecpwavs, std::vector<Rect2d>& vecRect);

	bool hasnextwave(SM_BigWave& bigwave);

	//ľͰ�����߼�
	void onPushObstaleMonster(xBattleObject* obj, const SM_ObstaleData& obstaleData);

	void onPushOreMonster(const SM_OreData& oreData);

	LONG64 m_oreGmid;

	//void notifyLoverReward(xBattleObject* obj,void* pData);
	//�õ�ͬ��������������
	uint32 GetMaxMonsterTotalCount() { return m_MaxMonsterTotal; }
	//�õ�ͬ��һ�������������
	uint32 GetMaxMonsterSingleCount(int32 monsterId);
	//����ͬ�����Ĺ�������
	void SetMaxMonsterCount(int32 count);
	//��ӵ���������������
	void AddSingleMonster(int32 monsterid, int32 maxcount);

	/// ������ѡ���ж�
	void selDropGroup(vec_scene& vecscenedrop);

	/// ������ѡ���ж�
	void selDropGroup(vec_scene& vecscenedrop, int32 professional);
	/// ���������Ʒ
	void saveDropGroupItem(const scene_drop& scenedrop);

	/// ���������Ʒ
	void saveDropGroupItem(const scene_drop& scenedrop, int32 professional);

	/// ��¼��������
	void recordMonsterDie(const uint32& monsterSceneID);

	/// �жϵ�ǰ���Ĺ����Ƿ�ȫ������
	int32 getFinishDieWave(const uint32& monsterSceneID);

	/// �жϵ�ǰ���Ĺ����Ƿ�ȫ������
	bool isWaveFinishDie();

	int32 getTotalWeight() { return m_nTotalWeight; }

	/// ����Ȩ��
	void setMonsterWeight(int32 nParam1, bool bAdd = true);
	int32 getMonsterWeight();

	/// �����������⴦��
	bool dropMonsterType(uint32 monsterid);

	/// ���� B================================================================================
	/// �������ݱ��浽��־
	void saveDropTestData(const DropItems& dItem);
	/// ��ӡ
	void printfLogMessage();
	/// ���� E================================================================================

protected:

	mapdibm			m_mapdropItemsBindMonster;	// ��Ҫ�������Ʒ�ŵ���������
	mapDropItems	m_mapdropItemsBindWave;		// ��Ҫ�������Ʒ�󶨵�������
	mapMonsterWave	m_mapMonsterWave;	/// ��ǰ���Ĺ�������״̬

	vecbigwave		m_wavedata;

	vecsmallwave	m_fixwaveinfo;//һ�������еĹ�����Ϣ
	vecsmallwave	m_trapwaveinfo;//������Ϣ

	SM_BossData		m_bossdata;

	xBattleScene*	m_scene;

	vecActiveBigWave m_vecActiveBigWave;

	map_obstaleData m_map_obstaleData;

	map_OreData m_map_oreData;
	map_TreasureData m_map_Treasure;
	xHashMap<int32, int32> m_friendMap;	//�ѷ���λID
	map<int32, int32> m_mapMonsterWeight;	/// �������Ͷ�Ӧ��Ȩ��
	int32 m_nTotalWeight;					/// ��Ȩ��
	mapMonsterWaveWeight m_nWaveWeight;		/// ĳһ����Ȩ��
	int32 m_nMonsterWeight;					/// ����Ȩ��
	uint32 m_MaxMonsterTotal;				//  ͬ�������������
	map<int32, int32> m_MaxMonsterSingle;	 //  ͬ��һ����������
	monsterWaveCount m_MapMonsterWaveCount;	//���ﲨ����Ӧ����������
	map_wave_rate every_wave_rate;			/// ÿһ�����Ĺ������
	map<int32, int32> m_mapItem;
};

#endif