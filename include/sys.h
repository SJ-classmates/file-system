#ifndef _SYS_H
#define _SYS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE           32//一个物理块的大小
#define DISK_SIZE            512*BLOCK_SIZE//磁盘大小
#define BLOCK_COUNT          ((DISK_SIZE)/(BLOCK_SIZE))//物理块数目
#define BIT_COUNT            32//位示图中的位数
#define BITMAP_SIZE          ((BLOCK_COUNT)/(BIT_COUNT))//位示图大小
#define FILE_DESC_TABLE_SIZE 1024//文件描述符表的大小
#define FILE_DIR_NODE_NAME   10//文件/目录名字长度

#define TYPE_FILE            0//文件
#define TYPE_DIR             1//目录

#define USERNAME             "SJ"
#define VERSION              "FILESYS-1.0"

//path struct 组成文件路径的链表
typedef struct file_node_path{
	char node_name[FILE_DIR_NODE_NAME];
	struct file_node_path *next;
}dir_path_t;

//物理块定义
typedef struct disk_bolck{
	char     data[BLOCK_SIZE+1];
	struct disk_bolck *next;//指向下一个物理块
}block_t;

typedef struct File_Dir_Node{
	char name[FILE_DIR_NODE_NAME];//名字
	int size;//文件大小
	int remain_size;//最后一个物理块剩下的大小
	block_t *block;//第一个物理块
	int type;//判定改节点是为目录/文件
	int limit_mode;//权限
	struct File_Dir_Node *next;//同目录下的下一个文件/目录(链表)
	struct File_Dir_Node *last;//上一级目录 根目录为NULL(指针)
	struct File_Dir_Node *dir_next;//type->file NULL  type->dir 目录不为空->目录下的第一个文件(链表)
}File_Dir_Node_t;

extern block_t DISK[DISK_SIZE];//磁盘
extern int Disk_BitMap[BITMAP_SIZE];//位示图定义
extern File_Dir_Node_t *root;//根节点
extern File_Dir_Node_t *Current_Dir;//当前目录
extern File_Dir_Node_t *File_Desc_Table[FILE_DESC_TABLE_SIZE];//文件描述符表指针数组

void file_desc_table_init(void);
void block_init(void);
block_t* malloc_block(void);
void free_block(block_t* block);
int create_dir(char *dir_name);
int remove_dir(char *dir_name);
int create_file(char *file_name, char *data, int data_size);
int rm_file(char *file_name);
void display_current_path(void);
void file_system_init(void);

#endif // _SYS_H
