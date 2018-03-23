#ifndef __COMMON_UTILS_H__
/*
desc:���õ����ݽṹ
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


//ע�⣺���ַ���Ϊ��ʱ��Ҳ�᷵��һ�����ַ���
extern void split(string& s,string& delim,vector< string >* ret);
//��ȡĳһ��ĳһ������
extern int GetDaysInMonth(int y,int m);
template<class T>
//int ����תstring
string Int_to_String(const T &t);
template<class T>
//��ȡ�����賿p��ʱ���(s)
unsigned int getMorningTime(const T &p);
//int ����תstring
template<class T>
string Int_to_String(const T &t)
{
	ostringstream stream;
	stream<<t;
	return stream.str();
}
//��ȡ�����賿p��,m�֣�s��ʱ���(s)
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
	//����
	class Card
	{
		private:
			//���棬���� 1��2,3��4,5,6 ...
			unsigned char face;
			//����  ������� ���� ÷�� ���� 1,2,3,4
			unsigned char type;
			//�˿�ԭʼֵ 1 - 54
			unsigned char value;
			//ͬ����ĸ���
			unsigned char facenum;
			//ͬ���͵ĸ���
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
			//���û�ȡ�˿�ԭʼֵ
			//basevalue ��ͬ����Ϸ���ò�ͬ��ֵ��
			//���綷����������3��С������basevalue=10;
			//����ţţ��1��С������basevalue=13;
			void setValue(unsigned char tvalue,unsigned char basevalue = BASEVALUE_DDZ);
			unsigned char getValue();
			//��ȡ����ֵ
			unsigned char getFace();
			//��������ֵ
			void setFace(unsigned char value);
			//��ȡ����ֵ
			unsigned char getType();
			//���û�ȡ�˿�ͬ����ĸ���
			void setFacenum(unsigned char tvalue);
			unsigned char getFacenum();
			//ͬ����ĸ�������һ��ֵ
			void incrbyFacenum(unsigned char tvalue);
			//���û�ȡ�˿�ͬ���͵ĸ���
			void setTypenum(unsigned char tvalue);
			unsigned char getTypenum();
			//ͬ���͵ĸ�������һ��ֵ
			void incrbyTypenum(unsigned char tvalue);
			~Card()
			{
				
			}
	};
	//�齫������
	enum MahjongCardType
	{
		CARD_TYPE_UNKNOW = 0,			//δ֪��
		CARD_TYPE_SUIT_CHARACTER = 1,	//����   һ��~����
		CARD_TYPE_SUIT_DOT = 2,			//Ͳ�� һͲ~��Ͳ
		CARD_TYPE_SUIT_BAMBOO = 3,		//���� һ��~����
		CARD_TYPE_HONOR = 4,			//���� ���������з���
		CARD_TYPE_FLOWER = 5,			//���� �����ﶬ÷�����
	};
	/*�齫��
	m_value
	1��9	   1��9�� 1,2,3,4,5,6,7,8,9
	10��18 1Ͳ��9Ͳ 10,11,12,13,14,15,16,17,18
	19��27 1����9�� 19,20,21,22,23,24,25,26,27
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
			//ͬ��ֵ�ĸ�������һ��ֵ
			void incrbyValueNum(unsigned char tvalue)
			{
				m_valuenum += tvalue;
			}
			//ͬ��ֵ�ĸ�������һ��ֵ
			void incrbyTypeNum(unsigned char tvalue)
			{
				m_typenum += tvalue;
			}
		private:
			unsigned char m_type;//������
			unsigned char m_typenum;//ͬ�����͵ĸ���
			unsigned char m_rank;
			unsigned char m_num;
			unsigned char m_value;//��ԭʼֵ
			unsigned char m_valuenum;//ͬvalue�ĸ���
	};
	//���������������͸���,���ʹ�С����
	extern bool sortCardByType(Card a,Card b);
	//���������������������С,���ʹ�С����
	extern bool sortCardByBS(Card a,Card b);
	//������������������������ൽ��,�����С�����ʹ�С����
	extern bool sortCardByFN(Card a,Card b);
	//�齫��������������������,���� С��������
	extern bool sortMahjongCardByTypeV(MahjongCard a,MahjongCard b);
	//�齫�������������������͸���,����,���� С��������
	extern bool sortMahjongCardByNumTypeV(MahjongCard a,MahjongCard b);
	//��������ַ���
	extern char *rand_str(char *str,const int len);
	//�� 1...n �У�ȡ m ������������� (�ݹ鷨)
	extern void combine_recur(int n,int m,vector<unsigned char> &b,unsigned char srcvec[],vector<vector<unsigned char> > &resvec);
	//����������
	template<class A,class B>
	void swap(A &a,B &b)
	{
		a = a + b;
		b = a - b;
		a = a - b;
	}
	//ȥ����β�Ŀո�
	void trimlr(string &s);
	//ȥ�����еĿո�
	void trim(string &s);
}

#endif
