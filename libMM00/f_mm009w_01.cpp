/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2020-04-08
Description: 仓库跨产线出库时数据倒灌到下产线
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 

/*<remark>========================================================= 
/// <summary>
/// 仓库跨产线出库时数据倒灌到下产线
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 


#endif
#if defined(_LINE_HR) || defined(_LINE_CR)


#endif
#if defined(_LINE_CR)


#endif
#if defined(_LINE_HP)


#endif

//外部函数声明
BM2_FUNCTION_IMPORT
int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	


BM2_FUNCTION_EXPORT 
int f_mm009w_01(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	
	/* 业务变量 */
	CString	datetime("");
	CString	cs_mat_no("");
	CString	cs_mat_kind("");
	CString	cs_mat_line_type("");
	CString	cs_factory_div("");
	CString	cs_to_stock_no("");
	CString cs_tc_mark_from("");
	CString cs_tc_mark_to("");

	/* 实体类定义 */
	#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
	CModel tmmsm01("TMMSM01");

	CModel tmmsm03("TMMSM03");
	CModel tmmsm04("TMMSM04");

	CModel tmmsm96("TMMSM96");
	#endif
	#if defined(_LINE_HR) || defined(_LINE_CR)
	CModel tmmhr01("TMMHR01");
	CModel tmmhr96("TMMHR96");
	#endif
	#if defined(_LINE_CR)
	CModel tmmcr01("TMMCR01");
	CModel tmmcr96("TMMCR96");
	#endif
	#if defined(_LINE_HP)
	CModel tmmhp01("TMMHP01");
	CModel tmmhp96("TMMHP96");

	CModel tmmhp02("TMMHP02");
	CModel tmmhp03("TMMHP03");

	#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("MM009W_01");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 MM009W_01 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 添加并设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
		}

		/* 获取输入参数 */
		cs_mat_no = bcls_rec->Tables["MM009W_01"].Rows[0]["MAT_NO"].ToString().Trim();
		cs_mat_kind = bcls_rec->Tables["MM009W_01"].Rows[0]["MAT_KIND"].ToString().Trim();
		cs_mat_line_type = bcls_rec->Tables["MM009W_01"].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();
		cs_factory_div = bcls_rec->Tables["MM009W_01"].Rows[0]["FACTORY_DIV"].ToString().Trim();
		cs_to_stock_no = bcls_rec->Tables["MM009W_01"].Rows[0]["TO_STOCK_NO"].ToString().Trim();
		cs_tc_mark_from = bcls_rec->Tables["MM009W_01"].Rows[0]["TC_MARK_FROM"].ToString().Trim();
		cs_tc_mark_to = bcls_rec->Tables["MM009W_01"].Rows[0]["TC_MARK_TO"].ToString().Trim();


		/* 查询材料主档 */
		if (cs_mat_kind.Trim() == "SM")
		{
#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
			tmmsm01["MAT_NO"] = cs_mat_no;
			tmmsm01.Query("MAT_NO");
			tmmsm01.TrimOrBlank();

			tmmsm96.CopyFrom(tmmsm01);
			tmmsm96["EVENT_ID"] = "WM27";
			tmmsm96["EVENT_LINE_TYPE"] = cs_mat_line_type;
			tmmsm96["SYSTEM_ID"] = "WM00";
			tmmsm96["FUNC_ID"] = s.svc_name;
			tmmsm96.TrimOrBlank();
			bcls_rec->Tables["MM0099"].Clear();
			tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
#endif
		}
		if (cs_mat_kind.Trim() == "HR")
		{
#if defined(_LINE_HR) || defined(_LINE_CR)
			tmmhr01["MAT_NO"] = cs_mat_no;
			tmmhr01.Query("MAT_NO");
			tmmhr01.TrimOrBlank();

			tmmhr96.CopyFrom(tmmhr01);
			tmmhr96["EVENT_ID"] = "WM27";
			tmmhr96["EVENT_LINE_TYPE"] = cs_mat_line_type;
			tmmhr96["SYSTEM_ID"] = "WM00";
			tmmhr96["FUNC_ID"] = s.svc_name;
			tmmhr96.TrimOrBlank();
			bcls_rec->Tables["MM0099"].Clear();
			tmmhr96.MergeTo(bcls_rec->Tables["MM0099"], false);
#endif

		}
		if (cs_mat_kind.Trim() == "CR")
		{
#if defined(_LINE_CR)
			tmmcr01["MAT_NO"] = cs_mat_no;
			tmmcr01.Query("MAT_NO");
			tmmcr01.TrimOrBlank();

			tmmcr96.CopyFrom(tmmcr01);
			tmmcr96["EVENT_ID"] = "WM27";
			tmmcr96["EVENT_LINE_TYPE"] = cs_mat_line_type;
			tmmcr96["SYSTEM_ID"] = "WM00";
			tmmcr96["FUNC_ID"] = s.svc_name;
			tmmcr96.TrimOrBlank();
			bcls_rec->Tables["MM0099"].Clear();
			tmmcr96.MergeTo(bcls_rec->Tables["MM0099"], false);

#endif

		}
		if (cs_mat_kind.Trim() == "HP")
		{
#if defined(_LINE_HP)
			tmmhp01["MAT_NO"] = cs_mat_no;
			tmmhp01.Query("MAT_NO");
			tmmhp01.TrimOrBlank();

			tmmhp96.CopyFrom(tmmhp01);
			tmmhp96["EVENT_ID"] = "WM27";
			tmmhp96["EVENT_LINE_TYPE"] = cs_mat_line_type;
			tmmhp96["SYSTEM_ID"] = "WM00";
			tmmhp96["FUNC_ID"] = s.svc_name;
			tmmhp96.TrimOrBlank();
			bcls_rec->Tables["MM0099"].Clear();
			tmmhp96.MergeTo(bcls_rec->Tables["MM0099"], false);

#endif
		}
		doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		Log::Trace("", __FUNCTION__, "HP 定制倒灌 0304 cs_mat_kind = [{0}] ",cs_mat_kind);
		Log::Trace("", __FUNCTION__, "HP 定制倒灌 0304 cs_tc_mark_from = [{0}] ", cs_tc_mark_from);
		Log::Trace("", __FUNCTION__, "HP 定制倒灌 0304 cs_tc_mark_to = [{0}] ", cs_tc_mark_to);

#pragma region 炼钢转厚板
		if (cs_mat_kind == "SM" && cs_tc_mark_from != "50" && cs_tc_mark_to == "50")  //炼钢->宽板
		{
			EPEX epex(&s, conn);
			/****** 厚板目的板坯电文 BEGIN 发送电文开始 *****/
			Log::Trace("", __FUNCTION__, "厚板目的板坯电文 BEGIN 发送电文开始 ");
			//发送板坯主档表&目的板坯表&板坯工序表的电文--目的板坯表
			int fetchRowCount_tmmsm03 = 0;
			// 初始化
			CString cs_tc_no = "00" + cs_tc_mark_to + "M2";

			/* 发送电文开始 */
			if (epex.Initialize(cs_tc_no) < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询目的板坯信息 */
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM TMMSM03 "
					" WHERE MAT_NO	= @tmmsm01.MAT_NO  ";
				break;
			}

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tmmsm01.MAT_NO", tmmsm01["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm03);
				tmmsm03.TrimOrBlank();
				
				if (epex.SetValue(fetchRowCount_tmmsm03, tmmsm03) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				fetchRowCount_tmmsm03++;
			}
			cmd_inq.Close();

			/* 发送电文 */
			if (fetchRowCount_tmmsm03 > 0)
			{
				if (epex.SendTele() < 0)
				{
					strcpy(s.msg, "电文发送失败。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			/* 释放 */
			epex.Uninitialize();
			/****** 厚板目的板坯电文 END 发送电文结束 *****/

			/****** 厚板板坯小工序电文 BEGIN 发送电文开始 *****/

			int fetchRowCount_tmmsm04 = 0;

			// 初始化
			cs_tc_no = "00" + cs_tc_mark_to + "M3";

			if (epex.Initialize(cs_tc_no) < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询目的板坯信息 */
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM TMMSM04 "
					" WHERE MAT_NO	= @tmmsm01.MAT_NO  ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tmmsm01.MAT_NO", tmmsm01["MAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tmmsm04);
				tmmsm04.TrimOrBlank();

				if (epex.SetValue(fetchRowCount_tmmsm04, tmmsm04) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				fetchRowCount_tmmsm04++;
			}
			cmd_inq.Close();

			/* 发送电文 */
			if (fetchRowCount_tmmsm04 > 0)
			{
				if (epex.SendTele() < 0)
				{
					strcpy(s.msg, "电文发送失败。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			/* 释放 */
			epex.Uninitialize();
			/****** 厚板板坯小工序电文 END 发送电文结束 *****/			
		}
#pragma endregion

#pragma region 厚板退炼钢
		if (cs_mat_kind == "SM" && cs_tc_mark_from == "50" && cs_tc_mark_to == "20")  //宽板->炼钢
		{
			EPEX epex2(&s, conn);
			/****** 厚板目的板坯电文 BEGIN 发送电文开始 *****/
			Log::Trace("", __FUNCTION__, "厚板目的板坯电文 BEGIN 发送电文开始 ");
			//发送板坯主档表&目的板坯表&板坯工序表的电文--目的板坯表
			int fetchRowCount_tmmsm03 = 0;

			// 初始化
			CString cs_tc_no = "00" + cs_tc_mark_to + "MB";

			/* 发送电文开始 */
			if (epex2.Initialize(cs_tc_no) < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询目的板坯信息 */
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
					"  FROM TMMSM03 "
					" WHERE MAT_NO	= @tmmsm01.MAT_NO  ";
				break;

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tmmsm01.MAT_NO", tmmsm01["MAT_NO"]);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm03);
					tmmsm03.TrimOrBlank();

					if (epex2.SetValue(fetchRowCount_tmmsm03, tmmsm03) < 0)
					{
						sprintf(s.msg, "发送电文失败，原因[%s]", epex2.GetMsg());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					fetchRowCount_tmmsm03++;
				}
				cmd_inq.Close();

				/* 发送电文 */
				if (fetchRowCount_tmmsm03 > 0)
				{
					if (epex2.SendTele() < 0)
					{
						strcpy(s.msg, "电文发送失败。");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				/* 释放 */
				epex2.Uninitialize();
				/****** 厚板目的板坯电文 END 发送电文结束 *****/

				/****** 厚板板坯小工序电文 BEGIN 发送电文开始 *****/

				int fetchRowCount_tmmsm04 = 0;

				// 初始化
				cs_tc_no = "00" + cs_tc_mark_to + "MC";

				if (epex2.Initialize(cs_tc_no) < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				/* 查询目的板坯信息 */
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = "SELECT * "
						"  FROM TMMSM04 "
						" WHERE MAT_NO	= @tmmsm01.MAT_NO  ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tmmsm01.MAT_NO", tmmsm01["MAT_NO"]);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					cmd_inq.Fetch(tmmsm04);
					tmmsm04.TrimOrBlank();

					if (epex2.SetValue(fetchRowCount_tmmsm04, tmmsm04) < 0)
					{
						sprintf(s.msg, "发送电文失败，原因[%s]", epex2.GetMsg());
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					fetchRowCount_tmmsm04++;
				}
				cmd_inq.Close();

				/* 发送电文 */
				if (fetchRowCount_tmmsm04 > 0)
				{
					if (epex2.SendTele() < 0)
					{
						strcpy(s.msg, "电文发送失败。");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				/* 释放 */
				epex2.Uninitialize();
				/****** 厚板板坯小工序电文 END 发送电文结束 *****/
			}
		}
#pragma endregion


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


