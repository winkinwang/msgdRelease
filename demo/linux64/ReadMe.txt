һ������׼����
��Ҫ��msgdRelease/lib/linux64�е�libmsgd.so  libos.so libtrpc.so�ŵ�ϵͳ/usr/lib/�ȿ��ҵ���ϵͳĿ¼�£�
Ҳ��ʹ��export LD_LIBRARY_PATH=$(Ŀ¼·��)/lib/linux64  �����ӿ��ļ�����·����

��������
��������ִ�У�cd $(Ŀ¼·��)/demo/linux64

1������˳���
./msgd_server -h�鿴��������
./msgd_server -p 7897   ����7897�˿����з������

2���ͻ��˳���
���������У�./msgd_client svrip svrport  localGroup localUser dstGroup dstUser persistTimeSec
������������£�
svrip��ָmsgd_server���е�����IP
svrport��ָmsgd_server���е����������Ķ˿�
localGroup�����ڷ�����ע����û���
localUser �����ڷ�����ע����û���
dstGroup��������Ŀ�Ľ��ն˵��û���
dstUser ��������Ŀ�Ľ��ն˵��û���
persistTimeSecĿ������û�������ʱ������������ʱ����
./msgd_client 127.0.0.1 7897 auto test auto dest 20
