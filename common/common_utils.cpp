
#include "common_utils.h"

using namespace lake;

//注意：当字符串为空时，也会返回一个空字符串  
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
//获取某一年某一月天数
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
 * 从 1...n 中，取 m 个数的所有组合 (递归法)
 * 
 * 使用方法：申明一个长度为 m 的整型向量 b 
 * 如： 
 *  unsigned char srcvec[];//原始数组
 *	vector<int> b; //临时结果
 *	vector<vector<int> > resvec; //存放结果
 *	combine(srcvec.size(),2,b,srcvec,resvec); 
 * 
 * 
 * 原理: 
 * (1) 确定 m 个元素中最大值 的可能值 i (i 等于 n~m 其中一个值), 存放在 b[m-1] 处 
 * (2) 当 m>1 时，按照 (1) 中方法 确定第 m-1 个元素中最大值 的可能值 
 * (3) 当 m=1 时，得到一个组合，在向量 b 中 
 */  
void lake::combine_recur(int n,int m,vector<unsigned char> &b,unsigned char srcvec[],vector<vector<unsigned char> > &resvec){  
	//printf("combine_recur,srcvec=%d",*srcvec);
    for(int i=n; i>=m; i--){  
          
        b[m-1] = srcvec[i - 1];  
          
        if (m > 1){  
            combine_recur(i-1,m-1,b,srcvec,resvec);  
        } else {  
  
            // m == 1, 输出一个组合  
            resvec.push_back(b);
        }  
    }
}
//生成随机字符串
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
//去除首尾的空格
void lake::trimlr(string &s)
{
	if(!s.empty())
	{
		s.erase(0,s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
}
//去除所有的空格
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
//牌类排序函数，牌型个数,牌型大到小排序
bool lake::sortCardByType(Card a,Card b)
{
	if(a.getTypenum() != b.getTypenum()){
		return a.getTypenum() > b.getTypenum();
	}else{
		return a.getType() < b.getType();
	}
	return false;
}
//牌类排序函数，根据牌面大小,牌型大到小排序
bool lake::sortCardByBS(Card a,Card b)
{
	if(a.getFace() != b.getFace()){
		return a.getFace() > b.getFace();
	}else{
		return a.getType() < b.getType();
	}
	return false;
}
//牌类排序函数，根据牌面个数多到少,牌面大小，牌型大到小排序
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
//麻将牌类排序函数，根据类型,牌型 小到大排序
bool lake::sortMahjongCardByTypeV(MahjongCard a,MahjongCard b)
{
	if(a.getType() != b.getType()){
		return a.getType() < b.getType();
	}else{
		return a.getValue() < b.getValue();
	}
}
//麻将牌类排序函数，根据类型个数,类型,牌型 小到大排序
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

//设置获取扑克原始值
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
		if(tvalue==53)//小王
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
//获取牌面值
unsigned char Card::getFace()
{
	return this->face;
}
//设置牌面值
void Card::setFace(unsigned char value)
{
	this->face = value;
}
//获取牌型值
unsigned char Card::getType()
{
	return this->type;
}
//设置获取扑克同牌面的个数
void Card::setFacenum(unsigned char tvalue)
{
	this->facenum = tvalue;
}
unsigned char Card::getFacenum()
{
	return this->facenum;
}
//同牌面的个数加上一个值
void Card::incrbyFacenum(unsigned char tvalue)
{
	this->facenum += tvalue;
}
//设置获取扑克同牌型的个数
void Card::setTypenum(unsigned char tvalue)
{
	this->typenum = tvalue;
}
unsigned char Card::getTypenum()
{
	return this->typenum;
}
//同牌型的个数加上一个值
void Card::incrbyTypenum(unsigned char tvalue)
{
	this->typenum += tvalue;
}



