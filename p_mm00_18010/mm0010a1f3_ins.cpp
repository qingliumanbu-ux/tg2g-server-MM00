/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-07-11
Description: 外购料信息管理_材料新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 外购料信息管理_材料新增
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
   



//外部函数声明
BM2_FUNCTION_IMPORT
int f_qmtq_pono_get(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);

BM2F_ENTERACE(mm0010a1f3_ins)

int f_mm0010a1f3_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime(""); 
	CString	cs_heat_no("");
	CString	cs_column_ename("");

	/* 实体类定义 */
	CModel tmm0010("TMM0010");
	CModel tqmtqb0("TQMTQB0");
	CModel tqmtqq0("TQMTQQ0");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
   {	
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加与设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("PONOBLOCK");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PONOBLOCK");
			bcls_rec->Tables["PONOBLOCK"].Columns.Add(DT_STRING, "HEAT_NO");        //熔炼号
			bcls_rec->Tables["PONOBLOCK"].Columns.Add(DT_STRING, "VENDOR_CODE");   //供应商代码
			bcls_rec->Tables["PONOBLOCK"].Columns.Add(DT_STRING, "SG_SIGN");       //牌号（钢级）
			bcls_rec->Tables["PONOBLOCK"].Rows.Add();
		}

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmm0010.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tmm0010.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "传入参数 tmm0010.ORIGIN_MAT_NO	= [{0}]", tmm0010["ORIGIN_MAT_NO"].ToString());  //外购材料号
			Log::Trace("", __FUNCTION__, "传入参数 tmm0010.RAW_ORIGIN		= [{0}]", tmm0010["RAW_ORIGIN"].ToString());     //原料来源

			/* 检查输入参数合法性 */
			if (tmm0010["RAW_ORIGIN"].ToString().Trim() == "")
			{
				strcpy(s.msg, "原料来源不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0010["RAW_ORIGIN"].ToString().Trim() == "1" ||  tmm0010["RAW_ORIGIN"].ToString().Trim() == "6")//1-外购，6-互供
			{
				if (tmm0010["ORIGIN_MAT_NO"].ToString().Trim() == "" && tmm0010["RAW_ORIGIN"].ToString().Trim() == "1")
				{
					strcpy(s.msg, "外购材料号不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm0010["ORIGIN_MAT_NO"].ToString().Trim() == "" && tmm0010["RAW_ORIGIN"].ToString().Trim() == "6")
				{
					strcpy(s.msg, "互供材料号不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm0010["ORIGIN_HEAT_NO"].ToString().Trim() == "")  //外购供应商炉号
				{
					strcpy(s.msg, "外购供应商炉号不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (tmm0010["RAW_ORIGIN"].ToString().Trim() == "4")//4-来料加工
			{
				if (tmm0010["ORIGIN_MAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "来料材料号不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm0010["HEAT_NO"].ToString().Trim() == "")
				{
					strcpy(s.msg, "炉号不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			if (tmm0010["MEASURE_WT_FLAG"].ToString().Trim() == "")     //称重标记
			{
				strcpy(s.msg, "称重标记不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0010["MAT_WT"].ToDecimal() <= 0)
			{
				strcpy(s.msg, "材料重量不能为0");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0010["MAT_THICK"].ToDecimal() <= 0)
			{
				strcpy(s.msg, "材料厚度不能为0");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0010["MAT_WIDTH"].ToDecimal() <= 0)
			{
				strcpy(s.msg, "材料宽度不能为0");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tmm0010["MAT_LEN"].ToDecimal() <= 0)
			{
				strcpy(s.msg, "材料长度不能为0");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			 
			/* 设置默认值 */
			tmm0010["AFFIRM_FLAG"] = "N";   //N-未确认
			tmm0010["MAT_ORIGIN"]	= tmm0010["RAW_ORIGIN"];	// 材料来源=原料来源
			if (tmm0010["MAT_KIND"].ToString() == "SM")
			{
				tmm0010["MAT_SHAPE_FLAG"] = "1";//1-板坯   MAT_SHAPE_FLAG 材料形态标记
			}
			else if (tmm0010["MAT_KIND"].ToString() == "HR")
			{
				if (tmm0010["PLATE_OR_COIL"].ToString() == "P")   //P-板
				{
					tmm0010["MAT_SHAPE_FLAG"] = "2";//2-钢板
				}
				else if (tmm0010["PLATE_OR_COIL"].ToString() == "C")  //C-卷
				{
					tmm0010["MAT_SHAPE_FLAG"] = "3";//3-钢卷
				}
			}
			else if (tmm0010["MAT_KIND"].ToString() == "CR")
			{
				if (tmm0010["PLATE_OR_COIL"].ToString() == "P") //P-板
				{
					tmm0010["MAT_SHAPE_FLAG"] = "2";//2-钢板
				}
				else if (tmm0010["PLATE_OR_COIL"].ToString() == "C") //C-卷
				{
					tmm0010["MAT_SHAPE_FLAG"] = "3";//1-钢卷
				}
			}
			if (tmm0010["MEASURE_WT_FLAG"].ToString().Trim() == "0")   // 称重标记   0-未称重
			{
				tmm0010["MAT_THEORY_WT"]		= tmm0010["MAT_WT"];
			}
			else if (tmm0010["MEASURE_WT_FLAG"].ToString().Trim() == "1")// 称重标记  1-已称重
			{
				tmm0010["MAT_ACT_WT"]		= tmm0010["MAT_WT"];
				if (tmm0010["MAT_THEORY_WT"].ToDecimal() <= 0)      //MAT_THEORY_WT 材料理论重量
				{
					tmm0010["MAT_THEORY_WT"]	= ((tmm0010["MAT_WIDTH"].ToDecimal() * tmm0010["MAT_THICK"].ToDecimal() * tmm0010["MAT_LEN"].ToDecimal() * 7.85) / 1000000000).Round(3);
				}
			}
			if (tmm0010["MAT_ACT_THICK"].ToDecimal() <= 0)
			{
				tmm0010["MAT_ACT_THICK"]	= tmm0010["MAT_THICK"];
			}
			if (tmm0010["MAT_ACT_WIDTH"].ToDecimal() <= 0)
			{
				tmm0010["MAT_ACT_WIDTH"]	= tmm0010["MAT_WIDTH"];
			}
			if (tmm0010["MAT_ACT_LEN"].ToDecimal() <= 0)
			{
				tmm0010["MAT_ACT_LEN"]	= tmm0010["MAT_LEN"];
			}
			if (tmm0010["PROD_TIME"].ToString().Trim() == "")
			{
				tmm0010["PROD_TIME"] = datetime;
			}


			/* 校验外购料是否存在 */
			if (tmm0010.QueryCount("ORIGIN_MAT_NO") > 0)
			{
				sprintf(s.msg, "外购材料号[%s]已存在", (const char*)tmm0010["ORIGIN_MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
				
			/* 外购材料处理 */
			if (tmm0010["RAW_ORIGIN"].ToString().Trim() == "1" || tmm0010["RAW_ORIGIN"].ToString().Trim() == "6")//1-外购，6-互供
			{
				Log::Trace("", __FUNCTION__, "生成制造命令号		= [{0}]", tmm0010["ORIGIN_MAT_NO"].ToString());	//外购材料号
				Log::Trace("", __FUNCTION__, "tmm0010.VENDOR_CODE	= [{0}]", tmm0010["VENDOR_CODE"].ToString());		//供应商
				Log::Trace("", __FUNCTION__, "ORIGIN_HEAT_NO		= [{0}]", tmm0010["ORIGIN_HEAT_NO"].ToString());	//外购供应商炉号
				Log::Trace("", __FUNCTION__, "tmm0010.RAW_ORIGIN	= [{0}]", tmm0010["RAW_ORIGIN"].ToString());		//原料来源

				/* 生成制造命令号 */
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:	        // MS sqlstr Server数据库
					case DB_KIND_ORACLE:	    // Oracle 数据库
					default:
						sqlstr = "SELECT PONO, "
								"		 HEAT_NO "
								"  FROM TMM0010 "
								" WHERE VENDOR_CODE	= @tmm0010.VENDOR_CODE "
								"   AND ORIGIN_HEAT_NO = @tmm0010.ORIGIN_HEAT_NO ";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0010.VENDOR_CODE", tmm0010["VENDOR_CODE"].ToString());
				cmd_inq.Parameters.Set("tmm0010.ORIGIN_HEAT_NO", tmm0010["ORIGIN_HEAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())	//获取第1行记录
				{
					tmm0010["PONO"] = cmd_inq.GetString(1).Trim();
					tmm0010["HEAT_NO"] = cmd_inq.GetString(2).Trim();
				}
				cmd_inq.Close();

				if (tmm0010["PONO"].ToString().Trim() == "" || tmm0010["HEAT_NO"].ToString().Trim() == "")	//外购供应商炉号不存在，新增PONO信息
				{
					//生成系统HEAT_NO   2位年末位+'W'+6位流水
					cs_heat_no = EPGetNextSeq("MM00_HEAT_NO", conn);
					if (cs_heat_no.Trim() == "")
					{
						sprintf(s.msg, "获取 炉号流水号 失败!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					tmm0010["HEAT_NO"] = datetime.Substring(2, 2) + "W" + cs_heat_no.Trim();

					//调用质量函数，新增炉次信息 
					bcls_rec->Tables["PONOBLOCK"].Rows[0]["HEAT_NO"] = tmm0010["HEAT_NO"];    //熔炼号
					bcls_rec->Tables["PONOBLOCK"].Rows[0]["VENDOR_CODE"] = tmm0010["VENDOR_CODE"]; //供应商代码
					bcls_rec->Tables["PONOBLOCK"].Rows[0]["SG_SIGN"] = tmm0010["SG_SIGN"];    //牌号（钢级）
					doFlag = f_qmtq_pono_get(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					tmm0010["PONO"] = bcls_ret->Tables["PONOBLOCK"].Rows[0]["PONO"];
					if (tmm0010["PONO"].ToString().Trim() == "")
					{
						strcpy(s.msg, "调用质量接口，产生的PONO为空!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}

				Log::Trace("", __FUNCTION__, "获取到的 tmm0010.HEAT_NO	= [{0}]", tmm0010["HEAT_NO"].ToString());  //熔炼号
				Log::Trace("", __FUNCTION__, "获取到的 tmm0010.PONO		= [{0}]", tmm0010["PONO"].ToString());     //制造命令号

				/* 新增时材料号暂时取外购材料号,外购确认时,生成内部材料号 */
				tmm0010["MAT_NO"] = tmm0010["ORIGIN_MAT_NO"];
			}

			/* 来料加工材料处理 */
			else if (tmm0010["RAW_ORIGIN"].ToString().Trim() == "4")//4-来料加工
			{
				Log::Trace("", __FUNCTION__, "生成制造命令号	= [{0}]", (const char*)tmm0010["MAT_NO"].ToString());
				Log::Trace("", __FUNCTION__, "tmm0010.VENDOR_CODE	= [{0}]", (const char*)tmm0010["VENDOR_CODE"].ToString());
				Log::Trace("", __FUNCTION__, "HEAT_NO	= [{0}]", (const char*)tmm0010["HEAT_NO"].ToString());

				/* 生成制造命令号 */
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:	        // MS sqlstr Server数据库
					case DB_KIND_ORACLE:	    // Oracle 数据库
					default:
						sqlstr = "SELECT PONO "
								"  FROM TMM0010 "
								" WHERE VENDOR_CODE	= @tmm0010.VENDOR_CODE "
								"   AND HEAT_NO		= @tmm0010.HEAT_NO ";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tmm0010.VENDOR_CODE", tmm0010["VENDOR_CODE"].ToString());
				cmd_inq.Parameters.Set("tmm0010.HEAT_NO", tmm0010["HEAT_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())	//获取第1行记录
				{
					tmm0010["PONO"] = cmd_inq.GetString(1).Trim();
				}
				cmd_inq.Close();

				/* PONO未生成过则重新生成PONO */
				if (tmm0010["PONO"].ToString().Trim() == "")
				{
					////生成系统HEAT_NO  2位年末位+'W'+6位流水
					//cs_heat_no = EPGetNextSeq("MM00_HEAT_NO", conn);
					//if (cs_heat_no.Trim() == "")
					//{
					//	sprintf(s.msg, "获取 炉号流水号 失败!");
					//	throw CApplicationException(-1, s.msg, s.svc_name);
					//}

					//tmm0010["HEAT_NO"] = datetime.Substring(2, 2) + "W" + cs_heat_no.Trim();

					//调用质量函数，新增炉次信息 
					bcls_rec->Tables["PONOBLOCK"].Rows[0]["HEAT_NO"] = tmm0010["HEAT_NO"];
					bcls_rec->Tables["PONOBLOCK"].Rows[0]["VENDOR_CODE"] = tmm0010["VENDOR_CODE"];
					bcls_rec->Tables["PONOBLOCK"].Rows[0]["SG_SIGN"] = tmm0010["SG_SIGN"];
					doFlag = f_qmtq_pono_get(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					tmm0010["PONO"] = bcls_ret->Tables["PONOBLOCK"].Rows[0]["PONO"];
					if (tmm0010["PONO"].ToString().Trim() == "")
					{
						strcpy(s.msg, "调用质量接口，产生的PONO为空!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}

				/* 新增时材料号为空时,默认取来料材料号 */
				if (tmm0010["MAT_NO"].ToString().Trim() == "")
				{
					tmm0010["MAT_NO"] = tmm0010["ORIGIN_MAT_NO"];
				}
				Log::Trace("", __FUNCTION__, "获取到的 tmm0010.HEAT_NO	= [{0}]", tmm0010["HEAT_NO"].ToString());
				Log::Trace("", __FUNCTION__, "获取到的 tmm0010.PONO		= [{0}]", tmm0010["PONO"].ToString());
			}

			///* 查找锭坯型 */
			//switch (conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:	        // MS sqlstr Server数据库
			//	case DB_KIND_ORACLE:	    // Oracle 数据库
			//	default:
			//		sqlstr = "SELECT INGOT_CODE "
			//				 "  FROM TQMTMD9 "
			//				 " WHERE BILLET_TYPE	='1'  "
			//				 "   AND SLAB_THICK = @tmm0010.MAT_THICK ";
			//		break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Clear();
			//cmd_inq.Parameters.Set("tmm0010.MAT_THICK", tmm0010["MAT_THICK"].ToDecimal());
			//cmd_inq.ExecuteReader();
			//if (cmd_inq.Read())	//获取第1行记录
			//{
			//	tmm0010.INGOT_CODE = cmd_inq.GetString(1).Trim();
			//}
			//cmd_inq.Close();
			//if (tmm0010.INGOT_CODE.Trim() == "")
			//{
			//	tmm0010.INGOT_CODE = "0000";
			//}			
			//Log::Trace("", __FUNCTION__, "生成制造命令号	= [{0}]", (const char*)tmm0010.INGOT_CODE);
			
			/* 新增外购料界面表 */
			tmm0010["REC_CREATOR"]		= s.userid;/* 记录新增责任者 */
			tmm0010["REC_CREATE_TIME"] = datetime;/* 记录新增时刻 */
			tmm0010.TrimOrBlank();
			tmm0010.Print();
			tmm0010.Insert();

			/* 调用外购料炉次信息新增 TQMTQB0 */
			tqmtqb0.Reset();
			tqmtqb0["PONO"] = tmm0010["PONO"];
			tqmtqb0["HEAT_NO"] = tmm0010["HEAT_NO"];
			if (tqmtqb0.QueryCount("PONO,HEAT_NO") == 0)
			{
				tqmtqb0["REC_CREATE_TIME"] = datetime;
				tqmtqb0["REC_CREATOR"] = s.userid;
				tqmtqb0["VENDOR_CODE"] = tmm0010["VENDOR_CODE"];
				tqmtqb0.TrimOrBlank();
				tqmtqb0.Insert();
			}

			/* 调用化学成分新增 TQMTQQ0 */
			tqmtqq0.Reset();
			tqmtqq0["PONO"]		= tmm0010["PONO"];
			tqmtqq0["HEAT_NO"]		= tmm0010["HEAT_NO"];
			tqmtqq0["VENDOR_CODE"] = tmm0010["VENDOR_CODE"];
			tqmtqq0["REC_CREATE_TIME"] = datetime;
			tqmtqq0["REC_CREATOR"] = s.userid;

			for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
			{
				cs_column_ename = bcls_rec->Tables[0].Columns[j].get_ColumnName().Trim();

				Log::Trace("", __FUNCTION__, "传入参数 cs_column_ename	= [{0}]", cs_column_ename);

				if (cs_column_ename.GetLength() < 4 || cs_column_ename.Substring(0, 4) != "ELM_")
				{
					continue;  
				}

				//ELM_C_ACT
				CString  cs_elm = cs_column_ename.Substring(3, cs_column_ename.GetLength() - 8);

				Log::Trace("", __FUNCTION__, "传入参数 cs_elm	= [{0}]", cs_elm);

				//查询TEP0002代码信息
				switch (conn->DatabaseKind)
				{
					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:	        // MS SQL Server数据库
					case DB_KIND_ORACLE:	    // Oracle 数据库
					default:
						sqlstr = "SELECT CODE,CODE_DESC_1_CONTENT,CODE_DESC_2_CONTENT,CODE_DESC_3_CONTENT "
								 "   FROM TEP0002 "
								 "  WHERE CODE_DESC_1_CONTENT = @cs_elm "
								 "    AND CODE_CLASS = 'QMYS'";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("cs_elm", cs_elm);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmtqq0["ELM_CODE"] = cmd_inq.GetString(1);
					if (tqmtqq0.QueryCount("PONO,ELM_CODE") == 0)
					{
						tqmtqq0["ELM_NAME"]	= cmd_inq.GetString(2);
						tqmtqq0["ELM_POS"]		= cmd_inq.GetDecimal(3);
						tqmtqq0["ELM_DESC"]	= cmd_inq.GetString(4);
						tqmtqq0["ELM_ACT"]		= bcls_rec->Tables[0].Rows[0][cs_column_ename];
						tqmtqq0.TrimOrBlank();
						tqmtqq0.Insert();
					}
				}
				cmd_inq.Close();
			}                 
		}
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
