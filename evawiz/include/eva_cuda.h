#ifndef CUDA_Evawiz_Eins_H
#define CUDA_Evawiz_Eins_H

namespace evawiz{
  
  inline int __CudaBlockNumber(int n){
    register int num=(n+__CudaThreadNumberPerBlock-1)/__CudaThreadNumberPerBlock;
    if (num>__CudaMaxBlockNumber) return __CudaMaxBlockNumber; 
    else return num;
  }

    //define = ----------------------------------------------------
  //set : *out = *in or *out = num

#if defined(__CUDACC__)
  template<class T1, class T2>
  __global__ void set_kernel(T1 *arr1, T2 *arr2,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arr1[tid] = T1( arr2[tid] ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2> void set_device( T1 *arr1, T2 *arr2, int n ){
#if defined(__CUDACC__)
    set_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arr1,arr2,n);
#endif
  }
  template<class T1, class T2> void set_host( T1 *arr1, T2 *arr2, int n ){
	  for (int i=0;i<n;i++) arr1[i]=T1(arr2[i]);
  }
  // *arr = num
#if defined(__CUDACC__)
  template<class T1, class T2>
  __global__ void set_kernel(T1 *arr, T2 num,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arr[tid] = T1( num ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2>
  void set_device( T1 *arr, T2 num, int n ){
#if defined(__CUDACC__)
    set_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arr,num,n);
#endif
  }
  template<class T1, class T2> void set_host( T1 *arr, T2 num, int n ){
    for (int i=0;i<n;i++) 
      arr[i]=T1(num);
  }
  //define + ---------------------------------------------------
  //number add *out = num + (*in) or (*in) + num
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void add_kernel(T1 *arrout, T2 num,T3 *arrin,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin[tid]+num );
      tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void add_device(T1 *arrout, T2 num,T3 *arrin,int n){
#if defined(__CUDACC__)
    add_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,num,arrin,n);
#endif
  }
  template<class T1, class T2, class T3> void add_device(T1 *arrout, T2 *arrin,T3 num,int n){
#if defined(__CUDACC__)
    add_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,num,arrin,n);
#endif
  }
  template<class T1, class T2, class T3> void add_host(T1 *arrout, T2 num,T3 *arrin,int n){
    for (int i=0;i<n;i++) arrout[i]= T1(num + arrin[i]);
  }
  template<class T1, class T2, class T3> void add_host(T1 *arrout, T2 *arrin,T3 num,int n){
    for (int i=0;i<n;i++) 
      arrout[i]= T1(num + arrin[i]);
  }
  //add: *out = *in1 + *in2
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void add_kernel(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin1[tid]+ arrin2[tid] );
      tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void add_device(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
#if defined(__CUDACC__)
    add_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,arrin1,arrin2,n);
#endif
  }
  template<class T1, class T2, class T3> void add_host(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    for (int tid =0;tid<n;tid++) 
      arrout[tid]= T1( arrin1[tid] + arrin2[tid]);
  }
  //define - ----------------------------------------------------
  //  *out = *in - num
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void minus_kernel(T1 *arrout, T2 *arrin,T3 num,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin[tid]-num );
      tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void minus_device(T1 *arrout, T2 *arrin,T3 num,int n){
#if defined(__CUDACC__)
    minus_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,arrin,num,n);
#endif
  }
  template<class T1, class T2, class T3> void minus_host(T1 *arrout, T2 *arrin,T3 num,int n){
    for (int i=0;i<n;i++) arrout[i] = T1( arrin[i] - num);
  }

  //minus  *out = num - *in
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void minus_kernel(T1 *arrout, T2 num,T3 *arrin,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( num-arrin[tid] );
      tid+=blockDim.x*gridDim.x;
    }
  } 
#endif
  template<class T1, class T2, class T3> void minus_device(T1 *arrout, T2 num,T3 *arrin,int n){
#if defined(__CUDACC__)
    minus_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,num,arrin,n);
#endif
  }

  template<class T1, class T2, class T3> void minus_host(T1 *arrout, T2 num,T3 *arrin,int n){
    for (int tid=0;tid<n;tid++)  
      arrout[tid] = T1( num-arrin[tid] );  
  }

  //minus  *out = *in1 - *in2
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void minus_kernel(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin1[tid]-arrin2[tid] ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void minus_device(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
#if defined(__CUDACC__)
    minus_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,arrin1,arrin2,n);
#endif
  }
  template<class T1, class T2, class T3> void minus_host(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    for (int tid=0;tid<n;tid++)  arrout[tid] = T1( arrin1[tid]-arrin2[tid] );
  }

  //define * ---------------------------------------------------
  //num multi *out = num * (*in) or (*in) * num
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void multi_kernel(T1 *arrout, T2 num,T3 *arrin,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin[tid]*num ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void multi_device(T1 *arrout, T2 num,T3 *arrin,int n){
#if defined(__CUDACC__)
    multi_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,num,arrin,n);
#endif
  }
  template<class T1, class T2, class T3> void multi_device(T1 *arrout, T2 *arrin,T3 num,int n){
#if defined(__CUDACC__)
    multi_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,num,arrin,n);
#endif
  }
  template<class T1, class T2, class T3> void multi_host(T1 *arrout, T2 num,T3 *arrin,int n){
    for (int tid=0; tid<n; tid++) arrout[tid] = T1( arrin[tid]*num ); 
  }
  template<class T1, class T2, class T3> void multi_host(T1 *arrout, T2 *arrin,T3 num,int n){
    for (int tid=0; tid<n; tid++) arrout[tid] = T1( arrin[tid] * num ); 
  }
  //multi: *out = *in1 * *in2
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void multi_kernel(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin1[tid] * arrin2[tid] ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void multi_device(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
#if defined(__CUDACC__)
    multi_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,arrin1,arrin2,n);
#endif
  }
  template<class T1, class T2, class T3> void multi_host(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    for (int tid=0; tid<n; tid++) arrout[tid] = T1( arrin1[tid]* arrin2[tid] );
  }
  //define / ----------------------------------------------------
  //numDiv  *out = *in / num
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void divi_kernel(T1 *arrout, T2 *arrin,T3 num,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin[tid]/num ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void divi_device(T1 *arrout, T2 *arrin,T3 num,int n){
#if defined(__CUDACC__)
    divi_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,arrin,num,n);
#endif
  }
  template<class T1, class T2, class T3> void divi_host(T1 *arrout, T2 *arrin,T3 num,int n){
    for (int tid=0; tid<n; tid++) arrout[tid] = T1( arrin[tid] / num );
  }

  //numDiv  *out = num / *in
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void divi_kernel(T1 *arrout, T2 num,T3 *arrin,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( num/arrin[tid] ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void divi_device(T1 *arrout, T2 num,T3 *arrin,int n){
#if defined(__CUDACC__)
    divi_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,num,arrin,n);
#endif
  }
  template<class T1, class T2, class T3> void divi_host(T1 *arrout, T2 num,T3 *arrin,int n){
    for (int tid=0; tid<n; tid++) arrout[tid] = T1( num / arrin[tid]  );
  }
  //div  *out = *in1 / *in2
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void divi_kernel(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( arrin1[tid]/arrin2[tid] ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2, class T3> void divi_device(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
#if defined(__CUDACC__)
    divi_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,arrin1,arrin2,n);
#endif
  }
  template<class T1, class T2, class T3> void divi_host(T1 *arrout, T2 *arrin1,T3 *arrin2,int n){
    for (int tid=0; tid<n; tid++) arrout[tid] = T1( arrin1[tid] / arrin2[tid]  );
  }
  //define ^ ----------------------------------------------------
  //numDiv  *out = *in ^ num
#if defined(__CUDACC__)
  template<class T1, class T2,class T3>
  __global__ void power_kernel(T1 *arrout, T2 *arrin,T3 num,int n){
    int tid= threadIdx.x+blockIdx.x*blockDim.x;
    while (tid<n) {
      arrout[tid] = T1( pow(arrin[tid],(double)num) ); tid+=blockDim.x*gridDim.x;
    }
  }
#endif
  template<class T1, class T2,class T3> void power_device(T1 *arrout, T2 *arrin,T3 num,int n){
#if defined(__CUDACC__)
    power_kernel <<<__CudaBlockNumber(n),__CudaThreadNumberPerBlock>>> (arrout,arrin,num,n);
#endif
  }
  template<class T1, class T2,class T3> void power_host(T1 *arrout, T2 *arrin,T3 num,int n){
    for (int tid=0; tid<n; tid++) 
      arrout[tid] = T1( pow(arrin[tid],(double)num) );
  }

};


#endif
