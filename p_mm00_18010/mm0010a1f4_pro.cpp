/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-07-13
Description: 外购料信息管理_设定目标库区
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 外购料信息管理_设定目标库区
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  


BM2F_ENTERACE(mm0010a1f4_pro)

int f_mm0010a1f4_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0; 

	/* 业务变量 */
	CString	datetime("");
	CString	cs_stock_no_to("");  

	/* 实体类定义 */
	CModel tmm0010("TMM0010");
	CModel tsi0021("TSI0021");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tsi0021["STOCK_NO"] = bcls_rec->Tables[1].Rows[0]["STOCK_NO_TO"].ToString().Trim();			//目的库区

		Log::Trace("", __FUNCTION__, "传入参数 tsi0021.STOCK_NO		= [{0}]", tsi0021["STOCK_NO"].ToString());
		
		/* 检查输入参数合法性 */
		if (tsi0021["STOCK_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "目标库区不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 获取库区信息 */
		if (tsi0021.Query("STOCK_NO") == false)          //库区号
		{
			strcpy(s.msg, "库区在系统中没有维护，请先维护库区!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmm0010["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();			//材料号

			Log::Trace("", __FUNCTION__, "传入参数 tmm0010.MAT_NO		= [{0}]", tmm0010["MAT_NO"].ToString());

			/* 获取材料信息 */
			tmm0010.Query("MAT_NO");
			tmm0010.TrimOrBlank();

			/* 校验外购材料信息是否已编入进料计划 */
			if (tmm0010["DEMAND_PLAN_NO"].ToString().Trim() != "")        //进料需求计划号
			{
				sprintf(s.msg, "材料号[%s]已编入进料计划表，不能进行操作!", (const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 校验外购材料信息是否已确认 */
			if (tmm0010["AFFIRM_FLAG"].ToString().Trim() == "Y")
			{
				sprintf(s.msg, "材料号[%s]已确认，不能进行操作!", (const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 修改目的库区 */
			tmm0010["REC_REVISOR"]		= s.userid;
			tmm0010["REC_REVISE_TIME"] = datetime;
			tmm0010["STOCK_NO_TO"]		= tsi0021["STOCK_NO"];         //库区号
			tmm0010["MAT_LINE_TYPE"]	= tsi0021["MAT_LINE_TYPE"];    //材料产线类型
			tmm0010.Update( "STOCK_NO_TO,"                     //目标库区号
							"MAT_LINE_TYPE,"
							"REC_REVISOR,"
							"REC_REVISE_TIME",
							"MAT_NO");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

