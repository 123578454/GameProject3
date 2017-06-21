#include "pch.h"
#include "monsterCreator.h"
#include "xBattleScene.h"
#include "xBattleObject.h"
#include "monsterBase.h"
#include "protoFiles/battle.txt.pb.h"
#include "xmath.h"
#include "FBLogic.h"
#include "protoFiles/InstanceReward.txt.pb.h"
#include "InstanceServerApp.h"
#include "FBPlayer.h"

#define NEXTCONTINUE()\
	xmlmonster=xmlmonster->next_sibling();\
	continue;
#define NEXTSUBCONTINUE()\
	xmlsubmonster=xmlsubmonster->next_sibling();\
	continue;
#include "FBRuneDrop.h"

bool monsterCreator::parserLoverData( rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize, uint32 hardlevel, uint32& loverID )
{
	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	if(xmlmonster != NULL)
	{
		Point2d loverposition;
		xstring tmpstr;
		uint32 lovetypeid;
		if(xmlHelper::parseChar(tmpstr, xmlmonster, "towerID"))
		{
			lovetypeid = Helper::StringToInt(tmpstr);
		}
		Point2d ftposition;
		if(xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", ftposition))
		{
			loverposition = xmlHelper::getServerPosition(ftposition, bondcenter, bondsize);
			xBattleObject* obj = m_scene->createMonster(lovetypeid, loverposition, CT_Monster, 0);
			if(obj != NULL)
			{
				loverID = obj->getid();
			}
			return true;
		}
	}
	return false;
}


bool monsterCreator::parserObstale( rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize )
{
	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	while(1)
	{
		if(xmlmonster != NULL)
		{
			SM_ObstaleData msend;
			Point2d ftposition;
			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", ftposition))
			{
				NEXTCONTINUE()
			}
			msend.pos = xmlHelper::getServerPosition(ftposition, bondcenter, bondsize);

			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "box_pos", msend.anchorPos))
			{
				NEXTCONTINUE()
			}
			msend.box_pos = xmlHelper::getServerPosition(msend.anchorPos, bondcenter, bondsize);

			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "box_scale", msend.boxsize))
			{
				//NEXTCONTINUE()
			}

			rapidxml::xml_node<>* xmlsubmonster = xmlmonster->first_node("EditorMonster");
			while(1)
			{
				if(xmlsubmonster != NULL)
				{
					SM_ObstaleMonster submonster;
					xstring str;
					if(!xmlHelper::parseChar(str, xmlsubmonster, "monsterId"))
					{
						NEXTSUBCONTINUE()
					}
					submonster.monsterid = Helper::StringToInt(str);
					if(!xmlHelper::parseChar(str, xmlsubmonster, "monsterNum"))
					{
						NEXTSUBCONTINUE()
					}
					submonster.monstercount = Helper::StringToInt(str);
					msend.obstaleMonsters.push_back(submonster);
				}
				else { break; }
				xmlsubmonster = xmlsubmonster->next_sibling();
			}

			xstring str;
			if(!xmlHelper::parseChar(str, xmlmonster, "obstacleId"))
			{
				NEXTCONTINUE()
			}
			msend.id = Helper::StringToInt(str);
			if(!xmlHelper::parseChar(str, xmlmonster, "typeId"))
			{
				NEXTCONTINUE()
			}
			msend.typeID = Helper::StringToInt(str);
			if(!xmlHelper::parseChar(str, xmlmonster, "possible"))
			{
				NEXTCONTINUE()
			}

			msend.monsterrate = Helper::StringToInt(str);
			m_map_obstaleData[msend.id] = msend;
		}
		else { break; }
		xmlmonster = xmlmonster->next_sibling();
	}
	return true;
}


///������ʯ;
bool monsterCreator::parseOreData(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize)
{
	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	while(1)
	{
		if(xmlmonster != NULL)
		{
			SM_OreData msend;
			Point2d orePostion;
			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", orePostion))
			{
				NEXTCONTINUE();
			}
			msend.pos = xmlHelper::getServerPosition(orePostion, bondcenter, bondsize);

			xstring str;
// 				if(!xmlHelper::parseChar(str,xmlmonster,"obstacleId"))
// 				{
// 					NEXTCONTINUE();
// 				}
// 				msend.id = Helper::StringToInt(str);

			msend.id = m_oreGmid++;

// 				if(!xmlHelper::parseChar(str,xmlmonster,"typeId"))
// 				{
// 					NEXTCONTINUE();
// 				}
			msend.typeID = Helper::StringToInt(str);
			m_map_oreData.insert(std::make_pair(msend.id, msend));
		}
		else
		{ break; }

		xmlmonster = xmlmonster->next_sibling();
	}
	return true;
}


///�������䣺
bool monsterCreator::parseTreasure(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize)
{
	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	while(1)
	{
		if(xmlmonster != NULL)
		{
			SM_WaveInfo newWaveInfo;
			newWaveInfo.roleID = 0;

			Point2d boxPostion;
			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", boxPostion))
			{
				NEXTCONTINUE();
			}
			newWaveInfo.startpos = xmlHelper::getServerPosition(boxPostion, bondcenter, bondsize);

			xstring str;
			if(!xmlHelper::parseChar(str, xmlmonster, "boxMonsterIds_"))
			{
				NEXTCONTINUE();
			}
			SM_monsterPare msend;
			msend.monsterID = Helper::StringToInt(str);
			msend.monsterCount = 1;
			msend.type = CT_NoobstacleMonster;

			newWaveInfo.monsters.push_back(msend);
			insertoneWave(newWaveInfo);
		}
		else
		{ break; }

		xmlmonster = xmlmonster->next_sibling();
	}
	return true;
}

bool monsterCreator::parseFriendData(rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize)
{

	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	vector<SM_WaveInfo> vecwavelist;
	while (1)
	{
		if (xmlmonster != NULL)
		{
			SM_WaveInfo newwaveinfo;
			newwaveinfo.roleID = 0;
			xstring tmpstr;
			auto tmpMonsterId = 0;
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "friendid"))
			{
				std::vector<xstring> tmpvecstr;
				Helper::splitString(tmpstr, " ", tmpvecstr);
				vector<int32> idvector;
				for (uint32 r = 0; r < tmpvecstr.size(); r++)
				{
					idvector.push_back(Helper::StringToInt(tmpvecstr[r]));
				}
				SM_monsterPare monsterdata;
				monsterdata.monsterID = idvector[Helper::getRandomInField(0, idvector.size() - 1)];
				monsterdata.monsterCount = 1;
				monsterdata.type = CT_Monster;
				newwaveinfo.monsters.push_back(monsterdata);
				Point2d ftposition;
				if (!xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", ftposition))
				{
					xLogMessager::getSingleton().logMessage("failed to parse monster position!", Log_ErrorLevel);
					NEXTCONTINUE()
				}
				newwaveinfo.startpos = xmlHelper::getServerPosition(ftposition, bondcenter, bondsize);
				tmpMonsterId = monsterdata.monsterID;
			}

			if (!xmlHelper::parseAngle(xmlmonster, "rotation", newwaveinfo.roY))
			{
				xLogMessager::getSingleton().logMessage("failed to parse rotation!", Log_ErrorLevel);
				NEXTCONTINUE()
			}

			if (xmlHelper::parseChar(tmpstr, xmlmonster, "buffid") == true)
			{
				newwaveinfo.buffid = Helper::StringToInt(tmpstr);
			}
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "buffTime") == true)
			{
				newwaveinfo.bufftime = Helper::StringToInt(tmpstr);
			}
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "index") == true)
			{
				newwaveinfo.index = Helper::StringToInt(tmpstr);
			}
			if (tmpMonsterId != 0)
			{
				m_friendMap[tmpMonsterId] = newwaveinfo.index;
			}

			//if (xmlHelper::parseChar(tmpstr, xmlmonster, "id") == false)
			//{
			//	NEXTCONTINUE();
			//}
			//newwaveinfo.id = Helper::StringToInt(tmpstr);

			//insertoneWave(newwaveinfo);
			vecwavelist.push_back(newwaveinfo);
		}
		else { break; }
		xmlmonster = xmlmonster->next_sibling();
	}
	//if (m_wavedata.size() > 0)
	//{
	//	takesort();
	//}
	sort(vecwavelist.begin(), vecwavelist.end(), waveindexcmp);
	for (auto i = 0; i < vecwavelist.size(); ++i)
	{
		insertoneWave(vecwavelist[i]);
	}
	return true;
}

bool monsterCreator::parseMonsterLimit(rapidxml::xml_node<>* xmlFTGroup)
{

	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	while (1)
	{
		if (xmlmonster != NULL)
		{
			xstring tmpstr;
			auto tmpMonsterId = 0;
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "typeid"))
			{
				tmpMonsterId = Helper::StringToInt(tmpstr);
			}
			auto count = 0;
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "count") == true)
			{
				count = Helper::StringToInt(tmpstr);
			}

			AddSingleMonster(tmpMonsterId, count);

		}
		else { break; }
		xmlmonster = xmlmonster->next_sibling();
	}

	return true;
}

///�õ���������;
void monsterCreator::getAllTreasure(map_TreasureData& treasureMap)
{
	map_TreasureData::iterator beg =  m_map_Treasure.begin();
	map_TreasureData::iterator end =  m_map_Treasure.end();
	for(; beg != end; beg++)
	{
		treasureMap.insert(std::make_pair(beg->second.boxId, beg->second));
	}
}

///�õ���ʯ����;
void monsterCreator::getOreMap(map_OreData& oreMap)
{
	map_OreData::iterator beg = m_map_oreData.begin();
	map_OreData::iterator end = m_map_oreData.end();
	for (; beg != end; beg++)
	{
		oreMap.insert(std::make_pair(beg->second.id, beg->second));
	}
}


///��ʯ�ɼ����;
bool monsterCreator::killOre(LONG64 id, SM_OreData& data)
{
	map_OreData::iterator it = m_map_oreData.find(id);	///���Id;
	if(it != m_map_oreData.end())
	{
		data = it->second;
		m_map_oreData.erase(it);
		return true;
	}
	return false;
}

///??????;
void monsterCreator::onPushOreMonster(const SM_OreData& oreData)
{
	int32 rnd = rand() % FIGHT_RANDMAX;
	if((uint32)rnd <= oreData.monsterrate)
	{
// 		float stposx = oreData.pos.x - oreData.anchorPos.x * oreData.boxsize.x;
// 		float stposy = oreData.pos.y - oreData.anchorPos.y * oreData.boxsize.y;
//
// 		for(uint32 r=0;r < oreData.obstaleMonsters.size();r++)
// 		{
// 			for(uint32 rr = 0;rr < oreData.obstaleMonsters[r].monstercount;rr++)
// 			{
// 				int32 realposx = int32(stposx) + rand() % (int32(oreData.boxsize.x));
// 				int32 realposy = int32(stposy) + rand() % (int32(oreData.boxsize.y));
// 				xBattleObject *obj = m_scene->createMonster(oreData.obstaleMonsters[r].monsterid,Point2d(realposx,realposy),CT_Monster,1);
// 				if(obj!=NULL)
// 				{
// 					m_scene->processBroadCastMonsterIn(obj);
// 				}
// 			}
// 		}
	}
}

uint32 monsterCreator::GetMaxMonsterSingleCount(int32 monsterId)
{
	auto it = m_MaxMonsterSingle.find(monsterId);
	if (it != m_MaxMonsterSingle.end())
	{
		return it->second;
	}
	return 0;
}

void monsterCreator::SetMaxMonsterCount(int32 count)
{
	m_MaxMonsterTotal = count;
}

void monsterCreator::AddSingleMonster(int32 monsterid, int32 maxcount)
{
	m_MaxMonsterSingle[monsterid] = maxcount;
}

bool monsterCreator::parserTrapMachine( rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize, uint32 hardlevel )
{
	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	while(1)
	{
		if(xmlmonster != NULL)
		{
			SM_WaveInfo newwaveinfo;
			newwaveinfo.roleID = 0;
			xstring tmpstr;
			Point2d ftposition;
			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", ftposition))
			{
				NEXTCONTINUE()
			}
			newwaveinfo.startpos = xmlHelper::getServerPosition(ftposition, bondcenter, bondsize);
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "m_delytime") == true)
			{
				newwaveinfo.cycleTime = Helper::StringTofloat(tmpstr);
			}
			int32 nTmp = 0;
			if(xmlHelper::parseInt(nTmp, xmlmonster, "m_showChance"))
			{
				if (rand() % 100 <= nTmp)
				{
					if(xmlHelper::parseChar(tmpstr, xmlmonster, "m_showCollect"))
					{
						std::vector<xstring> tmpvecstr;
						Helper::splitString(tmpstr, " ", tmpvecstr);
						uint32 nCount = tmpvecstr.size();
						if (nCount == 0)
						{
							NEXTCONTINUE()
						}
						else
						{
							SM_monsterPare monsterdata;
							monsterdata.monsterID = Helper::StringToInt(tmpvecstr[rand() % nCount]);
							monsterdata.monsterCount = 1;
							monsterdata.type = CT_Machine;
							newwaveinfo.monsters.push_back(monsterdata);

							/*if(xmlHelper::parseChar(tmpstr,xmlmonster,"m_machinePos"))
							{
								Helper::splitString(tmpstr," ",tmpvecstr);
								uint32 nCount = tmpvecstr.size();
								if (nCount == 0)
								{
									NEXTCONTINUE()
								}
								else
								{
									Helper::string_replace(tmpvecstr[rand()%nCount],"("," ");
									Helper::string_replace(tmpvecstr[rand()%nCount],")"," ");
									std::vector<xstring> stringvec;
									Helper::splitString(tmpvecstr[rand()%nCount],",",stringvec);
									if(stringvec.size()==3)
									{
										ftposition.x=Helper::StringTofloat(stringvec[0]);
										ftposition.y=Helper::StringTofloat(stringvec[2]);
										newwaveinfo.startpos=xmlHelper::getServerPosition(ftposition,bondcenter,bondsize);
									}
									else
									{
										NEXTCONTINUE()
									}
								}
							}*/
						}
					}
					else
					{
						NEXTCONTINUE()
					}
				}
			}
			else
			{
				if(xmlHelper::parseChar(tmpstr, xmlmonster, "typeId"))
				{
					SM_monsterPare monsterdata;
					monsterdata.monsterID = Helper::StringToInt(tmpstr);
					monsterdata.monsterCount = 1;
					monsterdata.type = CT_Machine;
					newwaveinfo.monsters.push_back(monsterdata);
				}
				else
				{
					NEXTCONTINUE()
				}
			}

			m_trapwaveinfo.push_back(newwaveinfo);
			//insertoneWave(newwaveinfo);
		}
		else { break; }
		xmlmonster = xmlmonster->next_sibling();
	}
	return true;
}


bool monsterCreator::parserWaveData( rapidxml::xml_node<>* xmlFTGroup, const Point2d& bondcenter, const Point2d& bondsize, uint32 hardlevel, bool bLoadItemDrop /*= true*/ )
{
	xstring strMapFile = "";
	if (NULL != m_scene)
	{
		strMapFile = m_scene->getMapFile();
	}

	rapidxml::xml_node<>* xmlmonster = xmlFTGroup->first_node();
	while(1)
	{
		if(xmlmonster != NULL)
		{
			SM_WaveInfo newwaveinfo;
			newwaveinfo.roleID = 0;
			xstring tmpstr;
			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxMonsterIds_"))
			{
				std::vector<xstring> tmpvecstr;
				Helper::splitString(tmpstr, " ", tmpvecstr);
				for(uint32 r = 0; r < tmpvecstr.size(); r++)
				{
					SM_monsterPare monsterdata;
					monsterdata.monsterID = Helper::StringToInt(tmpvecstr[r]);
					if (monsterdata.monsterID != 0)
					{
						monsterdata.monsterCount = 1;
						monsterdata.type = CT_Monster;
						newwaveinfo.monsters.push_back(monsterdata);
					}
				}
				Point2d ftposition;
				if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", ftposition))
				{
					xLogMessager::getSingleton().logMessage("failed to parse monster position! scene_file:" + strMapFile, Log_ErrorLevel);
					NEXTCONTINUE()
				}
				newwaveinfo.startpos = xmlHelper::getServerPosition(ftposition, bondcenter, bondsize);
			}
			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxMonsterCounts_"))
			{
				std::vector<xstring> tmpvecstr;
				Helper::splitString(tmpstr, " ", tmpvecstr);
				if(tmpvecstr.size() != newwaveinfo.monsters.size())
				{
					xLogMessager::getSingleton().logMessage("boxMonsterIds_ content mismatch boxMonsterIds_ scene_file:" + strMapFile, Log_ErrorLevel);
					NEXTCONTINUE()
				}
				for(uint32 r = 0; r < tmpvecstr.size(); r++)
				{
					SM_monsterPare& monsterdata = newwaveinfo.monsters[r];
					monsterdata.monsterCount = Helper::StringToInt(tmpvecstr[r]);
					if (monsterdata.monsterCount == 0)		//�ݴ��ж� ����ID û�������Ļ���ǿ�а���������Ϊ1
					{
						monsterdata.monsterCount = 1;
					}
					MonsterType monstertype = monsterBase::getSingleton().getMonsterType(monsterdata.monsterID);
					if(monstertype == MT_BOSS || monstertype == MT_BOSSSTATE || monstertype == MT_WORLDBOSS || monstertype == MT_ELITEBOSS || monstertype == MT_MAGICBOSS)
					{
						m_bossdata.hasboss = true;
						m_bossdata.monstersBoss.push_back(monsterdata);
						newwaveinfo.isBossWave = true;      //��һС��Ϊboss��
						if (xmlHelper::parseChar(tmpstr, xmlmonster, "delayTime"))
						{
							auto bossdelay = Helper::StringToInt(tmpstr);
							if (bossdelay  > 0)
							{
								newwaveinfo.bossdelaytime = max(bossdelay, newwaveinfo.bossdelaytime);
							}
						}
					}
				}
			}
			Point2d ftposition;
			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "position", ftposition))
			{
				xLogMessager::getSingleton().logMessage("failed to parse monster position! scene_file:" + strMapFile, Log_ErrorLevel);
				NEXTCONTINUE()
			}
			newwaveinfo.startpos = xmlHelper::getServerPosition(ftposition, bondcenter, bondsize);

			if(!xmlHelper::parseAngle(xmlmonster, "rotation", newwaveinfo.roY))
			{
				xLogMessager::getSingleton().logMessage("failed to parse rotation! scene_file:" + strMapFile, Log_ErrorLevel);
				NEXTCONTINUE()
			}

			if(!xmlHelper::parse3DrectToPoint2D(xmlmonster, "boxSize", newwaveinfo.boxSize))
			{
				xLogMessager::getSingleton().logMessage("failed to parse boxSize! scene_file:" + strMapFile, Log_ErrorLevel);
				NEXTCONTINUE()
			}

			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxCycleTime") == false)
			{
				NEXTCONTINUE()
			}
			newwaveinfo.cycleTime = Helper::StringTofloat(tmpstr);

			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxCycleCount") == false)
			{
				NEXTCONTINUE()
			}
			newwaveinfo.cycleCount = Helper::StringToInt(tmpstr);

			if(xmlHelper::parseChar(tmpstr, xmlmonster, "delayTime"))
			{
				newwaveinfo.m_DelayTime = Helper::StringToInt(tmpstr);
			}
			else
			{
				newwaveinfo.m_DelayTime = 0;
			}

			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxHard") == false)
			{
				NEXTCONTINUE()
			}
			newwaveinfo.hardbit = Helper::StringToInt(tmpstr);

			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxWaveBig") == false)
			{
				NEXTCONTINUE()
			}
			newwaveinfo.bigwaveID = Helper::StringToInt(tmpstr);

			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxWaveSmall") == false)
			{
				NEXTCONTINUE()
			}
			newwaveinfo.smallwaveID = Helper::StringToInt(tmpstr);

			if(xmlHelper::parseChar(tmpstr, xmlmonster, "boxIsMainWave") == false)
			{
				NEXTCONTINUE()
			}
			if(tmpstr == "true")
			{
				newwaveinfo.ismainwave = true;
			}
			else
			{
				newwaveinfo.ismainwave = false;
			}
			newwaveinfo.activedtime = 0;
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "buffid") == true)
			{
				newwaveinfo.buffid = Helper::StringToInt(tmpstr);
			}
			if (xmlHelper::parseChar(tmpstr, xmlmonster, "buffTime") == true)
			{
				newwaveinfo.bufftime = Helper::StringToInt(tmpstr);
			}
			///�ж���ǰ���Ƿ�����Ѷ��趨
			int32 currenthardbit = 1 << (int32)hardlevel;
			if((currenthardbit & newwaveinfo.hardbit) != currenthardbit)
			{
				//�����ϵ�ǰ�Ѷ�Ҫ��.ȥ�����й�
				newwaveinfo.monsters.clear();
			}
			if(newwaveinfo.cycleTime == 0.0f)
			{
				newwaveinfo.cycleCount = 0;
			}

			if(newwaveinfo.ismainwave)
			{
				if(xmlHelper::parseChar(tmpstr, xmlmonster, "Probability") == false)
				{
					newwaveinfo.probability = FIGHT_RANDMAX;
				}
				else
				{
					///���ز�,�������򲻼���
					int32 randval = Helper::StringToInt(tmpstr);
					newwaveinfo.probability = randval;
				}
			}

			if (xmlHelper::parseChar(tmpstr, xmlmonster, "id") == false)
			{
				NEXTCONTINUE();
			}
			newwaveinfo.id = Helper::StringToInt(tmpstr);

			/// �Ƿ�������
			if (false == xmlHelper::parseChar(tmpstr, xmlmonster, "influenceWin"))
			{
				newwaveinfo.influenceWin = 0;
			}
			else
			{
				newwaveinfo.influenceWin = Helper::StringToInt(tmpstr);
			}

			insertoneWave(newwaveinfo);
		}
		else { break; }
		xmlmonster = xmlmonster->next_sibling();
	}
	if(m_wavedata.size() > 0)
	{
		takesort();
	}
	// �����ɨ���Ͳ�������һ��
	if( true == bLoadItemDrop )
	{
		// ��ƷҪ�ҵ�������
		sceneDropItemBind();
	}
	return true;
}

void monsterCreator::insertoneWave( const SM_WaveInfo& data )
{
	if(data.bigwaveID == 0)
	{
		m_fixwaveinfo.push_back(data);
	}
	else
	{
		SM_BigWave* tmpbigwave = getbigWave(data.bigwaveID);
		if(tmpbigwave == NULL)
		{
			/*
			tmpbigwave=new SM_BigWave;
			tmpbigwave->m_bigwaveID=data.bigwaveID;
			tmpbigwave->m_currentSmallWave=1;
			tmpbigwave->m_currentWaveActived=false;
			tmpbigwave->m_vecsmallwave.push_back(data);
			m_wavedata.push_back(*tmpbigwave);
			*/
			SM_BigWave oTmp;
			oTmp.m_bigwaveID = data.bigwaveID;
			oTmp.m_currentSmallWave = 1;
			oTmp.m_currentWaveActived = false;
			oTmp.m_vecsmallwave.push_back(data);
			oTmp.m_DelayTime = data.m_DelayTime;
			m_wavedata.push_back(oTmp);
		}
		else
		{
			tmpbigwave->m_vecsmallwave.push_back(data);
		}
	}
}

bool monsterCreator::isspecialbigwave(const SM_BigWave& bigwave)
{
	for (auto i = 0; i < bigwave.m_vecsmallwave.size(); ++i)
	{
		auto& info = bigwave.m_vecsmallwave[i];
		if (info.monsters.size() > 0 && info.influenceWin != 1)
		{
			return false;
		}
	}
	return true;
}

bool monsterCreator::CheckAllFinished(const SM_BigWave& bigwave)
{
	//�Ⲩ�����Ⲩ���������
	bool isspecial = isspecialbigwave(bigwave);

	if (isspecial)
	{
		for (size_t i = 0; i < m_wavedata.size(); i++)
		{
			auto& info = m_wavedata[i];
			if (!info.m_currentWaveFinished && bigwave.m_bigwaveID != info.m_bigwaveID)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

SM_BigWave* monsterCreator::getbigWave(uint32 bigwaveid)
{
	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		SM_BigWave& ret = m_wavedata[r];
		if(ret.m_bigwaveID == bigwaveid)
		{
			return &ret;
		}
	}
	return NULL;
}

void monsterCreator::sortBigWave()
{

}

void monsterCreator::takesort()
{
	sort(m_wavedata.begin(), m_wavedata.end());
	//qsort(&(*m_wavedata.begin()),m_wavedata.size(),sizeof(SM_BigWave),comparebigwave);
	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		vecsmallwave& vecdata = m_wavedata[r].m_vecsmallwave;
		if(vecdata.size() > 0)
		{
			sort(vecdata.begin(), vecdata.end());
			//qsort(&(*vecdata.begin()),vecdata.size(),sizeof(SM_WaveInfo),comparewave);
		}
		bool isfound = false;
		for(uint32 i = 0; i < vecdata.size(); i++)
		{
			if(vecdata[i].smallwaveID == 1 && vecdata[i].ismainwave)
			{
				isfound = true;
			}
		}
		if(isfound == false)
		{
			vecdata[0].ismainwave = true;
			xstring strTmp = "no found mainwave,system add auto!";
			if (NULL != m_scene)
			{
				strTmp += ("  scene_file: " + m_scene->getMapFile());
			}
			xLogMessager::getSingleton().logMessage(strTmp, Log_ErrorLevel);
		}
	}
	///ͨ�����ʴ������ز�
	vecbigwave::iterator it = m_wavedata.begin();
	while(it != m_wavedata.end())
	{
		vecsmallwave& vecdata = (*it).m_vecsmallwave;
		bool needbreak = false;
		for(uint32 r = 0; r < vecdata.size(); r++)
		{
			if(vecdata[r].ismainwave && vecdata[r].probability < FIGHT_RANDMAX)
			{
				if(rand() % FIGHT_RANDMAX > vecdata[r].probability)
				{
					///�ò����������
					it = m_wavedata.erase(it);
					needbreak = true;
					break;
				}
			}
		}
		if(needbreak)
		{
			continue;
		}
		it++;
	}
}

monsterCreator::monsterCreator(xBattleScene* scene):
	m_scene(scene), m_oreGmid(0)
{
	m_mapdropItemsBindWave.clear();
	every_wave_rate.clear();
	m_mapMonsterWave.clear();
	m_nTotalWeight = 0;
	m_nMonsterWeight = 0;
	SetMaxMonsterCount(0);
}

bool monsterCreator::getisWaveFinish() const
{
	if(m_bossdata.hasboss == true && m_bossdata.monstersBoss.size() == 0)
	{
		return true;
	}
	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		if(!m_wavedata[r].m_currentWaveFinished)
		{
			return false;
		}
	}
	return true;
}

/// �Ƿ�Ϊ���һ��
bool monsterCreator::isLasterWave()
{
	int32 nTmp = 0;
	for (uint32 r = 0; r < m_wavedata.size(); r++)
	{
		if (m_wavedata[r].m_currentWaveFinished)
		{
			nTmp++;
		}
	}
	if (m_wavedata.size() == nTmp + 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void monsterCreator::onPlayerMovePoint( uint32 roleID, const Point2d& stPos, const Point2d& enPos )
{
	return;///���ɿͻ��˼���;
	if(stPos.x < 0.0f || stPos.y < 0.0f || enPos.x < 0.0f || enPos.y < 0.0f)
	{
		return;
	}
	if(getisWaveFinish())
	{
		return;
	}
	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		SM_BigWave& bigwave = m_wavedata[r];
		if(bigwave.m_currentWaveActived)
		{
			continue;
		}
		vecpsmallwave smallwave;
		if(getCurrentWaveData(bigwave, smallwave))
		{
			std::vector<Rect2d> vecRect;
			if(!getCurrentWaveActiveWaveRect(smallwave, vecRect))
			{
				continue;
			}
			for(uint32 rr = 0; rr < vecRect.size(); rr++)
			{
				//Rect2d rct(st.x,st.y,en.x,en.y);
				Point2d linest(stPos.x, stPos.y);
				Point2d lineen(enPos.x, enPos.y);
				if(linest == lineen)
				{
					continue;
				}
				if(xMath::IsLineIntersectRect(linest, lineen, vecRect[rr]) == true)
				{
					onStartBigWave(bigwave, roleID);
					break;
				}
			}
		}
	}
}

void monsterCreator::onPlayerMove( uint32 roleID, const Point2d& pos )
{
	return;///���ɿͻ��˼���;
	if(getisWaveFinish())
	{
		return;
	}
// 	for(uint32 r=0;r<m_wavedata.size();r++)
// 		if(getisWaveFinish())
// 	{
// 		return;
// 	}
	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		SM_BigWave& bigwave = m_wavedata[r];
		if(bigwave.m_currentWaveActived)
		{
			continue;
		}
		vecpsmallwave smallwave;
		if(getCurrentWaveData(bigwave, smallwave))
		{
			std::vector<Rect2d> vecRect;
			if(!getCurrentWaveActiveWaveRect(smallwave, vecRect))
			{
				continue;
			}
			for(uint32 rr = 0; rr < vecRect.size(); rr++)
			{
				if(pos.x >= vecRect[rr].left && pos.y >= vecRect[rr].top && pos.x <= vecRect[rr].right && pos.y <= vecRect[rr].bottom)
				{
					onStartBigWave(bigwave, roleID);
					break;
				}
			}
		}
	}
}

bool monsterCreator::getCurrentWaveActiveWaveRect(vecpsmallwave vecpwavs, std::vector<Rect2d>& vecRect )
{
	for(uint32 r = 0; r < vecpwavs.size(); r++)
	{
		if(vecpwavs[r]->ismainwave)
		{
			SM_WaveInfo* data = vecpwavs[r];
			Point2d st = data->startpos - data->boxSize / 2;
			Point2d en = data->startpos + data->boxSize / 2;
			Rect2d rct(st.x, st.y, en.x, en.y);
			vecRect.push_back(rct);
			//return true;
		}
	}
	return vecRect.size() != 0;
}

void monsterCreator::onActiveCurrentWave(SM_BigWave& bigwave, uint32 roleID)
{
	FBLogic* pLogic = m_scene->getFBLogic();
// 	if(pLogic!=NULL)
// 	{
// 		pLogic->resumeTime();
// 	}
	bigwave.m_currentWavePaused = false;
	m_scene->setAllPlayerAutoRecover(true);
	//xAppliction::getSingleton().addPrintMessage(xstring("onActiveCurrentWave:bigwaveid=")+Helper::IntToString(bigwave.m_bigwaveID)+xstring("smallwave:")+Helper::IntToString(bigwave.m_currentSmallWave));
	vecpsmallwave vecdata;
	if(getCurrentWaveData(bigwave, vecdata))
	{
		notifyWave(true, bigwave, !bigwave.m_currentWaveActived);
		bigwave.m_currentWaveActived = true;
		bool isbosswave = false;
		if (isBossSmallWave(bigwave.m_bigwaveID, bigwave.m_currentSmallWave))
		{
			isbosswave = true;
		}
		for(uint32 r = 0; r < vecdata.size(); r++)
		{
			auto& info = vecdata[r];
			info->activedtime = Helper::getCurrentTime();
			if (isbosswave)
			{
				info->activedtime += 1.8 * 1000.0f;   //boss���ӳ�3sˢ
			}
			info->roleID = roleID;
			//������ÿ��һ��ʱ���������,�����һ��
			if (Helper::getCurrentTime() > info->activedtime + int32(info->m_DelayTime * 1000.0f))
			{
				PushMonster(info);
			}
		}
		auto fb_logic = m_scene->getFBLogic();
		if (fb_logic != nullptr)
		{
			fb_logic->onStartBigWave(bigwave.m_bigwaveID);
		}
	}
}

int32 monsterCreator::onPushMonsters(uint32 roleID, uint32 monsterID, uint32 count, uint32 type, uint32 camps, SM_WaveInfo* waveinfo)
{
	float stx = waveinfo->startpos.x;
	float sty = waveinfo->startpos.y;
	float enx = waveinfo->startpos.x;
	float eny = waveinfo->startpos.y;
	if (0 != waveinfo->boxSize.x || 0 != waveinfo->boxSize.y)
	{
		stx = waveinfo->startpos.x - waveinfo->boxSize.x / 2.0f;
		sty = waveinfo->startpos.y - waveinfo->boxSize.y / 2.0f;
		enx = waveinfo->startpos.x + waveinfo->boxSize.x / 2.0f;
		eny = waveinfo->startpos.x + waveinfo->boxSize.y / 2.0f;
	}
	auto monsterCount = 0;
	for (uint32 r = 0; r < count; r++)
	{
		if (!m_scene->CheckMonsterRefresh(monsterID))
		{ break; }

		float createx = 0.f;
		float createy = 0.f;

		if (0 != waveinfo->boxSize.x || 0 != waveinfo->boxSize.y)
		{
			createx = float(rand() % (int32(waveinfo->boxSize.x)));
			createy = float(rand() % (int32(waveinfo->boxSize.y)));
		}
		xBattleObject* obj = m_scene->createMonster(monsterID, Point2d(stx + createx, sty + createy), (CharacterType)type, 1, roleID, waveinfo->roY, waveinfo->id, false, waveinfo->influenceWin);
		if(obj != NULL)
		{
			obj->setWave(waveinfo->bigwaveID);
			if (waveinfo->buffid != 0 && waveinfo->bufftime != 0)
			{
				obj->addBuff(waveinfo->buffid, waveinfo->bufftime);
			}
			waveinfo->vecmonsters.push_back(obj->getid());
			if(m_scene->getBattleType() == BATTLE_BOSSFIGHT)
			{
				auto fbplayer = m_scene->getPlayerByID(roleID);
				if (fbplayer)
				{
					obj->SetHP(fbplayer->GetBossLeftHp(), false);
				}
			}
			m_scene->processBroadCastMonsterIn(obj);
			m_scene->addhateplayer(roleID, obj->getid());
			monsterCount++;
		}
	}
	return monsterCount;
}

bool monsterCreator::getCurrentWaveData(SM_BigWave& bigwave, vecpsmallwave& data )
{
	vecsmallwave* smallwave = &bigwave.m_vecsmallwave;
	if (smallwave != NULL)
	{
		bool isfound = false;
		for (uint32 r = 0; r < smallwave->size(); r++)
		{
			SM_WaveInfo& info = smallwave->at(r);
			if (info.smallwaveID == bigwave.m_currentSmallWave)
			{
				data.push_back(&info);
				isfound = true;
			}
		}
		return isfound;
	}
	return false;
}

void monsterCreator::finishcurrentwave(SM_BigWave& bigwave)
{
	//if(bigwave.m_currentSmallWave<bigwave.m_vecsmallwave.size()-1)
//	{
	//xAppliction::getSingleton().addPrintMessage(xstring("finishcurrentwave:")+Helper::IntToString(bigwave.m_currentSmallWave));
	FBLogic* tlogic = m_scene->getFBLogic();

	bigwave.m_currentWavePaused = true;
	bigwave.m_currentSmallWave++;
	if(hasnextwave(bigwave))
	{
		SM_ActiveBigWave activewave;
		switch(m_scene->getBattleType())
		{
			case BATTLE_MIWANG_TOWER:
			case BATTLE_JUEWANG_TOWER:
			case  BATTLE_SIWANG_TOWER:
				activewave.activeTime = NowTime + 60;
				break;
			case BATTLE_LOVEKEEPER:
				activewave.activeTime = NowTime;
				break;
			case BATTLE_GOLDFB:
			case BATTLE_LIFEFB:
				activewave.activeTime = NowTime + 3;
				break;
			default:
				activewave.activeTime = NowTime;
				break;
		}
		activewave.bigwavedata = &bigwave;
		activewave.roleID = 0;
		m_vecActiveBigWave.push_back(activewave);
		if(tlogic != NULL)
		{
			tlogic->onSmallWaveFinish(bigwave.m_currentSmallWave);
		}
		//onActiveCurrentWave(bigwave,0);
	}
	else
	{
		bigwave.m_currentWaveFinished = true;
	}
	notifyWave(false, bigwave, bigwave.m_currentWaveFinished);

	if (bigwave.m_currentWaveFinished)
	{
		if (tlogic != nullptr)
		{
			tlogic->onFinishBigWave(bigwave.m_bigwaveID);
		}
	}
// 	}
// 	else
// 	{
// 		bigwave.m_currentWaveFinished=true;
// 	}
}

void monsterCreator::PushMonster(SM_WaveInfo* info)
{
	//int32 lastnum=info->cycleCount;
	if (info->cycleCount > 0)
	{
		///ִ��ÿ��һ��ʱ�����
		for (uint32 rr = 0; rr < info->monsters.size(); rr++)
		{
			SM_monsterPare& paredata = info->monsters[rr];
			for (size_t i = 0; i < info->cycleCount; i++)
			{
				if (paredata.freshCount == 0 || (paredata.monsterCount > 0 && (Helper::getCurrentTime() > info->cycleTime * 1000.0f + paredata.freshtime)))
				{
					//info->activedtime = Helper::getCurrentTime();
					paredata.freshtime = Helper::getCurrentTime();
					auto pushcount = onPushMonsters(info->roleID, paredata.monsterID, 1, paredata.type, 1, info);
					paredata.monsterCount -= pushcount;		 //"boxMonsterCounts_"�ֶ�
					paredata.freshCount += pushcount;
				}
				//info->cycleCount -= pushcount;
			}
		}
	}
	else     //һ���Գ����
	{
		for (uint32 rr = 0; rr < info->monsters.size(); rr++)
		{
			SM_monsterPare& paredata = info->monsters[rr];	///LLL;
			if (paredata.monsterCount > 0)
			{
				auto pushcount = onPushMonsters(info->roleID, paredata.monsterID, paredata.monsterCount, paredata.type, 1, info);
				paredata.monsterCount -= pushcount;
			}
			//finishcurrentwave(bigwave);
		}
	}
}

void monsterCreator::PushTrapMachine()
{
	for (auto i = 0; i < m_trapwaveinfo.size(); ++i)
	{
		auto& trapinfo = m_trapwaveinfo[i];
		for (uint32 rr = 0; rr < trapinfo.monsters.size(); rr++)
		{
			if (trapinfo.activedtime == 0)
			{
				trapinfo.activedtime = Helper::getCurrentTime();
			}
			if (Helper::getCurrentTime() >= trapinfo.activedtime + trapinfo.cycleTime * 1000.0f)
			{
				auto& monster_pare = trapinfo.monsters[rr];
				if (monster_pare.monsterCount > 0)
				{
					auto pushcount = onPushMonsters(0, monster_pare.monsterID, monster_pare.monsterCount, monster_pare.type, 1, &trapinfo);
					monster_pare.monsterCount -= pushcount;
				}
			}
		}
	}
}

bool monsterCreator::HaveMonsterAlive(SM_WaveInfo* info)
{
	for (uint32 k = 0; k < info->vecmonsters.size(); k++)
	{
		auto obj = m_scene->findMonster(info->vecmonsters[k]);
		if (obj != nullptr)
		{
			if (NULL != obj->getPlayer() && 0 == obj->getPlayer()->getInfluenceWin() && obj->getRoleState() != BOS_DIE)
			{
				return true;
			}
		}
	}

	//����������Ҫ�жϹ����Ƿ������ˢ��һ��
	if (m_scene->getBattleType() == BATTLE_PVE_CHESS)
	{
		auto it = m_scene->getMonsterItBegin();
		while (it != m_scene->getMonsterItEnd())
		{
			auto obj = it->second;
			if (obj != nullptr && obj->getCharacterType() == CT_Monster && obj->getCamps() != 0)
			{
				if (obj->getRoleState() != BOS_DIE)
				{
					return true;
				}
			}
			++it;
		}
	}

	return false;
}

uint32 monsterCreator::GetMonsterLeftCount(SM_WaveInfo* info)
{
	uint32 monstercount = 0;
	//��Ӱ��ͨ�صĹ�����������������
	for (uint32 j = 0; j < info->monsters.size(); j++)
	{
		monstercount += info->monsters[j].monsterCount;
	}
	return monstercount;
}

void monsterCreator::update()
{
	if(isBossAllDie())
	{
		return;
	}

	PushTrapMachine();

	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		SM_BigWave& bigwave = m_wavedata[r];
		if(!bigwave.m_currentWaveActived || bigwave.m_currentWaveFinished || bigwave.m_currentWavePaused)
		{
			continue;
		}
		vecpsmallwave smallwav;
		if(getCurrentWaveData(bigwave, smallwav))
		{
			bool allfinished = true;
			for(uint32 i = 0; i < smallwav.size(); i++)
			{
				SM_WaveInfo* info = smallwav[i];
				if (Helper::getCurrentTime() > info->activedtime + int32(info->m_DelayTime * 1000.0f) /*|| info->bossStartRefresh == true*/)
				{
					PushMonster(info);
				}

				uint32 monstercount = GetMonsterLeftCount(info);
				if(monstercount == 0)
				{
					if (HaveMonsterAlive(info)) //�����Ƿ���û�����Ĺ� ȫ�����˲������
					{
						allfinished = false;
					}
				}
				else
				{
					allfinished = false;
				}
			}

			if(allfinished || CheckAllFinished(bigwave))
			{
				finishcurrentwave(bigwave);
			}
		}
	}

	if(m_vecActiveBigWave.size() > 0)
	{
		vecActiveBigWave::iterator it = m_vecActiveBigWave.begin();
		while(it != m_vecActiveBigWave.end())
		{
			SM_ActiveBigWave& data = *it;
			if(NowTime >= data.activeTime)
			{
				if(data.roleID == 0)
				{
					PlayerIDMap::iterator itrator = m_scene->getPlayerItBegin();
					if(itrator != m_scene->getPlayerItEnd())
					{
						xBattleObject* obj = itrator->second;
						if(obj != NULL)
						{
							data.roleID = obj->getid();
						}
					}
				}
				onActiveCurrentWave(*data.bigwavedata, data.roleID);
				it = m_vecActiveBigWave.erase(it);
				continue;
			}
			it++;
		}
	}
}

bool monsterCreator::hasnextwave( SM_BigWave& bigwave )
{
	for(uint32 r = 0; r < bigwave.m_vecsmallwave.size(); r++)
	{
		if(bigwave.m_currentSmallWave == bigwave.m_vecsmallwave[r].smallwaveID)
		{
			return true;
		}
	}
	return false;
}

void monsterCreator::onMonsterDie( uint32 id )
{
	if(m_bossdata.hasboss == false)
	{
		return;
	}
	for(uint32 r = 0; r < m_bossdata.monstersBoss.size(); r++)
	{
		SM_monsterPare& monterdata = m_bossdata.monstersBoss[r];
		if(monterdata.monsterID == id)
		{
			if(monterdata.monsterCount > 0)
			{
				monterdata.monsterCount--;
			}
			if(monterdata.monsterCount == 0)
			{
				m_bossdata.monstersBoss.erase(m_bossdata.monstersBoss.begin() + r);
			}
			return;
		}
	}
}

bool monsterCreator::isBossAllDie()
{
	if(m_bossdata.hasboss == false)
	{
		return false;
	}
	if(m_bossdata.monstersBoss.size() == 0)
	{
		return true;
	}
	return false;
}

void monsterCreator::onPushFixMonsters()
{
	for(uint32 r = 0; r < m_fixwaveinfo.size(); r++)
	{
		SM_WaveInfo& waveinfo = m_fixwaveinfo[r];
		for(uint32 rr = 0; rr < waveinfo.monsters.size(); rr++)
		{
			auto& monster_pare = waveinfo.monsters[rr];
			auto count = monster_pare.monsterCount;
			if (m_scene->CheckMonsterRefresh(monster_pare.monsterID))
			{
				auto pushcount = onPushMonsters(0, monster_pare.monsterID, monster_pare.monsterCount, monster_pare.type, 1, &waveinfo);
				count -= pushcount;
			}
			for (auto i = 0; i < count; ++i)
			{
				GM_BuffInfo buffdata;
				buffdata.set_buffid(waveinfo.buffid);
				buffdata.set_lasttime(waveinfo.bufftime);
				m_scene->AddtempMonster(monster_pare.monsterID
				                        , Point2d(waveinfo.startpos.x, waveinfo.startpos.y)
				                        , static_cast<CharacterType>(monster_pare.type), 1, 0, buffdata);
			}
		}
	}
}


void monsterCreator::onPushFixMonstersEx(bool bFlag)
{
	if (bFlag == false)
	{
		return;
	}
	for (uint32 r = 0; r < m_fixwaveinfo.size(); r++)
	{
		SM_WaveInfo& waveinfo = m_fixwaveinfo[r];
		for (uint32 rr = 0; rr < waveinfo.monsters.size(); rr++)
		{
			auto& monster_pare = waveinfo.monsters[rr];
			auto count = monster_pare.monsterCount;
			if (m_scene->CheckMonsterRefresh(monster_pare.monsterID))
			{
				auto pushcount = onPushMonsters(0, monster_pare.monsterID, monster_pare.monsterCount, monster_pare.type, 1, &waveinfo);
				count -= pushcount;
			}
			for (auto i = 0; i < count; ++i)
			{
				GM_BuffInfo buffdata;
				buffdata.set_buffid(waveinfo.buffid);
				buffdata.set_lasttime(waveinfo.bufftime);
				m_scene->AddtempMonster(monster_pare.monsterID
				                        , Point2d(waveinfo.startpos.x, waveinfo.startpos.y)
				                        , static_cast<CharacterType>(monster_pare.type), 1, 0, buffdata);
			}
		}
	}
}

bool monsterCreator::IsFriendId(int32 id)
{
	if (m_friendMap.find(id) != m_friendMap.end())
	{ return true; }

	return false;
}

int32 monsterCreator::GetMaxIndexFriendId()
{
	auto index = 0;
	auto friendid = 0;
	for (auto it = m_friendMap.begin(); it != m_friendMap.end(); ++it)
	{
		if (it->second > index)
		{
			index = it->second;
			friendid = it->first;
		}
	}
	return friendid;
}

int32 monsterCreator::getFriendLoverId()
{
	auto friendid = 0;
	for (auto it = m_friendMap.begin(); it != m_friendMap.end(); ++it)
	{
		if (it->second >= 100 && it->second <= 200)
		{
			friendid = it->first;
			break;
		}
	}
	return friendid;
}

void monsterCreator::notifyWave(bool isstart, const SM_BigWave& bigwave, bool isbigwavestartorend)
{
	auto bigwaveid = bigwave.m_bigwaveID;
	auto smallwaveid = bigwave.m_currentSmallWave;

	GM_NotifyWaveInfo msend;
	msend.set_bigwaveid(bigwaveid);
	msend.set_smallwaveid(smallwaveid);
	msend.set_isbigwavestartorend(isbigwavestartorend);

	//ȡ��С����Ϣ
	auto isbosswave = isBossSmallWave(bigwaveid, smallwaveid);
	msend.set_isbosswave(isbosswave);
	msend.set_delaytime(GetSmallWaveDelayTime(bigwaveid, smallwaveid));

	if (isstart)
	{
		m_scene->foreachPlayer(boost::bind(&monsterCreator::notifywaveStart, this, _1, _2), (void*)&msend);
		auto rundrop = m_scene->getRunDropPtr();
		if (rundrop)
		{
			rundrop->OnStartNewWave(bigwave.m_bigwaveID, bigwave.m_currentSmallWave);
		}
	}
	else
	{
		m_scene->foreachPlayer(boost::bind(&monsterCreator::notifywaveEnd, this, _1, _2), (void*)&msend);
	}
}

void monsterCreator::notifywaveStart(xBattleObject* obj, void* pData)
{
	GM_NotifyWaveInfo* msend = (GM_NotifyWaveInfo*)(pData);
	obj->send(GM_NOTIFY_NEWWAVE_START, *msend);
	char nottime[DateTimeLength];
	Helper::getNowCharTime(nottime, DateTimeLength);
	xAppliction::getSingleton().addPrintMessage(xstring(nottime) + xstring("notifywaveStart,big waveID:") + Helper::IntToString(msend->bigwaveid()) + xstring(" smallwaveid:") + Helper::IntToString(msend->smallwaveid()));
}

void monsterCreator::notifywaveEnd( xBattleObject* obj, void* pData )
{
	GM_NotifyWaveInfo* msend = (GM_NotifyWaveInfo*)(pData);
	obj->send(GM_NOTIFY_NEWWAVE_END, *msend);
	char nottime[DateTimeLength];
	Helper::getNowCharTime(nottime, DateTimeLength);
	xAppliction::getSingleton().addPrintMessage(xstring(nottime) + xstring("notifywaveEnd,big waveID:") + Helper::IntToString(msend->bigwaveid()) + xstring(" smallwaveid:") + Helper::IntToString(msend->smallwaveid()));

	SM_Wave bigwave;
	bigwave.m_bigwaveID = msend->bigwaveid();
	bigwave.m_currentSmallWave = msend->smallwaveid();
	bigwave.m_currentWaveFinished = msend->isbigwavestartorend();

	NetworkServer::getSingleton().send(SM_NOTIFY_NEWWAVE_END, bigwave, GETGMIP(), obj->getid());

}

void monsterCreator::onActiveWaitWaveNow()
{
	vecActiveBigWave::iterator it = m_vecActiveBigWave.begin();
	while(it != m_vecActiveBigWave.end())
	{
		SM_ActiveBigWave& data = *it;
		data.activeTime = NowTime;
		it++;
	}
}

const int32 monsterCreator::getFirstSmallWaveID() const
{
	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		if(m_wavedata[r].m_currentWaveActived)
		{
			return m_wavedata[r].m_currentSmallWave;
		}
	}
	return 0;
}

void monsterCreator::onStartBigWave( SM_BigWave& bigwave, uint32 roleID )
{

	for(uint32 r = 0; r < m_vecActiveBigWave.size(); r++)
	{
		if(m_vecActiveBigWave[r].bigwavedata != NULL)
		{
			if(m_vecActiveBigWave[r].bigwavedata->m_bigwaveID == bigwave.m_bigwaveID)
			{
				return;
			}
		}
	}
	bigwave.m_currentWavePaused = true;
	SM_ActiveBigWave activewave;
	activewave.activeTime = 0;
	activewave.activeTime += bigwave.m_DelayTime;
	switch(m_scene->getBattleType())
	{
		case BATTLE_MIWANG_TOWER:
		case BATTLE_JUEWANG_TOWER:
		case  BATTLE_SIWANG_TOWER:
			activewave.activeTime += NowTime + 5;
			break;
		case BATTLE_LOVEKEEPER:
			activewave.activeTime += NowTime;
			break;
		default:
			activewave.activeTime += NowTime;
			break;
	}
	activewave.bigwavedata = &bigwave;
	activewave.roleID = roleID;
	m_vecActiveBigWave.push_back(activewave);
}

void monsterCreator::RecordChessSmallWaveCount(int32 smallwave, int32 count)
{
	if (m_scene->getBattleType() != BATTLE_PVE_CHESS)
	{ return; }

	m_MapMonsterWaveCount[smallwave] += count;
}

monsterWaveCount& monsterCreator::GetChessWaveCount()
{
	return m_MapMonsterWaveCount;
}

uint32 monsterCreator::getCurrentFloor()
{
	if(m_wavedata.size() > 0)
	{
		SM_BigWave& info = m_wavedata[0];
		return info.m_currentSmallWave;
	}
	return 0;
}

bool monsterCreator::killObstacle( uint32 id, SM_ObstaleData& data )
{
	map_obstaleData::iterator it = m_map_obstaleData.find(id);
	if(it != m_map_obstaleData.end())
	{
		PlayerIDMap::iterator itPlayer = m_scene->getPlayerItBegin();
		PlayerIDMap::iterator itPlayerend = m_scene->getPlayerItEnd();

		data = it->second;
		for (; itPlayer != itPlayerend; itPlayer++)
		{
			if (NULL != itPlayer->second)
			{
				onPushObstaleMonster(itPlayer->second, data);
				break;
			}
		}
		m_map_obstaleData.erase(it);

		itPlayer = m_scene->getPlayerItBegin();
		for(; itPlayer != itPlayerend; itPlayer++)
		{
			if (m_map_obstaleData.size() == 0)//������е�����
			{
				(itPlayer->second)->setisWreck();
			}
			if (!(itPlayer->second)->isKillAllWreck())
			{
				(itPlayer->second)->addisWreck();
			}
		}

		return true;
	}
	return false;
}

void monsterCreator::onPushObstaleMonster(xBattleObject* obj, const SM_ObstaleData& obstaleData)
{
	if (NULL == m_scene)
	{
		return;
	}

	int32 rnd = rand() % FIGHT_RANDMAX;
	if((uint32)rnd <= obstaleData.monsterrate)
	{
		float stx = obstaleData.box_pos.x;
		float sty = obstaleData.box_pos.y;
		float enx = obstaleData.box_pos.x;
		float eny = obstaleData.box_pos.y;
		if (0 != obstaleData.boxsize.x || 0 != obstaleData.boxsize.y)
		{
			stx = obstaleData.box_pos.x - obstaleData.boxsize.x / 2.0f;
			sty = obstaleData.box_pos.y - obstaleData.boxsize.y / 2.0f;
			enx = obstaleData.box_pos.x + obstaleData.boxsize.x / 2.0f;
			eny = obstaleData.box_pos.x + obstaleData.boxsize.y / 2.0f;
		}

		for(uint32 r = 0; r < obstaleData.obstaleMonsters.size(); r++)
		{
			for(uint32 rr = 0; rr < obstaleData.obstaleMonsters[r].monstercount; rr++)
			{
				float createx = 0.f;
				float createy = 0.f;

				if (0 != obstaleData.boxsize.x || 0 != obstaleData.boxsize.y)
				{
					createx = float(rand() % (int32(obstaleData.boxsize.x)));
					createy = float(rand() % (int32(obstaleData.boxsize.y)));
				}
				xBattleObject* objMonster = m_scene->createMonster(obstaleData.obstaleMonsters[r].monsterid, Point2d(stx + createx, sty + createy), CT_Monster, 1, 0U, 0.0f, 0, -1);
				if (objMonster != NULL)
				{
					m_scene->processBroadCastMonsterIn(objMonster);

					m_scene->addhateplayer(obj->getid(), objMonster->getid());
				}
			}
		}
	}
}

uint32 monsterCreator::getFirstBigWaveSmallWaveID()
{
	if(m_vecActiveBigWave.size() > 0)
	{
		return m_vecActiveBigWave[0].bigwavedata->m_currentSmallWave;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
void monsterCreator::activeBigWave(int32 bigwaveNum, int32 roleid)
{
	if(getisWaveFinish())
	{
		return;
	}
	for(uint32 r = 0; r < m_wavedata.size(); r++)
	{
		SM_BigWave& bigwave = m_wavedata[r];
		if(bigwave.m_currentWaveActived)
		{
			continue;
		}
		if ((int32)(bigwave.m_bigwaveID) != bigwaveNum)
		{
			continue;
		}
//		vecpsmallwave smallwave;
		//	if(getCurrentWaveData(bigwave,smallwave))
		//	{
		//	std::vector<Rect2d> vecRect;
		//	if(!getCurrentWaveActiveWaveRect(smallwave,vecRect))
		//	{
		//		continue;
		//	}
		//	for(uint32 rr=0;rr<vecRect.size();rr++)
		//	{
		////Rect2d rct(st.x,st.y,en.x,en.y);
		//Point2d linest(stPos.x,stPos.y);
		//Point2d lineen(enPos.x,enPos.y);
		//if(linest==lineen)
		//{
		//	continue;
		//}
		//if(xMath::IsLineIntersectRect(linest,lineen,vecRect[rr])==true)
		//{
		onStartBigWave(bigwave, roleid);

		break;
		//	}
		//		}
		//	}
	}
}
void monsterCreator::getVecSamllSave(vecsmallwave& monsterInfo)
{
	// 		vecsmallwave::iterator itSmall = m_fixwaveinfo.begin();
	// 		vecsmallwave::iterator itSmallend = m_fixwaveinfo.end();
	// 		for (;itSmall!=itSmallend;++itSmall)
	// 		{
	// 			monsterInfo.push_back(*itSmall)
	// 		}
	std::copy(m_fixwaveinfo.begin(), m_fixwaveinfo.end(), std::back_inserter(monsterInfo));
	vecbigwave::const_iterator itwave = m_wavedata.begin();
	vecbigwave::const_iterator itwaveend = m_wavedata.end();
	for (; itwave != itwaveend; ++itwave)
	{
		std::copy(itwave->m_vecsmallwave.begin(), itwave->m_vecsmallwave.end(), std::back_inserter(monsterInfo));
	}
}

void monsterCreator::onBossComing(int32 bigwaveid, int32 smallwaveid)
{
	auto isbosswave = isBossSmallWave(bigwaveid, smallwaveid);
	if (isbosswave)
	{
		setBossStartRefresh(bigwaveid, smallwaveid);
	}
}

bool monsterCreator::isBossSmallWave(int32 bigwaveid, int32 smallwaveid)
{
	auto bigwave = getbigWave(bigwaveid);
	if (bigwave)
	{
		for (auto r = 0; r < bigwave->m_vecsmallwave.size(); ++r)
		{
			auto& smallwave = bigwave->m_vecsmallwave[r];
			if (smallwave.smallwaveID == smallwaveid && smallwave.bigwaveID == bigwave->m_bigwaveID)
			{
				if (smallwave.isBossWave == true)
				{
					return true;
				}

			}
		}
		return false;
	}
	return false;
}

float monsterCreator::GetSmallWaveDelayTime(int32 bigwaveid, int32 smallwaveid)
{
	auto deleytime = 0;
	auto bigwave = getbigWave(bigwaveid);
	if (bigwave)
	{
		for (auto r = 0; r < bigwave->m_vecsmallwave.size(); ++r)
		{
			auto& smallwave = bigwave->m_vecsmallwave[r];
			if (smallwave.smallwaveID == smallwaveid && smallwave.bigwaveID == bigwave->m_bigwaveID)
			{
				if (smallwave.bossdelaytime > deleytime && smallwave.bossdelaytime > 0)
				{
					deleytime = smallwave.bossdelaytime;
				}
			}
		}
	}
	return deleytime;
}

void monsterCreator::setBossStartRefresh(int32 bigwaveid, int32 smallwaveid)
{
	auto bigwave = getbigWave(bigwaveid);
	if (bigwave)
	{
		for (auto r = 0; r < bigwave->m_vecsmallwave.size(); ++r)
		{
			auto& smallwave = bigwave->m_vecsmallwave[r];
			if (smallwave.smallwaveID == smallwaveid && smallwave.bigwaveID == bigwave->m_bigwaveID)
			{
				smallwave.bossStartRefresh = true;
			}
		}
	}
}

/// ���������
bool monsterCreator::onCalcRandPos()
{
	CHECKERRORANDRETURNRESULT(0 < m_wavedata.size(), false);

	vecsmallwave& tmpSmallWave = m_wavedata[0].m_vecsmallwave;
	int32 nRand = Helper::getRandomInField(0, FIGHT_RANDMAX);
	int32 nCount = tmpSmallWave.size();
	int32 nStep = ceil((FIGHT_RANDMAX * 1.0f) / (nCount * 1.0f));
	int32 nTotal = nStep;

	int32 nLoop = 0;
	vecsmallwave::iterator vet_it = tmpSmallWave.begin();
	for (; vet_it < tmpSmallWave.end(); vet_it++, nLoop++)
	{
		if (nRand > nTotal)
		{
			nTotal += nStep;
		}
		else
		{
			break;
		}
	}
	SM_WaveInfo tmpBigWave = tmpSmallWave[nLoop];
	tmpSmallWave.clear();
	tmpSmallWave.push_back(tmpBigWave);
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// ��ȡ�������Ʒ
void monsterCreator::getDropItems(vector<ObjCountDrop>& droplist, uint32 monsterSceneID, bool bFlag /*= false*/)
{
	/// �ҳ������Ӧ�Ĳ�
	int32 nMonsterWave = getMonsterWaveFromMonsterSceneID(monsterSceneID);

	/// �ҳ�ָ������Ʒ
	mapDropItems::iterator map_it = m_mapdropItemsBindWave.begin();
	for (; map_it != m_mapdropItemsBindWave.end(); map_it++)
	{
		/// �ӹ���(-1)
		/// ��������Ʒ����ͬ�Ĳ�
		/// �Ƿ�ָ��ȫ������
		if (-1 == map_it->second.monster_wave || nMonsterWave == map_it->second.monster_wave || true == bFlag)
		{
			vector<DropItems>::iterator vet_it = map_it->second.vec_itemids.begin();
			for (; vet_it != map_it->second.vec_itemids.end(); vet_it++)
			{
				ObjCountDrop dropItem;
				dropItem.objId = vet_it->nDropItemID;
				dropItem.count = vet_it->nDropItemNum;
				dropItem.profession = vet_it->nDropItemProfession;
				dropItem.nWave = map_it->second.monster_wave;
				droplist.push_back(dropItem);
			}
		}
	}
}


/// ��ȡ���ж�����Ʒ
void monsterCreator::getDecideDropItems(vector<ObjCountDrop>& droplist, bool bFlag /*= false*/, int32 nWave /*= -1*/)
{
	if (0 >= droplist.size())
	{
		return;
	}

	/// �Ƿ����ȫ����Ʒ(true ��)
	if (false == bFlag)
	{
		/// �����ȡ������Ʒ
		if(-1 == nWave)
		{
			ObjCountDrop TmpDropItem;
			/// ������һ���������Ʒ
			int32 nTmpRand = Helper::getRandomInField(0, droplist.size() - 1);
			if (nTmpRand < 0 || nTmpRand >= droplist.size())
			{
				nTmpRand = 0;
			}
			/// ͨ��Ȩ�����ж������Ʒ�Ƿ����
			TmpDropItem = droplist[nTmpRand];
			droplist.clear();
			droplist.push_back(TmpDropItem);
		}
		else
		{
			/// ȥ��������һ���ĵ�����Ʒ
			vector<ObjCountDrop>::iterator vet_it = droplist.begin();
			for (; vet_it != droplist.end(); )
			{
				if (nWave != vet_it->nWave)
				{
					vet_it = droplist.erase(vet_it);
				}
				else
				{
					vet_it++;
				}
			}
		}
	}
}

/// �ƶ��Ѿ��������Ʒ
void monsterCreator::removeDropItems(vector<ObjCountDrop>& droplist)
{
	/// ����������Ʒ
	vector<ObjCountDrop>::iterator vet_it = droplist.begin();
	for (; vet_it != droplist.end(); vet_it++)
	{
		/// �ҳ�ָ������Ʒ
		mapDropItems::iterator map_it = m_mapdropItemsBindWave.find(vet_it->nWave);
		/// ������ǰ����Ʒ����
		if ( map_it != m_mapdropItemsBindWave.end())
		{
			vector<DropItems>::iterator vetDropItem = map_it->second.vec_itemids.begin();
			for (; vetDropItem != map_it->second.vec_itemids.end(); )
			{
				/// �Ƴ��ҵ�����Ʒ
				if (vetDropItem->nDropItemID == vet_it->objId)
				{
					vetDropItem = map_it->second.vec_itemids.erase(vetDropItem);
					break;
				}
				else { vetDropItem++; }
			}
			/// ɾ��ָ����
			if (0 == map_it->second.vec_itemids.size())
			{
				m_mapdropItemsBindWave.erase(map_it);
			}
		}
	}
}

// ��ȡ��ǰ����ĵ���
void monsterCreator::getDropItemFromMonster(vector<ObjCountDrop>& droplist, uint32 monsterID)
{
	// �Ƿ����������͵Ĺ�
	mapdibm::iterator map_it	= m_mapdropItemsBindMonster.find(monsterID);
	if( map_it == m_mapdropItemsBindMonster.end() )
	{
		return ;
	}
	vecDIBM& vecdibm	= map_it->second;
	// ������ֻ��
	for( vecDIBM::iterator vec_it = vecdibm.begin(); vec_it != vecdibm.end(); vec_it++ )
	{
		// �޸ı�ʶ��
		if( 1 == vec_it->bDropFlag )
		{
			continue;
		}
		vec_it->bDropFlag	= 1;
		// ����˳����б�ʶ�޸� �� ��������Ʒ
		for( int32 nLoop = 0; nLoop < vec_it->vec_itemids.size(); nLoop++ )
		{
			// ���������Ʒ
			if( 0 < vec_it->vec_itemids.size() )
			{
				// ����Ʒ
				ObjCountDrop dropitems;
				// �жϵ�ǰְҵ�Ƿ�ƥ��
// 				if( -1 == vec_it->vec_itemids[nLoop].nDropItemProfession || rolePlayer->getBaseData().profession == vec_it->vec_itemids[nLoop].nDropItemProfession )
// 				{
// 					dropitems.objId	= vec_it->vec_itemids[nLoop].nDropItemID;
// 				}
//  				else
//  				{
// 					continue;
//  					dropitems.objId = itemDropBase::getSingleton().getProfessionItem(vec_it->vec_itemids[nLoop].nDropItemProfession, rolePlayer->getBaseData().profession, vec_it->vec_itemids[nLoop].nDropItemID);
//  				}
				dropitems.objId	= vec_it->vec_itemids[nLoop].nDropItemID;
				dropitems.profession = vec_it->vec_itemids[nLoop].nDropItemProfession;
				dropitems.count = 1;

				// ���������Ƿ��Ѿ����ˣ����˾�ֱ�Ӽӣ�û�оͲ���
				bool bFlag = false;
				for (int32 nLoop = 0; nLoop < droplist.size(); nLoop++)
				{
					if (droplist[nLoop].objId == dropitems.objId)
					{
						droplist[nLoop].count = droplist[nLoop].count + dropitems.count;
						bFlag = true;
						break;
					}
				}
				if (false == bFlag)
				{
					droplist.push_back(dropitems);
				}
			}
		}
		return ;
	}
}

/// ȫ������
void monsterCreator::getAllDropItemFromMonster(vector<ObjCountDrop>& droplist)
{
	// �Ƿ����������͵Ĺ�
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.begin();
	for (; map_it != m_mapdropItemsBindMonster.end(); map_it++)
	{
		vecDIBM& vecdibm = map_it->second;
		// ������ֻ��
		for (vecDIBM::iterator vec_it = vecdibm.begin(); vec_it != vecdibm.end(); vec_it++)
		{
			// �޸ı�ʶ��
			if (1 == vec_it->bDropFlag)
			{
				continue;
			}
			vec_it->bDropFlag = 1;
			// ����˳����б�ʶ�޸� �� ��������Ʒ
			for (int32 nLoop = 0; nLoop < vec_it->vec_itemids.size(); nLoop++)
			{
				// ���������Ʒ
				if (0 < vec_it->vec_itemids.size())
				{
					// ����Ʒ
					ObjCountDrop dropitems;
					dropitems.objId = vec_it->vec_itemids[nLoop].nDropItemID;
					dropitems.profession = vec_it->vec_itemids[nLoop].nDropItemProfession;
					dropitems.count = 1;

					// ���������Ƿ��Ѿ����ˣ����˾�ֱ�Ӽӣ�û�оͲ���
					bool bFlag = false;
					for (int32 nLoop = 0; nLoop < droplist.size(); nLoop++)
					{
						if (droplist[nLoop].objId == dropitems.objId)
						{
							droplist[nLoop].count = droplist[nLoop].count + dropitems.count;
							bFlag = true;
							break;
						}
					}
					if (false == bFlag)
					{
						droplist.push_back(dropitems);
					}
				}
			}
		}
	}
}

// �ѳ����е������Ʒ������
void monsterCreator::sceneDropItemBind(FirstDrop nFlag /*= FirstDrop::FD_FB_STATUS*/)
{
	// ��¼��ǰ��������������Ĺ�
	if ( 0 < m_wavedata.size() ) { recordMonster(); }
	if ( 0 < m_fixwaveinfo.size() ) { recordMonster_fix(); }

	/// ��Ȩ�ؼ���
	calcTotalWeight();
	calcWaveTotalWeight();

	vec_scene vecscenedrop;
	vecscenedrop.clear();
	m_mapdropItemsBindWave.clear();

	// ��ȡ��ǰ������Ӧ�õ������Ʒ��
	itemDropBase::getSingleton().getSceneSpecialDropData(vecscenedrop, m_scene->getFBTypeID(), m_scene->getSpecialJoinTimes(), nFlag);

	/// ������ѡ���ж�
	selDropGroup(vecscenedrop);
}

// �ѳ����е������Ʒ������
void monsterCreator::sceneDropItemBind(int32 professional, int32 nFlag /*= FirstDrop::FD_FB_STATUS*/)
{
	// ��¼��ǰ��������������Ĺ�
	if (0 < m_wavedata.size()) { recordMonster(); }
	if (0 < m_fixwaveinfo.size()) { recordMonster_fix(); }

	/// ��Ȩ�ؼ���
	calcTotalWeight();
	calcWaveTotalWeight();

	vec_scene vecscenedrop;
	vecscenedrop.clear();
	m_mapdropItemsBindWave.clear();

	// ��ȡ��ǰ������Ӧ�õ������Ʒ��
	itemDropBase::getSingleton().getSceneSpecialDropData(vecscenedrop, m_scene->getFBTypeID(), m_scene->getSpecialJoinTimes(), nFlag);

	/// ������ѡ���ж�
	selDropGroup(vecscenedrop, professional);
}

/// ������ѡ���ж�
void monsterCreator::selDropGroup(vec_scene& vecscenedrop)
{
	// ������Ʒ��
	vec_scene::iterator vet_it = vecscenedrop.begin();
	for (; vet_it != vecscenedrop.end(); vet_it++)
	{
		// �����ĸ���Ʒ��
		int32 nRandItem = Helper::getRandomInField(0, DEFAULT_RAND_MAX_NUMBER);
		/// 1. �����ж�
		/// 2. ����Ǳص�,��ô,��������������Ʒ������
		if (vet_it->rate >= nRandItem || 1 == vet_it->isMustDrop)
		{
			saveDropGroupItem(*vet_it);
		}
	}
}

void monsterCreator::selDropGroup(vec_scene& vecscenedrop, int32 professional)
{
	// ������Ʒ��
	vec_scene::iterator vet_it = vecscenedrop.begin();
	for (; vet_it != vecscenedrop.end(); vet_it++)
	{
		// �����ĸ���Ʒ��
		int32 nRandItem = Helper::getRandomInField(0, DEFAULT_RAND_MAX_NUMBER);
		/// 1. �����ж�
		/// 2. ����Ǳص�,��ô,��������������Ʒ������
		if (vet_it->rate >= nRandItem || 1 == vet_it->isMustDrop)
		{
			saveDropGroupItem(*vet_it, professional);
		}
	}
}

/// ���������Ʒ
void monsterCreator::saveDropGroupItem(const scene_drop& scenedrop)
{
	// �ӵ�������ȡ��Ҫ�������Ʒ����Ʒ����
	map<int32, int32> map_param;
	itemDropBase::getSingleton().getDropItemNum(scenedrop.dropgroupid, scenedrop.dropgroupnum, map_param);
	map<int32, int32>::iterator map_it = map_param.begin();
	/// ������Ʒ
	for (; map_it != map_param.end(); map_it++)
	{
		/// ָ������
		int32 nItemID = map_it->first;
		int32 nTmpProfession = -1;
		itemDropBase::getSingleton().getDropItemProfession(scenedrop.dropgroupid, nItemID, nTmpProfession);

		/// ����ӵ���Ʒ
		DropItems TmpDI;
		TmpDI.nDropItemID = nItemID;
		TmpDI.nDropItemNum = map_it->second;
		TmpDI.nDropItemProfession = nTmpProfession;

		if (0 == TmpDI.nDropItemID)
		{
			continue;
		}

		mapDropItems::iterator map_it_DropItem = m_mapdropItemsBindWave.find(scenedrop.dropwave);
		if (map_it_DropItem == m_mapdropItemsBindWave.end())
		{
			SM_DropItemsBindMonster dropItem;
			dropItem.monster_wave = scenedrop.dropwave;
			dropItem.vec_itemids.push_back(TmpDI);

			m_mapdropItemsBindWave[scenedrop.dropwave] = dropItem;
		}
		else
		{
			map_it_DropItem->second.vec_itemids.push_back(TmpDI);
		}
	}
}
void monsterCreator::saveDropGroupItem(const scene_drop& scenedrop, int32 professional)
{
	// �ӵ�������ȡ��Ҫ�������Ʒ����Ʒ����
	map<int32, int32> map_param;
	itemDropBase::getSingleton().getDropItemNum(scenedrop.dropgroupid, scenedrop.dropgroupnum, map_param, professional);
	map<int32, int32>::iterator map_it = map_param.begin();
	/// ������Ʒ
	for (; map_it != map_param.end(); map_it++)
	{
		/// ָ������
		int32 nItemID = map_it->first;
		int32 nTmpProfession = -1;
		itemDropBase::getSingleton().getDropItemProfession(scenedrop.dropgroupid, nItemID, nTmpProfession);

		/// ����ӵ���Ʒ
		DropItems TmpDI;
		TmpDI.nDropItemID = nItemID;
		TmpDI.nDropItemNum = map_it->second;
		TmpDI.nDropItemProfession = nTmpProfession;

		if (0 == TmpDI.nDropItemID)
		{
			continue;
		}

		mapDropItems::iterator map_it_DropItem = m_mapdropItemsBindWave.find(scenedrop.dropwave);
		if (map_it_DropItem == m_mapdropItemsBindWave.end())
		{
			SM_DropItemsBindMonster dropItem;
			dropItem.monster_wave = scenedrop.dropwave;
			dropItem.vec_itemids.push_back(TmpDI);

			m_mapdropItemsBindWave[scenedrop.dropwave] = dropItem;
		}
		else
		{
			map_it_DropItem->second.vec_itemids.push_back(TmpDI);
		}
		
	}
}
/// ͨ�������ID��ȡ����Ĳ���
int32 monsterCreator::getMonsterWaveFromMonsterSceneID(const uint32& monstersceneid)
{
	int32 nWave = -1;
	if (NULL == m_scene)
	{
		return nWave;
	}

	MonsterIDMap::iterator vet_it_begin = m_scene->getMonsterItBegin();
	MonsterIDMap::iterator vet_it_end = m_scene->getMonsterItEnd();
	for (; vet_it_begin != vet_it_end; vet_it_begin++)
	{
		if (NULL != vet_it_begin->second && monstersceneid == vet_it_begin->second->getid())
		{
			nWave = vet_it_begin->second->getWave();
			break;
		}
	}

	return nWave;
}

// ��¼��ǰ��������������Ĺ�
void monsterCreator::recordMonster_fix()
{
	// �������м������
	for (int32 nLoop = 0; nLoop < m_fixwaveinfo.size(); nLoop++)
	{
		// ÿһ���еĹ������
		for (int32 nLoopMonster = 0; nLoopMonster < m_fixwaveinfo[nLoop].monsters.size(); nLoopMonster++)
		{
			std::vector<SM_monsterPare>::iterator vec_it = m_fixwaveinfo[nLoop].monsters.begin();
			for (; vec_it != m_fixwaveinfo[nLoop].monsters.end(); vec_it++)
			{
				// ��������¼
				recordMonsterNum(vec_it->monsterID, vec_it->monsterCount, 0, m_fixwaveinfo[nLoop].influenceWin);
			}
		}
	}
}

void monsterCreator::recordMonster()
{
	// ������ǰ����
	for(int32 nLoop = 0; nLoop < m_wavedata.size(); nLoop++)
	{
		// �������м������
		for (int32 nLoopMonster = 0; nLoopMonster < m_wavedata[nLoop].m_vecsmallwave.size(); nLoopMonster++)
		{
			// ÿһ���еĹ������
			vector<SM_monsterPare>::iterator vec_it = m_wavedata[nLoop].m_vecsmallwave[nLoopMonster].monsters.begin();
			for( ; vec_it != m_wavedata[nLoop].m_vecsmallwave[nLoopMonster].monsters.end(); vec_it++ )
			{
				/// �����������⴦��
				if(true != dropMonsterType(vec_it->monsterID))
				{
					// ��������¼
					recordMonsterNum(vec_it->monsterID, vec_it->monsterCount, m_wavedata[nLoop].m_bigwaveID, m_wavedata[nLoop].m_vecsmallwave[nLoopMonster].influenceWin);
					RecordChessSmallWaveCount(m_wavedata[nLoop].m_vecsmallwave[nLoopMonster].smallwaveID, vec_it->monsterCount);
				}
			}
		}
	}
}

// ��������¼
void monsterCreator::recordMonsterNum(int32 nMonsterID, int32 nCount, int32 nMonsterWave, int32& nFluence)
{
	/// ��һ��Ƿ�Ϊ�ѷ�
	if (true == IsFriendId(nMonsterID))
	{
		return;
	}

	// ���������Ϣ
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.find(nMonsterID);
	SM_DropItemsBindMonster dibm;
	if (map_it == m_mapdropItemsBindMonster.end())
	{
		vecDIBM vec_dibm;
		dibm.monster_wave = nMonsterWave;
		vec_dibm.push_back(dibm);
		vec_dibm.resize(nCount);
		m_mapdropItemsBindMonster[nMonsterID] = vec_dibm;
		vecDIBM::iterator vet_it = vec_dibm.begin();
		for (; vet_it != vec_dibm.end(); vet_it++)
		{
			if (-10 == vet_it->monster_wave)
			{
				vet_it->monster_wave = nMonsterWave;
			}
		}
	}
	else
	{
		int32 nSize = map_it->second.size() + nCount;
		map_it->second.resize(nSize);
		vecDIBM::iterator vet_it = map_it->second.begin();
		for (; vet_it != map_it->second.end(); vet_it++)
		{
			if (-10 == vet_it->monster_wave)
			{
				vet_it->monster_wave = nMonsterWave;
			}
		}
	}

	/// ��ǰ�����Ƿ�������
	if (1 == nFluence) { return; }

	mapMonsterWave::iterator map_MW = m_mapMonsterWave.find(nMonsterWave);
	if (map_MW == m_mapMonsterWave.end())
	{
		MonsterWaveKillStatus monsterWaveKillStatus;
		monsterWaveKillStatus.mMonsterTotal += nCount;
		m_mapMonsterWave[nMonsterWave] = monsterWaveKillStatus;
	}
	else
	{
		map_MW->second.mMonsterTotal += nCount;
	}
}

// ���ݹ������Ͷ�����Ʒ�ĵ�������
void monsterCreator::calcItemsDropRateFromMonsterType( vec_scene& vecSceneDrop )
{
	// ͨ������ĸ�����ȷ����Ʒ���ĸ�������
// 	mapdibm::iterator map_it	= m_mapdropItemsBindMonster.begin();
// 	for( ; map_it != m_mapdropItemsBindMonster.end(); map_it++ )
// 	{
// 		bool bFlag = false;
// 		MonsterType mt	= monsterBase::getSingleton().getMonsterType( map_it->first );
// 		// �������еĵ�����
// 		for( int32 nLoop = 0; nLoop < vecSceneDrop.size(); nLoop++ )
// 		{
// 			// �����ܵ������
// 			if( (int32)mt >= vecSceneDrop[nLoop].vec_monster_tr.size() )
// 			{
// 				mt = static_cast<MonsterType>(vecSceneDrop[nLoop].vec_monster_tr.size() - 1);
// 			}
// 			/// ��ͬ�Ĳ����ĸ��ʲ�һ��
// 			vecSceneDrop[nLoop].allmonsterrate += vecSceneDrop[nLoop].vec_monster_tr[(int32)mt].rate;
//
// 			if (false == bFlag)
// 			{
// 				int32 nTmpWave = map_it->second[0].monster_wave;
// 				/// ÿһ���Ĺ������
// 				map_wave_rate::iterator map_rate_it = every_wave_rate.find(nTmpWave);
// 				if (map_rate_it != every_wave_rate.end())
// 				{
// 					map_rate_it->second += vecSceneDrop[nLoop].vec_monster_tr[(int32)mt].rate;
// 				}
// 				else
// 				{
// 					every_wave_rate[nTmpWave] = vecSceneDrop[nLoop].vec_monster_tr[(int32)mt].rate;
// 				}
//  				bFlag = true;
// 			}
// 		}
// 	}
}

// ��ȡ������Ʒ��Ӧ�Ĺ�������
bool monsterCreator::getDropItemMonsterType(MonsterType& mtData, scene_drop vecSceneDrop, int32 nItemID, int32 nMonsterWave)
{
	// ���ֹ�����յ������
// 	int32 nRandMonsterTotalDrop = Helper::getRandomInField(0, vecSceneDrop.allmonsterrate);
// 	int32 nRandMonsterDrop = Helper::getRandomInField(0, every_wave_rate[nMonsterWave]);
// 	int32 nTmpAddRand = 0;
// 	int32 nTmpAddRandWave = 0;
// 	vec_mtr::iterator vec_it = vecSceneDrop.vec_monster_tr.begin();
// 	for( int32 nLoopMonsterType = 0; nLoopMonsterType < vecSceneDrop.vec_monster_tr.size(); nLoopMonsterType++ )
// 	{
// 		// �����ǰ����Ĺ���Ϊ0����ô���������Ĺ��ﲻ���е���
// 		if( 0 == vecSceneDrop.vec_monster_tr[nLoopMonsterType].rate )
// 		{
// 			continue;
// 		}
//
// 		// ͨ������ĸ�����ȷ����Ʒ���ĸ�������
// 		mapdibm::iterator map_it	= m_mapdropItemsBindMonster.begin();
// 		for( ; map_it != m_mapdropItemsBindMonster.end(); map_it++ )
// 		{
// 			/// ��ǰ�� 0 ����,����,���������
// 			// �ҳ���ǰ����������Ƿ�����Ʒ�����������ͬ
// 			MonsterType mt	= monsterBase::getSingleton().getMonsterType( map_it->first );
// 			if ((int32)mt >= vecSceneDrop.vec_monster_tr.size())
// 			{
// 				mt = static_cast<MonsterType>(vecSceneDrop.vec_monster_tr.size() - 1);
// 			}
//
// 			if (0 == nMonsterWave && vecSceneDrop.vec_monster_tr[nLoopMonsterType].monstertype == mt)
// 			{
// 				// �ۼӵ���
// 				nTmpAddRand += vecSceneDrop.vec_monster_tr[nLoopMonsterType].rate;
//
// 				if (nRandMonsterTotalDrop < nTmpAddRand)
// 				{
// 					// ���һ����Ʒ
// 					int32 nTmpProfession = -1;
// 					itemDropBase::getSingleton().getDropItemProfession(vecSceneDrop.dropgroupid, nItemID, nTmpProfession);
//
// 					int32 nDropRandMax = map_it->second.size() * DEFAULT_RAND_MAX_NUMBER;
// 					// ��Ʒ�䵽�ĸ����������
// 					int32 nRandDropMonster = Helper::getRandomInField(0, nDropRandMax);
// 					nRandDropMonster = (nRandDropMonster / 1000 + nRandDropMonster % 10) % map_it->second.size();
// 					DropItems TmpDI;
// 					TmpDI.nDropItemID = nItemID;
// 					TmpDI.nDropItemProfession = nTmpProfession;
// 					(map_it->second)[nRandDropMonster].vec_itemids.push_back(TmpDI);
// 					return true;
// 				}
// 			}
//
//
// 			/// ���ҳ���������һ����
// 			vecDIBM::iterator vetDIBM_it = map_it->second.begin();
// 			for (; vetDIBM_it != map_it->second.end(); vetDIBM_it++)
// 			{
// 				if (nMonsterWave == vetDIBM_it->monster_wave && vecSceneDrop.vec_monster_tr[nLoopMonsterType].monstertype == mt)
// 				{
// 					nTmpAddRandWave += vecSceneDrop.vec_monster_tr[nLoopMonsterType].rate;
// 					if (nRandMonsterDrop < nTmpAddRandWave)
// 					{
// 						/// ָ������
// 						int32 nTmpProfession = -1;
// 						itemDropBase::getSingleton().getDropItemProfession(vecSceneDrop.dropgroupid, nItemID, nTmpProfession);
//
// 						DropItems TmpDI;
// 						TmpDI.nDropItemID = nItemID;
// 						TmpDI.nDropItemProfession = nTmpProfession;
// 						vetDIBM_it->vec_itemids.push_back(TmpDI);
// 						return true;
// 					}
// 				}
// 			}
// 		}
// 	}
	return false;
}

// ������Ʒ�󶨵���������
void monsterCreator::dropItemBindMonster(MonsterType& mtTmp, scene_drop sdData)
{
	// ͨ������ĸ�����ȷ����Ʒ���ĸ�������
	mapdibm::iterator map_it	= m_mapdropItemsBindMonster.begin();
	for( ; map_it != m_mapdropItemsBindMonster.end(); map_it++ )
	{
		// �ҳ���ǰ����������Ƿ�����Ʒ�����������ͬ
		MonsterType mt	= monsterBase::getSingleton().getMonsterType( map_it->first );
		if( mtTmp == mt )
		{
			int32 nDropRandMax	= map_it->second.size() * DEFAULT_RAND_MAX_NUMBER;
			// ��Ʒ�䵽�ĸ����������
			int32 nRandDropMonster	= Helper::getRandomInField(0, nDropRandMax);
			nRandDropMonster	= (nRandDropMonster / 1000 + nRandDropMonster % 10) % map_it->second.size();
			//(map_it->second)[nRandDropMonster].vec_itemids.push_back( sdData.dropgroupid );
		}
	}
}

// ɨ������
void monsterCreator::sweepFBDropItem( int32 nProfession, vector<ObjCountDrop>& vecDropItems )
{
	vec_scene vecscenedrop;
	// ��ȡ��ǰ������Ӧ�õ������Ʒ
	itemDropBase::getSingleton().getSceneDropData(m_scene->getFBTypeID(), vecscenedrop);

	// ������Ʒ
	for( int32 nLoopItems = 0; nLoopItems < vecscenedrop.size(); nLoopItems++ )
	{
		// ��ʲô������Ʒ��
		int32 nRandItem = Helper::getRandomInField(0, DEFAULT_RAND_MAX_NUMBER);
		if (vecscenedrop[nLoopItems].rate < nRandItem)
		{
			continue;
		}

		map<int32, int32> map_param;
		itemDropBase::getSingleton().getDropItemNum(vecscenedrop[nLoopItems].dropgroupid, vecscenedrop[nLoopItems].dropgroupnum, map_param);
		map<int32, int32>::iterator map_it = map_param.begin();
		for (; map_it != map_param.end(); map_it++)
		{
			int32 nTmpProfession = -1;
			itemDropBase::getSingleton().getDropItemProfession(vecscenedrop[nLoopItems].dropgroupid, map_it->first, nTmpProfession);
			// �жϵ�ǰְҵ�Ƿ�ƥ��
			if (-1 == nTmpProfession || nProfession == nTmpProfession)
			{
				ObjCountDrop dropitem;
				dropitem.objId = map_it->first;
				dropitem.count = map_it->second;
				vecDropItems.push_back(dropitem);
			}
		}
	}
}



// �жϹ����Ƿ����
bool monsterCreator::isKillAllMonster()
{
	// �Ƿ����������͵Ĺ�
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.begin();
	for (; map_it != m_mapdropItemsBindMonster.end(); map_it++)
	{
		vecDIBM& vecdibm = map_it->second;
		// ������ֻ��
		for (vecDIBM::iterator vec_it = vecdibm.begin(); vec_it != vecdibm.end(); vec_it++)
		{
			// �޸ı�ʶ��
			if (1 != vec_it->bDropFlag)
			{
				return false;
			}
		}
	}

	return true;
}

// ��ǰ�����м�ֻ��
int32 monsterCreator::getMonsterCount()
{
	int32 nCount = 0;
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.begin();
	for (; map_it != m_mapdropItemsBindMonster.end(); map_it++)
	{
		nCount = nCount + map_it->second.size();
	}
	return nCount;
}

// ����������Ϣ
void monsterCreator::getMonsterList(GM_Monster_list& pMonsterList)
{
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.begin();
	for (; map_it != m_mapdropItemsBindMonster.end(); map_it++)
	{
		GM_Monster_Info* pMonsterInfo = pMonsterList.add_monsterinfo();
		if (NULL != pMonsterInfo)
		{
			pMonsterInfo->set_monsterflag(map_it->first);
			pMonsterInfo->set_monstercount(map_it->second.size());
		}
	}
}

// ��ǰ������ɱ���˼�ֻ��
int32 monsterCreator::getKillMonsterCount()
{
	int32 nKillCount = 0;
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.begin();
	for (; map_it != m_mapdropItemsBindMonster.end(); map_it++)
	{
		for (int32 nLoopMonster = 0; nLoopMonster < map_it->second.size(); nLoopMonster++)
		{
			if (true == map_it->second[nLoopMonster].bDropFlag)
			{
				nKillCount++;
			}
		}
	}
	return nKillCount;
}


///--------------------------------------------------------------------------------------------------
/// ��Ȩ�ؼ���
int32 monsterCreator::calcTotalWeight()
{
	m_nTotalWeight = 0;
	if (NULL == m_scene)
	{
		return m_nTotalWeight;
	}

	/// ���㽱���Ļ��ұ���
	m_scene->coinRatioFromSceneType();

	// ͨ������ĸ�����ȷ����Ʒ���ĸ�������
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.begin();
	for (; map_it != m_mapdropItemsBindMonster.end(); map_it++)
	{
		// �ҳ���ǰ����������Ƿ�����Ʒ�����������ͬ
		MonsterType mt = monsterBase::getSingleton().getMonsterType(map_it->first);
		/// �ҳ���ǰ�������͵�Ȩ��
		drop_allot dropAllot;
		itemDropBase::getSingleton().findMonsterFBTypeToWeight(mt, m_scene->getBattleType(), dropAllot);
		/// ��ǰ�������͹ֵ�����Ȩ��
		int32 nMonsterTypeWeight = dropAllot.weight * map_it->second.size();
		/// ��������й����Ȩ��
		m_nTotalWeight += nMonsterTypeWeight;

		m_mapMonsterWeight[mt] = nMonsterTypeWeight;
	}
	return m_nTotalWeight;
}

///--------------------------------------------------------------------------------------------------
/// ����ĳһ����Ȩ��
void monsterCreator::calcWaveTotalWeight()
{
	m_nWaveWeight.clear();

	// ͨ������ĸ�����ȷ����Ʒ���ĸ�������
	mapdibm::iterator map_it = m_mapdropItemsBindMonster.begin();
	for (; map_it != m_mapdropItemsBindMonster.end(); map_it++)
	{
		// �ҳ���ǰ����������Ƿ�����Ʒ�����������ͬ
		MonsterType mt = monsterBase::getSingleton().getMonsterType(map_it->first);
		/// �ҳ���ǰ�������͵�Ȩ��
		drop_allot dropAllot;
		itemDropBase::getSingleton().findMonsterFBTypeToWeight(mt, m_scene->getBattleType(), dropAllot);

		vecDIBM::iterator vet_it = map_it->second.begin();
		for (; vet_it != map_it->second.end(); vet_it++)
		{
			mapMonsterWaveWeight::iterator map_weight_it = m_nWaveWeight.find(vet_it->monster_wave);
			if (map_weight_it != m_nWaveWeight.end())
			{
				map_weight_it->second.mWaveTotal += dropAllot.object_weight;
			}
			else
			{
				MonsterWaveWeight mww;
				mww.mWaveTotal = dropAllot.object_weight;
				m_nWaveWeight[vet_it->monster_wave] = mww;
			}
		}
	}
}

int32 monsterCreator::getWaveTotalWeight(int32 nWave)
{
	mapMonsterWaveWeight::iterator map_weight_it = m_nWaveWeight.find(nWave);
	if (map_weight_it != m_nWaveWeight.end())
	{
		return map_weight_it->second.mWaveTotal;
	}
	return 0;
}

int32 monsterCreator::getCurWaveWeight(int32 nWave)
{
	mapMonsterWaveWeight::iterator map_weight_it = m_nWaveWeight.find(nWave);
	if (map_weight_it != m_nWaveWeight.end())
	{
		return map_weight_it->second.mCurWaveCount;
	}
	return 0;
}

void monsterCreator::setWaveTotalWeight(int32 nWave, int32 nMonsterWeight, bool bFlag /*= true*/)
{
	mapMonsterWaveWeight::iterator map_weight_it = m_nWaveWeight.find(nWave);
	if (map_weight_it != m_nWaveWeight.end())
	{
		if (true == bFlag)
		{
			map_weight_it->second.mCurWaveCount += nMonsterWeight;
		}
		else
		{
			map_weight_it->second.mCurWaveCount = nMonsterWeight;
		}
	}
}


///--------------------------------------------------------------------------------------------------
/// ��ǰ�������Ľ�Ǯ
void monsterCreator::monsterDropCoin(const int32& nMonsterID, int32& nGold, int32& nDiamond)
{
	if (0 == m_nTotalWeight || NULL == m_scene)
	{
		return;
	}

	// �ҳ���ǰ����������Ƿ�����Ʒ�����������ͬ
	MonsterType mt = monsterBase::getSingleton().getMonsterType(nMonsterID);
	/// �ҳ���ǰ�������͵�Ȩ��
	drop_allot dropAllot;
	itemDropBase::getSingleton().findMonsterFBTypeToWeight(mt, m_scene->getBattleType(), dropAllot);

	nGold = m_scene->getTotalGold();
	nDiamond = m_scene->getTotalDiamond();

	/// �ù������Ͷ�ӦȨ�� / ��Ȩ��
	float fTmpWeight = dropAllot.weight * 1.0f / m_nTotalWeight * 1.0f;
	/// �����ֲ�����Ǯ = �ù������Ͷ�ӦȨ�� / ��Ȩ�� * ���������ܽ�Ǯ
	nGold = ceil(fTmpWeight * (nGold * 1.0f));
	nDiamond = ceil(fTmpWeight * (nDiamond * 1.0f));
}
///--------------------------------------------------------------------------------------------------
/// ���������İٷֱȵ���
void monsterCreator::poleyMonsterDrop(const int32& nMonsterID, int32& nDiamond)
{
	// �ҳ���ǰ����������Ƿ�����Ʒ�����������ͬ
	MonsterType mt = monsterBase::getSingleton().getMonsterType(nMonsterID);
	/// �ҳ���ǰ�������͵�Ȩ��
	drop_allot dropAllot;
	itemDropBase::getSingleton().findMonsterFBTypeToWeight(mt, m_scene->getBattleType(), dropAllot);

	nDiamond = dropAllot.total_diamond_hit;
}

///--------------------------------------------------------------------------------------------------
/// ��¼��������
void monsterCreator::recordMonsterDie(const uint32& monsterSceneID)
{
	/// �ҳ������Ӧ�Ĳ�
	int32 nMonsterWave = getMonsterWaveFromMonsterSceneID(monsterSceneID);

	mapMonsterWave::iterator map_it = m_mapMonsterWave.find(nMonsterWave);
	if (map_it != m_mapMonsterWave.end())
	{
		map_it->second.mMonsterKillCount++;
	}
}

///--------------------------------------------------------------------------------------------------
/// �жϵ�ǰ���Ĺ����Ƿ�ȫ������
int32 monsterCreator::getFinishDieWave(const uint32& monsterSceneID)
{
	/// �ҳ������Ӧ�Ĳ�
	int32 nMonsterWave = getMonsterWaveFromMonsterSceneID(monsterSceneID);

	mapMonsterWave::iterator map_it = m_mapMonsterWave.find(nMonsterWave);
	if (map_it != m_mapMonsterWave.end())
	{
		if (map_it->second.mMonsterKillCount == map_it->second.mMonsterTotal)
		{
			return nMonsterWave;
		}
	}
	return -1;
}

///--------------------------------------------------------------------------------------------------
/// �жϵ�ǰ���Ĺ����Ƿ�ȫ������
bool monsterCreator::isWaveFinishDie()
{
	mapMonsterWave::iterator map_it = m_mapMonsterWave.begin();
	for (; map_it != m_mapMonsterWave.end(); map_it++)
	{
		if (map_it->second.mMonsterKillCount != map_it->second.mMonsterTotal)
		{
			return false;
		}
	}
	return true;
}

///--------------------------------------------------------------------------------------------------
/// ����Ȩ��
void monsterCreator::setMonsterWeight(int32 nParam1, bool bAdd /*= true*/)
{
	if (true == bAdd)
	{
		m_nMonsterWeight += nParam1;
	}
	else
	{
		m_nMonsterWeight = nParam1;
	}
}

int32 monsterCreator::getMonsterWeight()
{
	return m_nMonsterWeight;
}

///--------------------------------------------------------------------------------------------------
/// �����������⴦��
bool monsterCreator::dropMonsterType(uint32 monsterid)
{
	// �ҳ���ǰ����������Ƿ�����Ʒ�����������ͬ
	MonsterType mt = monsterBase::getSingleton().getMonsterType(monsterid);

	if (MT_MACHINE == mt || MT_ORE == mt || MT_VIPBOX == mt)
	{
		return true;
	}

	return false;
}

/// �������ݱ��浽��־
void monsterCreator::saveDropTestData(const DropItems& dItem)
{
	map<int32, int32>::iterator map_it = m_mapItem.find(dItem.nDropItemID);
	if (map_it == m_mapItem.end())
	{
		m_mapItem[dItem.nDropItemID] = dItem.nDropItemNum;
	}
	else
	{
		m_mapItem[dItem.nDropItemID] += dItem.nDropItemNum;
	}
}

/// ��ӡ
void monsterCreator::printfLogMessage()
{
	if (NULL != m_scene)
	{
		map<int32, int32>::iterator map_it = m_mapItem.begin();

		for (; map_it != m_mapItem.end(); map_it++)
		{
			xstring strSceneID = Helper::IntToString(m_scene->getFBTypeID());
			xstring strDropItemID = Helper::IntToString(map_it->first);
			xstring strDropItemNum = Helper::IntToString(map_it->second);
			xLogMessager::getSingleton().logMessage("����ID:" + strSceneID + "   ��ƷID:" + strDropItemID + "   ��Ʒ����:" + strDropItemNum);
		}
	}
}

///--------------------------------------------------------------------------------------------------

Point2d xmlHelper::getServerPosition(Point2d FTStartPoint, Point2d FTBoundsCenter, Point2d FTBoundsSize)
{
	Point2d ret = FTStartPoint - FTBoundsCenter + FTBoundsSize.extents();
	return ret;
}

bool xmlHelper::parseChar(xstring& data, rapidxml::xml_node<>* xmlhandle, const xstring& nodename)
{
	if (xmlhandle->first_attribute(nodename.c_str()) != NULL)
	{
		data = xmlhandle->first_attribute(nodename.c_str())->value();
		return true;
	}
	return false;
}

bool xmlHelper::parseInt(int32& data, rapidxml::xml_node<>* xmlhandle, const xstring& nodename)
{
	if (xmlhandle->first_attribute(nodename.c_str()) != NULL)
	{
		xstring strTmp = xmlhandle->first_attribute(nodename.c_str())->value();
		data = Helper::StringToInt(strTmp);
		return true;
	}
	return false;
}


bool xmlHelper::parse3DrectToPoint2D(rapidxml::xml_node<>* xmlhandle, const xstring& nodename, Point2d& Point)
{
	if (NULL == xmlhandle)
	{
		return false;
	}
	xstring data;
	if (parseChar(data, xmlhandle, nodename))
	{
		Helper::string_replace(data, "(", " ");
		Helper::string_replace(data, ")", " ");
		std::vector<xstring> stringvec;
		Helper::splitString(data, ",", stringvec);
		if (stringvec.size() == 3)
		{
			Point.x = Helper::StringTofloat(stringvec[0]);
			Point.y = Helper::StringTofloat(stringvec[2]);
			return true;
		}
	}
	return false;
}

bool xmlHelper::parseAngle(rapidxml::xml_node<>* xmlhandle, const xstring& nodename, float& fRoY)
{
	xstring data;
	if (parseChar(data, xmlhandle, nodename))
	{
		Helper::string_replace(data, "(", " ");
		Helper::string_replace(data, ")", " ");
		std::vector<xstring> stringvec;
		Helper::splitString(data, ",", stringvec);
		if (stringvec.size() == 3)
		{
			fRoY = Helper::StringTofloat(stringvec[1]);
			return true;
		}
	}
	return false;
}
