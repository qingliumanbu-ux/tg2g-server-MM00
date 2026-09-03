/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料跟踪事件管理参数顺序调整
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件管理参数顺序调整
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(mm0097a1f8_pro)                                         

int f_mm0097a1f8_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    

	/* 实体类定义 */
	CModel tmm0099("TMM0099");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");



		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmm0099.Reset();
			tmm0099.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmm0099.TrimOrBlank();

			/* 打印输入参数 */
			Log::Trace("", __FUNCTION__, "传入参数 tmm0099.EVENT_ID				= [{0}]", tmm0099["EVENT_ID"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0099.MAT_KIND				= [{0}]", tmm0099["MAT_KIND"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0099.EVENT_LINE_TYPE		= [{0}]", tmm0099["EVENT_LINE_TYPE"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 tmm0099.ITEM_ENAME			= [{0}]", tmm0099["ITEM_ENAME"].ToString());

			/* 检查输入参数合法性 */
			if (tmm0099["EVENT_ID"].ToString().Trim() == "")
			{
				strcpy(s.msg, "事件号不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmm0099["MAT_KIND"].ToString().Trim() == "")
			{
				strcpy(s.msg, "物料种类不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmm0099["EVENT_LINE_TYPE"].ToString().Trim() == "")
			{
				strcpy(s.msg, "产线类型不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmm0099["ITEM_ENAME"].ToString().Trim() == "")
			{
				strcpy(s.msg, "字段英文名不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 设置顺序号 */
			tmm0099["SEQ_NO"] = i + 1;

			Log::Trace("", __FUNCTION__, "设置顺序号 tmm0099.SEQ_NO		= [{0}]", tmm0099["SEQ_NO"].ToDecimal());

			/* 修改事件接口参数表信息 */
			tmm0099["REC_REVISOR"]			=	s.userid;
			tmm0099["REC_REVISE_TIME"]		=	datetime;				
			tmm0099.TrimOrBlank();
			tmm0099.Update( "SEQ_NO,"
							"REC_REVISOR,"
							"REC_REVISE_TIME",
							"EVENT_ID,MAT_KIND,EVENT_LINE_TYPE,ITEM_ENAME");

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

