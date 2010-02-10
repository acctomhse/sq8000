+q  // Enter quiet mode
+mon
//________________________________________________________
// startice.cmd: Created by MAJIC Setup Wizard version 3.3 
// Creation Date: 7/12/2004 16:49:35
// Processor type: ARM926EJS -varm926ejs
// Project: My Project
// Description: 
//________________________________________________________
dv "Reading startice.cmd file\n"
//
// Software Settings
//
eo semi_hosting_enabled= on   // Semihosting support
eo semi_hosting_vector = 0x8   // Use default vector for Semihosting
eo vector_catch        = 0x3ff  // Set Vector Catch
//eo vector_catch        = 0x0  // Set Vector Catch
//
// Target Information Options
//
eo trgt_resets_jtag  = no   // Target reset does not reset JTAG controller
//
// MAJIC Settings
//
eo ice_jtag_clock_freq= 20   // JTAG clock frequency (MHz)
eo ice_jtag_use_rtclk = on
eo ice_jtag_use_trst  = on  // Controls whether MAJIC drives the TRST* signal
eo ice_reset_output   = off  // reset command does not pulse MAJICs reset output
//eo ice_power_sense    = TRST // Sense target voltage level on VREF pin
eo ice_power_sense    = VREF // Sense target voltage level on VREF pin
//
// Trace aliases (MAJIC-Plus or MAJIC-mx only)
//
ea etinst fr c etrace.cmd  // set up Trace options for instructions 
ea etdata fr c eview.cmd   // set up Trace options for Data 
//
ea dts  do ice_trig*; do trace* // display trace control settings
ea kts eo tt=auto; eo tta=stop; eo tg=none; // kill trace settings (set defaults)
//
// Aliases for mmu dumping and virtual->physical translation
//
ea MMU_DUMP; fr c xdmp.cmd
ea MMU_XLATE; fr c xlat.cmd
//
//For LII_A
//ew 0x19c04000=0x161
//ew 0x19c0400c=0x50a

//For 20MHZ
//ew 0x19c04000=0x1131
//ew 0x19c0400c=0x30a

//For LIIIv1 100M
//ew 0x19c04000=0x1161
//ew 0x19c0400c=0x40a
//ew 0x19c04030=0x11111111

//For LDK5
ew 0x180b4000=0x1161
ew 0x180b400c=0x30a
ew 0x1801c000=0xdeadbeef

//For LDK
//ew 0x1e8e000c=0xdeadbeef
//ew 0x1e8e000c=0x0
dv "Finished reading startice.cmd\n"
-mon
