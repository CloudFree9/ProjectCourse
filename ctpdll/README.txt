1. 说明
    以下所有说明基于Windows 10，  Windows 7或Windows Server 2012/2016类似，但需要自行验证。
    所有软件安装假定未缺省安装，以C盘为系统盘。

2. 准备工作

    2.1 下载安装MS Visual Studio 2017社区或更高版本，包含 Visual C++ Windows 系统编程组件。
    2.2 下载安装MT5官网最新版本。
    2.3 下载安装erlang官网最新版本 otp_XXXX.exe。
    2.4 下载安装RabbitMQ 3.7.7以上版本，并激活所有插件
          C:\Program Files\RabbitMQ Server\rabbitmq_server-3.7.7\sbin>rabbitmq-plugins.bat enable --all
                   
    2.6 从GITHUB上将本项目代码克隆到C:\workspace
    2.5 更新MT5
          将 MQL5目录下的所有文件目录复制到MT5的安装目录同等位置：
          %USERPROFILE%\AppData\Roaming\MetaQuotes\Terminal\D0E8209F77C8CF37AD8BF550E51FF075\MQL5


3. 联机实验
    3.1 检查Rabbit MQ服务，确保其正常启动
    3.2 浏览 http://127.0.0.1:15672 , 确保RabbitMQ的web管理终端能够打开，工作正常
    3.3 启动VisualStudio并打开 C:\worspace\ctpdll\ctpdll\ctpdll.sln ，重新构建三个项目
    3.4 启动测试数据生成程序
          c:\workspace\ctpdll\lib\fakeproducer.exe  :   上期所未开盘时，启动该程序生成伪随机报价数据流
          c:\workspace\ctpdll\lib\ctpdlltest.exe  :   上期所开盘时，启动该程序生成真实报价数据流
    3.5 启动MT5，打开 MQL5\Files\CustomSymbols里设置的交易品种，观看动态K线变化