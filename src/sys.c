#include "sys.h"

block_t DISK[DISK_SIZE];//磁盘
int Disk_BitMap[BITMAP_SIZE]={0};//位示图定义
File_Dir_Node_t *root;//根节点
File_Dir_Node_t *Current_Dir;//当前目录
File_Dir_Node_t *File_Desc_Table[FILE_DESC_TABLE_SIZE]={NULL};//文件描述符表指针数组

/*********************************************************************
 * @fn      file_desc_table_init
 *
 * @brief   初始化文件描述符表
 *
 * @param   none
 *
 * @return  none
 */
void file_desc_table_init(void)
{
	int i;

	for(i=0; i<FILE_DESC_TABLE_SIZE; ++i)
	{
		File_Desc_Table[i]=NULL;
	}
}

/*********************************************************************
 * @fn      block_init
 *
 * @brief   初始化磁盘
 *
 * @param   none
 *
 * @return  none
 */
void block_init(void)
{
	int i;

	for(i=0; i<DISK_SIZE; ++i)
	{
		DISK[i].data[BLOCK_SIZE] = '\0';
	}
}

/*********************************************************************
 * @fn      malloc_block
 *
 * @brief   申请一块物理块
 *
 * @param   none
 *
 * @return  成功返回改物理块的地址,失败返回NULL
 */
block_t* malloc_block(void)
{
	int i,j,flag=0;

	//遍历整个位示图
	for(i=0; i<BITMAP_SIZE; ++i)
	{
		for(j=0; j<BIT_COUNT; ++j)
		{
			//找到空闲物理块 退出循环
			if((Disk_BitMap[i] & (0x1<<j))==0)
			{
				flag=1;
				Disk_BitMap[i] |= (0x1<<j);//将对应位置1
				break;
			}
		}

		if(flag==1)
		{
			break;
		}
	}

	if(flag==0)
	{
		return NULL;
	}
	else
	{
		return &DISK[i*BIT_COUNT+j];
	}
}

/*********************************************************************
 * @fn      free_block
 *
 * @brief   释放一个物理块
 *
 * @param   none
 *
 * @return  none
 */
void free_block(block_t* block)
{
	int i,j,addr;

	addr = block - DISK;//数组相对位置
	i    = addr/32;
	j    = addr%32;
	Disk_BitMap[i] &= ~(0x01<<j);//位示图对应位置置0
	memset(block->data, 0, BLOCK_SIZE);
	block->next = NULL;
}

/*********************************************************************
 * @fn      create_dir
 *
 * @brief   创建一个目录
 *
 * @param   dir_name:目录名字
 *
 * @return  成功返回0,失败返回-1
 */
int create_dir(char *dir_name)
{
    if(dir_name==NULL)
    {
        return -1;
    }

	File_Dir_Node_t *dir_node;

	//分配内存空间
	dir_node              = (File_Dir_Node_t *)malloc(sizeof(File_Dir_Node_t));
	dir_node->type        = TYPE_DIR;//设置类型为目录
	dir_node->block       = NULL;//无磁盘内存
	dir_node->remain_size = 0;
	dir_node->size        = 0;
	dir_node->last        = Current_Dir;
	dir_node->dir_next    = NULL;
	//头插法
	dir_node->next        = Current_Dir->dir_next;
	Current_Dir->dir_next = dir_node;
	strcpy(dir_node->name, dir_name);

	return 0;
}

/*********************************************************************
 * @fn      remove_dir
 *
 * @brief   删除一个空目录
 *
 * @param   dir_name:目录名字
 *
 * @return  成功返回0,失败返回-1
 */
int remove_dir(char *dir_name)
{
	int res=-1,flag=0;

	if(dir_name==NULL)
    {
        return res;
    }

	File_Dir_Node_t *last_node = Current_Dir;
	File_Dir_Node_t *node      = Current_Dir->dir_next;
	while(node!=NULL)
	{
		//找出对应的目录
		if(strcmp(node->name, dir_name)==0)
		{
			flag=1;
			break;
		}
		else
		{
			last_node = node;
			node      = node->next;
		}
	}

	if(flag==0||node->type!=TYPE_DIR)
	{
		printf("No such dir:%s\n", dir_name);
		return res;
	}

	if(node->dir_next!=NULL)
	{
		printf("Dir %s not empty!\n", dir_name);
		return res;
	}

	//删除目录节点
	if(last_node==Current_Dir)
	{
		last_node->dir_next = node->next;
	}
	else
	{
		last_node->next = node->next;
	}
	free(node);
	res = 0;

	return res;
}

/*********************************************************************
 * @fn      create_file
 *
 * @brief   在当前目录下创建一个文件
 *
 * @param   file_name:文件名字 data:文件初始数据 data_size:初始数据大小
 *
 * @return  成功返回0,失败返回-1
 */
int create_file(char *file_name, char *data, int data_size)
{
	int write_size;
	File_Dir_Node_t *file_node;
	block_t *block_addr,*block_node;

	if(file_name==NULL)
    {
        return -1;
    }

	//分配内存空间
	file_node             = (File_Dir_Node_t *)malloc(sizeof(File_Dir_Node_t));
	file_node->type       = TYPE_FILE;//设置类型为目录
	file_node->last       = Current_Dir;
	file_node->dir_next   = NULL;//文件类型 改链表为空
	//头插法
	file_node->next       = Current_Dir->dir_next;
	Current_Dir->dir_next = file_node;
	strcpy(file_node->name, file_name);

	//申请一个物理块 第一次添加物理块 将数据写入文件
	block_node             = malloc_block();
	block_node->next       = NULL;
	file_node->block       = block_node;
	file_node->size        = data_size;//文件总大小
	block_addr             = block_node;
	write_size             = (data_size>BLOCK_SIZE)?BLOCK_SIZE:data_size;
	data_size              = data_size-write_size;
	file_node->remain_size = BLOCK_SIZE-write_size;
	memcpy(block_addr->data, data, write_size);
	data                   = data+write_size;//移动缓冲区指针

	while(data_size>0)
	{
		block_node       = malloc_block();
		block_node->next = NULL;
		block_addr->next = block_node;
		block_addr       = block_node;
		write_size       = (data_size>BLOCK_SIZE)?BLOCK_SIZE:data_size;
		memcpy(block_addr->data, data, write_size);
		data_size        = data_size-write_size;
		data             = data+write_size;//移动缓冲区指针
	}
	file_node->remain_size = BLOCK_SIZE-write_size;

	return 1;
}

/*********************************************************************
 * @fn      rm_file
 *
 * @brief   删除当前目录下的一个文件
 *
 * @param   file_name:文件名字
 *
 * @return  成功返回0,失败返回-1
 */
int rm_file(char *file_name)
{
	int res=-1,flag=0;
	block_t *block_node,*block_node_next;
	File_Dir_Node_t *last_node = Current_Dir;
	File_Dir_Node_t *node      = Current_Dir->dir_next;

	while(node!=NULL)
	{
		if(strcmp(node->name, file_name)==0)
		{
			flag=1;
			break;
		}
		else
		{
			last_node = node;
			node      = node->next;
		}
	}

	if(flag==0||node->type!=TYPE_FILE)
	{
		printf("No such file:%s\n", file_name);
		return res;
	}

	block_node_next = NULL;
	block_node      = node->block;

	while(block_node!=NULL)
	{
		block_node_next = block_node->next;
		free_block(block_node);
		block_node      = block_node_next;
	}

	if(last_node==Current_Dir)
	{
		last_node->dir_next = node->next;
	}
	else
	{
		last_node->next = node->next;
	}
	free(node);
	res = 0;

	return res;
}

/*********************************************************************
 * @fn      display_current_path
 *
 * @brief   输出当前所在的路径
 *
 * @param   none
 *
 * @return  none
 */
void display_current_path(void)
{
	dir_path_t *head_node         = (dir_path_t *)malloc(sizeof(dir_path_t));
	dir_path_t *node;//存放头节点
	dir_path_t *ma_node;
	File_Dir_Node_t *current_node = Current_Dir->last;

	strcpy(head_node->node_name, Current_Dir->name);
	head_node->next = NULL;

	while(current_node!=NULL)
	{
		ma_node       = (dir_path_t *)malloc(sizeof(dir_path_t));
		strcpy(ma_node->node_name, current_node->name);
		//逆置链表
		ma_node->next = head_node;
		head_node     = ma_node;
		current_node  = current_node->last;
	}

	printf("%s@%s:", USERNAME, VERSION);
	printf("%s", head_node->node_name);
	node = head_node->next;

	if(node==NULL)
	{
		printf("$ ");
	}

	while(node!=NULL)
	{
		printf("%s", node->node_name);
		if(node->next!=NULL)
		{
			printf("/");
		}
		else
		{
			printf("$ ");
		}
		node = node->next;
	}

	node = head_node;
	while(node!=NULL)
	{
		//保存下一个节点
		head_node = head_node->next;
		free(node);
		node      = head_node;
	}
}

/*********************************************************************
 * @fn      file_system_init
 *
 * @brief   初始化文件系统
 *
 * @param   none
 *
 * @return  none
 */
void file_system_init(void)
{
	root              = (File_Dir_Node_t *)malloc(sizeof(File_Dir_Node_t));
	root->type        = TYPE_DIR;//根目录
	root->block       = NULL;
	root->remain_size = 0;
	root->size        = 0;
	root->last        = NULL;
	root->dir_next    = NULL;
	strcpy(root->name, "/");

	Current_Dir = root;
}
