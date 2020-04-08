
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMBLOCK_MIN_SIZE 0x1000
#define ADDR_ALIGN(value, align) (((value + align - 1) / align) * align)

struct memblock {
  size_t prev_size;
  size_t next_size;
  size_t size_;
  char data;

  bool IsFree() { return size_ & 0x1; }
  void SetFree() { size_ |= 1; }
  size_t GetSize() { return size_ >> 1; }
  void SetSize(size_t s) { size_ = s << 1; }  // clear the free flag
};

static size_t g_alloc_left_size = 0;
static memblock* g_last_block = nullptr;

void* malloc(size_t size) {
  if (!size) {
    return nullptr;
  }
  size_t block_size = size + offsetof(memblock, data);

  if (!g_last_block || g_alloc_left_size <= block_size) {
    // Not enough space, allocate a new block
    memblock* root_block = nullptr;

    void* previous_block = root_block;
    size_t new_block_size = MEMBLOCK_MIN_SIZE;
    size_t size_needed = block_size + offsetof(memblock, data);

    if (new_block_size < size_needed) {
      new_block_size = ADDR_ALIGN(size_needed, MEMBLOCK_MIN_SIZE);
    }

    root_block = reinterpret_cast<struct memblock*>(
        VirtualAlloc(NULL, new_block_size, MEM_RESERVE | MEM_COMMIT,
                     PAGE_EXECUTE_READWRITE));
    if (!root_block) {
      return nullptr;
    }

    root_block->prev_size = 0;
    root_block->next_size = 0;
    root_block->SetSize(offsetof(memblock, data));
    root_block->SetFree();  // a root block is always tagged free
    g_alloc_left_size = new_block_size - offsetof(memblock, data);
    g_last_block = root_block;
  }

  struct memblock* new_block = reinterpret_cast<struct memblock*>(
      reinterpret_cast<char*>(g_last_block) + g_last_block->GetSize());
  g_alloc_left_size -= block_size;

  new_block->prev_size = g_last_block->GetSize();
  new_block->next_size = 0;
  new_block->SetSize(block_size);
  g_last_block->next_size = block_size;
  g_last_block = new_block;

  return &new_block->data;
}

void free(void* mem) {
  struct memblock* block = reinterpret_cast<struct memblock*>(
      reinterpret_cast<char*>(mem) - offsetof(memblock, data));

  block->SetFree();

  // Search the root block but exit if one chunk is not free
  while (block->prev_size) {
    block = reinterpret_cast<struct memblock*>(reinterpret_cast<char*>(block) -
                                               block->prev_size);

    if (!block->IsFree()) {
      return;
    }
  }
  struct memblock* root_block = block;

  // Look if all blocks are free and if g_last_block is inside
  bool is_all_free = true;
  bool is_last_block_inside = false;
  while (block->next_size) {
    block = reinterpret_cast<struct memblock*>(reinterpret_cast<char*>(block) +
                                               block->GetSize());
    if (block == g_last_block) {
      is_last_block_inside = true;
    }
    if (!block->IsFree()) {
      is_all_free = false;
    }
  }
  struct memblock* last_block = block;

  // If all blocks are free and it's not our current chunk, free everything
  if (is_all_free && !is_last_block_inside) {
    VirtualFree(root_block, 0, MEM_RELEASE);
    return;
  }

  if (!is_last_block_inside) {  // We are done here
    return;
  }

  // If the last block is inside, try to update it
  struct memblock* prev_block = nullptr;
  size_t sum_block_size = 0;
  while (block->prev_size && block->IsFree()) {
    prev_block = reinterpret_cast<struct memblock*>(
        reinterpret_cast<char*>(block) - block->prev_size);

    if (g_last_block == block) {
      g_last_block = prev_block;
      g_alloc_left_size += sum_block_size + block->GetSize();
      sum_block_size = 0;
    } else {
      sum_block_size += block->GetSize();
    }

    block = prev_block;
  }
}

int main() {
  char* str1 = (char*)malloc(0xF00);
  char* str2 = (char*)malloc(0x1200);
  char* str3 = (char*)malloc(0x200);

  memcpy(str1, "Pwet", 5);
  memcpy(str2, "Amazing", 8);
  memcpy(str3, "Test", 5);
  printf("str1(%p): %s\n", str1, str1);
  printf("str2(%p): %s\n", str2, str2);
  printf("str3(%p): %s\n", str3, str3);

  free(str1);
  free(str2);
  free(str3);

  return 0;
}
