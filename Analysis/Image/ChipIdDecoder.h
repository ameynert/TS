/* Copyright (C) 2011 Ion Torrent Systems, Inc. All Rights Reserved */
#ifndef CHIPIDDECODER_H
#define CHIPIDDECODER_H

typedef enum
{
  ChipIdUnknown = 0,
  ChipId314 = 1,
  ChipId316 = 2,
  ChipId316v2 = 3,        
  ChipId318 = 4,
  ChipId900 = 5,
  ChipId910 = 6,
} ChipIdEnum;

typedef struct
{
  char *id_str;
  ChipIdEnum id;
} ChipIdDecodeArrayType;

class ChipIdDecoder
{

  public:
    static ChipIdEnum GetGlobalChipId (void) { return glob_chip_id; }
    static void SetGlobalChipId (const char* id_str_from_explog);
    static char *GetChipType(void);
    ~ChipIdDecoder();
    ChipIdDecoder() { chipType = NULL; }
    
  private:
    static ChipIdDecodeArrayType chip_id_str_lookup_array[];
    static ChipIdEnum glob_chip_id;
    static char *chipType;
};


#endif // CHIPIDDECODER_H
