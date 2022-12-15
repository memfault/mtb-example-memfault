#include "cyhal.h"
#include "mtb_kvstore.h"

static cyhal_flash_t flash_obj = {0};
static cyhal_flash_block_info_t block_info = {0};
static mtb_kvstore_t obj = {0};

static uint32_t bd_read_size(void* context, uint32_t addr) {
  CY_UNUSED_PARAMETER(context);
  CY_UNUSED_PARAMETER(addr);
  return 1;
}

static uint32_t bd_program_size(void* context, uint32_t addr) {
  CY_UNUSED_PARAMETER(context);
  CY_UNUSED_PARAMETER(addr);
  return block_info.page_size;
}

static uint32_t bd_erase_size(void* context, uint32_t addr) {
  CY_UNUSED_PARAMETER(context);
  CY_UNUSED_PARAMETER(addr);
  return block_info.sector_size;
}

static cy_rslt_t bd_read(void* context, uint32_t addr, uint32_t length, uint8_t* buf) {
  CY_UNUSED_PARAMETER(context);
  memcpy(buf, (const uint8_t*)(addr), length);
  return CY_RSLT_SUCCESS;
}

static cy_rslt_t bd_program(void* context, uint32_t addr, uint32_t length, const uint8_t* buf) {
  uint32_t prog_size = bd_program_size(context, addr);
  CY_ASSERT(0 == (length % prog_size));

  volatile cy_rslt_t result = CY_RSLT_SUCCESS;
  for (uint32_t loc = addr; result == CY_RSLT_SUCCESS && loc < addr + length;
       loc += prog_size, buf += prog_size) {
    result = cyhal_flash_program((cyhal_flash_t*)context, loc, (const uint32_t*)buf);
  }
  return result;
}

static cy_rslt_t bd_erase(void* context, uint32_t addr, uint32_t length) {
  uint32_t erase_size = bd_erase_size(context, addr);
  CY_ASSERT(0 == (length % erase_size));

  cy_rslt_t result = CY_RSLT_SUCCESS;
  for (uint32_t loc = addr; result == CY_RSLT_SUCCESS && loc < addr + length; loc += erase_size) {
    result = cyhal_flash_erase((cyhal_flash_t*)context, loc);
  }
  return result;
}

static mtb_kvstore_bd_t block_device = {
  .read = bd_read,
  .program = bd_program,
  .erase = bd_erase,
  .read_size = bd_read_size,
  .program_size = bd_program_size,
  .erase_size = bd_erase_size,
  .context = &flash_obj,
};

void app_kvstore_init(void) {
  cy_rslt_t result = cyhal_flash_init(&flash_obj);
  CY_ASSERT(result == CY_RSLT_SUCCESS);

  cyhal_flash_info_t flash_info;
  cyhal_flash_get_info(&flash_obj, &flash_info);
  block_info = flash_info.blocks[flash_info.block_count - 1];

  uint32_t length = 16U * block_info.page_size;
  uint32_t start_addr = block_info.start_address + block_info.size - length;

  result = mtb_kvstore_init(&obj, start_addr, length, &block_device);
  CY_ASSERT(result == CY_RSLT_SUCCESS);
}

cy_rslt_t app_kvstore_write(const char* key, const uint8_t* data, uint32_t data_len) {
  return mtb_kvstore_write(&obj, key, data, data_len);
}

cy_rslt_t app_kvstore_read(const char* key, uint8_t* data, uint32_t* data_len) {
  return mtb_kvstore_read(&obj, key, data, data_len);
}

bool app_kvstore_key_exists(const char* key) {
  return mtb_kvstore_key_exists(&obj, key) == CY_RSLT_SUCCESS ? true : false;
}
