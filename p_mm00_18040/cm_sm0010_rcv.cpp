/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     179648
Version:    1.0
Date:       2023年2月1日
Description: 厂外库入库
**************************************************/
//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"
int f_mm0099(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2F_ENTERACE_TELE(cm_sm0010_rcv)

int f_cm_sm0010_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CString instockCode = "";
	CString aimSysCode = "";
	CString oldSysCode = "";
	CString stackingType = ""; //(发货码单  1；正常转库码单2 ； 转金家码单 3
	CString stackingNo = "";
	CString vehicleNo = "";

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	CDbCommand cmd(conn);

	try
	{
		/* 获取输入参数 */
		matNo = bcls_rec->Tables[0].Rows[0]["CUST_MAT_NO"].ToString().Trim();
		instockCode = bcls_rec->Tables[0].Rows[0]["IN_STOCK_CODE"].ToString().Trim();
		matKind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		stackingNo = bcls_rec->Tables[0].Rows[0]["STACKING_NO"].ToString().Trim();
		vehicleNo = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "matNo [{0}],stockCode [{1}],matKind [{2}]", matNo, instockCode, matKind);

		if (matNo == "" || instockCode == "" || matKind == "")
		{
			strcpy(s.msg, "从电文获取的材料号/目的库区/物料类型不能为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		
		//厂外库入库是否要考虑一下销售发起的厂内库入库（待定）
		Log::Trace("", __FUNCTION__, "oldSysCode = [{0}]", oldSysCode);

		cmd.SetCommandText("SELECT TC_MARK FROM TSI0021 WHERE STOCK_NO = @instockCode");
		cmd.Parameters.Set("instockCode", instockCode);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			oldSysCode = cmd.GetString(1);
		}
		cmd.Close();

		Log::Trace("", __FUNCTION__, "oldSysCode = [{0}]", oldSysCode);

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
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "VEHICLE_NO");
			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
		}

		Log::Trace("", __FUNCTION__, "开始调用物料入库... ");

		/* 设置物料跟踪参数*/
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "SMR0";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_KIND"] = matKind;
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MM" + matKind;
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "厂外库入库";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = matNo;
		bcls_rec->Tables["MM0099"].Rows[0]["STOCK_NO"] = instockCode;
		bcls_rec->Tables["MM0099"].Rows[0]["VEHICLE_NO"] = vehicleNo;

		doFlag = f_mm0099(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(doFlag, s.msg, log.Location);
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