/* Copyright (C) 2012 Ion Torrent Systems, Inc. All Rights Reserved */

//! @file     MultiFlowDist.h
//! @ingroup  VariantCaller
//! @brief    HP Indel detection


#ifndef STACKPLUS_H
#define STACKPLUS_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <ctype.h>
#include <algorithm>
#include "api/api_global.h"
#include "api/BamAux.h"
#include "api/BamConstants.h"
#include "api/BamReader.h"
#include "api/SamHeader.h"
#include "api/BamAlignment.h"
#include "api/SamReadGroup.h"
#include "api/SamReadGroupDictionary.h"
#include "api/SamSequence.h"
#include "api/SamSequenceDictionary.h"
#include "RandSchrange.h"

#include "sys/types.h"
#include "sys/stat.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>

#include <Variant.h>

#include "InputStructures.h"
#include "ExtendedReadInfo.h"
#include "ExtendParameters.h"


using namespace std;
using namespace BamTools;
using namespace ion;


// grab the "stack" of reads associated with a given sequence location in a bam file
class StackPlus{
public:
  RandSchrange             RandGen;       //!< Thread safe random number generator
  vector<ExtendedReadInfo> read_stack;    //!< Reads spanning the variant position
  string                   flow_order;    //!< variable duplication from global context
  bool                     no_coverage;   //!< Is true if there is zero coverage in this position

  int                      baseDepth;     //!< Read depth at variant as recorded in vcf variant object
  int                      read_counter;  //!< Gives the number of validly unpacked candidate reads that we have encountered
  int                      num_map_qv_filtered;    //!< Gives the number of reads that where filtered due to mapping qv.
  int                      num_snp_limit_filtered; //!< The number of reads that had more mismatches than allowed.
  int                      num_terminate_early; //!< number of reads terminated before completing the variant

  //! @brief  Creates a stack of reads that provide evidence in the case of our candidate variant
  //void StackUpOneVariant(BamMultiReader * bamReader, const string & local_contig_sequence,
  void StackUpOneVariant(PersistingThreadObjects &thread_objects,
                         int variant_start_pos, int variant_end_pos, vcf::Variant ** candidate_variant,
                         ExtendParameters * parameters, InputStructures &global_context);

  //! @brief   Checks whether we should stop reading in any more reads
  //! @param[in]  global_context    Globally useful data structures
  //! @param[in]  alignment         Alignment information of current read from BAM
  //! @param[in]  variant_start_pos Start (inclusive) of window of influence of this variant
  //! @param[out] false             if we should not read in any more reads
  bool CheckValidAlignmentPosition(const InputStructures &global_context, const BamTools::BamAlignment &alignment, string seqName, int variant_start_pos);

  //! @brief   Computes the absolute number of mismatches in alignment
  //! @param[in]  const BamTools alignment object
  //! @param[out] Absolute number of mismatches in alignment
  int  GetNumberOfMismatches(const BamTools::BamAlignment &alignment);

  //! @brief  Filters reads based on read span and alignment quality.
  //! @param[in]  global_context   Globally useful data structures
  //! @param[in]  alignment        Alignment information of current read from BAM
  //! @param[in]  variant_end_pos  End (inclusive) of window of influence of this variant
  //! @param[out] false            if read is filtered out
  bool AlignmentReadFilters(const InputStructures &global_context, const BamTools::BamAlignment &alignment, int variant_end_pos);


  //! @brief  Performing reservoir sampling on reads
  //! @param[in] current_read         ExtendedReadInfo of current read alignment
  //! @param[in] downsample_coverage  Desired maximum coverage
  void ReservoirSampleReads(ExtendedReadInfo &current_read, unsigned int max_coverage, int DEBUG);

  StackPlus(){
    no_coverage = false;
    baseDepth = 0;
    read_counter = 0;
    num_map_qv_filtered = 0;
    num_snp_limit_filtered = 0;
    num_terminate_early = 0;
  };
};


#endif //STACKPLUS_H
