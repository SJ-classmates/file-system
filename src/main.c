#include "sys.h"
#include "file_operator.h"
#include "cmd_handle.h"
#include "arg_handle.h"

int main(int argc, char **argv)
{
	block_init();
	file_desc_table_init();
	file_system_init();

	while(1)
	{
		display_current_path();
		get_arg();
		arg_handle();
	}
	return 0;
}

/******************************
 *@autor:SJ                   *
 *@Time:2020/8/13 17:16:53    *
 ******************************/
