/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 李婧昊
日期: 2012-6-21 16:02:23
功能: 从字符串中根据指定分隔符拆分数据
修改历史：
日期:________；修改人：________; 需求提出人________
变更内容:
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include
#include "stdafx.h"


#define zero 0.00001

/*<remark>=========================================================
/// <summary>
///  从字符串中根据指定分隔符拆分数据
/// <para>
///    功能叙述段落
//    前台传入的信息[101, 102, 103]
//    返回输出信息['101','102','103']

/// </para>
/// </summary>
/// <param name="bcls_rec">传入块  </param>
/// <param name="bcls_ret">返回块  </param>
/// <returns>返回：是否成功</returns>
===========================================================</remark>*/

// 
//v_in_str= 传入的字符串。
//v_spilit_flag=指定的分隔符。

BM2_FUNCTION_EXPORT
CString f_mm0015(CString v_in_str, CString v_spilit_flag, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_get_multi_value";                //定义函数英文名称  
	CString FunctionCname = "从字符串中根据指定分隔符拆分数据";              //定义函数中文名称

	/*Log::Trace("", __FUNCTION__, "IN== new address =TEST ");*/


	//程序用变量
	int   doFlag = 0;
	int   i = 0;
	int   find_id = 0;
	int   find_start = 0;
	int   str_len = 0;

	//CString v_in_str = ""; //传入的字符信息。
	//CString v_spilit_flag = ",";//分隔符号。
	CString v_out_str = ""; //返回的字符信息。
	CString v_out_str2 = ""; //返回的字符信息。
	CString v_out_str3 = ""; //返回的字符信息。

	CString str = "";
	CString str2 = "";
	CString str3 = "";

	try
	{
		//当前库区//STOCK_NO是前台传入的库区集合[101, 102, 103],需要进行拆分处理。
		//返回输出信息['101','102','103'] 

		v_out_str = " ";
		if (v_in_str.Trim() == "")
		{
			//LogTrace(1,1,"v_in_str[%s]是空，直接离开本函数",(const char*)v_in_str);
			//LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
			return v_out_str;
		}

		//根据陈添提醒，防止黑客，需要把一些特殊符号过滤掉。
		//，'  /*  */  -- 
		//===========on 2015-7-1 10:50:23
		//v_in_str = v_in_str.Replace(",", "");
		v_in_str = v_in_str.Replace("'", "");
		Log::Trace("", __FUNCTION__, "'==v_in_str[{0}] ,test。 ", v_in_str);
		v_in_str = v_in_str.Replace("/*", "");
		Log::Trace("", __FUNCTION__, "/*==v_in_str[{0}] ,test。 ", v_in_str);
		v_in_str = v_in_str.Replace("*/", "");
		Log::Trace("", __FUNCTION__, "*/==v_in_str[{0}] ,test。 ", v_in_str);
		v_in_str = v_in_str.Replace("--", "");
		Log::Trace("", __FUNCTION__, "--==v_in_str[{0}] ,test。 ", v_in_str);
		//v_in_str = v_in_str.Replace("or", "");
		//Log::Trace("", __FUNCTION__, "or ==v_in_str[{0}] ,test。 ", v_in_str);
		Log::Trace("", __FUNCTION__, "--防黑处理结果 ==>v_in_str[{0}]  ", v_in_str);

		/*
		//需要将[特殊符号]去掉后，判断一下是否有值。
		//制表符= \t  回车符 = \r 换行符= \n
		*/
		CString v_tmp = "";
		CString v_tmp2 = "";
		CString v_tmp3 = "";

		v_tmp = v_in_str.Replace("\t", "");
		v_tmp2 = v_tmp.Replace("\r", "");
		v_tmp3 = v_tmp2.Replace("\n", "");

		if (v_tmp3.Trim() == "")
		{//若信息]中，除去特殊符号后，没有内容了，则不作为查询条件。 

			Log::Trace("", __FUNCTION__, "v_tmp3[{0}] ,当前字段不不作为查询条件。 ", v_tmp3);
			v_out_str = " ";
			return v_out_str;
		}

		//需要将[制表符]去掉， 
		//制表符= \t  回车符 = \r 换行符= \n  
		//将制表符 替换成 空格
		str = v_in_str.Replace("\t", " ");

		find_start = 0;
		find_id = 0;
		find_id = str.Find(v_spilit_flag, find_start); //查找分隔符是[,]
		if (find_id < 0)
		{//从头找也没找到指定的分隔符,则说明只有一个值，直接处理后返回。

			//sprintf(s.msg,"v_in_str[%s]中不包括分隔符[%s]，直接离开本函数",(const char*)str,(const char*)v_spilit_flag);
			//throw CApplicationException(-1, s.msg, log.Location); 

			// 101--->返回的是  '101'
			v_out_str = "'" + str.Trim() + "'"; //v_out_str ='101' 
		}
		else
		{
			//存在分隔符。

			find_start = 0;
			find_id = 0;
			for (i = 1; i <= str.GetLength(); i++)
			{
				find_id = str.Find(v_spilit_flag, find_start); //查找分隔符是[,]
				////LogTrace(1,1,"111==i[%d]find_start[%d]find_id[%d]str2[%s]",i,find_start,find_id,(const char*)str2);

				if (find_id < 0)
				{
					str2 = str.Substring(find_start, str.GetLength() - find_start).Trim();
					//LogTrace(1,1,"final==i[%d]find_start[%d]find_id[%d]str2[%s]",i,find_start,find_id,(const char*)str2);
					str3 = str3 + ",'" + str2 + "' ";//添加一个 ['101']

					break;
				}
				str2 = str.Substring(find_start, find_id - find_start).Trim();
				find_start = find_id + 1;
				//LogTrace(1,1,"i[%d]find_start[%d]find_id[%d]str2[%s]",i,find_start,find_id,(const char*)str2);
				str3 = str3 + ",'" + str2 + "'"; //添加一个 [,'101'] 
			}
			//去掉第一个逗号。
			str_len = str3.GetLength();
			str3 = str3.Substring(1, str_len - 1).Trim(); //str3 ='101','102','103'

			v_out_str = str3;  //v_out_str ='101','102','103'
		}

		Log::Trace("", __FUNCTION__, "IN:v_in_str = [{0}] v_out_str =[{1}]", v_in_str, v_out_str);

		//过滤回车符，过滤换行符
		v_out_str2 = v_out_str.Replace("\r", "");
		v_out_str3 = v_out_str2.Replace("\n", "");

		Log::Trace("", __FUNCTION__, "IN:v_in_str = [{0}] v_out_str3 =[{1}]", v_in_str, v_out_str3);

		//LogTrace(1,1,"===final v_in_str[%s],v_out_str[%s]===",(const char*)v_in_str,(const char*)v_out_str);
	}

	catch (CApplicationException& cae)
	{
		doFlag = cae.GetCode();
		strcpy(s.msg, (const char*)cae.GetMsg());
	}
	catch (CDbException& cde)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)cde.GetMsg());
	}
	catch (CException& ce)
	{
		doFlag = ce.GetCode();
		strcpy(s.msg, (const char*)ce.GetMsg());
	}

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	//LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);

	s.flag = doFlag;
	//bcls_ret->SetSYS(s);
	return v_out_str3;
}