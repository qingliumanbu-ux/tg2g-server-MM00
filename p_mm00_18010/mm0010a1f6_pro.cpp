/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-07-11
Description: 外购料信息管理_进料收池
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 外购料信息管理_进料收池
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
   
   

//外部函数声明

BM2F_ENTERACE(mm0010a1f6_pro)

int f_mm0010a1f6_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime(""); 
	CString	datetime_8("");
	CString	cs_plan_no_seq("");

	/* 实体类定义 */
	CModel tmm0010("TMM0010");
	CModel tmm0011("TMM0011");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{	
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		datetime_8 = datetime.Substring(0, 8);

		/* 设置进料计划号 8位当前日期+2位流水号 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS sqlstr Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = " SELECT LPAD(TO_CHAR(NVL(MAX(SUBSTR(DEMAND_PLAN_NO,9,2)),0)+1),2,'0')  "
							"  FROM TMM0011 "						  
						   " WHERE DEMAND_PLAN_NO	LIKE	@datetime_8 ||'%' ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("datetime_8", datetime_8);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())	//获取第1行记录
		{
			cs_plan_no_seq = cmd_inq.GetString(1).Trim();
		}
		cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "cs_plan_no_seq	= [{0}]", cs_plan_no_seq);
		tmm0011["DEMAND_PLAN_NO"] = datetime.Substring(0, 8) + cs_plan_no_seq;
		Log::Trace("", __FUNCTION__, "tmm0011.DEMAND_PLAN_NO	= [{0}]", tmm0011["DEMAND_PLAN_NO"].ToString());

		/* 计算总重量 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmm0010.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmm0010.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "传入参数 tmm0010.MAT_NO	= [{0}]", tmm0010["MAT_NO"].ToString());

			/* 检查输入参数合法性 */
			if (tmm0010["MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "材料号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 获取材料信息 */
			tmm0010.Query("MAT_NO");
			tmm0010.TrimOrBlank();	

			/* 校验外购材料信息是否已编入进料计划 */
			if (tmm0010["DEMAND_PLAN_NO"].ToString().Trim() != "")
			{
				sprintf(s.msg, "材料号[%s]已编入进料计划表，不能再次收池!", (const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			/* 外购材料信息校验和赋值 */
			if (tmm0010["MAT_LINE_TYPE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "目标库区[%s]是厂外库，不能编入进料计划!", (const char*)tmm0010["STOCK_NO_TO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 设置进料计划表的值 */
			tmm0011["MAT_WT"] = tmm0011["MAT_WT"].ToDecimal() + tmm0010["MAT_WT"].ToDecimal();    //材料重量
			tmm0011["RAW_ORIGIN"] = tmm0010["RAW_ORIGIN"];			//原料来源

			if (tmm0011["RAW_ORIGIN"].ToString().Trim() != tmm0010["RAW_ORIGIN"].ToString().Trim())
			{
				sprintf(s.msg, "原料来源不一致的材料[%s]，不能编入同一计划!", (const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmm0011["STOCK_NO_TO"] = tmm0010["STOCK_NO_TO"];			//TO库区号 目标库区
			if (tmm0011["STOCK_NO_TO"].ToString().Trim() != tmm0010["STOCK_NO_TO"].ToString().Trim())
			{
				sprintf(s.msg, "目标库区不一致的材料[%s]，不能编入同一计划!", (const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 修改进料计划号 */
			tmm0010["REC_REVISOR"] = s.userid;
			tmm0010["REC_REVISE_TIME"] = datetime;
			tmm0010["DEMAND_PLAN_NO"] = tmm0011["DEMAND_PLAN_NO"];
			tmm0010.Update( "DEMAND_PLAN_NO,"
							"REC_REVISOR,"
							"REC_REVISE_TIME",
							"MAT_NO");   
		}

		/* 新增进料计划表 */
		tmm0011["PLAN_STATUS"]		= "10";		//计划状态      10-编入计划
		tmm0011["IN_STORE_DATE"]	= "";		//入库计划日期
		tmm0011["MAT_NUM"]			= bcls_rec->Tables[0].Rows.get_Count();		//材料件数
		tmm0011["PLAN_MAKER"]		= s.userid;	//计划责任者
		tmm0011["PLAN_SEND_TIME"]	= "";		//计划发送时刻
		tmm0011["REC_CREATOR"]		= s.userid;	//记录创建责任者
		tmm0011["REC_CREATE_TIME"] = datetime;	//记录创建时刻
		tmm0011.TrimOrBlank();
		tmm0011.Insert();

		/* 返回进料计划号 */
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEMAND_PLAN_NO");		//进料计划号
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0][0] = tmm0011["DEMAND_PLAN_NO"];

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
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
