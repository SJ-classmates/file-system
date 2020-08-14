#include "sys.h"
#include "file_operator.h"

/*********************************************************************
 * @fn      file_open
 *
 * @brief   打开一个文件
 *
 * @param   filename:文件名 mode:权限
 *
 * @return  成功返回fd,失败返回-1
 */
int file_open(char *filename, int mode)
{
	int i,j,flag=0;

	File_Dir_Node_t *dir_file_node = Current_Dir->dir_next;

	while(dir_file_node!=NULL)
	{
		if(dir_file_node->type!=TYPE_DIR&&strcmp(filename, dir_file_node->name)==0)
		{
			flag=1;
			break;
		}
		else
		{
			dir_file_node = dir_file_node->next;//同目录下的下一个文件
		}
	}

	if(flag==0)
	{
		return -1;
	}
	else
	{
		for(i=3; i<FILE_DESC_TABLE_SIZE; ++i)
		{
			if(File_Desc_Table[i]==NULL)
			{
				File_Desc_Table[i] = dir_file_node;
				break;
			}
		}

		if(i==FILE_DESC_TABLE_SIZE)
		{
			return -1;
		}
		else
		{
			return i;
		}
	}
}

/*********************************************************************
 * @fn      file_close
 *
 * @brief   关闭一个文件
 *
 * @param   fd:文件描述符
 *
 * @return  成功返回0,失败返回-1
 */
int file_close(int fd)
{
	int res=0;

	if(fd<0 || fd>1023)
	{
		printf("close fail:error fd\n");
		return -1;
	}

	printf("%s close\n", File_Desc_Table[fd]->name);
	//关闭链接
	File_Desc_Table[fd] = NULL;

	return res;
}

/*********************************************************************
 * @fn      file_read
 *
 * @brief   读取文件内容,存放在缓冲区buffer中
 *
 * @param   fd:文件描述符 buffer:读取数据缓冲区 size:读取大小
 *
 * @return  成功返回读取字节数,失败返回-1
 */
int file_read(int fd, char *buffer, int size)
{
	int res,real_size;
	block_t *block_addr;

	if(File_Desc_Table[fd]==NULL)
	{
		printf("file read fail:error fd\n");
		return -1;
	}
	block_addr = File_Desc_Table[fd]->block;

	//求出读取的实际大小
	real_size = File_Desc_Table[fd]->size>size?size:File_Desc_Table[fd]->size;
	res       = real_size;

	//磁盘块一块一块读取
	while(real_size>=BLOCK_SIZE&&block_addr!=NULL)
	{
		memcpy(buffer, block_addr->data, BLOCK_SIZE);
		buffer    = buffer+BLOCK_SIZE;
		real_size = real_size - BLOCK_SIZE;
		block_addr = block_addr->next;
	}

	if(real_size!=0&&block_addr!=NULL)
	{
		memcpy(buffer, block_addr->data, real_size);
	}

	return res;
}

/*********************************************************************
 * @fn      file_write
 *
 * @brief   将buffer中的size个字节写入文件中
 *
 * @param   fd:文件描述符 buffer:数据缓冲区 size:写入大小
 *
 * @return  成功返回读取字节数,失败返回-1
 */
int file_write(int fd, char *buffer, int size)
{
	int real_size,res=0;

	if(File_Desc_Table[fd]==NULL)
	{
		printf("file read fail:error fd\n");
		return -1;
	}

	//当前文件的第一个物理块
	block_t *block_addr = File_Desc_Table[fd]->block;
	block_t *ma_block;

	while(block_addr->next!=NULL)
	{
		block_addr = block_addr->next;
	}

	//将剩余的空间填充
	real_size = (size>=BLOCK_SIZE-File_Desc_Table[fd]->remain_size)?BLOCK_SIZE-File_Desc_Table[fd]->remain_size:size;
	memcpy(block_addr->data+(BLOCK_SIZE-File_Desc_Table[fd]->remain_size), buffer, real_size);
	size      = size-real_size;
	buffer    = buffer+File_Desc_Table[fd]->remain_size;//改变缓冲区指针位置
	res       = res+real_size;
    File_Desc_Table[fd]->remain_size = File_Desc_Table[fd]->remain_size-real_size;

	if(size>0)
	{
		while(size>0)
		{
			//申请一个物理块 使用尾插法插入到数据链表上
			ma_block = malloc_block();
			ma_block->next   = NULL;
			block_addr->next = ma_block;
			block_addr       = ma_block;

			//拷贝数据
			real_size = (size>=BLOCK_SIZE)?BLOCK_SIZE:size;
			memcpy(block_addr->data, buffer, real_size);
			size   = size - real_size;
			buffer = buffer + real_size;
			res    = res + real_size;
		}
		//计算剩下值
		File_Desc_Table[fd]->remain_size = BLOCK_SIZE-real_size;
	}

	File_Desc_Table[fd]->size += res;

	return res;
}
