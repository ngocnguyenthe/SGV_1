/*
 * flash.c
 *
 *  Created on: Jul 27, 2023
 *      Author: admin
 */
#include "flash.h"
#include "stm32l1xx.h"
//#include "stm32f1xx.h"
void Flash_Erase(uint32_t address){
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.NbPages =1;
	EraseInitStruct.PageAddress = address;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t pageerr;
	HAL_FLASHEx_Erase(&EraseInitStruct, &pageerr);
	HAL_FLASH_Lock();
}
void Flash_Write_Int(uint32_t address,int value){
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, value);
	HAL_FLASH_Lock();
}
void Flash_Write_Float(uint32_t address, float f){
	HAL_FLASH_Unlock();
	uint8_t data[4];
	*(float*)data = f;
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *(uint32_t*)data);
	HAL_FLASH_Lock();
}

void Flash_Write_Array(uint32_t address, uint8_t *arr, uint16_t len){
	HAL_FLASH_Unlock();
	uint32_t *pt = (uint32_t*)arr;
	for (uint16_t i= 0; i < (len+3)/4;i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address+4*i, *pt);
		pt++;
	}
	HAL_FLASH_Lock();
}
void Flash_Write_Struct(uint32_t address ,wifi_infor_t dta){
	Flash_Write_Array(address,(uint8_t*)&dta,sizeof(dta));
}

int Flash_Read_Int(uint32_t address){
	return *(__IO uint32_t *)(address);
}
void Flash_Read_Float(uint32_t address){
	uint32_t data = *(__IO uint32_t *)(address);
	return *(float*)(&data);

}
void Flash_Read_Array(uint32_t address,uint8_t *arr, uint16_t len){
	uint32_t *pt = (uint32_t*)arr;
	for (uint32_t i= 0; i < (len+4)/4;i++) {
		*pt = *(__IO uint32_t *)(address+4*i);
		pt++;
	}
}
void Flash_Read_Struct(uint32_t address, wifi_infor_t *dta){
	Flash_Read_Array(address,(uint8_t*)&dta,sizeof(wifi_infor_t));
}

