#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS 64
#define INODE_SIZE 256
#define INODE_TABLE_BLOCKS 5
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define TOTAL_INODES (INODES_PER_BLOCK * INODE_TABLE_BLOCKS)
#define SUPERBLOCK_MAGIC 0xD34D

#pragma pack(push, 1)
typedef struct
{
    uint16_t magic;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_start;
    uint32_t first_data_block;
    uint32_t inode_size;
    uint32_t inode_count;
    char reserved[4058];
} Superblock;

typedef struct
{
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint32_t links;
    uint32_t blocks;
    uint32_t direct;
    uint32_t indirect;
    uint32_t double_indirect;
    uint32_t triple_indirect;
    char reserved[156];
} Inode;
#pragma pack(pop)

uint8_t inode_bitmap[BLOCK_SIZE];
uint8_t data_bitmap[BLOCK_SIZE];
uint8_t data_block_usage[TOTAL_BLOCKS] = {0};
uint8_t valid_inodes[TOTAL_INODES] = {0};

FILE *image;
Superblock superblock;

void read_superblock()
{
    fseek(image, 0, SEEK_SET);
    fread(&superblock, sizeof(Superblock), 1, image);

    printf("Checking Superblock...\n");
    if (superblock.magic != SUPERBLOCK_MAGIC)
        printf("ERROR: Invalid magic number: 0x%X\n", superblock.magic);
    if (superblock.block_size != BLOCK_SIZE)
        printf("ERROR: Block size is not 4096\n");
    if (superblock.total_blocks != TOTAL_BLOCKS)
        printf("ERROR: Total blocks not 64\n");
    if (superblock.inode_size != INODE_SIZE)
        printf("ERROR: Inode size not 256 bytes\n");
    if (superblock.inode_count != TOTAL_INODES)
        printf("ERROR: Inode count not 160\n");

    printf("Superblock check complete.\n\n");
}

void read_bitmaps()
{
    fseek(image, superblock.inode_bitmap_block * BLOCK_SIZE, SEEK_SET);
    fread(inode_bitmap, BLOCK_SIZE, 1, image);

    fseek(image, superblock.data_bitmap_block * BLOCK_SIZE, SEEK_SET);
    fread(data_bitmap, BLOCK_SIZE, 1, image);
}

int is_inode_used(int i)
{
    return (inode_bitmap[i / 8] >> (i % 8)) & 1;
}

int is_data_used(int i)
{
    return (data_bitmap[i / 8] >> (i % 8)) & 1;
}

void check_inodes()
{
    printf("Checking Inodes...\n");
    for (int i = 0; i < TOTAL_INODES; i++)
    {
        if (!is_inode_used(i))
            continue;

        Inode inode;
        int block_num = superblock.inode_table_start + (i / INODES_PER_BLOCK);
        int offset = (i % INODES_PER_BLOCK) * INODE_SIZE;
        fseek(image, block_num * BLOCK_SIZE + offset, SEEK_SET);
        fread(&inode, sizeof(Inode), 1, image);

        if (inode.links > 0 && inode.dtime == 0)
        {
            valid_inodes[i] = 1; // valid
        }
        else
        {
            printf("ERROR: Inode %d is marked used but invalid (links=%d, dtime=%d)\n", i, inode.links, inode.dtime);
        }

        if (inode.direct >= superblock.first_data_block && inode.direct < TOTAL_BLOCKS)
        {
            if (!is_data_used(inode.direct))
            {
                printf("ERROR: Inode %d references block %d but it's not marked in data bitmap\n", i, inode.direct);
            }
            else if (data_block_usage[inode.direct])
            {
                printf("ERROR: Duplicate reference to block %d by inode %d\n", inode.direct, i);
            }
            data_block_usage[inode.direct] = 1;
        }
        else if (inode.direct != 0)
        {
            printf("ERROR: Inode %d references invalid block %d\n", i, inode.direct);
        }
    }
    printf("Inode check complete.\n\n");
}

void check_data_bitmap_consistency()
{
    printf("Checking Data Bitmap Consistency...\n");
    for (int i = superblock.first_data_block; i < TOTAL_BLOCKS; i++)
    {
        if (is_data_used(i) && !data_block_usage[i])
        {
            printf("ERROR: Data block %d marked used in bitmap but not referenced by any inode\n", i);
        }
    }
    printf("Data Bitmap Check complete.\n\n");
}

void check_inode_bitmap_consistency()
{
    printf("Checking Inode Bitmap Consistency...\n");
    for (int i = 0; i < TOTAL_INODES; i++)
    {
        if (valid_inodes[i] && !is_inode_used(i))
        {
            printf("ERROR: Inode %d is valid but not marked in inode bitmap\n", i);
        }
        if (!valid_inodes[i] && is_inode_used(i))
        {
            printf("ERROR: Inode %d is marked in inode bitmap but has no valid data (invalid inode)\n", i);
        }
    }
    printf("Inode Bitmap Check complete.\n\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <vsfs.img>\n", argv[0]);
        return 1;
    }

    image = fopen(argv[1], "rb");
    if (!image)
    {
        perror("Failed to open image file");
        return 1;
    }

    read_superblock();
    read_bitmaps();
    check_inodes();
    check_inode_bitmap_consistency();
    check_data_bitmap_consistency();

    fclose(image);
    return 0;
}
