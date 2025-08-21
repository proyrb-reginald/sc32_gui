#include "rom_cfg.h"
#include "rom_if.h"
#include <sc32_conf.h>
#include INC_LOG
#include INC_RTOS

#ifdef USE_TEST

#    define DATA_SIZE 512
#    define SCT_CNT ((2 * 1024) / DATA_SIZE)

static uint8_t test_r_data[DATA_SIZE] = {0};
static uint8_t test_w_data[DATA_SIZE] = {0};

__attribute__((noreturn)) void rom_test(void * thread_args) {
    PRTF_OS_LOG(NEWS_LOG, "start rom test!\n");

    for (uint16_t i = 0; i < DATA_SIZE; i++) {
        test_w_data[i] = i;
    }

    while (1) {
        for (uint8_t i = 0; i < SCT_CNT; i++) {
            PRTF_OS_LOG(INFO_LOG, "[[sct:%u]]\n", i);

            /********** 擦除扇区 **********/

            IAP_Unlock();
            IAP_EEPROMEraseSector(i);
            IAP_Lock();

            /********** 从扇区读数据 **********/

            const uint32_t adr  = EEPROM_BASE + i * ROM_SCT_SIZE;
            uint32_t       size = IAP_ReadByteArray(adr, test_r_data, DATA_SIZE);
            if (size < DATA_SIZE) {
                PRTF_OS_LOG(ERRO_LOG, "read fail!\n");
            } else {
                for (uint32_t i = 0; i < DATA_SIZE; i++) {
                    if (test_r_data[i] != 0) {
                        PRTF_OS_LOG(ERRO_LOG, "erase fail!\n");
                        rt_thread_suspend(rt_thread_self());
                    }
                }
                PRTF_OS_LOG(NEWS_LOG, "erase done!\n");
            }

            /********** 向扇区写数据 **********/

            IAP_Unlock();
            IAP_WriteCmd(ENABLE);
            size = IAP_ProgramByteArray(adr, test_w_data, DATA_SIZE);
            if (size < DATA_SIZE) {
                PRTF_OS_LOG(ERRO_LOG, "write fail!\n");
            }
            IAP_Lock();

            /********** 从扇区读数据 **********/

            size = IAP_ReadByteArray(adr, test_r_data, DATA_SIZE);
            if (size < DATA_SIZE) {
                PRTF_OS_LOG(ERRO_LOG, "read fail!\n");
            } else {
                for (uint32_t i = 0; i < DATA_SIZE; i++) {
                    if (test_r_data[i] != test_w_data[i]) {
                        PRTF_OS_LOG(ERRO_LOG, "write fail!\n");
                        rt_thread_suspend(rt_thread_self());
                    }
                }
                PRTF_OS_LOG(NEWS_LOG, "write done!\n");
            }
        }
    }
}

#endif