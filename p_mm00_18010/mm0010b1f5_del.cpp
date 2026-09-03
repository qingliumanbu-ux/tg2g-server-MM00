/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    李婧昊
Version:    1.0
Date:       2018-07-12
Description: 外购料进料计划管理_材料删除
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 外购料进料计划管理_材料删除
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  
  

//外部函数声明

BM2F_ENTERACE(mm0010b1f5_del)

int f_mm0010b1f5_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

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

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmm0010.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmm0010.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "tmm0010[\"MAT_NO\"] = [{0}]", (const char*)tmm0010["MAT_NO"].ToString());

			/* 材料是否在当前档 */
			if (tmm0010.Query("MAT_NO") == false)
			{
				sprintf(s.msg, "外购材料[%s]不存在!", (const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			tmm0010.TrimOrBlank();

			/* 获取计划号 */
			tmm0011["DEMAND_PLAN_NO"] = tmm0010["DEMAND_PLAN_NO"];    //进料需求计划号

			/* 校验外购材料信息是否已确认 */
			if (tmm0010["AFFIRM_FLAG"].ToString().Trim() == "Y")
			{
				sprintf(s.msg, "材料号[%s]已确认，不能删除!", (const char*)tmm0010["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 修改材料信息 */
			tmm0010["REC_REVISOR"] = s.userid;               //记录修改责任者
			tmm0010["REC_REVISE_TIME"] = datetime;           //记录修改责任时刻
			tmm0010["DEMAND_PLAN_NO"] = " ";                 //进料需求计划号
			tmm0010.TrimOrBlank();
			tmm0010.Update( "DEMAND_PLAN_NO,"
							"REC_REVISOR,"
							"REC_REVISE_TIME",
							"MAT_NO");
		}

		/* 按进料计划号查询材料信息 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
						"  FROM TMM0010 "
						" WHERE DEMAND_PLAN_NO = @tmm0011.DEMAND_PLAN_NO "
						" ORDER BY REC_CREATE_TIME ASC ";
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmm0011.DEMAND_PLAN_NO", tmm0011["DEMAND_PLAN_NO"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm0010);
			tmm0010.TrimOrBlank();
		
			tmm0011["MAT_NUM"] = tmm0011["MAT_NUM"].ToDecimal() + 1;					//材料件数
			tmm0011["MAT_WT"]	= tmm0011["MAT_WT"].ToDecimal() + tmm0010["MAT_WT"].ToDecimal();      //材料重量
		}
		cmd_inq.Close();

		/* 修改进料计划信息表 */
		tmm0011["REC_REVISOR"]		= s.userid;
		tmm0011["REC_REVISE_TIME"] = datetime;
		tmm0011.TrimOrBlank();
		tmm0011.Update( "MAT_NUM,"
						"MAT_WT,"
						"REC_REVISOR,"
						"REC_REVISE_TIME",
						"DEMAND_PLAN_NO");

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

