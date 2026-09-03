/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-07-24
Description: 计算材料外径
**************************************************/
//框架头文件
#include "stdafx.h" 
#include <math.h>

/*<remark>=========================================================
/// <summary>
/// 计算材料外径
/// <para>
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明
int f_mm0014(CDecimal  w_weight,			/* Weight 		    (t)  单位是吨  */
					CDecimal  w_width,				/* Width		    (mm)    */
					CDecimal w_india,				/* Coil inside diameter	 (mm)    */
					CDecimal  w_density,			/* Material density (g/cm3) */
					CDecimal&  w_outdia,			/* Coil outside diameter (mm)    */
					CDbConnection * conn)
{
	int doFlag = 0;
	double w_dub1;
	double w_dub2;
	CString sqlstr = " ";
	try
	{
		if (w_width <= 0 || w_india <= 0 || w_density <= 0)
		{
			sprintf(s.msg, "输入参数：内径、厚、密度不能为0");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		Log::Trace("", __FUNCTION__, "出口重：w_weight = [{0}] ", w_weight);
		Log::Trace("", __FUNCTION__, "出口宽：w_width = [{0}] ", w_width);
		Log::Trace("", __FUNCTION__, "出口内径：w_india = [{0}] ", w_india);
		Log::Trace("", __FUNCTION__, "出口密度：w_density = [{0}] ", w_density);

		w_dub1 = w_weight.ToDouble() * 1.0e3* 1.0e3;	//转换为克
		w_dub2 = 3.14159 * w_density.ToDouble() * (w_width.ToDouble() / 1000.0);
		w_dub1 = w_dub1 / w_dub2;
		w_dub2 = pow(w_india.ToDouble() / 2.0, 2.0);
		w_dub1 = 2.0 * sqrt(w_dub1 + w_dub2);
		w_outdia = w_dub1;
		w_outdia = w_outdia.Round(0);
		Log::Trace("", __FUNCTION__, "出口外径：w_outdia = [{0}] ", w_outdia);
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
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


