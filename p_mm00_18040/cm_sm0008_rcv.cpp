/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     179648
Version:    1.0
Date:       2023年2月1日
Description: 码单红冲
**************************************************/
//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"
int f_mm0099(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_pmof99_v3(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_qmtc_mat_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送质保书界面档
//int f_qmtc_red_mat_no(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2F_ENTERACE_TELE(cm_sm0008_rcv)

int f_cm_sm0008_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	CString matNo = "";
	CString sqlstr = "";
	CString matKind = "";
	CDecimal matWt = 0;
	CString orderNo = "";
	CString prodClassCode = "";
	CString redCause = "";
	CString stockCode = "";
	CString instockCode = "";
	CString aimSysCode = "";
	CString oldSysCode = "";
	CString stackingType = ""; //(发货码单  1；正常转库码单2 ； 转金家码单 3
	CString stackingNo = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tpmof03("TPMOF03");
	CModel tpmof02("TPMOF02");

	EIClass  qmtc_rec;  //抛质保书界面档
	EIClass  pmof_rec;  //生产合同跟踪

	CDbCommand cmd(conn);

	try
	{

		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "op_flag");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "stacking_no");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "mat_no");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "prod_class_code");
		qmtc_rec.Tables[0].Columns.Add(DT_STRING, "stacking_type");

		/* 获取输入参数 */
		matNo = bcls_rec->Tables[0].Rows[0]["CUST_MAT_NO"].ToString().Trim();
		stockCode = bcls_rec->Tables[0].Rows[0]["OUT_STOCK_CODE"].ToString().Trim();
		instockCode = bcls_rec->Tables[0].Rows[0]["IN_STOCK_CODE"].ToString().Trim();
		matKind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		stackingType = bcls_rec->Tables[0].Rows[0]["DELIVY_PLAN_TYPE"].ToString().Trim();
		redCause = bcls_rec->Tables[0].Rows[0]["RED_CAUSE_CODE"].ToString().Trim();
		stackingNo = bcls_rec->Tables[0].Rows[0]["STACKING_NO"].ToString().Trim();
		prodClassCode = bcls_rec->Tables[0].Rows[0]["PROD_CLASS_CODE"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "matNo [{0}],stockCode [{1}],matKind [{2}]", matNo, stockCode, matKind);

		if (matNo == "" || stockCode == "" || matKind == "")
		{
			strcpy(s.msg, "从电文获取的材料号/目的库区/物料类型不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (stackingType != "0" && stackingType != "1" && stackingType != "2" && stackingType != "3" )
		{
			strcpy(s.msg, "从电文获取的红冲码单类型不在约定的0/1/2/3内");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		cmd.SetCommandText("SELECT TC_MARK FROM TSI0021 WHERE STOCK_NO = @stockCode");
		cmd.Parameters.Set("stockCode", stockCode);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			aimSysCode = cmd.GetString(1);
		}
		cmd.Close();

		cmd.SetCommandText("SELECT TC_MARK FROM TSI0021 WHERE STOCK_NO = @instockCode");
		cmd.Parameters.Set("stockCode", instockCode);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			oldSysCode = cmd.GetString(1);
		}
		cmd.Close();

		//linggu 2023年2月1日
		//目前定义（来自梁宣）：0出厂1内倒2转库3领用
		//0,3为已发货归档材料，调用SM02事件，同时需判断OUT_STOCK_CODE为厂内库的需单独同步到PES（看是否能通过物料判断后同步下发PES）
		//1,2为内倒或内外转库的码单红冲，需根据OUT_STOCK_CODE与IN_STOCK_CODE进行判断
		//当OUT和IN属于不同分区时，OUT部分需倒灌并生成入库队列，IN的理论上需发一条信息红冲出库归档
		//当OUT和IN属于相同分区时，IN发一条出库并生成OUT入库队列
		if (stackingType == "0" || stackingType == "3")
		{
			blkNum = bcls_rec->Tables.IndexOf("MM0099");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MM0099");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_KIND");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "STOCK_NO");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYS_CODE");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "AIM_STORE");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
				bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			}

			/* 设置物料跟踪参数*/
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "SM02";
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_KIND"] = matKind;
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MM" + matKind;
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "码单红冲";
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = matNo;
			bcls_rec->Tables["MM0099"].Rows[0]["STOCK_NO"] = stockCode;
			bcls_rec->Tables["MM0099"].Rows[0]["SYS_CODE"] = aimSysCode;
			bcls_rec->Tables["MM0099"].Rows[0]["AIM_STORE"] = stockCode;
			bcls_rec->Tables["MM0099"].Rows[0]["STOCK_OPER_ORDER"] = "1N";

			doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(doFlag, s.msg, log.Location);
			}

			//抛质保书界面档
			qmtc_rec.Tables[0].Rows.Clear();
			qmtc_rec.Tables[0].Rows.Add();
			qmtc_rec.Tables[0].Rows[0]["op_flag"] = "3"; //1-码单生成，3-码单红冲
			qmtc_rec.Tables[0].Rows[0]["stacking_no"] = stackingNo;
			qmtc_rec.Tables[0].Rows[0]["mat_no"] = matNo;
			qmtc_rec.Tables[0].Rows[0]["prod_class_code"] = prodClassCode;
			qmtc_rec.Tables[0].Rows[0]["stacking_type"] = "W";

			doFlag = f_qmtc_mat_rcv(&qmtc_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				doFlag = 0; //界面档红冲时不做限制
				//throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//LINGGU ADD 2023年2月2日 按施辉的要求，9B工序还是需要抛合同跟踪

			sqlstr = "SELECT ORDER_NO,MAT_WT FROM TMM" + matKind + "01 WHERE MAT_NO = @matNo";
			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("matNo", matNo);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				orderNo = cmd.GetString(1);
				matWt = cmd.GetDecimal(2);
			}
			cmd.Close();

			Log::Trace("", __FUNCTION__, "调用生产合同跟踪BEGIN 合同[{0}]",orderNo);
			/*调用生产合同跟踪BEGIN*/
			CString wholeBacklog = "";
			tpmof02["ORDER_NO"] = orderNo;
			tpmof02["BACKLOG_FLAG"] = "0";
			tpmof02["WHOLE_BACKLOG_CODE"] = "9B";
			if (!tpmof02.Query("ORDER_NO,BACKLOG_FLAG,WHOLE_BACKLOG_CODE"))
			{
				sprintf(s.msg, _RES("合同全程途径码读取失败")/*合同全程途径码读取失败*/);
				throw	CApplicationException(-1, s.msg, s.svc_name);
			}

			tpmof03["EVENT_ID"] = "73";
			tpmof03["SYSTEM_ID"] = "MM";								//子系统标识
			tpmof03["FUNC_ID"] = "cm_sm0008_rcv"; 				//功能标识
			tpmof03["ORDER_NO"] = orderNo;	//合同号
			tpmof03["BACKLOG_FLAG"] = "0";	                    //主副制程标记
			tpmof03["WHOLE_BACKLOG"] = tpmof02["WHOLE_BACKLOG"];	//全程途径码
			tpmof03["WHOLE_BACKLOG_NO"] = tpmof02["WHOLE_BACKLOG_NO"];

			wholeBacklog = tpmof02["WHOLE_BACKLOG"];
			tpmof03["WHOLE_BACKLOG_SEQ"] = wholeBacklog.Find("9B") / 2 + 1;
			tpmof03["WHOLE_BACKLOG_CODE"] = "9B";
			tpmof03["MAT_NO"] = matNo;
			tpmof03["WT"] = matWt;

			pmof_rec.Tables.Add("PMOF99");
			pmof_rec.Tables["PMOF99"].Columns.Add(tpmof03);
			pmof_rec.Tables["PMOF99"].Rows.Add();
			pmof_rec.Tables["PMOF99"].Rows[0].Merge(tpmof03);

			//抛合同跟踪
			if (pmof_rec.Tables["PMOF99"].Rows.get_Count() > 0)
			{
				doFlag = f_pmof99_v3(&pmof_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}

		}
		else
		{
			blkNum = bcls_rec->Tables.IndexOf("MM0099");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MM0099");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_KIND");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "OUT_STOCK_TIME");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "STOCK_NO");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "AIM_STORE");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "TRNP_MODE_CODE");
				bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYS_CODE");
				bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			}
			/* 设置物料跟踪参数*/

			//不同分区时，OUT部分需倒灌并生成入库队列，IN的理论上需发一条信息红冲出库归档
			if (aimSysCode != oldSysCode)
			{
				if (oldSysCode != "00")
				{
					//PES之间的互转红冲,OUT PES已归档
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "SMR1";  //相同分区时，同步			
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_KIND"] = matKind;
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
					bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MM" + matKind;
					bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "转库码单红冲(目标库区归档)";
					bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = matNo;
					bcls_rec->Tables["MM0099"].Rows[0]["OUT_STOCK_TIME"] = datetime;
					bcls_rec->Tables["MM0099"].Rows[0]["STOCK_NO"] = instockCode;
					bcls_rec->Tables["MM0099"].Rows[0]["STOCK_OPER_ORDER"] = "2G";
					bcls_rec->Tables["MM0099"].Rows[0]["AIM_STORE"] = stockCode;
					bcls_rec->Tables["MM0099"].Rows[0]["TRNP_MODE_CODE"] = "";
					bcls_rec->Tables["MM0099"].Rows[0]["SYS_CODE"] = oldSysCode;
					doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(doFlag, s.msg, log.Location);
					}
				}				

				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "SMR2";  //相同分区时，同步			
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_KIND"] = matKind;
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MM" + matKind;
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "转库码单红冲(源库区回档)";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = matNo;
				bcls_rec->Tables["MM0099"].Rows[0]["OUT_STOCK_TIME"] = datetime;
				bcls_rec->Tables["MM0099"].Rows[0]["STOCK_NO"] = stockCode;
				bcls_rec->Tables["MM0099"].Rows[0]["STOCK_OPER_ORDER"] = "1N";
				bcls_rec->Tables["MM0099"].Rows[0]["AIM_STORE"] = stockCode;
				bcls_rec->Tables["MM0099"].Rows[0]["TRNP_MODE_CODE"] = "";
				bcls_rec->Tables["MM0099"].Rows[0]["SYS_CODE"] = aimSysCode;
				doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(doFlag, s.msg, log.Location);
				}
			}
			else if (aimSysCode == oldSysCode && oldSysCode == "00")
			{
				//厂外库转厂外库的红冲，仅做产销更新，不需要同步
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "SMRU";  			
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_KIND"] = matKind;
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MM" + matKind;
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "厂外库互转码单红冲";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = matNo;
				bcls_rec->Tables["MM0099"].Rows[0]["OUT_STOCK_TIME"] = datetime;
				bcls_rec->Tables["MM0099"].Rows[0]["STOCK_NO"] = stockCode;
				bcls_rec->Tables["MM0099"].Rows[0]["STOCK_OPER_ORDER"] = "2G";
				bcls_rec->Tables["MM0099"].Rows[0]["AIM_STORE"] = stockCode;
				bcls_rec->Tables["MM0099"].Rows[0]["TRNP_MODE_CODE"] = "";
				bcls_rec->Tables["MM0099"].Rows[0]["SYS_CODE"] = aimSysCode;
				doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(doFlag, s.msg, log.Location);
				}
			}
			else if(aimSysCode == oldSysCode && oldSysCode != "00")
			{
				//同分区下(PES未归档，仅重新生成队列)
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "SMR3";  //相同分区时，同步			
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_KIND"] = matKind;
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MM" + matKind;
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "同分区码单红冲";
				bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = matNo;
				bcls_rec->Tables["MM0099"].Rows[0]["OUT_STOCK_TIME"] = datetime;
				bcls_rec->Tables["MM0099"].Rows[0]["STOCK_NO"] = stockCode;
				bcls_rec->Tables["MM0099"].Rows[0]["STOCK_OPER_ORDER"] = "1Q";
				bcls_rec->Tables["MM0099"].Rows[0]["AIM_STORE"] = stockCode;
				bcls_rec->Tables["MM0099"].Rows[0]["TRNP_MODE_CODE"] = "";
				bcls_rec->Tables["MM0099"].Rows[0]["SYS_CODE"] = aimSysCode;
				doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(doFlag, s.msg, log.Location);
				}
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
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

	return doFlag;
}