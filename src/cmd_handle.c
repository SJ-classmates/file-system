#include "sys.h"
#include "file_operator.h"
#include "cmd_handle.h"

/*********************************************************************
 * @fn      cat_file
 *
 * @brief   cat命令 将文件内容分进行打印输出
 *
 * @param   filename:文件名
 *
 * @return  none
 */
void cat_file(char *file_name)
{
	int flag=0;
	File_Dir_Node_t *file_node = Current_Dir->dir_next;
	block_t *block_node;

	while(file_node!=NULL)
	{
		if(strcmp(file_node->name, file_name)==0)
		{
			flag=1;
			break;
		}
		else
		{
			file_node = file_node->next;//同级目录下的下一个文件
		}
	}

	if(flag==0)
	{
		printf("No such file:%s\n", file_name);
	}
	else
	{
		block_node = file_node->block;
		while(block_node!=NULL)
		{
			printf("%s", block_node->data);
			block_node = block_node->next;
		}

		printf("\n");
	}
}

/*********************************************************************
 * @fn      List_file
 *
 * @brief   ls命令 将当前目录下的文件/目录输出显示
 *
 * @param   none
 *
 * @return  none
 */
void List_file(void)
{
	int flag=0;
	File_Dir_Node_t *file_node = Current_Dir->dir_next;

	while(file_node!=NULL)
	{
		flag=1;
		printf("%s  ", file_node->name);
		file_node = file_node->next;//同一个目录下的下一个文件
	}

	if(flag)
	{
		printf("\n");
	}
}

/*********************************************************************
 * @fn      change_dir
 *
 * @brief   cd命令 改变当前所在的目录
 *
 * @param   dir_name:目录名字
 *
 * @return  none
 */
void change_dir(char *dir_name)
{
	int flag=0;
	File_Dir_Node_t *dir_node = Current_Dir->dir_next;

	if(strcmp(dir_name, "..")==0)
	{
		if(Current_Dir->last!=NULL)
		{
			Current_Dir = Current_Dir->last;
		}
	}
	else if(strcmp(dir_name, ".")==0)
	{

	}
	else
	{
		//从目录中找到
		while(dir_node!=NULL)
		{
			if(dir_node->type==TYPE_DIR&&strcmp(dir_node->name, dir_name)==0)
			{
				flag=1;
				break;
			}
			else
			{
				dir_node = dir_node->next;
			}
		}

		if(flag==0)
		{
			printf("No such dir:%s\n", dir_name);
		}
		else
		{
			Current_Dir = dir_node;
		}
	}
}
