/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      178053
Version:     1.0
Date:        2019-11-29 11:05:23
Description: 物料通用跨系统对账配置信息维护
**************************************************/

//框架头文件
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 物料通用跨系统对账配置信息维护
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件


BM2F_ENTERACE(mm00si02f6_pro)


int f_mm00si02f6_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel tmm00si02("TMM00SI02");

	/* 数据库SQL操作字符串 */
	CString sqlstr("");
	CString sqlstr_temp("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		// 新增事件
		if (bcls_rec->Tables.IndexOf("MM00SI02_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM00SI02_INS"].Rows.get_Count(); i++)
			{
				/* 获取输入参数 */
				tmm00si02.Reset();
				tmm00si02.MergeFrom(bcls_rec->Tables["MM00SI02_INS"].Rows[i]);
				tmm00si02.TrimOrBlank();

				/* 打印输入参数 */
				Log::Trace("", __FUNCTION__, "tmm00si02.SYS_CODE		= [{0}]", tmm00si02["SYS_CODE"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.MAT_KIND		= [{0}]", tmm00si02["MAT_KIND"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.FACTORY_DIV		= [{0}]", tmm00si02["FACTORY_DIV"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.ITEM_ENAME		= [{0}]", tmm00si02["ITEM_ENAME"].ToString());

				/* 检查输入参数合法性 */
				if (tmm00si02["SYS_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, "系统不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["MAT_KIND"].ToString().Trim() == "")
				{
					strcpy(s.msg, "物料种类不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["FACTORY_DIV"].ToString().Trim() == "")
				{
					strcpy(s.msg, "厂别区分不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["ITEM_ENAME"].ToString().Trim() == "")
				{
					strcpy(s.msg, "字段英文名不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["ITEM_CNAME"].ToString().Trim() == "")
				{
					strcpy(s.msg, "字段中文名不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["ITEM_KIND"].ToString().Trim() == "")
				{
					strcpy(s.msg, "字段种类不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				/* 新增信息 */
				tmm00si02["REC_CREATE_TIME"] = datetime;
				tmm00si02["REC_CREATOR"] = s.userid;
				tmm00si02.TrimOrBlank();
				tmm00si02.Insert();
			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MM00SI02_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM00SI02_DEL"].Rows.get_Count(); i++)
			{
				/* 获取输入参数 */
				tmm00si02.Reset();
				tmm00si02.MergeFrom(bcls_rec->Tables["MM00SI02_DEL"].Rows[i]);
				tmm00si02.TrimOrBlank();

				/* 打印输入参数 */
				Log::Trace("", __FUNCTION__, "tmm00si02.SYS_CODE		= [{0}]", tmm00si02["SYS_CODE"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.MAT_KIND		= [{0}]", tmm00si02["MAT_KIND"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.FACTORY_DIV		= [{0}]", tmm00si02["FACTORY_DIV"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.ITEM_ENAME		= [{0}]", tmm00si02["ITEM_ENAME"].ToString());

				/* 删除信息 */
				tmm00si02.Delete("SYS_CODE,MAT_KIND,FACTORY_DIV,ITEM_ENAME");
			}
		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MM00SI02_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MM00SI02_UPD"].Rows.get_Count(); i++)
			{
				/* 获取输入参数 */
				tmm00si02.Reset();
				tmm00si02.MergeFrom(bcls_rec->Tables["MM00SI02_UPD"].Rows[i]);
				tmm00si02.TrimOrBlank();

				/* 打印输入参数 */
				Log::Trace("", __FUNCTION__, "tmm00si02.SYS_CODE		= [{0}]", tmm00si02["SYS_CODE"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.MAT_KIND		= [{0}]", tmm00si02["MAT_KIND"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.FACTORY_DIV		= [{0}]", tmm00si02["FACTORY_DIV"].ToString());
				Log::Trace("", __FUNCTION__, "tmm00si02.ITEM_ENAME		= [{0}]", tmm00si02["ITEM_ENAME"].ToString());

				/* 检查输入参数合法性 */
				if (tmm00si02["SYS_CODE"].ToString().Trim() == "")
				{
					strcpy(s.msg, "系统不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["MAT_KIND"].ToString().Trim() == "")
				{
					strcpy(s.msg, "物料种类不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["FACTORY_DIV"].ToString().Trim() == "")
				{
					strcpy(s.msg, "厂别区分不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["ITEM_ENAME"].ToString().Trim() == "")
				{
					strcpy(s.msg, "字段英文名不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["ITEM_CNAME"].ToString().Trim() == "")
				{
					strcpy(s.msg, "字段中文名不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (tmm00si02["ITEM_KIND"].ToString().Trim() == "")
				{
					strcpy(s.msg, "字段种类不能为空");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				/* 修改信息 */
				tmm00si02["REC_REVISE_TIME"] = datetime;
				tmm00si02["REC_REVISOR"] = s.userid;
				tmm00si02.TrimOrBlank();
				tmm00si02.Update("REC_REVISE_TIME,REC_REVISOR,SYS_CODE,MAT_KIND,FACTORY_DIV,ITEM_ENAME,ITEM_CNAME,ITEM_KIND",
								 "SYS_CODE,MAT_KIND,FACTORY_DIV,ITEM_ENAME");
			}
		}
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


