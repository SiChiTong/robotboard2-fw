/*
	Sorry for this copy-pasta module. Startup code tends to end up like this, which is why it's often
	autogenerated. We get the memory addresses from the linker script.

*/

#include <stdint.h>

#include "ext_include/stm32h7xx.h"
#include "stm32_cmsis_extension.h"
#include "backup_ram.h"
#include "misc.h"

void stm32init();
void nmi_handler();
void invalid_handler();
void hardfault_handler();
void memmanage_handler();

extern void error(int code);
extern void main();

//extern void sbc_spi_eot_inthandler();
extern void sbc_spi_cs_end_inthandler();
extern void shutdown_handler();
extern void epc_rx_dma_inthandler();
extern void epc_i2c_inthandler();
extern void epc_dcmi_dma_inthandler();
extern void imu_fsm_inthandler();
extern void timebase_inthandler();
extern void bldc0_inthandler();
extern void bldc1_inthandler();

extern void agm01_errhandler();
extern void agm23_errhandler();
extern void agm45_errhandler0();
extern void agm45_errhandler1();
extern void agm45_errhandler2();

extern void adc12_inthandler();
extern void adc3_inthandler();

extern void adc1_dma_errhandler();
extern void adc2_dma_errhandler();
extern void adc3_dma_errhandler();

extern void charger_safety_errhandler();

extern void charger_adc2_pha_inthandler();

extern void drive_handler();
extern void compass_handler();

extern unsigned int _STACKTOP;

#define VECTOR_TBL_LEN 166

// Vector table on page 730 on the Reference Manual RM0433
unsigned int * the_nvic_vector[VECTOR_TBL_LEN] __attribute__ ((section(".nvic_vector"))) =
{
/* 0x0000                    */ (unsigned int *) &_STACKTOP,
/* 0x0004 RESET              */ (unsigned int *) stm32init,
/* 0x0008 NMI                */ (unsigned int *) nmi_handler,
/* 0x000C HARDFAULT          */ (unsigned int *) hardfault_handler,
/* 0x0010 MemManage          */ (unsigned int *) memmanage_handler,
/* 0x0014 BusFault           */ (unsigned int *) invalid_handler,
/* 0x0018 UsageFault         */ (unsigned int *) invalid_handler,
/* 0x001C                    */ (unsigned int *) invalid_handler,
/* 0x0020                    */ (unsigned int *) invalid_handler,
/* 0x0024                    */ (unsigned int *) invalid_handler,
/* 0x0028                    */ (unsigned int *) invalid_handler,
/* 0x002C SVcall             */ (unsigned int *) invalid_handler,
/* 0x0030 DebugMonitor       */ (unsigned int *) invalid_handler,
/* 0x0034                    */ (unsigned int *) invalid_handler,
/* 0x0038 PendSV             */ (unsigned int *) invalid_handler,
/* 0x003C SysTick            */ (unsigned int *) invalid_handler,
/* 0x0040 WWDG1              */ (unsigned int *) invalid_handler,
/* 0x0044 PVD (volt detector)*/ (unsigned int *) shutdown_handler,
/* 0x0048                    */ (unsigned int *) invalid_handler,
/* 0x004C                    */ (unsigned int *) invalid_handler,
/* 0x0050                    */ (unsigned int *) invalid_handler,
/* 0x0054                    */ (unsigned int *) invalid_handler,
/* 0x0058 EXTI0              */ (unsigned int *) invalid_handler,
/* 0x005C EXTI1              */ (unsigned int *) invalid_handler,
/* 0x0060 EXTI2              */ (unsigned int *) invalid_handler,
/* 0x0064 EXTI3              */ (unsigned int *) invalid_handler,
/* 0x0068 EXTI4              */ (unsigned int *) invalid_handler,
/* 0x006C DMA1 STREAM0       */ (unsigned int *) invalid_handler,
/* 0x0070            1       */ (unsigned int *) invalid_handler,
/* 0x0074            2       */ (unsigned int *) invalid_handler,
/* 0x0078            3       */ (unsigned int *) epc_rx_dma_inthandler,
/* 0x007C            4       */ (unsigned int *) agm01_errhandler,
/* 0x0080            5       */ (unsigned int *) agm45_errhandler0,
/* 0x0084            6       */ (unsigned int *) agm01_errhandler,
/* 0x0088 ADC1&2             */ (unsigned int *) charger_adc2_pha_inthandler, // set dynamically, but this default is needed
/* 0x008C                    */ (unsigned int *) invalid_handler,
/* 0x0090                    */ (unsigned int *) invalid_handler,
/* 0x0094                    */ (unsigned int *) invalid_handler,
/* 0x0098                    */ (unsigned int *) invalid_handler,
/* 0x009C EXTI9_5            */ (unsigned int *) invalid_handler,
/* 0x00A0 TIM1 break         */ (unsigned int *) invalid_handler,
/* 0x00A4 TIM1 update        */ (unsigned int *) invalid_handler,
/* 0x00A8 TIM1 trig/commu    */ (unsigned int *) invalid_handler,
/* 0x00AC TIM1 capt/comp     */ (unsigned int *) bldc0_inthandler,
/* 0x00B0 TIM2               */ (unsigned int *) invalid_handler,
/* 0x00B4 TIM3               */ (unsigned int *) invalid_handler, // set dynamically
/* 0x00B8 TIM4               */ (unsigned int *) invalid_handler,
/* 0x00BC I2C1 EVENT         */ (unsigned int *) epc_i2c_inthandler,
/* 0x00C0      ERR           */ (unsigned int *) invalid_handler,
/* 0x00C4 I2C2 EVENT         */ (unsigned int *) invalid_handler,
/* 0x00C8      ERR           */ (unsigned int *) invalid_handler,
/* 0x00CC SPI1               */ (unsigned int *) invalid_handler,
/* 0x00D0 SPI2               */ (unsigned int *) agm45_errhandler1,
/* 0x00D4 USART1             */ (unsigned int *) invalid_handler,
/* 0x00D8 USART2             */ (unsigned int *) invalid_handler,
/* 0x00DC USART3             */ (unsigned int *) invalid_handler,
/* 0x00E0 EXTI15_10          */ (unsigned int *) sbc_spi_cs_end_inthandler,
/* 0x00E4                    */ (unsigned int *) invalid_handler,
/* 0x00E8                    */ (unsigned int *) invalid_handler,
/* 0x00EC TIM8 break         */ (unsigned int *) invalid_handler,
/* 0x00F0 TIM8 update        */ (unsigned int *) invalid_handler,
/* 0x00F4 TIM8 trig/commu    */ (unsigned int *) invalid_handler,
/* 0x00F8 TIM8 capt/comp     */ (unsigned int *) bldc1_inthandler,
/* 0x00FC DMA1 STREAM7       */ (unsigned int *) agm45_errhandler2,
/* 0x0100                    */ (unsigned int *) invalid_handler,
/* 0x0104                    */ (unsigned int *) invalid_handler,
/* 0x0108 TIM5               */ (unsigned int *) timebase_inthandler,
/* 0x010C SPI3               */ (unsigned int *) invalid_handler,
/* 0x0110 UART4              */ (unsigned int *) invalid_handler,
/* 0x0114 UART5              */ (unsigned int *) invalid_handler,
/* 0x0118 TIM6&DAC UNDERRUN  */ (unsigned int *) invalid_handler,
/* 0x011C TIM7               */ (unsigned int *) invalid_handler,
/* 0x0120 DMA2 STREAM0       */ (unsigned int *) epc_dcmi_dma_inthandler,
/* 0x0124            1       */ (unsigned int *) adc1_dma_errhandler,
/* 0x0128            2       */ (unsigned int *) adc2_dma_errhandler,
/* 0x012C            3       */ (unsigned int *) adc3_dma_errhandler,
/* 0x0130            4       */ (unsigned int *) invalid_handler,
/* 0x0134                    */ (unsigned int *) invalid_handler,
/* 0x0138                    */ (unsigned int *) invalid_handler,
/* 0x013C                    */ (unsigned int *) invalid_handler,
/* 0x0140                    */ (unsigned int *) invalid_handler,
/* 0x0144                    */ (unsigned int *) invalid_handler,
/* 0x0148                    */ (unsigned int *) invalid_handler,
/* 0x014C                    */ (unsigned int *) invalid_handler,
/* 0x0150 DMA2 STREAM5       */ (unsigned int *) invalid_handler,
/* 0x0154            6       */ (unsigned int *) invalid_handler,
/* 0x0158            7       */ (unsigned int *) invalid_handler,
/* 0x015C USART6             */ (unsigned int *) invalid_handler,
/* 0x0160                    */ (unsigned int *) invalid_handler,
/* 0x0164                    */ (unsigned int *) invalid_handler,
/* 0x0168                    */ (unsigned int *) invalid_handler,
/* 0x016C                    */ (unsigned int *) invalid_handler,
/* 0x0170                    */ (unsigned int *) invalid_handler,
/* 0x0174                    */ (unsigned int *) invalid_handler,
/* 0x0178                    */ (unsigned int *) invalid_handler,
/* 0x017C                    */ (unsigned int *) invalid_handler,
/* 0x0180                    */ (unsigned int *) invalid_handler,
/* 0x0184                    */ (unsigned int *) invalid_handler,
/* 0x0188 UART7              */ (unsigned int *) invalid_handler,
/* 0x018C UART8              */ (unsigned int *) invalid_handler,
/* 0x0190 SPI4               */ (unsigned int *) agm01_errhandler,
/* 0x0194 SPI5               */ (unsigned int *) invalid_handler,
/* 0x0198 SPI6               */ (unsigned int *) agm23_errhandler,
/* 0x019C                    */ (unsigned int *) invalid_handler,
/* 0x01A0                    */ (unsigned int *) invalid_handler,
/* 0x01A4                    */ (unsigned int *) invalid_handler,
/* 0x01A8                    */ (unsigned int *) invalid_handler,
/* 0x01AC                    */ (unsigned int *) invalid_handler,
/* 0x01B0                    */ (unsigned int *) invalid_handler,
/* 0x01B4                    */ (unsigned int *) invalid_handler,
/* 0x01B8                    */ (unsigned int *) invalid_handler,
/* 0x01BC                    */ (unsigned int *) invalid_handler,
/* 0x01C0                    */ (unsigned int *) invalid_handler,
/* 0x01C4                    */ (unsigned int *) invalid_handler,
/* 0x01C8                    */ (unsigned int *) invalid_handler,
/* 0x01CC                    */ (unsigned int *) invalid_handler,
/* 0x01D0                    */ (unsigned int *) invalid_handler,
/* 0x01D4                    */ (unsigned int *) invalid_handler,
/* 0x01D8                    */ (unsigned int *) invalid_handler,
/* 0x01DC HRTIM MASTER       */ (unsigned int *) invalid_handler,
/* 0x01E0 HRTIM A            */ (unsigned int *) invalid_handler,
/* 0x01E4       B            */ (unsigned int *) invalid_handler,
/* 0x01E8       C            */ (unsigned int *) invalid_handler,
/* 0x01EC       D            */ (unsigned int *) charger_safety_errhandler,
/* 0x01F0       E            */ (unsigned int *) invalid_handler,
/* 0x01F4 HRTIM FAULT        */ (unsigned int *) invalid_handler,
/* 0x01F8                    */ (unsigned int *) invalid_handler,
/* 0x01FC                    */ (unsigned int *) invalid_handler,
/* 0x0200                    */ (unsigned int *) invalid_handler,
/* 0x0204                    */ (unsigned int *) invalid_handler,
/* 0x0208                    */ (unsigned int *) invalid_handler,
/* 0x020C                    */ (unsigned int *) invalid_handler,
/* 0x0210 TIM15              */ (unsigned int *) invalid_handler,
/* 0x0214 TIM16              */ (unsigned int *) invalid_handler,
/* 0x0218 TIM17              */ (unsigned int *) invalid_handler,
/* 0x021C                    */ (unsigned int *) invalid_handler,
/* 0x0220                    */ (unsigned int *) invalid_handler,
/* 0x0224                    */ (unsigned int *) invalid_handler,
/* 0x0228                    */ (unsigned int *) invalid_handler,
/* 0x022C                    */ (unsigned int *) invalid_handler,
/* 0x0230                    */ (unsigned int *) invalid_handler,
/* 0x0234                    */ (unsigned int *) invalid_handler,
/* 0x0238                    */ (unsigned int *) invalid_handler,
/* 0x023C ADC3               */ (unsigned int *) adc3_inthandler,
/* 0x0240                    */ (unsigned int *) invalid_handler,
/* 0x0244 BDMA 0             */ (unsigned int *) agm23_errhandler,
/* 0x0248      1             */ (unsigned int *) agm23_errhandler,
/* 0x024C      2             */ (unsigned int *) invalid_handler,
/* 0x0250      3             */ (unsigned int *) invalid_handler,
/* 0x0254      4             */ (unsigned int *) invalid_handler,
/* 0x0258      5             */ (unsigned int *) invalid_handler,
/* 0x025C      6             */ (unsigned int *) invalid_handler,
/* 0x0260      7             */ (unsigned int *) invalid_handler,
/* 0x0264                    */ (unsigned int *) invalid_handler,
/* 0x0268                    */ (unsigned int *) invalid_handler,
/* 0x026C                    */ (unsigned int *) invalid_handler,
/* 0x0270                    */ (unsigned int *) invalid_handler,
/* 0x0274                    */ (unsigned int *) invalid_handler,
/* 0x0278                    */ (unsigned int *) invalid_handler,
/* 0x027C                    */ (unsigned int *) invalid_handler,
/* 0x0280                    */ (unsigned int *) invalid_handler,
/* 0x0284                    */ (unsigned int *) invalid_handler,
/* 0x0288                    */ (unsigned int *) invalid_handler,
/* 0x028C (empty #147)       */ (unsigned int *) drive_handler,
/* 0x0290 (empty #148)       */ (unsigned int *) compass_handler,
/* 0x0294                    */ (unsigned int *) invalid_handler

};

extern unsigned int _DATA_BEGIN;
extern unsigned int _DATA_END;
extern unsigned int _DATAI_BEGIN;

extern unsigned int _BSS_BEGIN;
extern unsigned int _BSS_END;


extern unsigned int _DTCM_DATA_BEGIN;
extern unsigned int _DTCM_DATA_END;
extern unsigned int _DTCM_DATA_I_BEGIN;

extern unsigned int _DTCM_BSS_BEGIN;
extern unsigned int _DTCM_BSS_END;


extern unsigned int _SRAM1_DATA_BEGIN;
extern unsigned int _SRAM1_DATA_END;
extern unsigned int _SRAM1_DATA_I_BEGIN;

extern unsigned int _SRAM1_BSS_BEGIN;
extern unsigned int _SRAM1_BSS_END;

/*
extern unsigned int _SRAM2_DATA_BEGIN;
extern unsigned int _SRAM2_DATA_END;
extern unsigned int _SRAM2_DATA_I_BEGIN;

extern unsigned int _SRAM2_BSS_BEGIN;
extern unsigned int _SRAM2_BSS_END;
*/

extern unsigned int _SRAM3_DATA_BEGIN;
extern unsigned int _SRAM3_DATA_END;
extern unsigned int _SRAM3_DATA_I_BEGIN;

extern unsigned int _SRAM3_BSS_BEGIN;
extern unsigned int _SRAM3_BSS_END;


extern unsigned int _SRAM4_DATA_BEGIN;
extern unsigned int _SRAM4_DATA_END;
extern unsigned int _SRAM4_DATA_I_BEGIN;

extern unsigned int _SRAM4_BSS_BEGIN;
extern unsigned int _SRAM4_BSS_END;


extern unsigned int _RAM_BACKUP_DATA_BEGIN;
extern unsigned int _RAM_BACKUP_DATA_END;
extern unsigned int _RAM_BACKUP_DATA_I_BEGIN;

extern unsigned int _RAM_BACKUP_BSS_BEGIN;
extern unsigned int _RAM_BACKUP_BSS_END;


extern unsigned int _SETTINGS_BEGIN;
extern unsigned int _SETTINGS_END;
extern unsigned int _SETTINGS_I_BEGIN;

extern unsigned int _TEXT_ITCM_BEGIN;
extern unsigned int _TEXT_ITCM_END;
extern unsigned int _TEXT_ITCM_I_BEGIN;


extern void hwtest_main();

/*
void refresh_settings()
{
	volatile uint32_t* settings_begin  = (volatile uint32_t*)&_SETTINGS_BEGIN;
	volatile uint32_t* settings_end    = (volatile uint32_t*)&_SETTINGS_END;
	volatile uint32_t* settings_i_begin = (volatile uint32_t*)&_SETTINGS_I_BEGIN;

	while(settings_begin < settings_end)
	{
		*settings_begin = *settings_i_begin;
		settings_begin++;
		settings_i_begin++;
	}
	__DSB(); __ISB();
}
*/

#define _RELOCATED_VECTORS_BEGIN 0x0000FC00UL
#define ROM_ORIGIN 0x08000000UL

void delay_us(uint32_t i) __attribute__((section(".text_itcm")));
void delay_tenth_us(uint32_t i) __attribute__((section(".text_itcm")));
void delay_ms(uint32_t i) __attribute__((section(".text_itcm")));

void init_sram1234()
{
	// Enable RAMs from the start so that we can copy data sections / initialize bss sections on these:
	RCC->AHB2ENR |= 1UL<<31 /*SRAM3*/ | 1UL<<30 /*SRAM2*/ | 1UL<<29 /*SRAM1*/;
	// All other RAMs are accessible by CPU by default

	uint32_t* sram1_data_begin  = (uint32_t*)&_SRAM1_DATA_BEGIN;
	uint32_t* sram1_data_end    = (uint32_t*)&_SRAM1_DATA_END;
	uint32_t* sram1_data_i_begin = (uint32_t*)&_SRAM1_DATA_I_BEGIN;

	while(sram1_data_begin < sram1_data_end)
	{
		*sram1_data_begin = *sram1_data_i_begin;
		sram1_data_begin++;
		sram1_data_i_begin++;
	}

	uint32_t* sram1_bss_begin = (uint32_t*)&_SRAM1_BSS_BEGIN;
	uint32_t* sram1_bss_end   = (uint32_t*)&_SRAM1_BSS_END;
	while(sram1_bss_begin < sram1_bss_end)
	{
		*sram1_bss_begin = 0;
		sram1_bss_begin++;
	}


/*
	uint32_t* sram2_data_begin  = (uint32_t*)&_SRAM2_DATA_BEGIN;
	uint32_t* sram2_data_end    = (uint32_t*)&_SRAM2_DATA_END;
	uint32_t* sram2_data_i_begin = (uint32_t*)&_SRAM2_DATA_I_BEGIN;

	while(sram2_data_begin < sram2_data_end)
	{
		*sram2_data_begin = *sram2_data_i_begin;
		sram2_data_begin++;
		sram2_data_i_begin++;
	}

	uint32_t* sram2_bss_begin = (uint32_t*)&_SRAM2_BSS_BEGIN;
	uint32_t* sram2_bss_end   = (uint32_t*)&_SRAM2_BSS_END;
	while(sram2_bss_begin < sram2_bss_end)
	{
		*sram2_bss_begin = 0;
		sram2_bss_begin++;
	}
*/



	uint32_t* sram3_data_begin  = (uint32_t*)&_SRAM3_DATA_BEGIN;
	uint32_t* sram3_data_end    = (uint32_t*)&_SRAM3_DATA_END;
	uint32_t* sram3_data_i_begin = (uint32_t*)&_SRAM3_DATA_I_BEGIN;

	while(sram3_data_begin < sram3_data_end)
	{
		*sram3_data_begin = *sram3_data_i_begin;
		sram3_data_begin++;
		sram3_data_i_begin++;
	}

	uint32_t* sram3_bss_begin = (uint32_t*)&_SRAM3_BSS_BEGIN;
	uint32_t* sram3_bss_end   = (uint32_t*)&_SRAM3_BSS_END;
	while(sram3_bss_begin < sram3_bss_end)
	{
		*sram3_bss_begin = 0;
		sram3_bss_begin++;
	}

	uint32_t* sram4_data_begin  = (uint32_t*)&_SRAM4_DATA_BEGIN;
	uint32_t* sram4_data_end    = (uint32_t*)&_SRAM4_DATA_END;
	uint32_t* sram4_data_i_begin = (uint32_t*)&_SRAM4_DATA_I_BEGIN;

	while(sram4_data_begin < sram4_data_end)
	{
		*sram4_data_begin = *sram4_data_i_begin;
		sram4_data_begin++;
		sram4_data_i_begin++;
	}

	uint32_t* sram4_bss_begin = (uint32_t*)&_SRAM4_BSS_BEGIN;
	uint32_t* sram4_bss_end   = (uint32_t*)&_SRAM4_BSS_END;
	while(sram4_bss_begin < sram4_bss_end)
	{
		*sram4_bss_begin = 0;
		sram4_bss_begin++;
	}


	__DSB(); __ISB();

}

void init_dtcm()
{
	uint32_t* dtcm_data_begin  = (uint32_t*)&_DTCM_DATA_BEGIN;
	uint32_t* dtcm_data_end    = (uint32_t*)&_DTCM_DATA_END;
	uint32_t* dtcm_data_i_begin = (uint32_t*)&_DTCM_DATA_I_BEGIN;

	while(dtcm_data_begin < dtcm_data_end)
	{
		*dtcm_data_begin = *dtcm_data_i_begin;
		dtcm_data_begin++;
		dtcm_data_i_begin++;
	}

	uint32_t* dtcm_bss_begin = (uint32_t*)&_DTCM_BSS_BEGIN;
	uint32_t* dtcm_bss_end   = (uint32_t*)&_DTCM_BSS_END;
	while(dtcm_bss_begin < dtcm_bss_end)
	{
		*dtcm_bss_begin = 0;
		dtcm_bss_begin++;
	}
}

void init_axi_data()
{
	uint32_t* bss_begin = (uint32_t*)&_BSS_BEGIN;
	uint32_t* bss_end   = (uint32_t*)&_BSS_END;
	while(bss_begin < bss_end)
	{
		*bss_begin = 0;
		bss_begin++;
	}


	uint32_t* data_begin  = (uint32_t*)&_DATA_BEGIN;
	uint32_t* data_end    = (uint32_t*)&_DATA_END;
	uint32_t* datai_begin = (uint32_t*)&_DATAI_BEGIN;

	while(data_begin < data_end)
	{
		*data_begin = *datai_begin;
		data_begin++;
		datai_begin++;
	}
}

void relocate_vectors()
{
	/*
		Relocate the vector table to RAM.
		For some reason, reading at address 0 causes gcc to not generate any code at all, without any warnings,
		so we start copying at 4.
	*/
	uint32_t* vect_begin  = (uint32_t*)(_RELOCATED_VECTORS_BEGIN+4);
	uint32_t* vect_end    = (uint32_t*)(_RELOCATED_VECTORS_BEGIN+VECTOR_TBL_LEN*4);
	uint32_t* vecti_begin = (uint32_t*)(ROM_ORIGIN+4);

	while(vect_begin < vect_end)
	{
		*vect_begin = *vecti_begin;
		vect_begin++;
		vecti_begin++;
	}

	SCB->VTOR = _RELOCATED_VECTORS_BEGIN;

	__DSB(); __ISB();

}

/*
void init_delay_us(uint32_t i)
{
	i *= 10;
	i -= 7;
	while(i--)
		__asm__ __volatile__ ("nop");
}
*/

// Get the bare minimum ready ASAP so that the charge pump can start.
void stm32init(void)
{
	RCC->AHB4ENR |= 0b111111111; // enable GPIOA to GPIOI (J and K do not exist on the device)
//	IO_TO_GPO(GPIOC, 13); // LED

	PWR->CR1 |= 1UL<<8; // Enable write access to the backup SRAM, and the PWR->CR2 register
	__DSB();


//	int backup_sram_lost = 0;
	if(!(PWR->CR2 & 1UL)) // Backup regulator was turned off, for example, because the Vbat was disconnected
	{
//		backup_sram_lost = 1;
		PWR->CR2 |= 1UL; // Turn the regulator on, so it will continue working the next time the robot is turned off.
		RCC->BDCR = 1UL /*LSE oscillator on, for RTC*/ | 0b01UL<<8 /*LSE clock drives RTC*/ |
		            1UL<<15 /*Enable RTC*/;
//		LED_ON();
//		init_delay_us(200000);
//		LED_OFF();
	}

	RCC->AHB4ENR |= 1UL<<28; // Enable backup ram access clock.

#ifdef REV2A

	IO_TO_GPI(GPIOE,2); // power switch sense
	#define PWRSWITCH_PRESSED (!IN(GPIOE,2))

#endif

#ifdef REV2B

	IO_TO_GPI(GPIOI,11); // power switch sense
	#define PWRSWITCH_PRESSED (!IN(GPIOI,11))
#endif


	if(backup_ram.immediate_5v == 0x420b1a5e && !PWRSWITCH_PRESSED)
	{
		// If the power switch is pressed, the backup ram is wrong
		RCC->AHB4ENR |= 1UL<<5;
		BIG5V_ON();
		IO_TO_GPO(GPIOF, 5); // 5Vbig - same pin in REV2A, REV2B
		backup_ram.immediate_5v = 0;
		backup_ram.immediate_5v; // dummy read, see flash.c
	}

	RCC->APB4ENR |= 1UL<<1 /*SYSCFG needs to be on for some configuration thingies often needed when fighting against
		 device errata*/;


	/*
	To get us started, we jump right into our typical STM32 territory:
	The errata sheet will tell us they failed the SRAM bus connectivity, and random data corruption will occur
	unless we set a workaround bit which will limit the bus performance:
		"Set the READ_ISS_OVERRIDE bit in the AXI_TARG7_FN_MOD register."

	Easier said than done. Guess what? They didn't bother defining these register names in their header files. 
	Furthermore, they won't tell you the addresses these registers reside in; only some imaginary "offsets", but
	the base is hard to find- they refer you to a table which won't include said information - so we'll need to guess.
	Finally, you can find out the information, from a different table than you were referred at. Guess what they call
	AXIM in this table? Not AXIM, but GPV. Of course!
	*/

	(*(uint32_t*)(0x51000000UL /*The magical, hard to find GPV aka AXIM base*/ + 0x8108UL /*AXI_TARG7_FN_MOD*/)) 
		|= 1UL<<0;



	PWR->CR3 = /*bit1=0 Disable regulator*/  1UL<<0 /*bypass regulator*/;

	// Since we are using external Vcore, Before RAM write access is allowed, the VOS (voltage scaling) setting must
	// match the actual core voltage.
	// We are supplying 1.20V, which corresponds to the highest performance VOS1 scale.

	PWR->D3CR = 0b11UL<<14; // VOS1

	extern void pwrswitch_chargepump_init();
	pwrswitch_chargepump_init();
	// this may be necessary when reseting from flasher. In normal boot, the user still presses
	// the power switch, so this isn't necessary in that case:
	extern void chargepump_replenish_pulsetrain_before_itcm_copy();
	chargepump_replenish_pulsetrain_before_itcm_copy(); 

	uint32_t* text_itcm_begin  = (uint32_t*)&_TEXT_ITCM_BEGIN;
	uint32_t* text_itcm_end    = (uint32_t*)&_TEXT_ITCM_END;
	uint32_t* text_itcm_i_begin = (uint32_t*)&_TEXT_ITCM_I_BEGIN;

	while(text_itcm_begin < text_itcm_end)
	{
		*text_itcm_begin = *text_itcm_i_begin;
		text_itcm_begin++;
		text_itcm_i_begin++;
	}

	chargepump_replenish_pulsetrain_before_itcm_copy(); 

	__DSB(); __ISB();

	main();
}


extern void dump_scb();
extern void dump_stack();
void nmi_handler(void)
{
	SAFETY_SHUTDOWN();
	dump_scb();
	dump_stack();

	error(2);
}

void hardfault_handler(void)
{
	SAFETY_SHUTDOWN();
	dump_scb();
	dump_stack();
	error(3);
}

void memmanage_handler(void)
{
	SAFETY_SHUTDOWN();
	dump_scb();
	dump_stack();
	error(4);
}

void invalid_handler(void)
{
	error(5);
}
