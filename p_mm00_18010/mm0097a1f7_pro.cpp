/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description: 物料跟踪事件管理事件复制
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪事件管理事件复制
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  
  

 
  

//外部函数声明

BM2F_ENTERACE(mm0097a1f7_pro)                                         

int f_mm0097a1f7_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    
	CDecimal cd_count	= 0;

	/* 实体类定义 */
	CModel old_tmm0097("TMM0097");
	CModel new_tmm0097("TMM0097");
	CModel tmm0099("TMM0099");
	CModel tmm009a("TMM009A");
	CModel tmm009b("TMM009B");
	CModel tep0002("TEP0002");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 校验操作者权限 */
		tep0002["CODE_CLASS"]	= "M09M";
		tep0002["CODE"]		= s.userid;
		if (tep0002.QueryCount("CODE_CLASS,CODE") <= 0)
		{
			sprintf(s.msg, "事件操作者[%s]无维护权限,请联系MM管理员,在EPEP01画面配置代码为[M09M]的工号!",(const char*)tep0002["CODE"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 获得传入参数 */
		new_tmm0097.MergeFrom(bcls_rec->Tables[0].Rows[0]);	//将新增事件放在第0块
		new_tmm0097.TrimOrBlank();
		old_tmm0097.MergeFrom(bcls_rec->Tables[1].Rows[0]); //将参考事件放在第1块
		old_tmm0097.TrimOrBlank();

		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097.EVENT_ID		= [{0}]",new_tmm0097["EVENT_ID"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097.MAT_KIND		= [{0}]",new_tmm0097["MAT_KIND"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097.EVENT_LINE_TYPE	= [{0}]",new_tmm0097["EVENT_LINE_TYPE"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_0\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_0"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_1\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_1"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_2\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_2"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_3\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_3"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_4\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_4"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_AC\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_AC"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_KC\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_KC"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_5\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_5"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_6\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_6"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_7\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_7"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_8\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_8"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 new_tmm0097[\"EVENT_PROC_WAY_9\"] = [{0}]",new_tmm0097["EVENT_PROC_WAY_9"].ToString());	

		/* 检查输入参数合法性 */
		if(new_tmm0097["EVENT_ID"].ToString().Trim() == "")
		{
			strcpy(s.msg,"事件号不能为空。");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(new_tmm0097["MAT_KIND"].ToString().Trim() == "")
		{
			strcpy(s.msg,"物料种类不能为空。");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(new_tmm0097["EVENT_LINE_TYPE"].ToString().Trim() == "")
		{
			strcpy(s.msg,"产线类型不能为空。");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(new_tmm0097["TC_SEND_FLAG"].ToString().Trim() == "")
		{
			strcpy(s.msg,"电文发送标记不能为空。");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(new_tmm0097["EVENT_TYPE"].ToString().Trim() == "")
		{
			strcpy(s.msg,"事件性质不能为空。");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(new_tmm0097["EVENT_LOG_SWITCH"].ToString().Trim() == "")
		{
			strcpy(s.msg,"打印TRACE开关不能为空。");
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		if(new_tmm0097["EVENT_PROC_WAY_1"].ToString().Trim() == "1"	//从历史档返回
		&& new_tmm0097["EVENT_PROC_WAY_6"].ToString().Trim() == "1")	//归历史档
		{
			strcpy(s.msg,"从历史档返回和归历史档,从业务逻辑上考虑,不能并行!");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(new_tmm0097["EVENT_PROC_WAY_1"].ToString().Trim() == "1"	//从历史档返回
		&& new_tmm0097["EVENT_PROC_WAY_6"].ToString().Trim() == "1")	//材料删除
		{
			strcpy(s.msg,"从历史档返回和材料删除,从业务逻辑上考虑,不能并行!");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(new_tmm0097["EVENT_PROC_WAY_6"].ToString().Trim() == "1"	//归历史档
		&& new_tmm0097["EVENT_PROC_WAY_7"].ToString().Trim() == "1")	//材料删除
		{
			strcpy(s.msg,"归历史档和材料删除,从业务逻辑上考虑,不能并行!");
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if((new_tmm0097["EVENT_CALL_TYPE_CODE"].ToString().Trim() == "11" 
		 || new_tmm0097["EVENT_CALL_TYPE_CODE"].ToString().Trim() == "2")	//事件调用类型有单独函数 11-配置&单独函数；2-单独函数
		&& new_tmm0097["EVENT_SPEC_PRO"].ToString().Trim() == "")	
		{
			sprintf(s.msg, "事件号[%s]物料[%s]产线类型[%s]调用类型是[配置&单独函数],事件特殊处理标记需配置!",
										(const char*)new_tmm0097["EVENT_ID"].ToString(), (const char*)new_tmm0097["MAT_KIND"].ToString(), (const char*)new_tmm0097["EVENT_LINE_TYPE"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);		
		}

		/* 校验主键不能重复 */
		if(new_tmm0097.QueryCount("EVENT_ID,MAT_KIND,EVENT_LINE_TYPE") > 0)
		{
			strcpy(s.msg,"主键 事件号[" + new_tmm0097["EVENT_ID"].ToString() + "]种类[" + new_tmm0097["MAT_KIND"].ToString() + "]产线[" + new_tmm0097["EVENT_LINE_TYPE"].ToString() + "]重复!");
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		/* 设置事件处理规则 */
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY_0"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_1"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_2"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_3"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_4"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_AC"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_KC"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_5"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_6"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_7"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_8"].ToString() ;
		new_tmm0097["EVENT_PROC_WAY"]	= new_tmm0097["EVENT_PROC_WAY"].ToString() + new_tmm0097["EVENT_PROC_WAY_9"].ToString() ;

		Log::Info("",__FUNCTION__,"设置事件处理规则 new_tmm0097.EVENT_PROC_WAY	= [{0}]",new_tmm0097["EVENT_PROC_WAY"].ToString());	

		/* 非全产线事件,校验事件处理规则需与全产线相同 */
		if(new_tmm0097["EVENT_LINE_TYPE"].ToString().Trim() != "00"						
		&& new_tmm0097.QueryCount("EVENT_ID,MAT_KIND,EVENT_PROC_WAY") < 0)  //新增的产线的业务规则与全产线不同
		{
			strcpy(s.msg,"事件号[" + new_tmm0097["EVENT_ID"].ToString() + "]种类[" + new_tmm0097["MAT_KIND"].ToString() + "]产线[" + new_tmm0097["EVENT_LINE_TYPE"].ToString() + "]处理规则[" + new_tmm0097["EVENT_PROC_WAY"].ToString() +"]必须于全产线相同!");
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		/* 新增事件信息 */
		new_tmm0097["REC_REVISOR"]		= "";   
		new_tmm0097["REC_REVISE_TIME"]	= "";   
		new_tmm0097["REC_CREATOR"]		= s.userid;   //记录创建责任者
		new_tmm0097["REC_CREATE_TIME"]	= datetime;   //记录创建时刻
 		new_tmm0097.TrimOrBlank();
		new_tmm0097.Insert();

		/****** 自动新增事件接口信息 ******/
		Log::Info("",__FUNCTION__,"传入参数 old_tmm0097.EVENT_ID		= [{0}]",old_tmm0097["EVENT_ID"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 old_tmm0097.MAT_KIND		= [{0}]",old_tmm0097["MAT_KIND"].ToString());	
		Log::Info("",__FUNCTION__,"传入参数 old_tmm0097.EVENT_LINE_TYPE	= [{0}]",old_tmm0097["EVENT_LINE_TYPE"].ToString());	

		/* 查询参数信息 */
	    switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr	= " SELECT * "
						  "   FROM TMM0099 "
						  "  WHERE EVENT_ID			= @old_tmm0097.EVENT_ID "
						  "	   AND MAT_KIND			= @old_tmm0097.MAT_KIND "
						  "	   AND EVENT_LINE_TYPE	= @old_tmm0097.EVENT_LINE_TYPE "
						  "  ORDER BY EVENT_ID ASC,SEQ_NO ASC";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("",__FUNCTION__,"sqlstr		= [{0}]",sqlstr);	
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("old_tmm0097.EVENT_ID",old_tmm0097["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("old_tmm0097.MAT_KIND",old_tmm0097["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("old_tmm0097.EVENT_LINE_TYPE",old_tmm0097["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm0099);
			tmm0099.TrimOrBlank();

			///* 设置事件接口表流水号 EVENT_ITEM_SEQ_NO */
			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = "SELECT MAX(EVENT_ITEM_SEQ_NO) "
			//				 "  FROM TMM0099 ";
			//	break;
			//}
			//cmd_inq_1.SetCommandText(sqlstr);
			//cmd_inq_1.ExecuteReader();
			//if(cmd_inq_1.Read())
			//{
			//	tmm0099.EVENT_ITEM_SEQ_NO	= cmd_inq_1.GetDecimal(1);
			//}
			//cmd_inq_1.Close();
			//tmm0099["EVENT_ITEM_SEQ_NO"]	= tmm0099["EVENT_ITEM_SEQ_NO"].ToDecimal() + 1;
				
			tmm0099["EVENT_ID"]		= new_tmm0097["EVENT_ID"];
			tmm0099["MAT_KIND"]		= new_tmm0097["MAT_KIND"];
			tmm0099["EVENT_LINE_TYPE"]	= new_tmm0097["EVENT_LINE_TYPE"];

			tmm0099["REC_REVISOR"]		= "";   
			tmm0099["REC_REVISE_TIME"]	= "";   
			tmm0099["REC_CREATOR"]		= s.userid;   //记录创建责任者
			tmm0099["REC_CREATE_TIME"]	= datetime;   //记录创建时刻
 			tmm0099.TrimOrBlank();

			tmm0099.Insert();
		}
		cmd_inq.Close();

		/* 事件电文配置表 */
	    switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr	= " SELECT * "
						  "   FROM TMM009A "
						  "  WHERE EVENT_ID			= @old_tmm0097.EVENT_ID "
						  "	   AND MAT_KIND			= @old_tmm0097.MAT_KIND "
						  "	   AND EVENT_LINE_TYPE	= @old_tmm0097.EVENT_LINE_TYPE "
						  "  ORDER BY EVENT_ID ASC,TC_KEYVALUE ASC";
				break;
		}     
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("",__FUNCTION__,"sqlstr		= [{0}]",sqlstr);	
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("old_tmm0097.EVENT_ID",old_tmm0097["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("old_tmm0097.MAT_KIND",old_tmm0097["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("old_tmm0097.EVENT_LINE_TYPE",old_tmm0097["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm009a);
			tmm009a.TrimOrBlank();

			tmm009a["EVENT_ID"]		= new_tmm0097["EVENT_ID"];
			tmm009a["MAT_KIND"]		= new_tmm0097["MAT_KIND"];
			tmm009a["EVENT_LINE_TYPE"]	= new_tmm0097["EVENT_LINE_TYPE"];

			tmm009a["REC_CREATOR"]		= s.userid;   //记录创建责任者
			tmm009a["REC_CREATE_TIME"]	= datetime;   //记录创建时刻
 			tmm009a.TrimOrBlank();
			tmm009a.Insert();
		}
		cmd_inq.Close();

		/* 事件抛帐字段配置表 */
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT * "
						 "   FROM TMM009B "
						 "  WHERE EVENT_ID			= @old_tmm0097.EVENT_ID "
						 "	   AND MAT_KIND			= @old_tmm0097.MAT_KIND "
						 "	   AND EVENT_LINE_TYPE	= @old_tmm0097.EVENT_LINE_TYPE "
						 "  ORDER BY EVENT_ID ASC,THROW_KIND ASC";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("old_tmm0097.EVENT_ID", old_tmm0097["EVENT_ID"].ToString());
		cmd_inq.Parameters.Set("old_tmm0097.MAT_KIND", old_tmm0097["MAT_KIND"].ToString());
		cmd_inq.Parameters.Set("old_tmm0097.EVENT_LINE_TYPE", old_tmm0097["EVENT_LINE_TYPE"].ToString());
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tmm009b);
			tmm009b.TrimOrBlank();

			tmm009b["EVENT_ID"] = new_tmm0097["EVENT_ID"];
			tmm009b["MAT_KIND"] = new_tmm0097["MAT_KIND"];
			tmm009b["EVENT_LINE_TYPE"] = new_tmm0097["EVENT_LINE_TYPE"];

			tmm009b["REC_CREATOR"] = s.userid;   //记录创建责任者
			tmm009b["REC_CREATE_TIME"] = datetime;   //记录创建时刻
			tmm009b.TrimOrBlank();
			tmm009b.Insert();
		}
		cmd_inq.Close();
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

