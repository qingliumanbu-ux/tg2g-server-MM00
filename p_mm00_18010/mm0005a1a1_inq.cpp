/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料路径跟踪详细信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料路径跟踪详细信息查询
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


BM2F_ENTERACE(mm0005a1a1_inq) 

int f_mm0005a1a1_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */ 
	CString	datetime("");	
	CString	cs_in_mat_kind("");	
	CString	cs_yl_in_mat_id("");	
	CString	cs_yl_mat_no("");	
	CString	cs_in_mat_id("");	
	int fetchRowCount	= 0;
	
	/* 实体类定义 */ 
	CModel tmm0005("TMM0005");
	#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	#endif
	#if defined(_LINE_HR) || defined(_LINE_CR)
	CModel tmmhr01("TMMHR01");
	CModel hmmhr01("HMMHR01");
	#endif
	#if defined(_LINE_CR)
	CModel tmmcr01("TMMCR01");
	CModel hmmcr01("HMMCR01");
	#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss"); 

		Log::Info("",__FUNCTION__,"传入参数 11111 tmm0005.MAT_TRACK_NO	= [{0}]",tmm0005["MAT_TRACK_NO"].ToString());
		/* 获取输入参数 */
		tmm0005["MAT_TRACK_NO"]	= bcls_rec->Tables[0].Rows[0]["MAT_TRACK_NO"].ToString().Trim();		

		/* 打印输入参数 */
		Log::Info("",__FUNCTION__,"传入参数222  tmm0005.MAT_TRACK_NO	= [{0}]",tmm0005["MAT_TRACK_NO"].ToString());

		/* 设置原料和成品块名 */
		bcls_ret->Tables.Add("YUANLIAO");	
		bcls_ret->Tables.Add("CHENGPIN");	
		//#if defined(_LINE_SM) && defined(_LINE_HR) && defined(_LINE_CR) &&  defined(_LINE_HP) && defined(_LINE_BW) 

		/* 按路径跟踪信息查询原料和成品 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT * "
						 "  FROM TMM0005 "
						 " WHERE MAT_TRACK_NO = @tmm0005.MAT_TRACK_NO "
						 " ORDER BY REC_CREATE_TIME ASC ";	
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmm0005.MAT_TRACK_NO", tmm0005["MAT_TRACK_NO"].ToString());
		Log::Trace("",__FUNCTION__,"MAT_TRACK_NO sqlstr	= [{0}]",sqlstr);
		cmd_inq.ExecuteReader();
		while(cmd_inq.Read())	
		{
			cmd_inq.Fetch(tmm0005);
			tmm0005.TrimOrBlank();

			Log::Trace("",__FUNCTION__,"while tmm0005.PASS_BACKLOG_SEQ_NO	= [{0}]",tmm0005["PASS_BACKLOG_SEQ_NO"].ToDecimal().ToInt32());
			Log::Trace("",__FUNCTION__,"while tmm0005.MAT_KIND				= [{0}]",tmm0005["MAT_KIND"].ToString());

			// 该跟踪号,第一次插入的数据
			if(fetchRowCount == 0)	// if(tmm0005["PASS_BACKLOG_SEQ_NO"].ToDecimal() == 1)
			{
				// 获取原料的材料号和入口材料号
				cs_yl_in_mat_id = tmm0005["IN_MAT_ID"];
				cs_yl_mat_no	= tmm0005["MAT_NO"];
				#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
				if(tmm0005["MAT_KIND"].ToString().Trim() == "SM")
				{
					tmmsm01["MAT_NO"]	= tmm0005["MAT_NO"];
					tmmsm01.Query("MAT_NO");
					tmmsm01.TrimOrBlank();
					if(tmmsm01["MAT_ID"].ToString().Trim() != "")
					{
						Log::Trace("",__FUNCTION__,"原料 SM tmmsm01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						tmmsm01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
					}
					else
					{
						hmmsm01["MAT_ID"]	= tmm0005["MAT_ID"];
						hmmsm01.Query("MAT_ID");
						hmmsm01.TrimOrBlank();
						Log::Trace("",__FUNCTION__,"原料 SM hmmsm01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						hmmsm01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
					}
				}
				#endif

				if(tmm0005["MAT_KIND"].ToString().Trim() == "HR")
				{
					#if defined(_LINE_HR) || defined(_LINE_CR)
					tmmhr01["MAT_NO"]	= tmm0005["MAT_NO"];
					tmmhr01.Query();
					tmmhr01.TrimOrBlank();
					if(tmmhr01["MAT_ID"].ToString().Trim() != "")
					{
						Log::Trace("",__FUNCTION__,"原料 HR tmmhr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						tmmhr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
					}
					else
					{
						hmmhr01["MAT_ID"]	= tmm0005["MAT_ID"];
						hmmhr01.Query("MAT_ID");
						hmmhr01.TrimOrBlank();
						Log::Trace("",__FUNCTION__,"原料 HR hmmhr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						hmmhr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
					}
					#endif
				}
				if(tmm0005["MAT_KIND"].ToString().Trim() == "CR")
				{
					#if defined(_LINE_CR)
					tmmcr01["MAT_NO"]	= tmm0005["MAT_NO"];
					tmmcr01.Query();
					tmmcr01.TrimOrBlank();
					if(tmmcr01["MAT_ID"].ToString().Trim() != "")
					{
						Log::Trace("",__FUNCTION__,"原料 CR tmmcr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						tmmcr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
					}
					else
					{
						hmmcr01["MAT_ID"]	= tmm0005["MAT_ID"];
						hmmcr01.Query("MAT_ID");
						hmmcr01.TrimOrBlank();
						Log::Trace("",__FUNCTION__,"原料 CR hmmcr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						hmmcr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
					}
					#endif					
				}
			}
				
			// 成品
			#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
			if(tmm0005["MAT_KIND"].ToString().Trim() == "SM")
			{
				tmmsm01["MAT_NO"]	= tmm0005["MAT_NO"];
				tmmsm01.Query("MAT_NO");
				tmmsm01.TrimOrBlank();
				if(tmmsm01["MAT_ID"].ToString().Trim() != "")
				{
					if(tmmsm01["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						Log::Trace("",__FUNCTION__,"成品 SM tmmsm01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						tmmsm01.MergeTo(bcls_ret->Tables["CHENGPIN"],false);
					}
				}
				else
				{
					hmmsm01["MAT_ID"]	= tmm0005["MAT_ID"];
					hmmsm01.Query("MAT_ID");
					hmmsm01.TrimOrBlank();
					if(hmmsm01["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						Log::Trace("",__FUNCTION__,"成品 SM hmmsm01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						hmmsm01.MergeTo(bcls_ret->Tables["CHENGPIN"],false);
					}
				}
			}
			#endif
			if(tmm0005["MAT_KIND"].ToString().Trim() == "HR")
			{
				#if defined(_LINE_HR) || defined(_LINE_CR)
				tmmhr01["MAT_NO"]	= tmm0005["MAT_NO"];
				tmmhr01.Query("MAT_NO");
				tmmhr01.TrimOrBlank();
				if(tmmhr01["MAT_ID"].ToString().Trim() != "")
				{
					if(tmmhr01["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						Log::Trace("",__FUNCTION__,"成品 SM tmmhr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						tmmhr01.MergeTo(bcls_ret->Tables["CHENGPIN"],false);
					}
				}
				else
				{
					hmmhr01["MAT_ID"]	= tmm0005["MAT_ID"];
					hmmhr01.Query("MAT_ID");
					hmmhr01.TrimOrBlank();
					if(hmmhr01["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						Log::Trace("",__FUNCTION__,"成品 HR hmmhr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						hmmhr01.MergeTo(bcls_ret->Tables["CHENGPIN"],false);
					}
				}
				#endif				
			}
			if(tmm0005["MAT_KIND"].ToString().Trim() == "CR")
			{
				#if defined(_LINE_CR)
				tmmcr01["MAT_NO"]	= tmm0005["MAT_NO"];
				tmmcr01.Query("MAT_NO");
				tmmcr01.TrimOrBlank();
				if(tmmcr01["MAT_ID"].ToString().Trim() != "")
				{
					if(tmmcr01["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						Log::Trace("",__FUNCTION__,"成品 CR tmmcr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						tmmcr01.MergeTo(bcls_ret->Tables["CHENGPIN"],false);
					}
				}
				else
				{
					hmmcr01["MAT_ID"]	= tmm0005["MAT_ID"];
					hmmcr01.Query("MAT_ID");
					hmmcr01.TrimOrBlank();
					if(hmmcr01["PRODUCT_FLAG"].ToString().Trim() == "1")
					{
						Log::Trace("",__FUNCTION__,"成品 CR hmmcr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
						hmmcr01.MergeTo(bcls_ret->Tables["CHENGPIN"],false);
					}					
				}
				#endif

			}
			fetchRowCount++;
		}
		cmd_inq.Close();
		//#endif 

		/* 根据第一条原料信息,查询相同出口材料对应的多笔入口材料信息 */
		tmm0005.Reset();
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT IN_MAT_ID, "
						 "		 IN_MAT_KIND "
						 "  FROM TMM0005 "
						 " WHERE MAT_TRACK_NO = @tmm0005.MAT_TRACK_NO "
						 "	 AND IN_MAT_ID = @cs_yl_in_mat_id "
						 "	 AND MAT_NO <> @cs_yl_mat_no "
						 " ORDER BY REC_CREATE_TIME ASC ";	
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmm0005.MAT_TRACK_NO", tmm0005["MAT_TRACK_NO"].ToString());
		cmd_inq.Parameters.Set("cs_yl_in_mat_id", cs_yl_in_mat_id);
		cmd_inq.Parameters.Set("cs_yl_mat_no", cs_yl_mat_no);
		Log::Trace("",__FUNCTION__,"查询相同出口材料对应的多笔入口材料信息 sqlstr	= [{0}]",sqlstr);
		cmd_inq.ExecuteReader();
		while(cmd_inq.Read())	
		{
			cmd_inq.Fetch(tmm0005);
			tmm0005.TrimOrBlank();

			Log::Trace("",__FUNCTION__,"查询相同出口材料对应的多笔入口材料信息 tmm0005.PASS_BACKLOG_SEQ_NO	= [{0}]",tmm0005["PASS_BACKLOG_SEQ_NO"].ToDecimal().ToInt32());
			Log::Trace("",__FUNCTION__,"查询相同出口材料对应的多笔入口材料信息 tmm0005.MAT_KIND				= [{0}]",tmm0005["MAT_KIND"].ToString());

			#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) 
			if(tmm0005["MAT_KIND"].ToString().Trim() == "SM")
			{
				tmmsm01["MAT_NO"]	= tmm0005["MAT_NO"];
				tmmsm01.Query("MAT_NO");
				tmmsm01.TrimOrBlank();
				if(tmmsm01["MAT_ID"].ToString().Trim() != "")
				{
					Log::Trace("",__FUNCTION__,"多笔原料 SM tmmsm01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
					tmmsm01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
				}
				else
				{
					hmmsm01["MAT_ID"]	= tmm0005["MAT_ID"];
					hmmsm01.Query("MAT_ID");
					hmmsm01.TrimOrBlank();
					Log::Trace("",__FUNCTION__,"多笔原料 SM hmmsm01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
					hmmsm01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
				}
			}
			#endif
			if(tmm0005["MAT_KIND"].ToString().Trim() == "HR")
			{
				#if defined(_LINE_HR) || defined(_LINE_CR)
				tmmhr01["MAT_NO"]	= tmm0005["MAT_NO"];
				tmmhr01.Query();
				tmmhr01.TrimOrBlank();
				if(tmmhr01["MAT_ID"].ToString().Trim() != "")
				{
					Log::Trace("",__FUNCTION__,"多笔原料 HR tmmhr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
					tmmhr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
				}
				else
				{
					hmmhr01["MAT_ID"]	= tmm0005["MAT_ID"];
					hmmhr01.Query("MAT_ID");
					hmmhr01.TrimOrBlank();
					Log::Trace("",__FUNCTION__,"多笔原料 HR hmmhr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
					hmmhr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
				}
				#endif				
			}
			if(tmm0005["MAT_KIND"].ToString().Trim() == "CR")
			{
				#if defined(_LINE_CR)
				tmmcr01["MAT_NO"]	= tmm0005["MAT_NO"];
				tmmcr01.Query();
				tmmcr01.TrimOrBlank();
				if(tmmcr01["MAT_ID"].ToString().Trim() != "")
				{
					Log::Trace("",__FUNCTION__,"多笔原料 CR tmmcr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
					tmmcr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
				}
				else
				{
					hmmcr01["MAT_ID"]	= tmm0005["MAT_ID"];
					hmmcr01.Query("MAT_ID");
					hmmcr01.TrimOrBlank();
					Log::Trace("",__FUNCTION__,"多笔原料 CR hmmcr01.MAT_NO		= [{0}]",tmm0005["MAT_NO"].ToString());
					hmmcr01.MergeTo(bcls_ret->Tables["YUANLIAO"],false);
				}
				#endif				
			}
		}
		cmd_inq.Close();

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
		
