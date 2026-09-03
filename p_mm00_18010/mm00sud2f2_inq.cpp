/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      K17017
Version:     1.0
Date:        2022-07-06 10:33:43
Description: 查询在制品流向统计信息
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 查询物料滚动量信息
/// <para>
/// 查询物料滚动量信息
/// </para>
/// <para>数据库表：TMMSM01 板坯物料主表
///					TMMHR01 热轧物料主表
///					TMMCR01 冷轧物料主表         </para>
/// <para>主调用函数：前台MM00SUD2画面F2(查询)调用。   </para>
/// </summary>
/// <param name="">    </param>
/// <returns>具体调用函数返回的在制品流向统计信息</returns>
===========================================================</remark>*/

/******框架头******/
#include "stdafx.h"

/******service入口******/
BM2F_ENTERACE(mm00sud2f2_inq)


int f_mm00sud2f2_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
    /*打程序起止LOG*/
    CTracer log(__FUNCTION__);

    /*定义程序用变量*/
    int doFlag = 0;
    CString sqlstr = "";

    /****** 业务处理开始 ******/
    try {
        /*实体类定义*/


        /*数据库操作类定义*/
        CDbCommand cmd(conn);
        CDbCommand cmd_count(conn);


        CString v_mat_kind = "";//物料种类
        CString v_status = "";//去向、下游机组
        CDecimal v_yes_consign = 0;//有委托
        CDecimal v_no_consign = 0;//无委托
        CDecimal v_dummy_coil = 0;//返回卷

        CDecimal v_yes_consign_sm = 0;//板坯有委托小计
        CDecimal v_no_consign_sm = 0;//板坯无委托小计
        CDecimal v_yes_consign_hr = 0;//热卷有委托小计
        CDecimal v_no_consign_hr = 0;//热卷无委托小计
        CDecimal v_dummy_coil_hr = 0;//热卷返回卷小计
        CDecimal v_yes_consign_cr = 0;//冷卷有委托小计
        CDecimal v_no_consign_cr = 0;//冷卷无委托小计
        CDecimal v_dummy_coil_cr = 0;//冷卷返回卷小计
        CDecimal v_sum = 0;//总计


    /****** 获得输入参数 ******/

      //增加数据列
        bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_KIND");//物料种类
        bcls_ret->Tables[0].Columns.Add(DT_STRING, "DEST");//去向、下游机组
        bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "YES_CONSIGN");//有委托
        bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "NO_CONSIGN");//无委托
        bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DUMMY_COIL");//返回卷
        bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SUM");//总计


        /*定义查询SQL*/
        //板坯
        //查板坯所有去向
        CString cmd_query_sm_count = " SELECT DISTINCT MAT_STATUS FROM TMMSM01 WHERE MAT_STATUS <> '36' AND MAT_STATUS <> ' ' ";
        CString cmd_query_sm = " SELECT SUM(NVL(MAT_ACT_WT,0)) FROM TMMSM01 WHERE MAT_STATUS <> '36' AND MAT_STATUS <> ' ' ";//小计合计 

        CString cmd_query_sm_yes_consign = cmd_query_sm + " AND ORDER_NO > ' ' AND MAT_STATUS = @status ";//有委托
        CString cmd_query_sm_no_consign = cmd_query_sm + " AND ORDER_NO = ' ' AND MAT_STATUS = @status ";//无委托
        CString cmd_query_sm_yes_consign_xj = cmd_query_sm + " AND ORDER_NO <> ' ' ";//有委托小计
        CString cmd_query_sm_no_consign_xj = cmd_query_sm + " AND ORDER_NO = ' ' ";//无委托小计

        //热卷
        CString cmd_query_hr_count = " SELECT DISTINCT MAT_STATUS FROM TMMHR01 WHERE MAT_STATUS <> '36' AND MAT_STATUS <> ' ' ";
        CString cmd_query_hr = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMHR01 WHERE MAT_STATUS <> '36' AND MAT_STATUS <> ' ' ";//小计合计 

        CString cmd_query_hr_yes_consign = cmd_query_hr + " AND ORDER_NO > ' ' ";
        CString cmd_query_hr_no_consign = cmd_query_hr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' ";
        CString cmd_query_hr_dummy_coil = cmd_query_hr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' ";
        CString cmd_query_hr_yes_consign_xj = cmd_query_hr + " AND ORDER_NO > ' ' ";
        CString cmd_query_hr_no_consign_xj = cmd_query_hr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' ";
        CString cmd_query_hr_dummy_coil_xj = cmd_query_hr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' ";

        //冷卷
        CString cmd_query_cr_count = " SELECT DISTINCT MAT_STATUS FROM TMMCR01 WHERE MAT_STATUS <> '36' AND MAT_STATUS <> ' ' ";
        CString cmd_query_cr = " SELECT SUM(NVL(MAT_WT,0)) FROM TMMCR01 WHERE MAT_STATUS <> '36' AND MAT_STATUS <> ' ' ";//小计合计

        CString cmd_query_cr_yes_consign = cmd_query_cr + " AND ORDER_NO > ' ' "
            " AND (MAT_STATUS = @status ) "
            ;
        CString cmd_query_cr_no_consign = cmd_query_cr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' "
            " AND (MAT_STATUS = @status ) "
            ;
        CString cmd_query_cr_dummy_coil = cmd_query_cr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' "
            " AND (SUBSTR(PLAN_NO,1,4) = @status AND SUBSTR(PLAN_NO,1,4) <> ' ' "
            " OR (SUBSTR(PLAN_NO,1,4) = ' ' AND MAT_STATUS = @status)) "
            ;
        CString cmd_query_cr_yes_consign_xj = cmd_query_cr + " AND ORDER_NO > ' ' AND MAT_STATUS <> ' ' ";
        CString cmd_query_cr_no_consign_xj = cmd_query_cr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG = '0' AND MAT_STATUS <> ' ' ";
        CString cmd_query_cr_dummy_coil_xj = cmd_query_cr + " AND ORDER_NO = ' ' AND DUMMY_COIL_FLAG <> '0' AND MAT_STATUS <> ' ' ";

        //执行sql语句
        cmd_count.SetCommandText(cmd_query_sm_count);
        cmd_count.ExecuteReader();
        int i = 0;
        while (cmd_count.Read())
        {
            v_status = cmd_count.GetString(1);

            bcls_ret->Tables[0].Rows.Add();
            bcls_ret->Tables[0].Rows[i][0] = "板坯";
            bcls_ret->Tables[0].Rows[i][1] = v_status;

            cmd.SetCommandText(cmd_query_sm_yes_consign);
            cmd.Parameters.Clear();
            cmd.Parameters.Set("status", v_status);
            cmd.ExecuteReader();
            if (cmd.Read())
            {
                v_yes_consign = cmd.GetDecimal(1).Floor();
                bcls_ret->Tables[0].Rows[i][2] = v_yes_consign;
            }
            cmd.Close();

            cmd.SetCommandText(cmd_query_sm_no_consign);
            cmd.Parameters.Clear();
            cmd.Parameters.Set("status", v_status);
            cmd.ExecuteReader();
            if (cmd.Read())
            {
                v_no_consign = cmd.GetDecimal(1).Floor();
                bcls_ret->Tables[0].Rows[i][3] = v_no_consign;
            }
            cmd.Close();

            v_sum = v_yes_consign + v_no_consign;
            bcls_ret->Tables[0].Rows[i][5] = v_sum;

            i++;
        }
        cmd_count.Close();

        bcls_ret->Tables[0].Rows.Add();//增加小计行
        bcls_ret->Tables[0].Rows[i][0] = "板坯";
        bcls_ret->Tables[0].Rows[i][1] = "小计";
        //有委托小计
        cmd.SetCommandText(cmd_query_sm_yes_consign_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_yes_consign_sm = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][2] = v_yes_consign_sm;
        }
        cmd.Close();

        //无委托小计
        cmd.SetCommandText(cmd_query_sm_no_consign_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_no_consign_sm = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][3] = v_no_consign_sm;
        }
        cmd.Close();

        //小计合计
        bcls_ret->Tables[0].Rows[i][5] = v_yes_consign_sm + v_no_consign_sm;

        i++;

        //热卷
        cmd_count.SetCommandText(cmd_query_hr_count);
        cmd_count.ExecuteReader();
        while (cmd_count.Read())
        {
            v_status = cmd_count.GetString(1);

            bcls_ret->Tables[0].Rows.Add();
            bcls_ret->Tables[0].Rows[i][0] = "热卷";
            bcls_ret->Tables[0].Rows[i][1] = v_status;

            if (v_status.Trim().GetLength() == 0)
            {
                cmd.SetCommandText(cmd_query_hr_yes_consign);
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_yes_consign = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][2] = v_yes_consign;
                }
                cmd.Close();

                cmd.SetCommandText(cmd_query_hr_no_consign);
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_no_consign = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][3] = v_no_consign;
                }
                cmd.Close();

                cmd.SetCommandText(cmd_query_hr_dummy_coil + " AND SUBSTR(PLAN_NO,1,4) = ' ' AND WHOLE_BACKLOG_CODE = ' ' ");
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_dummy_coil = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][4] = v_dummy_coil;
                }
                cmd.Close();

                v_sum = v_yes_consign + v_no_consign + v_dummy_coil;
                bcls_ret->Tables[0].Rows[i][5] = v_sum;
            }
            else if (v_status.Trim().GetLength() == 2)
            {
                sqlstr = cmd_query_hr_yes_consign + " AND MAT_STATUS = @status ";
                Log::Trace("", __FUNCTION__, "sqlstr_hr1 = [{0}]", sqlstr);
                cmd.SetCommandText(sqlstr);
                cmd.Parameters.Clear();
                cmd.Parameters.Set("status", v_status);
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_yes_consign = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][2] = v_yes_consign;
                }
                cmd.Close();

                sqlstr = cmd_query_hr_no_consign + " AND MAT_STATUS = @status ";
                Log::Trace("", __FUNCTION__, "sqlstr_hr2 = [{0}]", sqlstr);
                cmd.SetCommandText(sqlstr);
                cmd.Parameters.Clear();
                cmd.Parameters.Set("status", v_status);
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_no_consign = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][3] = v_no_consign;
                }
                cmd.Close();

                //	  		cmd.SetCommandText(cmd_query_hr_dummy_coil + " AND MAT_DESTION = @status ");
                //	  		cmd.Parameters.Clear();
                //	  		cmd.Parameters.Set("status",v_status);
                //	  		cmd.ExecuteReader();
                //	  		if(cmd.Read())
                //	  		{
                //	  			v_dummy_coil = cmd.GetDecimal(1).Floor();
                //	  			bcls_ret->Tables[0].Rows[i][4] = v_dummy_coil;
                //	  		}
                //	  		cmd.Close();
                //  		
                //	  		v_sum = v_yes_consign + v_no_consign + v_dummy_coil;
                v_sum = v_yes_consign + v_no_consign;
                bcls_ret->Tables[0].Rows[i][5] = v_sum;

            }
            else if (v_status.Trim().GetLength() == 4)
            {
                cmd.SetCommandText(cmd_query_hr_yes_consign + " AND MAT_STATUS = @status ");
                cmd.Parameters.Clear();
                cmd.Parameters.Set("status", v_status);
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_yes_consign = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][2] = v_yes_consign;
                }
                cmd.Close();

                cmd.SetCommandText(cmd_query_hr_no_consign + " AND MAT_STATUS = @status ");
                cmd.Parameters.Clear();
                cmd.Parameters.Set("status", v_status);
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_no_consign = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][3] = v_no_consign;
                }
                cmd.Close();

                cmd.SetCommandText(cmd_query_hr_dummy_coil + " AND (MAT_STATUS = @status AND SUBSTR(PLAN_NO,1,4) > ' ' "
                    " OR (SUBSTR(PLAN_NO,1,4) = ' ' AND MAT_STATUS = @status)) ");
                cmd.Parameters.Clear();
                cmd.Parameters.Set("status", v_status);
                cmd.ExecuteReader();
                if (cmd.Read())
                {
                    v_dummy_coil = cmd.GetDecimal(1).Floor();
                    bcls_ret->Tables[0].Rows[i][4] = v_dummy_coil;
                }
                cmd.Close();

                v_sum = v_yes_consign + v_no_consign + v_dummy_coil;
                bcls_ret->Tables[0].Rows[i][5] = v_sum;
            }

            i++;
        }
        cmd_count.Close();

        bcls_ret->Tables[0].Rows.Add();//增加小计行
        bcls_ret->Tables[0].Rows[i][0] = "热卷";
        bcls_ret->Tables[0].Rows[i][1] = "小计";
        //有委托小计
        cmd.SetCommandText(cmd_query_hr_yes_consign_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_yes_consign_hr = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][2] = v_yes_consign_hr;
        }
        cmd.Close();

        //无委托小计
        cmd.SetCommandText(cmd_query_hr_no_consign_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_no_consign_hr = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][3] = v_no_consign_hr;
        }
        cmd.Close();

        //返回卷小计
        cmd.SetCommandText(cmd_query_hr_dummy_coil_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_dummy_coil_hr = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][4] = v_dummy_coil_hr;
        }
        cmd.Close();

        //小计合计
        bcls_ret->Tables[0].Rows[i][5] = v_yes_consign_hr + v_no_consign_hr + v_dummy_coil_hr;

        i++;

        //冷卷
        cmd_count.SetCommandText(cmd_query_cr_count);
        cmd_count.ExecuteReader();
        while (cmd_count.Read())
        {
            v_status = cmd_count.GetString(1);

            bcls_ret->Tables[0].Rows.Add();
            bcls_ret->Tables[0].Rows[i][0] = "冷卷";
            bcls_ret->Tables[0].Rows[i][1] = v_status;

            cmd.SetCommandText(cmd_query_cr_yes_consign);
            cmd.Parameters.Clear();
            cmd.Parameters.Set("status", v_status);
            cmd.ExecuteReader();
            if (cmd.Read())
            {
                v_yes_consign = cmd.GetDecimal(1).Floor();
                bcls_ret->Tables[0].Rows[i][2] = v_yes_consign;
            }
            cmd.Close();

            cmd.SetCommandText(cmd_query_cr_no_consign);
            cmd.Parameters.Clear();
            cmd.Parameters.Set("status", v_status);
            cmd.ExecuteReader();
            if (cmd.Read())
            {
                v_no_consign = cmd.GetDecimal(1).Floor();
                bcls_ret->Tables[0].Rows[i][3] = v_no_consign;
            }
            cmd.Close();

            cmd.SetCommandText(cmd_query_cr_dummy_coil);
            cmd.Parameters.Clear();
            cmd.Parameters.Set("status", v_status);
            cmd.ExecuteReader();
            if (cmd.Read())
            {
                v_dummy_coil = cmd.GetDecimal(1).Floor();
                bcls_ret->Tables[0].Rows[i][4] = v_dummy_coil;
            }
            cmd.Close();

            v_sum = v_yes_consign + v_no_consign + v_dummy_coil;
            bcls_ret->Tables[0].Rows[i][5] = v_sum;

            i++;
        }
        cmd_count.Close();

        bcls_ret->Tables[0].Rows.Add();//增加小计行
        bcls_ret->Tables[0].Rows[i][0] = "冷卷";
        bcls_ret->Tables[0].Rows[i][1] = "小计";
        //有委托小计
        cmd.SetCommandText(cmd_query_cr_yes_consign_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_yes_consign_cr = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][2] = v_yes_consign_cr;
        }
        cmd.Close();

        //无委托小计
        cmd.SetCommandText(cmd_query_cr_no_consign_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_no_consign_cr = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][3] = v_no_consign_cr;
        }
        cmd.Close();

        //返回卷小计
        cmd.SetCommandText(cmd_query_cr_dummy_coil_xj);
        cmd.ExecuteReader();
        if (cmd.Read())
        {
            v_dummy_coil_cr = cmd.GetDecimal(1).Floor();
            bcls_ret->Tables[0].Rows[i][4] = v_dummy_coil_cr;
        }
        cmd.Close();

        //小计合计
        bcls_ret->Tables[0].Rows[i][5] = v_yes_consign_cr + v_no_consign_cr + v_dummy_coil_cr;

        i++;
        bcls_ret->Tables[0].Rows.Add();//增加最末合计行
        //最末行合计
        bcls_ret->Tables[0].Rows[i][0] = "合计";
        bcls_ret->Tables[0].Rows[i][2] = v_yes_consign_sm + v_yes_consign_hr + v_yes_consign_cr;
        bcls_ret->Tables[0].Rows[i][3] = v_no_consign_sm + v_no_consign_hr + v_no_consign_cr;
        bcls_ret->Tables[0].Rows[i][4] = v_dummy_coil_hr + v_dummy_coil_cr;
        bcls_ret->Tables[0].Rows[i][5] = v_yes_consign_sm + v_yes_consign_hr + v_yes_consign_cr
            + v_no_consign_sm + v_no_consign_hr + v_no_consign_cr
            + v_dummy_coil_hr + v_dummy_coil_cr;

    }
    catch (const CApplicationException& ex)
    {
        strcpy(s.msg, ex.GetMsg());  //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
        s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
        doFlag = -1;
    }

    catch (const CException& ex)
    {
        strcpy(s.msg, ex.GetMsg());  //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.msg参数对应
        s.flag = ex.GetCode();       //返回前台，与EI.EITuxedo.CallService()方法返回的EI.EIInfo对象的sys_info.flag参数对应
        doFlag = -1;
    }

    //返回-1时事务将回滚，返回为0是事务将提交
    return doFlag;
}