#ifndef _CH700X_H_
#define _CH700X_H_

#define	CH7007A_I2C_CLIENT_ADDR		(0x75)
#define CH7007A_VID					0x50
#define CH700X_REG_PREFIX			0x80
#define CH700X_REG_BURST			0x40

#define	CH700X_DMR 		0x00 //8 Display mode selection
#define	CH700X_FFR 		0x01 //6 Flicker filter mode selection
#define	CH700X_VBW 		0x03 //8 Luma and chroma filter bandwidth selection
#define	CH700X_IDF 		0x04 //6 Data format and bit-width selections
#define	CH700X_CM 		0x06 //8 Sets the clock mode to be used
#define	CH700X_SAV 		0x07 //8+ Active video delay setting
#define	CH700X_PO 		0x08 //3 MSB bits of position values
#define	CH700X_BLR 		0x09 //8 Black level adjustment input latch clock edge select
#define	CH700X_HPR 		0x0A //8+ Enables horizontal movement of displayed image on TV
#define	CH700X_VPR 		0x0B //8+ Enables vertical movement of displayed image on TV
#define	CH700X_SPR 		0x0D //4 Determines the horizontal and vertical sync polarity
#define	CH700X_PMR 		0x0E //5 Enables power saving modes
#define	CH700X_CDR 		0x10 //4 Detection of TV presence
#define	CH700X_CE 		0x11 //3 Contrast enhancement setting
#define	CH700X_MNE 		0x13 //5 Contains the MSB bits for the M and N PLL values
#define	CH700X_PLLM 	0x14 //8+ Sets the PLL M value - bits (7:0)
#define	CH700X_PLLN 	0x15 //8+ Sets the PLL N value - bits (7:0)
#define	CH700X_BCO 		0x17 //6 Determines the clock output at pin 41
#define	CH700X_FSCI 	0x18 //-1FH 4 or 8 each Determines the subcarrier frequency
#define CH700X_FSCIN(n)	(CH700X_FSCI+n)
#define	CH700X_PLLC 	0x20 //6 Controls for the PLL and memory sections
#define	CH700X_CIVC 	0x21 //5 Control of CIV value
#define	CH700X_CIV 		0x21 //- 24H 8 each Readable register containing the calculated
#define CH700X_CIVN(n)	(CH700X_CIV+n)
#define	CH700X_VID 		0x25 //8 Device version number
#define	CH700X_TR 		0x26 //- 29H 30 Reserved for test (details not included herein)
#define CH700X_TRN(n)	(CH700X_TR+n)
#define	CH700X_AR 		0x3F //6 Current register being addressed


//Display mode InputDataFormat(ActiveVideo)_TotalPixels/LinexTotalLines/Frame_OutputFormat    
#define	CH700X_DMR_512x384_840x500_PAL		0x00
#define	CH700X_DMR_512x384_840x625_PAL      0x01
#define	CH700X_DMR_512x384_800x420_NTSC     0x08
#define	CH700X_DMR_512x384_784x525_NTSC     0x09
#define	CH700X_DMR_720X400_1125X500_PAL     0x20
#define	CH700X_DMR_720x400_1116x625_PAL     0x21
#define	CH700X_DMR_720x400_945x420_NTSC     0x28
#define	CH700X_DMR_720x400_936x525_NTSC     0x29
#define	CH700X_DMR_640x400_1000x500_PAL     0x40
#define	CH700X_DMR_640x400_1008x625_PAL     0x41
#define	CH700X_DMR_640x400_840x420_NTSC     0x48
#define	CH700X_DMR_640x400_840x525_NTSC     0x49
#define	CH700X_DMR_640x400_840x600_NTSC     0x4a
#define	CH700X_DMR_640x480_840x500_PAL      0x60
#define	CH700X_DMR_640x480_840x625_PAL      0x61
#define	CH700X_DMR_640x480_840x750_PAL      0x63
#define	CH700X_DMR_640x480_784x525_NTSC     0x69
#define	CH700X_DMR_640x480_784x600_NTSC     0x6a
#define	CH700X_DMR_640x480_800x630_NTSC     0x6b
#define	CH700X_DMR_800x600_944x625_PAL      0x81
#define	CH700X_DMR_800x600_960x750_PAL      0x83
#define	CH700X_DMR_800x600_936x836_PAL      0x84
#define	CH700X_DMR_800x600_1040x630_NTSC    0x8b
#define	CH700X_DMR_800x600_1040x700_NTSC    0x8c
#define	CH700X_DMR_800x600_1064x750_NTSC    0x8d
#define	CH700X_DMR_720x576_864x625_PAL      0xa1
#define	CH700X_DMR_720x480_858x525_NTSC     0xa9

//Flicker filter mode selection
#define	CH700X_FFR_LUMA_MIN					(0<<0)
#define	CH700X_FFR_LUMA_SLIGHT				(1<<0)
#define	CH700X_FFR_LUMA_MAX					(2<<0)
#define	CH700X_FFR_TEXT_MIN					(0<<2)
#define	CH700X_FFR_TEXT_SLIGHT				(1<<2)
#define	CH700X_FFR_TEXT_MAX					(2<<2)
#define	CH700X_FFR_CHROMA_MIN				(0<<4)
#define	CH700X_FFR_CHROMA_SLIGHT			(1<<4)
#define	CH700X_FFR_CHROMA_MAX				(2<<4)

//Luma and chroma filter bandwidth selection
#define	CH700X_VBW_YCV_LOW					(0<<0)
#define	CH700X_VBW_YCV_HIGH					(1<<0)
#define	CH700X_VBW_YSV_LOW					(0<<1)
#define	CH700X_VBW_YSV_MED					(1<<1)
#define	CH700X_VBW_YSV_HIGH					(2<<1)
#define	CH700X_VBW_YPEAK_DISABLE			(0<<3)
#define	CH700X_VBW_YPEAK_ENABLE				(1<<3)
#define	CH700X_VBW_CBW_LOW					(0<<4)
#define	CH700X_VBW_CBW_MED					(1<<4)
#define	CH700X_VBW_CBW_MED_HIGH				(2<<4)
#define	CH700X_VBW_CBW_HIGH					(3<<4) 
#define	CH700X_VBW_CVBW_DISABLE				(0<<6)
#define	CH700X_VBW_CVBW_ENABLE				(1<<6)
#define	CH700X_VBW_FLFF_DISABLE				(0<<7)
#define	CH700X_VBW_FLFF_ENABLE				(1<<7)

//Data format and bit-width selections
#define	CH700X_IDF_C_12_RGB					0x04	//12-bit multiplexed RGB (24-bit color) input (¡§C¡¨ multiplex scheme)
#define	CH700X_IDF_I_12_RGB					0x05	//12-bit multiplexed RGB (24-bit color) input (¡§I¡¨ multiplex scheme)
#define	CH700X_IDF_565_RGB					0x07	//8-bit multiplexed RGB (16-bit color, 565) input
#define	CH700X_IDF_555_RGB					0x08	//8-bit multiplexed RGB (15-bit color, 555) input
#define	CH700X_IDF_CCIR656_YCBCR			0x09	//8-bit multiplexed YCrCb (24-bit color) input (Y, Cr and Cb are multiplexed)
#define	CH700X_IDF_DACG_NTSC_PALm			0x00
#define	CH700X_IDF_DACG_NTSCj_PAL			0x40

//Sets the clock mode to be used
#define	CH700X_CM_PCM_1						0x00		// Pixel clock 
#define	CH700X_CM_PCM_2						0x01		// Pixel clock 
#define	CH700X_CM_PCM_3						0x02		// Pixel clock 
#define	CH700X_CM_XCM_1						(0x00<<2)	// Extern clock 
#define	CH700X_CM_XCM_2						(0x01<<2)	// Extern clock 
#define	CH700X_CM_XCM_3						(0x02<<2)	// Extern clock 
#define	CH700X_CM_MCP_POS					(1<<4)		//edge of the pixel clock output will be used to latch input data.
#define	CH700X_CM_MCP_NEG					(0<<4)		//edge of the pixel clock output will be used to latch input data.
#define	CH700X_CM_MS_MASTER					(1<<6)		//master or slave clock mode.
#define	CH700X_CM_MS_SLAVE					(0<<6)		//master or slave clock mode.						
#define	CH700X_CM_CFRB_LOOK					(1<<7)		//chroma subcarrier free-runs, or is locked to the video signal.
#define	CH700X_CM_CFRB_FREE					(0<<7)		//chroma subcarrier free-runs, or is locked to the video signal.

//Active video delay setting
#define CH700X_SAV_VALUE(sav)			((char)sav)

//MSB bits of position values		
#define	CH700X_PO_VALUE(sav,hp,vp)		( ((char)(sav>>8)<<2) | ((char)(hp>>8)<<1) |((char)(vp>>8)<<0) )

//Enables horizontal movement of displayed image on TV
#define CH700X_HPR_VALUE(hp)			((char)hp)

//Enables vertical movement of displayed image on TV
#define CH700X_VPR_VALUE(vp)			((char)vp)

//horizontal and vertical sync polarity
#define	CH700X_SPR_HSP_LOW					(0<<0)
#define	CH700X_SPR_HSP_HIGH					(1<<0)
#define	CH700X_SPR_VSP_LOW					(0<<1)
#define	CH700X_SPR_VSP_HIGH					(1<<1)
#define	CH700X_SPR_SYO_SLAVE				(0<<2)
#define	CH700X_SPR_SYO_MASTER				(1<<2)
#define	CH700X_SPR_DES_PIN					(0<<3)
#define	CH700X_SPR_DES_PIXEL				(1<<3)

//Enables power saving modes
#define	CH700X_PMR_PD_CO					0x00		//000 Composite Off CVBS DAC is powered down.                                               
#define	CH700X_PMR_PD_PIN					0x01		//001 Power Down Most pins and circuitry are disabled (except for the buffered clock outputs which are limited to the 14MHz output and VCO divided outputs).                           
#define	CH700X_PMR_PD_SV					0x02		//010 S-Video Off S-Video DACs are powered down.                                            
#define	CH700X_PMR_PD_ON					0x03		//011 Normal (On) All circuits and pins are active.                                         
#define	CH700X_PMR_PD_FULL					0x04		//1XX Full Power Down All circuitry is powered down except serial port interface circuit.   
#define	CH700X_PMR_RESET					(0<<3)
#define	CH700X_PMR_NORMAL					(1<<3)
#define CH700X_PMR_SCART					(1<<4)

//Detection of TV presence
#define	CH700X_CDR_SENSE					(1<<0)
#define	CH700X_CDR_NO_CVBST					(1<<1)    
#define	CH700X_CDR_NO_CT					(1<<2)
#define	CH700X_CDR_NO_YT					(1<<3)

//Contrast enhancement setting
#define	CH700X_CE_CONTRAST_LG3				(0<<0)
#define	CH700X_CE_CONTRAST_LG2				(1<<0)
#define	CH700X_CE_CONTRAST_LG1				(2<<0)
#define	CH700X_CE_CONTRAST_NORMAL			(3<<0)
#define	CH700X_CE_CONTRAST_HG1				(4<<0)
#define	CH700X_CE_CONTRAST_HG2				(5<<0)
#define	CH700X_CE_CONTRAST_HG3				(6<<0)
#define	CH700X_CE_CONTRAST_HG4				(7<<0)

//Sets the PLL M value                         
#define	CH700X_PLLM_512x384_840x500_PAL		13 
#define	CH700X_PLLM_512x384_840x625_PAL     4  
#define	CH700X_PLLM_512x384_800x420_NTSC    89 
#define	CH700X_PLLM_512x384_784x525_NTSC    63 
#define	CH700X_PLLM_720X400_1125X500_PAL    26 
#define	CH700X_PLLM_720x400_1116x625_PAL    138
#define	CH700X_PLLM_720x400_945x420_NTSC    63 
#define	CH700X_PLLM_720x400_936x525_NTSC    33 
#define	CH700X_PLLM_640x400_1000x500_PAL    61 
#define	CH700X_PLLM_640x400_1008x625_PAL    3  
#define	CH700X_PLLM_640x400_840x420_NTSC    63 
#define	CH700X_PLLM_640x400_840x525_NTSC    11 
#define	CH700X_PLLM_640x400_840x600_NTSC    89 
#define	CH700X_PLLM_640x480_840x500_PAL     13 
#define	CH700X_PLLM_640x480_840x625_PAL     4  
#define	CH700X_PLLM_640x480_840x750_PAL     3  
#define	CH700X_PLLM_640x480_784x525_NTSC    63 
#define	CH700X_PLLM_640x480_784x600_NTSC    63 
#define	CH700X_PLLM_640x480_800x630_NTSC    89 
#define	CH700X_PLLM_800x600_944x625_PAL     313
#define	CH700X_PLLM_800x600_960x750_PAL     33 
#define	CH700X_PLLM_800x600_936x836_PAL     103
#define	CH700X_PLLM_800x600_1040x630_NTSC   33 
#define	CH700X_PLLM_800x600_1040x700_NTSC   19 
#define	CH700X_PLLM_800x600_1064x750_NTSC   89 
#define	CH700X_PLLM_720x576_864x625_PAL     33 
#define	CH700X_PLLM_720x480_858x525_NTSC    33 

//Sets the PLL N value
#define	CH700X_PLLN_512x384_840x500_PAL		20   
#define	CH700X_PLLN_512x384_840x625_PAL     9    
#define	CH700X_PLLN_512x384_800x420_NTSC    126  
#define	CH700X_PLLN_512x384_784x525_NTSC    110  
#define	CH700X_PLLN_720X400_1125X500_PAL    53   
#define	CH700X_PLLN_720x400_1116x625_PAL    339  
#define	CH700X_PLLN_720x400_945x420_NTSC    106  
#define	CH700X_PLLN_720x400_936x525_NTSC    70   
#define	CH700X_PLLN_640x400_1000x500_PAL    108  
#define	CH700X_PLLN_640x400_1008x625_PAL    9    
#define	CH700X_PLLN_640x400_840x420_NTSC    94   
#define	CH700X_PLLN_640x400_840x525_NTSC    22   
#define	CH700X_PLLN_640x400_840x600_NTSC    190  
#define	CH700X_PLLN_640x480_840x500_PAL     20   
#define	CH700X_PLLN_640x480_840x625_PAL     9    
#define	CH700X_PLLN_640x480_840x750_PAL     9    
#define	CH700X_PLLN_640x480_784x525_NTSC    110  
#define	CH700X_PLLN_640x480_784x600_NTSC    126  
#define	CH700X_PLLN_640x480_800x630_NTSC    190  
#define	CH700X_PLLN_800x600_944x625_PAL     647  
#define	CH700X_PLLN_800x600_960x750_PAL     86   
#define	CH700X_PLLN_800x600_936x836_PAL     284  
#define	CH700X_PLLN_800x600_1040x630_NTSC   94   
#define	CH700X_PLLN_800x600_1040x700_NTSC   62   
#define	CH700X_PLLN_800x600_1064x750_NTSC   302  
#define	CH700X_PLLN_720x576_864x625_PAL     31   
#define	CH700X_PLLN_720x480_858x525_NTSC    31   

#define PLLM_7_0(m)	((char)(m))
#define PLLN_7_0(n)	((char)(n))	
#define PLLMNE(m,n)	( (m>>8) | ( ((char)(n>>8)) <<1 ) )


//Controls for the PLL and memory sections
#define	CH700X_PLLC_MEM5V					(1<<0)
#define	CH700X_PLLC_MEM33V					(0<<0)
#define	CH700X_PLLC_PLL5VA					(1<<1)
#define	CH700X_PLLC_PLL33VA                 (0<<1)
#define	CH700X_PLLC_PLL5VD					(1<<2)
#define	CH700X_PLLC_PLL33VD                 (0<<2)
#define	CH700X_PLLC_PLLS5					(1<<3)
#define	CH700X_PLLC_PLLS33                  (0<<3)

#define	CH700X_PLLC_512x384_840x500_PAL		(1<<4) 
#define	CH700X_PLLC_512x384_840x625_PAL     (1<<4) 
#define	CH700X_PLLC_512x384_800x420_NTSC    (1<<4) 
#define	CH700X_PLLC_512x384_784x525_NTSC    (0<<4) 
#define	CH700X_PLLC_720X400_1125X500_PAL    (1<<4) 
#define	CH700X_PLLC_720x400_1116x625_PAL    (0<<4) 
#define	CH700X_PLLC_720x400_945x420_NTSC    (1<<4) 
#define	CH700X_PLLC_720x400_936x525_NTSC    (1<<4) 
#define	CH700X_PLLC_640x400_1000x500_PAL    (0<<4) 
#define	CH700X_PLLC_640x400_1008x625_PAL    (1<<4) 
#define	CH700X_PLLC_640x400_840x420_NTSC    (1<<4) 
#define	CH700X_PLLC_640x400_840x525_NTSC    (1<<4) 
#define	CH700X_PLLC_640x400_840x600_NTSC    (0<<4) 
#define	CH700X_PLLC_640x480_840x500_PAL     (1<<4) 
#define	CH700X_PLLC_640x480_840x625_PAL     (1<<4) 
#define	CH700X_PLLC_640x480_840x750_PAL     (1<<4) 
#define	CH700X_PLLC_640x480_784x525_NTSC    (0<<4) 
#define	CH700X_PLLC_640x480_784x600_NTSC    (0<<4) 
#define	CH700X_PLLC_640x480_800x630_NTSC    (0<<4) 
#define	CH700X_PLLC_800x600_944x625_PAL     (0<<4) 
#define	CH700X_PLLC_800x600_960x750_PAL     (1<<4) 
#define	CH700X_PLLC_800x600_936x836_PAL     (0<<4) 
#define	CH700X_PLLC_800x600_1040x630_NTSC   (1<<4) 
#define	CH700X_PLLC_800x600_1040x700_NTSC   (1<<4) 
#define	CH700X_PLLC_800x600_1064x750_NTSC   (0<<4) 
#define	CH700X_PLLC_720x576_864x625_PAL     (1<<4) 
#define	CH700X_PLLC_720x480_858x525_NTSC    (1<<4) 


//Determines the subcarrier frequency
#define	CH700X_DMR_512x384_840x500_PAL_FCSI			806021060
#define	CH700X_DMR_512x384_840x500_PALn_FCSI		651209077
#define	CH700X_DMR_512x384_840x625_PAL_FCSI			644816848
#define	CH700X_DMR_512x384_840x625_PALn_FCSI		520967262
#define	CH700X_DMR_512x384_800x420_NTSC_FCSI 		763363328
#define	CH700X_DMR_512x384_800x420_NTSCnoDC_FCSI	763366524
#define	CH700X_DMR_512x384_800x420_PALm_FCSI 		762524467
#define	CH700X_DMR_512x384_784x525_NTSC_FCSI 	 	623153737
#define	CH700X_DMR_512x384_784x525_NTSCnoDC_FCSI	623156346
#define	CH700X_DMR_512x384_784x525_PALm_FCSI 	 	622468953
#define	CH700X_DMR_720X400_1125X500_PAL_FCSI 		601829058
#define	CH700X_DMR_720X400_1125X500_PALn_FCSI 		486236111
#define	CH700X_DMR_720x400_1116x625_PAL_FCSI 		485346014
#define	CH700X_DMR_720x400_1116x625_PALn_FCSI 		392125896
#define	CH700X_DMR_720x400_945x420_NTSC_FCSI 	 	574429782
#define	CH700X_DMR_720x400_945x420_NTSCnoDC_FCSI	574432187
#define	CH700X_DMR_720x400_945x420_PALm_FCSI 	 	573798541
#define	CH700X_DMR_720x400_936x525_NTSC_FCSI 	 	463962517
#define	CH700X_DMR_720x400_936x525_NTSCnoDC_FCSI	463964459
#define	CH700X_DMR_720x400_936x525_PALm_FCSI 	 	463452668
#define	CH700X_DMR_640x400_1000x500_PAL_FCSI 		677057690
#define	CH700X_DMR_640x400_1000x500_PALn_FCSI 		547015625
#define	CH700X_DMR_640x400_1008x625_PAL_FCSI 		537347373
#define	CH700X_DMR_640x400_1008x625_PALn_FCSI 		434139385
#define	CH700X_DMR_640x400_840x420_NTSC_FCSI 	 	646233505
#define	CH700X_DMR_640x400_840x420_NTSCnoDC_FCSI	646236211
#define	CH700X_DMR_640x400_840x420_PALm_FCSI 	 	645523358
#define	CH700X_DMR_640x400_840x525_NTSC_FCSI 	 	516986804
#define	CH700X_DMR_640x400_840x525_NTSCnoDC_FCSI	516988968
#define	CH700X_DMR_640x400_840x525_PALm_FCSI 	 	516418687
#define	CH700X_DMR_640x400_840x600_NTSC_FCSI 	 	452363454
#define	CH700X_DMR_640x400_840x600_NTSCnoDC_FCSI	452365347
#define	CH700X_DMR_640x400_840x600_PALm_FCSI 	 	451866351
#define	CH700X_DMR_640x480_840x500_PAL_FCSI  		806021060
#define	CH700X_DMR_640x480_840x500_PALn_FCSI  		651209077
#define	CH700X_DMR_640x480_840x625_PAL_FCSI  		644816848
#define	CH700X_DMR_640x480_840x625_PALn_FCSI  		520967262
#define	CH700X_DMR_640x480_840x750_PAL_FCSI  		537347373
#define	CH700X_DMR_640x480_840x750_PALn_FCSI  		434139385
#define	CH700X_DMR_640x480_784x525_NTSC_FCSI 	 	623153737
#define	CH700X_DMR_640x480_784x525_NTSCnoDC_FCSI	623156346
#define	CH700X_DMR_640x480_784x525_PALm_FCSI 	 	622468953
#define	CH700X_DMR_640x480_784x600_NTSC_FCSI 	 	545259520
#define	CH700X_DMR_640x480_784x600_NTSCnoDC_FCSI	545261803
#define	CH700X_DMR_640x480_784x600_PALm_FCSI 	 	544660334
#define	CH700X_DMR_640x480_800x630_NTSC_FCSI 	 	508908885
#define	CH700X_DMR_640x480_800x630_NTSCnoDC_FCSI	508911016
#define	CH700X_DMR_640x480_800x630_PALm_FCSI 	 	508349645
#define	CH700X_DMR_800x600_944x625_PAL_FCSI  		645499916
#define	CH700X_DMR_800x600_944x625_PALn_FCSI  		521519134
#define	CH700X_DMR_800x600_960x750_PAL_FCSI  		528951320
#define	CH700X_DMR_800x600_960x750_PALn_FCSI  		427355957
#define	CH700X_DMR_800x600_936x836_PAL_FCSI  		488262757
#define	CH700X_DMR_800x600_936x836_PALn_FCSI  		394482422
#define	CH700X_DMR_800x600_1040x630_NTSC_FCSI 	 	521957831
#define	CH700X_DMR_800x600_1040x630_NTSCnoDC_FCSI	521960016
#define	CH700X_DMR_800x600_1040x630_PALm_FCSI 	 	521384251
#define	CH700X_DMR_800x600_1040x700_NTSC_FCSI 	 	469762048
#define	CH700X_DMR_800x600_1040x700_NTSCnoDC_FCSI	469764015
#define	CH700X_DMR_800x600_1040x700_PALm_FCSI 	 	469245826
#define	CH700X_DMR_800x600_1064x750_NTSC_FCSI 	 	428554851
#define	CH700X_DMR_800x600_1064x750_NTSCnoDC_FCSI	428556645
#define	CH700X_DMR_800x600_1064x750_PALm_FCSI 	 	428083911
#define	CH700X_DMR_720x576_864x625_PAL_FCSI  		705268427
#define	CH700X_DMR_720x576_864x625_PALn_FCSI  		569807942
#define	CH700X_DMR_720x480_858x525_NTSC_FCSI 	 	569408543
#define	CH700X_DMR_720x480_858x525_NTSCnoDC_FCSI	569410927
#define	CH700X_DMR_720x480_858x525_PALm_FCSI 	 	568782819

#define CH700X_FSCIN_VALUE(n,v)						( (v>>((7-n)*4)) & 0x0F )


// Some Bit define at FSCI register
#define CH700X_FSCI3_GPIOIN1_LOW					(0<<7)
#define CH700X_FSCI3_GPIOIN1_HIGH					(1<<7) 
#define CH700X_FSCI3_GPIOIN0_LOW					(0<<6)
#define CH700X_FSCI3_GPIOIN0_HIGH					(1<<6)
#define CH700X_FSCI3_DVDD2_18V						(0<<5)
#define CH700X_FSCI3_DVDD2_33V                      (1<<5)
#define CH700X_FSCI3_POUTP_LOW						(0<<4)
#define CH700X_FSCI3_POUTP_HIGH                     (1<<4)

#define CH700X_FSCI4_GOENB1_OE						(0<<7)
#define CH700X_FSCI4_GOENB1_OD                      (1<<7)
#define CH700X_FSCI4_GOENB0_OE                      (0<<6)
#define CH700X_FSCI4_GOENB0_OD                      (1<<6)
#define CH700X_FSCI4_DSM_DIS                        (0<<5)
#define CH700X_FSCI4_DSM_EN                         (1<<5)
#define CH700X_FSCI4_DSEN_DIS                       (0<<4)
#define CH700X_FSCI4_DSEN_EN                        (1<<4)

//Control of CIV value
#define	CH700X_CIVC_ACIV_NON						(0<<0)
#define	CH700X_CIVC_ACIV_AUTO						(1<<0)


#define FORMAT_NTSC 0
#define FORMAT_PAL 1
extern int ch700x_init(int format);

#endif	//_CH700X_H_

















































