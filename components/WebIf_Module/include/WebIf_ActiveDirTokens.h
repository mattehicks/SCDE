#ifndef WebIf_ActiveDirTokens
#define WebIf_ActiveDirTokens

// enum for active directory content
enum ActiveDirContent			// CMDs in range 0xf0-0xff
  { ADC_DevName = 0xf0			// #0xf0 CMDs in range 0xfa-0xff
  , ADC_FeatNr				// #0xf1 extract ADID of feature from number
  , ADC_ForceAFN			// #0xf2 force an alternative filename

  , ADC_FeatureName			// #0xf1
/*
  #if PROJECT_HAS_LiGHT == TRUE
  , ADC_LiGHTFN				// #0x??
  #endif

  #if PROJECT_HAS_SwITCH == TRUE
  , ADC_SwITCHFN			// #0x??
  #endif

  #if PROJECT_HAS_ClIMA == TRUE
  , ADC_ClIMAFN				// #0x??
  #endif

  #if PROJECT_HAS_S0 == TRUE
  , ADC_S0FN				// #0x??
  #endif

  #if PROJECT_HAS_ADC == TRUE
  , ADC_ADCFN				// #0x??
  #endif

  #if PROJECT_HAS_FX == TRUE
  , ADC_FXFN				// #0x??
  #endif

  #if PROJECT_HAS_RFID == TRUE
  , ADC_RFIDFN				// #0x??
  #endif
*/
  , ADC_LAST				// #0x??
  };

/*
int HTTPD_ADirDevName( UrlProcHelper *MyUrlProcHelper);
int HTTPD_ADirFeatNr(UrlProcHelper *MyUrlProcHelper, int NumFeat);

int HTTPD_ADirLightFeatName( UrlProcHelper *MyUrlProcHelper, int NumFeat);
int HTTPD_ADirSwitchFeatName( UrlProcHelper *MyUrlProcHelper, int NumFeat);
int HTTPD_ADirClimaFeatName( UrlProcHelper *MyUrlProcHelper, int NumFeat);
int HTTPD_ADirS0FeatName( UrlProcHelper *MyUrlProcHelper, int NumFeat);
int HTTPD_ADirADCFeatName( UrlProcHelper *MyUrlProcHelper, int NumFeat);
int HTTPD_ADirFXFeatName( UrlProcHelper *MyUrlProcHelper, int NumFeat);
int HTTPD_ADirRFIDFeatName( UrlProcHelper *MyUrlProcHelper, int NumFeat);*/

int WebIf_ADirTokenFeatName(Common_Definition_t* Common_Definition, UrlProcHelper_t* MyUrlProcHelper);
bool WebIf_ExecActiveDirToken(WebIf_HTTPDConnSlotData_t* conn, Common_Definition_t* Common_Definition, UrlProcHelper_t* MyUrlProcHelper);

#endif //WebIf_ActiveDirTokens
