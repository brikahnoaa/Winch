void main()
{
Acquire();
}
/*************************************************************************************
**	WhatTime
**
**	Returns 20-char long current timein RTC time in GMT format. 
**
**	|RTC__--------------|
**	104 245:13:34:45:100
**  123456789012345678901
**           1         2         3         4
**	NOAA, Nerport, OR
**	Haru Matsumoto 1-st version 10/26/04	hm
*************************************************************************************/
ulong WhatTime(char *GMT_char)
	{
	RTCtm	*rtc_time;				//CFX's real time clock

	SecondsSince1970=RTCGetTime(&SecondsSince1970, &ticks);
	rtc_time = RTClocaltime (&SecondsSince1970);
	sprintf(GMT_char,"%.3d %.3d:%.2d:%.2d:%.2d:%.3u",
	rtc_time->tm_year, rtc_time->tm_yday + 1,\
	rtc_time->tm_hour,rtc_time->tm_min,rtc_time->tm_sec, TICKS_TO_MS(ticks));
	return SecondsSince1970;
	}	//WhatTime

//*********************************************************************************************
// Acquire
// Set up A/D, record the start time, write data in CompactFlash, and record the end time 
// Time is in seconds since Jan 1, 1970.
//*********************************************************************************************

short Acquire(char *fname)
	{
	bool	uni = true;		// true for unipolar, false for bipolar
	bool	sgl = true;		// true for single-ended, false for differential
	char 	longbuf[4],strbuf[4];
	long	i, sample;
		
	//Open a file.  Create one, if not there. Append, if already exists. 
	flogf("OPENING FILE FOR A/D LOGGING %s\n", fname); 
	if((fp = fopen(fname,"a+")) == NULL)
		{
	 	flogf("Can not open file\n");
	 	return false;
		}

	// Initialize QPB to accept our A-D with its QSPI connection.
	ad = CFxADInit(&adbuf, ADSLOT, ADInitFunction);
	if (! CFxADLock(ad))	// lock it down, we want to own the QSPI
		{
		flogf("\nCouldn't lock and own A-D with QSPI\n");
		return -1;
		}
	
	CFxADSample(ad, 0, uni, sgl, false);
	RTCDelayMicroSeconds(100000L);//wait to settle

	ADRawHead = ADRawTail = 0;	// for first dummy reading to set things up
	CFxADSampleBlock(ad, FCHAN, NCHAN, &ADSamplingRuptHandler, uni, sgl, true);
	RTCDelayMicroSeconds(100000L);//wait to settle

	PITSet100usPeriod(PITOff);	// disable timer
	PITRemoveChore(0);			// get rid of all current chores
	IEVInsertCFunct(&ADTimingRuptHandler, pitVector);	// replacement fast routine

	CTMRun(false);				// turn off CTM6 module

	ADRawHead = ADRawTail = 0;	// reset start indices

	SCITxWaitCompletion();		// let any pending UART data finish
	EIAForceOff(true);			// turn off the RS232 driver

	i=0L;
			
	sample=(QUE.CFPPBSZ-256L)/2L;
	IEVInsertCFunct(&ExtFinishPulseRuptHandler, level5InterruptAutovector);
	PinBus(IRQ5);					// enable PBM (IRQ5) interrupt button

	WhatTime(GMT_char); //get current time in seconds
	flogf("%s\n", GMT_char);
	BIRS.RTCLOGSTR = SecondsSince1970; //Global
	BIRS.RTCSTRTIC = TICKS_TO_MS(ticks);//convert to msec
	sprintf(longbuf, "%lu", BIRS.RTCLOGSTR);
	VEEStoreStr(BIR_RTCLOGSTR_NAME, longbuf);	//Keep wake-up time in VEE	
	sprintf(strbuf, "%u", BIRS.RTCSTRTIC);
	VEEStoreStr(BIR_RTCSTRTIC_NAME, strbuf);

	PITSet100usPeriod(10000/QUE.SRATEHZ);	// start the engine ...
	while (!ProgramFinished && i <sample)
		{
		TickleSWSR();	// another reprieve
		
		while (i<sample && (ADRawHead - ADRawTail) & AD_RAW_DATA_BUF_MASK)
			{
			if (! fwrite(&ADDataBuf[ADRawTail], sizeof(ushort), NCHAN, fp))
				break;	// could really do better error reporting...
			ADRawTail = (ADRawTail + 1) & AD_RAW_DATA_BUF_MASK;
			i++;
			}

		if (QPBTestRunning())	// conversions in process?
			{
			*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode
			LPStopCSE(CPUStop);			// VCO on (immed. wake) CPU on, SIMCLK is VCO clock
			}
		else
			{
			QSMStop();			// shutdown the QSM
			*(ushort *)0xffffe00c=0xF000; //force CF card into Card-1 active mode
			LPStopCSE(FastStop);		// VCO on (immed. wake) CPU off, SIMCLK is VCO clock
			}
		}
	PITSet100usPeriod(PITOff);	// stop sampling
	EIAForceOff(false);			// turn the RS232 driver back on

	WhatTime(GMT_char);// get the time in seconds again at the end
	flogf("%s\n", GMT_char);
	BIRS.RTCLOGEND = SecondsSince1970;
	BIRS.RTCENDTIC = TICKS_TO_MS(ticks);
	sprintf(longbuf, "%lu", BIRS.RTCLOGEND);
	VEEStoreStr(BIR_RTCLOGEND_NAME, longbuf);	//Keep wake-up time in VEE	
	sprintf(strbuf, "%u", BIRS.RTCENDTIC);
	VEEStoreStr(BIR_RTCENDTIC_NAME, strbuf);

	flogf("Sampled %ld\n",i);cdrain();coflush();

	fclose(fp);
	return 0;
	}