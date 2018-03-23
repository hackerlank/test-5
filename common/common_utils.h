#ifndef __COMMON_UTILS_H__
/*
desc:共用的数据结构
writer:Lake
*/
#define __COMMON_UTILS_H__

#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

using namespace std;


//注意：当字符串为空时，也会返回一个空字符串
extern void split(string& s,string& delim,vector< string >* ret);
//获取某一年某一月天数
extern int GetDaysInMonth(int y,int m);
template<class T>
//int 类型转string
string Int_to_String(const T &t);
template<class T>
//获取当天凌晨p点时间戳(s)
unsigned int getMorningTime(const T &p);
//int 类型转string
template<class T>
string Int_to_String(const T &t)
{
	ostringstream stream;
	stream<<t;
	return stream.str();
}
//获取当天凌晨p点,m分，s秒时间戳(s)
template<class T,class M,class S>
unsigned int getMorningTime(const T &p,const M &m,const S &s)
{
	time_t t = time(NULL);   // get time now
   	struct tm * zeroTm= localtime(&t);
   	zeroTm->tm_hour = p;
   	zeroTm->tm_min = m;
   	zeroTm->tm_sec = s;
   	unsigned long long zeroTime = mktime(zeroTm);
   	return zeroTime;
}

//lake
namespace lake{
	enum
	{
		BASEVALUE_DDZ = 10,
		BASEVALUE_NN = 13,
		BASEVALUE_TEXAS = 13
	};
	//牌类
	class Card
	{
		private:
			//牌面，比如 1，2,3，4,5,6 ...
			unsigned char face;
			//牌型  比如黑桃 红桃 梅花 方块 1,2,3,4
			unsigned char type;
			//扑克原始值 1 - 54
			unsigned char value;
			//同牌面的个数
			unsigned char facenum;
			//同牌型的个数
			unsigned char typenum;
		public:
			Card()
			{
				face = 0;
				type = 0;
				type = 0;
				facenum = 0;
				typenum = 0;
			}
			//设置获取扑克原始值
			//basevalue 不同的游戏设置不同的值，
			//比如斗地主，牌面3最小，设置basevalue=10;
			//比如牛牛，1最小，设置basevalue=13;
			void setValue(unsigned char tvalue,unsigned char basevalue = BASEVALUE_DDZ);
			unsigned char getValue();
			//获取牌面值
			unsigned char getFace();
			//设置牌面值
			void setFace(unsigned char value);
			//获取牌型值
			unsigned char getType();
			//设置获取扑克同牌面的个数
			void setFacenum(unsigned char tvalue);
			unsigned char getFacenum();
			//同牌面的个数加上一个值
			void incrbyFacenum(unsigned char tvalue);
			//设置获取扑克同牌型的个数
			void setTypenum(unsigned char tvalue);
			unsigned char getTypenum();
			//同牌型的个数加上一个值
			void incrbyTypenum(unsigned char tvalue);
			~Card()
			{
				
			}
	};
	//麻将牌类型
	enum MahjongCardType
	{
		CARD_TYPE_UNKNOW = 0,			//未知牌
		CARD_TYPE_SUIT_CHARACTER = 1,	//万牌   一万~九万
		CARD_TYPE_SUIT_DOT = 2,			//筒牌 一筒~九筒
		CARD_TYPE_SUIT_BAMBOO = 3,		//条牌 一条~九条
		CARD_TYPE_HONOR = 4,			//字牌 东南西北中发白
		CARD_TYPE_FLOWER = 5,			//花牌 春夏秋冬梅兰竹菊
	};
	/*麻将类
	m_value
	1到9	   1万到9万 1,2,3,4,5,6,7,8,9
	10到18 1筒到9筒 10,11,12,13,14,15,16,17,18
	19到27 1条到9条 19,20,21,22,23,24,25,26,27
	*/
	class MahjongCard
	{
		public:
			MahjongCard() :m_type(0),m_typenum(0), m_rank(0), m_num(0), m_value(0), m_valuenum(0)
			{
			}

			MahjongCard(unsigned char type, unsigned char rank) :
				m_type(type), m_rank(rank), m_num(1)
			{
			}

			~MahjongCard()
			{
			}
			unsigned char getType(){ return m_type;}
			unsigned char getRank(){ return m_rank;}
			unsigned char getNum(){ return m_num;}
			void setValue(unsigned char value)
			{
				m_value = value;
				m_type = ceil((float)m_value/9);
			}
			unsigned char getValue(){return m_value;}
			void setValuenum(unsigned char value)
			{
				m_valuenum = value;
			}
			unsigned char getValuenum(){ return m_valuenum;}
			void setTypenum(unsigned char value)
			{
				m_typenum = value;
			}
			unsigned char getTypenum(){ return m_typenum;}
			void Add(unsigned char num = 1)
			{
				m_num += num;
			};
			void Del(unsigned char num = 1)
			{
				m_num -= num;
			};
			//同数值的个数加上一个值
			void incrbyValueNum(unsigned char tvalue)
			{
				m_valuenum += tvalue;
			}
			//同数值的个数加上一个值
			void incrbyTypeNum(unsigned char tvalue)
			{
				m_typenum += tvalue;
			}
		private:
			unsigned char m_type;//牌类型
			unsigned char m_typenum;//同牌类型的个数
			unsigned char m_rank;
			unsigned char m_num;
			unsigned char m_value;//牌原始值
			unsigned char m_valuenum;//同value的个数
	};
	//牌类排序函数，牌型个数,牌型大到小排序
	extern bool sortCardByType(Card a,Card b);
	//牌类排序函数，根据牌面大小,牌型大到小排序
	extern bool sortCardByBS(Card a,Card b);
	//牌类排序函数，根据牌面个数多到少,牌面大小，牌型大到小排序
	extern bool sortCardByFN(Card a,Card b);
	//麻将牌类排序函数，根据类型,牌型 小到大排序
	extern bool sortMahjongCardByTypeV(MahjongCard a,MahjongCard b);
	//麻将牌类排序函数，根据类型个数,类型,牌型 小到大排序
	extern bool sortMahjongCardByNumTypeV(MahjongCard a,MahjongCard b);
	//生成随机字符串
	extern char *rand_str(char *str,const int len);
	//从 1...n 中，取 m 个数的所有组合 (递归法)
	extern void combine_recur(int n,int m,vector<unsigned char> &b,unsigned char srcvec[],vector<vector<unsigned char> > &resvec);
	//两个数交换
	template<class A,class B>
	void swap(A &a,B &b)
	{
		a = a + b;
		b = a - b;
		a = a - b;
	}
	//去除首尾的空格
	void trimlr(string &s);
	//去除所有的空格
	void trim(string &s);
}

#endif
