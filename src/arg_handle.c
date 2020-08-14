#include "sys.h"
#include "file_operator.h"
#include "cmd_handle.h"
#include "arg_handle.h"

char *arg[ARG_CMD_LENGTH],cmd[ARG_CMD_LENGTH];
int arg_length;

/*********************************************************************
 * @fn      get_arg
 *
 * @brief   获取一行命令
 *
 * @param   none
 *
 * @return  none
 */
void get_arg(void)
{
	arg_length = 0;
	if(fgets(cmd, ARG_CMD_LENGTH, stdin)!=NULL)
	{
		arg[arg_length]=strtok(cmd, " ");
		while(arg[arg_length]!=NULL)
		{
			++arg_length;
			arg[arg_length]=strtok(NULL, " ");
		}

		//处理最后一个换行符
		if(arg[arg_length-1][0]=='\n')
		{
			arg_length = arg_length-2;
		}
		else
		{
			arg_length = arg_length-1;
			arg[arg_length][strlen(arg[arg_length])-1] = '\0';
			//printf("len=%d\n", arg_length);
		}
	}
}

/*********************************************************************
 * @fn      arg_handle
 *
 * @brief   处理命令
 *
 * @param   none
 *
 * @return  none
 */
void arg_handle(void)
{
	int fd,len;
	char buffer[1024];

	if(strcmp(arg[0], "ls")==0)
	{
		List_file();
	}
	else if(strcmp(arg[0], "touch")==0)
	{
		if(arg_length<2)
		{
			printf("USAGE:touch filename filedata\n");
		}
		else
		{
			create_file(arg[1], arg[2], strlen(arg[2]));
		}
	}
	else if(strcmp(arg[0], "mkdir")==0)
	{
		if(arg_length<1)
		{
			printf("USAGE:mkdir dirname\n");
		}
		else
		{
			create_dir(arg[1]);
		}
	}
	else if(strcmp(arg[0], "rmdir")==0)
	{
		if(arg_length<1)
		{
			printf("USAGE:rmdir dirname\n");
		}
		else
		{
			remove_dir(arg[1]);
		}
	}
	else if(strcmp(arg[0], "rm")==0)
	{
		if(arg_length<1)
		{
			printf("USAGE:rm filename\n");
		}
		else
		{
			rm_file(arg[1]);
		}
	}
	else if(strcmp(arg[0], "cd")==0)
	{
		if(arg_length<1)
		{
			printf("USAGE:cd dirname\n");
		}
		else
		{
			change_dir(arg[1]);
		}
	}
	else if(strcmp(arg[0], "cat")==0)
	{
		if(arg_length<1)
		{
			printf("USAGE:cat filename\n");
		}
		else
		{
			cat_file(arg[1]);
		}
	}
	else if(strcmp(arg[0], "open")==0)
	{
		if(arg_length<1)
		{
			printf("USAGE:open filename\n");
		}
		else
		{
			fd = file_open(arg[1], 0);
			if(fd>0)
			{
				printf("%s->fd=%d\n", arg[1], fd);
			}
		}
	}
	else if(strcmp(arg[0], "read")==0)
	{
		if(arg_length<2)
		{
			printf("USAGE:read file_fd buffer_length\n");
		}
		else
		{
			fd  = atoi(arg[1]);
			len = atoi(arg[2]);
			len = len>sizeof(buffer)?sizeof(buffer):len;
			len = file_read(fd, buffer, len);
			if(len>0)
			{
				buffer[len] = '\0';
				printf("read:%dbyte\n", len);
				printf("read:%s\n", buffer);
			}
		}
	}
	else if(strcmp(arg[0], "write")==0)
	{
		if(arg_length<2)
		{
			printf("USAGE:read file_fd write_data\n");
		}
		else
		{
			fd  = atoi(arg[1]);
			len = file_write(fd, arg[2], strlen(arg[2]));
			printf("write:%dbyte\n", len);
		}
	}
	else if(strcmp(arg[0], "close")==0)
	{
		if(arg_length<1)
		{
			printf("USAGE:close file_fd\n");
		}
		else
		{
			file_close(atoi(arg[1]));
		}
	}
	else if(strcmp(arg[0], "quit")==0)
	{
        exit(0);
	}
	else
    {
        printf("No such cmd:%s\n", arg[0]);
    }
}
