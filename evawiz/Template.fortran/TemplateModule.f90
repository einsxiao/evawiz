
!following can be called within TemplateModule.cpp(cu)
module fortmod
contains
  subroutine fortfunc(input_var,output_var) 
    implicit none
    real*8 input_var
    real*8 output_var
    output_var = input_var*99
  end subroutine fortfunc
end module fortmod


