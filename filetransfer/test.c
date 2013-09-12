#include"utils.h"
#include"transfile.h"
#include"filetransfer_conf.h"

void test_address_dirname()
{
	char host[256];
	char port[16];
	const char* path="192.168.0.8:8080";
	const char* file="/home/nick/test";
	printf("address is %s\n",sock_address(host,path));
	printf("port is %s\n",sock_port(port,path));

	printf("director is %s\n",dirname(file));
}
void test_filetransfer_conf_read()
{
	struct FileTransferConf conf;
	filetransfer_conf_read(&conf,"./filetransfer.conf");
	printf("host=%s\n",conf.host);
	printf("port=%s\n",conf.port);
	printf("workdir=%s\n",conf.workdir);
	printf("passwd=%s\n",conf.passwd);
}
void test_set_work_dir(const char* dir)
{
	//const char* dir="/tmp";
	fprintf(stderr,"dir=%s\n",dir);

	if(set_work_dir(dir)<0)
		fprintf(stderr,"set_work_dir() failed\n");
	else
		fprintf(stderr,"set_work_dir() successed\n");

}
void test_log_error()
{
	log_msg("log_msg");
	log_sys("log_sys,%s","hello,world");
}


int main(int argc,char **argv)
{
	test_address_dirname();
	test_filetransfer_conf_read();
	test_set_work_dir(argv[1]);
	test_log_error();
	
	return 0;
}
