
#include "common_utils.h"

using namespace lake;

//ע�⣺���ַ���Ϊ��ʱ��Ҳ�᷵��һ�����ַ���  
void split(string& s, string& delim,vector< string >* ret)  
{  
    size_t last = 0;  
    size_t index=s.find_first_of(delim,last);  
    while (index!= string::npos)  
    {  
        ret->push_back(s.substr(last,index-last));
        last=index+1;  
        index=s.find_first_of(delim,last);  
    }  
    if (index-last>0)  
    {  
        ret->push_back(s.substr(last,index-last));  
    }  
}
//��ȡĳһ��ĳһ������
int GetDaysInMonth(int y,int m)
{
	if(m>12)
		return 0;
	int d;
	int day[]= {31,28,31,30,31,30,31,31,30,31,30,31};
	if (2==m)
	{
	    if((y%4==0&&y%100!=0)||(y%400==0)) 
		d = 29;
	    else 
		d = 28;
	}
	else
	{
	    d=day[m-1];

	}
	return d;
}
/** 
 * �� 1...n �У�ȡ m ������������� (�ݹ鷨)
 * 
 * ʹ�÷���������һ������Ϊ m ���������� b 
 * �磺 
 *  unsigned char srcvec[];//ԭʼ����
 *	vector<int> b; //��ʱ���
 *	vector<vector<int> > resvec; //��Ž��
 *	combine(srcvec.size(),2,b,srcvec,resvec); 
 * 
 * 
 * ԭ��: 
 * (1) ȷ�� m ��Ԫ�������ֵ �Ŀ���ֵ i (i ���� n~m ����һ��ֵ), ����� b[m-1] �� 
 * (2) �� m>1 ʱ������ (1) �з��� ȷ���� m-1 ��Ԫ�������ֵ �Ŀ���ֵ 
 * (3) �� m=1 ʱ���õ�һ����ϣ������� b �� 
 */  
void lake::combine_recur(int n,int m,vector<unsigned char> &b,unsigned char srcvec[],vector<vector<unsigned char> > &resvec){  
	//printf("combine_recur,srcvec=%d",*srcvec);
    for(int i=n; i>=m; i--){  
          
        b[m-1] = srcvec[i - 1];  
          
        if (m > 1){  
            combine_recur(i-1,m-1,b,srcvec,resvec);  
        } else {  
  
            // m == 1, ���һ�����  
            resvec.push_back(b);
        }  
    }
}
//��������ַ���
char* lake::rand_str(char *str,const int len)
{
    srand(time(NULL));
    int i;
    for(i=0;i<len;++i)
    {
         switch((rand()%3))
         {
	       case 1:
		       str[i]='A'+rand()%26;
		       break;
	      case 2:
			str[i]='a'+rand()%26;
			break;
	     default:
		       str[i]='0'+rand()%10;
		       break;
         }
    }
     str[++i]='\0';
    return str;
}
//ȥ����β�Ŀո�
void lake::trimlr(string &s)
{
	if(!s.empty())
	{
		s.erase(0,s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
}
//ȥ�����еĿո�
void lake::trim(string &s)
{
    unsigned int index = 0;
    if(!s.empty())
    {
        while((index = s.find(' ',index)) != string::npos)
        {
        	if(index < s.size()){
        		//printf("index=%d,size=%d\n",index,s.size());
            	s.erase(index,1);
            }else{
            	break;
            }
        }
    }
}
//���������������͸���,���ʹ�С����
bool lake::sortCardByType(Card a,Card b)
{
	if(a.getTypenum() != b.getTypenum()){
		return a.getTypenum() > b.getTypenum();
	}else{
		return a.getType() < b.getType();
	}
	return false;
}
//���������������������С,���ʹ�С����
bool lake::sortCardByBS(Card a,Card b)
{
	if(a.getFace() != b.getFace()){
		return a.getFace() > b.getFace();
	}else{
		return a.getType() < b.getType();
	}
	return false;
}
//������������������������ൽ��,�����С�����ʹ�С����
bool lake::sortCardByFN(Card a,Card b)
{
	if(a.getFacenum() != b.getFacenum()){
		return a.getFacenum() > b.getFacenum();
	}else{
		if(a.getFace() != b.getFace()){
			return a.getFace() > b.getFace();
		}else{
			return a.getType() < b.getType();
		}
	}
	return false;
}
//�齫��������������������,���� С��������
bool lake::sortMahjongCardByTypeV(MahjongCard a,MahjongCard b)
{
	if(a.getType() != b.getType()){
		return a.getType() < b.getType();
	}else{
		return a.getValue() < b.getValue();
	}
}
//�齫�������������������͸���,����,���� С��������
bool lake::sortMahjongCardByNumTypeV(MahjongCard a,MahjongCard b)
{
	if(a.getTypenum() != b.getTypenum()){
		return a.getTypenum() < b.getTypenum();
	}else{
		if(a.getType() != b.getType()){
			return a.getType() < b.getType();
		}else{
			return a.getValue() < b.getValue();
		}
	}
}

//���û�ȡ�˿�ԭʼֵ
void Card::setValue(unsigned char tvalue,unsigned char basevalue)
{
	this->value = tvalue;
	if(tvalue < 53)
	{
		this->face = (tvalue + basevalue)%13;
		this->type = ceil((float)tvalue/13);
	}
	else
	{
		this->face = 18;
		if(tvalue==53)//С��
		{
			this->type = 2;
		}else
		{
			this->type = 1;
		}
	}
}
unsigned char Card::getValue()
{
	return this->value;
}
//��ȡ����ֵ
unsigned char Card::getFace()
{
	return this->face;
}
//��������ֵ
void Card::setFace(unsigned char value)
{
	this->face = value;
}
//��ȡ����ֵ
unsigned char Card::getType()
{
	return this->type;
}
//���û�ȡ�˿�ͬ����ĸ���
void Card::setFacenum(unsigned char tvalue)
{
	this->facenum = tvalue;
}
unsigned char Card::getFacenum()
{
	return this->facenum;
}
//ͬ����ĸ�������һ��ֵ
void Card::incrbyFacenum(unsigned char tvalue)
{
	this->facenum += tvalue;
}
//���û�ȡ�˿�ͬ���͵ĸ���
void Card::setTypenum(unsigned char tvalue)
{
	this->typenum = tvalue;
}
unsigned char Card::getTypenum()
{
	return this->typenum;
}
//ͬ���͵ĸ�������һ��ֵ
void Card::incrbyTypenum(unsigned char tvalue)
{
	this->typenum += tvalue;
}



