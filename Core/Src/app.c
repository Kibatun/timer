#include "main.h"
#include "app.h"

extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

int8_t flag = 0;
int8_t start = 0;
int8_t count = 0;   
uint32_t PageError; 
uint32_t Address = 0x0803F800;
FLASH_EraseInitTypeDef EraseInitStruct;


uint16_t getDigit(uint8_t dig)
{
  switch(dig)
  {
    case 0x00: return 0x3F;
    case 0x01: return 0x06;
    case 0x02: return 0x5B;
    case 0x03: return 0x4F;
    case 0x04: return 0x66;
    case 0x05: return 0x6D;
    case 0x06: return 0x7D;
    case 0x07: return 0x07;
    case 0x08: return 0x7F;
    case 0x09: return 0x6F;
    case 0x0A: return 0x77;
    case 0x0B: return 0x7C;
    case 0x0C: return 0x39;
    case 0x0D: return 0x5E;
    case 0x0E: return 0x79;
    case 0x0F: return 0x71;
    default:  return 0x40;    
  }
}

  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim6)
  {
  static uint16_t stage=0;
  HAL_GPIO_WritePin(GPIOD, 0xFF00, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);
  uint16_t digit = 0;
  uint32_t timer = count ;
  switch(stage)
  {
    case 0: digit = getDigit(timer/100); break;
    case 1: digit = getDigit((timer/10)%10)| 0x80; break;
    case 2: digit = getDigit(timer%10); break;    
  }
  HAL_GPIO_WritePin(GPIOD, digit << 8, GPIO_PIN_SET);  
  HAL_GPIO_WritePin(GPIOB, 1 << (13 + stage), GPIO_PIN_RESET);
  stage ++;
  stage = stage%3;
  }
    if(htim==&htim7)
{
  if(start == 1)
  count++;
    if(count%10 == 0)
    {
      if(count == *(uint32_t*)Address)
        return;
    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, count);
    HAL_FLASH_Lock();
    }

}
}



void AppInit()
{
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = Address;
    EraseInitStruct.NbPages = 1;
    count =*(uint32_t*)Address;
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start_IT(&htim7);
                
                  
}

void AppWork()
{
       if(( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7)== GPIO_PIN_SET) && (flag == 0))
    {
      flag=1;
        if(start == 0)
        {
          start = 1;
        }
          else if(start ==1)
          {
          start =0;
          }
    }
         else if ( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7)== GPIO_PIN_RESET)
         {
             flag = 0;
         }

    if( HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0)== GPIO_PIN_SET)
    {
      if(count==0) return;
      count = 0;
    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, count);
    HAL_FLASH_Lock();
    }

}
