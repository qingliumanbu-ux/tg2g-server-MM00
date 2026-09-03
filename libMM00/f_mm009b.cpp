/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-08-23 08:47:19
Description: 根据抛账事件表,过滤不需抛成本账的数据
**************************************************/

#include "CDynaTable.h"
  
 

BM2_FUNCTION_EXPORT 
int f_mm009b(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	
	/* 业务变量 */
	CString	datetime("");
	int	differ_num = 0;

	/* 实体类定义 */
	CModel tmm0097("TMM0097");
	CModel tmm009b("TMM009B");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM009B");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM009B");
		}

		CDataTable dtEventData = bcls_rec->Tables["EVENT_DATA"];
		CDataTable dtOldMat = bcls_rec->Tables["OLDMM_TABLE"];
		CDataTable dtNewMat = bcls_rec->Tables["NEWMM_TABLE"];

		tmm0097["EVENT_ID"]		= dtEventData.Rows[0]["EVENT_ID"].ToString();
		tmm0097["MAT_KIND"]		= dtEventData.Rows[0]["MAT_KIND"].ToString();
		tmm0097["EVENT_LINE_TYPE"] = dtEventData.Rows[0]["EVENT_LINE_TYPE"].ToString();
		tmm0097["EVENT_PROC_WAY_3"] = dtEventData.Rows[0]["EVENT_PROC_WAY_3"].ToString();

		Log::Info("", __FUNCTION__, "根据事件抛账表的字段 校验字段值是否相同 tmm0097.EVENT_PROC_WAY_3 = [{0}]", tmm0097["EVENT_PROC_WAY_3"].ToString());

		/* 根据事件抛账表，校验新旧数据的字段是否一致 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						"   FROM TMM009B "
						"  WHERE THROW_KIND = 'AC' "
						"    AND EVENT_ID			= @tmm0097.EVENT_ID "
						"	 AND MAT_KIND			= @tmm0097.MAT_KIND "
						"	 AND EVENT_LINE_TYPE	= @tmm0097.EVENT_LINE_TYPE "
						"  ORDER BY EVENT_ID ASC,THROW_KIND ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tmm0097.EVENT_ID", tmm0097["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("tmm0097.MAT_KIND", tmm0097["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("tmm0097.EVENT_LINE_TYPE", tmm0097["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm009b);
			tmm009b.TrimOrBlank();

			Log::Info("", __FUNCTION__, "根据事件抛账表，校验新旧数据的字段是否一致 tmm009b.ITEM_ENAME = [{0}]", tmm009b["ITEM_ENAME"].ToString());

			/* 根据事件抛账表的字段在主档中存在，则校验该字段 */
			if (dtOldMat.Columns.Contains(tmm009b["ITEM_ENAME"].ToString()) == true
				&& dtNewMat.Columns.Contains(tmm009b["ITEM_ENAME"].ToString()) == true)
			{
				tmm009b.MergeTo(bcls_rec->Tables["MM009B"], false);
			}
				
		}
		cmd_inq.Close();

		/* 根据事件抛账表的字段 校验字段值是否相同 */
		for (int i = 0; i < dtOldMat.Rows.get_Count(); i++)
		{
			/* 事件为非修改类，则不做此校验 */
			if (tmm0097["EVENT_PROC_WAY_3"].ToString() != "1")
			{
				bcls_rec->Tables["OLDMM_TABLE"].Rows[i]["ARCHIVE_FLAG"] = "0";
				continue;
			}

			tmm009b.Reset();
			for (int j = 0; j < bcls_rec->Tables["MM009B"].Rows.get_Count(); j++)
			{
				tmm009b.MergeFrom(bcls_rec->Tables["MM009B"].Rows[j]);

				Log::Info("", __FUNCTION__, "根据事件抛账表的字段 校验字段值是否相同 tmm009b.ITEM_ENAME = [{0}]", tmm009b["ITEM_ENAME"].ToString());

				if (tmm009b["ITEM_KIND"].ToString().Trim() == "S")
				{
					Log::Info("", __FUNCTION__, "根据事件抛账表的字段S校验字段值是否相同 dtOldMat.ITEM_ENAME = [{0}]", dtOldMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToString());
					Log::Info("", __FUNCTION__, "根据事件抛账表的字段S校验字段值是否相同 dtNewMat.ITEM_ENAME = [{0}]", dtNewMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToString());
					if (dtOldMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToString() == dtNewMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToString())
					{
						differ_num = differ_num + 0;
					}
					else
					{
						differ_num = differ_num + 1;
					}
				}
				if (tmm009b["ITEM_KIND"].ToString().Trim() == "D")
				{
					Log::Info("", __FUNCTION__, "根据事件抛账表的字段D校验字段值是否相同 dtOldMat.ITEM_ENAME = [{0}]", dtOldMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToDecimal());
					Log::Info("", __FUNCTION__, "根据事件抛账表的字段D校验字段值是否相同 dtNewMat.ITEM_ENAME = [{0}]", dtNewMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToDecimal());
					if (dtOldMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToDecimal() == dtNewMat.Rows[i][tmm009b["ITEM_ENAME"].ToString()].ToDecimal())
					{
						differ_num = differ_num + 0;
					}
					else
					{
						differ_num = differ_num + 1;
					}
				}
				Log::Info("", __FUNCTION__, "根据事件抛账表的字段 校验字段值是否相同 differ_num = [{0}]", differ_num);
			}
			Log::Info("", __FUNCTION__, "222 根据事件抛账表的字段 校验字段值是否相同 differ_num = [{0}]", differ_num);

			if (differ_num > 0)
			{
				bcls_rec->Tables["OLDMM_TABLE"].Rows[i]["ARCHIVE_FLAG"] = "1";
			}
			else
			{
				bcls_rec->Tables["OLDMM_TABLE"].Rows[i]["ARCHIVE_FLAG"] = "0";
			}

		}

	l_end:
		doFlag = 0;

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


