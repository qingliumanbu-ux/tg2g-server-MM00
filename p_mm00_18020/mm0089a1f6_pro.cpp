/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-12-12
Description: 材料标签信息维护
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 材料标签信息维护
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
 

//外部函数声明

BM2F_ENTERACE(mm0089a1f6_pro)                                         

int f_mm0089a1f6_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    
	CDecimal cd_count	= 0;

	/* 实体类定义 */
	CModel tmm0089("TMM0089");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		/* 维护表 */
		// 新增
		if(bcls_rec->Tables.IndexOf("MM0089A1_INS") >= 0)
		{
			for(int i = 0; i < bcls_rec->Tables["MM0089A1_INS"].Rows.get_Count(); i++)
			{
				tmm0089.Reset();
				tmm0089.MergeFrom(bcls_rec->Tables["MM0089A1_INS"].Rows[i]);
				tmm0089.TrimOrBlank();

				//Log::Info("",__FUNCTION__,"新增 tmm0089.IN_MAT_NO		= [{0}]",tmm0089["IN_MAT_NO"].ToString());	
				//Log::Info("",__FUNCTION__,"新增 tmm0089.UNIT_CODE		= [{0}]",tmm0089["UNIT_CODE"].ToString());	

				/* 检查输入参数合法性 */
				if(tmm0089["IN_MAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg,"入口材料号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				if(tmm0089["UNIT_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg,"机组不能为空。");
					throw CApplicationException(-1, s.msg, log.Location); 
				}
	

				/* 校验主键不能重复 */
	/*			if(tmm0089.QueryCount("IN_MAT_NO,UNIT_CODE,PLAN_NO") > 0)
				{
					strcpy(s.msg, "主键 入口材料号[" + tmm0089["IN_MAT_NO"].ToString() + "]机组[" + tmm0089["UNIT_CODE"].ToString() + "]计划号[" + tmm0089["PLAN_NO"].ToString() + "]重复!");
					throw CApplicationException(-1, s.msg, log.Location); 
				}*/


				/* 新增信息 */
				tmm0089["REC_CREATOR"]		= s.userid;   //记录创建责任者
				tmm0089["REC_CREATE_TIME"]	= datetime;   //记录创建时刻
 				tmm0089.TrimOrBlank();
				tmm0089.Insert();


			}
		}

		// 修改
		if(bcls_rec->Tables.IndexOf("MM0089A1_UPD") >= 0)
		{
			for(int i = 0; i < bcls_rec->Tables["MM0089A1_UPD"].Rows.get_Count(); i++)
			{
				tmm0089.Reset();
				tmm0089.MergeFrom(bcls_rec->Tables["MM0089A1_UPD"].Rows[i]);
				tmm0089.TrimOrBlank();

				Log::Info("", __FUNCTION__,"修改 tmm0089.IN_MAT_NO		= [{0}]",tmm0089["IN_MAT_NO"].ToString());	
				Log::Info("", __FUNCTION__, "修改 tmm0089.SAMPLE_LOT_NO	= [{0}]", tmm0089["SAMPLE_LOT_NO"].ToString());

				/* 检查输入参数合法性 */
				if(tmm0089["IN_MAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg,"入口材料号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				if(tmm0089["UNIT_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg,"机组不能为空。");
					throw CApplicationException(-1, s.msg, log.Location); 
				}
			

				///* 非全产线,校验处理规则需与全产线相同 */
				//if(tmm0089.EVENT_LINE_TYPE.Trim() != "00"						
				//&& tmm0089.QueryCount("IN_MAT_NO,UNIT_CODE,EVENT_PROC_WAY") < 0)  //新增的产线的业务规则与全产线不同
				//{
				//	strcpy(s.msg,"入口材料号[" + tmm0089["IN_MAT_NO"].ToString() + "]种类[" + tmm0089["UNIT_CODE"].ToString() + "]产线[" + tmm0089.EVENT_LINE_TYPE + "]处理规则[" + tmm0089.EVENT_PROC_WAY +"]必须于全产线相同!");
				//	throw CApplicationException(-1, s.msg, log.Location); 
				//}

				/* 修改信息 */
				tmm0089["REC_REVISOR"]			=	s.userid;
				tmm0089["REC_REVISE_TIME"]		=	datetime;
				tmm0089.TrimOrBlank();
				tmm0089.Update( "PROD_SHIFT_NO," 
								"PROD_SHIFT_GROUP,"
								"PROD_DATE,"
								"PROD_TIME,"
								"SAMPLE_LOT_NO,"
								"PRINT_FLAG,"
								"PRINT_DATE,"
								"PRINT_MAKER,"
								"PRINT_NUM,"
								"UNIT_CODE,"
								"PLAN_NO,"
								"REC_REVISOR,"
								"REC_REVISE_TIME",
								"IN_MAT_NO,SAMPLE_LOT_NO");
			}
		}
		
		// 删除
		if(bcls_rec->Tables.IndexOf("MM0089A1_DEL") >= 0)
		{
			for(int i = 0; i < bcls_rec->Tables["MM0089A1_DEL"].Rows.get_Count(); i++)
			{
				tmm0089.Reset();
				tmm0089.MergeFrom(bcls_rec->Tables["MM0089A1_DEL"].Rows[i]);
				tmm0089.TrimOrBlank();

				Log::Info("", __FUNCTION__,"删除 tmm0089.IN_MAT_NO		= [{0}]",tmm0089["IN_MAT_NO"].ToString());	
				Log::Info("", __FUNCTION__,"删除 tmm0089.SAMPLE_LOT_NO	= [{0}]", tmm0089["SAMPLE_LOT_NO"].ToString());

				/* 检查输入参数合法性 */
				if(tmm0089["IN_MAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg,"入口材料号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location); 
				}
				if (tmm0089["SAMPLE_LOT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg,"试批号不能为空。");
					throw CApplicationException(-1, s.msg, log.Location); 
				}

				/* 删除信息 */
				tmm0089.Delete("IN_MAT_NO,SAMPLE_LOT_NO"); 
			}
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

