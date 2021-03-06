/* Copyright (C) 2010 Ion Torrent Systems, Inc. All Rights Reserved */
#ifndef BKGFITMATRIXPACKER_H
#define BKGFITMATRIXPACKER_H


#include <string.h>
#include <stdlib.h>
#include <float.h>
#include "BkgMagicDefines.h"

class BkgFitMatDat;

/* #ifdef ION_USE_MKL */
/* #include <mkl_cblas.h> */
/* #else */
/* #include <cblas.h> */
/* #endif */

//#define BKG_FIT_MAX_FLOWS   20

// identifiers for all the partial derivatives that might be used in the fit
// this needs to be a bit-field
typedef enum 
  {
    TBL_END    =    0x0,
    // per-well fitting PartialDeriv components
    DFDP        =0x00000001,
    DFDR        =0x00000002,
    DFDPDM      =0x00000004, 
    DFDGAIN     =0x00000008,
    DFDA        =0x00000010,
    DFDDKR      =0x00000020,

    // per-region fitting PartialDeriv components

    DFDSIGMA    =0x00001000,
    DFDTSH      =0x00002000,
    DFDERR      =0x00004000,
    DFDT0       =0x00008000, 
    DFDTAUMR    =0x00010000,
    DFDTAUOR    =0x00020000,

    DFDRDR      =0x00040000,
    DFDKRATE    =0x00080000,
    DFDSENS     =0x00100000,
    DFDD        =0x00200000,
    DFDPDR      =0x00400000,
    DFDMR       =0x00800000,
    DFDKMAX     =0x01000000,
    DFDT0DLY    =0x02000000,
    DFDSMULT    =0x04000000,
    DFDTAUE     =0x08000000,
    // special cases (function value and fit error)
    YERR        =0x10000000,
    FVAL        =0x20000000,


  } PartialDerivComponent;

// identifiers for the two matricies that must be constructed
typedef enum {
  JTJ_MAT,
  RHS_MAT
} AssyMatID;

// item used in list of partial derivatives.  This list forms the linkage between
// the logical PartialDerivComponent value specified in the table (at link time), and the 
// run time address of the component's data
struct PartialDeriv_comp_list_item {
  PartialDerivComponent comp;
  float *addr;
};

// structure that holds all the information to describe how a single jtj or rhs
// matrix element can be constructed from pieceis of two different PartialDeriv components
struct mat_assy_input_line {
  PartialDerivComponent comp1;
  PartialDerivComponent comp2;
  int affected_flows[NUMFB];

  AssyMatID matId;
  int mat_row;
  int mat_col;
};


struct sub_instr {
  int   len;      // numer of consecutive data points to mult-add together
  float *src1;
  float *src2;
};

// the run-time information used to control the matrix building process.  Once the
// address of each PartialDeriv component is known, a list of mat_assembly_instructions is built
// an each item indicates the basic information needed to construct each non-zero 
// matrix element
struct mat_assembly_instruction {
  int cnt;        // number of sub_instr blocks to process
  struct sub_instr *si;
  double *dst;    // output address for dot-product value
};

// structure that holds the output mapping of the solution matrix (the delta vector)
// to the params structure
struct delta_mat_output_line {
  int delta_ndx;
  int param_ndx;
};

// collection of all input and output instruction information for a single fit configuration
struct fit_instructions {
  struct mat_assy_input_line *input;
  int    input_len;
  struct delta_mat_output_line *output;
  int    output_len;
};

typedef enum {
  LinearSolverException,
  LinearSolverSuccess

} LinearSolverResult;

class BkgFitMatrixPacker
{
 public:
  BkgFitMatrixPacker(int imgLen,fit_instructions &fi,PartialDeriv_comp_list_item *PartialDeriv_list,int PartialDeriv_list_len);

  LinearSolverResult GetOutput(float *dptr,double lambda, double regularizer);

  unsigned int GetPartialDerivMask(void) {return PartialDeriv_mask;}
	
  ~BkgFitMatrixPacker();
  BkgFitMatDat *data;
  /* Mat<double> *jtj; */
  /* Col<double> *rhs; */
  /* Col<double> *delta; */

  double *GetDestMatrixPtr(AssyMatID mat_id,int row,int col);


 private:


  float *GetPartialDerivComponent(PartialDerivComponent comp);

  void CreateInstrFromInputLine(mat_assembly_instruction *p,mat_assy_input_line *src,int imgLen);

  delta_mat_output_line *outputList;
  int nOutputs;

  mat_assembly_instruction *instList;
  int nInstr;

  PartialDeriv_comp_list_item *PartialDeriv_list;
  int nPartialDeriv;

  unsigned int PartialDeriv_mask;
  int numException;

 public:

  fit_instructions& my_fit_instructions;
  delta_mat_output_line* getOuputList() { return outputList; }
  mat_assembly_instruction* getInstList() { return instList; }
  void BuildMatrix(bool accum);
  int getNumInstr() { return nInstr; }
  int getNumOutputs() { return nOutputs; }
  int getNumException() { return numException; }
  void resetNumException() { numException = 0; }
  void SetDataRhs(float, int);
  void SetDataJtj(float, int, int);
};


#endif // BKGFITMATRIXPACKER_H
