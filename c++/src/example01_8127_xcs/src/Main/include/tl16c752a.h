
#ifndef	TL16C752AH
#define	TL16C752AH


#define BAUDRATE1200	1200
#define BAUDRATE2400	2400
#define BAUDRATE4800	4800
#define BAUDRATE9600	9600
#define BAUDRATE14400	14400
#define BAUDRATE19200	19200 

#define WORDLEN5   	0x05
#define WORDLEN6   	0x06
#define WORDLEN7   	0x07
#define WORDLEN8   	0x08

#define STOPBIT1		0x00
#define STOPBIT2		0x01

#define NOPARITY		0x00
#define SETPARITY	0x01

#define ODDPARITY	0x01
#define EVENPARITY	0x00

#define NORMALMODE	0x00
#define MODEMMODE	0x01

/***********Ioctl cmd define*************/
#define SET_BAUDRATE				0
#define SET_WORD_LENGTH			1
#define SET_STOP_BIT_NUM			2
#define SET_PARITY_SET				3
#define SET_ODD_OR_EVEN			4
#define SET_UART_MODE				5
#define GET_READBUF_LENGTH			6



/***********GPIO cmd define*************/
#define SWICTH_MASK		 0x0000000f
#define SWICTH_WARNING			0x00000001
#define SWITCH_IN_NUM				4

#define MISC_RESET_VIDEO	_IO('M', 0x01)	/* video reset */
#define MISC_RESET_IP101	_IO('M', 0x02)	/* ip101 reset */
#define MISC_RESET_ATA	    _IO('M', 0x03)	/* ata reset */
#define MISC_RESET_FLASH 	_IO('M', 0x04)	/* flash reset */
#define MISC_RESET_2550	    _IO('M', 0x05)	/* 2550 reset */
#define MISC_RESET_FS8806	_IO('M', 0x06)	/* fs8806 reset */

#define MISC_BUZZER_ON		_IO('M', 0x11)	/* buzzer. enable on		*/
#define MISC_BUZZER_OFF		_IO('M', 0x12)	/* ... off			*/
#define MISC_RUN_LED_ON		_IO('M', 0x13)	/* run led. enable on	*/
#define MISC_RUN_LED_OFF	 _IO('M', 0x14)	/* ... off			*/

/* 报警输出设备ioctl命令宏定义 */
#define MISC_ALARM_OUT_SET	 _IOW('M', 0x15, unsigned long) /* Set Alarm out sta */
#define MISC_ALARM_STA_READ	 _IOR('M', 0x16, unsigned long)	 /* Read Alarm sta*/

#define MISC_HARDWARE_READ	 _IOW('M', 0x17, unsigned long) /* Set Alarm out sta */
#define MISC_RESET_ATMEGA8	 _IOW('M', 0x18, unsigned long) /* Set Alarm out sta */
#define MISC_RESET_DM642     _IO('M',0x19)/*dm642 reset*/
#define MISC_RESET_PCI       _IO('M',0x20)
#define MISC_CHIP_RESET      _IO('M',0x21)
#define MISC_SYSTEM_READ     _IOR('M',0x22,unsigned long)


typedef struct tagMiscStr
{
	unsigned char chan;
	unsigned char ctrl;
}MISC_STR;



#endif

