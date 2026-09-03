/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2018-12-12
Description: 材料标签信息打印处理
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 材料标签信息打印处理
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(mm0089a1b1_pro)

int f_mm0089a1b1_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	

	/* 实体类定义 */ 
	CModel tmm0089("TMM0089");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		

		/* 获取输入参数 */
		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			tmm0089["IN_MAT_NO"]		= bcls_rec->Tables[0].Rows[i]["IN_MAT_NO"].ToString().Trim();
			tmm0089["SAMPLE_LOT_NO"]	= bcls_rec->Tables[0].Rows[i]["SAMPLE_LOT_NO"].ToString().Trim();

			/* 打印传入参数 */
			Log::Trace("", __FUNCTION__, "打印传入参数 tmm0089.IN_MAT_NO		= [{0}]", (const char*)tmm0089["IN_MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "打印传入参数 tmm0089.SAMPLE_LOT_NO	= [{0}]", (const char*)tmm0089["SAMPLE_LOT_NO"].ToString());

			/* 检查输入参数合法性 */
			if (tmm0089["IN_MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"材料号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}	  

			/* 查询材料信息*/
			tmm0089["PRINT_FLAG"]	= "Y";
			tmm0089["PRINT_MAKER"] = s.userid;
			tmm0089["PRINT_DATE"]	= datetime;
			tmm0089["PRINT_NUM"]   = tmm0089["PRINT_NUM"].ToDecimal() + 1;
			tmm0089["REC_REVISOR"] = s.userid;
			tmm0089["REC_REVISE_TIME"] = datetime;
			tmm0089.TrimOrBlank();
			tmm0089.Update( "PRINT_FLAG,"
							"PRINT_DATE,"
							"PRINT_MAKER,"
							"PRINT_NUM,"
							"REC_REVISOR,"
							"REC_REVISE_TIME",
							"IN_MAT_NO,SAMPLE_LOT_NO");
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



