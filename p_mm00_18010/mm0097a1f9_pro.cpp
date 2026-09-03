/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2020
Author:      178053
Version:     1.0
Date:        2020-05-06 15:01:22
Description: 物料跟踪事件管理归历史档
**************************************************/

//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件管理归历史档
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

  




  



BM2F_ENTERACE(mm0097a1f9_pro)


int f_mm0097a1f9_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel hmm0097("HMM0097");
	CModel hmm0099("HMM0099");
	CModel hmm009a("HMM009A");
	CModel hmm009b("HMM009B");

	CModel tmm0097("TMM0097");
	CModel tmm0099("TMM0099");
	CModel tmm009a("TMM009A");
	CModel tmm009b("TMM009B");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		tmm0097["EVENT_ID"]		= bcls_rec->Tables[0].Rows[0]["EVENT_ID"].ToString().Trim();
		tmm0097["MAT_KIND"]		= bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		tmm0097["EVENT_LINE_TYPE"] = bcls_rec->Tables[0].Rows[0]["EVENT_LINE_TYPE"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "rec tmm0097.EVENT_ID			= [{0}]", (const char*)tmm0097["EVENT_ID"].ToString());
		Log::Trace("", __FUNCTION__, "rec tmm0097.MAT_KIND			= [{0}]", (const char*)tmm0097["MAT_KIND"].ToString());
		Log::Trace("", __FUNCTION__, "rec tmm0097.EVENT_LINE_TYPE	= [{0}]", (const char*)tmm0097["EVENT_LINE_TYPE"].ToString());

		/* 检查输入参数合法性 */
		if (tmm0097["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg, "事件号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 判断事件是否存在 */
		if (tmm0097.Query("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE") == false)
		{
			strcpy(s.msg, "事件号不存在!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 新增历史事件表 */
		hmm0097.CopyFrom(tmm0097);
		hmm0097["REC_CREATOR"] = s.userid;			//记录创建责任者
		hmm0097["REC_CREATE_TIME"] = datetime;		//记录创建时刻
		hmm0097.TrimOrBlank();
		Log::Trace("", __FUNCTION__, "Insert hmm0097[\"EVENT_ID\"] = [{0}]", (const char*)hmm0097["EVENT_ID"].ToString());
		hmm0097.Insert();

		/* 删除当前事件表 */
		Log::Trace("", __FUNCTION__, "Delete tmm0097[\"EVENT_ID\"] = [{0}]", (const char*)tmm0097["EVENT_ID"].ToString());
		tmm0097.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");

		/* 查询事件参数表 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM TMM0099 "
						 "  WHERE EVENT_ID = @tmm0097.EVENT_ID "
						 "	  AND MAT_KIND = @tmm0097.MAT_KIND "
						 "	  AND EVENT_LINE_TYPE = @tmm0097.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,SEQ_NO ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0097.EVENT_ID", tmm0097["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0097.MAT_KIND", tmm0097["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0097.EVENT_LINE_TYPE", tmm0097["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			hmm0099.Reset();
			tmm0099.Reset();

			cmd_inq.Fetch(tmm0099);

			/* 新增历史事件参数表 */
			hmm0099.CopyFrom(tmm0099);
			hmm0099["REC_CREATOR"] = s.userid;			//记录创建责任者
			hmm0099["REC_CREATE_TIME"] = datetime;     //记录创建时刻
			hmm0099.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "Insert hmm0099[\"ITEM_ENAME\"] = [{0}]", (const char*)hmm0099["ITEM_ENAME"].ToString());
			hmm0099.Insert();

			/* 删除当前事件参数表 */
			Log::Trace("", __FUNCTION__, "Delete tmm0099[\"ITEM_ENAME\"] = [{0}]", (const char*)tmm0099["ITEM_ENAME"].ToString());
			tmm0099.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");
		}
		cmd_inq.Close();

		/* 查询事件电文配置表 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM TMM009A "
						 "  WHERE EVENT_ID = @tmm0097.EVENT_ID "
						 "	  AND MAT_KIND = @tmm0097.MAT_KIND "
						 "	  AND EVENT_LINE_TYPE = @tmm0097.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,TC_KEYVALUE ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0097.EVENT_ID", tmm0097["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0097.MAT_KIND", tmm0097["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0097.EVENT_LINE_TYPE", tmm0097["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			hmm009a.Reset();
			tmm009a.Reset();

			cmd_inq.Fetch(tmm009a);

			/* 新增历史事件电文配置表 */
			hmm009a.CopyFrom(tmm009a);
			hmm009a["REC_CREATOR"] = s.userid;			//记录创建责任者
			hmm009a["REC_CREATE_TIME"] = datetime;     //记录创建时刻
			hmm009a.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "Insert hmm009a[\"TC_NO\"] = [{0}]", (const char*)hmm009a["TC_NO"].ToString());
			hmm009a.Insert();

			/* 删除当前事件电文配置表 */
			Log::Trace("", __FUNCTION__, "Delete tmm009a[\"TC_NO\"] = [{0}]", (const char*)tmm009a["TC_NO"].ToString());
			tmm009a.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");
		}
		cmd_inq.Close();

		/* 查询事件抛帐字段配置表 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM TMM009B "
						 "  WHERE EVENT_ID = @tmm0097.EVENT_ID "
						 "	  AND MAT_KIND = @tmm0097.MAT_KIND "
						 "	  AND EVENT_LINE_TYPE = @tmm0097.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,THROW_KIND ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0097.EVENT_ID", tmm0097["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0097.MAT_KIND", tmm0097["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0097.EVENT_LINE_TYPE", tmm0097["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			hmm009b.Reset();
			tmm009b.Reset();

			cmd_inq.Fetch(tmm009b);

			/* 新增事件抛帐字段配置表 */
			hmm009b.CopyFrom(tmm009b);
			hmm009b["REC_CREATOR"] = s.userid;			//记录创建责任者
			hmm009b["REC_CREATE_TIME"] = datetime;     //记录创建时刻
			hmm009b.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "Insert hmm009b[\"THROW_KIND\"] = [{0}]", hmm009b["THROW_KIND"].ToString());
			hmm009b.Insert();

			/* 删除历史事件抛帐字段配置表 */
			Log::Trace("", __FUNCTION__, "Delete tmm009b[\"THROW_KIND\"] = [{0}]", tmm009b["THROW_KIND"].ToString());
			tmm009b.Delete("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE");
		}
		cmd_inq.Close();
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


