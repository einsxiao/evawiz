#ifndef EvaDefinition_Evawiz_H
#define EvaDefinition_Evawiz_H

namespace evawiz{


  ///////////////////////////////////////////
  const int   MaxRecursionDepth		          = 256;
  const int   NormalQuitCode		            = 999;
  const int   ErrorQuitCode			            = -999;
  const int   ErrorContinueCode	           	= -1;

  const int   COMPARE_LESS 			            = -1;
  const int   COMPARE_EQUAL 		            = 0;
  const int   COMPARE_BEGGER 		            = 1;

  const int 	MatrixStateNormal		          = 0;
  const int 	MatrixStateCopied		          = 1;
  const int 	MatrixStateTempFree           = 2;
  const int 	MatrixStateTempOccupied       = 3;
  
  const int 	MatrixNone		                = 0;
  const int 	MatrixHost		                = 1;
  const int 	MatrixDevice	                = 2;
  const int 	MatrixHostDevice              = 3;

  const int 	CalculatePositionHost		      = 1;
  const int 	CalculatePositionDevice	      = 2;

  const int   RunningModeNormal             = 0;
  const int   RunningModeCpu                = 1;
  const int   RunningModeGpu                = 0;

  const int   __CudaMaxBlockNumber          = 65535;

  class EvaSettings{
    friend class SystemModule;
  private:
    static int    calculatePosition;             //Host
    static int    matrixPosition;                //Host
    static int    runningMode;                   //Gpu 
  public:
    static int    cudaThreadsNumberPerBlock;     // multiple of 32
    static int    evawizCpuKernelThreadsNumber;  //4
    static int    precision;                     //15
    static bool   scientificFormat;              //false
    static double epsilon;                       //0.000000000000005
  public:
    static void   SetFormat(int precision,bool sci=false);
    inline static void   SetMatrixPosition(int pos)
    {
      matrixPosition = pos;
    };
    inline static int    MatrixPosition()
    {
#ifdef __CUDACC__
      if ( RunningMode() == RunningModeGpu )
        return matrixPosition;
      else
        return MatrixHost;
#else
      return MatrixHost;
#endif
    };
    inline static void   SetExecutePosition(int pos){
      SetMatrixPosition( pos );
    };
    inline static int    ExecutePosition(){
      return MatrixPosition();
    };
    inline static bool   IsOnHost()
    {
#ifdef __CUDACC__
      return MatrixPosition() == MatrixHost or MatrixPosition() == MatrixHostDevice;
#else
      return true;
#endif
    };
    inline static bool   IsOnDevice()
    {
#ifdef __CUDACC__
      return MatrixPosition() == MatrixDevice or MatrixPosition() == MatrixHostDevice;
#else
      return false;
#endif
    };
    inline static void   SetRunningMode(int mode)
    {
      runningMode = mode;
    };
    inline static int    RunningMode()
    {
#ifdef __CUDACC__
      return runningMode;
#else
      return RunningModeCpu;
#endif
    };
    static void   SetCudaThreadsNumberPerBlock(int num);
    static void   SetEvawizCpuKernelThreadsNumber(int num);
  };


}

#endif
