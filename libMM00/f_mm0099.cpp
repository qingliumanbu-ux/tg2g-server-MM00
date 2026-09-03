/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2013-05-24
Description:物料跟踪总入口
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 物料跟踪总入口
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明
#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) || defined(_LINE_SF)
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
#if defined(_LINE_HR) || defined(_LINE_CR)
BM2_FUNCTION_IMPORT
int f_mmhr99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
#if defined(_LINE_CR) 
BM2_FUNCTION_IMPORT
int f_mmcr99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
#if defined(_LINE_HP) 
BM2_FUNCTION_IMPORT
int f_mmhp99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
#if defined(_LINE_BW) 
BM2_FUNCTION_IMPORT
int f_mmbw99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
#if defined(_LINE_SF) 
BM2_FUNCTION_IMPORT
int f_mmsf99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

BM2_FUNCTION_EXPORT
int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_mat_kind("");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 判断是否存在指定块 */
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			strcpy(s.msg, "未传入数据块MM0099！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() == 0)
		{
			strcpy(s.msg, "没有传入物料跟踪数据！");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 获取输入参数 */
		cs_mat_kind = bcls_rec->Tables["MM0099"].Rows[0]["MAT_KIND"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "111 MAT_KIND	= [{0}]", (const char*)cs_mat_kind);

		/* 检查输入参数合法性 */
		if (cs_mat_kind.Trim() == "")
		{
			strcpy(s.msg, _RES("MM00S0000143")/*数据校验出错，物料类型为空*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (cs_mat_kind.Trim() != "SM"
			&& cs_mat_kind.Trim() != "HR"
			&& cs_mat_kind.Trim() != "CR"
			&& cs_mat_kind.Trim() != "HP"
			&& cs_mat_kind.Trim() != "BW"
			&& cs_mat_kind.Trim() != "SF")
		{
			strcpy(s.msg, "物料类型MAT_KIND" + cs_mat_kind + "错误!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		/* 程序处理 */
#if defined(_LINE_SM) || defined(_LINE_HR) || defined(_LINE_HP) || defined(_LINE_BW) || defined(_LINE_SF)
		if (cs_mat_kind.Trim() == "SM")
		{
			Log::Trace("", __FUNCTION__, "f_mmsm99 cs_mat_kind	= [{0}]", (const char*)cs_mat_kind);
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
#endif

#if defined(_LINE_HR) || defined(_LINE_CR)
		if (cs_mat_kind.Trim() == "HR")
		{
			doFlag = f_mmhr99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

#endif

#if defined(_LINE_CR)
		if (cs_mat_kind.Trim() == "CR")
		{
			doFlag = f_mmcr99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

#endif

#if defined(_LINE_HP)
		if (cs_mat_kind.Trim() == "HP")
		{
			doFlag = f_mmhp99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

#endif

#if defined(_LINE_BW)
		if (cs_mat_kind.Trim() == "BW")
		{
			doFlag = f_mmbw99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
#endif

#if defined(_LINE_TR)
		if (cs_mat_kind.Trim() == "TR")
		{
			//doFlag = f_mmtr99(bcls_rec, bcls_ret,conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
#endif

#if defined(_LINE_SF)
		if (cs_mat_kind.Trim() == "SF")
		{
			doFlag = f_mmsf99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
#endif
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
